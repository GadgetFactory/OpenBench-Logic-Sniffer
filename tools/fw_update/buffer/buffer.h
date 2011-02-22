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

#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

#include "../osdep/osdep.h"

/**
*	Buffer with direct access.
*/
class Buffer
{
public:
	class Iterator;
	Buffer(); /**< Constructs an empty buffer. */
	~Buffer(); /**< Deletes internal data */

	/**
	*	Constructs buffer of size specified.
	*	The buffer data will be filled with \a fill value.
	*
	*	@param size	The size of the buffer.
	*	@param fill	The value to fill buffer with. Default = 0xFF.
	*/
	Buffer(size_t size, unsigned char fill = 0xFF);



	/**
	*	Fill buffer with value provided.
	*
	*	@param val			Byte value with which to fill internal buffer.
	*/
	void fill(unsigned char val);

	/**
	*	Reads internal buffer data from specified addr.
	*
	*	Copies data starting from \a addr address of the internal buffer into \a buf.
	*	If end of internal buffer reached, only part of the \a buf will be filled.
	*	Check return value for number bytes copied.
	*
	*	@param buf			Pointer to a block of memory to receive the read data.
	*	@param buf_size		Maximum number of bytes to read.
	*	@param addr			Address to read from.
	*	@return				The number of bytes actually read.
	*/
	size_t read(unsigned char* buf, size_t buf_size, size_t addr);

	/**
	*	Overwrites internal buffer with data provided
	*
	*	@param buf		Pointer to the buffer containing the data to be written.
	*	@param buf_size	Number of bytes to be written.
	*	@param addr		Address of the first byte provided in \a buf.
	*   @throws out_of_range Data couldn't fit into buffer.
	*/
	void write(unsigned char* buf, size_t buf_size, size_t addr);

	/**
	*	Resize the internal buffer.
	*	All previous data will be lost. The buffer data will be filled with \a fill value.
	*	Call \a resize(0) to delete the internal buffer.
	*
	*	@param size	The size of the new internal buffer.
	*	@param fill	The value to fill buffer with. Default = 0xFF.
	*/
	void resize(size_t size, unsigned char fill = 0xFF);

	/**
	*	Returns the size of the internal buffer.
	*
	*   @return Buffer size.
	*/
	size_t size() {return _size;}

	/**
	*	Swaps bytes in internal buffer.
	*	Block of \a word_size bytes swaps to reverse order.
	*	Useful to switch between big-endian and little-endian bytes order.
	*   @param	word_size	The size in bytes of the blocks to be switched to reverse order.
	*/
	void swapBytes(unsigned long word_size);
protected:
	Buffer(const Buffer&){}; /**< Disables copy constructor. */
	unsigned char *_data;
	size_t _size;
};


#endif // BUFFER_H_INCLUDED

