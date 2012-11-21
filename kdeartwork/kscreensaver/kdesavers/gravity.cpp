//-----------------------------------------------------------------------------
//
// kgravity - Partical Gravity Screen Saver for KDE 2
//
// Copyright (c)  Ian Reinhart Geiser 2001
//
// KConfig code and KScreenSaver "Setup..." improvements by
// Nick Betcher <nbetcher@usinternet.com> 2001
//
#include <stdlib.h>
#include <qlabel.h>
#include <qlayout.h>
#include <kapplication.h>
#include <klocale.h>
#include <kconfig.h>
#include <kcolordialog.h>
#include <kbuttonbox.h>
#include <kcolorbutton.h>
#include "gravity.h"
#include "gravity.moc"
#ifdef Q_WS_MACX
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif
#include <qimage.h>
#include <kdebug.h>
#include <qpainter.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <kstandarddirs.h>
#include <math.h>
#include <kmessagebox.h>
// libkscreensaver interface
extern "C"
{
	KDE_EXPORT const char *kss_applicationName = "kgravity.kss";
	KDE_EXPORT const char *kss_description = I18N_NOOP( "Particle Gravity Screen Saver" );
	KDE_EXPORT const char *kss_version = "2.2.0";

	KDE_EXPORT KScreenSaver *kss_create( WId id )
	{
		return new KGravitySaver( id );
	}

	KDE_EXPORT QDialog *kss_setup()
	{
		return new KGravitySetup();
	}
}

//-----------------------------------------------------------------------------
// dialog to setup screen saver parameters
//
KGravitySetup::KGravitySetup( QWidget *parent, const char *name )
	: SetupUi( parent, name, TRUE )
{
	readSettings();

	preview->setFixedSize( 220, 170 );
	preview->setBackgroundColor( black );
#ifdef Q_WS_X11
	preview->show();    // otherwise saver does not get correct size
#endif
	saver = new KGravitySaver( preview->winId() );
;
	connect( PushButton1, SIGNAL( clicked() ), SLOT( slotOkPressed() ) );
	connect( PushButton2, SIGNAL( clicked() ), SLOT( reject() ) );
	connect( PushButton3, SIGNAL( clicked() ), SLOT( aboutPressed() ) );
	connect(  SpinBox1, SIGNAL( valueChanged(int)), saver, SLOT( updateSize(int)));
	connect( RadioButton1, SIGNAL( toggled(bool)), saver, SLOT( doStars(bool)));

}

KGravitySetup::~KGravitySetup()
{
    delete saver;
}

// read settings from config file
void KGravitySetup::readSettings()
{
	KConfig config("kssgravityrc", false, false);

	config.setGroup( "Settings" );
	QString boolval = config.readEntry( "Stars", "false" );
	if (boolval == "true") {
		RadioButton1->setDown(true);
		RadioButton1_2->setDown(false);
	} else {
		if (boolval == "false")
		{
			RadioButton1->setDown(false);
			RadioButton1_2->setDown(true);
		}
	}

	QString starammount = config.readEntry("StarSize", "75");
	SpinBox1->setValue(starammount.toInt());

}

// Ok pressed - save settings and exit
void KGravitySetup::slotOkPressed()
{
	KConfig config("kssgravityrc", false, false);
	config.setGroup( "Settings" );

	if (RadioButton1->isOn() == true)
	{
		config.writeEntry( "Stars", "true" );
	} else {
		if (RadioButton1_2->isOn() == true)
		{
			config.writeEntry( "Stars", "false" );
		}
	}
	config.writeEntry( "StarSize", QString::number(SpinBox1->value()) );

	config.sync();

	accept();
}

void KGravitySetup::aboutPressed()
{
    KMessageBox::about(this,
        i18n("<h3>Gravity</h3>\n<p>Particle Gravity Screen Saver for KDE</p>\nCopyright (c)  Ian Reinhart Geiser 2001<br>\n\n<p>KConfig code and KScreenSaver \"Setup...\" improvements by Nick Betcher <nbetcher@usinternet.com> 2001</p>"));
}
//-----------------------------------------------------------------------------


KGravitySaver::KGravitySaver( WId id ) : KScreenSaver( id )
{

	kdDebug() << "Blank" << endl;

	timer = new QTimer( this );
    	timer->start( 25, TRUE );
	setBackgroundColor( black );
        erase();
	gravity = new Gravity();
	embed(gravity);
#ifdef Q_WS_X11
	gravity->show();
#endif
	connect( timer, SIGNAL(timeout()), this, SLOT(blank()) );
}

KGravitySaver::~KGravitySaver()
{

}

// read configuration settings from config file
void KGravitySaver::readSettings()
{
// Please remove me

}

void KGravitySaver::blank()
{
	// Play gravity

	gravity->updateGL();
	timer->start( 25, TRUE );

}
Gravity::Gravity( QWidget * parent, const char * name) : QGLWidget (parent,name)
{
	rainbow=true;
	slowdown=2.0f;
	zoom=-50.0f;
	index=0;
	size = 3.95f;
//	obj = gluNewQuadric();

// This has to be here because you can't update the gravity until 'gravity' is created!
	KConfig config("kssgravityrc", false, false);
	config.setGroup( "Settings" );
	QString boolval = config.readEntry( "Stars", "false" );
	if (boolval == "true") {
		setStars(true);
	} else {
		if (boolval == "false")
		{
			setStars(false);
		}
	}

	QString starammount = config.readEntry("StarSize", "75");
	float passvalue = (starammount.toInt() / 100.0);
	setSize(passvalue);

}

Gravity::~Gravity()
{
	glDeleteTextures( 1, &texture[0] );
	gluDeleteQuadric(obj);
}

/** load the particle file */
bool Gravity::loadParticle()
{
    /* Status indicator */
    bool Status = TRUE;
	QImage buf;

    kdDebug() << "Loading: " << locate("data", "kscreensaver/particle.png") << endl;
 if (buf.load( locate("data", "kscreensaver/particle.png") ) )

        {
		tex = convertToGLFormat(buf);  // flipped 32bit RGBA
		kdDebug() << "Texture loaded: " << tex.numBytes () << endl;
	}
	else
	{
		QImage dummy( 32, 32, 32 );
  		dummy.fill( Qt::white.rgb() );
        	buf = dummy;
		tex = convertToGLFormat( buf );
	}

            /* Set the status to true */
            //Status = TRUE;
	glGenTextures(1, &texture[0]);   /* create three textures */
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	/* use linear filtering */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	/* actually generate the texture */
	glTexImage2D(GL_TEXTURE_2D, 0, 4, tex.width(), tex.height(), 0,
	GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());



    return Status;
}
/** setup the GL enviroment */
void Gravity::initializeGL ()
{

	kdDebug() << "InitGL" << endl;

	if (loadParticle())						// Jump To Texture Loading Routine
	{
    /* Enable smooth shading */
    glShadeModel( GL_SMOOTH );

    /* Set the background black */
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    /* Depth buffer setup */
    glClearDepth( 1.0f );

    /* Enables Depth Testing */
    glDisable( GL_DEPTH_TEST );

    /* Enable Blending */
    glEnable( GL_BLEND );
    /* Type Of Blending To Perform */
    glBlendFunc( GL_SRC_ALPHA, GL_ONE );


    /* Really Nice Perspective Calculations */
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
    /* Really Nice Point Smoothing */
    glHint( GL_POINT_SMOOTH_HINT, GL_NICEST );

    /* Enable Texture Mapping */
    glEnable( GL_TEXTURE_2D );
    /* Select Our Texture */
    glBindTexture( GL_TEXTURE_2D, texture[0] );

		for (loop=0;loop<MAX_PARTICLES;loop++)				// Initials All The Textures
		 {
			buildParticle(loop);
		}
	}
	else
		exit(0);
}
/** resize the gl view */
void Gravity::resizeGL ( int width, int height )
{
	kdDebug() << "ResizeGL " << width << "," <<height<< endl;
	if (height==0)							// Prevent A Divide By Zero By
	{
		height=1;						// Making Height Equal One
	}

	glViewport(0,0,width,height);					// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);					// Select The Projection Matrix
	glLoadIdentity();						// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);					// Select The Modelview Matrix
	glLoadIdentity();
}
/** paint the GL view */
void Gravity::paintGL ()
{
	//kdDebug() << "PaintGL" << endl;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
	glLoadIdentity();
						// Reset The ModelView Matrix
	transIndex++;
	//glRotatef(transIndex, 1,0,0);
	//glRotatef(transIndex, 0,1,0);
	//glRotatef(transIndex, 0,0,1);
	float xmax = 5.0;
	float ymax = 5.0;
	glTranslatef( GLfloat(xmax*sin(3.14*transIndex/360)-xmax),
			GLfloat(ymax*cos(3.14*transIndex/360)-ymax),
			0.0 );
	//glRotatef(transIndex, 0,GLfloat(zmax*cos(3.14*transIndex/360000)), GLfloat(zmax*cos(3.14*transIndex/360000)));

	for (loop=0;loop<MAX_PARTICLES;loop++)				// Loop Through All The Particles
	{
		if (particle[loop].active)				// If The Particle Is Active
		{
			float x=particle[loop].x;			// Grab Our Particle X Position
			float y=particle[loop].y;			// Grab Our Particle Y Position
			float z=particle[loop].z+zoom;			// Particle Z Pos + Zoom
    /* Select Our Texture */

                    /* Draw The Particle Using Our RGB Values,
                     * Fade The Particle Based On It's Life
                     */
			particle[loop].life=(particle[loop].index/particle[loop].indexo)*2.0f;
                    glColor4f( particle[loop].r,
                               particle[loop].g,
                               particle[loop].b,
                               particle[loop].life );

                    /* Build Quad From A Triangle Strip */
		  if( !stars )
		    glBegin( GL_TRIANGLE_STRIP );
		  else
		glBegin( GL_TRIANGLE_FAN );
			/* Top Right */
			glTexCoord2d( 1, 1 );
			glVertex3f( x + particle[loop].size, y + particle[loop].size, z );
			/* Top Left */
			glTexCoord2d( 0, 1 );
			glVertex3f( x - particle[loop].size, y + particle[loop].size, z );
			/* Bottom Right */
			glTexCoord2d( 1, 0 );
			glVertex3f( x + particle[loop].size, y - particle[loop].size, z );
			/* Bottom Left */
			glTexCoord2d( 0, 0 );
			glVertex3f( x - particle[loop].size, y - particle[loop].size, z );
		glEnd( );
			particle[loop].x=(particle[loop].xo*sin(particle[loop].index))*pow((double) particle[loop].index/particle[loop].indexo,(double) 8.0);
			particle[loop].y=(particle[loop].yo*sin(particle[loop].index))*pow((double) particle[loop].index/particle[loop].indexo,(double) 8.0);
			particle[loop].z=(particle[loop].zo*sin(particle[loop].index))*pow((double) particle[loop].index/particle[loop].indexo,(double) 8.0);
			particle[loop].index-=0.05;
			if (particle[loop].index<0.0f )			// If Particle Is Burned Out
			{
				buildParticle(loop);
			}
			// Lets stir some things up
		}
	}

	glFlush();
}
void Gravity::setSize( float newSize )
{
	size = newSize;
}
void Gravity::setStars( bool doStars )
{
	stars = doStars;
}

void KGravitySaver::updateSize(int newSize)
{
	gravity->setSize(newSize/100);
}
void KGravitySaver::doStars(bool starState)
{
	gravity->setStars(starState);
}

void Gravity::buildParticle(int loop)
{
	GLfloat colors[12][3]=
	{{1.0f,0.5f,0.5f},{1.0f,0.75f,0.5f},{1.0f,1.0f,0.5f},{0.75f,1.0f,0.5f},
	{0.5f,1.0f,0.5f},{0.5f,1.0f,0.75f},{0.5f,1.0f,1.0f},{0.5f,0.75f,1.0f},
	{0.5f,0.5f,1.0f},{0.75f,0.5f,1.0f},{1.0f,0.5f,1.0f},{1.0f,0.5f,0.75f}};
	col = ( ++col ) % 12;
	particle[loop].active=true;
	particle[loop].index=KApplication::random()%100;
	particle[loop].indexo=particle[loop].index;
	particle[loop].fade=float(KApplication::random()%100)/1000.0f+0.003f;	// Random Fade Value
	particle[loop].r=colors[col][0];			// Select Red From Color Table
	particle[loop].g=colors[col][1];			// Select Green From Color Table
	particle[loop].b=colors[col][2];			// Select Blue From Color Table
	particle[loop].size=size;
	particle[loop].x = float(KApplication::random()%100-50)*4.0;
	particle[loop].y = float(KApplication::random()%20-10)*4.0;
	particle[loop].z = float(KApplication::random()%100-50)*4.0;
	particle[loop].xo = particle[loop].x;
	if ((1+(KApplication::random() % 10) > 5))
		particle[loop].yo = particle[loop].y;
	else
		particle[loop].yo = 0.0;
	particle[loop].zo = particle[loop].z;

}

