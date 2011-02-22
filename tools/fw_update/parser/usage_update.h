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

#ifndef USAGE_UPDATE_H_INCLUDED
#define USAGE_UPDATE_H_INCLUDED

#include "../osdep/osdep.h"

#include "usage_descriptor.h"

class UsageUpdate
{
public:



	/**
	*	Initializes usage descriptor object.
	*/
	UsageUpdate(
		unsigned int id,	/**< Numerical ID for current argument. */
		UsageDescriptor::ValueRelevance valueRelevance, /**< The relevance of the value for current argument. */
		std::string valueAlias, /**< Alias for value to be displayed in usage. */
		std::string description /**< Argument description to be displayed in usage. */
		);


	/**
	*	Checks whether current usage descriptor has the numerical ID equal to provided one.
	*
	*   @param	id	Numerical ID to check with.
	*   @return
	*		\arg true if id corresponds to current usage descriptor.
	*		\arg false if id doesn't corresponds to current usage descriptor.
	*/
	bool operator==(const unsigned int &id);

	operator unsigned int() const
	{ return _id;};
	std::string valueAlias() const {return _valueAlias;};
	std::string description() const {return _description;};
	UsageDescriptor::ValueRelevance valueRelevance() const {return _valueRelevance;};
private:
	unsigned int _id;	/**< Numerical ID for current argument. */
	UsageDescriptor::ValueRelevance _valueRelevance;	/**< The relevance of the value for current argument. */
	std::string _valueAlias;		/**< Alias for value to be displayed in usage. */
	std::string _description;	/**< Argument description to be displayed in usage. */
};


#endif // USAGE_UPDATE_H_INCLUDED
