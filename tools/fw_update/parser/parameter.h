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

#ifndef PARAMETER_H_INCLUDED
#define PARAMETER_H_INCLUDED

#include "../osdep/osdep.h"

#include <string>

class Parameter
{
public:
	Parameter(unsigned int id, std::string argument, std::string value);
	operator unsigned int() const
	{ return _id;};
	unsigned long ulongValue() const;
	unsigned char ucharValue() const;
	unsigned short ushortValue() const;
	bool boolValue() const;
	bool hasValue() const;

	/**
	*	Checks whether current parameter has the numerical ID equal to provided one.
	*
	*   @param	id	Numerical ID to check with.
	*   @return
	*		\arg true if id corresponds to current parameter.
	*		\arg false if id doesn't corresponds to current parameter.
	*/
	bool operator==(const unsigned int &id)
	{ return (_id == id);};


	/**
	*	Returns string value provided for current parameter.
	*
	*	@return String value provided for current parameter.
	*   @throws	DEValueAbsent	Value doesn't provided for parameter.
	*/
	std::string value() const;


	/**
	*	Checks whether parameter value is numerical and fits into range provided.
	*
	*   @param	min Minimum acceptable value.
	*   @param	max Maximum acceptable value.
	*   @throws	DEValueAbsent	Value doesn't provided for parameter.
	*	@throws DEBadValue	Value doesn't fill into range.
	*/
	void range(unsigned int min, unsigned int max) const;

	std::string argument() const;

	unsigned int id() const {return _id;};
private:
	unsigned int _id;
	std::string _value;
	std::string _argument;
};



#endif // PARAMETER_H_INCLUDED

