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

// definition of the mapped particle
#include "..\hud.h"
#include "..\cl_util.h"
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include "pm_shared.h"
#include "pmtrace.h"
#include "pm_defs.h"
#include "particle_header.h"

// creates the mapped particle
CMappedParticle::CMappedParticle(mapped_particle_system *pSystem, particle_texture_s *pParticleTexture) : CParticle()
{
	pSys = pSystem;
	sParticle.pTexture = pParticleTexture;
	InitValues();
}

// loads the values the mapper set
void CMappedParticle::InitValues( void )
{
	sParticle.vPosition = pSys->vPostion;
	sParticle.vDirection = pSys->vDirection;
	sParticle.vVelocity = pSys->vStartingVel;

	sParticle.iRed = 255;
	sParticle.iBlue = sParticle.iGreen = sParticle.iRed;
	sParticle.iTransparency = pSys->iTransparency;

	sParticle.bUseTrans = true;

	sParticle.flRotation = gEngfuncs.pfnRandomFloat( -pSys->flParticleRotationSpeed, pSys->flParticleRotationSpeed);
	sParticle.flCurrentRotation = 0.0;

	sParticle.flAge = 0.0f;
	sParticle.flMaxAge = pSys->flParticleLife + gEngfuncs.pfnRandomFloat( -pSys->flParticleLifeVariation, pSys->flParticleLifeVariation );

	sParticle.flSize = pSys->flParticleScaleSize;
	sParticle.flGrowth = pSys->flScaleIncreasePerSecond;
	sParticle.flScale = 30.0f;

	if(pSys->iSystemShape == SHAPE_PLANE) {
		sParticle.vPosition.x = sParticle.vPosition.x + gEngfuncs.pfnRandomFloat( -(float)(pSys->iPlaneXLength/2), (float)(pSys->iPlaneXLength/2));
		sParticle.vPosition.y = sParticle.vPosition.y + gEngfuncs.pfnRandomFloat( -(float)(pSys->iPlaneYLength/2), (float)(pSys->iPlaneYLength/2));
	}

	if(pSys->bWindy == true) {
		sParticle.vWind.x = gEngfuncs.pfnRandomFloat( 0.01f,0.74f );
		sParticle.vWind.y = gEngfuncs.pfnRandomFloat( 0.01f,0.81f );
		sParticle.vWind.z = gEngfuncs.pfnRandomFloat( 0.01f,0.64f );
	}

	if(pSys->bSnowRain == true) {
		cl_entity_t* m_pPlayer = gEngfuncs.GetLocalPlayer();
		sParticle.vPosition.x = (m_pPlayer->origin.x + gEngfuncs.pfnRandomFloat( -500.0,500.0 ));
		sParticle.vPosition.y = (m_pPlayer->origin.y + gEngfuncs.pfnRandomFloat( -500.0,500.0 ));
		sParticle.vPosition.z = (m_pPlayer->origin.z + gEngfuncs.pfnRandomFloat( 100.0, 150.0 ));
	}

	sParticle.vVelocity.x = gEngfuncs.pfnRandomFloat((sParticle.vVelocity.x - pSys->vVelocityVar.x), (sParticle.vVelocity.x + pSys->vVelocityVar.x));
	sParticle.vVelocity.y = gEngfuncs.pfnRandomFloat((sParticle.vVelocity.y - pSys->vVelocityVar.y), (sParticle.vVelocity.y + pSys->vVelocityVar.y));
	sParticle.vVelocity.z = gEngfuncs.pfnRandomFloat((sParticle.vVelocity.z - pSys->vVelocityVar.z), (sParticle.vVelocity.z + pSys->vVelocityVar.z));

	flParticleTime = 0.0;
	bWindChanged = false;
	bIgnoreParticle = false;

	for(int i = 0; i < 6; i++) {
		bCollisionChecked[i] = false;
		iCollisionTimer[i] = 0;
	}

	sParticle.flLastSort = 0.0;
	sParticle.flSquareDistanceToPlayer = 0.0;

	// check lightlevel once when the particle is spawned
	if (pSys->iParticleLightCheck == CHECK_ONCE)
	{
		// great thanks go to Sneaky_Bastard and randomnine for helping me alot to finding 
		// this great method of obtaining lightlevel on the clientside
		
		// create temporary entity to get illumination from it
		alight_t lighting;
		cl_entity_t *LightEntity;
		vec3_t dir;

		// bogus data as valve calls it :)
		LightEntity = gEngfuncs.GetLocalPlayer();

		if ( !LightEntity )
			return;

		// move it in the particles location
		LightEntity->origin = sParticle.vPosition;
		
		// I have no idea what this does but if you don't do it -> crash
		lighting.plightvec = dir;

		IEngineStudio.StudioDynamicLight(LightEntity, &lighting );
		IEngineStudio.StudioSetupLighting (&lighting);
		IEngineStudio.StudioEntityLight( &lighting );
		
		sParticle.iRed = (int)(lighting.color[0] * lighting.shadelight);
		sParticle.iGreen = (int)(lighting.color[1] * lighting.shadelight);
		sParticle.iBlue = (int)(lighting.color[2] * lighting.shadelight);
	}

}

// draws a mapped particle
void CMappedParticle::Draw( void )
{
	if(bIgnoreParticle)
		return;

	if(sParticle.pTexture == NULL)
		return;

	int iHealth = 0;

	// bp document you're equations!! 
	// i can get the basic jist of whats happening here but its hard,
	// due to your lack of commenting
	// add to that magic numbers
	if (pSys->bFadeIn && ((sParticle.flAge / sParticle.flMaxAge) < 0.5)) {
		iHealth = (sParticle.flAge * sParticle.iTransparency / (sParticle.flMaxAge / 2));
		if (iHealth < 0)
			return;
	} else if (pSys->bFadeOut && ((sParticle.flAge / sParticle.flMaxAge) > 0.5)) {
		iHealth = sParticle.iTransparency - ((sParticle.flAge - (sParticle.flMaxAge / 2)) * sParticle.iTransparency / (sParticle.flMaxAge / 2));
		if (iHealth < 0)
			return;
	} else {
		if (sParticle.iTransparency > 255)
			sParticle.iTransparency = 255;
		if (sParticle.iTransparency < 0)
			sParticle.iTransparency = 0;
		iHealth = sParticle.iTransparency;
	}

	// fade out particles that are closer to the player if smoke is true
	if (pSys->bSmoke) {
		float flThresholdStartSqrd = (PARTICLE_THRESHOLD_START * PARTICLE_THRESHOLD_START);
		float flThresholdEndSqrd = (PARTICLE_THRESHOLD_END * PARTICLE_THRESHOLD_END);

		float flDistance = sParticle.flSquareDistanceToPlayer;
		if ((flDistance > flThresholdStartSqrd) && (flDistance < flThresholdEndSqrd)) {
			float flTransparencyFactor = ((flDistance - flThresholdStartSqrd) / (flThresholdEndSqrd - flThresholdStartSqrd));
			if (flTransparencyFactor > 1)
				flTransparencyFactor = 1;
			if (flTransparencyFactor <0)
				flTransparencyFactor = 0;
			iHealth *= flTransparencyFactor;
		}
	}

	vec3_t vNormal,vForward, vRight, vUp, vPoint, vPosition;

	// We again copy part->origin into another vector to prevent us accidentally messing with it
	VectorCopy( sParticle.vPosition, vPosition );

	// We then get the view angles for the player so that we can "billboard" the sprites 
	gEngfuncs.GetViewAngles((float*)vNormal);

	if(pSys->bPlanar == true) {
		vNormal.x = 90;
		vNormal.y = 0;
		vNormal.z = 0;
	}

	vNormal.z += sParticle.flCurrentRotation;
	AngleVectors(vNormal, vForward, vRight, vUp);

	float flInverse = 0.0;
	unsigned int iNumFrames = pSys->iFramesPerTexture;
	
	if (iNumFrames)
		flInverse = 1 / sqrt((float)iNumFrames);
	if ((int)sqrt((float)iNumFrames) != (int)abs(sqrt((float)iNumFrames)))
		return;

	if (iNumFrames == 0)
		iNumFrames = 1;

	float co01, co02, co03, co04;
	co01 = flInverse * (int)((iCurrentFrame) % ((int)sqrt((float)iNumFrames)));
	co02 = 1 - flInverse * (int)((iCurrentFrame) / sqrt((float)iNumFrames));
	co03 = 1 - flInverse * (int)((iCurrentFrame) / sqrt((float)iNumFrames) + 1);
	co04 = flInverse * (int)( (iCurrentFrame) % ((int)sqrt((float)iNumFrames)) + 1);

	if (pSys->iDisplayMode == TWENTY_FOUR_BIT_ADDITIVE)
		glColor3ub(iHealth, iHealth, iHealth);
	else
		glColor4ub(sParticle.iRed, sParticle.iGreen, sParticle.iBlue,iHealth); 

	// Finally, we draw the particle
	glBindTexture(GL_TEXTURE_2D, sParticle.pTexture->iID);
	glBegin(GL_TRIANGLE_FAN);

	glTexCoord2f(co01, co02);
	VectorMA ( vPosition, (sParticle.flSize * sParticle.flScale), vUp, vPoint);
	VectorMA ( vPoint, (-sParticle.flSize * sParticle.flScale), vRight, vPoint);
	glVertex3fv(vPoint);

   	glTexCoord2f(co04, co02);
	VectorMA ( vPosition, (sParticle.flSize * sParticle.flScale), vUp, vPoint);
	VectorMA ( vPoint, (sParticle.flSize * sParticle.flScale), vRight, vPoint);
	glVertex3fv(vPoint);

	glTexCoord2f(co04, co03); 
	VectorMA ( vPosition, (-sParticle.flSize * sParticle.flScale), vUp, vPoint);
	VectorMA ( vPoint, (sParticle.flSize * sParticle.flScale), vRight, vPoint);
	glVertex3fv(vPoint); 

	glTexCoord2f(co01, co03);
	VectorMA ( vPosition, (-sParticle.flSize * sParticle.flScale), vUp, vPoint);
	VectorMA ( vPoint, (-sParticle.flSize * sParticle.flScale), vRight, vPoint);
	glVertex3fv(vPoint);    

	glEnd();

	if (pSys->iDisplayMode == TWENTY_FOUR_BIT_ADDITIVE) {
		glDisable(GL_COLOR_MATERIAL);
	}
}

// updates the internals of a mapped particle
void CMappedParticle::Update( float flTimeSinceLastDraw )
{
	flParticleTime += flTimeSinceLastDraw;

	// update distance between particle and player g_ParticleSorts times a second
	float flTimeSinceLastSort = (gEngfuncs.GetClientTime() - sParticle.flLastSort);
	if(flTimeSinceLastSort == 0 || (((int)(1 / flTimeSinceLastSort)) < g_ParticleSorts->value))
	{
		// update lightlevel everytime we sort
		if (pSys->iParticleLightCheck == CHECK_EVERY_SORT)
		{
			// great thanks go to Sneaky_Bastard and randomnine for helping me alot to finding 
			// this great method of obtaining lightlevel on the clientside
			
			// create temporary entity to get illumination from it
			alight_t lighting;
			cl_entity_t *LightEntity;
			vec3_t dir;

			// bogus data as valve calls it :)
			LightEntity = gEngfuncs.GetLocalPlayer();

			if ( !LightEntity )
				return;

			// move it in the particles location
			LightEntity->origin = sParticle.vPosition;
			
			// I have no idea what this does but if you don't do it -> crash
			lighting.plightvec = dir;

			IEngineStudio.StudioDynamicLight(LightEntity, &lighting );
			IEngineStudio.StudioSetupLighting (&lighting);
			IEngineStudio.StudioEntityLight( &lighting );

			sParticle.iRed = (int)(lighting.color[0] * lighting.shadelight );
			sParticle.iGreen = (int)(lighting.color[1] * lighting.shadelight );
			sParticle.iBlue = (int)(lighting.color[2] * lighting.shadelight );
		}
	}

	// dont draw the particle if smoke is enabled and its close to the player
	if (pSys->bSmoke == true)
	{
		if (sParticle.flSquareDistanceToPlayer < (PARTICLE_THRESHOLD_START*PARTICLE_THRESHOLD_START))
			bIgnoreParticle = true;
		else
			bIgnoreParticle = false;
	}
	
	// if we're using a custom mode for displaying our texture
	if (pSys->iFPS != 0) {
		if (pSys->iAnimSpeed == CUSTOM) {
			if (pSys->iAnimBehaviour == LOOP) {
				while (flParticleTime > (1.0f / pSys->iFPS)) {
					iCurrentFrame++;
					flParticleTime -= (1.0f / pSys->iFPS);
				}

				if (iCurrentFrame > (pSys->iEndingFrame - 1))
					iCurrentFrame = (pSys->iStartingFrame - 1);

			} else if (pSys->iAnimBehaviour == REVERSE_LOOP) {
				bool bCountUp = true;

				if (iCurrentFrame >= (pSys->iEndingFrame - 1))
					bCountUp = false;
				if (iCurrentFrame <= (pSys->iStartingFrame - 1))
					bCountUp = true;

				while (flParticleTime > (1.0f / pSys->iFPS))
				{
					if (bCountUp == true)
						iCurrentFrame++;
					else
						iCurrentFrame--;

					flParticleTime -= (1.0f / pSys->iFPS);
				}
			} else { // once_through
				while (flParticleTime > (1.0f / pSys->iFPS))
				{
					iCurrentFrame++;
					flParticleTime -= (1.0f / pSys->iFPS);
				}
				if (iCurrentFrame > (pSys->iEndingFrame - 1))
					iCurrentFrame = (pSys->iEndingFrame - 1);
			}
		}
	}


	if (pSys->iAnimSpeed == START_FAST_END_SLOW)
		iCurrentFrame = pSys->iEndingFrame * (1 - exp(-3*sParticle.flAge));

	if (pSys->iAnimSpeed == ANIMATE_OVER_LIFE && sParticle.flMaxAge)
		iCurrentFrame = (pSys->iEndingFrame) * (sParticle.flAge / sParticle.flMaxAge);
	
	if (flTimeSinceLastDraw < 0) // how the hell can the time between updates be less than nothing?
		flTimeSinceLastDraw = -flTimeSinceLastDraw;

	if (pSys->flVelocityDampening > 0.01)
	{
	sParticle.vVelocity.x *= ( 1 / ( 1 + fabsf(pSys->flVelocityDampening * flTimeSinceLastDraw * sParticle.vVelocity.x)));
	sParticle.vVelocity.y *= ( 1 / ( 1 + fabsf(pSys->flVelocityDampening * flTimeSinceLastDraw * sParticle.vVelocity.y)));
	sParticle.vVelocity.z *= ( 1 / ( 1 + fabsf(pSys->flVelocityDampening * flTimeSinceLastDraw * sParticle.vVelocity.z)));
	}
	
	sParticle.vVelocity.z -= pSys->flGravity * flTimeSinceLastDraw;

	if (pSys->bWindy) {
		sParticle.vVelocity.x += (sParticle.vWind.x * flTimeSinceLastDraw);
		sParticle.vVelocity.y += (sParticle.vWind.y * flTimeSinceLastDraw);
		sParticle.vVelocity.z += (sParticle.vWind.z * flTimeSinceLastDraw);
	}
	
	VectorMA( sParticle.vPosition, (60.0 * flTimeSinceLastDraw), sParticle.vVelocity, sParticle.vPosition );
	
	sParticle.flSize += flTimeSinceLastDraw * sParticle.flGrowth;
	sParticle.flCurrentRotation += flTimeSinceLastDraw * sParticle.flRotation;
	while (sParticle.flCurrentRotation > 360) {
		sParticle.flCurrentRotation -= 360;
	}
 	
	sParticle.flAge += flTimeSinceLastDraw;
}

// tests whether the particle has collided or whether it needs to die
bool CMappedParticle::Test( void )
{
	if( !pSys )
		return false;

	for (int i = 5; i > -1; i--)
	{
		if (bCollisionChecked[i])
		{
			iCollisionTimer[i]--;
			if (iCollisionTimer[i] < 1)
				bCollisionChecked[i] = false;
		}
	}

	if (pSys->iParticleCollision != PARTICLE_PASS_THROUGH)
	{
		// Now we create four test vectors and copy origin into each one
		vec3_t vTest[6];
		for( i = 0; i < 6; i++ )
			VectorCopy( sParticle.vPosition, vTest[i] );

		// We then set the test vectors
		vTest[0].z -= 5; // down
		vTest[1].x += 5;  // x+
		vTest[2].x -= 5; // x-
		vTest[3].y += 5; // y+
		vTest[4].y -= 5; // y-
		vTest[5].z += 5; // up


		bool bCollX = false;
		bool bCollY = false;
		bool bCollZ = false;

		
		// This long test basically checks whether any of the test vectors are in solid ground 
		// It also checks if the particle is in the sky, in which case we do NOT want to do these solid tests
				
		if(gEngfuncs.PM_PointContents( vTest[0], NULL ) == CONTENTS_SOLID ||
		gEngfuncs.PM_PointContents( vTest[5], NULL ) == CONTENTS_SOLID)
			bCollY = true;
		
		if(gEngfuncs.PM_PointContents( vTest[1], NULL ) == CONTENTS_SOLID ||
		gEngfuncs.PM_PointContents( vTest[2], NULL ) == CONTENTS_SOLID)
			bCollX = true;
		
		if(gEngfuncs.PM_PointContents( vTest[3], NULL ) == CONTENTS_SOLID ||
		gEngfuncs.PM_PointContents( vTest[4], NULL ) == CONTENTS_SOLID)
			bCollZ = true;

		if ((bCollX == true || bCollY == true || bCollZ == true) && (pSys->iParticleCollision == PARTICLE_STUCK)) {
			
			sParticle.vWind.z = 0;
			pSys->flGravity = 0;
			if (pSys->bWindy == true && (bWindChanged == false))
			{
				sParticle.vVelocity.x = gEngfuncs.pfnRandomFloat( (0.01*(cos(10*sParticle.vPosition.y)+1)), (0.3*(sin(10*sParticle.vPosition.x)+1))); 
				sParticle.vVelocity.y = gEngfuncs.pfnRandomFloat( (0.01*(cos(10*sParticle.vPosition.y)+1)), (0.3*(cos(10*sParticle.vPosition.y)+1))); 
				sParticle.vVelocity.z = gEngfuncs.pfnRandomFloat( 0.01f, (0.3*cos(10*sParticle.vPosition.y)+1));
				sParticle.flRotation = gEngfuncs.pfnRandomFloat( -100, 100);
				bWindChanged = true;
			} else if (pSys->bWindy == false) {
				sParticle.vVelocity = Vector(0, 0, 0); 
				sParticle.flRotation = 0.0;			
			}
		}
		
		if ((bCollX == true || bCollY == true || bCollZ == true) && (pSys->iParticleCollision == PARTICLE_DIE))
			return false;

		if(pSys->iParticleCollision == PARTICLE_BOUNCE) {
			if (bCollZ == true) {
				if (bCollisionChecked[0] != true) { //  bounce up/down particle
					sParticle.vVelocity.z *= -0.3;
					bCollisionChecked[0] = true;
					iCollisionTimer[0] = 5;
				}
			}			
			if (bCollX) {
				if (bCollisionChecked[1] != true) { //  bounce x particle
					sParticle.vVelocity.x *= -0.3;
					bCollisionChecked[1] = true;
					iCollisionTimer[1] = 5;
				}
			}
		
			if (bCollY) {
				if (bCollisionChecked[2] != true) { //  bounce y particle
					sParticle.vVelocity.y *= -0.3;
					bCollisionChecked[2] = true;
					iCollisionTimer[2] = 5;
				}
			}
		}
	}
	return sParticle.flAge < sParticle.flMaxAge;
}

// prepares the system for drawing
void CMappedParticle::Prepare( void )
{
	if (pSys->iDisplayMode == TWENTY_FOUR_BIT_ADDITIVE) {
		glEnable(GL_COLOR_MATERIAL);
		glBlendFunc(GL_ONE,GL_ONE);
	} else {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}