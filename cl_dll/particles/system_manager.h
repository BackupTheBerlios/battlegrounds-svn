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

// declaration of the particle system manager

#ifndef PARTICLE_SYSTEM_MANAGER_H
#define PARTICLE_SYSTEM_MANAGER_H

// in an ideal world this would be a true singleton,
// but I don't have a singleton template class atm, so this will do
class CParticleSystemManager {
private:
	// our list of particle systems.
	vector <CParticleSystem*> m_pParticleSystems;
	vector <particle_texture_cache*> m_pTextures;
	vector <CParticle*> m_pParticles;

	// time of last sort
	float m_flLastSort;
	// time of last draw
	float m_flLastDraw;

	// adds in a new system
	void AddSystem(CParticleSystem* pSystem);
	// determines whether we're gonna allow drawing atm
	bool CheckDrawSystem( void );
	// deletes all particles
	void RemoveParticles( void );
	// clears the textures map
	void RemoveTextures( void );
	// returns the length of time between this draw and the last one
	inline float TimeSinceLastDraw( void ) const { return (gEngfuncs.GetClientTime() - m_flLastDraw); }

public:
	// creates and destroys a particle system
	CParticleSystemManager() { m_flLastSort = 0.0; }
	~CParticleSystemManager() { RemoveParticles(); RemoveSystems(); RemoveTextures(); }

	// allows us to remove all our systems
	void RemoveSystems( void );
	// remove a single system
	void RemoveSystem( unsigned int iSystem);

	// updates all systems
	void UpdateSystems( void );

	// wrappers to create particle systems
	void CreateFlintPS(vec3_t vPosition);
	void CreateBarrelPS(vec3_t vPosition, vec3_t vDirection);
	void CreateSparkPS(vec3_t vPosition, vec3_t vDirection);
	void CreateWhitePS(vec3_t vPosition, vec3_t vDirection);
	void CreateBrownPS(vec3_t vPosition, vec3_t vDirection);
	void CreateGrassPS(char* sFile, particle_system_management* pSystem );
	void CreateMappedPS(char* sFile, particle_system_management* pSystem );
	void CreatePresetPS(unsigned int iPreset, particle_system_management* pSystem);

	// Adds Particles so we can do the actual drawing
	void AddParticle(CParticle* pParticle);
	// deletes a particle
	void RemoveParticle( CParticle* pParticle );

	// Allows us to maintain a lookup table of loaded textures
	void AddTexture(char* sName, particle_texture_s* pTexture);
	particle_texture_s* HasTexture(char* sName);
};

// TODO : Singleton the particle system manager
#endif
