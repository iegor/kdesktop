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

#include <math.h>
#include <stdlib.h>

#include "firesaverparticle.h"
#include "firesaver.h"

//current color scheme :		red  orng grn  blue w  m  c
static const GLfloat c_red_min[7] =   { 0.4, 0.9,   0,   0, 1, 0, 0 };
static const GLfloat c_red_max[7] =   {   1,   1,   0,   0, 1, 1, 0 };
static const GLfloat c_green_min[7] = {   0, 0.4, 0.4,   0, 1, 0, 0 };
static const GLfloat c_green_max[7] = {   0, 0.5,   1, 0.5, 1, 0, 1 };
static const GLfloat c_blue_min[7] =  {   0, 0.1,   0, 0.5, 1, 0, 0 };
static const GLfloat c_blue_max[7] =  {   0, 0.2,   0,   1, 1, 1, 1 };


//default initialization (good for Leader / Fire)
Particle :: Particle( ParticleType pT )
    : particleType( pT ), explosionsDepth( 0 ), texture( 0 ),
    xpos( 0.0 ), ypos( 0.0 ), zpos( -9.9 ),
    xspeed( 0.0 ), yspeed( 0.0 ), zspeed( 0.0 ), zacc( -9.807 ),
    life( 0.0 ), startLife( 0.0 ),
    pixelSize( 5.0 ), useLife( true ), flicker( 0 )
{
	colour[0] = 0;
	colour[1] = 0;
	colour[2] = 0;
	colour[3] = 1;
}


void Particle :: initializeValues (
	int cs,
	Particle* debrisParent,
	GLfloat powermin, GLfloat powermax,
	bool flickers,
	GLfloat *displace )
//note:	this function is called when a particle needs a set of default parameters.
//	these depends on the ParticleType.
{
	switch (particleType)
	{
//	-- FireParticle (params: [cs])
//		born in a rectangle centered in { 0, 0, -9.9 }
//		speed 3-6 [m/s] blue/cyan colored
//		this is only done at the beginning or when a particle dies
	    case FireParticle:
		xpos = DRAND * FIELDWIDTH - FIELDW_2;
		ypos = DRAND - 2.0;

		xspeed = DRAND * 4.0 - 2.0;
		yspeed = DRAND * 2.0;
		zspeed = DRAND * 3.0 + 3.0;

		useLife = false;
		pixelSize = 2.0 + DRAND * 2.0;
		break;

//	-- FireWorkLeaderParticle (params: none)
//		they start in a rectangle at the same height but
//		with different power and the same 'orange' colour
	    case FireWorkLeaderParticle:
		xpos = DRAND * 14.0 - 7.0;
		ypos = DRAND * 2.0 - 1.0;

		xspeed = DRAND * 8.0 - 4.0 - (xpos / 2.0)*DRAND;
		yspeed = DRAND * 8.0 - 4.0;
		zspeed = DRAND * 6.5 + 18.0;

		colour[0] = 0.6;
		colour[1] = DRAND * 0.4;
		colour[2] = 0.0;

		useLife = false;
		break;

//	-- LogoParticle (params: none)
//		they start in the middle of the screen.
//		little g-force, constant life, weavy-y
	    case LogoParticle:
		yspeed = 3*(DRAND - DRAND);
		zacc = -9.807f / 5.0f;

		startLife = 0.7f;
		life = 1.7f;
		break;

//	-- StarParticle (params: none)
//		spherically distributed. xpos and ypos are the
//		transformed screen positions of the star.
	    case StarParticle:
		colour[0] = DRAND * 0.2 + 0.5;
		colour[1] = DRAND * 0.2 + 0.5;
		colour[2] = DRAND * 0.2 + 0.5;

		{bool accepted = false;
		while (!accepted) {
			float module = 30,
			      theta = DRAND * M_PI * 2.0,
			      u = DRAND * 2.0 - 1.0,
			      root = sqrt( 1 - u*u );
			xpos = module * root * cos(theta);
			ypos = fabs(module * root * sin(theta)) - 10.0;
			zpos = fabs(module * u);

			float sfactor = 256.0 / (256.0 + PERSP_MAG_FACTOR*ypos);
			xpos *= sfactor;
			ypos = sfactor * zpos - FIELDW_2;

			pixelSize = sfactor * (2.0 + 3.0*DRAND);

			accepted = xpos > -FIELDW_2 && xpos < FIELDW_2 &&
				   ypos > -FIELDW_2 && ypos < FIELDW_2;
		}}
		break;

//	-- FireWorkDebrisParticle (params: cs, parent, [powerm], [powerM], [flickers], [displace])
//		parameters are randomized for a 'spherical' explosion.
//		power{min,max}, flickers and displace applies only for that
//		kind of ParticleType.
	    case FireWorkDebrisParticle:

		//same origin of the dead leader
		xpos = debrisParent->xpos;
		ypos = debrisParent->ypos;
		zpos = debrisParent->zpos;

		//true spherical randomization
		float module = powermin + DRAND * (powermax - powermin),
		      theta = DRAND * M_PI * 2.0,
		      u = DRAND * 2.0 - 1.0,
		      root = sqrt( 1 - u*u );
		xspeed = debrisParent->xspeed + module * root * cos(theta) * (1.0 + DRAND/3.0);
		yspeed = debrisParent->yspeed + module * root * sin(theta) * (1.0 + DRAND/3.0);
		zspeed = module * u * (1.0 + DRAND/3.0);	//was 0.9 + DRAND/3

		//if set add a displace to speed
		if ( displace ) {
			xspeed += displace[0];
			yspeed += displace[1];
			zspeed += displace[2];
		}

		//randomize the color choosing on current palette
		colour[0] = c_red_min[cs] + (c_red_max[cs]-c_red_min[cs]) * DRAND;
		colour[1] = c_green_min[cs] + (c_green_max[cs]-c_green_min[cs]) * DRAND;
		colour[2] = c_blue_min[cs] + (c_blue_max[cs]-c_blue_min[cs]) * DRAND;

		pixelSize = DRAND * 2.0 + 2.0;
		zacc = -9.807 / (6.0 - pixelSize);

		life = startLife = pixelSize / 2.0;

		//if flickers is set the current visible delay is randomized
		if ( flickers )
			flicker = (int) ((DRAND * 2.0 - 1.0) * (float)FLICKER_FRAMES_DELAY);
		break;
	}
}


void Particle :: updateParameters( float dT )
//note:	this procedure uses a reduced set of parameters
//	x and y axis acceleration is no more used
//	the only external iterations are:
//	- the g force
//	- a sort of air friction that limits speed in x,y and
//	  acceleration on z
{
	xpos += xspeed * dT;
	ypos += yspeed * dT;
	zpos += (zspeed + zacc*dT/2) * dT;

	zspeed += zacc * dT;

	xspeed *= 0.998;
	yspeed *= 0.998;
	zspeed *= 0.999;

	if (useLife)
	    life -= dT;
}


// BEGIN TurningParticle class 

TurningParticle :: TurningParticle( ParticleType pT )
	: Particle( pT ) {}

void TurningParticle :: initializeValues (
	int cs,
	Particle* leader,
	GLfloat powermin,
	GLfloat powermax,
	bool /*flickers*/,
	GLfloat * /*displace*/ )
{
	//same origin of the parent
	xpos = leader->xpos;
	ypos = leader->ypos;
	zpos = leader->zpos;

	//velocity : true spherical randomization
	float   module = powermin + (powermax - powermin) * DRAND * 0.6,
		theta = DRAND * M_PI * 2.0,
		u = DRAND * 2.0 - 1.0,
		root = sqrt( 1 - u*u );
	xspeed = -module * root * cos(theta);
	yspeed = -module * root * sin(theta);
	zspeed = module * u;

	//spin axis : in quadrature with velocity
	module = (1 + DRAND) / 40;
	u = DRAND * 2.0 - 1.0;
	root = sqrt( 1 - u*u );
	// axis to spin around
	wx = module * root * cos(theta + M_PI_2);
	wy = module * root * sin(theta + M_PI_2);
	wz = module * u;

	//randomize the color choosing on current palette
	colour[0] = c_red_min[cs] + (c_red_max[cs]-c_red_min[cs]) * DRAND;
	colour[1] = c_green_min[cs] + (c_green_max[cs]-c_green_min[cs]) * DRAND;
	colour[2] = c_blue_min[cs] + (c_blue_max[cs]-c_blue_min[cs]) * DRAND;

        pixelSize = DRAND * 2.0 + 2.0;
	zacc = -9.807 / 5.0;
	life = startLife = pixelSize / 2.0;
}


void TurningParticle :: updateParameters ( float dT )
{
	//update position
	xpos += xspeed * dT;
	ypos += yspeed * dT;
	zpos += zspeed * dT;

	//tan vector = velocity vector (vect producted by) spin axis
	float vx = yspeed * wz - zspeed * wy,
	      vy = zspeed * wx - xspeed * wz,
	      vz = xspeed * wy - yspeed * wx;

	//update velocity adding a tangential component (aka infinitesimally
	//rotating the vector)
	xspeed += vx;
	yspeed += vy;
	zspeed += vz + zacc * dT;

	if (useLife)
		life -= dT;
}

//END TurningParticle 
