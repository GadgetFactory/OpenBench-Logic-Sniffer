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

#include <string>

#include "u2io_fw_update.h"
#include "pic_bootloader.h"
#include "exception/exception.h"
#include "buffer/buffer.h"
#include "buffer/iterator.h"
#include "fu_usage.h"
#include <cstdlib>
#ifdef BUILD_WIN32

#pragma comment(lib, "hid.lib")
#pragma comment(lib, "setupapi.lib")

#include <wtypes.h>
#include <sal.h>
#include <sal_supp.h>
#include <driverspecs.h>
#include <windows.h>

extern "C"
{
    #include <api/hidsdi.h>
}

#include <setupapi.h>

#else
#include <usb.h>
#endif /* BUILD_WIN32 */ 

PicBootloader::MemoryType PicBootloader::memory_type(const Parameters& params)
{
	if (params.contain(ARG_MEM_TYPE))
	{
		if (params[ARG_MEM_TYPE].value() == "flash")
			return MEM_FLASH;
		else
			if (params[ARG_MEM_TYPE].value() == "eeprom")
				return MEM_EEPROM;
		else
			if (params[ARG_MEM_TYPE].value() == "id")
				return MEM_ID;
		else
			if (params[ARG_MEM_TYPE].value() == "all")
				return MEM_ALL;
		else
		{
			list<string> l;
			l.push_back("flash");
			l.push_back("eeprom");
			l.push_back("id");
			l.push_back("all");
			throw DEBadValue(params[ARG_MEM_TYPE].argument(), l);
		}
	}
	return MEM_FLASH;
}

PicBootloader::MemoryType PicBootloader::memory_type(std::string memory)
{
	if (memory == "flash")
		return MEM_FLASH;
	else
	if (memory == "eeprom")
			return MEM_EEPROM;
	else
	if (memory == "id")
			return MEM_ID;
	else
	if (memory == "all")
			return MEM_ALL;
	else
	{
		list<string> l;
		l.push_back("flash");
		l.push_back("eeprom");
		l.push_back("id");
		l.push_back("all");
		throw DEBadValue(memory, l);
	}
	return MEM_FLASH;
}

std::string PicBootloader::memory_str(MemoryType memory)
{
	switch (memory)
	{
		case MEM_FLASH:
			return std::string("flash");
		case MEM_EEPROM:
			return std::string("eeprom");
		case MEM_ID:
			return std::string("id");
		case MEM_ALL:
			return std::string("all");
	}
	return std::string("");
}

void PicBootloader::mem_range(MemoryType memory, size_t * start, size_t * size)
{
	switch (memory)
	{
		case MEM_FLASH:
			*start = BOOT_FLASH_ADDR;
			*size = BOOT_FLASH_SIZE;
			return;
		case MEM_EEPROM:
			*start = BOOT_EEPROM_ADDR;
			*size = BOOT_EEPROM_SIZE;
			return;
		case MEM_ID:
			*start = BOOT_ID_ADDR;
			*size = BOOT_ID_SIZE;
			return;
		default:
			*start = 0;
			*size = 0;
	}
}

void PicBootloader::mem_border(PicBootloader::MemoryType memory, size_t max_size, size_t * start, size_t * end, size_t * size, const Parameters &params)
{
	/* Adjust start address to point to start of current memory type */
	*start = 0;
	*end = max_size;
	if (memory == MEM_FLASH)
		*start = BOOT_DEFAULT_ADDR;

	/* Check if user provide non-default addtress and size
	   Ignore if memory type is ALL
	*/
	if (memory != MEM_ALL)
	{
		if (params.contain(ARG_ADDR))
		{
			*start = params[ARG_ADDR].ulongValue();
			if (*start >= max_size)
				throw DEBadValue(params[ARG_ADDR].argument(), 0, max_size);
			/* Adjust address to 8 byte boundary */
			*start &= ~7UL;
		}
		if (params.contain(ARG_DATA_SIZE))
		{
			*size = params[ARG_DATA_SIZE].ulongValue();
			if (*size == 0)
				throw DEBadValue(params[ARG_DATA_SIZE].argument(), 8, max_size);
			/* Adjust size to 8 byte boundary */
			*end = *size & 7;
			*size &= ~7UL;
			if (*end & 7)
				*size += 8;
		}
	}
	*end = *start + *size;
	if (*end > max_size)
		*end = max_size;
	if (*end <= *start)
		throw DEBadValue(params[ARG_DATA_SIZE].argument(), 0, max_size - *start);
}

size_t PicBootloader::page_size(PicBootloader::MemoryType memory)
{
	switch (memory)
	{
		case MEM_FLASH:
			return 2;
		case MEM_EEPROM:
			return 8;
		case MEM_ID:
			return BOOT_ID_LEN;
		default:
			return 8;
	}
}

void PicBootloader::erase(string memory, const Parameters &params)
{
	PicBootloader::MemoryType mem = memory_type(memory);
	// EEPROM an ID eraising not supported
	// Use programm() with Buffer filled by 0xFF for EEPROM and ID
	if (mem != MEM_FLASH)
		return;

	boot_cmd command;
	boot_rsp response;
	memset(&command, 0, sizeof(boot_cmd));
	memset(&response, 0, sizeof(boot_rsp));
	command.erase_flash.cmd = BOOT_ERASE_FLASH;

	try
	{
		show_progress(OP_ERASING, type(), memory, 0);
		open(params);
		// erase 0x800-0x4000 //!!! 18f24j50 change
		command.erase_flash.echo = ++command_id;
		command.erase_flash.addr_hi = 0x08;
		command.erase_flash.addr_lo = 0x00;
		command.erase_flash.size_x64 = 0x0D;
		transaction(&command, &response);
		close();
		show_progress(OP_ERASING, type(), memory, OPERATION_DONE);
	}
	catch(...)
	{
		show_progress(OP_ERASING, type(), memory, OPERATION_FAILED);
		throw;
	}
}

void PicBootloader::reset(const Parameters &params)
{
	boot_cmd command;
	memset(&command, 0, sizeof(boot_cmd));
	command.reset.cmd = BOOT_RESET;
	open(params);
	send_command(&command);
	close();
}

void PicBootloader::verify(Buffer::Iterator* buffer, string memory, const Parameters &params)
{
	PicBootloader::MemoryType mem = memory_type(memory);
	unsigned char cmd;
	size_t size, address, def_addr, def_size;
	boot_cmd command;
	boot_rsp response;
	boot_cmd reference;

	memset(&reference, 0xFF, sizeof(boot_cmd));
	prepare_cmd(mem, true, &cmd, &def_addr, &def_size, &command, &response);
	buffer->page(page_size(mem));

	try
	{
		show_progress(OP_VERIFYING, type(), memory, 0);
		open(params);
		while(0 != (size = buffer->read(reference.write_flash.data, page_size(mem), &address)))
		{
			if (address < def_addr)
			{
				eTrace2("address = %d memory = %s", address, memory.c_str());
				continue;
			}
			if (address >= def_size)
				continue;

			command.header.cmd = cmd;
			command.read_flash.echo = ++command_id;
			command.read_flash.addr_hi = (unsigned char)((address >> 8) & 0xFF);
			command.read_flash.addr_lo = (unsigned char)(address & 0xFF);
			// size must be 8 dividable
			if (size % 8 != 0)
				size = (size/8 + 1) * 8;
			eAssert(size <= page_size(mem));
			command.read_flash.size8 = (unsigned char)size;
			for(int retry = 0; true; retry++)
			{
				transaction(&command, &response);
				if (memcmp(response.read_flash.data, reference.write_flash.data, size) == 0)
					break;
				printf("retry = %d\n\n", retry);
				if (retry == MAX_HID_RETRY)
					throw DEVerificationFailed();
			}
			show_progress(OP_VERIFYING, type(), memory, buffer->progress());
			memset(&reference, 0xFF, sizeof(boot_cmd));
		}
		close();
		show_progress(OP_VERIFYING, type(), memory, OPERATION_DONE);
	}
	catch(...)
	{
		show_progress(OP_VERIFYING, type(), memory, OPERATION_FAILED);
		if (m_hDevice != INVALID_HANDLE_VALUE) /* If device is not opened close() raise new unhandled exception */
			close();
		throw;
	}

}

void PicBootloader::read(Buffer::Iterator* buffer, string memory, const Parameters &params)
{
	PicBootloader::MemoryType mem = memory_type(memory);
	size_t size, address, def_addr, def_size;
	unsigned char cmd;
	boot_cmd command;
	boot_rsp response;

	prepare_cmd(mem, true, &cmd, &def_addr, &def_size, &command, &response);
	buffer->page(page_size(mem));

	try
	{
		show_progress(OP_READING, type(), memory, 0);
		open(params);
		while(0 != (size = buffer->blockSize()))
		{
			address = buffer->address();
			if (address >= def_size)
				continue;

			command.header.cmd = cmd;
			command.read_flash.echo= ++command_id;
			command.read_flash.addr_hi = (unsigned char)((address >> 8) & 0xFF);
			command.read_flash.addr_lo = (unsigned char)(address & 0xFF);
			// size must be 8 dividable
			if (size % 8 != 0)
				size = (size/8 + 1) * 8;
			eAssert(size <= page_size(mem));
			command.read_flash.size8 = (unsigned char)size;

			transaction(&command, &response);
			buffer->write(response.read_flash.data, size);
			show_progress(OP_READING, type(), memory, buffer->progress());
			memset(&response, 0xFF, sizeof(boot_rsp));
		}
		close();
		show_progress(OP_READING, type(), memory, OPERATION_DONE);
	}
	catch(...)
	{
		show_progress(OP_READING, type(), memory, OPERATION_FAILED);
		if (m_hDevice != INVALID_HANDLE_VALUE) /* If device is not opened close() raise new unhandled exception */
			close();
		throw;
	}
}

void PicBootloader::program(Buffer::Iterator* buffer, string memory, const Parameters &params)
{
	PicBootloader::MemoryType mem = memory_type(memory);
	size_t size, address, def_addr, def_size;
	unsigned char cmd;
	boot_cmd command;
	boot_rsp response;

	prepare_cmd(mem, false, &cmd, &def_addr, &def_size, &command, &response);
	buffer->page(page_size(mem));

	try
	{
		show_progress(OP_PROGRAMMING, type(), memory, 0);
		open(params);
		while(0 != (size = buffer->read(command.write_flash.data, page_size(mem), &address)))
		{
			if (address < def_addr)
			{
				eTrace2("address = %d memory = %s", address, memory.c_str());
				continue;
			}
			if (address >= def_size)
				continue;

			command.header.cmd = cmd;
			command.write_flash.echo = ++command_id;
			command.write_flash.addr_hi = (unsigned char)((address >> 8) & 0xFF);
			command.write_flash.addr_lo = (unsigned char)(address & 0xFF);
			command.write_flash.flush = (unsigned char) 0xff;
			// size must be 8 dividable
			//eAssert(size % 8 == 0);
			command.write_flash.size8 = size;
			transaction(&command, &response);
			show_progress(OP_PROGRAMMING, type(), memory, buffer->progress());
			memset(&command, 0xFF, sizeof(boot_cmd));
		}
		close();
		show_progress(OP_PROGRAMMING, type(), memory, OPERATION_DONE);
	}
	catch(...)
	{
		show_progress(OP_PROGRAMMING, type(), memory, OPERATION_FAILED);
		if (m_hDevice != INVALID_HANDLE_VALUE) /* If device is not opened close() raise new unhandled exception */
			close();
		throw;
	}

}

void PicBootloader::getversion(boot_rsp * response, const Parameters &params)
{
	string memory("BootLoader Version");

	boot_cmd command;
	memset(&command, 0, sizeof(boot_cmd));
	memset(response, 0, sizeof(boot_rsp));
	command.get_fw_ver.cmd = BOOT_GET_FW_VER;
	try
	{
		show_progress(OP_READING, type(), memory, 0);
		open(params);
		transaction(&command, response);
		close();
		show_progress(OP_READING, type(), memory, 100);
		show_progress(OP_READING, type(), memory, OPERATION_DONE);
	}
	catch(...)
	{
		show_progress(OP_READING, type(), memory, OPERATION_FAILED);
		close();
		throw;
	}
}

PicBootloader::PicBootloader()
: m_hDevice(INVALID_HANDLE_VALUE), command_id(0)
{

}

PicBootloader::~PicBootloader()
{
	eAssert(m_hDevice == INVALID_HANDLE_VALUE);
	if (m_hDevice != INVALID_HANDLE_VALUE)
		CloseHandle(m_hDevice);
}

void PicBootloader::open(const Parameters &params)
{
	eAssert(m_hDevice == INVALID_HANDLE_VALUE);
	try
	{
#ifdef BUILD_WIN32
		GUID HidGuid;
		HidD_GetHidGuid( &HidGuid);
		HDEVINFO hDevInfo;
		hDevInfo = SetupDiGetClassDevs(&HidGuid, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
		if (hDevInfo == INVALID_HANDLE_VALUE)
		{
			eTrace0("INVALID_HANDLE_VALUE");
			return;
		}
		SP_DEVICE_INTERFACE_DATA DevInterfaceData;
		DevInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

		for (unsigned long DevIndex = 0; true; DevIndex++)
		{
			if (!SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &HidGuid, DevIndex, &DevInterfaceData))
				break;
			unsigned long DetailsSize;
			SetupDiGetDeviceInterfaceDetail(hDevInfo, &DevInterfaceData, NULL, 0, &DetailsSize, NULL);
			PSP_INTERFACE_DEVICE_DETAIL_DATA pDetails =
				(PSP_INTERFACE_DEVICE_DETAIL_DATA) malloc(DetailsSize);
			if (pDetails == NULL)
			{
				SetupDiDestroyDeviceInfoList(hDevInfo);
				eTrace0("Couldn't allocate memory for device interface detail data");
				return;
			}
			pDetails->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
			if (!SetupDiGetDeviceInterfaceDetail(hDevInfo, &DevInterfaceData, pDetails, DetailsSize, NULL, NULL))
			{
				free(pDetails);
				SetupDiDestroyDeviceInfoList(hDevInfo);
				eTrace0("SetupDiGetDeviceInterfaceDetail failed");
				return;
			}
			std::string DevicePath(pDetails->DevicePath);

			if (isValidDevice(DevicePath, params))
			{
				// Valid device found
				m_hDevice = CreateFile(DevicePath.c_str(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
//					NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
				free(pDetails);
				break;
			}

			free(pDetails);
		}
		SetupDiDestroyDeviceInfoList(hDevInfo);
#else
		struct usb_bus *busses;
		struct usb_bus *bus;
		struct usb_device *dev;
		struct usb_dev_handle *devHandle;
		bool found = false;

		/* for some reason the second command fails, if we don't let the PIC to settle */
		usleep(500000);
		m_hDevice = INVALID_HANDLE_VALUE;

		usb_init();
		usb_find_busses();
		usb_find_devices();

		busses = usb_get_busses();
		for (bus = busses; bus; bus = bus->next)
		{
			for (dev = bus->devices; dev; dev = dev->next)
			{
				if (isValidDevice(dev, params))
				{
					// Valid device found
					found = true;
					devHandle = usb_open(dev);

					if (devHandle == NULL)
						break;
					
					char buf[40];
					int ret = usb_get_driver_np(devHandle, 0, buf, sizeof(buf));
					if (ret == 0) {
						usb_detach_kernel_driver_np(devHandle, 0);
					}

					if (usb_claim_interface(devHandle, 0) < 0) {
						usb_close(devHandle);
						break;
					}
					// This code reached if everything above is OK
					m_hDevice = devHandle;
					break;
				}
			}
			if (found)
				break;
		}
#endif /* BUILD_WIN32 */
	}
	catch(...)
	{
		// toDo: add clean up
		throw;
	}
	if (m_hDevice == INVALID_HANDLE_VALUE)
		throw DEDeviceAbsent();
}

void PicBootloader::close()
{
	eAssert(m_hDevice != INVALID_HANDLE_VALUE);
#ifdef BUILD_WIN32
	CloseHandle(m_hDevice);
#else
	struct usb_dev_handle *dev = (struct usb_dev_handle *)m_hDevice;
	usb_release_interface(dev, 0);
	usb_close(dev);
#endif
	m_hDevice = INVALID_HANDLE_VALUE;
}

#ifdef BUILD_WIN32
bool PicBootloader::isValidDevice(string DevicePath, const Parameters &params)
{
	HANDLE hHidDevice = CreateFile(DevicePath.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,				// no SECURITY_ATTRIBUTES structure
		OPEN_EXISTING,			// no special create flags
		0,				// no special attributes
		NULL);				// no template file

	if (hHidDevice == INVALID_HANDLE_VALUE)
		return NULL;

	HIDD_ATTRIBUTES Attr;
	if (!HidD_GetAttributes (hHidDevice, &Attr)) {
		CloseHandle(hHidDevice);
		return false;
	}
	CloseHandle(hHidDevice);

	if ((Attr.VendorID == params[ARG_VID].ushortValue()) && (Attr.ProductID == params[ARG_PID].ushortValue()))
		return true;
	else
		return false;
}
#else
bool PicBootloader::isValidDevice(struct usb_device *dev, const Parameters &params)
{
	if ((dev->descriptor.idVendor == params[ARG_VID].ushortValue()) && (dev->descriptor.idProduct == params[ARG_PID].ushortValue()))
		return true;

	return false;
}
#endif /* BUILD_WIN32 */

void PicBootloader::send_command(boot_cmd* pOut)
{
#ifdef BUILD_WIN32
	unsigned char write_buf[sizeof(boot_cmd)+1];
	OVERLAPPED write_over;
	try
	{
		memset(&write_over, 0, sizeof(OVERLAPPED));
		write_over.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		unsigned long written;
		memcpy(write_buf + 1, pOut, sizeof(boot_cmd));
		write_buf[0] = 0;
		BOOL write_res = WriteFile(m_hDevice, write_buf, sizeof(boot_cmd) + 1, &written, &write_over);
		if ((!write_res)
			&& (GetLastError() != ERROR_IO_PENDING))
		{
			eTrace1("LastError = %d", GetLastError());
			throw DEHidWriteFailed();
		}
		if (WaitForSingleObject(write_over.hEvent, 100) == WAIT_TIMEOUT)
		{
			eTrace0("Write timeout");
			throw DEHidWriteTimeout();
		}
		CloseHandle(write_over.hEvent);
	}
	catch(...)
	{
		CancelIo(m_hDevice);
		CloseHandle(write_over.hEvent);
		throw;
	}
#else
	try
	{
		int retval;
		struct usb_dev_handle *dev = (struct usb_dev_handle *) m_hDevice;
		/*
		    bmRequestType = Host to Device, Class type, Interface recipient
		    bRequest      = SET_REPORT
		    wValue        = 0
		    wIndex        = 0
		    wSize         = determined by libusb
		 */
		retval = usb_control_msg(dev, 0x21, 0x09, 0x0000, 0x0000, (char *)pOut, sizeof(boot_cmd), CTRL_TIMEOUT);
		if (retval < 0)
		{
			eTrace2("LastError = %d, %s\n", errno, strerror(errno));
			throw DEHidWriteFailed();
		}
		if (retval != sizeof(boot_cmd))
		{
			eTrace2("Writen less data then expected: %d != %d\n", sizeof(boot_cmd), retval);
			throw DEHidWriteFailed();
		}
	}
	catch (...)
	{
		throw;
	}
#endif /* BUILD_WIN32 */
}


void PicBootloader::transaction(boot_cmd* pOut, boot_rsp* pIn)
{
#ifdef BUILD_WIN32
	OVERLAPPED write_over, read_over;
	unsigned char read_buf[sizeof(boot_rsp)+1];
	unsigned char write_buf[sizeof(boot_cmd)+1];
	try
	{
		memset(&write_over, 0, sizeof(OVERLAPPED));
		memset(&read_over, 0, sizeof(OVERLAPPED));
		write_over.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		read_over.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		unsigned long readed, written;
		if ((!ReadFile(m_hDevice, read_buf, sizeof(boot_rsp)+1, &readed, &read_over))
			&& (GetLastError() != ERROR_IO_PENDING))
		{
			eTrace1("LastError = %d", GetLastError());
			throw DEHidReadFailed();
		}
		memcpy(write_buf + 1, pOut, sizeof(boot_cmd));
		write_buf[0] = 0;
		BOOL write_res = WriteFile(m_hDevice, write_buf, sizeof(boot_cmd) + 1, &written, &write_over);
		if ((!write_res)
			&& (GetLastError() != ERROR_IO_PENDING))
		{
			eTrace1("LastError = %d", GetLastError());
			throw DEHidWriteFailed();
		}
		if (WaitForSingleObject(write_over.hEvent, 100) == WAIT_TIMEOUT)
		{
			eTrace0("Write timeout");
			throw DEHidWriteTimeout();
		}
		unsigned long res;
		if (WAIT_TIMEOUT == (res = WaitForSingleObject(read_over.hEvent, 5000)))
		{
			eTrace0("Write timeout");
			throw DEHidReadTimeout();
		}
		if ((!GetOverlappedResult(m_hDevice, &read_over, &readed, FALSE))
			|| (readed != sizeof(boot_rsp) + 1))
		{
			eTrace2("LastError = %d, readed = %d", GetLastError(), readed);
			throw DEHidReadFailed();
		}
		memcpy(pIn, read_buf + 1, sizeof(boot_rsp));
		if ((pOut->header.cmd != pIn->header.cmd) || (pOut->header.echo != pIn->header.echo))
		{
			eTrace2("pOut->header.cmd = %d,  pIn->header.cmd = %d", pOut->header.cmd, pIn->header.cmd);
			eTrace2("pOut->header.echo = %d,  pIn->header.echo = %d", pOut->header.echo, pIn->header.echo);
			throw DEHidResponseIdMismatch();
		}
		CloseHandle(write_over.hEvent);
		CloseHandle(read_over.hEvent);

	}
	catch(...)
	{
		CancelIo(m_hDevice);
		CloseHandle(write_over.hEvent);
		CloseHandle(read_over.hEvent);
		throw;
	}
#else
	struct usb_dev_handle *dev = (struct usb_dev_handle *) m_hDevice;
	int retval;

	try
	{
		for (int retry = 0; true; retry++)
		{
			memset((char *)pIn, 0, sizeof(boot_rsp));
			/*
			bmRequestType = Host to Device, Class type, Interface recipient
			bRequest      = SET_REPORT
			wValue        = 0
			wIndex        = 0
			wSize         = determined by libusb
			*/
			retval = usb_control_msg(dev, 0x21, 0x09, 0x0000, 0x0000, (char *)pOut, sizeof(boot_cmd), CTRL_TIMEOUT);
			if (retval < 0)
			{
				eTrace2("LastError = %d, %s\n", errno, strerror(errno));
				if (retry >= MAX_HID_RETRY)
					throw DEHidWriteFailed();
				else
					continue;
			}
			if (retval != sizeof(boot_cmd))
			{
				eTrace2("Writen less data then expected: %d != %d\n", sizeof(boot_cmd), retval);
				if (retry >= MAX_HID_RETRY)
					throw DEHidWriteFailed();
				else
					continue;
			}
			retval = usb_interrupt_read(dev, 1, (char *)pIn, sizeof(boot_rsp), INT_TIMEOUT);
			if (retval < 0)
			{
				eTrace2("LastError = %d, %s\n", errno, strerror(errno));
				if (retry >= MAX_HID_RETRY)
					throw DEHidReadFailed();
				else
					continue;
			}
			if (retval != sizeof(boot_rsp))
			{
				eTrace2("Read less data then expected: %d != %d\n", sizeof(boot_rsp), retval);
				if (retry >= MAX_HID_RETRY)
					throw DEHidReadFailed();
				else
					continue;
			}
			if ((pOut->header.cmd != pIn->header.cmd) || (pOut->header.echo != pIn->header.echo))
			{
				eTrace2("pOut->cmd = %d,  pIn->cmd = %d\n", pOut->header.cmd, pIn->header.cmd);
				eTrace2("pOut->id = %d,  pIn->id = %d\n", pOut->header.echo, pIn->header.echo);
				if (retry >= MAX_HID_RETRY)
					throw DEHidResponseIdMismatch();
				else
					continue;
			}
			if (retval >= 0)
				break;
		}
	}
	catch(...)
	{
		throw;
	}
#endif /* BUILD_WIN32 */
}

void PicBootloader::prepare_cmd(PicBootloader::MemoryType memory, bool rd, unsigned char * cmd, size_t * def_addr, size_t * def_size, boot_cmd * command, boot_rsp * response)
{
	memset(command, 0xFF, sizeof(boot_cmd));
	memset(response, 0xFF, sizeof(boot_rsp));
	switch (memory)
	{
		case MEM_FLASH:
			*def_addr = BOOT_DEFAULT_ADDR;
			*def_size = BOOT_FLASH_SIZE;
			if (rd)
				*cmd = BOOT_READ_FLASH;
			else
				*cmd = BOOT_WRITE_FLASH;
			break;
		case MEM_EEPROM:
			*def_addr = 0;
			*def_size = BOOT_EEPROM_SIZE;
			if (rd)
				*cmd = BOOT_READ_EEPROM;
			else
				*cmd = BOOT_WRITE_EEPROM;
				break;
		case MEM_ID:
			*def_addr = 0;
			*def_size = BOOT_ID_SIZE;
			if (rd)
				*cmd = BOOT_READ_ID;
			else
				*cmd = BOOT_WRITE_ID;
			break;
		default:
			throw DEBadMemoryType("BootLoader", memory_str(memory));
	}
}

/* End of file */
