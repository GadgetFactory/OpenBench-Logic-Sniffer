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

#ifndef DEXCEPTION_H_INCLUDED
#define DEXCEPTION_H_INCLUDED

#include "../osdep/osdep.h"
#include <string>
#include <list>

using namespace std;

class DException
{
public:
	DException(){};
	virtual ~DException(){};
	string getErrMessage() { return _message;};
protected:
	string _message;
};

class DEBadValue: public DException
{
public:
	DEBadValue(string argument);
	DEBadValue(string argument, string rangeBegin, string rangeEnd);
	DEBadValue(string argument, unsigned int rangeBegin, unsigned int rangeEnd);
	DEBadValue(string argument, list<string> values);
};

class DEFileOpenFailed: public DException
{
public:
	DEFileOpenFailed(string fileName, bool read);
};

class DEFileCloseFailed: public DException
{
public:
	DEFileCloseFailed(string fileName)
	{_message = string("Failed to close file \"") + fileName + string("\".");}
};

class DEFileLargeImage: public DException
{
public:
	DEFileLargeImage()
	{_message = string("Image file is too large to fit in device memory.");}
};

class DEFileReadFailed: public DException
{
public:
	DEFileReadFailed()
	{_message = string("Failed to read image file.");}
};

class DEFileWriteFailed: public DException
{
public:
	DEFileWriteFailed()
	{_message = string("Failed to write image file.");}
};

/**
*	No data has been loaded from image.
*/
class DEFileNoDataLoad: public DException
{
public:
	DEFileNoDataLoad()
	{_message = string("No data has been loaded from the image provided.");}
};

class DEFileInvalidHexFormat: public DException
{
public:
	DEFileInvalidHexFormat()
	{_message = string("Invalid HEX file format.");}
};

class DEDevComandNotSupported: public DException
{
public:
	DEDevComandNotSupported(string command, string devSubtype, string memory="");
};


class DEVerificationFailed: public DException
{
public:
	DEVerificationFailed()
	{_message = "Verification failed.";}
};

class DEReadingFailed: public DException
{
public:
	DEReadingFailed()
	{_message = "Reading failed.";}
};

/**
*	USB universal device programmer is not found.
*/
class DEU2cAbsent: public DException
{
public:
	DEU2cAbsent()
	{_message = "U2C-11 device is not found.";}
};

class DEImageAbsent: public DException
{
public:
	DEImageAbsent()
	{_message = "Image file has to be provided for current operation.";}
};

class DEBadDeviceType: public DException
{
public:
	DEBadDeviceType(string devType)
	{
		_message = string("Device \"") + devType + string("\" is not supported.");
		_message += string("\nUse \"u2prog -h\" to see the list of supported devices.");
	}
};

class DEBadArgument: public DException
{
public:
	DEBadArgument(string argument)
	{	_message = string("Wrong argument - \"") + argument + string("\".");}
	DEBadArgument(string argument, string command)
	{
		_message = string("Argument \"") + argument +
			string("\" is not supported by \"") + command + string("\" command.");
	}
};

class DEArgumentMultipleUsage: public DException
{
public:
	DEArgumentMultipleUsage(string argument)
	{_message = string("Argument \"") + argument + string("\" provided more then once.");}
};

class DEArgumentAbsent: public DException
{
public:
	DEArgumentAbsent(string argument)
	{_message = string("Argument \"") + argument + string(" \" is required for the selected operation."); }
};

class DEValueAbsent: public DException
{
public:
	DEValueAbsent(string argument)
	{_message = string("Value not provided for \"") + argument + string("\".");}
};

class DEValueRedundant: public DException
{
public:
	DEValueRedundant(string argument)
	{_message = string("Argument \"") + argument + string("\" doesn't accept parameters.");}
};

class DEMultipleCommands: public DException
{
public:
	DEMultipleCommands()
	{_message = "Only one command allowed.";}
};

class DECommandAbsent: public DException
{
public:
	DECommandAbsent()
	{_message = "Command not provided.";}
};


class DETimeout: public DException
{
public:
	DETimeout()
	{_message = "Wait timeout.";}
};

class DEProgrammingModeEnterFailed: public DException
{
public:
	DEProgrammingModeEnterFailed()
	{_message = "Failed to enter programming mode.";}
};

class DEI2cSlaveAbsent: public DException
{
public:
	DEI2cSlaveAbsent()
	{_message = "I2C EEPROM is not found.";}
};


class DEDeviceAbsent: public DException
{
public:
	DEDeviceAbsent()
	{_message = "Device is not found.";}
};

class DEHidWriteFailed: public DException
{
public:
	DEHidWriteFailed()
	{_message = "Failed to send HID buffer.";}
};

class DEHidReadFailed: public DException
{
public:
	DEHidReadFailed()
	{_message = "Failed to read HID buffer.";}
};

class DEHidWriteTimeout: public DException
{
public:
	DEHidWriteTimeout()
	{_message = "HID buffer write timeout.";}
};

class DEHidReadTimeout: public DException
{
public:
	DEHidReadTimeout()
	{_message = "HID buffer read timeout.";}
};

class DEHidResponseIdMismatch: public DException
{
public:
	DEHidResponseIdMismatch()
	{_message = "HID response ID doesn't correspond to command ID.";}
};

class DEInternalError: public DException
{
public:
	DEInternalError()
	{_message = "Internal error occurred. Please contact Diolan Technical Support at support@diolan.com.";}
};

/**
*	Exception thrown if invalid device memory type is provided by user.
*/
class DEBadMemoryType: public DException
{
public:
	/**
	*	Memory type is not supported by device provided.
	*
	*   @param	device  Device type.
	*   @param	memory	Memory type.
	*/
	DEBadMemoryType(string device, string memory);

			/**
	*	Operation is not supported for provided device memory type.
	*
	*   @param	device		Device type.
	*   @param	memory		Memory type.
	*   @param	operation	Requested operation.
	*/
	DEBadMemoryType(string device, string memory, string operation);
};

#endif //DEXCEPTION_H_INCLUDED
