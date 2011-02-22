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

#include "argumentimage.h"
#include "../exception/exception.h"
//#include "encoder_usage.h"
#include <stdlib.h>

ArgumentImage::ArgumentImage(const unsigned int  argument, const Parameters &params)
: _data(NULL), _size(0), _readAddress(0), _params(params)
{
	try
	{
		char buffer[3], *end;
		string data = params[argument].value();
		size_t current;
		if(data.size() & 0x01)
			throw DEBadValue(params[argument].argument());
		_size = data.size()/2;
		_data = new (unsigned char[_size]);
		for (current = 0; current < _size; current++)
		{
			buffer[0] = data[current*2];
			buffer[1] = data[current*2+1];
			buffer[2] = 0;
			_data[current] = (unsigned char)(strtoul(buffer, &end, 16));
			if (*end)
				throw DEBadValue(params[argument].argument());
		}

	}
	catch(...)
	{
		if (_data != NULL)
		{
			delete[] _data;
			_data = NULL;
			_size = 0;
		}
		throw;
	}
}

ArgumentImage::~ArgumentImage()
{
	if (_data != NULL)
	{
		delete[] _data;
		_data = NULL;
		_size = 0;
	}
}

void ArgumentImage::open(bool read)
{
}

void ArgumentImage::close()
{
}

size_t ArgumentImage::read(unsigned char *buffer, size_t bufSize, size_t *address)
{
	if (_readAddress == _size)
		return 0;
	size_t pc;
	if (_readAddress + bufSize > _size)
		pc = _size - _readAddress;
	else
		pc = bufSize;
	memcpy(buffer, &_data[_readAddress], pc);
	*address = _readAddress;
	_readAddress += pc;
	return pc;
}

void ArgumentImage::write(unsigned char *, size_t)
{
}
