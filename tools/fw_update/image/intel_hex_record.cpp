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

#include "intel_hex_record.h"
#include <stdio.h>
#include <stdlib.h>
#include "../exception/exception.h"
#include "../dtrace/dtrace.h"

bool CIntelHexRec::InitFromString(string str)
{
	string strVal;
	char *pEnds;
	unsigned long uVal;
	int i;
	string::size_type strLen;
	string::size_type start, end;
	start = str.find_first_not_of(' ');
	end = str.find_last_not_of(' ');
	str = str.substr(start, end);
	if ((str.size() < 10) || (str[0] != ':'))
		return false;
	errno = 0;
	// extract size
	strVal = str.substr(1, 2);
	uVal = strtoul(strVal.c_str(), &pEnds, 16);
	if ((*pEnds) || errno || (uVal > 255))
		return false;
	m_Size = (unsigned char)uVal;
	strLen = m_Size * 2 + 11;
	if (str.size() != strLen)
		return false;
	// extract address
	strVal = str.substr(3, 4);
	uVal = strtoul(strVal.c_str(), &pEnds, 16);
	if ((*pEnds) || errno || (uVal > 0xFFFF))
		return false;
	m_Addr = (unsigned short)uVal;
	// extract type
	strVal = str.substr(7, 2);
	uVal = strtoul(strVal.c_str(), &pEnds, 16);
	if ((*pEnds) || errno || (uVal > 4))
		return false;
	m_Type = (unsigned char)uVal;
	// extract data
	for (i = 0; i < m_Size; i++)
	{
		strVal = str.substr(9+i*2, 2);
		uVal = strtoul(strVal.c_str(), &pEnds, 16);
		if ((*pEnds) || errno || (uVal > 0xFF))
			return false;
		m_Data[i] = (unsigned char)uVal;
	}
	// compare crc
	strVal = str.substr(9+m_Size*2, 2);
	uVal = strtoul(strVal.c_str(), &pEnds, 16);
	if ((*pEnds) || errno || (uVal > 0xFF))
		return false;
	if (CalcCrc()!=(unsigned char)uVal)
		return false;
	return true;
}

unsigned char CIntelHexRec::CalcCrc()
{
	unsigned long Res = 0;
	Res += m_Size;
	Res += HIBYTE(m_Addr);
	Res += LOBYTE(m_Addr);
	Res += m_Type;
	for (int i = 0; i < m_Size; i++)
		Res += m_Data[i];
	Res = 0x100 - (Res & 0xFF);
	return (unsigned char)Res;
}

string CIntelHexRec::GetString()
{
	string pc = "";
	const size_t buffer_size = 20;
	char strTmp[buffer_size];
	sprintf(strTmp, ":%02X%04X%02X", m_Size, m_Addr, m_Type);
	pc += strTmp;
	for (int i = 0; i < m_Size; i++)
	{
		sprintf(strTmp, "%02X", m_Data[i]);
		pc+=strTmp;
	}
	sprintf(strTmp, "%02X\n", CalcCrc());

	pc += strTmp;
	return pc;
}

void CIntelHexRec::InitData(unsigned char Size, unsigned short Addr, unsigned char* pData)
{
	eAssert(pData);
	m_Type = cHexTypeData;
	m_Size = Size;
	m_Addr = Addr;
	CopyMemory(m_Data, pData, Size);
}

void CIntelHexRec::InitEndOfData()
{
	m_Type = cHexTypeEndOfData;
	m_Size = 0;
	m_Addr = 0;
}

void CIntelHexRec::InitExtAddr(unsigned short ExtAddr)
{
	m_Type = cHexTypeExtLinearAddr;
	m_Size = 2;
	m_Addr = 0;
	m_Data[0] = HIBYTE(ExtAddr);
	m_Data[1] = LOBYTE(ExtAddr);
}

unsigned long CIntelHexRec::GetExtAddr()
{
	eAssert(m_Size == 2);
	unsigned long pc = 0;
	switch(m_Type)
	{
	case cHexTypeExtLinearAddr:
		pc |= m_Data[0] << 24;
		pc |= m_Data[1] << 16;
		return pc;
	case cHexTypeExtSegmentAddr:
		pc |= m_Data[0] << 12;
		pc |= m_Data[1] << 4;
		return pc;
	default:
		eTrace1("m_Type = %d", m_Type);
		throw DEFileInvalidHexFormat();
	}
}
