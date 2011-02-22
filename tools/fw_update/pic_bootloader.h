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

#ifndef PIC_BOOTLOADER_H_INCLUDED
#define PIC_BOOTLOADER_H_INCLUDED

#include "osdep/osdep.h"

#include "parser/parameters.h"
#include "buffer/buffer.h"
#include "buffer/iterator.h"
#include "boot_if.h"
#ifdef BUILD_WIN32
/* Nothing WIN32 specific */
#else
#include <usb.h>
#define CTRL_TIMEOUT	500
#define INT_TIMEOUT	500
#endif /* BUILD_WIN32 */

#define MAX_HID_RETRY	4

#define BOOT_DEFAULT_ADDR	0x800	/* First 2K occupied by BootLoader */
#define BOOT_FLASH_ADDR		0x00
#define BOOT_ID_ADDR		0x00200000
#define BOOT_CONFIG_ADDR	0x00300000
#define BOOT_EEPROM_ADDR	0x00F00000
#define BOOT_FLASH_SIZE		0x6000	/* Program Memory Size */
#define BOOT_EEPROM_SIZE	0x100	/* EEPROM Memory Size */
#define BOOT_ID_SIZE		0x08	/* ID Locations size */
#define BOOT_CONFIG_SIZE	0x0E	/* CONFIG Space Size */

#define BOOT_MEM_COUNT		3

class PicBootloader
{
public:
	// MUST be ordered by address in HEX file
	typedef enum {
		MEM_FLASH = 0x01,
		MEM_ID = 0x02,
		MEM_EEPROM = 0x04,
// TODO CONFIG Space.
// 		MEM_CONFIG = 0x08,
		MEM_ALL = MEM_FLASH | MEM_ID | MEM_EEPROM,
	} MemoryType;

	PicBootloader();
	~PicBootloader();

	/**
	 *	Return memory type from command line parameters
	 *
	 *	@param	params	Command line parameters
	 *	@return		Type of memory
	 */
	MemoryType memory_type(const Parameters& params);

	/**
	 *	Return memory type from string name
	 *
	 *	@param	memory	Name of memory type
	 *	@return		Type of memory
	 */
	MemoryType memory_type(std::string memory);

	/**
	 *	Return a name for memory type
	 *
	 *	@param	memory	Type of memory
	 *	@return		Name of memory type as string
	 */
	std::string memory_str(MemoryType memory);

	/**
	 *	Return a start address and size of a given memory type
	 *
	 *	@param	memory	Type of memory
	 *	@param	start	Start address of given memory type stored here
	 *	@param	size	Size of given memory type stored here
	 */
	void mem_range(MemoryType memory, size_t * start, size_t * size);

	/**
	 *	Parse command line and fill memory borders for
	 *	current memory type
	 *
	 *	@param	memory		Type of memory
	 *	@param	max_size	Size of memory of given type. This may be a size of data read from image.
	 *	@param	start		Start for Buffer::Iterator
	 *	@param	end		End for Buffer::Iterator
	 *	@param	size		Size of memory type
	 *	@param	params		Command line parameters
	 */
	void mem_border(MemoryType memory, size_t max_size, size_t * start, size_t * end, size_t * size, const Parameters &params);

	/**
	 *	Return a page size for given memory type
	 *
	 *	@param	memory	Type of memory
	 *	@return		Page size
	 */
	size_t page_size(MemoryType memory);

	/**
	*	Erases device memory.
	*	Device subtype has to be initialized before function calling.
	*
	*   @param	memory	Memory type to be erased.
	*					If empty string is provided entire device will be erased.
	*   @throws
	*/
	virtual void erase(string memory, const Parameters &params);

	/**
	*	Reads device memory.
	*	Device subtype has to be initialized before function calling.
	*
	*   @param	buffer	Buffer to store read data in.
	*   @param	memory	Memory type to be read.
	*			If empty string is provided default device memory will be read.
	*   @throws
	*/
	virtual void read(Buffer::Iterator* buffer, string memory, const Parameters &params);

	/**
	*	Programs device memory.
	*	Device subtype has to be initialized before function calling.
	*
	*   @param	buffer	Buffer to get data from.
	*   @param	memory	Memory type to be programmed.
	*			If empty string is provided default device memory will be programmed.
	*   @throws
	*/
	virtual void program(Buffer::Iterator* buffer, string memory, const Parameters &params);

	/**
	*	Verifies device memory.
	*	Device subtype has to be initialized before function calling.
	*
	*   @param	buffer	Buffer containing data to verify device memory with.
	*   @param	memory	Memory type to be verified.
	*			If empty string is provided default device memory will be verified.
	*   @throws
	*/
	virtual void verify(Buffer::Iterator* buffer, string memory, const Parameters &params);

	/**
	 *	Read Bootloader version from device
	 *
	 *	@param	response	version of bootloader read from device
	 *	@param	params		command line options
	 */
	virtual void getversion(boot_rsp * response, const Parameters &params);

	virtual void reset(const Parameters &params);

private:
	void open(const Parameters &params);
	void close();
	void transaction(boot_cmd* pOut, boot_rsp* pIn);

	/**
	 *	Prepare to execute commands for given memory type
	 *
	 *	@param	memory		Type of memory
	 *	@param	rd		Operation: @c true for READ, @c false for WRITE
	 *	@param	cmd		Command code to send to device for given memory type
	 *	@param	def_addr	Default address for given memory type
	 *	@param	command		Command
	 *	@param	response	Reasponse
	 */
	void prepare_cmd(MemoryType memory, bool rd, unsigned char * cmd, size_t * def_addr, size_t * def_size, boot_cmd * command, boot_rsp * response);

	// the same as transaction but doesn't wait for response
	// useful for reset command (when device sends no response)
	void send_command(boot_cmd* pOut);
#ifdef BUILD_WIN32
	bool isValidDevice(string DevicePath, const Parameters &params);
#else
	bool isValidDevice(struct usb_device *dev, const Parameters &params);
#endif
	HANDLE m_hDevice;
	virtual string type() {return "U2IO";};
	unsigned char command_id;
//	Parameters m_Params;
};

#endif // PIC_BOOTLOADER_H_INCLUDED
