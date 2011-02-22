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

#include <stdexcept>
#include "buffer.h"
#include "../dtrace/dtrace.h"

using namespace std;

Buffer::Buffer()
:	_data(NULL), _size(0)
{
}

Buffer::~Buffer()
{
	if (_data != NULL)
	{
		delete[] _data;
	}
}

Buffer::Buffer(size_t size, unsigned char fill /* = 0xFF */)
: _data(NULL), _size(0)
{
	resize(size, fill);
}

void Buffer::fill(unsigned char val)
{
	if ((_data == NULL) || (_size == 0))
	{
		eTrace2("data = %d, dataSize = %d", _data, _size);
		return;
	}
	memset(_data, val, _size);
}

size_t Buffer::read(unsigned char* buf, size_t buf_size, size_t addr)
{
	eAssert(_data != NULL); eAssert(addr < _size);
	size_t size;
	if (addr + buf_size > _size)
		size = _size - addr;
	else
		size = buf_size;
	memcpy(buf, _data+addr, size);
	return size;
}

void Buffer::write(unsigned char* buf, size_t buf_size, size_t addr)
{
	eAssert(buf != NULL); eAssert(addr + buf_size <= _size); eAssert(_data != NULL);
	if (addr + buf_size > _size)
		throw out_of_range("Buffer is full.");
	memcpy(_data+addr, buf, buf_size);
}
void Buffer::resize(size_t size, unsigned char fill /* = 0xFF */)
{
	if (_data != NULL)
	{
		delete[] _data;
		_data = NULL;
	}
	if (size != 0)
		_data = new unsigned char[size];
	_size = size;
	memset(_data, fill, _size);
}

void Buffer::swapBytes(unsigned long word_size)
{
	size_t current, i, j;
	unsigned char temp;
	for (current = 0; current <= _size - word_size; current += word_size)
	{
		for (i = current, j = current + word_size - 1; i < j; i++, j--)
		{
			temp = _data[j];
			_data[j] = _data[i];
			_data[i] = temp;
		}
	}
}
