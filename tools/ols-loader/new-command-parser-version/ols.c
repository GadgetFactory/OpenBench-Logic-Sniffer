/*
 * Michal Demin - 2010
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data_file.h"
#include "serial.h"
#include "ols.h"
extern int cmd_ignore_jedec;
int PUMP_FlashID = -1;
const struct pump_flash_t PUMP_Flash[] = {
	{
		"\x1f\x24\x00\x00",
		264, // size of page
		2048, // number of pages
		"ATMEL AT45DB041D"
	},
	{
		"\x1f\x23\x00\x00",
		264, // size of page
		1024, // number of pages
		"ATMEL AT45DB021D"
	},
};

#define PUMP_FLASH_NUM (sizeof(PUMP_Flash)/sizeof(struct pump_flash_t))

/*
 * Does OLS self-test
 * pump_fd - fd of pump com port
 */
int PUMP_selftest(int pump_fd) {
	uint8_t cmd[4] = {0x07, 0x00, 0x00, 0x00};
	uint8_t status;
	int res;

	res = DPwrite(pump_fd, cmd, 4);
	if (res != 4) {
		printf("Error writing to OLS\n");
		return -2;
	}

	res = readWithTimeout(pump_fd, &status, 1, 100);

	if (res != 1) {
		printf("Error reading OLS status\n");
		return -1;
	}

    if(status==0x00){
        printf("Passed self-test :) \n");
    }else{
        if(status &  0b1) printf("ERROR: 1V2 supply failed self-test :( \n");
        if(status &  0b10) printf("ERROR: 2V5 supply failed self-test :( \n");
        if(status &  0b100) printf("ERROR: PROG_B pull-up failed self-test :( \n");
        if(status &  0b1000) printf("ERROR: DONE pull-up failed self-test :( \n");
        if(status &  0b10000) printf("ERROR: unknown ROM JEDEC ID (this could be ok...) :( \n");
        if(status &  0b100000) printf("ERROR: UPDATE button pull-up failed self-test :( \n");
    }

	return 0;
}


/*
 * Reads OLS status
 * pump_fd - fd of pump com port
 */
int PUMP_GetStatus(int pump_fd) {
	uint8_t cmd[4] = {0x05, 0x00, 0x00, 0x00};
	uint8_t status;
	int res;

	res = DPwrite(pump_fd, cmd, 4);
	if (res != 4) {
		printf("Error writing to OLS\n");
		return -2;
	}

	res = readWithTimeout(pump_fd, &status, 1, 1);

	if (res != 1) {
		printf("Error reading OLS status\n");
		return -1;
	}

	printf("OLS status: %02x\n", status);
	return 0;
}

/*
 * Reads OLS version
 * pump_fd - fd of pump com port
 */
int PUMP_GetID(int pump_fd) {
	uint8_t cmd[4] = {0x00, 0x00, 0x00, 0x00};
	uint8_t ret[7];
	int res;

	res = DPwrite(pump_fd, cmd, 4);
	if (res != 4) {
		printf("Error writing to OLS\n");
		return -2;
	}

	res = readWithTimeout(pump_fd, ret, 7, 1);

	if (res != 7) {
		printf("Error reading OLS id\n");
		return -1;
	}

	printf("Found OLS HW: %d, FW: %d.%d, Boot: %d\n", ret[1], ret[3], ret[4], ret[6]);
	return 0;
}

/*
 * commands OLS to enter bootloader mode
 * pump_fd - fd of pump com port
 */
int PUMP_EnterBootloader(int pump_fd) {
	uint8_t cmd[4] = {0x24, 0x24, 0x24, 0x24};
	int res;

	res = DPwrite(pump_fd, cmd, 4);
	if (res != 4) {
		printf("Error writing to OLS\n");
		return -2;
	}

	printf("OLS switched to bootloader mode\n");
	return 0;
}

/*
 * Switch the OLS to run mode
 * pump_fd - fd of pump com port
 */
int PUMP_EnterRunMode(int pump_fd) {
	uint8_t cmd[4] = {0xFF, 0xFF, 0xFF, 0xFF};
	int res;

	res = DPwrite(pump_fd, cmd, 4);
	if (res != 4) {
		printf("Error writing to OLS\n");
		return -2;
	}

	printf("OLS switched to RUN mode\n");
	return 0;
}
/*
 * ask the OLS for JEDEC id
 * pump_fd - fd of pump com port
 */
int PUMP_GetFlashID(int pump_fd) {
	uint8_t cmd[4] = {0x01, 0x00, 0x00, 0x00};
	uint8_t ret[4];
	int res;
	int i;

	res = DPwrite(pump_fd, cmd, 4);
	if (res != 4) {
		printf("Error writing to OLS\n");
		return -2;
	}

	res = readWithTimeout(pump_fd, ret, 4, 1);
    // ignore jedec if flag cmd_ignore_jedec = 1
    if (cmd_ignore_jedec ==0) {
        if (res != 4) {
            printf("Error reading JEDEC ID\n");
            return -1;
        }
	}

	for (i=0; i< PUMP_FLASH_NUM; i++) {
		if (memcmp(ret, PUMP_Flash[i].jedec_id, 4) == 0) {
			PUMP_FlashID = i;
			printf("Found flash: %s \n", PUMP_Flash[i].name);
			break;
		}

	}
    if (cmd_ignore_jedec ==1) {
          PUMP_FlashID = 0;     // pass next line
	}


	if(PUMP_FlashID == -1) {
		printf("Error - unknown flash type (%02x %02x %02x %02x)\n", ret[0], ret[1], ret[2], ret[3]);
		return -1;
	}

	return 0;
}

/*
 * erases OLS flash
 * pump_fd - fd of pump com port
 */
int PUMP_FlashErase(int pump_fd) {
	uint8_t cmd[4] = {0x04, 0x00, 0x00, 0x00};
	uint8_t status;
	int res;
	int retry = 0;
//	int i;

	if (PUMP_FlashID == -1) {
		printf("Cannot erase unknown flash\n");
		return -3;
	}

	res = DPwrite(pump_fd, cmd, 4);
	if (res != 4) {
		printf("Error writing to OLS\n");
		return -2;
	}

	printf("Chip erase ... ");

	while (1) {
		res = readWithTimeout(pump_fd, &status, 1, 1);

		if (res == 0) {
			retry ++;
		}

		if (res == 1) {
			if (status == 0x01) {
				printf("done :)\n");
				return 0;
			}
			printf("failed :( - bad reply\n");
			return -1;
		}

		// 20 second timenout
		if (retry > 20) {
			printf("failed :( - timeout\n");
			return -1;
		}


	}

	return 0;
}

/*
 * Reads data from flash
 * pump_fd - fd of pump com port
 * page - which page should be read
 * buf - buffer where the data will be stored
 */
int PUMP_FlashRead(int pump_fd, uint16_t page, uint8_t *buf) {
	uint8_t cmd[4] = {0x03, 0x00, 0x00, 0x00};
//	uint8_t status;
	int res;
//	int retry = 0;
//	int i;

	if (PUMP_FlashID == -1) {
		printf("Cannot READ  unknown flash\n");
		return -3;
	}

	if (page > PUMP_Flash[PUMP_FlashID].pages) {
		printf("You are trying to read page %d, but we have only %d !\n", page, PUMP_Flash[PUMP_FlashID].pages);
		return -2;
	}

	cmd[1] = (page >> 7) & 0xff;
	cmd[2] = (page << 1) & 0xff;

	printf("Page 0x%04x read ... ", page);

	res = DPwrite(pump_fd, cmd, 4);
	if (res != 4) {
		printf("Error writing CMD to OLS\n");
		return -2;
	}

	res = readWithTimeout(pump_fd, buf, PUMP_Flash[PUMP_FlashID].page_size, 1);

	if (res == PUMP_Flash[PUMP_FlashID].page_size) {
		printf("OK\n");
		return 0;
	}

	printf("Failed :(\n");
	return -1;
}

/*
 * writes data to flash
 * pump_fd - fd of pump com port
 * page - where the data should be written to
 * buf - data to be written
 */
int PUMP_FlashWrite(int pump_fd, uint16_t page, uint8_t *buf) {
	uint8_t cmd[4] = {0x02, 0x00, 0x00, 0x00};
	uint8_t status;
	uint8_t chksum;
	int res;
//	int retry = 0;
//	int i;

	if (PUMP_FlashID == -1) {
		printf("Cannot READ  unknown flash\n");
		return -3;
	}

	if (page > PUMP_Flash[PUMP_FlashID].pages) {
		printf("You are trying to read page %d, but we have only %d !\n", page, PUMP_Flash[PUMP_FlashID].pages);
		return -2;
	}

	cmd[1] = (page >> 7) & 0xff;
	cmd[2] = (page << 1) & 0xff;

	chksum = Data_Checksum(buf, PUMP_Flash[PUMP_FlashID].page_size);

	printf("Page 0x%04x write ... ", page);

	res = DPwrite(pump_fd, cmd, 4);
	res += DPwrite(pump_fd, buf, PUMP_Flash[PUMP_FlashID].page_size);
	res += DPwrite(pump_fd, &chksum, 1);
	if (res != 4+1+PUMP_Flash[PUMP_FlashID].page_size) {
		printf("Error writing CMD to OLS\n");
		return -2;
	}

	res = readWithTimeout(pump_fd, &status, 1, 1);

	if (res != 1) {
		printf("timeout\n");
		return -1;
	}

	if (status == 0x01) {
		printf("OK\n");
		return 0;
	}

	printf("Checksum error :(\n");
	return -1;
}

