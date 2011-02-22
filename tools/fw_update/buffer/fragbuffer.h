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

#ifndef FRAGBUFFER_H_INCLUDED
#define FRAGBUFFER_H_INCLUDED

#include "../osdep/osdep.h"

#include <map>

/**
 *	FragBuffer - Fragmented buffer for HEX files with holes.
 */
class FragBuffer : public std::map<size_t, unsigned char>
{
public:
	~FragBuffer(); /**< Deletes internal data */

	/**
	 * Constructs an empty buffer.
	 * The buffer data will be filled with \a fill value.
	 *
	 * @param size	The size of the buffer.
	 * @param fill	The value to fill buffer with. Default = 0xFF.
	 */
	FragBuffer(unsigned char fill = 0xFF);

	/**
	 * @brief Fill buffer with value provided.
	 *
	 * Fill buffer with value provided. Value is stored for further usage.
	 * If latter user request for nonexistent data extra data will filled with this value.
	 *
	 * @param val	Byte value with which to fill internal buffer.
	 */
	void fill(unsigned char val);

	/**
	 * @brief Reads internal buffer data from specified addr.
	 *
	 * Copies data starting from \a addr address of the internal buffer into \a buf.
	 * If \a padding is \b false and buffer contains a chunk less then requested
	 * a smaller chunk will copied to destination buffer and return value is a number of bytes
	 * actually copied to destination buffer.
	 *
	 * If \a padding is \b true and requested chunk is bigger then interal chunk for
	 * specified address range or end of internal buffer reached, destination buffer will padded with default value.
	 * Buffer itself does not extended.
	 *
	 * If buffer does not hold any data in requested address range this function returns zero.
	 *
	 * Check return value for number bytes copied.
	 *
	 * @param buf			Pointer to a block of memory to receive the read data.
	 * @param buf_size		Maximum number of bytes to read.
	 * @param addr			Address to read from.
	 * @param padding		If true destination buffer will be padded with default value.
	 * @return			The number of bytes actually read.
	 *
	 * @sa fill
	 */
	size_t read(unsigned char* buf, size_t buf_size, size_t addr, bool padding = false);

	/**
	 * Overwrites internal buffer with data provided
	 *
	 * @param buf		Pointer to the buffer containing the data to be written.
	 * @param buf_size	Number of bytes to be written.
	 * @param addr		Address of the first byte provided in \a buf.
	 * @throws out_of_range Data couldn't fit into buffer.
	 */
	void write(unsigned char* buf, size_t buf_size, size_t addr);

	/**
	 * Swaps bytes in internal buffer.
	 * Block of \a word_size bytes swaps to reverse order.
	 * Useful to switch between big-endian and little-endian bytes order.
	 * @param	word_size	The size in bytes of the blocks to be switched to reverse order.
	 */
	void swapBytes(const size_t word_size);

	/**
	 * @brief Ensure the non-existent byte returned with valid value.
	 *
	 * \sa fill
	 */
	unsigned char & operator [] (size_t);

	/**
	 * @brief Check if data in buffer exists.
	 *
	 * Check if data exists in buffer.
	 * Returns true if byte at address \a addr exists.
	 *
	 * @param	addr	An address to check.
	 *
	 * @return	true if data with \a addr exists, false otherwise.
	 */
	bool exists(size_t addr);

	/**
	 * @brief Check if data in buffer exists.
	 *
	 * Check if data exists in buffer. This function is like above but check for range.
	 * Returns true if at least one byte exists in address range from \a from_addr to \a from_addr plus \a size .
	 *
	 * @param	from_addr	A start address to check.
	 * @param	size		Size of address block to check.
	 *
	 * @return	true if any data exists in specified range, false otherwise.
	 */
	bool exists(size_t from_addr, size_t size);

	/**
	 * @brief Return an address for given iterator
	 *
	 * @param	it	Iterator for data
	 * @return	Address of data
	 *
	 * @sa value
	 */
	size_t address(const FragBuffer::iterator & it);

	/**
	 * @brief Return a value for given iterator
	 *
	 * @param	it	Iterator for data
	 * @return	Value of data
	 *
	 * @sa address
	 */
	unsigned char value(const FragBuffer::iterator & it);

protected:
	FragBuffer(const FragBuffer&){}; /**< Disables copy constructor. */
	unsigned char _fillVal;
	size_t read_data(unsigned char* buf, size_t buf_size, size_t addr);
};

#endif // FRAGBUFFER_H_INCLUDED
