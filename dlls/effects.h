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
*	Copyright (c) 1999, 2000 Valve LLC. All rights reserved.
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

#ifndef EFFECTS_H
#define EFFECTS_H

#define SF_BEAM_STARTON			0x0001
#define SF_BEAM_TOGGLE			0x0002
#define SF_BEAM_RANDOM			0x0004
#define SF_BEAM_RING			0x0008
#define SF_BEAM_SPARKSTART		0x0010
#define SF_BEAM_SPARKEND		0x0020
#define SF_BEAM_DECALS			0x0040
#define SF_BEAM_SHADEIN			0x0080
#define SF_BEAM_SHADEOUT		0x0100
#define SF_BEAM_TEMPORARY		0x8000

#define SF_SPRITE_STARTON		0x0001
#define SF_SPRITE_ONCE			0x0002
#define SF_SPRITE_TEMPORARY		0x8000

class CSprite : public CPointEntity
{
public:
	void Spawn( void );
	void Precache( void );

	int		ObjectCaps( void )
	{ 
		int flags = 0;
		if ( pev->spawnflags & SF_SPRITE_TEMPORARY )
			flags = FCAP_DONT_SAVE;
		return (CBaseEntity :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | flags; 
	}
	void EXPORT AnimateThink( void );
	void EXPORT ExpandThink( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void Animate( float frames );
	void Expand( float scaleSpeed, float fadeSpeed );
	void SpriteInit( const char *pSpriteName, const Vector &origin );

	inline void SetAttachment( edict_t *pEntity, int attachment )
	{
		if ( pEntity )
		{
			pev->skin = ENTINDEX(pEntity);
			pev->body = attachment;
			pev->aiment = pEntity;
			pev->movetype = MOVETYPE_FOLLOW;
		}
	}
	void TurnOff( void );
	void TurnOn( void );
	inline float Frames( void ) { return m_maxFrame; }
	inline void SetTransparency( int rendermode, int r, int g, int b, int a, int fx )
	{
		pev->rendermode = rendermode;
		pev->rendercolor.x = (float)r;
		pev->rendercolor.y = (float)g;
		pev->rendercolor.z = (float)b;
		pev->renderamt = (float)a;
		pev->renderfx = fx;
	}
	inline void SetTexture( int spriteIndex ) { pev->modelindex = spriteIndex; }
	inline void SetScale( float scale ) { pev->scale = scale; }
	inline void SetColor( int r, int g, int b ) { pev->rendercolor.x = (float)r; pev->rendercolor.y = (float)g; pev->rendercolor.z = (float)b; }
	inline void SetBrightness( int brightness ) { pev->renderamt = (float)brightness; }

	inline void AnimateAndDie( float framerate ) 
	{ 
		SetThink(&CSprite::AnimateUntilDead); 
		pev->framerate = framerate;
		pev->dmgtime = gpGlobals->time + (m_maxFrame / framerate); 
		pev->nextthink = gpGlobals->time; 
	}

	void EXPORT AnimateUntilDead( void );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];
	static CSprite *SpriteCreate( const char *pSpriteName, const Vector &origin, bool animate );

private:

	float		m_lastTime;
	float		m_maxFrame;
};


class CBeam : public CBaseEntity
{
public:
	void	Spawn( void );
	void	Precache( void );
	int		ObjectCaps( void )
	{ 
		int flags = 0;
		if ( pev->spawnflags & SF_BEAM_TEMPORARY )
			flags = FCAP_DONT_SAVE;
		return (CBaseEntity :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | flags; 
	}

	void EXPORT TriggerTouch( CBaseEntity *pOther );

	// These functions are here to show the way beams are encoded as entities.
	// Encoding beams as entities simplifies their management in the client/server architecture
	inline void	SetType( int type ) { pev->rendermode = (pev->rendermode & 0xF0) | (type&0x0F); }
	inline void	SetFlags( int flags ) { pev->rendermode = (pev->rendermode & 0x0F) | (flags&0xF0); }
	inline void SetStartPos( const Vector& pos ) { pev->origin = pos; }
	inline void SetEndPos( const Vector& pos ) { pev->angles = pos; }
	void SetStartEntity( int entityIndex );
	void SetEndEntity( int entityIndex );

	inline void SetStartAttachment( int attachment ) { pev->sequence = (pev->sequence & 0x0FFF) | ((attachment&0xF)<<12); }
	inline void SetEndAttachment( int attachment ) { pev->skin = (pev->skin & 0x0FFF) | ((attachment&0xF)<<12); }

	inline void SetTexture( int spriteIndex ) { pev->modelindex = spriteIndex; }
	inline void SetWidth( int width ) { pev->scale = (float)width; }
	inline void SetNoise( int amplitude ) { pev->body = amplitude; }
	inline void SetColor( int r, int g, int b ) { pev->rendercolor.x = (float)r; pev->rendercolor.y = (float)g; pev->rendercolor.z = (float)b; }
	inline void SetBrightness( int brightness ) { pev->renderamt = (float)brightness; }
	inline void SetFrame( float frame ) { pev->frame = frame; }
	inline void SetScrollRate( int speed ) { pev->animtime = (float)speed; }

	inline int	GetType( void ) { return pev->rendermode & 0x0F; }
	inline int	GetFlags( void ) { return pev->rendermode & 0xF0; }
	inline int	GetStartEntity( void ) { return pev->sequence & 0xFFF; }
	inline int	GetEndEntity( void ) { return pev->skin & 0xFFF; }

	const Vector &GetStartPos( void );
	const Vector &GetEndPos( void );

	Vector Center( void ) { return (GetStartPos() + GetEndPos()) * 0.5; }; // center point of beam

	inline int  GetTexture( void ) { return pev->modelindex; }
	inline int  GetWidth( void ) { return (int)pev->scale; }
	inline int  GetNoise( void ) { return pev->body; }
	// inline void GetColor( int r, int g, int b ) { pev->rendercolor.x = r; pev->rendercolor.y = g; pev->rendercolor.z = b; }
	inline int  GetBrightness( void ) { return (int)pev->renderamt; }
	inline int  GetFrame( void ) { return (int)pev->frame; }
	inline int  GetScrollRate( void ) { return (int)pev->animtime; }

	// Call after you change start/end positions
	void		RelinkBeam( void );
//	void		SetObjectCollisionBox( void );

	void		DoSparks( const Vector &start, const Vector &end );
	CBaseEntity *RandomTargetname( const char *szName );
	void		BeamDamage( TraceResult *ptr );
	// Init after BeamCreate()
	void		BeamInit( const char *pSpriteName, int width );
	void		PointsInit( const Vector &start, const Vector &end );
	void		PointEntInit( const Vector &start, int endIndex );
	void		EntsInit( int startIndex, int endIndex );
	void		HoseInit( const Vector &start, const Vector &direction );

	static CBeam *BeamCreate( const char *pSpriteName, int width );

	inline void LiveForTime( float time ) { SetThink(&CBeam::SUB_Remove); pev->nextthink = gpGlobals->time + time; }
	inline void	BeamDamageInstant( TraceResult *ptr, float damage ) 
	{ 
		pev->dmg = damage; 
		pev->dmgtime = gpGlobals->time - 1;
		BeamDamage(ptr); 
	}
};


#define SF_MESSAGE_ONCE			0x0001		// Fade in, not out
#define SF_MESSAGE_ALL			0x0002		// Send to all clients


class CLaser : public CBeam
{
public:
	void	Spawn( void );
	void	Precache( void );
	void	KeyValue( KeyValueData *pkvd );

	void	TurnOn( void );
	void	TurnOff( void );

	bool	IsOn( void );

	void	FireAtPoint( TraceResult &point );

	void	EXPORT StrikeThink( void );
	void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	CSprite	*m_pSprite;
	int		m_iszSpriteName;
	Vector  m_firePosition;
};
#define SF_FOG_ACTIVE 1
#define SF_FOG_FADING 0x8000

class CEnvFog : public CBaseEntity
{
public:
	void Spawn( void );
	void Precache( void );
	void EXPORT ResumeThink( void );
	void EXPORT Resume2Think( void );
	void EXPORT TurnOn( void );
	void EXPORT TurnOff( void );
	void EXPORT FadeInDone( void );
	void EXPORT FadeOutDone( void );
	void KeyValue( KeyValueData *pkvd );
	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	int m_iStartDist;
	int m_iEndDist;
	int m_iFadeIn;
	int m_iFadeOut;
	float m_fHoldTime;
	float m_fFadeStart; // if we're fading in/out, then when did the fade start?
};

#endif		//EFFECTS_H
