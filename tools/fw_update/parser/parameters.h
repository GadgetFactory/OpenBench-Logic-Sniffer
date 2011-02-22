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

#ifndef PARAMETERS_H_INCLUDED
#define PARAMETERS_H_INCLUDED

#include "../osdep/osdep.h"

#include "parameter.h"
#include "usage.h"

class Parameters
{
public:

	Parameters(const Usage &usage);
	virtual ~Parameters();


	/**
	*	Checks whether parameters list contains parameter corresponds to provided numerical ID.
	*	If descriptor is found returns true.
	*	In contrast to real_contain() function it also returns true if parameters list
	*	doesn't contain the parameter but it has default value.
	*
	*   @param	id Numeric ID to search corresponding parameter.
	*   @return
	*		\arg true if parameter is found.
	*		\arg false if parameter is not found
	*/
	bool contain(unsigned int id) const;

	/**
	*	Returns reference to the parameter with corresponding numerical ID.
	*
	*   @param	id Numeric ID to search corresponding parameter.
	*   @return	Reference to the parameter with corresponding numerical ID.
	*   @throws	DEArgumentAbsent Provided ID doesn't correspond to any parameter.
	*/
	Parameter operator[](const unsigned int &id) const;
//	const Parameter& operator[](const unsigned int &id) const;

	/**
	*
	*
	*   @param
	*   @param
	*   @return
	*   @throws DEArgumentMultipleUsage
	*/
	void add(std::string argument, std::string value);

	Parameter command();


protected:
	/**
	*	Checks whether parameters list contains parameter corresponds to provided numerical ID.
	*	In contrast to contain() function doesn't check default value of the parameter.
	*	If descriptor is found returns true.
	*
	*   @param	id Numeric ID to search corresponding parameter.
	*   @return
	*		\arg true if parameter is found.
	*		\arg false if parameter is not found
	*/
	bool real_contain(unsigned int id) const;

	std::list<Parameter> _params;
	Usage _usage;
};


#endif // PARAMETERS_H_INCLUDED
