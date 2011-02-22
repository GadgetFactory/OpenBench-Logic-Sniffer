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

#include "usage.h"
#include <assert.h>
#include <algorithm>
#include <string>
#include <iostream>
#include "parser_exceptions.h"
using namespace std;



Usage::Usage()
{
}

Usage::~Usage()
{
}

void Usage::add(const UsageDescriptor& descriptor)
{
	assert(!contain(descriptor.id()));
	assert(!contain(descriptor.shortArgument()));
	assert(!contain(descriptor.verboseArgument()));
	_descriptors.push_back(descriptor);
}

const UsageDescriptor& Usage::operator[](const unsigned int &id) const
{
	list<UsageDescriptor>::const_iterator pc;
	pc = std::find(_descriptors.begin(), _descriptors.end(), id);
	assert(pc != _descriptors.end());
	if (pc == _descriptors.end())
		throw invalid_argument("Invalid descriptor ID");
	return (*pc);
}

const UsageDescriptor& Usage::operator[](const string &argument) const
{
	list<UsageDescriptor>::const_iterator pc;
	pc = std::find(_descriptors.begin(), _descriptors.end(), argument);
	if (pc == _descriptors.end())
		throw EBadArgument(argument);
	return (*pc);
}


bool Usage::contain(unsigned int id)
{
	if (_descriptors.end() == std::find(_descriptors.begin(), _descriptors.end(), id))
		return false;
	else
		return true;
}

bool Usage::contain(string argument)
{
	if (_descriptors.end() == std::find(_descriptors.begin(), _descriptors.end(), argument))
		return false;
	else
		return true;
}

void Usage::print()
{
	list<UsageUpdate> all;
	list<UsageDescriptor>::iterator i;
	for (i = _descriptors.begin(); i != _descriptors.end(); i++)
		all.push_back(UsageUpdate(i->id(), i->valueRelevance(), i->valueAlias(), i->description()));
	print(all);
}

void Usage::print(const list<UsageUpdate> &update)
{
	cout << "COMMANDS:\n";
	print(update, UsageDescriptor::command);
	cout << "OPTIONS:\n";
	print(update, UsageDescriptor::option);
}

void Usage::print(const list<UsageUpdate> &update, UsageDescriptor::Type type)
{
	list<UsageDescriptor>::iterator pc;
	for (pc = _descriptors.begin(); pc != _descriptors.end(); pc++)
	{
		if (pc->type() == type)
		{
			list<UsageUpdate>::const_iterator i;
			i = find(update.begin(), update.end(), pc->id());
			if (i == update.end())
				continue;
			string valueAlias, description;
			if (i->valueAlias().size() != 0)
				valueAlias = i->valueAlias();
			else
				valueAlias = pc->valueAlias();
			if (i->description().size() != 0)
				description = i->description();
			else
				description = pc->description();
			if (pc->shortArgument().size() != 0)
				cout << "-" << pc->shortArgument() << " ";
			if (pc->verboseArgument().size() != 0)
				cout << "-" << pc->verboseArgument() << " ";
			switch (i->valueRelevance())
			{
			case UsageDescriptor::absent:
				break;
			case UsageDescriptor::possible:
				cout << " [" << valueAlias << "]";
				break;
			case UsageDescriptor::required:
				cout << " <" << valueAlias << ">";
				break;
			}
			cout << "\n\t" << description << endl;
			if(pc->defaultValue().size() != 0)
				cout << "\tDefault: " << pc->defaultValue() << endl;
		}
	}
}

/* End of file */
