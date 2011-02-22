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

#ifndef PARSER_EXCEPTIONS_H_INCLUDED
#define PARSER_EXCEPTIONS_H_INCLUDED

#include "../osdep/osdep.h"

#include <stdexcept>
#include <string>
#include <list>
#include <cstdio>
#include <errno.h>

class EParserException: public std::runtime_error
{
public:
	explicit EParserException (const std::string& msg)
		: std::runtime_error(""), _message(msg) {};

	virtual ~EParserException() throw() {;};

	virtual const char *what() const throw()
	{	// return pointer to message string
		return (_message.c_str());
	}

protected:
	std::string _message;
};

class EArgumentAbsent: public EParserException
{
public:
	explicit EArgumentAbsent(std::string argument)
		: EParserException(std::string("Argument \"") + argument + std::string("\" is required for the selected operation.")){};
};

class EArgumentMultipleUsage: public EParserException
{
public:
	explicit EArgumentMultipleUsage(std::string argument)
		: EParserException(std::string("Argument \"") + argument + std::string("\" is provided more then once.")){};
};

class EBadArgument: public EParserException
{
public:
	explicit EBadArgument(std::string argument)
		: EParserException(std::string("Wrong argument - \"")  + argument + std::string("\".")){};
};

class EValueAbsent: public EParserException
{
public:
	explicit EValueAbsent(std::string argument)
		: EParserException(std::string("Value not provided for \"") + argument + std::string("\".")){};
};


class EBadValue: public EParserException
{
public:
	explicit EBadValue(std::string argument)
		: EParserException(std::string("Bad value provided for \"") + argument + std::string("\" argument.")){};
	EBadValue(std::string argument, std::string rangeBegin, std::string rangeEnd)
		: EParserException(std::string("Bad value provided for \"") + argument + std::string("\" argument.")
			+ std::string("\nValue must be in range from ") + rangeBegin
			+ std::string(" to ") + rangeEnd + std::string(".")){};
	EBadValue(std::string argument, unsigned int rangeBegin, unsigned int rangeEnd)
		: EParserException("")
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
	EBadValue(std::string argument, std::list<std::string> values)
		: EParserException("")
	{
		_message = std::string("Bad value provided for \"") + argument + std::string("\" argument.");
		_message += std::string("\nValid values for this argument are:");
		for (std::list<std::string>::iterator i = values.begin(); i != values.end(); i++)
			_message += std::string("\n\t") + (*i);
	}
};




class EMultipleCommands: public EParserException
{
public:
	EMultipleCommands()
		: EParserException("Only one command allowed."){};
};


class ECommandAbsent: public EParserException
{
public:
	ECommandAbsent()
		: EParserException("Command not provided."){};
};



#endif // PARSER_EXCEPTIONS_H_INCLUDED

