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

#include "cout_image.h"
#include "../exception/exception.h"
#include "../dtrace/dtrace.h"

CoutImage::CoutImage()
	: _writeAddress(0),
			_linAddr(1)  // LOW 4 bit MUST be nonzero at first time write() called
{
}

CoutImage::~CoutImage()
{
}

void CoutImage::open(bool read)
{
	if (read)
		throw DEImageAbsent();
}

void CoutImage::close()
{
	printf("\n");
}

size_t CoutImage::getSize()
{
	eTrace0("getSize in CoutImage");
	return 0;
}

size_t CoutImage::read(unsigned char*, size_t, size_t*)
{
	throw DEImageAbsent();
}

void CoutImage::write(unsigned char *buffer, size_t bufSize)
{
	for (size_t i = 0; i < bufSize; i++)
	{
		if ((_linAddr & 0x0FU) || ((_linAddr & 0xFFFFFFF0U) != (_writeAddress & 0xFFFFFFF0U)) )
		{
			printf("\n0x%08X: ", _writeAddress & 0xFFFFFFF0U);
			for (size_t offs = 0; offs < (_writeAddress & 0x0FU); offs++)
				printf("na ");
			_linAddr = _writeAddress & 0xFFFFFFF0U;
		}
		printf("%02X ", buffer[i]);
		_writeAddress++;
	}
}

void CoutImage::setWriteAddress(size_t addr)
{
	_writeAddress = addr;
}

/* End of file */
