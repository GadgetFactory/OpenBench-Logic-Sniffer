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

#include "osdep/osdep.h"

#include "fu_usage.h"
#include <string>
using namespace std;


FUUsage::FUUsage()
{
	add(UsageDescriptor(
		ARG_WRITE,
		"w",
		"write",
		UsageDescriptor::absent,
		"",
		"Write data to device.",
		UsageDescriptor::command
		));

	add(UsageDescriptor(
		ARG_READ,
		"r",
		"read",
		UsageDescriptor::absent,
		"",
		"Read data from device.",
		UsageDescriptor::command
		));

	add(UsageDescriptor(
		ARG_VERIFY,
		"v",
		"verify",
		UsageDescriptor::absent,
		"",
		"Verify device memory.",
		UsageDescriptor::command
		));

	add(UsageDescriptor(
		ARG_ERASE,
		"e",
		"erase",
		UsageDescriptor::absent,
		"",
		"Erase device memory.",
		UsageDescriptor::command
		));

	add(UsageDescriptor(
		ARG_VERSION,
		"ver",
		"version",
		UsageDescriptor::absent,
		"",
		"Display versions of fw_update and BootLoader if present.",
		UsageDescriptor::command
		));

	add(UsageDescriptor(
		ARG_HELP,
		"h",
		"help",
		UsageDescriptor::absent,
		"",
		"Print this message.",
		UsageDescriptor::command
		));

	add(UsageDescriptor(
		ARG_MEM_TYPE,
		"m",
		"memory",
		UsageDescriptor::required,
		"MEMMORY TYPE",
		"Memory type for read, write and verify operations."
		"\n\tPossible values: flash, eeprom, id, all.",
		UsageDescriptor::option,
		"flash"));

	add(UsageDescriptor(
		ARG_VID,
		"vid",
		"vendorid",
		UsageDescriptor::required,
		"VID",
		"Device vendor ID.",
		UsageDescriptor::option
		));

	add(UsageDescriptor(
		ARG_PID,
		"pid",
		"productid",
		UsageDescriptor::required,
		"PID",
		"Device product ID.",
		UsageDescriptor::option
		));

	add(UsageDescriptor(
		ARG_RESET,
		"t",
		"reset",
		UsageDescriptor::required,
		"YES | NO",
		"Reset device after operation is finished.",
		UsageDescriptor::option,
		"YES"
		));

	add(UsageDescriptor(
		ARG_DATA_SIZE,
		"s",
		"size",
		UsageDescriptor::required,
		"SIZE",
		"The data size for read, write and verify operations.",
		UsageDescriptor::option
		));

	add(UsageDescriptor(
		ARG_ADDR,
		"a",
		"address",
		UsageDescriptor::required,
		"ADDR",
		"The start address for read, write and verify operations."
		"\n\tDefault: 0x800 for flash, 0x00 for other memory types.",
		UsageDescriptor::option
		/* !!! CAUTION !!!
			Do NOT use default parameter!
			May be INVALID for different memory types!
		*/
		));

	add(UsageDescriptor(
		ARG_INPUT_HEX,
		"ix",
		"image_hex_in",
		UsageDescriptor::required,
		"FILE_NAME",
		"Image file in Intel Hex Format for write and verify operations.",
		UsageDescriptor::option
		));

	add(UsageDescriptor(
		ARG_INPUT_BIN,
		"ib",
		"image_bin_in",
		UsageDescriptor::required,
		"FILE_NAME",
		"Image file in Binary Format for write and verify operations.",
		UsageDescriptor::option
		));

	add(UsageDescriptor(
		ARG_OUTPUT_HEX,
		"ox",
		"image_hex_out",
		UsageDescriptor::required,
		"FILE_NAME",
		"Image file in Intel Hex Format for read operation to save data.",
		UsageDescriptor::option
		));

	add(UsageDescriptor(
		ARG_OUTPUT_BIN,
		"ob",
		"image_bin_out",
		UsageDescriptor::required,
		"FILE_NAME",
		"Image file in Binary Format for read operation to save data.",
		UsageDescriptor::option
		));

	add(UsageDescriptor(
		ARG_IMAGE_DATA,
		"d",
		"data",
		UsageDescriptor::possible,
		"DATA",
		"Immediate mode - actual byte(s) specified in the command line."
		"\n\tData for write and verify operations must be provided"
		"\n\tas string of two hex digits per byte (like 01202040ac)."
		"\n\tFor read operation data is printed on screen.",
		UsageDescriptor::option
		));
}

/* End of file */
