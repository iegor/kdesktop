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

#ifndef KFIRESAVER_H
#define KFIRESAVER_H

#define PERSP_MAG_FACTOR 8.0
#define NUMBER_OF_FIREPARTICLES 180

#define MSECPERIOD 17		// 1000ms / 60fps = 16.6667 ~= 17 [ms/frame]

#define FIELDWIDTH 30.0
#define FIELDW_2 (FIELDWIDTH/2.0)
#define FIELDW_4 (FIELDWIDTH/4.0)

#include <qgl.h>
#include <qptrlist.h>
#include <qimage.h>
#include <qstring.h>
#include <qcolor.h>

#include <arts/kplayobject.h>
#include <arts/kartsserver.h>
#include <arts/kartsdispatcher.h>

class Particle;
class Writer;

class KFireSaver : public QGLWidget
{
    public:
	KFireSaver( QWidget *parent=0, const char *name=0 );
	~KFireSaver();

    protected:
	void initializeGL();
	void resizeGL( int, int );
	void paintGL();
	void timerEvent( class QTimerEvent * );

    private:
	enum enumFireworkType
	{
		Sphere = 0,
		NoFW = 1,
		Circle = 2,
		BiCircle = 3,
		AngelHairs = 4,
		Splitter = 5,
		Spirals = 6,
		SuperNova = 7,
		NoRender = 8
	};

	inline enumFireworkType pickType();
	inline int pickColour();
	void explodeFirework(Particle* fireWorkLeaderParticle);
	void burnLogo(QImage * image);
	void playSound(QString file);
	bool loadTexture(QString file, unsigned int & textureID);
	void freeTexture(unsigned int & textureID);
	void readConfig();

	//list of particles and stars
	QPtrList<Particle>
		particleList,
		starList;

	//stuff for 'exploding' pixmaps (kde, tux, icons..)
	QPtrList<QImage>
		imageList;

	//texture stuff
	unsigned int
		currentTexture,
		particleTexture,
		starTexture,
		flareTexture,
		diastarTexture; // (to add) novaTexture cometTexture

	//sound stuff
	KArtsDispatcher artsDispatcher;
	KArtsServer artsServer;
	QPtrList<KPlayObject> playObjectList;
	QString	sound_explosion,
		sound_debris;

	//other stuff
	class Writer * writer;
	float	unitX,
		unitY;
	double	timeStampFrame;
	bool 	firstGLFrame;

	struct {
	   bool	enableBottomFire,	//show
		enableSound,
		enableNoOverhead,
		enableRealtime,
		enableCombos,		//fireworks
		enableLogos,		//specials
		enableReduceLogo,
		enableStars,
		enableStarFlickering,
		enableStarGradient,
		enableWritings,
		enableSphereLight,	//effects
		enableFlash,
		enableFade,
		enableMegaFlares,
		enableTrails;
	   int	fireworksFrequency,	//show
		particleSize,
		logoFrequency,		//specials
		starsNumber,
		flashOpacity,		//effects
		fadeAmount,
		megaFlares;
	   QColor
		bottomFireColor;
	   int	colorsCount,
		colorsT[8],
		typesCount;
	   enumFireworkType
		typesT[9];
	} parameters;


	enum enumShowType {
	    Show = 0,
	    Random = 1,
	    Monotype = 2,
	    Monochrome = 3
	};

	struct {
	   enum enumShowType ShowType;
	   int	colour,
		colourSec;
	   enumFireworkType
		type,
		typeSec;
	   bool	forceBicolour,
		forceColour,
		forcePower,
		forceType;
	   float powerEnvelop;
	   double timeStamp,
		timeGap;
	} showp;
};

#endif
