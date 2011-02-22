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

#include <map>
#include <stdexcept>
#include "fragbuffer.h"
#include "../dtrace/dtrace.h"

using namespace std;

FragBuffer::FragBuffer(unsigned char fill)
	: _fillVal(fill)
{
}

FragBuffer::~FragBuffer()
{
	clear();
}

void FragBuffer::fill(unsigned char val)
{
	_fillVal = val;
	for (FragBuffer::iterator it = begin(); it != end(); it++)
		(*it).second = _fillVal;
}

unsigned char & FragBuffer::operator[](size_t k)
{
	FragBuffer::iterator it = find(k);
	if( it != end())
		return (*it).second;
	std::map<size_t, unsigned char>::operator[](k) = _fillVal;
	return std::map<size_t, unsigned char>::operator[](k);
}

size_t FragBuffer::read_data(unsigned char* buf, size_t buf_size, size_t addr)
{
	size_t size = buf_size;
	size_t maxaddr = addr + buf_size - 1;

	/* Check for any data exists for requested chunk */
	if (!exists(addr, size))
		return 0;

	FragBuffer::iterator it = lower_bound(maxaddr);
	/* Check if requested more data then currently in buffer */
	if (it == end())
		it--;
	/* Check if there is a chunk size less then requested */
	if (address(it) > maxaddr)
		it--;
	size = address(it) - addr + 1;
	for (size_t i = 0; i < size; i++)
		buf[i] = (*this)[i + addr];
	return size;
}

size_t FragBuffer::read(unsigned char* buf, size_t buf_size, size_t addr, bool padding)
{
	size_t size;
	if (padding)
		memset(buf, _fillVal, buf_size);
	size = read_data(buf, buf_size, addr);
	if (padding && (size > 0))
		size = buf_size;
	return size;
}

void FragBuffer::write(unsigned char* buf, size_t buf_size, size_t addr)
{
	for (size_t i = 0; i < buf_size; i++)
		(*this)[i + addr] = buf[i];
}

void FragBuffer::swapBytes(const size_t word_size)
{
	size_t current, i, j, address;
	unsigned char temp;
	unsigned char * data;

	if ((word_size > 1) && ((word_size & 1U) == 0))
	{
		data = new unsigned char [word_size];
		FragBuffer::iterator it = begin();
		while (it != end())
		{
			address = (*it).first;
			/* Align data at word_size boundary */
			address = (address / word_size) * word_size;
			/* Ensure we have valid values for non-existance data */
			read(data, word_size, address, true);
			for (i = 0, j = word_size - 1; i < j; i++, j--)
			{
				temp = data[j];
				data[j] = data[i];
				data[i] = temp;
			}
			write(data, word_size, address);

			it = lower_bound(address + word_size);
		}
		delete[] data;
	}
	else
	{
		throw out_of_range("Word size must be even value and greatter then 1");
	}
}

bool FragBuffer::exists(size_t addr)
{
	return (find(addr) != end());
}

bool FragBuffer::exists(size_t from_addr, size_t size)
{
	for (size_t i = from_addr; i < (from_addr + size); i++)
	{
		if (exists(i))
			return true;
	}
	return false;
}

size_t FragBuffer::address(const FragBuffer::iterator & it)
{
	return (*it).first;
}

unsigned char FragBuffer::value(const FragBuffer::iterator & it)
{
	return (*it).second;
}

/* End of file */
