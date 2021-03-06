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

// declares the classes for the actual particles

#ifndef PARTICLE_H
#define PARTICLE_H

// keeps as much of the interface hidden for everything but the particle system
class CParticle
{
protected:
	// allows the particle system to interact with the interface
	friend class CParticleSystem;

	// prepares drawing
	virtual void Prepare( void );
	// sets up the base attributes
	virtual void InitValues( void ) = 0;
	// shows this particle on the screen
	virtual void Draw( void );
	// tests whether this particle is ready to die
	virtual bool Test( void );
	// updates the particle
	virtual void Update( float flTimeSinceLastDraw ) = 0;

	// distances between this particle and the player
	virtual float DistanceToThisPlayer( bool square );

	// creates and destroy's particle
	// only the particle system can do this
	CParticle();
	virtual ~CParticle();

	// this particle's attributes
	base_particle sParticle;

	friend class CParticleSystemManager;
	friend bool less_than(CParticle *pPart, CParticle *pPart2);
};

bool less_than(CParticle *pPart, CParticle *pPart2);

// now lets define the individual particles
class CFlintParticle : public CParticle
{
protected:
	friend class CParticleSystem;
	friend class CFlintlockSmokeParticleSystem;

	// sets up the base attributes
	virtual void InitValues( void );
	// updates the particle
	virtual void Update( float flDrawingTime );

	CFlintParticle(vec3_t vPosition, particle_texture_s *pParticleTexture);
	~CFlintParticle() {}
};

// creates a barrel particle
class CBarrelParticle : public CParticle
{
protected:
	friend class CParticleSystem;
	friend class CBarrelSmokeParticleSystem;

	// sets up the base attributes
	virtual void InitValues( void );
	// updates the particle
	virtual void Update( float flDrawingTime );

	CBarrelParticle(vec3_t vPosition, vec3_t vDirection, particle_texture_s *pParticleTexture);
	~CBarrelParticle() {}
};

// creates a spark particle
class CSparkParticle : public CParticle
{
protected:
	friend class CParticleSystem;
	friend class CSparkParticleSystem;

	// prepares drawing
	virtual void Prepare( void );
	// sets up the base attributes
	virtual void InitValues( void );
	// updates the particle
	virtual void Update( float flDrawingTime );
	// draws the particle
	virtual void Draw( void );

	CSparkParticle(vec3_t vPosition, vec3_t vDirection, particle_texture_s *pParticleTexture);
	~CSparkParticle() {}
};

// creates a black or white smoke particle
class CWhiteSmokeParticle : public CParticle
{
protected:
	friend class CParticleSystem;
	friend class CWhiteSmokeParticleSystem;
	friend class CBrownSmokeParticleSystem;

	// sets up the base attributes
	virtual void InitValues( void );
	// updates the particle
	virtual void Update( float flDrawingTime );

	CWhiteSmokeParticle(vec3_t vPosition, vec3_t vDirection, particle_texture_s *pParticleTexture);
	~CWhiteSmokeParticle() {}
};

// creates a mapped particle system
class CMappedParticle : public CParticle
{
private:
	mapped_particle_system *pSys;

	// internals used by bp which i don't really understand
	float flParticleTime;
	unsigned int iCurrentFrame;
	bool bWindChanged;
	bool bIgnoreParticle;
	bool bCollisionChecked[6];
	unsigned int iCollisionTimer[6];
protected:
	friend class CParticleSystem;
	friend class CMappedParticleSystem;

	// prepares drawing
	virtual void Prepare( void );
	// sets up the base attributes
	virtual void InitValues( void );
	// tests whether this particle is ready to die
	virtual bool Test( void );
	// updates the particle
	virtual void Update( float flDrawingTime );
	// draws the particle
	virtual void Draw( void );

	// create and destroy the particle
	CMappedParticle(mapped_particle_system *pSystem, particle_texture_s *pParticleTexture);
	~CMappedParticle() { }
};

// creates a grass particle
class CGrassParticle : public CParticle
{
private:
	// details about the system
	grass_particle_system *pSys;

	// internals used by bp which i don't really understand
	bool m_bCollisionChecked;
	bool m_bIngoreParticle;
	vec3_t m_vNormal;
	vec3_t m_vWaveNormal;
	float m_flLodMinDistance;
	float m_flLodMaxDistance;
	float m_flParticleTime;
protected:
	friend class CParticleSystem;
	friend class CGrassParticleSystem;

	// sets up the base attributes
	virtual void InitValues( void );
	// tests whether this particle is ready to live some more
	virtual  bool Test( void );
	// updates the particle
	virtual void Update( float flDrawingTime );
	// draws the particle
	virtual void Draw( void );

	CGrassParticle(grass_particle_system *pSystem, particle_texture_s *pParticleTexture);
	~CGrassParticle() { }
};
// forward declare our classes
class CGrassParticle;

#endif
