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

#include "parser.h"
#include "parser_exceptions.h"

using namespace std;

Parser::Parser(const Usage &usage)
: _usage(usage)
{
}

Parser::~Parser()
{
}

void Parser::parse(int argc, char *argv[], Parameters* params)
{
	int i;
	size_t j;
	string argument, value;
	for (i = 1; i < argc; i++)
	{
		argument.erase();
		value.erase();
		if (argv[i][0] != '-')
			throw EBadArgument(argv[i]);
		// Search argument in usage descriptors
		argument = &argv[i][1];
		if (argument.find_first_of('=') != string::npos)
			argument.erase(argument.find_first_of('='));
		j = argument.size() + 1; // If there is value in form -x=VALUE it will start immediately after string
		if ((argv[i][j] != 0) && (argv[i][j] != '='))
			throw EBadArgument(argv[i]);

		// Determine option value
		if (argv[i][j] == '=')
			value = &argv[i][j+1]; // Value in =VALUE format
		else if ((i+1 < argc) && (argv[i+1][0] != '-'))
		{	// Take next argument as value
			value = argv[i+1];
			i++;
		}
		params->add(argument, value);
	}
}

/* End of file */
