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

#include "parameters.h"
#include "parser_exceptions.h"

using namespace std;



Parameters::Parameters(const Usage &usage)
: _usage(usage)
{
}

Parameters::~Parameters()
{
}

bool Parameters::real_contain(unsigned int id) const
{
	list<Parameter>::const_iterator i;
	for (i = _params.begin(); i != _params.end(); i++)
	{
		if (i->id() == id)
			return true;
	}
	return false;
}


bool Parameters::contain(unsigned int id) const
{
	if (real_contain(id) || (_usage[id].defaultValue().size() != 0))
		return true;
	return false;
}

Parameter Parameters::operator[](const unsigned int &id) const
{
	list<Parameter>::const_iterator i;
	for (i = _params.begin(); i != _params.end(); i++)
	{
		if (i->id() == id)
			return (*i);
	}
	if (_usage[id].defaultValue().size() != 0)
		return Parameter(id, _usage[id].verboseArgument(), _usage[id].defaultValue());
	throw EArgumentAbsent(_usage[id].argument());
}

void Parameters::add(string argument, string value)
{
	unsigned int id = _usage[argument].id();
	if (real_contain(id))
		throw EArgumentMultipleUsage(argument);
	if (value.size()==0)
		value = _usage[argument].defaultValue();
	Parameter param(id, argument, value);
	_params.push_back(param);
}


Parameter Parameters::command()
{
	bool found = false;
	unsigned int cmd;
	list<Parameter>::iterator pc;
	for (pc = _params.begin(); pc != _params.end(); pc++)
	{
		if (_usage[pc->id()].type() != UsageDescriptor::command)
			continue;
		if (found)
			throw EMultipleCommands();
		found = true;
		cmd = pc->id();
	}
	if (!found)
		throw ECommandAbsent();
	return (*this)[cmd];
}

/* End of file */
