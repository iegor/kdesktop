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
#include <sys/time.h>
#include <stdlib.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kurl.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <arts/kmedia2.h>
#include <arts/kplayobject.h>
#include <arts/kplayobjectfactory.h>

#include "firesaversetup.h"
#include "firesaverparticle.h"
#include "firesaverwriter.h"
#include "firesaver.h"


/* Factory code for KScreensaver begins *here* *\
\*                                             */

#include <kscreensaver.h>
#include <kdialogbase.h>

class KFireSaverKSS : public KScreenSaver
{
    public:
	KFireSaverKSS( WId id )
	    : KScreenSaver( id )
	{
	    setBackgroundColor( black );
	    erase();
	    saver = new KFireSaver;
	    embed(saver);
	    saver->show();
	}

	~KFireSaverKSS()
	{
	    delete saver;
	}

    private:
	KFireSaver* saver;
};

class KFireSaverSetupKDB : public KDialogBase
{
    public:
	KFireSaverSetupKDB( QWidget* parent = 0, const char* name = 0 )
	    : KDialogBase( parent, name, true, i18n("Setup Screen Saver"),
	    Ok | Cancel | Help, Ok, true )
	{
		setup = new KFireSaverSetup( this );
		setMainWidget( setup );
		setButtonText( KDialogBase::Help, i18n( "A&bout" ) );
	}

    private slots:
	void slotHelp()
	{
		KMessageBox::about(this, i18n("<h3>KFireSaver 3D 1.0</h3>\n<p>TEST Koral - Enrico Ros::2004</p>") );
	}
	void slotOk()
	{
		setup->writeConfig();
		accept();
	}

    private:
	KFireSaverSetup * setup;
};

extern "C"
{
    KDE_EXPORT const char *kss_applicationName = "kfiresaver.kss";
    KDE_EXPORT const char *kss_description = I18N_NOOP( "Fireworks 3D (GL)" );
    KDE_EXPORT const char *kss_version = "0.7";

    KDE_EXPORT KScreenSaver *kss_create( WId id )
    {
	return new KFireSaverKSS( id );
    }

    KDE_EXPORT QDialog *kss_setup()
    {
	return new KFireSaverSetupKDB;
    }
}

/*                                             *\
\* Factory code for KScreensaver ends *here*   */


KFireSaver :: KFireSaver( QWidget *parent, const char *name )
	: QGLWidget( parent, name )
{
	// set random seed to initialize drand48() calls
	timeval tv;
	gettimeofday(&tv,NULL);
	srand48( (long)tv.tv_usec );

	readConfig();

	particleList.setAutoDelete(true);
	starList.setAutoDelete(true);
	imageList.setAutoDelete(true);
	playObjectList.setAutoDelete(true);

	//initialize openGL context before managing GL calls
	makeCurrent();
	loadTexture( locate("data","kfiresaver/kfs_particle.png"), particleTexture );
	loadTexture( locate("data","kfiresaver/kfs_particle_flare.png"), flareTexture );
	loadTexture( locate("data","kfiresaver/kfs_particle_diastar.png"), diastarTexture );
	starTexture = particleTexture;

	//generate stars
	if (parameters.enableStars) {
		int number = parameters.starsNumber + 1;
		number *= 10 * number;
		for (int i=0 ; i<number ; i++)
		{
			Particle * star = new Particle( Particle::StarParticle );
			star->initializeValues();
			star->texture = starTexture;
			if (parameters.enableStarGradient)
			{
				GLfloat red = star->colour[0],
					green = star->colour[1],
					blue = star->colour[2],
					tint = 0.5 + star->ypos / FIELDWIDTH,
					merge = 0.3 + DRAND*0.7,
					mergen = 1 - merge;
				star->colour[0] = red * merge + (1.0-tint) * mergen;
				star->colour[1] = green * merge + tint * mergen;
				star->colour[2] = blue * merge + tint * mergen;
			}
			starList.append( star );
		}
	}

	//generate bottom fire
	if (parameters.enableBottomFire) {
		float cRed = (float)parameters.bottomFireColor.red() / 255.0,
		      cGreen = (float)parameters.bottomFireColor.green() / 255.0,
		      cBlue = (float)parameters.bottomFireColor.blue() / 255.0;
		for (int i=0 ; i<NUMBER_OF_FIREPARTICLES ; i++)
		{
			Particle* particle = new Particle( Particle::FireParticle );
			particle->initializeValues();
			particle->texture = particleTexture;
			particle->zspeed *= 4.0;
			particle->colour[0] = cRed * (0.6 + 0.4*DRAND);
			particle->colour[1] = cGreen * (0.6 + 0.4*DRAND);
			particle->colour[2] = cBlue * (0.6 + 0.4*DRAND);
			particleList.append(particle);
		}
	}

	//get sound files
	if (parameters.enableSound) {
		sound_explosion = locate("data","kfiresaver/kfs_explode.ogg");
		sound_debris = locate("data","kfiresaver/kfs_debris.ogg");
	}

	//create the writer class that manages flying writings.
	if ( parameters.enableWritings )
		writer = new Writer("kfs_letters.desc");

	showp.forceBicolour =
	showp.forceColour =
	showp.forcePower =
	showp.forceType = false;
	showp.timeStamp = 0.0;
	startTimer(MSECPERIOD);

	//force initialization of "show" variables for the first time
	timerEvent(NULL);
}


KFireSaver :: ~KFireSaver()
{
	freeTexture( particleTexture );
	freeTexture( starTexture );
	particleList.clear();
	starList.clear();
	imageList.clear();
	playObjectList.clear();
	if ( parameters.enableWritings )
		delete writer;
}


void KFireSaver :: initializeGL()
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glShadeModel( GL_SMOOTH );

	resizeGL( 640, 480 );
}


void KFireSaver :: resizeGL( int width, int height )
{
	glViewport( 0, 0, width, height );

	glMatrixMode(GL_PROJECTION);
	 glLoadIdentity();
	 glOrtho( -FIELDW_2, FIELDW_2, -FIELDW_2, FIELDW_2, 5.0, 60.0 );

	glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();

	float ratio = (float)width / (float)height,
	      numH = 750 - 90 * parameters.particleSize,
	      numW = 1000 - 120 * parameters.particleSize;
	if ( ratio >= (4.0/3.0) ) {
		unitX = FIELDWIDTH / (numH * ratio);
		unitY = FIELDWIDTH / numH;
	} else {
		unitX = FIELDWIDTH / numW;
		unitY = FIELDWIDTH / (numW / ratio);
	}

	timeval tv;
	gettimeofday(&tv,NULL);
	timeStampFrame = (double)tv.tv_sec + (double)tv.tv_usec/1000000.0;

	firstGLFrame = true;
}


void KFireSaver :: paintGL ()
/* Main procedure. It does the following:
   - calculate time diff between current and previous frame
   - clear the color buffer
   - simple render of stars
   - advanced render of particles
     - render
     - update physics based on time difference
     - check die/change conditions
       - call to explode_firework if a leader dies
   - if random -> start a new firework
   - if random -> explode a penquin or kde logo
*/
{
	/* calculate TIME ELAPSED between current and previous frame */

	timeval tv;
	gettimeofday(&tv,NULL);
	double timeCurrent = (double)tv.tv_sec + (double)tv.tv_usec/1000000.0;
	double timeDiff = (MSECPERIOD / 1000.0);
	if (parameters.enableRealtime)
	{
		timeDiff = timeCurrent - timeStampFrame;
		timeStampFrame = timeCurrent;
		timeDiff = (timeDiff > 0.5) ? 0.5 : (timeDiff < 0.005) ? 0.005 : timeDiff;
	}

	/* CLEAR SCREEN: to do it there are 2 ways */

	glLoadIdentity();
	glTranslatef( 0, 0, -50.0 );
	glDisable( GL_TEXTURE_2D );

	if ( !parameters.enableFade || firstGLFrame )
	{	// quick - clear the OpenGL color buffer
		glClearColor( 0.0, 0.0, 0.0, 1.0 );
		glClear( GL_COLOR_BUFFER_BIT );
		firstGLFrame = false;
	}
	else
	{	// good looking
		/* superpose a semi-transparent black rectangle so we
		   can see a sort of 'tail' for each particle drawn. */
		const GLfloat conv_tab[10] = {
			0.50, 0.33, 0.22, 0.15, 0.10,
			0.07, 0.05, 0.03, 0.02, 0.01 };
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0.0,0.0,0.0, conv_tab[parameters.fadeAmount]);
		glBegin( GL_TRIANGLE_STRIP );
		  glVertex2f( FIELDW_2, FIELDW_2 );
		  glVertex2f( -FIELDW_2, FIELDW_2 );
		  glVertex2f( FIELDW_2, -FIELDW_2 );
		  glVertex2f( -FIELDW_2, -FIELDW_2 );
		glEnd();
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	}

	/* render STARS */

	if (parameters.enableStars) {
		if ( starTexture ) {
			glEnable( GL_TEXTURE_2D );
			glBindTexture( GL_TEXTURE_2D, currentTexture = starTexture );
		} else
			glDisable( GL_TEXTURE_2D );

		glBegin( GL_QUADS );
		bool	flickers = parameters.enableStarFlickering;
		float	alpha = flickers ? 0.5 : 1.0;
		Particle * star = starList.first();
		for (; star; star = starList.next())
		{
			if (flickers && DRAND<0.6)
				continue;

			GLfloat sizeX = star->pixelSize * unitX,
				sizeY = star->pixelSize * unitY,
				pLeft = star->xpos - sizeX,
				pRight = star->xpos + sizeX,
				pTop = star->ypos + sizeY,
				pBottom = star->ypos - sizeY;
			glColor4f(star->colour[0], star->colour[1], star->colour[2], alpha);
			glTexCoord2f( 0, 0 );	// Bottom Left
			glVertex2f( pLeft, pBottom );
			glTexCoord2f( 0, 1 );	// Top Left
			glVertex2f( pLeft, pTop );
			glTexCoord2f( 1, 1 );	// Top Right
			glVertex2f( pRight, pTop );
			glTexCoord2f( 1, 0 );	// Bottom Right
			glVertex2f( pRight, pBottom );
		}
		glEnd();
	}

	/* render FIREWORKS */

	glBegin( GL_QUADS );
	bool playedExplodeSound = false;
	bool flashedScreen = false;
	Particle * particle = particleList.first();
	for (; particle; particle = particleList.next())
	{
		//bind the texture for current particle (if not already bound)
		if ( !particle->texture ) {
			glEnd();
			glDisable( GL_TEXTURE_2D );
			glBegin( GL_QUADS );
			currentTexture = 0;
		} else if ( particle->texture != currentTexture ) {
			glEnd();
			glEnable( GL_TEXTURE_2D );
			glBindTexture( GL_TEXTURE_2D, currentTexture = particle->texture );
			glBegin( GL_QUADS );
		}

		//perspective projection (done by hand to make it funnier than opengl's :-)
		float mfactor = PERSP_MAG_FACTOR * particle->ypos;
		if ( mfactor < -246.0 ) {
			particleList.remove();
			particleList.prev();
			continue;
		}
		float sfactor = 256.0 / (256.0 + mfactor),
		      posx = sfactor * particle->xpos,
		      posy = sfactor * particle->zpos - 4.0;
		//size computation (magnify if enableMegaFlares is set)
		if ( parameters.enableMegaFlares ) {
			mfactor = parameters.megaFlares*particle->ypos;
			if ( mfactor < -255.0 || mfactor > 512.0 ) {
				particleList.remove();
				particleList.prev();
				continue;
			}
			sfactor = 256.0 / (256.0 + mfactor);
			if ( sfactor > 64 )
				sfactor = 76.8 - sfactor / 5.0;
		}
		float size = sfactor * particle->pixelSize,
		      sizeX = size * unitX,
		      sizeY = size * unitY;

		//determine brightness (alpha component) for the particle
		if ( particle->useLife ) {
			float life = particle->life,
			      startLife = particle->startLife;
			//bright changes with the following curve: "2*k - k^2" (or "k(2-k)")
			if ( life > startLife )
				particle->colour[3] = startLife + 1 - life;
			else
				particle->colour[3] = life / startLife;
			//apply flickering if enabled
			if (particle->flicker < 0) {
				particle->colour[3] = 0.2;
				if (++particle->flicker >= 0)
					particle->flicker = FLICKER_FRAMES_DELAY;
			} else if (particle->flicker > 0) {
				if ( life <= startLife )
					particle->colour[3] = 1.0;
				if (--particle->flicker <= 0)
					particle->flicker = -FLICKER_FRAMES_DELAY;
			}
			glColor4fv( particle->colour );
		} else
			glColor3fv( particle->colour );

		//draw particle
		float pLeft = posx - sizeX,
		      pTop = posy + sizeY,
		      pRight = posx + sizeX,
		      pBottom = posy - sizeY;
		glTexCoord2f( 0, 0 );	// Bottom Left
		glVertex2f( pLeft, pBottom );
		glTexCoord2f( 0, 1 );	// Top Left
		glVertex2f( pLeft, pTop );
		glTexCoord2f( 1, 1 );	// Top Right
		glVertex2f( pRight, pTop );
		glTexCoord2f( 1, 0 );	// Bottom Right
		glVertex2f( pRight, pBottom );

		//phisically update parameters of the particle
		particle->updateParameters( timeDiff );

		//check for particle death / explosion
		switch (particle->particleType)
		{
			//a Fireparticle is restarted when in right conditions
		    case Particle::FireParticle:
			if ( posx < -FIELDW_2 || posx > FIELDW_2 ||
				(particle->zpos < -10.0 && posy < -FIELDW_2) )
			{
				particle->initializeValues();
				if ( DRAND > 0.9995 )
					particle->zspeed *= 4;
			}
			break;

			//a leader explodes when his z speed drops to zero
			//or, if it uses life, at death
		    case Particle::FireWorkLeaderParticle:
			if ((particle->zspeed <= 0.0f && !particle->useLife) ||
			    (particle->useLife && particle->life <= 0.0) )
			{
				// play sound if enabled (and once per frame)
				if (parameters.enableSound && !playedExplodeSound)
				{
					playSound(sound_explosion);
					playedExplodeSound = true;
				}
				// flash screen if enabled
				if (parameters.enableFlash && !flashedScreen) {
					glEnd();
					glDisable( GL_TEXTURE_2D );
					glColor4f( 1,1,1, parameters.flashOpacity / 10.0 );
					glBegin( GL_TRIANGLE_STRIP );
					glVertex2f( FIELDW_2, FIELDW_2 );
					glVertex2f( -FIELDW_2, FIELDW_2 );
					glVertex2f( FIELDW_2, -FIELDW_2 );
					glVertex2f( -FIELDW_2, -FIELDW_2 );
					glEnd();
					if ( particleTexture )
						glEnable( GL_TEXTURE_2D );
					glBegin( GL_QUADS );
					flashedScreen = true;
				}
				// generating children and removing parent
				int elementIndex = particleList.at();
				explodeFirework(particle);
				particleList.remove(elementIndex);
				particleList.prev();
			} else if ( parameters.enableTrails && DRAND < 0.4 ) {
				// leave trail behind the particle (it'a small and slow red debris)
				Particle * p = new Particle( Particle::FireWorkDebrisParticle );
				p->initializeValues( 0, particle, 1, 1 );
				p->texture = particleTexture;
				p->xspeed /= 4;
				p->yspeed /= 4;
				p->zspeed /= 8;
				p->zacc /= 4;
				p->pixelSize = 2;
				p->colour[0] /= 2;
				int elementIndex = particleList.at();
				particleList.append( p );
				particleList.at( elementIndex );
			}
			break;

			//remove if dead or outside field
		    default:
			if (particle->life <= 0.0 || posx<-FIELDW_2 || posx>FIELDW_2 || posy<-FIELDW_2) {
				particleList.remove();
				particleList.prev();
			}
			break;
		}
	}
	glEnd();

	/* render WRITINGS */

	if ( parameters.enableWritings )
	{
		int chance = (int) (1000.0 * DRAND);
		if ( !chance ) {
			static const QString someStrings[] = {
				i18n("www.kde.org"),
				i18n("My KDE, please!"),
				i18n("KoNqUeR the World"),
				i18n("KFIRESAVER 3D"),
				i18n("Gimme your eyes..."),
				i18n("Thank you for using KDE"),
				i18n("Going insane tonight"),
			};
			int n = (int)(6.0 * DRAND);
			writer->spawnWords( someStrings[n], Writer::Fun1 );
		}
		writer->render( timeDiff );
	}

	/* generate a new FIREWORK_LEADER */

	int random = (int) ((float)parameters.fireworksFrequency * DRAND);
	if (showp.ShowType == Show)
	{
		//double the chances ('frequency') to raise a new leaderParticle
		//but envelop it under a sine function
		float step = (showp.timeStamp - timeCurrent) / showp.timeGap;
		if (DRAND > sin(M_PI*step))
			random = -1;
		if (showp.type == AngelHairs && DRAND < 0.5)
			random = -1;
	}
	if ( !random )
	{
		Particle * particle = new Particle( Particle::FireWorkLeaderParticle );
		particle->initializeValues();
		particle->texture = flareTexture;
		particleList.append( particle );
	}

	/* explode a logo */

	int logoImages = imageList.count();
	if ( logoImages > 0 ) {
		random = (int) (parameters.logoFrequency * logoImages * 200.0 * DRAND);
		if ( random < logoImages )
		{
			if (parameters.enableFlash && !flashedScreen) {
				glDisable( GL_TEXTURE_2D );
				glColor4f( 1,1,1, parameters.flashOpacity / 10.0 );
				glBegin( GL_TRIANGLE_STRIP );
				glVertex2f( FIELDW_2, FIELDW_2 );
				glVertex2f( -FIELDW_2, FIELDW_2 );
				glVertex2f( FIELDW_2, -FIELDW_2 );
				glVertex2f( -FIELDW_2, -FIELDW_2 );
				glEnd();
			}
			burnLogo( imageList.at(random) );
		}
	}
}


int KFireSaver :: pickColour()
{
	int color = (int) (DRAND * parameters.colorsCount);
	return parameters.colorsT[ color ];
}


KFireSaver :: enumFireworkType KFireSaver :: pickType()
{
	int type = (int) (DRAND * parameters.typesCount);
	return parameters.typesT[ type ];
}


void KFireSaver :: explodeFirework(Particle* leaderParticle)
{
	GLfloat displace[3] = {0.0,0.0,0.0};
	float tmp1 = 0.0, tmp2 = 0.0, tmp3 = 0.0, tmp4 = 0.0, tmp5 = 0.0;

	// color of exploded particles
	bool bicolor = parameters.enableCombos && (showp.forceBicolour || DRAND > 0.95),
	     flickers = false;
	int cscheme = showp.forceColour ? showp.colour : pickColour(),
	    cscheme2 = showp.forceColour ? showp.colourSec : pickColour();

	// randomize type of exploding firework
	enumFireworkType fwType =
		showp.forceType ? (enumFireworkType) showp.type : pickType();

	// other options for generated particles
	int number = (int) ((DRAND + DRAND) * 150.0);
	float power = showp.forcePower ?
		showp.powerEnvelop * (0.8 + 0.3*DRAND) :
		DRAND * 11.0 + 2.0,
	      powermin = DRAND * power;

	// now some rules ...
        //a splitter can't split up more than 2 times
	if (fwType == Splitter && leaderParticle->explosionsDepth > 1) {
		if (parameters.typesCount == 1)
			return;
		if (showp.forceType)
			fwType = showp.typeSec;
		if (fwType == Splitter)
			while ( (fwType = pickType()) == Splitter );
	}

	// PRE-ADJUST parameters for the firework we're creating
	switch ( fwType )
	{
		//no need to handle this. it's the default configuration.
	    case Sphere:
		break;

		//explosion whithout emitting particles, only a flash
	    case NoFW:
		number = 1;
		power = powermin = 0;
		break;

		//splits up into 'number' orange pieces. tmp1 holds base_life
	    case Splitter:
		cscheme = showp.forceColour ? showp.colour : 1;
		bicolor = false;
		number = 3 + (int) (DRAND * 4);
		power /= 2.0;
		powermin = power / 2.0;
		tmp1 = 0.4 + DRAND * 1.5;
		break;

		//randomize a couple of angles (phi - theta) for exploding circle
	    case BiCircle:
		number *= 2;
	    case Circle:
		power = DRAND * 5.0 + 4.0;
		tmp1 = DRAND * M_PI;
		tmp2 = DRAND * M_PI;
		tmp4 = cos(tmp2);	//c2
		tmp3 = sin(tmp2);	//s2
		tmp2 = cos(tmp1);	//c1
		tmp1 = sin(tmp1);	//s1
		break;

		//cascade of flickering orange particles
	    case AngelHairs:
		cscheme = showp.forceColour ? showp.colour : 1;
		bicolor = false;
		flickers = true;
		power = 0.8 + DRAND * 1.9;
		powermin = DRAND*0.5;
		number = 100 + (int)(DRAND * 150);
		displace[0] = -leaderParticle->xspeed/2;
		displace[1] = -leaderParticle->yspeed/2;
		displace[2] = power;
		break;

		//behave as a standard spherical firework
	    case Spirals:
		break;

		//not yet implemented, suppressing particles
	    case SuperNova:
	    case NoRender:
		number = 0;
		break;
	}

	//limit number of particles as we are getting to the capacity saturation
	float currentParticles = (float) particleList.count();
	const float particleCapacity = 15000;
	const float particleGap = 8000;
	if ( number > 10 && currentParticles > (particleCapacity - particleGap) )
	{
		//NoFW, Splitter and NoRender aren't limited.
		number = (int)( (float)number * (particleCapacity - currentParticles) / particleGap );
		if ( number < 10 )
			number = 0;
	}

	int newExplosionsDepth = leaderParticle->explosionsDepth + 1;
	for (int i=0 ; i<number ; i++)
	{
		Particle * particle;
		if ( fwType == Spirals )
			particle = new TurningParticle( Particle::FireWorkDebrisParticle );
		else
			particle = new Particle( Particle::FireWorkDebrisParticle );

		particle->initializeValues (
			bicolor && (i>number/2) ? cscheme2 : cscheme,
			leaderParticle, powermin, power,
			flickers, displace );
		particle->texture = particleTexture;
		particle->explosionsDepth = newExplosionsDepth;

		// POST-ADJUST particle coefficients adapting to current FireworkType
		switch ( fwType )
		{
			//create a big, white particle, simulating explosion
		    case NoFW:
			if (parameters.enableFade)
				particle->startLife = particle->life = 0.030;
			else
				particle->startLife = particle->life = 0.075;
			particle->texture = flareTexture;
			particle->colour[0]=
			particle->colour[1]=
			particle->colour[2]=1.0;
			particle->pixelSize = 50.0 + 75.0 * DRAND;
			particle->zacc = 0;
			break;

			//default. no need to change parameters, only create the
			//'sphere light' as the first big particle if set.
		    case Sphere:
			if (i==0 && parameters.enableSphereLight && number > 40) {
				particle->texture = flareTexture;
				particle->xspeed = leaderParticle->xspeed;
				particle->yspeed = leaderParticle->yspeed;
				particle->zspeed = 0.0;
				particle->colour[0] /= 2.0;
				particle->colour[1] /= 2.0;
				particle->colour[2] /= 2.0;
				float impression = power * (float)number/5.0;
				particle->pixelSize = 25.0 * DRAND + impression;
				if (parameters.enableFade) {
					particle->startLife = particle->life = 0.040;
				} else {
					particle->startLife = 1.3;
					particle->life = 1.8;
				}
			}
			break;

			//
		    case Splitter:
			particle->particleType = Particle::FireWorkLeaderParticle;
			particle->pixelSize *= 3.0;
			particle->startLife = particle->life = tmp1 * (0.75 + DRAND/3.0);
			if (particle->zspeed < 0)
				particle->zspeed = -particle->zspeed;
			break;

		    case Circle:
		    case BiCircle:
			tmp5 = 2 * M_PI * DRAND;
			//MMX can be useful.. if you know how to use it :-)
			if ( fwType == BiCircle && i > number/2 ) {
				GLfloat ey = cos(tmp5),
					ez = sin(tmp5);
				particle->xspeed = power * (                tmp3*ez );
				particle->yspeed = power * ( tmp2*ey - tmp1*tmp4*ez );
				particle->zspeed = power * ( tmp1*ey + tmp2*tmp4*ez );
			} else {
				GLfloat ex = sin(tmp5),
					ey = cos(tmp5);
				particle->xspeed = power * (       tmp4*ex           );
				particle->yspeed = power * (  tmp1*tmp3*ex + tmp2*ey );
				particle->zspeed = power * ( -tmp2*tmp3*ex + tmp1*ey );
			}
			break;

		    case AngelHairs:
			particle->zacc = -9.807 * (0.05 + DRAND*0.20);
			particle->life = 2.0 + DRAND*2.5;
			particle->startLife = particle->life - 1;
			if (particle->zspeed < 0)
				particle->zspeed *= -1;
			//particle->pixelSize = 5.0;
			break;

		    case Spirals:
			particle->texture = diastarTexture;
			break;

			//discard cases
		    case SuperNova:
		    case NoRender:
			break;
		}
		particleList.append(particle);
	}
}

void KFireSaver :: timerEvent(QTimerEvent*)
{
	timeval tv;
	gettimeofday(&tv,NULL);
	double currentTime = (double)tv.tv_sec + (double)tv.tv_usec/1000000.0;

	bool	chooseType = false,
		chooseColor = false,
		chooseOthers = false,
		updateTimings = false;
	bool	firstTime = showp.timeStamp == 0.0;
	bool	endOfScene = currentTime >= showp.timeStamp;

	if (firstTime)
	switch (showp.ShowType)
	{
	    case Monotype:
		/* first time choose the type, color and attributes will
		   be choosed randomly for every firework which explodes*/
		showp.forceType = true;
		chooseType = true;
		break;

	    case Monochrome:
		/* first time choose the color, type and attributes will
		   be choosed randomly for every firework which explodes*/
		showp.forceColour = true;
		chooseColor =
		chooseOthers = true;
		break;

	    default: break;
	}

	if (endOfScene || firstTime)
	switch (showp.ShowType)
	{
	    case Show:
		/* if a scene ended, randomize global parameters for the
		   whole next scene */
		showp.forceType = true;
		showp.forceColour = true;
		showp.forcePower = true;
		chooseOthers =
		chooseColor =
		chooseType = true;
		updateTimings = true;
		break;

	    default: break;
	}

	if ( chooseType )
	{
		showp.type = pickType();
		if (parameters.typesCount < 2)
			showp.typeSec = NoRender;
		else
			while ((showp.typeSec = pickType()) == showp.type);
	}
	if ( chooseColor ) {
		showp.colour = pickColour();
		showp.colourSec = pickColour();
	}
	if ( chooseOthers )
	{
		showp.powerEnvelop = DRAND * 8.0 + 3.0;
		if (DRAND > 0.9)
		{
			showp.forceBicolour = true;
			showp.colourSec = pickColour();
		} else
			showp.forceBicolour = false;
	}
	if ( firstTime || updateTimings )
	{
		if (DRAND < 0.2)
			showp.timeGap = 1.0 + DRAND * 2.0;
		else
			showp.timeGap = 3.0 + DRAND * 10.0;
		showp.timeStamp = currentTime + showp.timeGap;
		showp.timeGap /= 1.5; //hack to introduce delay in sine func
	}

	updateGL();
}

void KFireSaver :: burnLogo(QImage * image)
{
	if (!image || image->isNull())
		return;
	int step = parameters.enableReduceLogo ? 2 : 1,
	    imageW = image->width(),
	    imageH = image->height(),
	    offsetX = imageW / 2,
	    offsetY = imageH / 2;
	float speed = FIELDW_4 / (imageW > imageH ? imageW : imageH),
	      speedXOffs = 5 * (DRAND - DRAND),
	      speedYOffs = DRAND + DRAND + 1;
	//if image is too big, lower sample points
	while ((imageW/step)>96 || (imageH/step)>96)
		step *= 2;
	for (int y=0 ; y<imageH ; y+=step)
	{
		for (int x=0 ; x<imageW ; x+=step)
		{
			QRgb pixel = image->pixel(x,y);
			if ( qAlpha(pixel) < 250 )
				continue;
			//if ( DRAND > 0.9 )
			//	continue;

			Particle* particle = new Particle( Particle::LogoParticle );
			particle->initializeValues();
			particle->texture = particleTexture;

			float xI = (x - offsetX) ,
			      yI = (offsetY - y) ;
			particle->xpos = xI * speed * 0.5;
			particle->zpos = yI * speed * 0.5 + 5;
			particle->xspeed = xI * speed + speedXOffs;
			particle->zspeed = yI * speed + speedYOffs;

			particle->colour[0] = qRed(pixel) / 255.0f;
			particle->colour[1] = qGreen(pixel) / 255.0f;
			particle->colour[2] = qBlue(pixel) / 255.0f;

			particleList.append(particle);
		}
	}
	if (parameters.enableSound)
		playSound(sound_debris);
}

void KFireSaver :: playSound(QString file)
{
	//flush inactive players
	KPlayObject * playObject = playObjectList.first();
	while ( playObject )
	{
		if ( playObject->state() != Arts::posPlaying )
		{
			playObjectList.remove();
			playObject = playObjectList.current();
		} else
			playObject = playObjectList.next();
	}

	//discart this sound if the player queue is already full (4 channels playing)
	if ( playObjectList.count() >= 6 )
		return;

	// not needed when all of the files are in the distribution
	//if (!QFile::exists(file))
		//return;

	KPlayObjectFactory factory(artsServer.server());
	playObject = factory.createPlayObject(KURL(file), true);

	if (playObject && !playObject->isNull())
	{
		playObject->play();
		playObjectList.append(playObject);
	}
}

bool KFireSaver :: loadTexture( QString fileName, unsigned int & textureID )
{
	//reset texture ID to the default EMPTY value
	textureID = 0;

	//load image
	QImage tmp;
	if ( !tmp.load( fileName ) )
		return false;

	//convert it to suitable format (flipped RGBA)
	QImage texture = QGLWidget::convertToGLFormat( tmp );
	if ( texture.isNull() )
		return false;

	//get texture number and bind loaded image to that texture
	glGenTextures( 1, &textureID );
	glBindTexture( GL_TEXTURE_2D, textureID );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0, 4 /* 3 ??? */, texture.width(), texture.height(),
		0, GL_RGBA, GL_UNSIGNED_BYTE, texture.bits() );
	return true;
}

void KFireSaver :: freeTexture( unsigned int & textureID )
{
	if ( textureID > 0 )
	    glDeleteTextures( 1, &textureID );
	textureID = 0;
}

void KFireSaver :: readConfig ()
{
	KConfig config("kfiresaverrc",true,false);

	// show
	config.setGroup( "Show" );
	showp.ShowType = (enum enumShowType)config.readNumEntry( "ShowType", 1 );
	parameters.fireworksFrequency = 11 - config.readNumEntry( "FireworksFrequency", 7 );
	if ( parameters.fireworksFrequency < 1 )
		parameters.fireworksFrequency = 1;
	if ( parameters.fireworksFrequency > 11 )
		parameters.fireworksFrequency = 11;
	parameters.fireworksFrequency *= (parameters.fireworksFrequency + 1); //*karl gauss's sum*
	parameters.particleSize = config.readNumEntry( "ParticlesSize", 0 );
	if ( parameters.particleSize < -5 )
		parameters.particleSize = -5;
	if ( parameters.particleSize > 5 )
		parameters.particleSize = 5;
	if ( parameters.enableBottomFire = config.readBoolEntry( "enable-BottomFire", true ) )
	{
		QColor blue = Qt::darkBlue;
		parameters.bottomFireColor = config.readColorEntry( "BottomFireColor", &blue );
	}
	parameters.enableSound = config.readBoolEntry( "enable-Sounds", false );
	parameters.enableNoOverhead = config.readBoolEntry( "enable-NoOverhead", true );
	parameters.enableRealtime = config.readBoolEntry( "enable-FrameSkip", true );

	// fireworks
	config.setGroup( "Fireworks" );
	parameters.typesCount = 0;
	if ( config.readBoolEntry( "use-Classic", true ) )
		parameters.typesT[parameters.typesCount++] = Sphere;
	if ( config.readBoolEntry( "use-Explosion", false ) )
		parameters.typesT[parameters.typesCount++] = NoFW;
	if ( config.readBoolEntry( "use-FlameRing", false ) )
		parameters.typesT[parameters.typesCount++] = Circle;
	if ( config.readBoolEntry( "use-FlameWorld", false ) )
		parameters.typesT[parameters.typesCount++] = BiCircle;
	if ( config.readBoolEntry( "use-Fall", false ) )
		parameters.typesT[parameters.typesCount++] = AngelHairs;
	if ( config.readBoolEntry( "use-Splitter", false ) )
		parameters.typesT[parameters.typesCount++] = Splitter;
	if ( config.readBoolEntry( "use-Spirals", false ) )
		parameters.typesT[parameters.typesCount++] = Spirals;
	if ( config.readBoolEntry( "use-SuperNova", false ) )
		parameters.typesT[parameters.typesCount++] = SuperNova;
	if ( !parameters.typesCount ) {
		kdWarning() << "KFireSaver3D: Warning, no fireworks enabled in config file" << endl;
		kdWarning() << "    enabling 'Classic Spherical'" << endl;
		parameters.typesCount = 1;
		parameters.typesT[0] = Sphere;
	}
	parameters.typesT[ parameters.typesCount ] =
		parameters.typesT[ parameters.typesCount-1 ];
	parameters.colorsCount = 0;
	if ( config.readBoolEntry( "use-Red", false ) )
		parameters.colorsT[parameters.colorsCount++] = 0;
	if ( config.readBoolEntry( "use-Orange", true ) )
		parameters.colorsT[parameters.colorsCount++] = 1;
	if ( config.readBoolEntry( "use-Green", false ) )
		parameters.colorsT[parameters.colorsCount++] = 2;
	if ( config.readBoolEntry( "use-Blue", false ) )
		parameters.colorsT[parameters.colorsCount++] = 3;
	if ( config.readBoolEntry( "use-White", true ) )
		parameters.colorsT[parameters.colorsCount++] = 4;
	if ( config.readBoolEntry( "use-Purple", false ) )
		parameters.colorsT[parameters.colorsCount++] = 5;
	if ( config.readBoolEntry( "use-DeepGreen", true ) )
		parameters.colorsT[parameters.colorsCount++] = 6;
	if ( !parameters.colorsCount )
	{
		kdWarning() << "KFireSaver3D: Warning enable at least one color" << endl;
		kdWarning() << "    enabling 'Blinding White'" << endl;
		parameters.colorsCount = 1;
		parameters.colorsT[0] = 4;
	}
	parameters.colorsT[ parameters.colorsCount ] =
		parameters.colorsT[ parameters.colorsCount-1 ];
	parameters.enableCombos = config.readBoolEntry( "use-Multicolor", true );

	// specials
	config.setGroup( "Specials" );
	if ( parameters.enableLogos = config.readBoolEntry( "enable-Logos", true ) )
	{
		QImage tempImage;
		tempImage.setAlphaBuffer( true );
		if ( config.readBoolEntry( "LogosTux", true ) )
			if ( tempImage.load(locate("data","kfiresaver/kfs_tux.png")) )
				imageList.append( new QImage(tempImage) );
		if ( config.readBoolEntry( "LogosKonqui", true ) )
			if ( tempImage.load(locate("data","kfiresaver/kfs_kde.png")) )
				imageList.append( new QImage(tempImage) );
		if ( config.readBoolEntry( "LogosKDEIcons", true ) ) {
			const QString icons[] = {
				"3floppy_unmount", "cdrom_unmount", "hdd_mount", "kmix",
				"network", "my-computer", "folder_home", "konqueror",
				"kmail", "penguin", "personal" };
			for ( int i = 0; i < 11; i++ )
				imageList.append( new QImage(DesktopIcon(icons[i],64).convertToImage()) );
		}
		parameters.enableReduceLogo = config.readBoolEntry( "LogosReduceDetail", true );
		parameters.logoFrequency = 11 - config.readNumEntry( "LogosFrequency", 4 );
		if ( parameters.logoFrequency < 1 )
			parameters.logoFrequency = 1;
		if ( parameters.logoFrequency > 11 )
			parameters.logoFrequency = 11;
	}
	if ( parameters.enableStars = config.readBoolEntry( "enable-Stars", true ) )
	{
		parameters.enableStarFlickering = config.readBoolEntry( "StarsFlicker", false );
		parameters.enableStarGradient = config.readBoolEntry( "StarsGradient", true );
		parameters.starsNumber = config.readNumEntry( "StarsNumber", 4 );
		if ( parameters.starsNumber < 0 )
			parameters.starsNumber = 0;
		if ( parameters.starsNumber > 10 )
			parameters.starsNumber = 10;
	}
	parameters.enableWritings = config.readBoolEntry( "enable-Writings", true );

	// effects
	config.setGroup( "Effects" );
	parameters.enableSphereLight = config.readBoolEntry( "enable-SphericalLight", true );
	if ( parameters.enableFlash = config.readBoolEntry( "enable-Flash", false ) )
	{
		parameters.flashOpacity = config.readNumEntry( "FlashOpacity", 5 );
		if ( parameters.flashOpacity < 0 )
			parameters.flashOpacity = 0;
		if ( parameters.flashOpacity > 10 )
			parameters.flashOpacity = 10;
	}
	if ( parameters.enableFade = config.readBoolEntry( "enable-Fade", false ) )
	{
		parameters.fadeAmount = config.readNumEntry( "FadeIntensity", 3 );
		if ( parameters.fadeAmount < 0 )
			parameters.fadeAmount = 0;
		if ( parameters.fadeAmount > 9 )
			parameters.fadeAmount = 9;
	}
	if ( parameters.enableMegaFlares = config.readBoolEntry( "enable-Flares", true ) )
	{
		parameters.megaFlares = config.readNumEntry( "FlaresDimension", 5 );
		if ( parameters.megaFlares < 0 )
			parameters.megaFlares = 0;
		if ( parameters.megaFlares > 10 )
			parameters.megaFlares = 10;
		parameters.megaFlares += 4;
		parameters.megaFlares *= 2;
	}
	parameters.enableTrails = config.readBoolEntry( "enable-Trail", false );
}
