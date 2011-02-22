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

#ifndef U2IO_FW_UPDATE_H_INCLUDED
#define U2IO_FW_UPDATE_H_INCLUDED

#include "osdep/osdep.h"

const unsigned short vid = 0x1994;
const unsigned short pid = 0x1001;

#include "dtrace/dtrace.h"

// stl includes
#include <string>
#include <algorithm>
#include <list>
#include <iostream>
using namespace std;

#define OP_PROGRAMMING "programming"
#define OP_READING "reading"
#define OP_VERIFYING "verifying"
#define OP_ERASING "erasing"
#define OP_FILLING "filling"


const unsigned int OPERATION_DONE = 200;
const unsigned int OPERATION_FAILED = 201;
/**
*	Displays operation progress.
*
*   @param	operation	Executed operation.
*   @param	device		Device type.
*	@param	memory		Memory type.
*			If \a memory contains empty string the operation is performed on entire device
*			or device default memory type.
*	@param	progress	Operation progress.
*			Values from 0 to 100 indicates that operation is still executed.\n
*			Two special values are also available:\n
*			\arg OPERATION_DONE - when the operation is successfully finished.
*			\arg OPERATION_FAILED - when the operation is failed during execution.
*/
inline void show_progress(string operation, string device, string memory, unsigned int progress)
{
	cout << "\r" << device << " ";

	if (memory != "")
		cout << memory << " ";
	cout << operation << ": ";
	switch(progress)
	{
	case OPERATION_DONE:
		cout << "DONE.\n";
		break;
	case OPERATION_FAILED:
		cout << "FAILED.\n";
		break;
	default:
		eAssert(progress <= 100);
		cout << progress << "%";
	}
}

#endif // U2IO_FW_UPDATE_H_INCLUDED
