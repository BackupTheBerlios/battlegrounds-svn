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

//This file was created on the 15/6/03 at 15:48 by Ben Banfield for the Battle Grounds modification for Half-Life
//Its Purpose is to define an entity which will reset other entities to their default state

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "animation.h"

class CInfoReset : public CPointEntity
{
public:
	void Spawn( void ) { SetUse( &CInfoReset::ResetUse ); }
	void ResetEnt( void );
	void KeyValue( KeyValueData *pKeyValueData );
	void EXPORT ResetUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) { ResetEnt(); }

	char szTarget[256];
};

// -stores the entity we want
void CInfoReset::KeyValue( KeyValueData *pKeyValueData )
{
	if(FStrEq(pKeyValueData->szKeyName, "target_ent"))
	{
		sprintf(szTarget, "%s", pKeyValueData->szValue);
	}
}

// -resets the entity we want
void CInfoReset::ResetEnt()
{
	CBaseEntity *pTrigger = NULL;
	pTrigger = (CBaseEntity*)UTIL_FindEntityByTargetname( pTrigger, szTarget );

	if (pTrigger)
	{ 
		ALERT(at_console, "Found info_reset. Firing: %s\n", szTarget);
		pTrigger->ResetEnt();
//		pTrigger->Use( this, this, USE_TOGGLE, 0.0 );
		pTrigger->SUB_UseTargets(pTrigger, USE_TOGGLE, 0.0);
		pTrigger = (CBaseEntity*)UTIL_FindEntityByTargetname( pTrigger, szTarget );
	}
	else
	{
		ALERT(at_console, "Found info_reset. Could not fire: %s (does not exist)\n", szTarget);

	}
}

LINK_ENTITY_TO_CLASS(info_reset, CInfoReset);
