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
#if !defined( TRIANGLEAPIH )
#define TRIANGLEAPIH
#ifdef _WIN32
#pragma once
#endif

typedef enum 
{
	TRI_FRONT = 0,
	TRI_NONE = 1,
} TRICULLSTYLE;

#define TRI_API_VERSION		1

#define TRI_TRIANGLES		0
#define TRI_TRIANGLE_FAN	1
#define TRI_QUADS			2
#define TRI_POLYGON			3
#define TRI_LINES			4	
#define TRI_TRIANGLE_STRIP	5
#define TRI_QUAD_STRIP		6

typedef struct triangleapi_s
{
	int			version;

	void		( *RenderMode )( int mode );
	void		( *Begin )( int primitiveCode );
	void		( *End ) ( void );

	void		( *Color4f ) ( float r, float g, float b, float a );
	void		( *Color4ub ) ( unsigned char r, unsigned char g, unsigned char b, unsigned char a );
	void		( *TexCoord2f ) ( float u, float v );
	void		( *Vertex3fv ) ( float *worldPnt );
	void		( *Vertex3f ) ( float x, float y, float z );
	void		( *Brightness ) ( float brightness );
	void		( *CullFace ) ( TRICULLSTYLE style );
	int			( *SpriteTexture ) ( struct model_s *pSpriteModel, int frame );
	int			( *WorldToScreen ) ( float *world, float *screen );  // Returns 1 if it's z clipped
	void		( *Fog ) ( float flFogColor[3], float flStart, float flEnd, int bOn ); //Works just like GL_FOG, flFogColor is r/g/b.
	void		( *ScreenToWorld ) ( float *screen, float *world  ); 

} triangleapi_t;

#endif // !TRIANGLEAPIH
