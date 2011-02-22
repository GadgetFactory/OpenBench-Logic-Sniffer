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

#include <assert.h>
#include "binimage.h"
#include "../exception/exception.h"
#include "../dtrace/dtrace.h"

BinImage::BinImage(string name)
	: _name(name), _file(NULL)
{
}

BinImage::~BinImage()
{
	eAssert(_file == NULL);
}


void BinImage::open(bool read)
{

	if (read)
	{
		_file = fopen(_name.c_str(), "rb");
		if (_file == NULL)
			throw DEFileOpenFailed(_name, read);
		fseek(_file, 0, SEEK_END);
		long pos = ftell(_file);
		assert(pos >= 0);
		_size = size_t(pos);
		fseek(_file, 0, SEEK_SET);
	}
	else
	{
		_file = fopen(_name.c_str(), "wb");
		if (_file == NULL)
			throw DEFileOpenFailed(_name, read);
	}
	_read = read;
	_readAddress = 0;
}

void BinImage::close()
{
	if (fclose(_file) != 0)
		throw DEFileCloseFailed(_name);
	_file = NULL;
}


/*
void BinImage::setSize(size_t memSize, size_t pageSize)
{
_memSize = memSize;
_pageSize = pageSize;
if (_read)
{
if (_size > _memSize)
throw DEFileLargeImage();
}
}


size_t BinImage::getSize()
{
assert(_read);
if (_isFill)
return _memSize;
else
return _size;
}
*/

size_t BinImage::read(unsigned char *buffer, size_t bufSize, size_t *addr)
{
	size_t pc = fread(buffer, 1, bufSize, _file);
	if ((pc == 0) && (feof(_file)==0))
		throw DEFileReadFailed();
	*addr = _readAddress;
	_readAddress += pc;
	return pc;
}

void BinImage::write(unsigned char *buffer, size_t bufSize)
{
	size_t pc = fwrite(buffer, 1, bufSize, _file);
	if (pc != bufSize)
		throw DEFileWriteFailed();
}

