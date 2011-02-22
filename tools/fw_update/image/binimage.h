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

#ifndef BINIMAGE_H_INCLUDED
#define BINIMAGE_H_INCLUDED

#include "../osdep/osdep.h"

#include <string>
#include "image.h"

using namespace std;

class BinImage: public Image
{
public:
	BinImage(string name);
	virtual ~BinImage();
	void open(bool read);
	void close();
	size_t getSize() {return _size;};
	size_t read(unsigned char *buffer, size_t bufSize, size_t *addr);
	void write(unsigned char *buffer, size_t bufSize);
	virtual void setWriteAddress(size_t addr) {;};

private:
	string _name;
	FILE *_file;
	size_t _size;
	bool _read;
	/**
	*	Stores address for sequence \a read operation.
	*	Address has to be incremented each time part of the file was read.
	*/
	size_t _readAddress;
};


#endif // BINIMAGE_H_INCLUDED
