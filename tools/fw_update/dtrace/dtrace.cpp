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
#include "dtrace.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define TRACE_MESSAGE_SIZE 1024

void AddTraceType(char* pMessage, const ETraceType TraceType)
{
	char TraceMessage[TRACE_MESSAGE_SIZE];
	switch(TraceType)
	{
	case ttError:
		strncpy(TraceMessage, "!!! Err: ", TRACE_MESSAGE_SIZE);
//		assert(false);
		break;
	case ttDebug:
		break;
	default:
		assert(false);
		break;
	}
	strncat(TraceMessage, pMessage, TRACE_MESSAGE_SIZE);
	strncpy(pMessage, TraceMessage, TRACE_MESSAGE_SIZE);
}

void AddFileLine(char* pMessage, const char* FileName, const int LineNumber)
{
	char TraceMessage[TRACE_MESSAGE_SIZE];
	_snprintf(TraceMessage, TRACE_MESSAGE_SIZE, "  File: %s, Line: %d",
		FileName, LineNumber);
	strncat(pMessage, TraceMessage, TRACE_MESSAGE_SIZE);
}

void AddDateTime(char* pMessage)
{
	SYSTEMTIME t;
	GetLocalTime(&t);
	char TraceMessage[TRACE_MESSAGE_SIZE];

	_snprintf(TraceMessage, TRACE_MESSAGE_SIZE, "[%02d/%02d/%04d %02d:%02d:%02d] ",
		  t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute, t.wSecond);
	strncat(TraceMessage, pMessage, TRACE_MESSAGE_SIZE);
	strncpy(pMessage, TraceMessage, TRACE_MESSAGE_SIZE);
}

void DebuggerTrace(const ETraceType TraceType, const char* FileName,
			 const int LineNumber, char* TraceMessage)
{
	AddTraceType(TraceMessage, TraceType);
	AddFileLine(TraceMessage, FileName, LineNumber);
	strncat(TraceMessage, "\n", TRACE_MESSAGE_SIZE);

	OutputDebugStringA(TraceMessage);
}

void FileTrace(const ETraceType TraceType, const char* FileName,
				   const int LineNumber, char* TraceMessage)
{
	AddTraceType(TraceMessage, TraceType);
	AddFileLine(TraceMessage, FileName, LineNumber);
	AddDateTime(TraceMessage);
	strncat(TraceMessage, "\n", TRACE_MESSAGE_SIZE);

	HANDLE hFile = CreateFileA("log.txt", GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
		return;
	SetFilePointer(hFile, 0, 0, FILE_END);
	unsigned long Written;
	WriteFile(hFile, TraceMessage, (unsigned long)(strlen(TraceMessage)), &Written, NULL);
	CloseHandle(hFile);
}

void OutputTrace(const ETraceType TraceType, const ETraceOutput TraceOutput,
				 const char* FileName, const int LineNumber, char* TraceMessage)
{
	switch(TraceOutput)
	{
	case toDebugger:
		DebuggerTrace(TraceType, FileName, LineNumber, TraceMessage);
		break;
	case toFile:
		FileTrace(TraceType, FileName, LineNumber, TraceMessage);
		break;
	default:
		assert(false);
		break;
	}

}

void imp_Trace(const ETraceType TraceType, const ETraceOutput TraceOutput,
			   const char* FileName, const int LineNumber, const char* Str, ...)
{
	if (TraceOutput == toNowhere)
		return;

	char TraceMessage[TRACE_MESSAGE_SIZE];
	va_list params;
	va_start(params, Str);
	_vsnprintf(TraceMessage, TRACE_MESSAGE_SIZE, Str, params);
	va_end(params);

	OutputTrace(TraceType, TraceOutput, FileName, LineNumber, TraceMessage);
}


void imp_eAssert(const char* TraceString, const ETraceType TraceType,
				 const ETraceOutput TraceOutput, const char* FileName,
				 const int LineNumber)
{
	char TraceMessage[TRACE_MESSAGE_SIZE];
	strncpy(TraceMessage, "Assertion: ", TRACE_MESSAGE_SIZE);
	strncat(TraceMessage, TraceString, TRACE_MESSAGE_SIZE);
	OutputTrace(TraceType, TraceOutput, FileName, LineNumber, TraceMessage);
}
