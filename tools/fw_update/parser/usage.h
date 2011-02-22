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

#ifndef USAGE_H_INCLUDED
#define USAGE_H_INCLUDED

#include "../osdep/osdep.h"

#include "usage_descriptor.h"
#include "usage_update.h"

#include <list>

class Usage
{
public:


	Usage();
	virtual ~Usage();
	void add(const UsageDescriptor& descriptor);

	/**
	*	Returns reference to the descriptor with corresponding numerical ID.
	*
	*   @param	id Numeric ID to search corresponding descriptor.
	*   @return	Reference to the descriptor with corresponding numerical ID.
	*   @throws	invalid_argument Provided ID doesn't correspond to any descriptor.
	*/
	const UsageDescriptor& operator[](const unsigned int &id) const;

	/**
	*	Returns reference to the descriptor corresponding to argument provided.
	*
	*   @param	argument Argument to search corresponding descriptor.
	*   @return	Reference to the descriptor corresponding to argument provided.
	*   @throws	EBadArgument Provided argument doesn't correspond to any descriptor.
	*/
	const UsageDescriptor& operator[](const std::string &argument) const;
	void print();
	void print(const std::list<UsageUpdate> &update);
protected:
	void print(const std::list<UsageUpdate> &update, UsageDescriptor::Type type);

	/**
	*	Checks whether usage contains descriptor with corresponding numerical ID.
	*	If descriptor is found returns true.
	*
	*   @param	id Numeric ID to search corresponding descriptor.
	*   @return
	*		\arg true if descriptor is found.
	*		\arg false if descriptor is not found
	*/
	bool contain(unsigned int id);

	/**
	*	Checks whether usage contains descriptor corresponding for argument provided.
	*	If descriptor is found returns true.
	*
	*   @param	argument Argument to search corresponding descriptor.
	*   @return
	*		\arg true if descriptor is found.
	*		\arg false if descriptor is not found
	*/
	bool contain(std::string argument);
	std::list<UsageDescriptor> _descriptors;
};


#endif // USAGE_H_INCLUDED
