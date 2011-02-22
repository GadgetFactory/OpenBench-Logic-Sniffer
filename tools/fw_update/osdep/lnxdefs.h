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

#ifndef LNXDEFS_H_INCLUDED
#define LNXDEFS_H_INCLUDED

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h> 

#include <cstdio>
#include <iostream>
#include <cstring>

/* NOTE: Because fopen() do nothing with "t" mode on non-Windows we need to translate CRLF -> LF manually */
#define NEED_CRLF_TRANSLATION

typedef uint32_t XTEA_KEY_T;
typedef void * HANDLE;

typedef struct tm SYSTEMTIME;

static inline void GetLocalTime(SYSTEMTIME * t)
{
	time_t cur;

	time(&cur);
	t = localtime(&cur);
}

static inline void OutputDebugStringA(char *msg)
{
	std::cerr << msg;
}

#define INVALID_HANDLE_VALUE	( (HANDLE) ((-1L)) )

#define GENERIC_READ		1
#define GENERIC_WRITE		2

#ifdef USE_OPEN_MODE		/* May be defined by -D gcc option if required */
#define OPEN_EXISTING		1
#endif

#define FILE_SHARE_READ		1
#define FILE_SHARE_WRITE	2

#define OPEN_ALWAYS		0

#define FILE_ATTRIBUTE_NORMAL	0

#define FILE_BEGIN		SEEK_SET
#define FILE_CURRENT		SEEK_CUR
#define FILE_END		SEEK_END

static inline HANDLE CreateFileA(char *name, int flags, int, int, int crmode, int, int)
{
	FILE *f;
	char m[4] = {0, 0, 0, 0};

	switch (flags & 0x03) {
		case 0:
			break;
		case GENERIC_READ:
			m[0] = 'r';
			break;
		case GENERIC_WRITE:
			m[0] = 'w';
			break;
		default:
			m[0] = 'w';
			m[1] = '+';
			break;
	}

#ifdef USE_OPEN_MODE
	if (crmode == OPEN_EXISTING) {
		struct stat st;
		int retval;
		retval = stat(name, &st);
		if (retval < 0)
			return INVALID_HANDLE_VALUE;
	}
#endif

	f = fopen(name, m);

	if (f == NULL)
		return INVALID_HANDLE_VALUE;

	return f;
}

static inline bool SetFilePointer(HANDLE hFile, int posl, int, int whence)
{
	FILE *f = (FILE *)(hFile);

	if (fseek(f, posl, whence))
		return false;

	return true;
}

static inline bool WriteFile(HANDLE hFile, char *buf, unsigned long size, unsigned long * count, void *)
{
	FILE *f = (FILE *)(hFile);

	*count = fwrite(buf, 1, size, f);

	return !(ferror(f));
}

static inline void CloseHandle(HANDLE hFile)
{
	FILE *f = (FILE *)(hFile);
	fclose(f);
}

static inline void CopyMemory(void * dst, void * src, unsigned long size)
{
	memcpy(dst, src, size);
}
/*
static inline char * ultoa(unsigned long l, char * str, int base)
{
	char buff[65];		// length of MAX_ULLONG in base 2
	char *p = buff + 64;	// One char before end

	if ((base < 2) || (base > 36)) {
		*str = 0;
		return NULL;
	}

	*p = '0'; // If l == 0 simply write 0 to buff
	while (l != 0) {
		int c = l % base;

		if (c < 10)
			*p = '0' + c;
		else
			*p = c - 10 + 'a';

		--p;

		l /= base;
	}

	int len = 65 - (p - buff);
	memcpy(str, p, len);
	str[len] = 0;

	return str;
}

#define _ultoa(x, y, z)		ultoa(x, y, z)
*/

#define wDay	tm_mday
#define wMonth	tm_mon
#define wYear	tm_year
#define wHour	tm_hour
#define wMinute	tm_min
#define wSecond	tm_sec

#define _snprintf(a, b, c, args...)	snprintf(a, b, c, ## args)
#define _vsnprintf(a, b, c, d)		vsnprintf(a, b, c, d)

#define LOBYTE(x)	((unsigned char)((x) & 0xFF))
#define HIBYTE(x)	((unsigned char)((x >> 8) & 0xFF))
#define LOWORD(x)	((unsigned short)((x) & 0xFFFF))
#define HIWORD(x)	((unsigned short)((x >> 16) & 0xFFFF))

#endif //LNXDEFS_H_INCLUDED
