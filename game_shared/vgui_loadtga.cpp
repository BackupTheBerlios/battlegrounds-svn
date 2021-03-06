/*
    Copyright 2001 to 2004. The Battle Grounds Team and Contributors

    This file is part of the Battle Grounds Modification for Half-Life.

    The Battle Grounds Modification for Half-Life is free software;
    you can redistribute it and/or modify it under the terms of the
    GNU Lesser General Public License as published by the Free
    Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    The Battle Grounds Modification for Half-Life is distributed in
    the hope that it will be useful, but WITHOUT ANY WARRANTY; without
    even the implied warranty of MERCHANTABILITY or FITNESS FOR A
    PARTICULAR PURPOSE.  See the GNU Lesser General Public License
    for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with The Battle Grounds Modification for Half-Life;
    if not, write to the Free Software Foundation, Inc., 59 Temple Place,
    Suite 330, Boston, MA  02111-1307  USA

    You must obey the GNU General Public License in all respects for
    all of the code used other than code  distributed with the Half-Life
    SDK developed by Valve.  If you modify this file, you may extend this
    exception to your version of the file, but you are not obligated to do so.
    If you do not wish to do so, delete this exception statement from your
    version.
*/

//========= Copyright � 1996-2001, Valve LLC, All rights reserved. ============

#include "../cl_dll/wrect.h"
#include "../cl_dll/cl_dll.h"
#include "vgui.h"
#include "vgui_loadtga.h"
#include "vgui_inputstream.h"


// ---------------------------------------------------------------------- //
// Helper class for loading tga files.
// ---------------------------------------------------------------------- //
class MemoryInputStream : public vgui::InputStream
{
public:
				MemoryInputStream()
				{
					m_pData = NULL;
					m_DataLen = m_ReadPos = 0;
				}
		
	virtual void  seekStart(bool& success)					{m_ReadPos=0; success=true;}
	virtual void  seekRelative(int count,bool& success)		{m_ReadPos+=count; success=true;}
	virtual void  seekEnd(bool& success)					{m_ReadPos=m_DataLen; success=true;}
	virtual int   getAvailable(bool& success)				{success=false; return 0;} // This is what vgui does for files...
	
	virtual uchar readUChar(bool& success)					
	{
		if(m_ReadPos>=0 && m_ReadPos<m_DataLen)
		{
			success=true;
			uchar ret = m_pData[m_ReadPos];
			++m_ReadPos;
			return ret;
		}
		else
		{
			success=false;
			return 0;
		}
	}

	virtual void  readUChar(uchar* buf,int count,bool& success)
	{
		for(int i=0; i < count; i++)
			buf[i] = readUChar(success);
	}

	virtual void  close(bool& success) 
	{
		m_pData = NULL;
		m_DataLen = m_ReadPos = 0;
	}

	uchar		*m_pData;
	int			m_DataLen;
	int			m_ReadPos;
};

vgui::BitmapTGA* vgui_LoadTGA(char const *pFilename)
{
	MemoryInputStream stream;
	
	stream.m_pData = gEngfuncs.COM_LoadFile((char*)pFilename, 5, &stream.m_DataLen);
	if(!stream.m_pData)
		return NULL;
	
	stream.m_ReadPos = 0;
	vgui::BitmapTGA *pRet = new vgui::BitmapTGA(&stream, true);
	gEngfuncs.COM_FreeFile(stream.m_pData);
	
	return pRet;
}

vgui::BitmapTGA* vgui_LoadTGANoInvertAlpha(char const *pFilename)
{
	MemoryInputStream stream;
	
	stream.m_pData = gEngfuncs.COM_LoadFile((char*)pFilename, 5, &stream.m_DataLen);
	if(!stream.m_pData)
		return NULL;
	
	stream.m_ReadPos = 0;
	vgui::BitmapTGA *pRet = new vgui::BitmapTGA(&stream, false);
	gEngfuncs.COM_FreeFile(stream.m_pData);
	
	return pRet;
}
