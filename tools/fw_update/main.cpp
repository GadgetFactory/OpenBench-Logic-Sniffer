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

#include "osdep/osdep.h"

#include "u2io_fw_update.h"
#include "pic_bootloader.h"
#include "parser/parser.h"
#include "parser/parameters.h"
#include "parser/parser_exceptions.h"
#include "fu_usage.h"
#include "buffer/buffer.h"
#include "buffer/iterator.h"
#include "image/argumentimage.h"
#include "image/binimage.h"
#include "image/intel_hex_image.h"
#include "image/cout_image.h"
#include "exception/exception.h"

#define FW_UPDATE_VERSION_STRING	"0.2.0"

Image* createInputImage(const Parameters& params)
{
	if (params.contain(ARG_INPUT_BIN))
		return new BinImage(params[ARG_INPUT_BIN].value());
	if (params.contain(ARG_IMAGE_DATA))
		return new ArgumentImage(ARG_IMAGE_DATA, params);
	if (params.contain(ARG_INPUT_HEX))
		return new IntelHexImage(params[ARG_INPUT_HEX].value());

	throw DEImageAbsent();
}

Image* createOutputImage(const Parameters& params)
{
	if (params.contain(ARG_OUTPUT_BIN))
		return new BinImage(params[ARG_OUTPUT_BIN].value());
	if (params.contain(ARG_IMAGE_DATA))
		return new CoutImage();
	if (params.contain(ARG_OUTPUT_HEX))
		return new IntelHexImage(params[ARG_OUTPUT_HEX].value());

	throw DEImageAbsent();
}

void save(size_t start, Buffer::Iterator *iter, Image * image, const Parameters& params)
{
	try
	{
		const size_t dataSize = 0x100;
		unsigned char data[dataSize];
		size_t size, address;
		image->setWriteAddress(start + iter->address());
		while(0 != (size = iter->read(data, dataSize, &address)))
		{
			image->write(data, size);
		}
	}
	catch(...)
	{
		throw;
	}
}

size_t load(Buffer* buffer, PicBootloader * device, const PicBootloader::MemoryType memory, const Parameters& params)
{
	Image *image = NULL;
	const size_t dataSize = 0x100;
	unsigned char data[dataSize];
	size_t max = 0;
	size_t start, end;

	try
	{
		device->mem_range(memory, &start, &end);

		if (params.contain(ARG_INPUT_HEX))
			end += start;
		else
			start = 0;

		image = createInputImage(params);
		image->open(true);
		size_t size, address;
		while (0 != (size = image->read(data, dataSize, &address)))
		{
			if ((address >= start) && (address < end))
			{
				buffer->write(data, size, address - start);
				max = (address - start) + size;
			}
		}
		image->close();
		delete image;
		image = NULL;
		// buffer size have to be devidable by 8
		if (max % 8 != 0)
			max = (max/8+1)*8;
		return max;
	}
	catch(...)
	{
		if (image != NULL)
			delete image;
		throw;
	}
}

void erase(PicBootloader *device, const Parameters& params)
{
	PicBootloader::MemoryType memory = device->memory_type(params);

	for (int i = 0; i < BOOT_MEM_COUNT; i++)
	{
		if (memory & (1 << i))
		{
			PicBootloader::MemoryType mem = (PicBootloader::MemoryType)(1 << i);

			if (mem & PicBootloader::MEM_FLASH)
			{
				device->erase(device->memory_str(PicBootloader::MEM_FLASH), params);
			}
			else
			{
				size_t start, end, size, dummy;
				// start is an address for Image (HEX file)
				// mem_range() is used to determine a size for memory type
				device->mem_range(mem, &start, &size);
				// Need to adjust start address
				// Other values are ignored
				device->mem_border(mem, size, &start, &end, &dummy, params);

				Buffer buffer(size, 0xFF);
				Buffer::Iterator iter(&buffer);
				iter.begin(start);
				iter.end(size); // Always erase all locations in memory type
				device->program(&iter, device->memory_str(mem), params);
			}
		}
	}
}

typedef struct {
	size_t start;
	Buffer *buffer;
	Buffer::Iterator *iter;
} BUFFERS_T;

void read(PicBootloader *device, const Parameters& params)
{
	Image *image = NULL;
	BUFFERS_T buffers[BOOT_MEM_COUNT];
	PicBootloader::MemoryType memory = device->memory_type(params);

	try
	{
		for (int i = 0; i < BOOT_MEM_COUNT; i++)
		{
			buffers[i].buffer = NULL;
			buffers[i].iter = NULL;

		}
		for (int i = 0; i < BOOT_MEM_COUNT; i++)
		{
			if (memory & (1 << i))
			{
				PicBootloader::MemoryType mem = (PicBootloader::MemoryType)(1 << i);
				size_t start, end, size;
				// start is an address for Image (HEX file)
				// mem_range() is used to determine a size for memory type
				device->mem_range(mem, &start, &size);
// 				Buffer buffer(size);
				buffers[i].start = start;
				buffers[i].buffer = new Buffer(size);
// 				cout << "start = " << start << " end = " << end << " size = " << size << endl;
				device->mem_border(mem, size, &start, &end, &size, params);

// 				Buffer::Iterator iter(&buffer);
				buffers[i].iter = new Buffer::Iterator(buffers[i].buffer);

// 				cout << "start = " << start << " end = " << end << " size = " << size << endl;
				buffers[i].iter->begin(start);
				buffers[i].iter->end(end);
				device->read(buffers[i].iter, device->memory_str(mem), params);

// 				cout << "start = " << start << " end = " << end << " size = " << size << endl;
				buffers[i].iter->begin(start);
				buffers[i].iter->end(end);
			}
		}
		image = createOutputImage(params);
		image->open(false);
		for (int i = 0; i < BOOT_MEM_COUNT; i++)
		{
			if (buffers[i].buffer != NULL)
			{
				save(buffers[i].start, buffers[i].iter, image, params);
				delete buffers[i].buffer;
				delete buffers[i].iter;
			}
		}
		image->close();
		delete image;
		image = NULL;
	}
		catch (...)
	{
		if (image != NULL)
			delete image;
	}
}

void verify(PicBootloader *device, const Parameters& params)
{
	PicBootloader::MemoryType memory = device->memory_type(params);

	for (int i = 0; i < BOOT_MEM_COUNT; i++)
	{
		if (memory & (1 << i))
		{
			PicBootloader::MemoryType mem = (PicBootloader::MemoryType)(1 << i);
			size_t max, start, end, size;
			// start is an address for Image (HEX file)
			// mem_range() is used to determine a size for memory type
			device->mem_range(mem, &start, &size);
			Buffer buffer(size);
			max = load(&buffer, device, mem, params);
			if (max == 0)
			{
				cerr << "WARNING: No data for memory type = " << device->memory_str(mem) << endl;
				continue;
			}

			device->mem_border(mem, max, &start, &end, &size, params);

// 			cout << "start = " << start << " end = " << end << " size = " << size << endl;
			Buffer::Iterator iter(&buffer);
			iter.begin(start);
			iter.end(end);
			device->verify(&iter, device->memory_str(mem), params);
		}
	}
}

void write(PicBootloader *device, const Parameters& params)
{
	PicBootloader::MemoryType memory = device->memory_type(params);

	for (int i = 0; i < BOOT_MEM_COUNT; i++)
	{
		if (memory & (1 << i))
		{
			PicBootloader::MemoryType mem = (PicBootloader::MemoryType)(1 << i);
			size_t max, start, end, size;
			// start is an address for Image (HEX file)
			// mem_range() is used to determine a size for memory type
			device->mem_range(mem, &start, &size);
			Buffer buffer(size);
			max = load(&buffer, device, mem, params);
			if (max == 0)
			{
				cerr << "WARNING: No data for memory type = " << device->memory_str(mem) << endl;
				continue;
			}

			device->mem_border(mem, max, &start, &end, &size, params);

// 			cout << "start = " << start << " end = " << end << " size = " << size << endl;
			Buffer::Iterator iter(&buffer);
			iter.begin(start);
			iter.end(end);
			device->program(&iter, device->memory_str(mem), params);
		}
	}
}

void getversion(PicBootloader *device, const Parameters& params)
{
	boot_rsp response;

	device->getversion(&response, params);
	cout << "BootLoader Version" << ": " << (unsigned int)response.get_fw_ver.major << "."
			<< (unsigned int)response.get_fw_ver.minor << "."
			<< (unsigned int)response.get_fw_ver.sub_minor
			<< endl;
}

int main (int argc, char *argv[])
{
	bool reset = true;

	try
	{
		FUUsage usage;
		Parser parser(usage);
		Parameters params(usage);
		parser.parse(argc, argv, &params);

		if (params.contain(ARG_HELP))
		{
			cout << "USAGE:\n";
			cout << "fw_update <COMMAND> [OPTIONS...]\n";
			usage.print();
			return 0;
		}

		if (params.contain(ARG_VERSION))
		{
			cout << "fw_update Version: " << FW_UPDATE_VERSION_STRING << endl;
			PicBootloader device;
			getversion(&device, params);
			return 0;
		}

		int commands = 0;

		if (params.contain(ARG_READ))
			commands |= 1 << ARG_READ;
		if (params.contain(ARG_WRITE))
			commands |= 1 << ARG_WRITE;
		if (params.contain(ARG_VERIFY))
			commands |= 1 << ARG_VERIFY;
		if (params.contain(ARG_ERASE))
			commands |= 1 << ARG_ERASE;
		if (params.contain(ARG_RESET))
			reset = params[ARG_RESET].boolValue();

		if (commands == 0)
		{
			cout << "At least one command must be specified.\n";
			return 1;
		}

		/* First check if we have VID/PID values */
		if (!params.contain(ARG_VID))
			throw DEArgumentAbsent(usage[ARG_VID].shortArgument());
		if (!params.contain(ARG_PID))
			throw DEArgumentAbsent(usage[ARG_PID].shortArgument());

		int img_args = 0;
		if (params.contain(ARG_INPUT_BIN))
			img_args++;
		if (params.contain(ARG_IMAGE_DATA))
			img_args++;
		if (params.contain(ARG_INPUT_HEX))
			img_args++;
		if (img_args > 1)
			throw DEArgumentMultipleUsage("input image");

		img_args = 0;
		if (params.contain(ARG_OUTPUT_BIN))
			img_args++;
		if (params.contain(ARG_IMAGE_DATA))
			img_args++;
		if (params.contain(ARG_OUTPUT_HEX))
			img_args++;
		if (img_args > 1)
			throw DEArgumentMultipleUsage("output image");

		PicBootloader device;
		/* Check if memory type is valid */
		if (device.memory_type(params) == PicBootloader::MEM_ALL)
		{
			if (params.contain(ARG_ADDR))
				cerr << "WARNING: argument \"" << params[ARG_ADDR].argument() << "\" ignored for memory type = all" << endl;
			if (params.contain(ARG_DATA_SIZE))
				cerr << "WARNING: argument \"" << params[ARG_DATA_SIZE].argument() << "\" ignored for memory type = all" << endl;
			if (commands & (1 << ARG_WRITE))
			{
				if (!params.contain(ARG_INPUT_HEX))
				{
					string img;
					if (params.contain(ARG_INPUT_BIN))
						img = params[ARG_INPUT_BIN].argument();
					if (params.contain(ARG_IMAGE_DATA))
						img = params[ARG_IMAGE_DATA].argument();
					cerr << "Write to memory type ALL supported on HEX images only." << endl;
					throw DEBadArgument(img);
				}
			}
		}
		if (commands & (1 << ARG_READ))
		{
			read(&device, params);
		}
		if (commands & (1 << ARG_ERASE))
		{
			erase(&device, params);
		}
		if (commands & (1 << ARG_WRITE))
		{
			write(&device, params);
		}
		if (commands & (1 << ARG_VERIFY))
		{
			verify(&device, params);
		}

		if (reset)
		{
			cout << "RESET Device" << endl;
			device.reset(params);
		}

		cout << "Operation successfully completed.\n";
	}
	catch(DException &err)
	{
		cout << err.getErrMessage() << endl;
		cout << "Operation aborted.\n";
		return 2;
	}
	catch (EParserException &err)
	{
		cout << err.what() << endl;
		return 3;
	}
	catch (...)
	{
		cout << "Unknown exception.\n";
		return 4;
	}
}

/* End of file */
