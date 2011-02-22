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

#ifndef USAGE_DESCRIPTOR_H_INCLUDED
#define USAGE_DESCRIPTOR_H_INCLUDED

#include "../osdep/osdep.h"

#include <string>

/**
*	Complete argument descriptor.
*/
class UsageDescriptor
{
public:
	enum ValueRelevance {absent, possible, required};
	enum Type {command, option};

	/**
	*	Initializes usage descriptor object.
	*/
	UsageDescriptor(
		unsigned int id,	/**< Numerical ID for current argument. */
		std::string shortArgument,	/**< Argument text in short format. */
		std::string verboseArgument,	/**< Argument text in verbose (long) format. */
		ValueRelevance valueRelevance, /**< The relevance of the value for current argument. */
		std::string valueAlias, /**< Alias for value to be displayed in usage. */
		std::string description, /**< Argument description to be displayed in usage. */
		Type type, /**< Argument type (command, option). */
		std::string defaultValue = ""
		);

	/**
	*	Checks whether the \a argument provided corresponds to short or verbose form of
	*	the argument from current usage descriptor.
	*
	*   @param	argument Argument to compare internal argument with.
	*	@return
	*		\arg true if argument corresponds to current usage descriptor.
	*		\arg false if argument doesn't correspond to current usage descriptor.
	*/
	bool operator==(const std::string &argument) const;

	/**
	*	Checks whether current usage descriptor has the numerical ID equal to provided one.
	*
	*   @param	id	Numerical ID to check with.
	*   @return
	*		\arg true if id corresponds to current usage descriptor.
	*		\arg false if id doesn't corresponds to current usage descriptor.
	*/
	bool operator==(const unsigned int &id) const;

	/**
	*	Returns argument in verbose form if present or in short form otherwise.
	*
	*   @return	Argument in verbose form if present or in short form otherwise.
	*/
	std::string argument() const;

	unsigned int id() const {return _id;};
	std::string shortArgument() const {return _shortArgument;};
	std::string verboseArgument() const {return _verboseArgument;};
	Type type() const {return _type;};
	ValueRelevance valueRelevance() const {return _valueRelevance;};
	std::string valueAlias() const { return _valueAlias;};
	std::string description() const { return _description;};
	std::string defaultValue() const { return _defaultValue;};
private:
	unsigned int _id;	/**< Numerical ID for current argument. */
	std::string _shortArgument;	/**< Argument text in short format. */
	std::string _verboseArgument;	/**< Argument text in verbose (long) format. */
	ValueRelevance _valueRelevance;	/**< The relevance of the value for current argument. */
	std::string _valueAlias;		/**< Alias for value to be displayed in usage. */
	std::string _description;	/**< Argument description to be displayed in usage. */
	Type _type;		/**< Argument type (command, option). */
	std::string _defaultValue;
};



#endif // USAGE_DESCRIPTOR_H_INCLUDED
