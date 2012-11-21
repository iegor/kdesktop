//     This file is part of KFireSaver3D.

//     KFireSaver3D is free software; you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation; either version 2 of the License, or
//     (at your option) any later version.

//     KFireSaver3D is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.

//     You should have received a copy of the GNU General Public License
//     along with KFireSaver3D; if not, write to the Free Software
//     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA


//     Author: Enrico Ros, based on the great work of David Sansome (kfiresaver)
//     Email:  asy@libero.it

#ifndef KFIRESAVER_PARTICLE_H
#define KFIRESAVER_PARTICLE_H

#include <qgl.h>

#define FLICKER_FRAMES_DELAY 8
#define DRAND ((float)rand() / (float)RAND_MAX)  /*random float between 0 and 1*/

/* -- Particle class.
 * Sets initial parameters and takes care of updating physics for a single 
 * fireworks particle. The physics model is the Newtonian one.
 */
class Particle
{
    public:
	//enum definitions for type of particle
	enum ParticleType
	{
		FireParticle,
		FireWorkLeaderParticle,
		FireWorkDebrisParticle,
		LogoParticle,
		StarParticle
	};

	Particle( ParticleType pT );

	//public methods for initializing default parameters and update them
	virtual void initializeValues (
		int color_scheme = 0,
		Particle* leader = 0L,
		GLfloat powermin = 5.0,
		GLfloat powermax = 10.0,
		bool flickers = false,
		GLfloat *displace = 0L );

	virtual void updateParameters ( float timeGap );

	//public accessible variables of the class
	ParticleType particleType;
	int explosionsDepth;
	unsigned int texture;

	GLfloat	xpos,	ypos,	zpos,
		xspeed,	yspeed,	zspeed,
		zacc;

	GLfloat colour[4],
		life, startLife,
		pixelSize;

	bool useLife;
	int flicker;

    private:
	Particle();
};


/* -- TurningParticle class.
 * Randomize initial parameters similar to a standard 'spherical' particle 
 * and takes care of updating physics. The physics model is a funny 'bees'
 * (vectorial-product) one.
 */
class TurningParticle : public Particle
{
    public:
	TurningParticle( ParticleType pT );

	virtual void initializeValues (
		int color_scheme = 0,
		Particle* leader = 0L,
		GLfloat powermin = 5.0,
		GLfloat powermax = 10.0,
		bool flickers = false,
		GLfloat *displace = 0L );

	virtual void updateParameters ( float dT );

    private:
	float wx, wy, wz;
	TurningParticle();
};

#endif
