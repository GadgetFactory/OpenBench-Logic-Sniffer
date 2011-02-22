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

#ifndef DTRACE_H_INCLUDED
#define DTRACE_H_INCLUDED

#include "../osdep/osdep.h"

#include <stdio.h>

enum ETraceType
{
	ttError,
	ttDebug,
};

enum ETraceOutput
{
	toNowhere,
	toDebugger,
	toFile,
};

static const ETraceOutput eTraceOut = toFile;
static const ETraceOutput dTraceOut = toFile;

void imp_Trace(const ETraceType TraceType, const ETraceOutput TraceOutput,
			   const char* FileName, const int LineNumber, const char* Str, ...);
void imp_eAssert(const char* TraceString, const ETraceType TraceType,
				 const ETraceOutput TraceOutput, const char* FileName,
				 const int LineNumber);

#define eTrace0(mes) imp_Trace(ttError, eTraceOut, __FILE__, __LINE__, mes)
#define eTrace1(mes, par1) imp_Trace(ttError, eTraceOut, __FILE__, __LINE__, mes, par1)
#define eTrace2(mes, par1, par2) imp_Trace(ttError, eTraceOut, __FILE__, __LINE__, mes, par1, par2)
#define eTrace3(mes, par1, par2, par3) imp_Trace(ttError, eTraceOut, __FILE__, __LINE__, mes, par1, par2, par3)
#define eTrace4(mes, par1, par2, par3, par4) imp_Trace(ttError, eTraceOut, __FILE__, __LINE__, mes, par1, par2, par3, par4)

#define dTrace0(mes) imp_Trace(ttDebug, dTraceOut, __FILE__, __LINE__, mes)
#define dTrace1(mes, par1) imp_Trace(ttDebug, dTraceOut, __FILE__, __LINE__, mes, par1)
#define dTrace2(mes, par1, par2) imp_Trace(ttDebug, dTraceOut, __FILE__, __LINE__, mes, par1, par2)
#define dTrace3(mes, par1, par2, par3) imp_Trace(ttDebug, dTraceOut, __FILE__, __LINE__, mes, par1, par2, par3)
#define dTrace4(mes, par1, par2, par3, par4) imp_Trace(ttDebug, dTraceOut, __FILE__, __LINE__, mes, par1, par2, par3, par4)

#define eAssert(exp) (void)((exp)||(imp_eAssert(#exp, ttError, eTraceOut, __FILE__, __LINE__), 0) )

#endif /* DTRACE_H_INCLUDED */
