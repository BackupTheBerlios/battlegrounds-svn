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

/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#ifndef CVARDEF_H
#define CVARDEF_H

#define	FCVAR_ARCHIVE		(1<<0)	// set to cause it to be saved to vars.rc
#define	FCVAR_USERINFO		(1<<1)	// changes the client's info string
#define	FCVAR_SERVER		(1<<2)	// notifies players when changed
#define FCVAR_EXTDLL		(1<<3)	// defined by external DLL
#define FCVAR_CLIENTDLL     (1<<4)  // defined by the client dll
#define FCVAR_PROTECTED     (1<<5)  // It's a server cvar, but we don't send the data since it's a password, etc.  Sends 1 if it's not bland/zero, 0 otherwise as value
#define FCVAR_SPONLY        (1<<6)  // This cvar cannot be changed by clients connected to a multiplayer server.
#define FCVAR_PRINTABLEONLY (1<<7)  // This cvar's string cannot contain unprintable characters ( e.g., used for player name etc ).
#define FCVAR_UNLOGGED		(1<<8)  // If this is a FCVAR_SERVER, don't log changes to the log file / console if we are creating a log

typedef struct cvar_s
{
	char	*name;
	char	*string;
	int		flags;
	float	value;
	struct cvar_s *next;
} cvar_t;
#endif
