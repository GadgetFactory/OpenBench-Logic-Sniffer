/*
 * Michal Demin - 2010
 */
#ifndef PUMP_H_
#define PUMP_H_

#include <stdint.h>

struct pump_flash_t {
	const char *jedec_id;
	uint16_t page_size;
	uint16_t pages;
	const char *name;
};

extern int PUMP_FlashID;
extern const struct pump_flash_t PUMP_Flash[];

int PUMP_GetStatus(int pump_fd);
int PUMP_GetID(int pump_fd);
int PUMP_EnterBootloader(int pump_fd);
int PUMP_EnterRunMode(int pump_fd);
int PUMP_GetFlashID(int pump_fd);
int PUMP_FlashErase(int pump_fd);
int PUMP_FlashRead(int pump_fd, uint16_t page, uint8_t *buf);
int PUMP_FlashWrite(int pump_fd, uint16_t page, uint8_t *buf);

#endif

