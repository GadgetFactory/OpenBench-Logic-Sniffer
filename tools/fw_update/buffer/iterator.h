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

#ifndef ITERATOR_H_INCLUDED
#define ITERATOR_H_INCLUDED

#include "../osdep/osdep.h"

#include "buffer.h"
/**
*	Provides sequencing access to \a Buffer class.
*/
class Buffer::Iterator
{
public:
	/**
	*	Initializes buffer sequencer.
	*
	*	@param buffer	Buffer to provide sequencing access to.
	*/
	Iterator(Buffer* buffer);

	/**
	*	Destroys buffer sequencer.
	*	Doesn't free internal buffer (provided in constructor).
	*/
	~Iterator();


	/**
	*	Sets the page size.
	*	Buffer returned using \a read() or \a readBack() will fit into the page.
	*
	*	@param size Page size to set. \a page(0) cancels page boundaries check.
	*/
	void page(size_t size) {_page = size;};

	/**
	*	Returns current page size.
	*	Buffer returned using \a read() or \a readBack() will fit into the page.
	*
	*	@return Current page size. \a 0 means no page boundaries check applied.
	*/
	size_t page() {return _page;};

	/**
	*	Sets current address.
	*	Address has to fit into access boundaries.
	*	Use  \a begin() and \a end() to obtain lower and upper boundaries.
	*
	*   @param	addr Address to start sequence access from.
	*	@throws out_of_range When /a addr do not fit into access boundaries.
	*/
	void address(size_t addr);

	/**
	*	Returns current address.
	*
	*   @return Current address.
	*/
	size_t address();

	/**
	*	Returns the size of the block that can be used for single \a write or \a read operation.
	*	Takes into consideration current address, page size and
	*	upper boundary of the buffer accessed part.
	*
	*   @return Size of the block for single \a write or \a read operation.
	*/
	size_t blockSize();

	///**
	//*	Returns size of the buffer to be access.
	//*
	//*   @return	Size of the encapsulated buffer.
	//*/
	//size_t size();

	//void size(size_t size);

	/**
	*	Sets lower boundary for the part of the buffer to be accessed via iterator.
	*	Resets current address to value provided.
	*	Use \a address(size_t) to change current address without changing the boundary.
	*
	*   @param	address Address of the first element to be accessed via iterator.
	*   @throws out_of_range When /a address exceeds the buffer size.
	*/
	void begin(size_t address);

	/**
	*	Returns lower boundary for the part of the buffer to be accessed via iterator.
	*	Do not confuse with current address which can be obtained using \a address() function.
	*
	*   @return Address of the first element to be accessed via iterator.
	*/
	size_t begin() {return _begin;};

	/**
	*	Sets upper boundary for the part of the buffer to be accessed via iterator.
	*
	*   @param	address Address just beyond of the last accessed element.
	*   @throws out_of_range When /a address exceeds the buffer size.
	*/
	void end(size_t address);

	/**
	*	Returns upper boundary for the part of the buffer to be accessed via iterator.
	*
	*   @return Address just beyond of the last accessed element.
	*/
	size_t end() {return _end;};


	/**
	*	Reads block of data from encapsulated buffer.
	*	Reading block will fit into device memory page.
	*
	*   @param	buf		Pointer to the buffer that receives the data.
	*   @param	bufSize Maximum number of bytes to be read.
	*	@param	addr	Pointer to the value to be filled with block start address.
	*   @return	Number of bytes actually read.
	*			It depends on page size, \bufSize and encapsulated buffer size.
	*/
	size_t read(unsigned char *buf, size_t bufSize, size_t *addr);
	/**
	*	Write block of data into encapsulated buffer.
	*	Page boundaries check is not applied.
	*
	*   @param	buf		Pointer to the buffer containing the data to be written.
	*   @param	bufSize	Number of bytes to be written.
	*   @throws out_of_range When data couldn't fit into encapsulated buffer.
	*/
	void write(unsigned char *buf, size_t bufSize);

	/**
	*	Returns pointer to encapsulated buffer.
	*
	*   @return	Pointer to encapsulated buffer.
	*/
	Buffer* getBuffer() {return _buffer;};

	/**
	*	Returns progress in percents of the current address with respect to access boundaries.
	*
	*   @return Current address progress in percents.
	*/
	unsigned int progress();

private:
	/**
	*	Page size.
	*	Buffer returned using \a read() or \a readBack() will fit into the page.
	*/
	size_t _page;
	size_t _address; /**< Current address. */
	Buffer* _buffer; /**< Pointer to encapsulated buffer. */
	size_t _begin; /**< Address to begin sequence access from. */
	size_t _end; /**< Address just beyond of the last accessed element. */
};

#endif //ITERATOR_H_INCLUDED
