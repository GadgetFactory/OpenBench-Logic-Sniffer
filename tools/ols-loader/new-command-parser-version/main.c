/*
 * OLS flash loader
 *
 * 2010 - Michal Demin
 *
 * Loosely based on Pirate-Loader:
 * http://the-bus-pirate.googlecode.com/svn/trunk/bootloader-v4/pirate-loader/source/pirate-loader.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "ols.h"
#include "serial.h"
#include "data_file.h"

#undef NO_PUMP
//#define NO_PUMP

uint8_t *bin_buf;
uint32_t bin_buf_size;

int verbose = 0;
int cmd_read = 0;
int cmd_erase = 0;
int cmd_write = 0;
int cmd_status = 0;
int cmd_boot = 0;
int cmd_run = 0;
int cmd_ignore_jedec = 0;
int cmd_selftest = 0;

int param_limit = -1;
unsigned long param_speed = 921600;
const char *param_port = NULL;
const char *param_read_hex = NULL;
const char *param_read_bin = NULL;
const char *param_write_hex = NULL;
const char *param_write_bin = NULL;

void helpme(char** argv){

//print usage
		//printf("%s\n", argv[0]);
		printf(" parameters: \n");
		printf("   --port PORT  - port of Logic Sniffer, needs to be specified. Same as  -p Port \n");
		printf("   --speed SPEED  - sets speed of the serial port. Same as -s Speed\n");
		printf("   --wH FILE - HEX file to be uploaded to OLS\n");
		printf("   --wB FILE - BIN file to be uploaded to OLS\n");
		printf("   --rH FILE - HEX file to be downloaded from OLS\n");
		printf("   --rB FILE - BIN file to be downloaded from OLS\n");
//		printf("  -l:0     - send only bytes from file (0-padded) - write only\n");
		printf("   --l X  or -l X   - send only first X paged \n");
//		printf("  -verbode - be noisy\n");
		printf(" commands:\n");
		printf("   --erase  - erases Flash, also same as using -e \n");
		printf("   --write  - writes data to Flash, as  -w \n");
		printf("   --read   - reads data from Flash, same as -r \n");
		printf("   --ignore_jedec  - ignore jedec id, same as -i \n");
		printf("   --run    - enter run mode after finished, same as -r \n");
		printf("   --status - get OLS status, same as -s \n");
		printf("   --boot   - enter bootloader mode - ignore other commands, same as -b\n");
        printf("   --selftest - run self-test - ignore other commands\n");

		printf("\n\n");
		printf("   When no command specified, program will check FW version, and Flash ID\n\n");
		printf("   Usage examples:\n");
		#ifdef _WIN32
		printf("   To read Flash to HEX file 'OLS.hex' from OLS on COM2:\n");
		printf("   ols-loader --port COM2 --rH OLS.hex --read\n");
		printf("   or\n");
		printf("   ols-loader -p COM2 --rH OLS.hex -r\n");
		printf("   To erase and write flash, data in BIN file 'OLS.bin', OLS on COM2:\n");
		printf("   ols-loader --port COM2 --wB OLS.hex --write --erase\n");
		printf("   To get status, and jump to run mode, OLS on COM2:\n");
		printf("   ols-loader --port COM2 --run --status\n");

		#else
		printf("   To read Flash to HEX file 'OLS.hex' from OLS on \\dev\\ttyS1: \n");
		printf("   ols-loader --port \\dev\ttyS1 --rH OLS.hex --read\n");
		printf("   or\n");
	    printf("   ols-loader -p \\dev\ttyS1 --rH OLS.hex -r\n");
		printf("   To erase and write flash, data in BIN file 'OLS.bin', OLS on \\dev\\ttyS1:\n");
		printf("   ols-loader --p \\dev\\ttyS1 --wB OLS.hex --write --erase\n");
		printf("   To get status, and jump to run mode, OLS on \\dev\\ttyS1:\n");
		printf("   ols-loader --p \\dev\\ttyS1 --run --status\n");

		#endif
        printf("   Take note of the use of long options (--) and short options (-) in specifying parameters.\n");
        printf("   options and parameter are case-sensitive\n");
        exit(-1);

}

int parseCommandLine(int argc, char** argv)
{

    int c,index;

    while (1) {

        int option_index = 0;

        static struct option long_options[] = {
            {"port", 1, 0, 'p'},
            {"speed", 1, 0, 't'},
            {"wH", 1, 0, 0},
            {"wB", 1, 0, 0},
            {"rH", 1, 0, 0},
            {"rB", 1, 0, 0},
            {"l", 1, 0, 'l'},
            {"verbose", 0, 0, 'v'},
            {"erase", 0, 0, 'e'},
            {"write", 0, 0, 'w'},
            {"read", 0, 0, 'r'},
            {"ignore_jedec", 0, 0, 'i'},
            {"run", 0, 0, 'r'},
            {"status", 0, 0, 's'},
            {"boot", 0, 0, 'b'},
            {"selftest", 0, 0, 0},
            {"help", 0, 0, 'h'},
            {0, 0, 0, 0}
        };


        c = getopt_long (argc, argv, "p:t:l:veisbwrh",long_options, &option_index);

        if (c == -1)
            break;

        switch (c) {
        case 0:
            index=option_index;
            switch (index) {
                   case 0:  // port
                        param_port=optarg;
                        break;
                   case 1:  // speed
                        param_speed = atol(optarg);
                        break;
                   case 2:  // wH
                        if (param_write_bin != NULL) {
                            printf("can write only one file\n");
                            exit(-1);
                        }
                          param_write_hex=optarg;
                        break;
                   case 3:  // wB
                        if (param_write_hex != NULL) {
                            printf("can write only one file\n");
                            exit(-1);
                        }
                        param_write_bin=optarg;
                        break;
                   case 4:  // rH
                        param_read_hex=optarg;
                        break;
                   case 5:  // rB
                        param_read_bin=optarg;
                        break;
                   case 6:  // l
                        param_limit = atoi(optarg);
                        break;
                   case 7:  // verbose
                        verbose = 1;
                        break;
                   case 8:  // erase
                        cmd_erase = 1;
                        break;
                   case 9:  // write
                        cmd_write = 1;
                        break;
                   case 10: // read
                        cmd_read = 1;
                        break;
                   case 11: // ignore_jedec
                        cmd_ignore_jedec = 1;
                        break;
                   case 12: // run
                        cmd_run = 1;
                   case 13: // status
                        cmd_status = 1;
                        break;
                   case 14: // boot
                        cmd_boot = 1;
                        break;
                   case 15: // selftest
               			cmd_selftest = 1;
                        break;
                   case 16: // help
               			helpme(argv);
                        break;

            }

            break;
       case 'v':
            verbose = 1;
            break;
       case 'e':
            cmd_erase = 1;
            break;
       case 'i':
            cmd_ignore_jedec = 1;
            break;

       case 's':
            cmd_status = 1;
             break;

       case 'b':
            cmd_boot = 1;
            break;
       case 'w':
              cmd_write = 1;
            break;
       case 'r':
          cmd_read = 1;
            break;
       case 'p':
            param_port=optarg;
            break;
       case 't':
            param_speed = atol(optarg);
            break;
        case 'h':
            helpme(argv);
            break;
       case '?':
            helpme(argv);
            break;
       default:
            printf ("Unknown option: %c\n", c);
            helpme(argv);
        }
    }


	if( argc == 1 )	{
		helpme(argv);
		return 0;
	}

	return 1;
}


int main(int argc, char** argv) {
	int		dev_fd = -1, res = -1;
	uint32_t i;
    printf("----------------------------------------------------------------\n");
    printf("        Logic Sniffer ROM loader v0.3 (September 30, 2010)\n");
    printf("----------------------------------------------------------------\n");
	if( (res = parseCommandLine(argc, argv)) < 0 ) {
		return -1;
	} else if( res == 0 ) {
		return 0;
	}

	if( !param_port ) {
		fprintf(stderr, "Please specify serial port \n");
		return -1;
	}

	printf("Opening serial port '%s' @ %ld ... ", param_port, param_speed);
#ifndef NO_PUMP
	dev_fd = openPort(param_port, 0);
	if (dev_fd < 0) {
	    printf("Error opening :(\n");
			return -1;
	}

	if( configurePort(dev_fd, param_speed) < 0 ) {
	    printf("Error configuring :(\n");
			return -1;
	}
#endif
	printf("  OK\n");

#ifndef NO_PUMP
	if ((res = PUMP_GetID(dev_fd))!=0) {
		return -1;
	}
#endif

#ifndef NO_PUMP
	if((res = PUMP_GetFlashID(dev_fd))!=0) {
		return -1;
	}
#else
	PUMP_FlashID = 0;
#endif

	if (cmd_boot) {
#ifndef NO_PUMP
		PUMP_EnterBootloader(dev_fd);
#endif
		return 0;
	}

	// now the PUMP_FlashID contains offset into PUMP_Flash array
	bin_buf_size = PUMP_Flash[PUMP_FlashID].pages * PUMP_Flash[PUMP_FlashID].page_size * sizeof(uint8_t);
	bin_buf = (uint8_t*)malloc(bin_buf_size);

	if (bin_buf == NULL) {
		printf("Error allocating %ld bytes of memory\n", (long)bin_buf_size);
		return -1;
	}
	memset(bin_buf, 0, bin_buf_size);

	if (cmd_status) {
#ifndef NO_PUMP
		PUMP_GetStatus(dev_fd);
#endif
	}

	if (cmd_selftest){
        PUMP_selftest(dev_fd);
        return -1;
	}

	if (cmd_erase) {
#ifndef NO_PUMP
		PUMP_FlashErase(dev_fd);
#endif
	}

	if (cmd_write) {
		uint32_t pages = 0;
		uint32_t read_size;

		if(param_write_hex) {
			printf("Reading HEX file '%s' ... ", param_write_hex);
			read_size = HEX_ReadFile(param_write_hex, bin_buf, bin_buf_size);
		} else if(param_write_bin) {
			printf("Reading BIN file '%s' ... ", param_write_bin);
			read_size = BIN_ReadFile(param_write_bin, bin_buf, bin_buf_size);
		} else {
			printf("no input file specified ! \n");
			return -1;
		}

		if (read_size == 0) {
			printf("Error!\n");
			return -1;
		}
		printf("OK! (binary size = %ld)\n", (long int)read_size);

		if (param_limit <= 0) {
			// read_size / page_size .. round up when necessary
			pages = (read_size+PUMP_Flash[PUMP_FlashID].page_size-1)/PUMP_Flash[PUMP_FlashID].page_size;
		} else {
			pages = param_limit;
		}
		if (pages > PUMP_Flash[PUMP_FlashID].pages)
		   pages = PUMP_Flash[PUMP_FlashID].pages;

		printf("Will write %ld pages \n", (long int)pages);

		for (i = 0; i < pages; i ++) {
#ifndef NO_PUMP
			PUMP_FlashWrite(dev_fd, i, bin_buf + (PUMP_Flash[PUMP_FlashID].page_size * i));
#else
			printf("Writing from mem=0x%p to page=%ld\n", bin_buf + (PUMP_Flash[PUMP_FlashID].page_size * i), (long int)i);
#endif
		}
	}

	if (cmd_read) {
		uint32_t pages;
		uint32_t temp_size;

		if (param_read_hex == NULL && param_read_bin == NULL) {
			printf("no output file specified ! \n");
			return -1;
		}
		if (param_limit <= 0) {
			pages = PUMP_Flash[PUMP_FlashID].pages;
		} else {
			pages = param_limit;
		}

		temp_size = pages * PUMP_Flash[PUMP_FlashID].page_size;

		printf("Will read %ld pages \n", (long int)pages);

		for (i = 0; i < pages; i ++) {
#ifndef NO_PUMP
			PUMP_FlashRead(dev_fd, i, bin_buf + (PUMP_Flash[PUMP_FlashID].page_size * i));
#else
			printf("Reading from page=%ld to mem=0x%p\n", (long int)i, bin_buf + (PUMP_Flash[PUMP_FlashID].page_size * i));
#endif
		}

		if (param_read_hex) {
			printf("Writing HEX file '%s' ... ", param_read_hex);
			HEX_WriteFile(param_read_hex, bin_buf, temp_size);
			printf("done!\n");
		}
		if (param_read_bin) {
			printf("Writing BIN file '%s' ... ", param_read_bin);
			BIN_WriteFile(param_read_bin, bin_buf, temp_size);
			printf("done!\n");
		}
	}


	if (cmd_run) {
#ifndef NO_PUMP
		PUMP_EnterRunMode(dev_fd);
#endif
	}

	free(bin_buf);
	return 0;
}
