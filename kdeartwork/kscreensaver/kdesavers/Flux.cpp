//============================================================================
//
// Terence Welsh Screensaver - Flux
// http://www.reallyslick.com/
//
// Ported to KDE by Karl Robillard
//
/*
 * Copyright (C) 2002  Terence M. Welsh
 *
 * Flux is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Flux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
//============================================================================


/*
  TODO

  [ ] Regular and others are messed up after Sparkler.
      Insane seems to reset them.
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <qtimer.h>
#include "Flux.h"
#include "Flux.moc"


#define NUMCONSTS   8
#define PIx2        6.28318530718f
#define DEG2RAD     0.0174532925f


void hsl2rgb(float h, float s, float l, float &r, float &g, float &b)
{
    // hue influence
    if(h < 0.166667){  // full red, some green
        r = 1.0;
        g = h * 6.0f;
        b = 0.0;
    }
    else {
        if(h < 0.5){  // full green
            g = 1.0;
            if(h < 0.333333){  // some red
                r = 1.0f - ((h - 0.166667f) * 6.0f);
                b = 0.0;
            }
            else{  // some blue
                b = (h - 0.333333f) * 6.0f;
                r = 0.0;
            }
        }
        else{
            if(h < 0.833333){  // full blue
                b = 1.0;
                if(h < 0.666667){  // some green
                    g = 1.0f - ((h - 0.5f) * 6.0f);
                    r = 0.0;
                }
                else{  // some red
                    r = (h - 0.666667f) * 6.0f;
                    g = 0.0;
                }
            }
            else{  // full red, some blue
                r = 1.0;
                b = 1.0f - ((h - 0.833333f) * 6.0f);
                g = 0.0;
            }
        }
    }

    // saturation influence
    r = 1.0f - (s * (1.0f - r));
    g = 1.0f - (s * (1.0f - g));
    b = 1.0f - (s * (1.0f - b));

    // luminosity influence
    r *= l;
    g *= l;
    b *= l;
}


// Useful random number macros
// Don't forget to initialize with srand()
inline int myRandi(int x){
    return((rand() * x) / RAND_MAX);
}
inline float myRandf(float x){
    return(float(rand() * x) / float(RAND_MAX));
}


//----------------------------------------------------------------------------


// Flux context to allow many instances.
static FluxWidget* _fc = 0;

static int whichparticle;


// This class is poorly named.  It's actually a whole trail of particles.
class particle
{
public:

    particle();
    ~particle();
    float update(float *c);

private:

    float** vertices;
    short trails;
    short counter;
    float offset[3];
};


particle::particle()
{
    // Offsets are somewhat like default positions for the head of each
    // particle trail.  Offsets spread out the particle trails and keep
    // them from all overlapping.
    offset[0] = cos(PIx2 * float(whichparticle) / float(_fc->dParticles));
    offset[1] = float(whichparticle) / float(_fc->dParticles) - 0.5f;
    offset[2] = sin(PIx2 * float(whichparticle) / float(_fc->dParticles));
    whichparticle++;

    // Initialize memory and set initial positions out of view of the camera
    trails = _fc->dTrail;
    vertices = new float*[ trails ];

    int i;
    for(i=0; i<trails; i++)
    {
        vertices[i] = new float[5];  // 0,1,2 = position, 3 = hue, 4 = saturation
        vertices[i][0] = 0.0f;
        vertices[i][1] = 3.0f;
        vertices[i][2] = 0.0f;
        vertices[i][3] = 0.0f;
        vertices[i][4] = 0.0f;
    }

    counter = 0;
}


particle::~particle()
{
    for(int i=0; i<trails; i++)
        delete[] vertices[i];
    delete[] vertices;
}


float particle::update(float *c)
{
    int i, p, growth;
    float rgb[3];
    float cx, cy, cz;  // Containment variables
    float luminosity;
    static float expander = 1.0f + 0.0005f * float(_fc->dExpansion);
    static float blower = 0.001f * float(_fc->dWind);
    //static float otherxyz[3];
    float depth = 0;

    // Record old position
    int oldc = counter;
    float oldpos[3];
    oldpos[0] = vertices[oldc][0];
    oldpos[1] = vertices[oldc][1];
    oldpos[2] = vertices[oldc][2];

    counter ++;
    if(counter >= _fc->dTrail)
        counter = 0;

    // Here's the iterative math for calculating new vertex positions
    // first calculate limiting terms which keep vertices from constantly
    // flying off to infinity
    cx = vertices[oldc][0] * (1.0f - 1.0f / (vertices[oldc][0] * vertices[oldc][0] + 1.0f));
    cy = vertices[oldc][1] * (1.0f - 1.0f / (vertices[oldc][1] * vertices[oldc][1] + 1.0f));
    cz = vertices[oldc][2] * (1.0f - 1.0f / (vertices[oldc][2] * vertices[oldc][2] + 1.0f));
    // then calculate new positions
    vertices[counter][0] = vertices[oldc][0] + c[6] * offset[0] - cx
        + c[2] * vertices[oldc][1]
        + c[5] * vertices[oldc][2];
    vertices[counter][1] = vertices[oldc][1] + c[6] * offset[1] - cy
        + c[1] * vertices[oldc][2]
        + c[4] * vertices[oldc][0];
    vertices[counter][2] = vertices[oldc][2] + c[6] * offset[2] - cz
        + c[0] * vertices[oldc][0]
        + c[3] * vertices[oldc][1];

    // Pick a hue
    vertices[counter][3] = cx * cx + cy * cy + cz * cz;
    if(vertices[counter][3] > 1.0f)
        vertices[counter][3] = 1.0f;
    vertices[counter][3] += c[7];
    // Limit the hue (0 - 1)
    if(vertices[counter][3] > 1.0f)
        vertices[counter][3] -= 1.0f;
    if(vertices[counter][3] < 0.0f)
        vertices[counter][3] += 1.0f;
    // Pick a saturation
    vertices[counter][4] = c[0] + vertices[counter][3];
    // Limit the saturation (0 - 1)
    if(vertices[counter][4] < 0.0f)
        vertices[counter][4] = -vertices[counter][4];
    vertices[counter][4] -= float(int(vertices[counter][4]));
    vertices[counter][4] = 1.0f - (vertices[counter][4] * vertices[counter][4]);

    // Bring particles back if they escape
    if(!counter){
        if((vertices[0][0] > 1000000000.0f) || (vertices[0][0] < -1000000000.0f)
            || (vertices[0][1] > 1000000000.0f) || (vertices[0][1] < -1000000000.0f)
            || (vertices[2][2] > 1000000000.0f) || (vertices[0][2] < -1000000000.0f)){
            vertices[0][0] = myRandf(2.0f) - 1.0f;
            vertices[0][1] = myRandf(2.0f) - 1.0f;
            vertices[0][2] = myRandf(2.0f) - 1.0f;
        }
    }

    // Draw every vertex in particle trail
    p = counter;
    growth = 0;
    luminosity = _fc->lumdiff;
    for(i=0; i<_fc->dTrail; i++){
        p ++;
        if(p >= _fc->dTrail)
            p = 0;
        growth++;

        // assign color to particle
        hsl2rgb(vertices[p][3], vertices[p][4], luminosity, rgb[0], rgb[1], rgb[2]);
        glColor3fv(rgb);

        glPushMatrix();
        if(_fc->dGeometry == 1)  // Spheres
            glTranslatef(vertices[p][0], vertices[p][1], vertices[p][2]);
        else{  // Points or lights
            depth = _fc->cosCameraAngle * vertices[p][2] - _fc->sinCameraAngle * vertices[p][0];
            glTranslatef(_fc->cosCameraAngle * vertices[p][0] + _fc->sinCameraAngle
                * vertices[p][2], vertices[p][1], depth);
        }
        if(_fc->dGeometry){  // Spheres or lights
            switch(_fc->dTrail - growth){
            case 0:
                glScalef(0.259f, 0.259f, 0.259f);
                break;
            case 1:
                glScalef(0.5f, 0.5f, 0.5f);
                break;
            case 2:
                glScalef(0.707f, 0.707f, 0.707f);
                break;
            case 3:
                glScalef(0.866f, 0.866f, 0.866f);
                break;
            case 4:
                glScalef(0.966f, 0.966f, 0.966f);
            }
        }
        switch(_fc->dGeometry){
        case 0:  // Points
            switch(_fc->dTrail - growth){
            case 0:
                glPointSize(float(_fc->dSize * (depth + 200.0f) * 0.001036f));
                break;
            case 1:
                glPointSize(float(_fc->dSize * (depth + 200.0f) * 0.002f));
                break;
            case 2:
                glPointSize(float(_fc->dSize * (depth + 200.0f) * 0.002828f));
                break;
            case 3:
                glPointSize(float(_fc->dSize * (depth + 200.0f) * 0.003464f));
                break;
            case 4:
                glPointSize(float(_fc->dSize * (depth + 200.0f) * 0.003864f));
                break;
            default:
                glPointSize(float(_fc->dSize * (depth + 200.0f) * 0.004f));
            }
            glBegin(GL_POINTS);
                glVertex3f(0.0f,0.0f,0.0f);
            glEnd();
            break;
        case 1:  // Spheres
        case 2:  // Lights
            glCallList(1);
        }
        glPopMatrix();
        vertices[p][0] *= expander;
        vertices[p][1] *= expander;
        vertices[p][2] *= expander;
        vertices[p][2] += blower;
        luminosity += _fc->lumdiff;
    }

    // Find distance between new position and old position and return it
    oldpos[0] -= vertices[counter][0];
    oldpos[1] -= vertices[counter][1];
    oldpos[2] -= vertices[counter][2];
    return(float(sqrt(oldpos[0] * oldpos[0] + oldpos[1] * oldpos[1] + oldpos[2] * oldpos[2])));
}


// This class is a set of particle trails and constants that enter
// into their equations of motion.
class flux
{
public:

    flux();
    ~flux();
    void update();

    particle *particles;
    int randomize;
    float c[NUMCONSTS];     // constants
    float cv[NUMCONSTS];    // constants' change velocities
    int currentSmartConstant;
    float oldDistance;
};


flux::flux()
{
    whichparticle = 0;

    particles = new particle[_fc->dParticles];
    randomize = 1;

    float instability = _fc->dInstability;
    int i;
    for(i=0; i<NUMCONSTS; i++)
    {
        c[i] = myRandf(2.0f) - 1.0f;
        cv[i] = myRandf(0.000005f * instability * instability)
                + 0.000001f * instability * instability;
    }

    currentSmartConstant = 0;
    oldDistance = 0.0f;
}


flux::~flux()
{
    delete[] particles;
}


void flux::update()
{
    int i;

    // randomize constants
    if(_fc->dRandomize){
        randomize --;
        if(randomize <= 0){
            for(i=0; i<NUMCONSTS; i++)
                c[i] = myRandf(2.0f) - 1.0f;
            int temp = 101 - _fc->dRandomize;
            temp = temp * temp;
            randomize = temp + myRandi(temp);
        }
    }

    // update constants
    for(i=0; i<NUMCONSTS; i++){
        c[i] += cv[i];
        if(c[i] >= 1.0f){
            c[i] = 1.0f;
            cv[i] = -cv[i];
        }
        if(c[i] <= -1.0f){
            c[i] = -1.0f;
            cv[i] = -cv[i];
        }
    }

    // update all particles in this flux field
    float dist;
    for(i=0; i<_fc->dParticles; i++)
        dist = particles[i].update(c);

    // use dist from last particle to activate smart constants
    _fc->dSmart = 0;
    if(_fc->dSmart){
        const float upper = 0.4f;
        const float lower = 0.2f;
        int beSmart = 0;
        if(dist > upper && dist > oldDistance)
            beSmart = 1;
        if(dist < lower && dist < oldDistance)
            beSmart = 1;
        if(beSmart){
            cv[currentSmartConstant] = -cv[currentSmartConstant];
            currentSmartConstant ++;
            if(currentSmartConstant >= _fc->dSmart)
                currentSmartConstant = 0;
        }
        oldDistance = dist;
    }
}


//----------------------------------------------------------------------------


FluxWidget::FluxWidget( QWidget* parent, const char* name )
                      : QGLWidget(parent, name), _fluxes(0)
{
    setDefaults( Regular );

    _frameTime = 1000 / 60;
    _timer = new QTimer( this );
    connect( _timer, SIGNAL(timeout()), this, SLOT(nextFrame()) );
}


FluxWidget::~FluxWidget()
{
    // Free memory
    delete[] _fluxes;
}


void FluxWidget::paintGL()
{
    // clear the screen
    glLoadIdentity();

    if(dBlur)   // partially
    {
        int viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        float viewRatio = float(viewport[2]) / float(viewport[3]);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glColor4f(0.0f, 0.0f, 0.0f, 0.5f - (float(sqrt(sqrt(double(dBlur)))) * 0.15495f));
        glBegin(GL_TRIANGLE_STRIP);
            glVertex3f(-3.0f * viewRatio, -3.0f, 0.0f);
            glVertex3f(3.0f * viewRatio, -3.0f, 0.0f);
            glVertex3f(-3.0f * viewRatio, 3.0f, 0.0f);
            glVertex3f(3.0f * viewRatio, 3.0f, 0.0f);
        glEnd();
    }
    else  // completely
    {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    cameraAngle += 0.01f * float(dRotation);
    if(cameraAngle >= 360.0f)
        cameraAngle -= 360.0f;
    if(dGeometry == 1)  // Only rotate for spheres
        glRotatef(cameraAngle, 0.0f, 1.0f, 0.0f);
    else
    {
        cosCameraAngle = cos(cameraAngle * DEG2RAD);
        sinCameraAngle = sin(cameraAngle * DEG2RAD);
    }

    // set up blend modes for rendering particles
    switch(dGeometry)
    {
    case 0:  // Blending for points
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glEnable(GL_BLEND);
        glEnable(GL_POINT_SMOOTH);
        glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
        break;

    case 1:  // No blending for spheres, but we need z-buffering
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        break;

    case 2:  // Blending for lights
        glBlendFunc(GL_ONE, GL_ONE);
        glEnable(GL_BLEND);
    }

    // Update particles
    if( _fluxes )
    {
        _fc = this;
        int i;
        for(i=0; i<dFluxes; i++)
            _fluxes[i].update();
    }

    glFlush();
}


void FluxWidget::resizeGL( int w, int h )
{
    glViewport(0, 0, w, h );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(100.0, (float) w / (float) h, 0.01, 200);
    glTranslatef(0.0, 0.0, -2.5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


// Window initialization
void FluxWidget::initializeGL()
{
    //resizeGL( width(), height() );

    updateParameters();

    _timer->start( _frameTime, true );
}


#ifdef UNIT_TEST
void FluxWidget::keyPressEvent( QKeyEvent* e )
{
    if( e->key() == Qt::Key_0 ) { setDefaults( 0 ); updateParameters(); }
    if( e->key() == Qt::Key_1 ) { setDefaults( 1 ); updateParameters(); }
    if( e->key() == Qt::Key_2 ) { setDefaults( 2 ); updateParameters(); }
    if( e->key() == Qt::Key_3 ) { setDefaults( 3 ); updateParameters(); }
    if( e->key() == Qt::Key_4 ) { setDefaults( 4 ); updateParameters(); }
    if( e->key() == Qt::Key_5 ) { setDefaults( 5 ); updateParameters(); }
}
#endif


void FluxWidget::nextFrame()
{
    updateGL();
    _timer->start( _frameTime, true );
}


/**
  May be called at any time - makes no OpenGL calls.
*/
void FluxWidget::setDefaults( int which )
{
    switch(which)
    {
    case Hypnotic:
        dFluxes = 2;
        dParticles = 10;
        dTrail = 40;
        dGeometry = 2;
        dSize = 15;
        dRandomize = 80;
        dExpansion = 20;
        dRotation = 0;
        dWind = 40;
        dInstability = 10;
        dBlur = 30;
        break;

    case Insane:
        dFluxes = 4;
        dParticles = 30;
        dTrail = 8;
        dGeometry = 2;
        dSize = 25;
        dRandomize = 0;
        dExpansion = 80;
        dRotation = 60;
        dWind = 40;
        dInstability = 100;
        dBlur = 10;
        break;

    case Sparklers:
        dFluxes = 3;
        dParticles = 20;
        dTrail = 6;
        dGeometry = 1;
        dSize = 20;
        dComplexity = 3;
        dRandomize = 85;
        dExpansion = 60;
        dRotation = 30;
        dWind = 20;
        dInstability = 30;
        dBlur = 0;
        break;

    case Paradigm:
        dFluxes = 1;
        dParticles = 40;
        dTrail = 40;
        dGeometry = 2;
        dSize = 5;
        dRandomize = 90;
        dExpansion = 30;
        dRotation = 20;
        dWind = 10;
        dInstability = 5;
        dBlur = 10;
        break;

    case Galactic:
        dFluxes = 1;
        dParticles = 2;
        dTrail = 1500;
        dGeometry = 2;
        dSize = 10;
        dRandomize = 0;
        dExpansion = 5;
        dRotation = 25;
        dWind = 0;
        dInstability = 5;
        dBlur = 0;
        break;

    case Regular:
    default:
        dFluxes = 1;
        dParticles = 20;
        dTrail = 40;
        dGeometry = 2;
        dSize = 15;
        dRandomize = 0;
        dExpansion = 40;
        dRotation = 30;
        dWind = 20;
        dInstability = 20;
        dBlur = 0;
        break;
    }
}


/**
  Called after dGeometry, dTrail, or dFluxes is changed
  (such as with setDefaults).
*/
void FluxWidget::updateParameters()
{
    int i, j;
    float x, y, temp;

    srand((unsigned)time(NULL));
    rand(); rand(); rand(); rand(); rand();

    cameraAngle = 0.0f;

    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(dGeometry == 0)
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_COLOR_MATERIAL);
        glDisable(GL_TEXTURE_2D);

        glEnable(GL_POINT_SMOOTH);
        //glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    }
    else if(dGeometry == 1)      // Spheres and their lighting
    {
        glNewList(1, GL_COMPILE);
        GLUquadricObj* qobj = gluNewQuadric();
        gluSphere(qobj, 0.005f * dSize, dComplexity + 2, dComplexity + 1);
        gluDeleteQuadric(qobj);
        glEndList();

        glDisable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        float ambient[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        float diffuse[4] = {1.0f, 1.0f, 1.0f, 0.0f};
        float specular[4] = {1.0f, 1.0f, 1.0f, 0.0f};
        float position[4] = {500.0f, 500.0f, 500.0f, 0.0f};

        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
        glLightfv(GL_LIGHT0, GL_POSITION, position);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    }
    else if(dGeometry == 2)      // Init lights
    {
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
                lightTexture[i][j] = (unsigned char) (255.0f * temp * temp);
            }
        }

        glDisable(GL_LIGHTING);
        glDisable(GL_COLOR_MATERIAL);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 1);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, 1, LIGHTSIZE, LIGHTSIZE, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, lightTexture);

        temp = float(dSize) * 0.005f;
        glNewList(1, GL_COMPILE);
            glBindTexture(GL_TEXTURE_2D, 1);
            glBegin(GL_TRIANGLES);
                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(-temp, -temp, 0.0f);
                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(temp, -temp, 0.0f);
                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(temp, temp, 0.0f);
                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(-temp, -temp, 0.0f);
                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(temp, temp, 0.0f);
                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(-temp, temp, 0.0f);
            glEnd();
        glEndList();
    }

    // Initialize luminosity difference
    lumdiff = 1.0f / float(dTrail);

    _fc = this;
    delete[] _fluxes;
    _fluxes = new flux[dFluxes];
}


//----------------------------------------------------------------------------


#ifndef UNIT_TEST
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>


// libkscreensaver interface
extern "C"
{
    KDE_EXPORT const char* kss_applicationName = "kflux.kss";
    KDE_EXPORT const char* kss_description = I18N_NOOP( "Flux" );
    KDE_EXPORT const char* kss_version = "1.0";

    KDE_EXPORT KScreenSaver* kss_create( WId id )
    {
        return new KFluxScreenSaver( id );
    }

    KDE_EXPORT QDialog* kss_setup()
    {
        return new KFluxSetup;
    }
}


//----------------------------------------------------------------------------


KFluxScreenSaver::KFluxScreenSaver( WId id ) : KScreenSaver( id )
{
    _flux = new FluxWidget;

    readSettings();

    embed( _flux );
    _flux->show();
}


KFluxScreenSaver::~KFluxScreenSaver()
{
}


static int filterRandom( int n )
{
    if( (n < 0) || (n >= FluxWidget::DefaultModes) )
    {
        srand((unsigned)time(NULL));
        n = rand() % FluxWidget::DefaultModes;
    }
    return n;
}


void KFluxScreenSaver::readSettings()
{
    KConfig* config = KGlobal::config();
    config->setGroup("Settings");

    _mode = config->readNumEntry( "Mode", FluxWidget::Regular );
    _flux->setDefaults( filterRandom(_mode) );
}


/**
  Any invalid mode will select one at random.
*/
void KFluxScreenSaver::setMode( int id )
{
    _mode = id;
    _flux->setDefaults( filterRandom(id) );
    _flux->updateParameters();
}


//----------------------------------------------------------------------------


#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <kbuttonbox.h>
#include <kmessagebox.h>


static const char* defaultText[] =
{
    I18N_NOOP( "Regular" ),
    I18N_NOOP( "Hypnotic" ),
    I18N_NOOP( "Insane" ),
    I18N_NOOP( "Sparklers" ),
    I18N_NOOP( "Paradigm" ),
    I18N_NOOP( "Galactic" ),
    I18N_NOOP( "(Random)" ),
    0
};


KFluxSetup::KFluxSetup( QWidget* parent, const char* name )
    : KDialogBase( parent, name, true, i18n( "Setup Flux Screen Saver" ),
      Ok|Cancel|Help, Ok, true )
{
    setButtonText( Help, i18n( "A&bout" ) );
    QWidget *main = makeMainWidget();

    QHBoxLayout* top = new QHBoxLayout( main, 0, spacingHint() );
    QVBoxLayout* leftCol = new QVBoxLayout;
    top->addLayout( leftCol );

    // Parameters
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
    _saver = new KFluxScreenSaver( preview->winId() );
    top->addWidget(preview);

    // Now that we have _saver...
    modeW->setCurrentItem( _saver->mode() );    // set before we connect
    connect( modeW, SIGNAL(activated(int)), _saver, SLOT(setMode(int)) );
}


KFluxSetup::~KFluxSetup()
{
    delete _saver;
}


void KFluxSetup::slotHelp()
{
    KMessageBox::about(this,
        i18n("<h3>Flux 1.0</h3>\n<p>Copyright (c) 2002 Terence M. Welsh<br>\n<a href=\"http://www.reallyslick.com/\">http://www.reallyslick.com/</a></p>\n\n<p>Ported to KDE by Karl Robillard</p>"),
        QString::null, KMessageBox::AllowLink);
}


/**
  Ok pressed - save settings and exit
*/
void KFluxSetup::slotOk()
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
// moc Flux.h -o Flux.moc
// g++ -g -DUNIT_TEST Flux.cpp -I/usr/lib/qt3/include -lqt -L/usr/lib/qt3/lib -lGLU -lGL

#include <qapplication.h>

int main( int argc, char** argv )
{
    QApplication app( argc, argv );

    FluxWidget w;
    w.setDefaults( FluxWidget::Sparklers );
    app.setMainWidget( &w );
    w.show();

    return app.exec();
}
#endif


//EOF
