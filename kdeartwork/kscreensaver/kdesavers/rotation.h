//============================================================================
//
// KRotation screen saver for KDE
// Copyright (C) 2004 Georg Drenkhahn
// $Id: rotation.h 543449 2006-05-22 04:17:51Z howells $
//
//============================================================================

#ifndef __ROTATION_H__
#define __ROTATION_H__

#include <math.h>
// STL headers
#include <valarray>
// Qt headers
#include <qwidget.h>
#include <qtimer.h>
#include <qgl.h>
// GL headers
#include <GL/glu.h>
#include <GL/gl.h>
// KDE headers
#include <kscreensaver.h>

#include "vec3.h"
#include "rkodesolver.h"

// KRotationSetupUi
#include "rotationcfg.h"

//--------------------------------------------------------------------

/** @brief ODE solver for the Euler equations.
 *
 * Class implements RkOdeSolver<double> to solve the Euler equations of motion
 * tor the rotating object. */
class EulerOdeSolver : public RkOdeSolver<double>
{
  public:
   /** @brief Constructor for the ODE solver for the Euler equations.
    * @param t Time in seconds, integration variable
    * @param dt Initial time increment in seconds for integration, auto adjusted
    * later to guarantee precision
    * @param _A Moment of inertia along 1. figure axis
    * @param _B Moment of inertia along 2. figure axis
    * @param _C Moment of inertia along 3. figure axis
    * @param _y Vector of 12 elements containing the initial rotation vector
    * omega (elements 0 to 2), and the initial rotating systems coordinate
    * vectors e1, e2, e3 (elements 3 to 5, 6 to 8, and 9 to 11).
    * @param eps Relative precision per integration step, see
    * RkOdeSolver::RkOdeSolver(). */
   EulerOdeSolver(
      const double &t_,
      const double &dt_,
      const double &A_,
      const double &B_,
      const double &C_,
      std::valarray<double> &y_,
      const double &eps);

  protected:
   /** @brief ODE function for the Euler equation system
    * @param x time in seconds
    * @param y Vector of 12 elements containing the rotation vector omega
    * (elements 0 to 2), and the rotating systems coordinate vectors e1, e2, e3
    * (elements 3 to 5, 6 to 8, and 9 to 11).
    * @return derivation dy/dx */
   std::valarray<double>
      f(const double &x, const std::valarray<double> &y) const;

  private:
   /** Moments of inertia along the three figure axes */
   double A, B, C;
};


//--------------------------------------------------------------------

/** @brief GL widget class for the KRotation screen saver
 *
 * Class implements QGLWidget to display the KRotation screen saver. */
class RotationGLWidget : public QGLWidget
{
   Q_OBJECT

  public:
   /** @brief Constructor of KRotation's GL widget
    * @param parent parent widget, passed to QGLWidget's constructor
    * @param name name of widget, passed to QGLWidget's constructor
    * @param omega current rotation vector
    * @param e1 x trace data
    * @param e2 y trace data
    * @param e3 z trace data
    * @param J 3 vector with momenta of inertia with respect to the 3 figure
    * axes. */
   RotationGLWidget(QWidget* parent, const char* name,
                    const vec3<double>& omega,
                    const std::deque<vec3<double> >& e1,
                    const std::deque<vec3<double> >& e2,
                    const std::deque<vec3<double> >& e3,
                    const vec3<double>& J);

  protected:
   /** Called if scenery (GL view) must be updated */
   virtual void paintGL();
   /** Called if gl widget was resized.  Method makes adjustments for new
    * perspective */
   virtual void resizeGL(int w, int h);
   /** Setup the GL enviroment */
   virtual void initializeGL();

  private:
   /** @brief Draw 3D arrow
    * @param total_length total length of arrow
    * @param head_length length of arrow head (cone)
    * @param base_width width of arrow base
    * @param head_width width of arrow head (cone)
    *
    * The arrow is drawn from the coordinates zero point along th z direction.
    * The cone's tip is located at (0,0,@a total_length). */
   void myGlArrow(GLfloat total_length, GLfloat head_length,
                  GLfloat base_width,   GLfloat head_width);
   /** Draw the traces in the GL area */
   void draw_traces (void);

  private: // Private attributes
   /** Eye position distance from coordinate zero point */
   GLfloat eyeR;
   /** Eye position theta angle from z axis */
   GLfloat eyeTheta;
   /** Eye position phi angle (longitude) */
   GLfloat eyePhi;
   /** Box size */
   vec3<double> boxSize;
   /** GL object list of fixed coordinate systems axses */
   GLuint fixedAxses;
   /** GL object list of rotating coordinate systems axses */
   GLuint bodyAxses;
   /** Light position distance from coordinate zero point */
   GLfloat lightR;
   /** Light position theta angle from z axis */
   GLfloat lightTheta;
   /** Light position phi angle (longitude) */
   GLfloat lightPhi;

   /** stores position where the mouse button was pressed down */
   QPoint mouse_press_pos;

   /** Length of the rotating coordinate system axses */
   GLfloat bodyAxsesLength;
   /** Length of the fixed coordinate system axses */
   GLfloat fixedAxsesLength;

   /** The openGL rotation matrix for the box. */
   GLfloat rotmat[16];

   /** reference to current rotation vector */
   const vec3<double>& omega;
   /** reference to x trace values */
   const std::deque<vec3<double> >& e1;
   /** reference to y trace values */
   const std::deque<vec3<double> >& e2;
   /** reference to z trace values */
   const std::deque<vec3<double> >& e3;
};

//--------------------------------------------------------------------

/** @brief Main class of the KRotation screen saver
 *
 * This class implements KScreenSaver for the KRotation screen saver. */
class KRotationSaver : public KScreenSaver
{
   Q_OBJECT

  public:
   /** @brief Constructor of the KRotation screen saver object
    * @param drawable Id of the window in which the screen saver is drawed
    *
    * Initial settings are read from disk, the GL widget is set up and displayed
    * and the eq. of motion solver is started. */
   KRotationSaver(WId drawable);
   /** @brief Destructor of the KPendulum screen saver object
    *
    * Only KPendulumSaver::solver is destoyed. */
   ~KRotationSaver();
   /** read the saved settings from disk */
   void readSettings();
   /** init physical quantities and set up the ode solver */
   void initData();

   /** Returns length of traces in seconds of visibility, parameter from setup
    * dialog */
   inline double traceLengthSeconds(void) const {return m_traceLengthSeconds;}
   /** Sets the length of traces in seconds of visibility. */
   void setTraceLengthSeconds(const double& t);
   /** Lower argument limit for setTraceLengthSeconds() */
   static const double traceLengthSecondsLimitLower;
   /** Upper argument limit for setTraceLengthSeconds() */
   static const double traceLengthSecondsLimitUpper;
   /** Default value of KRotationSaver::m_traceLengthSeconds */
   static const double traceLengthSecondsDefault;

   /** Flags indicating if the traces for x,y,z are shown.  Only relevant if
    * ::randomTraces is not set to true.  Parameter from setup dialog */
   inline bool traceFlag(unsigned int n) const {return m_traceFlag[n];}
   /** (Un)Sets the x,y,z traces flags. */
   inline void setTraceFlag(unsigned int n, const bool& flag)
      {m_traceFlag[n] = flag;}
   /** Default values for KRotationSaver::m_traceFlag */
   static const bool traceFlagDefault[3];

   /** If flag is set to true the traces will be (de)activated randomly all 10
    * seconds.  Parameter from setup dialog */
   inline bool randomTraces(void) const {return m_randomTraces;}
   /** (Un)Sets the random trace flag. */
   inline void setRandomTraces(const bool& flag) {m_randomTraces = flag;}
   /** Default value for KRotationSaver::m_randomTraces */
   static const bool randomTracesDefault = true;

   /** Returns the angular momentum. */
   inline double Lz(void) const {return m_Lz;}
   /** Sets the angular momentum. */
   void setLz(const double& Lz);
   /** Lower argument limit for setLz() */
   static const double LzLimitLower;
   /** Upper argument limit for setLz() */
   static const double LzLimitUpper;
   /** Default value for KRotationSaver::m_Lz */
   static const double LzDefault;

   /** Returns initial eulerian angle theta of the top body at t=0 sec. */
   inline double initEulerTheta(void) const {return m_initEulerTheta;}
   /** Set the initial eulerian angle theta of the top body at t=0 sec. */
   void setInitEulerTheta(const double& theta);
   /** Lower argument limit for setInitEulerTheta() */
   static const double initEulerThetaLimitLower;
   /** Upper argument limit for setInitEulerTheta() */
   static const double initEulerThetaLimitUpper;
   /** Default value for KRotationSaver::m_initEulerTheta */
   static const double initEulerThetaDefault;

  public slots:
   /** slot is called if integration should proceed by ::deltaT */
   void doTimeStep();
   /** slot is called if setup dialog changes in size and the GL area should be
    * adjusted */
   void resizeGlArea(QResizeEvent* e);

  private:
   /** Momentum of inertia along figure axes */
   vec3<double> J;
   /** Initial eulerian angles phi of the top body at t=0s */
   double initEulerPhi;
   /** Initial eulerian angles psi of the top body at t=0s */
   double initEulerPsi;

   /** The ode solver which is used to integrate the equations of motion */
   EulerOdeSolver*   solver;
   /** Gl widget of simulation */
   RotationGLWidget* glArea;
   /** Timer for the real time integration of the Euler equations */
   QTimer*           timer;

   /** current rotation vector */
   vec3<double> omega;
   /** deque of unit vectors of e1 figure axes in fixed frame coordinates */
   std::deque<vec3<double> > e1;
   /** deque of unit vectors of e2 figure axes in fixed frame coordinates */
   std::deque<vec3<double> > e2;
   /** deque of unit vectors of e3 figure axes in fixed frame coordinates */
   std::deque<vec3<double> > e3;

   /** Time step size for the integration in milliseconds.  Used in
    * ::KRotationSaver and ::RotationGLWidget. */
   static const unsigned int deltaT = 20;

   /** Length of traces in seconds of visibility, parameter from setup dialog */
   double m_traceLengthSeconds;
   /** Flags indicating if the traces for x,y,z are shown.  Only relevant if
    * ::randomTraces is not set to true.  Parameter from setup dialog */
   bool m_traceFlag[3];
   /** If flag is set to true the traces will be (de)activated randomly all 10
    * seconds.  Parameter from setup dialog */
   bool m_randomTraces;
   /** Angular momentum.  This is a constant of motion and points always into
    * positive z direction.  Parameter from setup dialog */
   double m_Lz;
   /** Initial eulerian angles theta of the top body at t=0 sec.  Parameter from
    * setup dialog */
   double m_initEulerTheta;
};

//--------------------------------------------------------------------

/** @brief KRotation screen saver setup dialog.
 *
 * This class handles the KRotation screen saver setup dialog. */
class KRotationSetup : public KRotationSetupUi
{
   Q_OBJECT

  public:
   KRotationSetup(QWidget* parent = NULL, const char* name = NULL);
   ~KRotationSetup();

  public slots:
   /// slot for the OK Button: save settings and exit
   void okButtonClickedSlot(void);
   /// slot for the About Button: show the About dialog
   void aboutButtonClickedSlot(void);
   void randomTracesToggled(bool state);
   void xTraceToggled(bool state);
   void yTraceToggled(bool state);
   void zTraceToggled(bool state);
   void lengthEnteredSlot(const QString& s);
   void LzEnteredSlot(const QString& s);
   void thetaEnteredSlot(const QString& s);

  private:
   /// the screen saver widget which is displayed in the preview area
   KRotationSaver* saver;
};

#endif
