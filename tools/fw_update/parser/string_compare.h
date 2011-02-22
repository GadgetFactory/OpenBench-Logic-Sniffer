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

#ifndef STRING_COMPARE_H_INCLUDED
#define STRING_COMPARE_H_INCLUDED

#include "../osdep/osdep.h"

// compare two strings disregarding the case
inline int str_cmp(const std::string& s1, const std::string& s2)
{
	std::string::const_iterator p1 = s1.begin();
	std::string::const_iterator p2 = s2.begin();
	for(p1 = s1.begin(), p2 = s2.begin(); p1!=s1.end()&&p2!=s2.end(); p1++,p2++)
		if (toupper(*p1)!=toupper(*p2))
			return (toupper(*p1) < toupper(*p2))?-1:1;
	return (s1.size()==s2.size())?0:(s1.size()<s2.size())?-1:1;
}
inline bool isEqual(const std::string& s1, const std::string& s2)
{
	return str_cmp(s1, s2)==0;
}



#endif // STRING_COMPARE_H_INCLUDED
