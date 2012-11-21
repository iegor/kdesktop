//============================================================================
//
// Terence Welsh Screensaver - Euphoria
// http://www.reallyslick.com/
//
// Ported to KDE by Karl Robillard
//
/*
 * Copyright (C) 2002  Terence M. Welsh
 *
 * Euphoria is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Euphoria is distributed in the hope that it will be useful,
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
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <qtimer.h>
#include "Euphoria.h"
#include "Euphoria.moc"
#include "EuphoriaTexture.h"


#define NUMCONSTS 9
#define PIx2 6.28318530718f


//----------------------------------------------------------------------------


#include <sys/time.h>
#include <unistd.h>


// Returns the system time, in seconds.
double timeGetTime()
{
    struct timeval tp;
    gettimeofday( &tp, 0 );
    return (double)tp.tv_sec + (double)tp.tv_usec / 1000000;
}


//----------------------------------------------------------------------------


class rsVec
{
public:

    float v[3];

    rsVec() {}
    rsVec(float xx, float yy, float zz);

    void set(float xx, float yy, float zz);
    float normalize();
    float dot(rsVec);
    void cross(rsVec, rsVec);

    float & operator [] (int i) {return v[i];}
    const float & operator [] (int i) const {return v[i];}
    rsVec & operator = (const rsVec &vec)
        {v[0]=vec[0];v[1]=vec[1];v[2]=vec[2];return *this;};
    rsVec operator + (const rsVec &vec)
        {return(rsVec(v[0]+vec[0], v[1]+vec[1], v[2]+vec[2]));};
    rsVec operator - (const rsVec &vec)
        {return(rsVec(v[0]-vec[0], v[1]-vec[1], v[2]-vec[2]));};
    rsVec operator * (const float &mul)
        {return(rsVec(v[0]*mul, v[1]*mul, v[2]*mul));};
    rsVec operator / (const float &div)
        {float rec = 1.0f/div; return(rsVec(v[0]*rec, v[1]*rec, v[2]*rec));};
    rsVec & operator += (const rsVec &vec)
        {v[0]+=vec[0];v[1]+=vec[1];v[2]+=vec[2];return *this;};
    rsVec & operator -= (const rsVec &vec)
        {v[0]-=vec[0];v[1]-=vec[1];v[2]-=vec[2];return *this;};
    rsVec & operator *= (const rsVec &vec)
        {v[0]*=vec[0];v[1]*=vec[1];v[2]*=vec[2];return *this;};
    rsVec & operator *= (const float &mul)
        {v[0]*=mul;v[1]*=mul;v[2]*=mul;return *this;};
};


rsVec::rsVec(float xx, float yy, float zz){
    v[0] = xx;
    v[1] = yy;
    v[2] = zz;
}


void rsVec::set(float xx, float yy, float zz){
    v[0] = xx;
    v[1] = yy;
    v[2] = zz;
}


float rsVec::normalize(){
    float length = float(sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]));
    if(length == 0.0f){
        v[1] = 1.0f;
        return(0.0f);
    }
    float reciprocal = 1.0f / length;
    v[0] *= reciprocal;
    v[1] *= reciprocal;
    v[2] *= reciprocal;
    // Really freakin' stupid compiler bug fix for VC++ 5.0
    /*v[0] /= length;
    v[1] /= length;
    v[2] /= length;*/
    return(length);
}


float rsVec::dot(rsVec vec1){
    return(v[0] * vec1[0] + v[1] * vec1[1] + v[2] * vec1[2]);
}


void rsVec::cross(rsVec vec1, rsVec vec2){
    v[0] = vec1[1] * vec2[2] - vec2[1] * vec1[2];
    v[1] = vec1[2] * vec2[0] - vec2[2] * vec1[0];
    v[2] = vec1[0] * vec2[1] - vec2[0] * vec1[1];
}


//----------------------------------------------------------------------------


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


// Context pointer to allow many instances.
static EuphoriaWidget* _ec = 0;


class wisp
{
public:

	wisp();
	~wisp();
	void update();
	void draw();
	void drawAsBackground();


    int density;
	float*** vertices;
	float c[NUMCONSTS];     // constants
	float cr[NUMCONSTS];    // constants' radial position
	float cv[NUMCONSTS];    // constants' change velocities
	float hsl[3];
	float rgb[3];
	float hueSpeed;
	float saturationSpeed;
};


wisp::wisp()
{
	int i, j;
	float recHalfDens = 1.0f / (float(_ec->dDensity) * 0.5f);

    density = _ec->dDensity;
	vertices = new float**[density+1];
	for(i=0; i<=density; i++)
    {
		vertices[i] = new float*[density+1];
		for(j=0; j<=density; j++)
        {
			vertices[i][j] = new float[7];
			vertices[i][j][3] = float(i) * recHalfDens - 1.0f;  // x position on grid
			vertices[i][j][4] = float(j) * recHalfDens - 1.0f;  // y position on grid
			// distance squared from the center
			vertices[i][j][5] = vertices[i][j][3] * vertices[i][j][3]
				+ vertices[i][j][4] * vertices[i][j][4];
			vertices[i][j][6] = 0.0f;  // intensity
		}
	}

	// initialize constants
	for(i=0; i<NUMCONSTS; i++)
    {
		c[i] = myRandf(2.0f) - 1.0f;
		cr[i] = myRandf(PIx2);
		cv[i] = myRandf(_ec->dSpeed * 0.03f) + (_ec->dSpeed * 0.001f);
	}

	// pick color
	hsl[0] = myRandf(1.0f);
	hsl[1] = 0.1f + myRandf(0.9f);
	hsl[2] = 1.0f;
	hueSpeed = myRandf(0.1f) - 0.05f;
	saturationSpeed = myRandf(0.04f) + 0.001f;
}


wisp::~wisp()
{
	int i, j;

	for(i=0; i<=density; i++)
    {
		for(j=0; j<=density; j++)
        {
			delete[] vertices[i][j];
		}
		delete[] vertices[i];
	}
	delete[] vertices;
}


void wisp::update()
{
	int i, j;
	rsVec up, right, crossvec;
	// visibility constants
	static float viscon1 = float(_ec->dVisibility) * 0.01f;
	static float viscon2 = 1.0f / viscon1;

	// update constants
	for(i=0; i<NUMCONSTS; i++){
		cr[i] += cv[i] * _ec->elapsedTime;
		if(cr[i] > PIx2)
			cr[i] -= PIx2;
		c[i] = cos(cr[i]);
	}

	// update vertex positions
	for(i=0; i<=density; i++){
		for(j=0; j<=density; j++){
			vertices[i][j][0] = vertices[i][j][3] * vertices[i][j][3] * vertices[i][j][4] * c[0]
				+ vertices[i][j][5] * c[1] + 0.5f * c[2];
			vertices[i][j][1] = vertices[i][j][4] * vertices[i][j][4] * vertices[i][j][5] * c[3]
				+ vertices[i][j][3] * c[4] + 0.5f * c[5];
			vertices[i][j][2] = vertices[i][j][5] * vertices[i][j][5] * vertices[i][j][3] * c[6]
				+ vertices[i][j][4] * c[7] + c[8];
		}
	}

	// update vertex normals for most of mesh
	for(i=1; i<density; i++){
		for(j=1; j<density; j++){
			up.set(vertices[i][j+1][0] - vertices[i][j-1][0],
				vertices[i][j+1][1] - vertices[i][j-1][1],
				vertices[i][j+1][2] - vertices[i][j-1][2]);
			right.set(vertices[i+1][j][0] - vertices[i-1][j][0],
				vertices[i+1][j][1] - vertices[i-1][j][1],
				vertices[i+1][j][2] - vertices[i-1][j][2]);
			up.normalize();
			right.normalize();
			crossvec.cross(right, up);
			// Use depth component of normal to compute intensity
			// This way only edges of wisp are bright
			if(crossvec[2] < 0.0f)
				crossvec[2] *= -1.0f;
			vertices[i][j][6] = viscon2 * (viscon1 - crossvec[2]);
			if(vertices[i][j][6] > 1.0f)
				vertices[i][j][6] = 1.0f;
			if(vertices[i][j][6] < 0.0f)
				vertices[i][j][6] = 0.0f;
		}
	}

	// update color
	hsl[0] += hueSpeed * _ec->elapsedTime;
	if(hsl[0] < 0.0f)
		hsl[0] += 1.0f;
	if(hsl[0] > 1.0f)
		hsl[0] -= 1.0f;
	hsl[1] += saturationSpeed * _ec->elapsedTime;
	if(hsl[1] <= 0.1f){
		hsl[1] = 0.1f;
		saturationSpeed = -saturationSpeed;
	}
	if(hsl[1] >= 1.0f){
		hsl[1] = 1.0f;
		saturationSpeed = -saturationSpeed;
	}
	hsl2rgb(hsl[0], hsl[1], hsl[2], rgb[0], rgb[1], rgb[2]);
}


void wisp::draw()
{
	int i, j;

	glPushMatrix();

	if(_ec->dWireframe)
    {
		for(i=1; i<density; i++){
			glBegin(GL_LINE_STRIP);
			for(j=0; j<=density; j++){
				glColor3f(rgb[0] + vertices[i][j][6] - 1.0f, rgb[1] + vertices[i][j][6] - 1.0f, rgb[2] + vertices[i][j][6] - 1.0f);
				glTexCoord2d(vertices[i][j][3] - vertices[i][j][0], vertices[i][j][4] - vertices[i][j][1]);
				glVertex3fv(vertices[i][j]);
			}
			glEnd();
		}
		for(j=1; j<density; j++){
			glBegin(GL_LINE_STRIP);
			for(i=0; i<=density; i++){
				glColor3f(rgb[0] + vertices[i][j][6] - 1.0f, rgb[1] + vertices[i][j][6] - 1.0f, rgb[2] + vertices[i][j][6] - 1.0f);
				glTexCoord2d(vertices[i][j][3] - vertices[i][j][0], vertices[i][j][4] - vertices[i][j][1]);
				glVertex3fv(vertices[i][j]);
			}
			glEnd();
		}
	}
	else
    {
		for(i=0; i<density; i++){
			glBegin(GL_TRIANGLE_STRIP);
				for(j=0; j<=density; j++){
					glColor3f(rgb[0] + vertices[i+1][j][6] - 1.0f, rgb[1] + vertices[i+1][j][6] - 1.0f, rgb[2] + vertices[i+1][j][6] - 1.0f);
					glTexCoord2d(vertices[i+1][j][3] - vertices[i+1][j][0], vertices[i+1][j][4] - vertices[i+1][j][1]);
					glVertex3fv(vertices[i+1][j]);
					glColor3f(rgb[0] + vertices[i][j][6] - 1.0f, rgb[1] + vertices[i][j][6] - 1.0f, rgb[2] + vertices[i][j][6] - 1.0f);
					glTexCoord2d(vertices[i][j][3] - vertices[i][j][0], vertices[i][j][4] - vertices[i][j][1]);
					glVertex3fv(vertices[i][j]);
				}
			glEnd();
		}
	}

	glPopMatrix();
}


void wisp::drawAsBackground()
{
	int i, j;

	glPushMatrix();
	glTranslatef(c[0] * 0.2f, c[1] * 0.2f, 1.6f);

	if(_ec->dWireframe)
    {
		for(i=1; i<density; i++){
			glBegin(GL_LINE_STRIP);
			for(j=0; j<=density; j++){
				glColor3f(rgb[0] + vertices[i][j][6] - 1.0f, rgb[1] + vertices[i][j][6] - 1.0f, rgb[2] + vertices[i][j][6] - 1.0f);
				glTexCoord2d(vertices[i][j][3] - vertices[i][j][0], vertices[i][j][4] - vertices[i][j][1]);
				glVertex3f(vertices[i][j][3], vertices[i][j][4], vertices[i][j][6]);
			}
			glEnd();
		}
		for(j=1; j<density; j++){
			glBegin(GL_LINE_STRIP);
			for(i=0; i<=density; i++){
				glColor3f(rgb[0] + vertices[i][j][6] - 1.0f, rgb[1] + vertices[i][j][6] - 1.0f, rgb[2] + vertices[i][j][6] - 1.0f);
				glTexCoord2d(vertices[i][j][3] - vertices[i][j][0], vertices[i][j][4] - vertices[i][j][1]);
				glVertex3f(vertices[i][j][3], vertices[i][j][4], vertices[i][j][6]);
			}
			glEnd();
		}
	}
	else
    {
		for(i=0; i<density; i++){
			glBegin(GL_TRIANGLE_STRIP);
				for(j=0; j<=density; j++){
					glColor3f(rgb[0] + vertices[i+1][j][6] - 1.0f, rgb[1] + vertices[i+1][j][6] - 1.0f, rgb[2] + vertices[i+1][j][6] - 1.0f);
					glTexCoord2d(vertices[i+1][j][3] - vertices[i+1][j][0], vertices[i+1][j][4] - vertices[i+1][j][1]);
					glVertex3f(vertices[i+1][j][3], vertices[i+1][j][4], vertices[i+1][j][6]);
					glColor3f(rgb[0] + vertices[i][j][6] - 1.0f, rgb[1] + vertices[i][j][6] - 1.0f, rgb[2] + vertices[i][j][6] - 1.0f);
					glTexCoord2d(vertices[i][j][3] - vertices[i][j][0], vertices[i][j][4] - vertices[i][j][1]);
					glVertex3f(vertices[i][j][3], vertices[i][j][4], vertices[i][j][6]);
				}
			glEnd();
		}
	}

	glPopMatrix();
}


//----------------------------------------------------------------------------


EuphoriaWidget::EuphoriaWidget( QWidget* parent, const char* name )
              : QGLWidget(parent, name), texName(0), _wisps(0), _backwisps(0),
	      feedbackmap(0), feedbacktex(0)
{
    setDefaults( Regular );

    _frameTime = 1000 / 60;
    _timer = new QTimer( this );
    connect( _timer, SIGNAL(timeout()), this, SLOT(nextFrame()) );
}


EuphoriaWidget::~EuphoriaWidget()
{
	// Free memory
	if ( texName )
		glDeleteTextures( 1, &texName );
	if ( feedbacktex )
		glDeleteTextures( 1, &feedbacktex );
	delete[] _wisps;
	delete[] _backwisps;
}


void EuphoriaWidget::paintGL()
{
	int i;
	static double lastTime = timeGetTime();

	// update time
	elapsedTime = timeGetTime() - lastTime;
	lastTime += elapsedTime;

    _ec = this;

	// Update wisps
	for(i=0; i<dWisps; i++)
		_wisps[i].update();
	for(i=0; i<dBackground; i++)
		_backwisps[i].update();


	if(dFeedback)
    {
		float feedbackIntensity = float(dFeedback) / 101.0f;

		// update feedback variables
		for(i=0; i<4; i++)
        {
			fr[i] += elapsedTime * fv[i];
			if(fr[i] > PIx2)
				fr[i] -= PIx2;
		}
		f[0] = 30.0f * cos(fr[0]);
		f[1] = 0.2f * cos(fr[1]);
		f[2] = 0.2f * cos(fr[2]);
		f[3] = 0.8f * cos(fr[3]);
		for(i=0; i<3; i++)
        {
			lr[i] += elapsedTime * lv[i];
			if(lr[i] > PIx2)
				lr[i] -= PIx2;
			l[i] = cos(lr[i]);
			l[i] = l[i] * l[i];
		}

		// Create drawing area for feedback texture
		glViewport(0, 0, feedbacktexsize, feedbacktexsize);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(30.0, aspectRatio, 0.01f, 20.0f);
		glMatrixMode(GL_MODELVIEW);

		// Draw
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(feedbackIntensity, feedbackIntensity, feedbackIntensity);
		glBindTexture(GL_TEXTURE_2D, feedbacktex);
		glPushMatrix();
		glTranslatef(f[1] * l[1], f[2] * l[1], f[3] * l[2]);
		glRotatef(f[0] * l[0], 0, 0, 1);
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(-0.5f, -0.5f);
			glVertex3f(-aspectRatio*2.0f, -2.0f, 1.25f);
			glTexCoord2f(1.5f, -0.5f);
			glVertex3f(aspectRatio*2.0f, -2.0f, 1.25f);
			glTexCoord2f(-0.5f, 1.5f);
			glVertex3f(-aspectRatio*2.0f, 2.0f, 1.25f);
			glTexCoord2f(1.5f, 1.5f);
			glVertex3f(aspectRatio*2.0f, 2.0f, 1.25f);
		glEnd();
		glPopMatrix();
		glBindTexture(GL_TEXTURE_2D, texName);
		for(i=0; i<dBackground; i++)
			_backwisps[i].drawAsBackground();
		for(i=0; i<dWisps; i++)
			_wisps[i].draw();

		// readback feedback texture
		glReadBuffer(GL_BACK);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, feedbacktexsize);
		glBindTexture(GL_TEXTURE_2D, feedbacktex);
		glReadPixels(0, 0, feedbacktexsize, feedbacktexsize, GL_RGB, GL_UNSIGNED_BYTE, feedbackmap);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, feedbacktexsize, feedbacktexsize, GL_RGB, GL_UNSIGNED_BYTE, feedbackmap);

		// create regular drawing area
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(20.0, aspectRatio, 0.01f, 20.0f);
		glMatrixMode(GL_MODELVIEW);

		// Draw again
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(feedbackIntensity, feedbackIntensity, feedbackIntensity);
		glPushMatrix();
		glTranslatef(f[1] * l[1], f[2] * l[1], f[3] * l[2]);
		glRotatef(f[0] * l[0], 0, 0, 1);
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(-0.5f, -0.5f);
			glVertex3f(-aspectRatio*2.0f, -2.0f, 1.25f);
			glTexCoord2f(1.5f, -0.5f);
			glVertex3f(aspectRatio*2.0f, -2.0f, 1.25f);
			glTexCoord2f(-0.5f, 1.5f);
			glVertex3f(-aspectRatio*2.0f, 2.0f, 1.25f);
			glTexCoord2f(1.5f, 1.5f);
			glVertex3f(aspectRatio*2.0f, 2.0f, 1.25f);
		glEnd();
		glPopMatrix();

		glBindTexture(GL_TEXTURE_2D, texName);
	}
	else
		glClear(GL_COLOR_BUFFER_BIT);

	//
	for(i=0; i<dBackground; i++)
		_backwisps[i].drawAsBackground();
	for(i=0; i<dWisps; i++)
		_wisps[i].draw();

	glFlush();
}


void EuphoriaWidget::resizeGL( int w, int h )
{
    glViewport(0, 0, w, h );

	viewport[0] = 0;
	viewport[1] = 0;
	viewport[2] = w;
	viewport[3] = h;

	aspectRatio = (float) w / (float) h;

	// setup regular drawing area just in case feedback isn't used
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective(20.0, aspectRatio, 0.01, 20);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -5.0);
}


// Window initialization
void EuphoriaWidget::initializeGL()
{
    // Need to call this to setup viewport[] parameters used in
    // the next updateParameters() call
    resizeGL( width(), height() );

    updateParameters();

    _timer->start( _frameTime, true );
}


#ifdef UNIT_TEST
void EuphoriaWidget::keyPressEvent( QKeyEvent* e )
{
    if( e->key() == Qt::Key_0 ) { setDefaults( 0 ); updateParameters(); }
    if( e->key() == Qt::Key_1 ) { setDefaults( 1 ); updateParameters(); }
    if( e->key() == Qt::Key_2 ) { setDefaults( 2 ); updateParameters(); }
    if( e->key() == Qt::Key_3 ) { setDefaults( 3 ); updateParameters(); }
    if( e->key() == Qt::Key_4 ) { setDefaults( 4 ); updateParameters(); }
    if( e->key() == Qt::Key_5 ) { setDefaults( 5 ); updateParameters(); }
    if( e->key() == Qt::Key_6 ) { setDefaults( 6 ); updateParameters(); }
    if( e->key() == Qt::Key_7 ) { setDefaults( 7 ); updateParameters(); }
    if( e->key() == Qt::Key_8 ) { setDefaults( 8 ); updateParameters(); }
}
#endif


void EuphoriaWidget::nextFrame()
{
    updateGL();
    _timer->start( _frameTime, true );
}


void EuphoriaWidget::updateParameters()
{
	srand((unsigned)time(NULL));
	rand(); rand(); rand(); rand(); rand();

    elapsedTime = 0.0f;

    fr[0] = 0.0f;
    fr[1] = 0.0f;
    fr[2] = 0.0f;
    fr[3] = 0.0f;

    lr[0] = 0.0f;
    lr[1] = 0.0f;
    lr[2] = 0.0f;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glLineWidth(2.0f);

	// Commented out because smooth lines and textures don't mix on my TNT.
	// It's like it rendering in software mode
	glEnable(GL_LINE_SMOOTH);
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	if(dTexture)
    {
		int whichtex = dTexture;
		if(whichtex == 4)  // random texture
			whichtex = myRandi(3) + 1;
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		// Initialize texture
		glGenTextures(1, &texName);
		glBindTexture(GL_TEXTURE_2D, texName);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		switch(whichtex){
		case 1:
			gluBuild2DMipmaps(GL_TEXTURE_2D, 1, TEXSIZE, TEXSIZE, GL_LUMINANCE, GL_UNSIGNED_BYTE, plasmamap);
			break;
		case 2:
			gluBuild2DMipmaps(GL_TEXTURE_2D, 1, TEXSIZE, TEXSIZE, GL_LUMINANCE, GL_UNSIGNED_BYTE, stringymap);
			break;
		case 3:
			gluBuild2DMipmaps(GL_TEXTURE_2D, 1, TEXSIZE, TEXSIZE, GL_LUMINANCE, GL_UNSIGNED_BYTE, linesmap);
		}
	} else if ( texName ) {
		glDeleteTextures( 1, &texName );
		texName = 0;
	}

	if(dFeedback)
    {
		feedbacktexsize = int(pow(2.0, dFeedbacksize));
		while(feedbacktexsize > viewport[2] || feedbacktexsize > viewport[3]){
			dFeedbacksize -= 1;
			feedbacktexsize = int(pow(2.0, dFeedbacksize));
		}

		// feedback texture setup
		glEnable(GL_TEXTURE_2D);
		delete [] feedbackmap;
		feedbackmap = new unsigned char[feedbacktexsize*feedbacktexsize*3];
		glGenTextures(1, &feedbacktex);
		glBindTexture(GL_TEXTURE_2D, feedbacktex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, feedbacktexsize, feedbacktexsize, 0, GL_RGB, GL_UNSIGNED_BYTE, feedbackmap);

		// feedback velocity variable setup
		fv[0] = float(dFeedbackspeed) * (myRandf(0.025f) + 0.025f);
		fv[1] = float(dFeedbackspeed) * (myRandf(0.05f) + 0.05f);
		fv[2] = float(dFeedbackspeed) * (myRandf(0.05f) + 0.05f);
		fv[3] = float(dFeedbackspeed) * (myRandf(0.1f) + 0.1f);
		lv[0] = float(dFeedbackspeed) * (myRandf(0.0025f) + 0.0025f);
		lv[1] = float(dFeedbackspeed) * (myRandf(0.0025f) + 0.0025f);
		lv[2] = float(dFeedbackspeed) * (myRandf(0.0025f) + 0.0025f);
	} else if ( feedbacktex ) {
		glDeleteTextures( 1, &feedbacktex );
		feedbacktex = 0;
	}

	// Initialize wisps
    _ec = this;
    delete[] _wisps;
    delete[] _backwisps;
	_wisps     = new wisp[dWisps];
	_backwisps = new wisp[dBackground];
}


/**
  May be called at any time - makes no OpenGL calls.
*/
void EuphoriaWidget::setDefaults(int which)
{
	switch(which)
    {
	case Grid:
		dWisps = 4;
		dBackground = 1;
		dDensity = 25;
		dVisibility = 70;
		dSpeed = 15;
		dFeedback = 0;
		dFeedbackspeed = 1;
		dFeedbacksize = 8;
		dWireframe = 1;
		dTexture = 0;
		break;

    case Cubism:
		dWisps = 15;
		dBackground = 0;
		dDensity = 4;
		dVisibility = 15;
		dSpeed = 10;
		dFeedback = 0;
		dFeedbackspeed = 1;
		dFeedbacksize = 8;
		dWireframe = 0;
		dTexture = 0;
		break;

    case BadMath:
		dWisps = 2;
		dBackground = 2;
		dDensity = 20;
		dVisibility = 40;
		dSpeed = 30;
		dFeedback = 40;
		dFeedbackspeed = 5;
		dFeedbacksize = 8;
		dWireframe = 1;
		dTexture = 2;
		break;

    case MTheory:
		dWisps = 3;
		dBackground = 0;
		dDensity = 25;
		dVisibility = 15;
		dSpeed = 20;
		dFeedback = 40;
		dFeedbackspeed = 20;
		dFeedbacksize = 8;
		dWireframe = 0;
		dTexture = 0;
		break;

	case UHFTEM:
		dWisps = 0;
		dBackground = 3;
		dDensity = 35;
		dVisibility = 5;
		dSpeed = 50;
		dFeedback = 0;
		dFeedbackspeed = 1;
		dFeedbacksize = 8;
		dWireframe = 0;
		dTexture = 0;
		break;

    case Nowhere:
		dWisps = 0;
		dBackground = 3;
		dDensity = 30;
		dVisibility = 40;
		dSpeed = 20;
		dFeedback = 80;
		dFeedbackspeed = 10;
		dFeedbacksize = 8;
		dWireframe = 1;
		dTexture = 3;
		break;

    case Echo:
		dWisps = 3;
		dBackground = 0;
		dDensity = 25;
		dVisibility = 30;
		dSpeed = 20;
		dFeedback = 85;
		dFeedbackspeed = 30;
		dFeedbacksize = 8;
		dWireframe = 0;
		dTexture = 1;
		break;

    case Kaleidoscope:
		dWisps = 3;
		dBackground = 0;
		dDensity = 25;
		dVisibility = 40;
		dSpeed = 15;
		dFeedback = 90;
		dFeedbackspeed = 3;
		dFeedbacksize = 8;
		dWireframe = 0;
		dTexture = 0;
		break;

    case Regular:
    default:
		dWisps = 5;
		dBackground = 0;
		dDensity = 25;
		dVisibility = 35;
		dSpeed = 15;
		dFeedback = 0;
		dFeedbackspeed = 1;
		dFeedbacksize = 8;
		dWireframe = 0;
		dTexture = 2;
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
    KDE_EXPORT const char* kss_applicationName = "keuphoria.kss";
    KDE_EXPORT const char* kss_description = I18N_NOOP( "Euphoria" );
    KDE_EXPORT const char* kss_version = "1.0";

    KDE_EXPORT KScreenSaver* kss_create( WId id )
    {
        return new KEuphoriaScreenSaver( id );
    }

    KDE_EXPORT QDialog* kss_setup()
    {
        return new KEuphoriaSetup;
    }
}


//----------------------------------------------------------------------------


KEuphoriaScreenSaver::KEuphoriaScreenSaver( WId id ) : KScreenSaver( id )
{
    _effect = new EuphoriaWidget;

    readSettings();

    embed( _effect );
    _effect->show();
}


KEuphoriaScreenSaver::~KEuphoriaScreenSaver()
{
}


static int filterRandom( int n )
{
    if( (n < 0) || (n >= EuphoriaWidget::DefaultModes) )
    {
        srand((unsigned)time(NULL));
        n = rand() % EuphoriaWidget::DefaultModes;
    }
    return n;
}


void KEuphoriaScreenSaver::readSettings()
{
    KConfig* config = KGlobal::config();
    config->setGroup("Settings");

    _mode = config->readNumEntry( "Mode", EuphoriaWidget::Regular );
    _effect->setDefaults( filterRandom(_mode) );
}


/**
  Any invalid mode will select one at random.
*/
void KEuphoriaScreenSaver::setMode( int id )
{
    _mode = id;
    _effect->setDefaults( filterRandom(id) );
    _effect->updateParameters();
}


//----------------------------------------------------------------------------


#include <qlayout.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <kmessagebox.h>


static const char* defaultText[] =
{
    I18N_NOOP( "Regular" ),
    I18N_NOOP( "Grid" ),
    I18N_NOOP( "Cubism" ),
    I18N_NOOP( "Bad Math" ),
    I18N_NOOP( "M-Theory" ),
    I18N_NOOP( "UHFTEM" ), //"ultra high frequency tunneling electron microscope",
    I18N_NOOP( "Nowhere" ),
    I18N_NOOP( "Echo" ),
    I18N_NOOP( "Kaleidoscope" ),
    I18N_NOOP( "(Random)" ),
    0
};


KEuphoriaSetup::KEuphoriaSetup( QWidget* parent, const char* name )
        : KDialogBase( parent, name, true, i18n("Setup Euphoria Screen Saver"),
          Ok|Cancel|Help, Ok, true )
{
    setButtonText( Help, i18n( "A&bout" ) );

    QWidget *main = makeMainWidget();

    QHBoxLayout* top    = new QHBoxLayout(main, 0, spacingHint());
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
    preview->setFixedSize( 220, 170 );
    preview->setBackgroundColor( black );
    preview->show();    // otherwise saver does not get correct size
    _saver = new KEuphoriaScreenSaver( preview->winId() );
    top->addWidget(preview);

    // Now that we have _saver...
    modeW->setCurrentItem( _saver->mode() );    // set before we connect
    connect( modeW, SIGNAL(activated(int)), _saver, SLOT(setMode(int)) );

    setMinimumSize( sizeHint() );
}


KEuphoriaSetup::~KEuphoriaSetup()
{
    delete _saver;
}


void KEuphoriaSetup::slotHelp()
{
    KMessageBox::about(this,
        i18n("<h3>Euphoria 1.0</h3>\n<p>Copyright (c) 2002 Terence M. Welsh<br>\n<a href=\"http://www.reallyslick.com/\">http://www.reallyslick.com/</a></p>\n\n<p>Ported to KDE by Karl Robillard</p>"),
        QString::null, KMessageBox::AllowLink);
}


/**
  Ok pressed - save settings and exit
*/
void KEuphoriaSetup::slotOk()
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
// moc Euphoria.h -o Euphoria.moc
// g++ -g -DUNIT_TEST Euphoria.cpp -I/usr/lib/qt3/include -lqt -L/usr/lib/qt3/lib -lGLU -lGL

#include <qapplication.h>

int main( int argc, char** argv )
{
    QApplication app( argc, argv );

    EuphoriaWidget w;
    w.setDefaults( EuphoriaWidget::UHFTEM );
    app.setMainWidget( &w );
    w.show();

    return app.exec();
}
#endif


//EOF
