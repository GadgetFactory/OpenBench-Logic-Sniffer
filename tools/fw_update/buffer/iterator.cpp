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
#include "../dtrace/dtrace.h"
#include "iterator.h"
#include "buffer.h"

using namespace std;

Buffer::Iterator::Iterator(Buffer* buffer)
: _page(0), _buffer(buffer)
{
	eAssert(_buffer != NULL);
	_begin = 0;
	_address = 0;
	_end = _buffer->size();
}


Buffer::Iterator::~Iterator()
{
}

void Buffer::Iterator::address(size_t addr)
{
	if ((addr < _begin) || (addr > _end))
		throw out_of_range("Address doesn't fit boundaries.");
	_address = addr;
}

size_t Buffer::Iterator::address()
{
	eAssert(_address >= _begin);
	eAssert(_address <= _end);
	return _address;
}

size_t Buffer::Iterator::blockSize()
{
	if (address() >= end())
		return 0;
	size_t next;
	if (page() == 0)
		next = end();
	else
		next = (address()/page() + 1)*page();
	if (next > end())
		next = end();
	return next - address();
}

size_t Buffer::Iterator::read(unsigned char *buf, size_t bufSize, size_t *addr)
{
	size_t size = blockSize();
	if (size > bufSize)
		size = bufSize;
	if (size == 0)
		return size;
	size_t pc = _buffer->read(buf, size, address());
	*addr = address();
	address(address() + pc);
	return pc;
}

void Buffer::Iterator::write(unsigned char *buf, size_t bufSize)
{
	if (address() + bufSize > end())
		throw out_of_range("Buffer is full.");
	_buffer->write(buf, bufSize, address());
	address(address() + bufSize);
}


void Buffer::Iterator::begin(size_t address)
{
	eAssert(_buffer != NULL);
	if (address > _buffer->size())
		throw out_of_range("Begin address exceeds buffer size.");
	_begin = address;
	_address = address;
}

void Buffer::Iterator::end(size_t address)
{
	eAssert(_buffer != NULL);
	if (address > _buffer->size())
		throw out_of_range("End address exceeds buffer size.");
	_end = address;
}

unsigned int Buffer::Iterator::progress()
{
	if (end() == begin())
	{
		eTrace2("begin = %d, end = %d", begin(), end());
		return 100;
	}
	return ((address()-begin())*100)/(end()-begin());
}

/* End of file */
