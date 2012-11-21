//============================================================================
//
// KRotation screen saver for KDE
//
// The screen saver displays a physically realistic simulation of a force free
// rotating asymmetric body.  The equations of motion for such a rotation, the
// Euler equations, are integrated numerically by the Runge-Kutta method.
//
// Developed by Georg Drenkhahn, georg-d@users.sourceforge.net
//
// $Id: rotation.cpp 501235 2006-01-22 14:06:24Z mueller $
//
/*
 * Copyright (C) 2004 Georg Drenkhahn
 *
 * KRotation is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation.
 *
 * KRotation is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02110-1301 USA
 */
//============================================================================

// std. C++ headers
#include <cstdlib>
// STL
#include <deque>
// Qt headers
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qtooltip.h>
// KDE headers
#include <klocale.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>

#include "sspreviewarea.h"

// rotation.moc includes rotation.h
#include "rotation.moc"

/** Version number of this screen saver */
#define KROTATION_VERSION "1.1"

// libkscreensaver interface
extern "C"
{
   /** application name for the libkscreensaver interface */
   KDE_EXPORT const char *kss_applicationName = "krotation.kss";
   /** application description for the libkscreensaver interface */
   KDE_EXPORT const char *kss_description =
   I18N_NOOP("Simulation of a force free rotating asymmetric body");
   /** application version for the libkscreensaver interface */
   KDE_EXPORT const char *kss_version = KROTATION_VERSION;

   /** function to create screen saver object */
   KDE_EXPORT  KScreenSaver* kss_create(WId id)
   {
      return new KRotationSaver(id);
   }

   /** function to create setup dialog for screen saver */
   KDE_EXPORT QDialog* kss_setup()
   {
      return new KRotationSetup();
   }
}

//-----------------------------------------------------------------------------
// EulerOdeSolver implementation
//-----------------------------------------------------------------------------

EulerOdeSolver::EulerOdeSolver(
   const double &t_,
   const double &dt_,
   const double &A_,
   const double &B_,
   const double &C_,
   std::valarray<double> &y_,
   const double &eps_)
   : RkOdeSolver<double>(t_,y_,dt_,eps_),
     A(A_), B(B_), C(C_)
{
}

std::valarray<double> EulerOdeSolver::f(
   const double &x,
   const std::valarray<double> &y) const
{
   // unused
   (void)x;

   // vec omega in body coor. sys.: omega_body = (p, q, r)
   const vec3<double> omega_body(y[std::slice(0,3,1)]);
   // body unit vectors in fixed frame coordinates
   const vec3<double> e1(y[std::slice(3,3,1)]);
   const vec3<double> e2(y[std::slice(6,3,1)]);
   const vec3<double> e3(y[std::slice(9,3,1)]);

   // don't use "const vec3<double>&" here because slice_array must be
   // value-copied to vec3<double>.

   // vec omega in global fixed coor. sys.
   vec3<double> omega(
      omega_body[0] * e1
      + omega_body[1] * e2
      + omega_body[2] * e3);

   // return vector y'
   std::valarray<double> ypr(y.size());

   // omega_body'
   ypr[0] = -(C-B)/A * omega_body[1] * omega_body[2]; // p'
   ypr[1] = -(A-C)/B * omega_body[2] * omega_body[0]; // q'
   ypr[2] = -(B-A)/C * omega_body[0] * omega_body[1]; // r'

   // e1', e2', e3'
   ypr[std::slice(3,3,1)] = vec3<double>::crossprod(omega, e1);
   ypr[std::slice(6,3,1)] = vec3<double>::crossprod(omega, e2);
   ypr[std::slice(9,3,1)] = vec3<double>::crossprod(omega, e3);

   return ypr;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Rotation: screen saver widget
//-----------------------------------------------------------------------------

RotationGLWidget::RotationGLWidget(
   QWidget* parent, const char* name,
   const vec3<double>& _omega,
   const std::deque<vec3<double> >& e1_,
   const std::deque<vec3<double> >& e2_,
   const std::deque<vec3<double> >& e3_,
   const vec3<double>& J)
   : QGLWidget(parent, name),
     eyeR(25), eyeTheta(1), eyePhi(M_PI*0.25),
     boxSize(1,1,1),
     fixedAxses(0),
     bodyAxses(0),
     lightR(10), lightTheta(M_PI/4), lightPhi(0),
     bodyAxsesLength(6),
     fixedAxsesLength(8),
     omega(_omega),
     e1(e1_),
     e2(e2_),
     e3(e3_)
{
   // set up initial rotation matrix as unit matrix, only non-constant elements
   // are set later on
   for (int i=0; i<16; i++)
      rotmat[i] = ((i%5)==0) ? 1:0;

   // Set the box sizes from the momenta of inertia.  J is the 3 vector with
   // momenta of inertia with respect to the 3 figure axes.

   // the default values must be valid so that w,h,d are real!
   GLfloat
      x2 = 6.0*(-J[0] + J[1] + J[2]),
      y2 = 6.0*( J[0] - J[1] + J[2]),
      z2 = 6.0*( J[0] + J[1] - J[2]);

   if (x2>=0 && y2>=0 && z2>=0)
   {
      boxSize = vec3<double>(sqrt(x2), sqrt(y2), sqrt(z2));
   }
   else
   {
      kdDebug() << "parameter error" << endl;
      boxSize = vec3<double>(1, 1, 1);
   }
}

/* --------- protected methods ----------- */

void RotationGLWidget::initializeGL(void)
{
   qglClearColor(QColor(black)); // set color to clear the background

   glClearDepth(1);             // depth buffer setup
   glEnable(GL_DEPTH_TEST);     // depth testing
   glDepthFunc(GL_LEQUAL);      // type of depth test

   glShadeModel(GL_SMOOTH);     // smooth color shading in poygons

   // nice perspective calculation
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

   // set up the light
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   // set positon of light0
   GLfloat lightPos[4]=
      {lightR * sin(lightTheta) * sin(lightPhi),
       lightR * sin(lightTheta) * cos(lightPhi),
       lightR * cos(lightTheta), 1.};
   glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

   // enable setting the material colour by glColor()
   glEnable(GL_COLOR_MATERIAL);

   // set up display lists

   if (fixedAxses == 0)
      fixedAxses = glGenLists(1); // list to be returned
   glNewList(fixedAxses, GL_COMPILE);

   // fixed coordinate system axes

   glPushMatrix();
   glLoadIdentity();

   // z-axis, blue
   qglColor(QColor(blue));
   myGlArrow(fixedAxsesLength, 0.5f, 0.03f, 0.1f);

   // x-axis, red
   qglColor(QColor(red));
   glRotatef(90, 0, 1, 0);

   myGlArrow(fixedAxsesLength, 0.5f, 0.03f, 0.1f);

   // y-axis, green
   qglColor(QColor(green));
   glLoadIdentity();
   glRotatef(-90, 1, 0, 0);
   myGlArrow(fixedAxsesLength, 0.5f, 0.03f, 0.1f);

   glPopMatrix();
   glEndList();
   // end of axes object list


   // box and box-axses
   if (bodyAxses == 0)
      bodyAxses = glGenLists(1); // list to be returned
   glNewList(bodyAxses, GL_COMPILE);

   // z-axis, blue
   qglColor(QColor(blue));
   myGlArrow(bodyAxsesLength, 0.5f, 0.03f, 0.1f);

   // x-axis, red
   qglColor(QColor(red));
   glPushMatrix();
   glRotatef(90, 0, 1, 0);
   myGlArrow(bodyAxsesLength, 0.5f, 0.03f, 0.1f);
   glPopMatrix();

   // y-axis, green
   qglColor(QColor(green));
   glPushMatrix();
   glRotatef(-90, 1, 0, 0);
   myGlArrow(bodyAxsesLength, 0.5f, 0.03f, 0.1f);
   glPopMatrix();

   glEndList();
}

void RotationGLWidget::draw_traces(void)
{
   if (e1.size()==0 && e2.size()==0 && e3.size()==0)
      return;

   glPushMatrix();
   glScalef(bodyAxsesLength, bodyAxsesLength, bodyAxsesLength);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   for (int j=0; j<3; ++j)
   {
      const std::deque<vec3<double> >& e =
         j==0 ? e1 : j==1 ? e2 : e3;

      // trace must contain at least 2 elements
      if (e.size() > 1)
      {
         // emission colour
         GLfloat em[4] = {0,0,0,1};
         em[j] = 1; // set either red, green, blue emission colour

         glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, em);
         glColor4fv(em);

         // set iterator of the tail part
         std::deque<vec3<double> >::const_iterator eit  = e.begin();
         std::deque<vec3<double> >::const_iterator tail =
            e.begin() +
            static_cast<std::deque<vec3<double> >::difference_type>
            (0.9*e.size());

         glBegin(GL_LINES);
         for (; eit < e.end()-1; ++eit)
         {
            glVertex3f((*eit)[0], (*eit)[1], (*eit)[2]);
            // decrease transparency for tail section
            if (eit > tail)
               em[3] =
                  static_cast<GLfloat>
                  (1.0 - double(eit-tail)/(0.1*e.size()));
            glColor4fv(em);
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, em);
            glVertex3f((*(eit+1))[0], (*(eit+1))[1], (*(eit+1))[2]);
         }
         glEnd();
      }
   }

   glDisable(GL_BLEND);

   glPopMatrix();
}

void RotationGLWidget::paintGL(void)
{
   // clear color and depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

   glMatrixMode(GL_MODELVIEW);           // select modelview matrix

   glLoadIdentity();
   GLfloat const em[] = {0,0,0,1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, em);

   // omega vector
   vec3<double> rotvec =
      vec3<double>::crossprod(vec3<double>(0,0,1), omega).normalize();
   GLfloat rotdeg =
      180./M_PI * vec3<double>::angle(vec3<double>(0,0,1), omega);
   glPushMatrix();
   glRotatef(rotdeg, rotvec[0], rotvec[1], rotvec[2]);
   qglColor(QColor(white));
   myGlArrow(7, .5f, .1f, 0.2f);
   glPopMatrix();

   // fixed axes
   glCallList(fixedAxses);

   glPushMatrix();

   // set up variable part of rotation matrix for body
   // set gl body rotation matrix from e1,e2,e3
   const vec3<double>& e1b = e1.front();
   const vec3<double>& e2b = e2.front();
   const vec3<double>& e3b = e3.front();

   rotmat[0]  = e1b[0];
   rotmat[1]  = e1b[1];
   rotmat[2]  = e1b[2];
   rotmat[4]  = e2b[0];
   rotmat[5]  = e2b[1];
   rotmat[6]  = e2b[2];
   rotmat[8]  = e3b[0];
   rotmat[9]  = e3b[1];
   rotmat[10] = e3b[2];

   glMultMatrixf(rotmat);

   glCallList(bodyAxses);

   glScalef(boxSize[0]/2, boxSize[1]/2, boxSize[2]/2);

   // paint box
   glBegin(GL_QUADS);
   // front (z)
   qglColor(QColor(blue));
   glNormal3f( 0,0,1);
   glVertex3f( 1,  1,  1);
   glVertex3f(-1,  1,  1);
   glVertex3f(-1, -1,  1);
   glVertex3f( 1, -1,  1);
   // back (-z)
   glNormal3f( 0,0,-1);
   glVertex3f( 1,  1, -1);
   glVertex3f(-1,  1, -1);
   glVertex3f(-1, -1, -1);
   glVertex3f( 1, -1, -1);
   // top (y)
   qglColor(QColor(green));
   glNormal3f( 0,1,0);
   glVertex3f( 1,  1,  1);
   glVertex3f( 1,  1, -1);
   glVertex3f(-1,  1, -1);
   glVertex3f(-1,  1,  1);
   // bottom (-y)
   glNormal3f( 0,-1,0);
   glVertex3f( 1, -1,  1);

   glVertex3f( 1, -1, -1);
   glVertex3f(-1, -1, -1);
   glVertex3f(-1, -1,  1);
   // left (-x)
   qglColor(QColor(red));
   glNormal3f( -1,0,0);
   glVertex3f(-1,  1,  1);
   glVertex3f(-1,  1, -1);
   glVertex3f(-1, -1, -1);
   glVertex3f(-1, -1,  1);
   // right (x)
   glNormal3f( 1,0,0);
   glVertex3f( 1,  1,  1);
   glVertex3f( 1,  1, -1);
   glVertex3f( 1, -1, -1);
   glVertex3f( 1, -1,  1);
   glEnd();

   // traces
   glPopMatrix();
   draw_traces ();

   glFlush();
}

void RotationGLWidget::resizeGL(int w, int h)
{
   // Prevent a divide by zero
   if (h == 0) h = 1;

   // set the new view port
   glViewport(0, 0, (GLint)w, (GLint)h);

   // set up projection matrix
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   // Perspective view
   gluPerspective(40.0f, (GLdouble)w/(GLdouble)h, 1.0, 100.0f);

   // Viewing transformation, position for better view
   // Theta is polar angle 0<Theta<Pi
   gluLookAt(
      eyeR * sin(eyeTheta) * sin(eyePhi),
      eyeR * sin(eyeTheta) * cos(eyePhi),
      eyeR * cos(eyeTheta),
      0,0,0,
      0,0,1);
}

/* --------- privat methods ----------- */

void RotationGLWidget::myGlArrow(
   GLfloat total_length, GLfloat head_length,
   GLfloat base_width,   GLfloat head_width)
{
   GLUquadricObj* quadAx = gluNewQuadric();
   glPushMatrix();
   gluCylinder(quadAx, base_width, base_width,
               total_length-head_length, 10, 1);
   glTranslatef(0, 0, total_length-head_length);
   gluCylinder(quadAx, head_width, 0, head_length, 10, 1);
   glPopMatrix();
   gluDeleteQuadric(quadAx);
}


//-----------------------------------------------------------------------------
// KRotationSaver: screen saver class
//-----------------------------------------------------------------------------

KRotationSaver::KRotationSaver(WId id)
   : KScreenSaver(id),
     J(4,2,3),                  // fixed box sizes!
     initEulerPhi(0),
     initEulerPsi(0),
     solver(0),
     glArea(0),
     timer(0),
     m_traceLengthSeconds(traceLengthSecondsDefault),
     m_Lz(LzDefault),
     m_initEulerTheta(initEulerThetaDefault)
{
   readSettings();              // read global settings
   initData();                  // init e1,e2,e3,omega,solver

   setEraseColor(black);
   erase();                     // erase area
   glArea = new RotationGLWidget(
      this, 0, omega, e1, e2, e3, J); // create gl widget
   embed(glArea);               // embed gl widget and resize it
   glArea->show();              // show gl widget

   timer = new QTimer(this);
   timer->start(deltaT, TRUE);
   connect(timer, SIGNAL(timeout()), this, SLOT(doTimeStep()));
}

KRotationSaver::~KRotationSaver()
{
   // time, rotation are automatically deleted with parent KRotationSaver
   delete solver;
}

void KRotationSaver::initData()
{
   // reset coordiante system
   vec3<double> e1t(1,0,0), e2t(0,1,0), e3t(0,0,1);
   // rotation by phi around z = zhat axis
   e1t.rotate(initEulerPhi*e3t);
   e2t.rotate(initEulerPhi*e3t);
   // rotation by theta around new x axis
   e2t.rotate(m_initEulerTheta*e1t);
   e3t.rotate(m_initEulerTheta*e1t);
   // rotation by psi around new z axis
   e1t.rotate(initEulerPsi*e3t);
   e2t.rotate(initEulerPsi*e3t);
   // set first vector in deque
   e1.clear(); e1.push_front(e1t);
   e2.clear(); e2.push_front(e2t);
   e3.clear(); e3.push_front(e3t);

   // calc L in body frame: 1. determine z-axis of fixed frame in body
   // coordinates, undo the transformations for unit z vector of the body frame

   // calc omega_body from ...
   vec3<double> e1_body(1,0,0), e3_body(0,0,1);
   // rotation by -psi along z axis
   e1_body.rotate(-initEulerPsi*e3_body);
   // rotation by -theta along new x axis
   e3_body.rotate(-m_initEulerTheta*e1_body);
   // omega_body = L_body * J_body^(-1)
   vec3<double> omega_body = e3_body * m_Lz;
   omega_body /= J;

   // assemble initial y for solver
   std::valarray<double> y(12);
   y[std::slice(0,3,1)] = omega_body;
   // 3 basis vectors of body system in fixed coordinates
   y[std::slice(3,3,1)] = e1t;
   y[std::slice(6,3,1)] = e2t;
   y[std::slice(9,3,1)] = e3t;

   // initial rotation vector
   omega
      = omega_body[0]*e1t
      + omega_body[1]*e2t
      + omega_body[2]*e3t;

   if (solver!=0) delete solver;
   // init solver
   solver = new EulerOdeSolver(
      0.0,      // t
      0.01,     // first dt step size estimation
      J[0], J[1], J[2], // A,B,C
      y,        // omega_body,e1,e2,e3
      1e-5);    // eps
}

void KRotationSaver::readSettings()
{
   // read configuration settings from config file
   KConfig *config = KGlobal::config();
   config->setGroup("Settings");

   // internal saver parameters are set to stored values or left at their
   // default values if stored values are out of range
   setTraceFlag(0, config->readBoolEntry("x trace", traceFlagDefault[0]));
   setTraceFlag(1, config->readBoolEntry("y trace", traceFlagDefault[1]));
   setTraceFlag(2, config->readBoolEntry("z trace", traceFlagDefault[2]));
   setRandomTraces(config->readBoolEntry("random traces", randomTracesDefault));
   setTraceLengthSeconds(
      config->readDoubleNumEntry("length", traceLengthSecondsDefault));
   setLz(
      config->readDoubleNumEntry("Lz",     LzDefault));
   setInitEulerTheta(
      config->readDoubleNumEntry("theta",  initEulerThetaDefault));
}

void KRotationSaver::setTraceLengthSeconds(const double& t)
{
   if (t >= traceLengthSecondsLimitLower
       && t <= traceLengthSecondsLimitUpper)
   {
      m_traceLengthSeconds = t;
   }
}

const double KRotationSaver::traceLengthSecondsLimitLower = 0.0;
const double KRotationSaver::traceLengthSecondsLimitUpper = 99.0;
const double KRotationSaver::traceLengthSecondsDefault = 3.0;

const bool KRotationSaver::traceFlagDefault[3] = {false, false, true};

void KRotationSaver::setLz(const double& Lz)
{
   if (Lz >= LzLimitLower && Lz <= LzLimitUpper)
   {
      m_Lz = Lz;
   }
}

const double KRotationSaver::LzLimitLower =   0.0;
const double KRotationSaver::LzLimitUpper = 500.0;
const double KRotationSaver::LzDefault    =  10.0;

void KRotationSaver::setInitEulerTheta(const double& theta)
{
   if (theta >= initEulerThetaLimitLower
       && theta <= initEulerThetaLimitUpper)
   {
      m_initEulerTheta = theta;
   }
}

const double KRotationSaver::initEulerThetaLimitLower =   0.0;
const double KRotationSaver::initEulerThetaLimitUpper = 180.0;
const double KRotationSaver::initEulerThetaDefault    =   0.03;

//   public slots

void KRotationSaver::doTimeStep()
{
   // integrate a step ahead
   solver->integrate(0.001*deltaT);

   // read new y
   std::valarray<double> y = solver->Y();

   std::deque<vec3<double> >::size_type
      max_vec_length =
      static_cast<std::deque<vec3<double> >::size_type>
      ( m_traceLengthSeconds/(0.001*deltaT) );

   for (int j=0; j<3; ++j)
   {
      std::deque<vec3<double> >& e =
         j==0 ? e1 :
         j==1 ? e2 : e3;

      // read out new body coordinate system
      if (m_traceFlag[j] == true
          && max_vec_length > 0)
      {
         e.push_front(y[std::slice(3*j+3, 3, 1)]);
         while (e.size() > max_vec_length)
         {
            e.pop_back();
         }
      }
      else
      {
         // only set the 1. element
         e.front() = y[std::slice(3*j+3, 3, 1)];
         // and delete all other emements
         if (e.size() > 1)
            e.resize(1);
      }
   }

   // current rotation vector omega
   omega = y[0]*e1.front() + y[1]*e2.front() + y[2]*e3.front();

   // set new random traces every 10 seconds
   if (m_randomTraces==true)
   {
      static unsigned int counter=0;
      ++counter;
      if (counter > unsigned(10.0/(0.001*deltaT)))
      {
         counter=0;
         for (int i=0; i<3; ++i)
            m_traceFlag[i] = rand()%2==1 ? true : false;
      }
   }

   glArea->updateGL();
   timer->start(deltaT, TRUE); // restart timer
}

// public slot of KRotationSaver, forward resize event to public slot of glArea
// to allow the resizing of the gl area withing the setup dialog
void KRotationSaver::resizeGlArea(QResizeEvent* e)
{
   glArea->resize(e->size());
}

//-----------------------------------------------------------------------------
// KRotationSetup: dialog to setup screen saver parameters
//-----------------------------------------------------------------------------

KRotationSetup::KRotationSetup(QWidget* parent, const char* name)
   : KRotationSetupUi(parent, name),
     // create ssaver and give it the WinID of the preview area
     saver(new KRotationSaver(preview->winId()))
{
   // the dialog should block, no other control center input should be possible
   // until the dialog is closed
   setModal(TRUE);

   lengthEdit->setValidator(
      new QDoubleValidator(
         KRotationSaver::traceLengthSecondsLimitLower,
         KRotationSaver::traceLengthSecondsLimitUpper,
         3, lengthEdit));
   LzEdit->setValidator(
      new QDoubleValidator(
         KRotationSaver::LzLimitLower,
         KRotationSaver::LzLimitUpper,
         3, LzEdit));
   thetaEdit->setValidator(
      new QDoubleValidator(
         KRotationSaver::initEulerThetaLimitLower,
         KRotationSaver::initEulerThetaLimitUpper,
         3, thetaEdit));

   // set tool tips of editable fields
   QToolTip::add(
      lengthEdit,
      i18n("Length of traces in seconds of visibility.\nValid values from %1 to %2.")
      .arg(KRotationSaver::traceLengthSecondsLimitLower, 0, 'f', 2)
      .arg(KRotationSaver::traceLengthSecondsLimitUpper, 0, 'f', 2));
   QToolTip::add(
      LzEdit,
      i18n("Angular momentum in z direction in arbitrary units.\nValid values from %1 to %2.")
      .arg(KRotationSaver::LzLimitLower, 0, 'f', 2)
      .arg(KRotationSaver::LzLimitUpper, 0, 'f', 2));
   QToolTip::add(
      thetaEdit,
      i18n("Gravitational constant in arbitrary units.\nValid values from %1 to %2.")
      .arg(KRotationSaver::initEulerThetaLimitLower, 0, 'f', 2)
      .arg(KRotationSaver::initEulerThetaLimitUpper, 0, 'f', 2));

   // init preview area
   preview->setBackgroundColor(black);
   preview->show();    // otherwise saver does not get correct size

   // read settings from saver and update GUI elements with these values, saver
   // has read settings in its constructor

   // set editable fields with stored values as defaults
   xTrace->setChecked(saver->traceFlag(0));
   yTrace->setChecked(saver->traceFlag(1));
   zTrace->setChecked(saver->traceFlag(2));
   randTraces->setChecked(saver->randomTraces());
   QString text;
   text.setNum(saver->traceLengthSeconds());
   lengthEdit->validateAndSet(text,0,0,0);
   text.setNum(saver->Lz());
   LzEdit->validateAndSet(text,0,0,0);
   text.setNum(saver->initEulerTheta());
   thetaEdit->validateAndSet(text,0,0,0);

   // if the preview area is resized it emmits the resized() event which is
   // caught by the saver.  The embedded GlArea is resized to fit into the
   // preview area.
   connect(preview, SIGNAL(resized(QResizeEvent*)),
           saver,   SLOT(resizeGlArea(QResizeEvent*)));
}

KRotationSetup::~KRotationSetup()
{
   delete saver;
}

// Ok pressed - save settings and exit
void KRotationSetup::okButtonClickedSlot(void)
{
   KConfig* config = KGlobal::config();
   config->setGroup("Settings");
   config->writeEntry("x trace",       saver->traceFlag(0));
   config->writeEntry("y trace",       saver->traceFlag(1));
   config->writeEntry("z trace",       saver->traceFlag(2));
   config->writeEntry("random traces", saver->randomTraces());
   config->writeEntry("length",        saver->traceLengthSeconds());
   config->writeEntry("Lz",            saver->Lz());
   config->writeEntry("theta",         saver->initEulerTheta());
   config->sync();
   accept();
}

void KRotationSetup::aboutButtonClickedSlot(void)
{
   KMessageBox::about(this, i18n("\
<h3>KRotation Screen Saver for KDE</h3>\
<p>Simulation of a force free rotating asymmetric body</p>\
<p>Copyright (c) Georg&nbsp;Drenkhahn 2004</p>\
<p><tt>georg-d@users.sourceforge.net</tt></p>"));
}

void KRotationSetup::xTraceToggled(bool state)
{
   saver->setTraceFlag(0, state);
}
void KRotationSetup::yTraceToggled(bool state)
{
   saver->setTraceFlag(1, state);
}
void KRotationSetup::zTraceToggled(bool state)
{
   saver->setTraceFlag(2, state);
}
void KRotationSetup::randomTracesToggled(bool state)
{
   saver->setRandomTraces(state);
   if (state==false)
   {
      // restore settings from gui if random traces are turned off
      saver->setTraceFlag(0, xTrace->isChecked());
      saver->setTraceFlag(1, yTrace->isChecked());
      saver->setTraceFlag(2, zTrace->isChecked());
   }
}
void KRotationSetup::lengthEnteredSlot(const QString& s)
{
   saver->setTraceLengthSeconds(s.toDouble());
}
void KRotationSetup::LzEnteredSlot(const QString& s)
{
   saver->setLz(s.toDouble());
   if (saver!=0) saver->initData();
}
void KRotationSetup::thetaEnteredSlot(const QString& s)
{
   saver->setInitEulerTheta(s.toDouble());
   if (saver!=0) saver->initData();
}
