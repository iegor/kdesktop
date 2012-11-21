//============================================================================
//
// KPendulum screen saver for KDE
//
// The screen saver displays a physically realistic simulation of a two-part
// pendulum.
//
// Developed by Georg Drenkhahn, georg-d@users.sourceforge.net
//
// $Id: pendulum.cpp 501235 2006-01-22 14:06:24Z mueller $
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

// Qt headers
#include <qlineedit.h>
#include <qspinbox.h>
#include <qvalidator.h>
#include <qcolordialog.h>
#include <qpushbutton.h>
#include <qtooltip.h>
// KDE headers
#include <klocale.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>

#include "sspreviewarea.h"

// pendulum.moc includes pendulum.h
#include "pendulum.moc"

#define KPENDULUM_VERSION "1.1"

// libkscreensaver interface
extern "C"
{
   /// application name for libkscreensaver interface
   KDE_EXPORT const char *kss_applicationName = "kpendulum.kss";
   /// application description for libkscreensaver interface
   KDE_EXPORT const char *kss_description = I18N_NOOP("Simulation of\
 a two-part pendulum");
   /// application version for libkscreensaver interface
   KDE_EXPORT const char *kss_version = KPENDULUM_VERSION;

   /// function to create screen saver object
   KDE_EXPORT KScreenSaver* kss_create(WId id)
   {
      return new KPendulumSaver(id);
   }

   /// function to create setup dialog for screen saver
   KDE_EXPORT QDialog* kss_setup()
   {
      return new KPendulumSetup();
   }
}

//-----------------------------------------------------------------------------
// PendulumOdeSolver
//-----------------------------------------------------------------------------

PendulumOdeSolver::PendulumOdeSolver(
   const double &_t,
   const double &_dt,
   std::valarray<double> &_y,
   const double &_eps,
   const double &_m1,
   const double &_m2,
   const double &_l1,
   const double &_l2,
   const double &_g)
   : RkOdeSolver<double>(_t,_y,_dt,_eps),
     A(1.0/(_m2*_l1*_l1)),
     B1(_m2*_l1*_l2),           // constants for faster numeric calculation
     B(1.0/B1),                 // derived from m1,m2,l1,l2,g
     C((_m1+_m2)/(_m2*_m2*_l2*_l2)),
     D(_g*(_m1+_m2)*_l1),
     E(_g*_m2*_l2),
     M((_m1+_m2)/_m2)
{
}

std::valarray<double> PendulumOdeSolver::f(
   const double &x,
   const std::valarray<double> &y) const
{
   (void)x; // unused

   const double& q1 = y[0];
   const double& q2 = y[1];
   const double& p1 = y[2];
   const double& p2 = y[3];

   const double cosDq = std::cos(q1-q2);
   const double iden  = 1.0/(M - cosDq*cosDq); // invers denominator
   const double dq1dt = (A*p1 - B*cosDq*p2)*iden;
   const double dq2dt = (C*p2 - B*cosDq*p1)*iden;

   std::valarray<double> ypr(y.size());
   ypr[0] = dq1dt;
   ypr[1] = dq2dt;

   const double K = B1 * dq1dt*dq2dt * std::sin(q1-q2);
   ypr[2] = -K - D * std::sin(q1);
   ypr[3] =  K - E * std::sin(q2);

   return ypr;
}

//-----------------------------------------------------------------------------
// Rotation: screen saver widget
//-----------------------------------------------------------------------------

PendulumGLWidget::PendulumGLWidget(QWidget* parent, const char* name)
   : QGLWidget(parent, name),
     eyeR(30),                  // eye coordinates (polar)
     eyeTheta(M_PI*0.45),
     eyePhi(0),
     lightR(eyeR),              // light coordinates (polar)
     lightTheta(M_PI*0.25),
     lightPhi(M_PI*0.25),
     quadM1(gluNewQuadric()),
     m_barColor(KPendulumSaver::barColorDefault),
     m_m1Color(KPendulumSaver::m1ColorDefault),
     m_m2Color(KPendulumSaver::m2ColorDefault)
{
}

PendulumGLWidget::~PendulumGLWidget(void)
{
   gluDeleteQuadric(quadM1);
}

void PendulumGLWidget::setEyePhi(double phi)
{
   eyePhi = phi;
   while (eyePhi < 0)      eyePhi += 2.*M_PI;
   while (eyePhi > 2*M_PI) eyePhi -= 2.*M_PI;

   // get the view port
   static GLint vp[4];
   glGetIntegerv(GL_VIEWPORT, vp);
   // calc new perspective, a resize event is simulated here
   resizeGL(static_cast<int>(vp[2]), static_cast<int>(vp[3]));
}

void PendulumGLWidget::setAngles(const double& q1, const double& q2)
{
   ang1 = static_cast<GLfloat>(q1*180./M_PI);
   ang2 = static_cast<GLfloat>(q2*180./M_PI);
}

void PendulumGLWidget::setMasses(const double& m1, const double& m2)
{
   sqrtm1 = static_cast<GLfloat>(sqrt(m1));
   sqrtm2 = static_cast<GLfloat>(sqrt(m2));
}

void PendulumGLWidget::setLengths(const double& _l1, const double& _l2)
{
   l1 = static_cast<GLfloat>(_l1);
   l2 = static_cast<GLfloat>(_l2);
}

void PendulumGLWidget::setBarColor(const QColor& c)
{
   if (c.isValid())
   {
      m_barColor = c;
   }
}

void PendulumGLWidget::setM1Color(const QColor& c)
{
   if (c.isValid())
   {
      m_m1Color = c;
   }
}
void PendulumGLWidget::setM2Color(const QColor& c)
{
   if (c.isValid())
   {
      m_m2Color = c;
   }
}

/* --------- protected methods ----------- */

void PendulumGLWidget::initializeGL(void)
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
   glEnable(GL_LIGHT1);

   glMatrixMode(GL_MODELVIEW);           // select modelview matrix
   glLoadIdentity();
   // set positon of light0
   GLfloat lightPos[4]=
      {lightR * sin(lightTheta) * sin(lightPhi),
       lightR * sin(lightTheta) * cos(lightPhi),
       lightR * cos(lightTheta),
       0};
   glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
   // set positon of light1
   lightPos[0] = lightR * sin(lightTheta) * sin(lightPhi+M_PI);
   lightPos[1] = lightR * sin(lightTheta) * cos(lightPhi+M_PI);
   glLightfv(GL_LIGHT1, GL_POSITION, lightPos);

   // only for lights #>0
   GLfloat spec[]={1,1,1,1};
   glLightfv(GL_LIGHT1, GL_SPECULAR, spec);
   glLightfv(GL_LIGHT1, GL_DIFFUSE, spec);

   // enable setting the material colour by glColor()
   glEnable(GL_COLOR_MATERIAL);

   GLfloat emi[4] = {.13, .13, .13, 1};
   glMaterialfv(GL_FRONT, GL_EMISSION, emi);
}

void PendulumGLWidget::paintGL(void)
{
   // clear color and depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

   glMatrixMode(GL_MODELVIEW);           // select modelview matrix

   glLoadIdentity();

   static const GLfloat width     = 2.0;
   static const GLfloat masswidth = 1.0;
   static const int noOfSlices    = 20;

   // top axis, left (x>0)
   glTranslatef(0.5*width, 0, 0);
   glRotatef(90, 0, 1, 0);
   qglColor(m_barColor);
   gluCylinder(quadM1, 0.2, 0.2, 5, 10, 1);
   gluSphere(quadM1, 0.2, 10, 10);
   // top axis, right
   glLoadIdentity();
   glTranslatef(-0.5*width, 0, 0);
   glRotatef(-90, 0, 1, 0);
   gluCylinder(quadM1, 0.2, 0.2, 5, 10, 1);
   gluSphere(quadM1, 0.2, 10, 10);
   // 1. part, left
   glLoadIdentity();
   glRotatef(ang1, 1, 0, 0);
   glPushMatrix();
   glTranslatef(0.5*width, 0, -l1);
   gluCylinder(quadM1, 0.2, 0.2, l1, 10, 1);
   glPopMatrix();

   // 1. part, right
   glPushMatrix();
   glTranslatef(-0.5*width, 0, -l1);
   gluCylinder(quadM1, 0.2, 0.2, l1, 10, 1);
   // 1. part, bottom
   glRotatef(90, 0, 1, 0);
   gluSphere(quadM1, 0.2, 10, 10); // bottom corner 1
   gluCylinder(quadM1, 0.2, 0.2, width, 10, 1); // connection
   glTranslatef(0, 0, 0.5*(width-masswidth));
   qglColor(m_m1Color);
   gluCylinder(quadM1, sqrtm1, sqrtm1, masswidth, noOfSlices, 1); // mass 1
   gluQuadricOrientation(quadM1, GLU_INSIDE);
   gluDisk(quadM1, 0, sqrtm1, noOfSlices,1); // bottom of mass
   gluQuadricOrientation(quadM1, GLU_OUTSIDE);
   glTranslatef(0, 0, masswidth);
   gluDisk(quadM1, 0, sqrtm1, noOfSlices,1); // top of mass

   glTranslatef(0, 0, 0.5*(width-masswidth));
   qglColor(m_barColor);
   gluSphere(quadM1, 0.2, 10, 10); // bottom corner 2
   glPopMatrix();

   // 2. pendulum bar
   glLoadIdentity();
   glTranslatef(0, l1*std::sin(ang1*M_PI/180.), -l1*std::cos(ang1*M_PI/180.));
   glRotatef(ang2, 1, 0, 0);
   glTranslatef(0, 0, -l2);
   qglColor(m_barColor);
   gluCylinder(quadM1, 0.2, 0.2, l2, 10, 1);

   // mass 2
   glRotatef(90, 0, 1, 0);
   glTranslatef(0, 0, -0.5*masswidth);
   qglColor(m_m2Color);
   gluCylinder(quadM1, sqrtm2, sqrtm2, masswidth, noOfSlices, 1);
   gluQuadricOrientation(quadM1, GLU_INSIDE);
   gluDisk(quadM1, 0, sqrtm2, noOfSlices,1); // bottom of mass
   gluQuadricOrientation(quadM1, GLU_OUTSIDE);
   glTranslatef(0, 0, masswidth);
   gluDisk(quadM1, 0, sqrtm2, noOfSlices,1); // top of mass

   glFlush();
}

void PendulumGLWidget::resizeGL(int w, int h)
{
   // Prevent a divide by zero
   if (h == 0) h = 1;

   // set the new view port
   glViewport(0, 0, static_cast<GLint>(w), static_cast<GLint>(h));

   // set up projection matrix
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   // Perspective view
   gluPerspective(40.0f,
                  static_cast<GLdouble>(w)/static_cast<GLdouble>(h),
                  1.0, 100.0f);

   // Viewing transformation, position for better view
   // Theta is polar angle 0<Theta<Pi
   gluLookAt(
      eyeR * sin(eyeTheta) * sin(eyePhi),
      eyeR * sin(eyeTheta) * cos(eyePhi),
      eyeR * cos(eyeTheta),
      0,0,0,
      0,0,1);
}

//-----------------------------------------------------------------------------
// KPendulumSaver: screen saver class
//-----------------------------------------------------------------------------

KPendulumSaver::KPendulumSaver(WId id) :
   KScreenSaver(id),
   solver(0),
   m_massRatio(massRatioDefault),
   m_lengthRatio(lengthRatioDefault),
   m_g(gDefault),
   m_E(EDefault),
   m_persChangeInterval(persChangeIntervalDefault)
{
   setEraseColor(black);
   erase();                     // erase area
   glArea = new PendulumGLWidget(this); // create gl widget
   glArea->setEyePhi(eyePhiDefault);

   readSettings();              // read global settings into pars
   initData();                  // init solver and glArea with read settings

   embed(glArea);               // embed gl widget and resize it
   glArea->show();              // show gl widget

   // set up and start cyclic timer
   timer = new QTimer(this);
   timer->start(deltaT, TRUE);
   connect(timer, SIGNAL(timeout()), this, SLOT(doTimeStep()));
}

KPendulumSaver::~KPendulumSaver()
{
   // time, rotation are automatically deleted with parent KPendulumSaver
   delete solver;
}


void KPendulumSaver::readSettings()
{
   // read configuration settings from config file
   KConfig *config = KGlobal::config();
   config->setGroup("Settings");

   // internal saver parameters are set to stored values or left at their
   // default values if stored values are out of range
   setMassRatio(
      config->readDoubleNumEntry(
         "mass ratio",
         KPendulumSaver::massRatioDefault));
   setLengthRatio(
      config->readDoubleNumEntry(
         "length ratio",
         KPendulumSaver::lengthRatioDefault));
   setG(
      config->readDoubleNumEntry(
         "g",
         KPendulumSaver::gDefault));
   setE(
      config->readDoubleNumEntry(
         "E",
         KPendulumSaver::EDefault));
   setPersChangeInterval(
      config->readUnsignedNumEntry(
         "perspective change interval",
         KPendulumSaver::persChangeIntervalDefault));

   // set the colours
   setBarColor(config->readColorEntry("bar color", &barColorDefault));
   setM1Color( config->readColorEntry("m1 color",  &m1ColorDefault));
   setM2Color( config->readColorEntry("m2 color",  &m2ColorDefault));
}

void KPendulumSaver::initData()
{
   const double m1plusm2 = 2;   // m1+m2
   const double m2 = m_massRatio * m1plusm2;
   const double m1 = m1plusm2 - m2;
   glArea->setMasses(m1, m2);
   glArea->setAngles(0, 0);

   const double l1plusl2 = 9;   // l1+l2
   const double l2 = m_lengthRatio * l1plusl2;
   const double l1 = l1plusl2 - l2;
   glArea->setLengths(l1, l2);

   // kinetic energy of m2 and m1
   const double kin_energy = m_E * m_g * (l1*m1 + (m1+m2)*(l1+l2));
   // angular velocity for 1. and 2. pendulum
   const double qp = sqrt(2.*kin_energy/((m1+m2)*l1*l1 + m2*l2*l2 + m2*l1*l2));

   // assemble initial y for solver
   std::valarray<double> y(4);
   y[0] = 0;                    // q1
   y[1] = 0;                    // q2
   y[2] = (m1+m2)*l1*l1*qp + 0.5*m2*l1*l2*qp; // p1
   y[3] = m2*l2*l2*qp + 0.5*m2*l1*l2*qp; // p2

   // delete old solver
   if (solver!=0) delete solver;
   // init new solver
   solver = new PendulumOdeSolver(
      0.0,                      // t
      0.01,                     // first dt step size estimation
      y,
      1e-5,                     // eps
      m1,
      m2,
      l1,
      l2,
      m_g);
}


void KPendulumSaver::setBarColor(const QColor& c)
{
   glArea->setBarColor(c);
}
QColor KPendulumSaver::barColor(void) const
{
   return glArea->barColor();
}

const QColor KPendulumSaver::barColorDefault(255, 255, 127);

void KPendulumSaver::setM1Color(const QColor& c)
{
   glArea->setM1Color(c);
}
QColor KPendulumSaver::m1Color(void) const
{
   return glArea->m1Color();
}

const QColor KPendulumSaver::m1ColorDefault(170,   0, 127);

void KPendulumSaver::setM2Color(const QColor& c)
{
   glArea->setM2Color(c);
}
QColor KPendulumSaver::m2Color(void) const
{
   return glArea->m2Color();
}

const QColor KPendulumSaver::m2ColorDefault( 85, 170, 127);


void KPendulumSaver::setMassRatio(const double& massRatio)
{
   // range check is not neccessary in normal operation because validators check
   // the values at input.  But the validators do not check for corrupted
   // settings read from disk.
   if (massRatio >= massRatioLimitLower
       && massRatio <= massRatioLimitUpper
       && m_massRatio != massRatio)
   {
      m_massRatio = massRatio;
      if (timer!=0)
      {
         initData();
      }
   }
}

const double KPendulumSaver::massRatioLimitLower = 0.01;
const double KPendulumSaver::massRatioLimitUpper = 0.99;
const double KPendulumSaver::massRatioDefault    = 0.5;

void KPendulumSaver::setLengthRatio(const double& lengthRatio)
{
   if (lengthRatio >= lengthRatioLimitLower
       && lengthRatio <= lengthRatioLimitUpper
       && m_lengthRatio != lengthRatio)
   {
      m_lengthRatio = lengthRatio;
      if (timer!=0)
      {
         initData();
      }
   }
}

const double KPendulumSaver::lengthRatioLimitLower = 0.01;
const double KPendulumSaver::lengthRatioLimitUpper = 0.99;
const double KPendulumSaver::lengthRatioDefault    = 0.5;

void KPendulumSaver::setG(const double& g)
{
   if (g >= gLimitLower
       && g <= gLimitUpper
       && m_g != g)
   {
      m_g = g;
      if (timer!=0)
      {
         initData();
      }
   }
}

const double KPendulumSaver::gLimitLower = 0.1;
const double KPendulumSaver::gLimitUpper = 300.0;
const double KPendulumSaver::gDefault    = 40.0;

void KPendulumSaver::setE(const double& E)
{
   if (E >= ELimitLower
       && E <= ELimitUpper
       && m_E != E)
   {
      m_E = E;
      if (timer!=0)
      {
         initData();
      }
   }
}

const double KPendulumSaver::ELimitLower = 0.0;
const double KPendulumSaver::ELimitUpper = 5.0;
const double KPendulumSaver::EDefault    = 1.2;

void KPendulumSaver::setPersChangeInterval(
   const unsigned int& persChangeInterval)
{
   if (persChangeInterval >= persChangeIntervalLimitLower
       && persChangeInterval <= persChangeIntervalLimitUpper
       && m_persChangeInterval != persChangeInterval)
   {
      m_persChangeInterval = persChangeInterval;
      // do not restart simulation here
   }
}

const double KPendulumSaver::eyePhiDefault = 0.25*M_PI;


void KPendulumSaver::doTimeStep()
{
   /* time (in seconds) of perspective change.
    * - t<0: no change yet
    * - t=0: change starts
    * - 0<t<moving time: change takes place
    * - t=moving time: end of the change */
   static double persChangeTime = -5;

   // integrate a step ahead
   solver->integrate(0.001*deltaT);

   // read new y from solver
   const std::valarray<double> y = solver->Y();

   // tell glArea the new coordinates/angles of the pendulum
   glArea->setAngles(y[0], y[1]);

   // handle perspective change
   persChangeTime += 0.001*deltaT;
   if (persChangeTime > 0)
   {
      // phi value at the start of a perspective change
      static double eyePhi0     = eyePhiDefault;
      // phi value at the end of a perspective change
      static double eyePhi1     = 0.75*M_PI;
      static double deltaEyePhi = eyePhi1-eyePhi0;

      // movement acceleration/deceleration
      const double a = 3;
      // duration of the change period
      const double movingTime = 2.*sqrt(fabs(deltaEyePhi)/a);

      // new current phi of eye
      double eyePhi = persChangeTime < 0.5*movingTime ?
         // accelerating phase
         eyePhi0 + (deltaEyePhi>0?1:-1)
         * 0.5*a*persChangeTime*persChangeTime:
         // decellerating phase
         eyePhi1 - (deltaEyePhi>0?1:-1)
         * 0.5*a*(movingTime-persChangeTime)*(movingTime-persChangeTime);

      if (persChangeTime>movingTime)
      {  // perspective change has finished
         // set new time till next change
         persChangeTime = -double(m_persChangeInterval);
         eyePhi0 = eyePhi = eyePhi1;
         // find new phi value with angleLimit < phi < Pi-angleLimit or
         // Pi+angleLimit < phi < 2*Pi-angleLimit
         const double angleLimit = M_PI*0.2;
         for (eyePhi1 = 0;
              eyePhi1<angleLimit
                 || (eyePhi1<M_PI+angleLimit && eyePhi1>M_PI-angleLimit)
                 || eyePhi1>2*M_PI-angleLimit;
              eyePhi1 = double(rand())/RAND_MAX * 2*M_PI)
         {}
         // new delta phi for next change
         deltaEyePhi = eyePhi1 - eyePhi0;
         // find shortest perspective change
         if (deltaEyePhi < -M_PI) deltaEyePhi += 2*M_PI;
      }

      glArea->setEyePhi(eyePhi); // set new perspective
   }

   glArea->updateGL();          // repaint scenery
   timer->start(deltaT, TRUE);  // restart timer
}

// public slot of KPendulumSaver, forward resize event to public slot of glArea
// to allow the resizing of the gl area withing the setup dialog
void KPendulumSaver::resizeGlArea(QResizeEvent* e)
{
   glArea->resize(e->size());
}

//-----------------------------------------------------------------------------
// KPendulumSetup: dialog to setup screen saver parameters
//-----------------------------------------------------------------------------

KPendulumSetup::KPendulumSetup(QWidget* parent, const char* name)
   : KPendulumSetupUi(parent, name),
     // create saver and give it the WinID of the preview area
     saver(new KPendulumSaver(preview->winId()))
{
   // the dialog should block, no other control center input should be possible
   // until the dialog is closed
   setModal(TRUE);

   // create input validators
   mEdit->setValidator(new QDoubleValidator(
                          KPendulumSaver::massRatioLimitLower,
                          KPendulumSaver::massRatioLimitUpper,
                          5, mEdit));
   lEdit->setValidator(new QDoubleValidator(
                          KPendulumSaver::lengthRatioLimitLower,
                          KPendulumSaver::lengthRatioLimitUpper,
                          5, lEdit));
   gEdit->setValidator(new QDoubleValidator(
                          KPendulumSaver::gLimitLower,
                          KPendulumSaver::gLimitUpper,
                          5, gEdit));
   eEdit->setValidator(new QDoubleValidator(
                          KPendulumSaver::ELimitLower,
                          KPendulumSaver::ELimitUpper,
                          5, eEdit));

   // set input limits for the perspective change interval time
   persSpinBox->setMinValue(KPendulumSaver::persChangeIntervalLimitLower);
   persSpinBox->setMaxValue(KPendulumSaver::persChangeIntervalLimitUpper);

   // set tool tips of editable fields
   QToolTip::add(
      mEdit,
      i18n("Ratio of 2nd mass to sum of both masses.\nValid values from %1 to %2.")
      .arg(KPendulumSaver::massRatioLimitLower, 0, 'f', 2)
      .arg(KPendulumSaver::massRatioLimitUpper, 0, 'f', 2));
   QToolTip::add(
      lEdit,
      i18n("Ratio of 2nd pendulum part length to the sum of both part lengths.\nValid values from %1 to %2.")
      .arg(KPendulumSaver::lengthRatioLimitLower, 0, 'f', 2)
      .arg(KPendulumSaver::lengthRatioLimitUpper, 0, 'f', 2));
   QToolTip::add(
      gEdit,
      i18n("Gravitational constant in arbitrary units.\nValid values from %1 to %2.")
      .arg(KPendulumSaver::gLimitLower, 0, 'f', 2)
      .arg(KPendulumSaver::gLimitUpper, 0, 'f', 2));
   QToolTip::add(
      eEdit,
      i18n("Energy in units of the maximum potential energy of the given configuration.\nValid values from %1 to %2.")
      .arg(KPendulumSaver::ELimitLower, 0, 'f', 2)
      .arg(KPendulumSaver::ELimitUpper, 0, 'f', 2));
   QToolTip::add(
      persSpinBox,
      i18n("Time in seconds after which a random perspective change occurs.\nValid values from %1 to %2.")
      .arg(KPendulumSaver::persChangeIntervalLimitLower)
      .arg(KPendulumSaver::persChangeIntervalLimitUpper));

   // init preview area
   preview->setBackgroundColor(black);
   preview->show();    // otherwise saver does not get correct size

   // read settings from saver and update GUI elements with these values, saver
   // has read settings in its constructor

   // set editable fields with stored values as defaults
   QString text;
   text.setNum(saver->massRatio());
   mEdit->setText(text);
   text.setNum(saver->lengthRatio());
   lEdit->setText(text);
   text.setNum(saver->g());
   gEdit->setText(text);
   text.setNum(saver->E());
   eEdit->setText(text);

   persSpinBox->setValue(saver->persChangeInterval());

   barColorButton->setPaletteBackgroundColor(saver->barColor());
   m1ColorButton->setPaletteBackgroundColor(saver->m1Color());
   m2ColorButton->setPaletteBackgroundColor(saver->m2Color());

   // if the preview area is resized it emmits the resized() event which is
   // caught by the saver.  The embedded GlArea is resized to fit into the
   // preview area.
   connect(preview, SIGNAL(resized(QResizeEvent*)),
           saver,   SLOT(resizeGlArea(QResizeEvent*)));
}

KPendulumSetup::~KPendulumSetup()
{
   delete saver;
}

// Ok pressed - save settings and exit
void KPendulumSetup::okButtonClickedSlot()
{
   KConfig* config = KGlobal::config();
   config->setGroup("Settings");

   config->writeEntry("mass ratio",   saver->massRatio());
   config->writeEntry("length ratio", saver->lengthRatio());
   config->writeEntry("g",            saver->g());
   config->writeEntry("E",            saver->E());
   config->writeEntry("perspective change interval",
                      saver->persChangeInterval());
   config->writeEntry("bar color",    saver->barColor());
   config->writeEntry("m1 color",     saver->m1Color());
   config->writeEntry("m2 color",     saver->m2Color());

   config->sync();
   accept();
}

void KPendulumSetup::aboutButtonClickedSlot()
{
   KMessageBox::about(this, i18n("\
<h3>KPendulum Screen Saver for KDE</h3>\
<p>Simulation of a two-part pendulum</p>\
<p>Copyright (c) Georg&nbsp;Drenkhahn 2004</p>\
<p><tt>georg-d@users.sourceforge.net</tt></p>"));
}

void KPendulumSetup::mEditLostFocusSlot(void)
{
   if (mEdit->hasAcceptableInput())
   {
      saver->setMassRatio(mEdit->text().toDouble());
   }
   else
   {  // write current setting back into input field
      QString text;
      text.setNum(saver->massRatio());
      mEdit->setText(text);
   }
}
void KPendulumSetup::lEditLostFocusSlot(void)
{
   if (lEdit->hasAcceptableInput())
   {
      saver->setLengthRatio(lEdit->text().toDouble());
   }
   else
   {  // write current setting back into input field
      QString text;
      text.setNum(saver->lengthRatio());
      lEdit->setText(text);
   }
}
void KPendulumSetup::gEditLostFocusSlot(void)
{
   if (gEdit->hasAcceptableInput())
   {
      saver->setG(gEdit->text().toDouble());
   }
   else
   {  // write current setting back into input field
      QString text;
      text.setNum(saver->g());
      gEdit->setText(text);
   }
}
void KPendulumSetup::eEditLostFocusSlot(void)
{
   if (eEdit->hasAcceptableInput())
   {
      saver->setE(eEdit->text().toDouble());
   }
   else
   {  // write current setting back into input field
      QString text;
      text.setNum(saver->E());
      eEdit->setText(text);
   }
}
void KPendulumSetup::persChangeEnteredSlot(int t)
{
   saver->setPersChangeInterval(t);
}

void KPendulumSetup::barColorButtonClickedSlot(void)
{
   QColor color = QColorDialog::getColor(
      saver->barColor(), this, "bar color dialog");
   if (color.isValid())
   {
      saver->setBarColor(color);
      barColorButton->setPaletteBackgroundColor(color);
   }
}
void KPendulumSetup::m1ColorButtonClickedSlot(void)
{
   QColor color = QColorDialog::getColor(
      saver->m1Color(), this, "mass 1 color dialog");
   if (color.isValid())
   {
      saver->setM1Color(color);
      m1ColorButton->setPaletteBackgroundColor(color);
   }
}
void KPendulumSetup::m2ColorButtonClickedSlot(void)
{
   QColor color = QColorDialog::getColor(
      saver->m2Color(), this, "mass 2 color dialog");
   if (color.isValid())
   {
      saver->setM2Color(color);
      m2ColorButton->setPaletteBackgroundColor(color);
   }
}
