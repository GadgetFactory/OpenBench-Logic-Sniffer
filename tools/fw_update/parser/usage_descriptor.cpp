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

#include <assert.h>
#include "usage_descriptor.h"
#include "string_compare.h"

using namespace std;



UsageDescriptor::UsageDescriptor( unsigned int id,  string shortArgument,
								 string verboseArgument,  ValueRelevance valueRelevance,
								 string valueAlias,  string description,  Type type, std::string defaultValue)
:	_id(id), _shortArgument(shortArgument), _verboseArgument(verboseArgument),
	_valueRelevance(valueRelevance), _valueAlias(valueAlias),
	_description(description), _type(type), _defaultValue(defaultValue)
{
	assert((shortArgument.size() != 0) || (verboseArgument.size()!=0));
	assert((valueRelevance == absent) || (valueAlias.size() != 0));
	assert((valueRelevance != absent) || (valueAlias.size() == 0));
	assert(description.size() != 0);
}

bool UsageDescriptor::operator==(const string& argument) const
{
	if (argument.size() == 0)
		return false;
	if (isEqual(argument, _shortArgument))
		return true;
	if (isEqual(argument, _verboseArgument))
		return true;
	return false;
}

bool UsageDescriptor::operator==(const unsigned int &id) const
{
	return (id == _id);
}

string UsageDescriptor::argument() const
{
	if (_verboseArgument.size() != 0)
		return _verboseArgument;
	else
		return _shortArgument;
}

/* End of file */
