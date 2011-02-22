/*
 * PUMP flash loader
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

#include "pump.h"
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

int param_limit = -1;
unsigned long param_speed = 921600;
const char *param_port = NULL;
const char *param_read_hex = NULL;
const char *param_read_bin = NULL;
const char *param_write_hex = NULL;
const char *param_write_bin = NULL;

int parseCommandLine(int argc, char** argv)
{
	int i = 0;

	for(i=1; i<argc; i++)
	{
		if( !strncmp(argv[i], "-p", 2) ) {
			param_port = argv[i] + 3;
		} else if ( !strncmp(argv[i], "-l", 2) ) {
			param_limit = atoi(argv[i] + 3);
		} else if ( !strncmp(argv[i], "-t", 2) ) {
			param_speed = atol(argv[i] + 3);
		} else if ( !strncmp(argv[i], "-rH", 3) ) {
			param_read_hex = argv[i] + 4;
		} else if ( !strncmp(argv[i], "-rB", 3) ) {
			param_read_bin = argv[i] + 4;
		} else if ( !strncmp(argv[i], "-wH", 3) ) {
			if (param_write_bin != NULL) {
				printf("can write only one file\n");
				exit(-1);
			}
			param_write_hex = argv[i] + 4;
		} else if ( !strncmp(argv[i], "-wB", 3) ) {
			if (param_write_hex != NULL) {
				printf("can write only one file\n");
				exit(-1);
			}
			param_write_bin = argv[i] + 4;
		} else if ( !strcmp(argv[i], "-verbose") ) {
			verbose = 1;
		} else if ( !strcmp(argv[i], "-read") ) {
			cmd_read = 1;
		} else if ( !strcmp(argv[i], "-erase") ) {
			cmd_erase = 1;
		} else if ( !strcmp(argv[i], "-status") ) {
			cmd_status = 1;
		} else if ( !strcmp(argv[i], "-write") ) {
			cmd_write = 1;
		} else if ( !strcmp(argv[i], "-run") ) {
			cmd_run = 1;
		} else if ( !strcmp(argv[i], "-boot") ) {
			cmd_boot = 1;
		} else if ( !strcmp(argv[i], "--help") ) {
			argc = 1; //that's not pretty, but it works :)
			break;
		} else {
			fprintf(stderr, "Unknown parameter '%s'\n", argv[i]);
			return -1;
		}
	}

	if( argc == 1 )	{
		//print usage
		//printf("%s\n", argv[0]);
		printf("parameters:\n");
		printf("-p:PORT  - port of Logic Sniffer, needs to be specified\n\n");
		printf("-t:SPEED - sets speed of the serial port\n");
		printf("-wH:FILE - HEX file to be uploaded to PUMP\n");
		printf("-wB:FILE - BIN file to be uploaded to PUMP\n");
		printf("-rH:FILE - HEX file to be downloaded from PUMP\n");
		printf("-rB:FILE - BIN file to be downloaded from PUMP\n");
//		printf("-l:0     - send only bytes from file (0-padded) - write only\n");
		printf("-l:X     - send only first X paged \n");
//		printf("-verbode - be noisy\n");
		printf("commands:\n");
		printf("-erase  - erases Flash\n");
		printf("-write  - writes data to Flash\n");
		printf("-read   - reads data from Flash\n");
		printf("-run    - enter run mode after finished\n");
		printf("-status - get OLS stauts\n");
		printf("-boot   - enter bootloader mode - ignore other commands\n");

		printf("\n\n");
		printf("When no command specified, program will check FW version, and Flash ID\n\n");
		printf("Usage examples:\n");
		printf("To read Flash to HEX file 'ols.hex' from OLS on COM2:\n");
		printf("ols-loader -p:COM2 -rH:ols.hex -read\n");
		printf("To erase and write flash, data in BIN file 'ols.bin', OLS on COM2:\n");
		printf("ols-loader -p:COM2 -wB:old.hex -write -erase\n");
		printf("To get status, and jump to run mode, OLS on COM2:\n");
		printf("ols-loader -p:COM2 -run -status\n");

		return 0;
	}

	return 1;
}

int main(int argc, char** argv) {
	int		dev_fd = -1, res = -1;
	uint32_t i;

	printf("Logic Sniffer ROM loader v0.2 (September 10, 2010)\n");

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
	printf("OK\n");

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
