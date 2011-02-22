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

#ifndef FU_USAGE_H_INCLUDED
#define FU_USAGE_H_INCLUDED

#include "osdep/osdep.h"

#include "parser/usage.h"

/* Commands */

const unsigned int ARG_WRITE = 1;
const unsigned int ARG_READ = 2;
const unsigned int ARG_VERIFY = 3;
const unsigned int ARG_ERASE = 4;
const unsigned int ARG_VERSION = 5;
const unsigned int ARG_HELP = 6;

/* Options */

const unsigned int ARG_MEM_TYPE = 7;
const unsigned int ARG_VID = 8;
const unsigned int ARG_PID = 9;
const unsigned int ARG_RESET = 10;
const unsigned int ARG_IMAGE_DATA = 11;
const unsigned int ARG_DATA_SIZE = 12;
const unsigned int ARG_ADDR = 13;
const unsigned int ARG_INPUT_HEX = 14;
const unsigned int ARG_INPUT_BIN = 15;
const unsigned int ARG_OUTPUT_HEX = 16;
const unsigned int ARG_OUTPUT_BIN = 17;

class FUUsage: public Usage
{
public:
	FUUsage();
};

#endif // FU_USAGE_H_INCLUDED
