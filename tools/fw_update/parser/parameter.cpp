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
#include <stdlib.h>
#include "parameter.h"
#include "string_compare.h"
#include "parser_exceptions.h"

using namespace std;

Parameter::Parameter(unsigned int id, string argument, string value)
: _id(id), _argument(argument), _value(value)
{
}

bool Parameter::hasValue() const
{
	return (_value.size() != 0);
}


string Parameter::value() const
{
	if (_value.size() == 0)
		throw EValueAbsent(_argument);
	return _value;
}

string Parameter::argument() const
{
	return _argument;
}

unsigned long Parameter::ulongValue() const
{
	string str = value();
	unsigned int val;
	char* endptr;
	errno = 0;
	val = strtoul(str.c_str(), &endptr, 0);
	if ((*endptr) || (errno))
		throw EBadValue(_argument, 0, 0xFFFFFFFF);
	return val;
}

bool Parameter::boolValue() const
{
	string str = value();
	if (isEqual(str, "yes") || isEqual(str, "y") || isEqual(str, "true") || isEqual(str, "enable"))
		return true;
	if (isEqual(str, "no") || isEqual(str, "n") || isEqual(str, "false") || isEqual(str, "disable"))
		return false;
	throw EBadValue(_argument);
}


unsigned char Parameter::ucharValue() const
{
	try
	{
		unsigned long val = ulongValue();
		if (val > 0xFF)
			throw EBadValue(_argument, 0, 0xFF);
		return (unsigned char)val;
	}
	catch(EBadValue)
	{
		throw EBadValue(_argument, 0, 0xFF);
	}
}

unsigned short Parameter::ushortValue() const
{
	try
	{
		unsigned long val = ulongValue();
		if (val > 0xFFFF)
			throw EBadValue(_argument, 0, 0xFFFF);
		return (unsigned short)val;
	}
	catch(EBadValue)
	{
		throw EBadValue(_argument, 0, 0xFFFF);
	}
}


void Parameter::range(unsigned int min, unsigned int max) const
{
	try
	{
		unsigned long val = ulongValue();
		if ((val < min) || (val > max))
			throw EBadValue(_argument, min, max);
	}
	catch(EBadValue)
	{
		throw EBadValue(_argument, min, max);
	}
}

/* End of file */
