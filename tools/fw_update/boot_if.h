/***************************************************************************
 *   Copyright (C) 2007 by Diolan                                          *
 *   www.diolan.com                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef BOOT_IF_H_INCLUDED
#define BOOT_IF_H_INCLUDED

/*
*-----------------------------------------------------------------------------
* Constants
*-----------------------------------------------------------------------------
*/
#define BOOT_CMD_SIZE	64 //!!!18f24j50 change
#if( defined HID_OUT_EP_SIZE && (HID_OUT_EP_SIZE<BOOT_CMD_SIZE) )
	#error "HID_OUT and CMD size mismatch: HID_OUT_EP_SIZE<BOOT_CMD_SIZE"
#endif
#define BOOT_RSP_SIZE	64 //!!!18f24j50 change
#if( defined HID_IN_EP_SIZE && (HID_IN_EP_SIZE<BOOT_CMD_SIZE) )
	#error "HID_IN and CMD size mismatch: HID_IN_EP_SIZE<BOOT_RSP_SIZE"
#endif

/*
 *-----------------------------------------------------------------------------
 * Types
 *-----------------------------------------------------------------------------
 */

#define BOOT_READ_FLASH 	0x01
#define BOOT_WRITE_FLASH	0x02
#define BOOT_ERASE_FLASH	0x03
#define BOOT_GET_FW_VER		0x04
#define BOOT_RESET     		0x05
#define	BOOT_READ_ID		0x06
#define	BOOT_WRITE_ID		0x07
#define	BOOT_READ_EEPROM	0x08
#define	BOOT_WRITE_EEPROM	0x09
#define	BOOT_CMD_UNKNOWN	0xFF

#define BOOT_ID_LEN		8

/*
 * common for all commands fields
 */
typedef struct
{
	unsigned char cmd;		/* command code, see BOOT_xxx constants */
	unsigned char echo;		/* echo is used to link between command and response */
} boot_cmd_header;

/*
 * READ_FLASH - read flash memory
 */
typedef struct
{
	unsigned char cmd;		/* command code, see BOOT_xxx constants */
	unsigned char echo;		/* echo is used to link between command and response */
	unsigned char addr_lo;		/* address must be divisible by 2 */
	unsigned char addr_hi;
	unsigned char reserved[1];
	unsigned char size8;		/* size must be divisible by 8 */
} boot_cmd_read_flash;

/*
* WRITE_FLASH - write flash memory
*/
typedef struct
{
	unsigned char cmd;		/* command code, see BOOT_xxx constants */
	unsigned char echo;		/* echo is used to link between command and response */
	unsigned char addr_lo;		/* address must be divisible by 2 */
	unsigned char addr_hi;
	unsigned char flush;	/*!!! 18F24j50 writes in 64 byte blocks, we need to split that over two HID packets. We use the reserved byte to indicate when to save*/ 
	unsigned char size8;		/* size must be divisible by 8*/
	unsigned char data[BOOT_CMD_SIZE - 6];
} boot_cmd_write_flash;

/*
 * ERASE_FLASH - erase flash memory
 */
typedef struct
{
	unsigned char cmd;		/* command code, see BOOT_xxx constants */
	unsigned char echo;		/* echo is used to link between command and response */
	unsigned char addr_lo;		/* address must be divisible by 64 */
	unsigned char addr_hi;
	unsigned char reserved[1];
	unsigned char size_x64;		/* size in 64 byte blocks */
} boot_cmd_erase_flash;


/*
* GET_FW_VERSION - get bootloader firmware version
*/
typedef struct
{
	unsigned char cmd;		/* command code, see BOOT_xxx constants */
	unsigned char echo;		/* echo is used to link between command and response */
} boot_cmd_get_fw_ver;


/*
* RESET - reset microcontroller
*/
typedef struct
{
	unsigned char cmd;		/* command code, see BOOT_xxx constants */
} boot_cmd_reset;

/*
* READ_ID - read ID location
*/
typedef struct
{
	unsigned char cmd;		/* command code, see BOOT_xxx constants */
	unsigned char echo;		/* echo is used to link between command and response */
	unsigned char reserved[3];
	unsigned char size;
} boot_cmd_read_id;

/*
* WRITE_ID - write ID location
*/
typedef struct
{
	unsigned char cmd;		/* command code, see BOOT_xxx constants */
	unsigned char echo;		/* echo is used to link between command and response */
	unsigned char reserved[3];
	unsigned char size;
	unsigned char data[BOOT_ID_LEN];
} boot_cmd_write_id;

/*
 * READ_EEPROM - read EEPROM memory
 */
typedef struct
{
	unsigned char cmd;		/* command code, see BOOT_xxx constants */
	unsigned char echo;		/* echo is used to link between command and response */
	unsigned char addr;		/* EEPROM address to start read from LSB */
	unsigned char reserved[2];
	unsigned char size;		/* size of EEPROM data to read */
} boot_cmd_read_eeprom;

/*
 * WRITE_EEPROM - write EEPROM memory
 */
typedef struct
{
	unsigned char cmd;		/* command code, see BOOT_xxx constants */
	unsigned char echo;		/* echo is used to link between command and response */
	unsigned char addr;		/* EEPROM address to start write to */
	unsigned char reserved[2];
	unsigned char size;		/* size of EEPROM data to write */
	unsigned char data[BOOT_CMD_SIZE - 6];
} boot_cmd_write_eeprom;


typedef union
{
/*
 * data field is used to make all commands the same length
 */
	unsigned char data[BOOT_CMD_SIZE];
	boot_cmd_header header;
	boot_cmd_read_flash read_flash;
	boot_cmd_write_flash write_flash;
	boot_cmd_erase_flash erase_flash;
	boot_cmd_get_fw_ver get_fw_ver;
	boot_cmd_reset reset;
	boot_cmd_read_id read_id;
	boot_cmd_write_id write_id;
	boot_cmd_read_eeprom read_eeprom;
	boot_cmd_write_eeprom write_eeprom;
} boot_cmd;

/*
 * common for all responses fields
 */
typedef struct
{
	unsigned char cmd;		/* command code, see BOOT_xxx constants */
	unsigned char echo;		/* echo is used to link between command and response */
} boot_rsp_header;

/*
* READ_FLASH - read flash memory
*/
typedef struct
{
	unsigned char cmd;		/* command code, see BOOT_xxx constants */
	unsigned char echo;		/* echo is used to link between command and response */
	unsigned char addr_lo;		/* address must be divisible by 2 */
	unsigned char addr_hi;
	unsigned char reserved[1];
	unsigned char size8;		/* size must be divisible by 8*/
	unsigned char data[BOOT_RSP_SIZE - 6];
} boot_rsp_read_flash;

/*
* WRITE_FLASH - write flash memory
*/
typedef struct
{
	unsigned char cmd;		/* command code, see BOOT_xxx constants */
	unsigned char echo;		/* echo is used to link between command and response */
} boot_rsp_write_flash;

/*
* ERASE_FLASH - erase flash memory
*/
typedef struct
{
	unsigned char cmd;		/* command code, see BOOT_xxx constants */
	unsigned char echo;		/* echo is used to link between command and response */
} boot_rsp_erase_flash;

/*
 * GET_FW_VER
 */
typedef struct
{
	unsigned char cmd;		/* command code, see BOOT_xxx constants */
	unsigned char echo;		/* echo is used to link between command and response */
	unsigned char major;
	unsigned char minor;
	unsigned char sub_minor;
} boot_rsp_get_fw_ver;

/*
* READ_ID - read ID locations
*/
typedef struct
{
	unsigned char cmd;		/* command code, see BOOT_xxx constants */
	unsigned char echo;		/* echo is used to link between command and response */
	unsigned char reserved[4];
	unsigned char data[BOOT_ID_LEN];
} boot_rsp_read_id;

/*
* WRITE_ID - write ID locations
*/
typedef struct
{
	unsigned char cmd;		/* command code, see BOOT_xxx constants */
	unsigned char echo;		/* echo is used to link between command and response */
} boot_rsp_write_id;


/*
 * READ_EEPROM - read EEPROM memory
 */
typedef struct
{
	unsigned char cmd;		/* command code, see BOOT_xxx constants */
	unsigned char echo;		/* echo is used to link between command and response */
	unsigned char addr;		/* EEPROM address the data was read from*/
	unsigned char reserved[2];
	unsigned char size;		/* size of EEPROM data */
	unsigned char data[BOOT_RSP_SIZE - 6];
} boot_rsp_read_eeprom;

/*
* WRITE_EEPROM - write EEPROM memory
*/
typedef struct
{
	unsigned char cmd;		/* command code, see BOOT_xxx constants */
	unsigned char echo;		/* echo is used to link between command and response */
} boot_rsp_write_eeprom;


typedef union
{
/*
 * data field is used to make all responses the same length
 */
	unsigned char data[BOOT_RSP_SIZE];
	boot_rsp_header header;
	boot_rsp_read_flash read_flash;
	boot_rsp_write_flash write_flash;
	boot_rsp_erase_flash erase_flash;
	boot_rsp_get_fw_ver get_fw_ver;
	boot_rsp_read_id read_id;
	boot_rsp_write_id write_id;
	boot_rsp_read_eeprom read_eeprom;
	boot_rsp_write_eeprom write_eeprom;
} boot_rsp;

#endif /*_H_INCLUDED*/
