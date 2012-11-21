//============================================================================
//
// Terence Welsh Screensaver - Solar Winds
// http://www.reallyslick.com/
//
// Ported to KDE by Karl Robillard
//
/*
 * Copyright (C) 2002  Terence M. Welsh
 *
 * Solar Winds is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Solar Winds is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
//============================================================================


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <qtimer.h>
#include "SolarWinds.h"
#include "SolarWinds.moc"


#define NUMCONSTS   9
#define PIx2        6.28318530718f
#define DEG2RAD     0.0174532925f


// Useful random number macro
// Don't forget to initialize with srand()

// This is the original myRandf() which does not work on Linux.
// I grabed the inline version from Flux.cpp.
//#define myRandf(x) (float(rand()) * (x * 0.0000305185095f))

inline float myRandf(float x){
    return(float(rand() * x) / float(RAND_MAX));
}


// Context pointer to allow many instances.
static SWindsWidget* _ec = 0;


struct Emitter
{
    float x, y, z;
};


struct Particle
{
    float x, y, z;
    float r, g, b;
};


class wind
{
public:

    wind();
    ~wind();
    void update();

    Emitter* emitters;
    Particle* particles;
    int** linelist;
    int* lastparticle;
    int whichparticle;
    int partCount;
    int emitCount;
    bool useLines;
    float c[NUMCONSTS];
    float ct[NUMCONSTS];
    float cv[NUMCONSTS];
};


wind::wind()
{
    int i;

    partCount = _ec->dParticles;
    emitCount = _ec->dEmitters;
    useLines  = (_ec->dGeometry == 2);

    emitters = new Emitter[emitCount];
    for(i=0; i<emitCount; i++)
    {
        emitters[i].x = myRandf(60.0f) - 30.0f;
        emitters[i].y = myRandf(60.0f) - 30.0f;
        emitters[i].z = myRandf(30.0f) - 15.0f;
    }

    particles = new Particle[partCount];
    for(i=0; i<partCount; i++)
    {
        particles[i].x = 0.0f;
        particles[i].y = 0.0f;
        particles[i].z = 100.0f;  // start particles behind viewer
    }

    whichparticle = 0;

    if( useLines )
    {
        linelist = new int*[partCount];
        for(i=0; i<partCount; i++)
        {
            linelist[i] = new int[2];
            linelist[i][0] = -1;
            linelist[i][1] = -1;
        }
        lastparticle = new int[emitCount];
        for(i=0; i<emitCount; i++)
            lastparticle[i] = i;
    }

    float windspeed = (float) (_ec->dWindspeed);
    for(i=0; i<NUMCONSTS; i++)
    {
        ct[i] = myRandf(PIx2);
        cv[i] = myRandf(0.00005f * windspeed * windspeed)
                + 0.00001f * windspeed * windspeed;
        //printf( "KR ct %g cv %g\n", ct[i], cv[i] );
    }
}


wind::~wind()
{
    delete[] emitters;
    delete[] particles;

    if( useLines )
    {
        int i;
        for(i=0; i<partCount; i++)
            delete[] linelist[i];
        delete[] linelist;
        delete[] lastparticle;
    }
}


void wind::update()
{
    int i;
    float x, y, z;
    float temp;
    float particleSpeed = (float) _ec->dParticlespeed;

    float evel = float(_ec->dEmitterspeed) * 0.01f;
    float pvel = particleSpeed * 0.01f;
    float pointsize = 0.04f * _ec->dSize;
    float linesize = 0.005f * _ec->dSize;

    // update constants
    for(i=0; i<NUMCONSTS; i++)
    {
        ct[i] += cv[i];
        if(ct[i] > PIx2)
            ct[i] -= PIx2;
        c[i] = cos(ct[i]);
    }

    // calculate emissions
    for(i=0; i<emitCount; i++)
    {
        emitters[i].z += evel;  // emitter moves toward viewer
        if(emitters[i].z > 15.0f)
        {
            // reset emitter
            emitters[i].x = myRandf(60.0f) - 30.0f;
            emitters[i].y = myRandf(60.0f) - 30.0f;
            emitters[i].z = -15.0f;
        }

        particles[whichparticle].x = emitters[i].x;
        particles[whichparticle].y = emitters[i].y;
        particles[whichparticle].z = emitters[i].z;

        if( useLines )
        {
            // link particles to form lines
            if(linelist[whichparticle][0] >= 0)
                linelist[linelist[whichparticle][0]][1] = -1;
            linelist[whichparticle][0] = -1;
            if(emitters[i].z == -15.0f)
                linelist[whichparticle][1] = -1;
            else
                linelist[whichparticle][1] = lastparticle[i];
            linelist[lastparticle[i]][0] = whichparticle;
            lastparticle[i] = whichparticle;
        }

        whichparticle++;
        if(whichparticle >= partCount)
            whichparticle = 0;
    }

    // calculate particle positions and colors
    // first modify constants that affect colors
    c[6] *= 9.0f / particleSpeed;
    c[7] *= 9.0f / particleSpeed;
    c[8] *= 9.0f / particleSpeed;
    // then update each particle
    for(i=0; i<partCount; i++)
    {
        Particle* part = particles + i;

        // store old positions
        x = part->x;
        y = part->y;
        z = part->z;

        // make new positins
        part->x = x + (c[0] * y + c[1] * z) * pvel;
        part->y = y + (c[2] * z + c[3] * x) * pvel;
        part->z = z + (c[4] * x + c[5] * y) * pvel;

        // calculate colors
        part->r = fabs((part->x - x) * c[6]);
        part->g = fabs((part->y - y) * c[7]);
        part->b = fabs((part->z - z) * c[8]);

        // clamp colors
        if( part->r > 1.0f )
            part->r = 1.0f;
        if( part->g > 1.0f )
            part->g = 1.0f;
        if( part->b > 1.0f )
            part->b = 1.0f;
    }

    // draw particles
    switch(_ec->dGeometry)
    {
    case 0:  // lights
        for(i=0; i<partCount; i++)
        {
            glColor3fv(&particles[i].r);
            glPushMatrix();
                glTranslatef(particles[i].x, particles[i].y, particles[i].z);
                glCallList(1);
            glPopMatrix();
#if 0
            if( i == 0 )
                printf( "KR %d %g %g %g\n", i,
                    particles[i].x, particles[i].y, particles[i].z);
#endif
        }
        break;

    case 1:  // points
        for(i=0; i<partCount; i++)
        {
            temp = particles[i].z + 40.0f;
            if(temp < 0.01f)
                temp = 0.01f;
            glPointSize(pointsize * temp);

            glBegin(GL_POINTS);
                glColor3fv(&particles[i].r);
                glVertex3fv(&particles[i].x);
            glEnd();
        }
        break;

    case 2:  // lines
        for(i=0; i<partCount; i++)
        {
            temp = particles[i].z + 40.0f;
            if(temp < 0.01f)
                temp = 0.01f;
            glLineWidth(linesize * temp);

            glBegin(GL_LINES);
            if(linelist[i][1] >= 0)
            {
                glColor3fv(&particles[i].r);
                if(linelist[i][0] == -1)
                    glColor3f(0.0f, 0.0f, 0.0f);
                glVertex3fv(&particles[i].x);

                glColor3fv(&particles[linelist[i][1]].r);
                if(linelist[linelist[i][1]][1] == -1)
                    glColor3f(0.0f, 0.0f, 0.0f);
                glVertex3fv(&particles[linelist[i][1]].x);
            }
            glEnd();
        }
    }
}


//----------------------------------------------------------------------------


SWindsWidget::SWindsWidget( QWidget* parent, const char* name )
                      : QGLWidget(parent, name), _winds(0)
{
    setDefaults( Regular );

    _frameTime = 1000 / 60;
    _timer = new QTimer( this );
    connect( _timer, SIGNAL(timeout()), this, SLOT(nextFrame()) );
}


SWindsWidget::~SWindsWidget()
{
    // Free memory
    delete[] _winds;
}


void SWindsWidget::paintGL()
{
    glLoadIdentity();

    if( ! dBlur )
    {
        glClear(GL_COLOR_BUFFER_BIT);
    }
    else
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0f, 0.0f, 0.0f, 0.5f - (float(dBlur) * 0.0049f));

        glBegin(GL_QUADS);
            glVertex3f(-40.0f, -17.0f, 0.0f);
            glVertex3f(40.0f, -17.0f, 0.0f);
            glVertex3f(40.0f, 17.0f, 0.0f);
            glVertex3f(-40.0f, 17.0f, 0.0f);
        glEnd();

        if(!dGeometry)
            glBlendFunc(GL_ONE, GL_ONE);
        else
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);  // Necessary for point and line smoothing (I don't know why)
                // Maybe it's just my video card...
    }

    // You should need to draw twice if using blur, once to each buffer.
    // But wglSwapLayerBuffers appears to copy the back to the
    // front instead of just switching the pointers to them.  It turns
    // out that both NVidia and 3dfx prefer to use PFD_SWAP_COPY instead
    // of PFD_SWAP_EXCHANGE in the PIXELFORMATDESCRIPTOR.  I don't know why...
    // So this may not work right on other platforms or all video cards.

    // Update surfaces
    if( _winds )
    {
        _ec = this;
        int i;
        for(i=0; i<dWinds; i++)
            _winds[i].update();
    }

    glFlush();
}


void SWindsWidget::resizeGL( int w, int h )
{
    glViewport(0, 0, w, h );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0, (float) w / (float) h, 1.0, 10000);
    glTranslatef(0.0, 0.0, -15.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


// Window initialization
void SWindsWidget::initializeGL()
{
    updateParameters();
    _timer->start( _frameTime, true );
}


#ifdef UNIT_TEST
void SWindsWidget::keyPressEvent( QKeyEvent* e )
{
    if( e->key() == Qt::Key_0 ) { setDefaults( 0 ); updateParameters(); }
    if( e->key() == Qt::Key_1 ) { setDefaults( 1 ); updateParameters(); }
    if( e->key() == Qt::Key_2 ) { setDefaults( 2 ); updateParameters(); }
    if( e->key() == Qt::Key_3 ) { setDefaults( 3 ); updateParameters(); }
    if( e->key() == Qt::Key_4 ) { setDefaults( 4 ); updateParameters(); }
    if( e->key() == Qt::Key_5 ) { setDefaults( 5 ); updateParameters(); }
}
#endif


void SWindsWidget::nextFrame()
{
    updateGL();
    _timer->start( _frameTime, true );
}


void SWindsWidget::updateParameters()
{
    int i, j;
    float x, y, temp;

    srand((unsigned)time(NULL));
    rand(); rand(); rand(); rand(); rand();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if(!dGeometry)
        glBlendFunc(GL_ONE, GL_ONE);
    else
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);  // Necessary for point and line smoothing (I don't know why)
    glEnable(GL_BLEND);

    if( ! dGeometry )
    {
        // Init lights
        for(i=0; i<LIGHTSIZE; i++)
        {
            for(j=0; j<LIGHTSIZE; j++)
            {
                x = float(i - LIGHTSIZE / 2) / float(LIGHTSIZE / 2);
                y = float(j - LIGHTSIZE / 2) / float(LIGHTSIZE / 2);
                temp = 1.0f - float(sqrt((x * x) + (y * y)));
                if(temp > 1.0f)
                    temp = 1.0f;
                if(temp < 0.0f)
                    temp = 0.0f;
                lightTexture[i][j] = (unsigned char) (255.0f * temp);
            }
        }

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 1);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, 1, LIGHTSIZE, LIGHTSIZE, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, lightTexture);

        temp = 0.02f * dSize;
        glNewList(1, GL_COMPILE);
            glBindTexture(GL_TEXTURE_2D, 1);
            glBegin(GL_TRIANGLE_STRIP);
                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(-temp, -temp, 0.0f);
                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(temp, -temp, 0.0f);
                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(-temp, temp, 0.0f);
                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(temp, temp, 0.0f);
            glEnd();
        glEndList();
    }
    else if(dGeometry == 1)
    {
        // init point smoothing
        glEnable(GL_POINT_SMOOTH);
        glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

        glDisable(GL_TEXTURE_2D);
    }
    else if(dGeometry == 2)
    {
        // init line smoothing
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

        glDisable(GL_TEXTURE_2D);
    }


    // Initialize surfaces
    _ec = this;
    delete[] _winds;
    _winds = new wind[dWinds];
}


/**
  May be called at any time - makes no OpenGL calls.
*/
void SWindsWidget::setDefaults(int which)
{
    switch(which)
    {
    case CosmicStrings:
        dWinds = 1;
        dEmitters = 50;
        dParticles = 3000;
        dGeometry = 2;
        dSize = 20;
        dWindspeed = 10;
        dEmitterspeed = 10;
        dParticlespeed = 10;
        dBlur = 10;
        break;

    case ColdPricklies:
        dWinds = 1;
        dEmitters = 300;
        dParticles = 3000;
        dGeometry = 2;
        dSize = 5;
        dWindspeed = 20;
        dEmitterspeed = 100;
        dParticlespeed = 15;
        dBlur = 70;
        break;

    case SpaceFur:
        dWinds = 2;
        dEmitters = 400;
        dParticles = 1600;
        dGeometry = 2;
        dSize = 15;
        dWindspeed = 20;
        dEmitterspeed = 15;
        dParticlespeed = 10;
        dBlur = 0;
        break;

    case Jiggly:
        dWinds = 1;
        dEmitters = 40;
        dParticles = 1200;
        dGeometry = 1;
        dSize = 20;
        dWindspeed = 100;
        dEmitterspeed = 20;
        dParticlespeed = 4;
        dBlur = 50;
        break;

    case Undertow:
        dWinds = 1;
        dEmitters = 400;
        dParticles = 1200;
        dGeometry = 0;
        dSize = 40;
        dWindspeed = 20;
        dEmitterspeed = 1;
        dParticlespeed = 100;
        dBlur = 50;
        break;

    case Regular:
    default:
        dWinds = 1;
        dEmitters = 30;
        dParticles = 2000;
        dGeometry = 0;
        dSize = 50;
        dWindspeed = 20;
        dEmitterspeed = 15;
        dParticlespeed = 10;
        dBlur = 40;
        break;
    }
}


//----------------------------------------------------------------------------


#ifndef UNIT_TEST
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>


// libkscreensaver interface
extern "C"
{
    KDE_EXPORT const char* kss_applicationName = "ksolarwinds.kss";
    KDE_EXPORT const char* kss_description = I18N_NOOP( "Solar Winds" );
    KDE_EXPORT const char* kss_version = "1.0";

    KDE_EXPORT KScreenSaver* kss_create( WId id )
    {
        return new KSWindsScreenSaver( id );
    }

    KDE_EXPORT QDialog* kss_setup()
    {
        return new KSWindsSetup;
    }
}


//----------------------------------------------------------------------------


KSWindsScreenSaver::KSWindsScreenSaver( WId id ) : KScreenSaver( id )
{
    _flux = new SWindsWidget;

    readSettings();

    embed( _flux );
    _flux->show();
}


KSWindsScreenSaver::~KSWindsScreenSaver()
{
}


static int filterRandom( int n )
{
    if( (n < 0) || (n >= SWindsWidget::DefaultModes) )
    {
        srand((unsigned)time(NULL));
        n = rand() % SWindsWidget::DefaultModes;
    }
    return n;
}


void KSWindsScreenSaver::readSettings()
{
    KConfig* config = KGlobal::config();
    config->setGroup("Settings");

    _mode = config->readNumEntry( "Mode", SWindsWidget::Regular );
    _flux->setDefaults( filterRandom(_mode) );
}


/**
  Any invalid mode will select one at random.
*/
void KSWindsScreenSaver::setMode( int id )
{
    _mode = id;
    _flux->setDefaults( filterRandom(id) );
    _flux->updateParameters();
}


//----------------------------------------------------------------------------


#include <qlayout.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <kmessagebox.h>


static const char* defaultText[] =
{
    I18N_NOOP( "Regular" ),
    I18N_NOOP( "Cosmic Strings" ),
    I18N_NOOP( "Cold Pricklies" ),
    I18N_NOOP( "Space Fur" ),
    I18N_NOOP( "Jiggly" ),
    I18N_NOOP( "Undertow" ),
    I18N_NOOP( "(Random)" ),
    0
};


KSWindsSetup::KSWindsSetup( QWidget* parent, const char* name )
        : KDialogBase( parent, name, true, i18n( "Setup Solar Wind" ),
          Ok|Cancel|Help, Ok, true )
{
    setButtonText( Help, i18n( "A&bout" ) );
    QWidget *main = makeMainWidget();

    QHBoxLayout* top = new QHBoxLayout( main, 0, spacingHint() );

    QVBoxLayout* leftCol = new QVBoxLayout;
    top->addLayout( leftCol );

    QLabel* label = new QLabel( i18n("Mode:"), main );
    leftCol->addWidget( label );

    modeW = new QComboBox( main );
    int i = 0;
    while (defaultText[i])
        modeW->insertItem( i18n(defaultText[i++]) );
    leftCol->addWidget( modeW );

    leftCol->addStretch();

    // Preview
    QWidget* preview;
    preview = new QWidget( main );
    preview->setFixedSize( 220, 165 );
    preview->setBackgroundColor( black );
    preview->show();    // otherwise saver does not get correct size
    _saver = new KSWindsScreenSaver( preview->winId() );
    top->addWidget(preview);

    // Now that we have _saver...
    modeW->setCurrentItem( _saver->mode() );    // set before we connect
    connect( modeW, SIGNAL(activated(int)), _saver, SLOT(setMode(int)) );
}


KSWindsSetup::~KSWindsSetup()
{
    delete _saver;
}


void KSWindsSetup::slotHelp()
{
    KMessageBox::about(this,
        i18n("<h3>Solar Winds 1.0</h3>\n<p>Copyright (c) 2002 Terence M. Welsh<br>\n<a href=\"http://www.reallyslick.com/\">http://www.reallyslick.com/</a></p>\n\n<p>Ported to KDE by Karl Robillard</p>"),
        QString::null, KMessageBox::AllowLink);
}


/**
  Ok pressed - save settings and exit
*/
void KSWindsSetup::slotOk()
{
    KConfig* config = KGlobal::config();
    config->setGroup("Settings");

    QString val;
    val.setNum( modeW->currentItem() );
    config->writeEntry("Mode", val );

    config->sync();
    accept();
}
#endif


//----------------------------------------------------------------------------


#ifdef UNIT_TEST
// moc SolarWinds.h -o SolarWinds.moc
// g++ -g -DUNIT_TEST SolarWinds.cpp -I/usr/lib/qt3/include -lqt -L/usr/lib/qt3/lib -lGLU -lGL

#include <qapplication.h>

int main( int argc, char** argv )
{
    QApplication app( argc, argv );

    SWindsWidget w;
    app.setMainWidget( &w );
    w.show();

    return app.exec();
}
#endif


//EOF
