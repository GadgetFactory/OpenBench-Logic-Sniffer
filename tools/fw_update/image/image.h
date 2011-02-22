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

#ifndef IMAGE_H_INCLUDED
#define IMAGE_H_INCLUDED

#include "../osdep/osdep.h"

class Image
{
public:
	class Clipper;
	virtual void open(bool read) = 0;
	virtual void close() = 0;
	virtual size_t getSize() = 0;

	/**
	*	Reads data from image and stores it into buffer provided.
	*	If \a size was set returned data do not exceed it.
	*	If \a offset was set returned address is calculated taking offset into account.
	*
	*   @param	buffer Pointer to the byte array to be filled with data read from image.
	*   @param	bufSize Maximum number of bytes to be read from image.
	*	@param	address	Current address of the first byte stored in buffer relatively to
	*		the beginning of the image or image \a offset if it was applied.
	*   @return Number of bytes actually read.
	*/
	virtual size_t read(unsigned char *buffer, size_t bufSize, size_t *address) = 0;
	// returns stored buffer size
	virtual void write(unsigned char *buffer, size_t bufSize) = 0;
	virtual void setWriteAddress(size_t addr) = 0;
};

#endif //IMAGE_H_INCLUDED
