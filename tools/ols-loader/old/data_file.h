/*
 * HEX/BIN Data file parser and writer
 *
 * 2010 - Michal Demin
 *
 */
#ifndef DATA_FILE_H_
#define DATA_FILE_H_

#include <stdint.h>

uint8_t Data_Checksum(uint8_t *buf, uint16_t size);
uint32_t HEX_ReadFile(const char *file, uint8_t *out_buf, uint32_t out_buf_size);
int HEX_WriteFile(const char *file, uint8_t *in_buf, uint32_t in_buf_size);
uint32_t BIN_ReadFile(const char *file, uint8_t *out_buf, uint32_t out_buf_size);
int BIN_WriteFile(const char *file, uint8_t *in_buf, uint32_t in_buf_size);

#endif

