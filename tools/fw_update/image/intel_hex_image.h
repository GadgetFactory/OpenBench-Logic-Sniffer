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

#ifndef INTEL_HEX_IMAGE_H_INCLUDED
#define INTEL_HEX_IMAGE_H_INCLUDED

#include "../osdep/osdep.h"

#include "intel_hex_record.h"
#include "image.h"

class IntelHexImage: public Image
{
public:
	IntelHexImage(string name);
	virtual ~IntelHexImage();
	void open(bool read);
	void close();
	size_t read(unsigned char *buffer, size_t bufSize, size_t *addr);
	void write(unsigned char *buffer, size_t bufSize);
	virtual void setWriteAddress(size_t addr);
	size_t getSize() {return _size;};

private:
	// The function will be called from open() only. It affects the current file pointer.
	size_t calcSize();
	string _name;
	FILE* _file;
	bool _read;
	size_t _size;

	CIntelHexRec _record;
	size_t _readCurrent;
	size_t _readAddr;

	size_t _writeAddr;
	size_t _linAddr;
};



#endif // INTEL_HEX_IMAGE_H_INCLUDED

