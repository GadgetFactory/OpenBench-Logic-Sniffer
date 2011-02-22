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

#include "../osdep/osdep.h"
#include "../exception/exception.h"
#include "../dtrace/dtrace.h"
#include "intel_hex_image.h"

IntelHexImage::IntelHexImage(string name)
	: _name(name), _file(NULL), _size(0),
		_linAddr(1) // LOW 16 bit MUST be nonzero at first time write() called
{
}

IntelHexImage::~IntelHexImage()
{
	eAssert(_file == NULL);
}

void IntelHexImage::setWriteAddress(size_t addr)
{
	_writeAddr = addr;
}

void IntelHexImage::open(bool read)
{

	if (read)
	{
		/* NOTE: "t" mode do nothing on non-Windows platform!!! See also  IntelHexImage::read() */
		_file = fopen(_name.c_str(), "rt");
		if (_file == NULL)
			throw DEFileOpenFailed(_name, read);
		_size = calcSize();
	}
	else
	{
		/* NOTE: "t" mode do nothing on non-Windows platform!!! See also  IntelHexImage::write() */
		_file = fopen(_name.c_str(), "wt");
		if (_file == NULL)
			throw DEFileOpenFailed(_name, read);
		_size = 0;
	}
	_read = read;
	_readCurrent = 0;
	_readAddr = 0;
	_writeAddr = 0;
	_linAddr = 1; // LOW 16 bit MUST be nonzero at first time write() called
	_record.clear();
}

void IntelHexImage::close()
{
	if (!_read)
	{
		_record.InitEndOfData();
		string str = _record.GetString();
#ifdef NEED_CRLF_TRANSLATION
/* NOTE: Because in most cases HEX files created on Windows with MPLAB we need CRLF */
			if ((str[str.size() - 1] == '\n') && (str[str.size() - 2] != '\r'))
				str.insert(str.size() - 1, "\r");
#endif
		int tmp = fputs(str.c_str(), _file);
		if (tmp < 0)
		{
			eTrace1("tmp = %d", tmp);
			throw DEFileWriteFailed();
		}
	}
	if (fclose(_file) != 0)
		throw DEFileCloseFailed(_name);
	_file = NULL;
}

size_t IntelHexImage::calcSize()
{
	size_t max = 0;
	const int buf_size = 0x100;
	unsigned char buffer[buf_size];
	size_t size, address;
	while (0 != (size = read(buffer, buf_size, &address)))
	{
		if (address + size > max)
			max = address + size;
	}
	fseek(_file, 0, SEEK_SET);
	return max;
}


size_t IntelHexImage::read(unsigned char *buffer, size_t bufSize, size_t *addr)
{
	const size_t buffer_size = 500;
	char f_buffer[buffer_size];
	char* fgets_rc;
	size_t pc;
	size_t address;

	while (_readCurrent >= _record.m_Size)
	{	// read from file
		fgets_rc = fgets(f_buffer, buffer_size, _file);
		if (fgets_rc == NULL)
		{
			if (feof(_file))
				return 0;
			else
				throw DEFileReadFailed();
		}
#ifdef NEED_CRLF_TRANSLATION
/* NOTE: Because fopen() do nothing with "t" mode on non-Windows we need to translate CRLF -> LF manually */
		size_t len = strlen(f_buffer);
		if ((f_buffer[len - 2] == '\r') && (f_buffer[len - 1] == '\n'))
		{
			f_buffer[len - 2] = '\n';
			f_buffer[len - 1] = 0;
		}
#endif
		_record.InitFromString(f_buffer);
		switch(_record.m_Type)
		{
		case cHexTypeData:
			_readCurrent = 0;
			break;
		case cHexTypeExtLinearAddr:
		case cHexTypeExtSegmentAddr:
			_readAddr = _record.GetExtAddr();
			_record.clear();
			break;
		case cHexTypeEndOfData:
			_record.clear();
			break;
		default:
			eTrace1("_record.m_Type = %d", _record.m_Type);
			throw DEFileReadFailed();
		}
	}
	if (bufSize > _record.m_Size - _readCurrent)
		pc = _record.m_Size - _readCurrent;
	else
		pc = bufSize;
	memcpy(buffer, &_record.m_Data[_readCurrent], pc);

	address = _readAddr & 0xFFFF0000;
	address |= _record.m_Addr;
	address += _readCurrent;
	*addr = address;

	_readCurrent += pc;
	return pc;
}

void IntelHexImage::write(unsigned char *buffer, size_t bufSize)
{
	size_t address, offset, current;
	string str;
	CIntelHexRec record;
	for (offset = 0; offset < bufSize; offset += current)
	{
		address = _writeAddr + offset;
		if ((_linAddr & 0xFFFFU) || ((_linAddr & 0xFFFF0000U) != (_writeAddr & 0xFFFF0000U)) || ((address != 0) && (address%0x10000 == 0)))
		{
			record.InitExtAddr(HIWORD(address));
			str = record.GetString();
#ifdef NEED_CRLF_TRANSLATION
/* NOTE: Because in most cases HEX files created on Windows with MPLAB we need CRLF */
			if ((str[str.size() - 1] == '\n') && (str[str.size() - 2] != '\r'))
				str.insert(str.size() - 1, "\r");
#endif
			int tmp = fputs(str.c_str(), _file);
			if (tmp < 0)
			{
				eTrace1("tmp = %d", tmp);
				throw DEFileWriteFailed();
			}
			_linAddr = _writeAddr & 0xFFFF0000U;
		}
		if (offset + 0x10 > bufSize)
			current = bufSize - offset;
		else
			current = 0x10;

		if (address/0x10000 != (address + current)/0x10000)
			current = 0x10000 - (address%0x10000);
		if (current != 0)
		{
			record.InitData((unsigned char)current, LOWORD(address),buffer+offset);
			str = record.GetString();
#ifdef NEED_CRLF_TRANSLATION
/* NOTE: Because in most cases HEX files created on Windows with MPLAB we need CRLF */
			if ((str[str.size() - 1] == '\n') && (str[str.size() - 2] != '\r'))
				str.insert(str.size() - 1, "\r");
#endif
			int tmp = fputs(str.c_str(), _file);
			if (tmp < 0)
			{
				eTrace1("tmp = %d", tmp);
				throw DEFileWriteFailed();
			}
		}
		else
			eTrace0("current = 0");
	}
	_writeAddr += bufSize;
}

