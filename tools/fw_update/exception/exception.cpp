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
#include <string>
#include <list>
#include "exception.h"

using namespace std;

DEBadValue::DEBadValue(string argument)
{
	_message = string("Bad value provided for \"") + argument + string("\" argument.");
}

DEBadValue::DEBadValue(string argument, string rangeBegin, string rangeEnd)
{
	_message = string("Bad value provided for \"") + argument + string("\" argument.");
	_message += string("\nValue must be in range from ") + rangeBegin
			+ string(" to ") + rangeEnd +(".");
}

DEBadValue::DEBadValue(string argument, unsigned int rangeBegin, unsigned int rangeEnd)
{
	char str[12];
	sprintf(str, "%d", rangeBegin);
	std::string strBegin(str);
	sprintf(str, "%d", rangeEnd);
	std::string strEnd(str);
	_message = std::string("Bad value provided for \"") + argument + std::string("\" argument.");
	_message += std::string("\nValue must be in range from ") + strBegin
			+ std::string(" to ") + strEnd + std::string(".");

}


DEBadValue::DEBadValue(string argument, list<string> values)
{
	_message = string("Bad value provided for \"") + argument + string("\" argument.");
	_message += string("\nValid values for this argument are:");
	for (list<string>::iterator i = values.begin(); i != values.end(); i++)
		_message += string("\n\t") + (*i);
}

DEFileOpenFailed::DEFileOpenFailed(string fileName, bool read)
{
	_message = string("Failed to open file \"") + fileName + string("\" ");
	if (read)
		_message += string(" for reading.");
	else
		_message += string(" for writing.");
}

DEDevComandNotSupported::DEDevComandNotSupported(string command, string devSubtype, string memory)
{
	if (memory == "")
	{
		_message = string("Command \"") + command + string("\" is not supported by ")
				+ devSubtype + string(" device.");
	}
	else
	{
		_message = string("Command \"") + command + string("\" is not supported by ")
				+ devSubtype + string(" device for \"") + memory + string("\" memory type.");
	}
	_message += string("\nPlease use \"u2prog -h:") + devSubtype
			+ string("\" to see the list of commands supported by current device.");
}


DEBadMemoryType::DEBadMemoryType(string device, string memory)
{
	_message = string("Memory type \"") + memory + string("\" is not supported by \"")
			+ device + string("\" device.");
	_message += string("\nPlease use \"u2prog -h:") + device
			+ string("\" to see the list of memory types supported by current device.");
}

DEBadMemoryType::DEBadMemoryType(string device, string memory, string operation)
{
	_message = operation + string(" is not supported by \"")
			+ device + string("\" device for \"") + memory + string("\" memory type");
	_message += string("\nPlease use \"u2prog -h:") + device
			+ string("\" to see the list of memory types supported by current device.");
}

/* End of file */
