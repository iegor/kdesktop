/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2000-2003 by Andreas Zehender
    email                : zehender@kde.org
**************************************************************************

**************************************************************************
*                                                                        *
*  This program is free software; you can redistribute it and/or modify  *
*  it under the terms of the GNU General Public License as published by  *
*  the Free Software Foundation; either version 2 of the License, or     *
*  (at your option) any later version.                                   *
*                                                                        *
**************************************************************************/

#include "pmglview.h"
#include "pmpart.h"
#include "pmrendermanager.h"
#include "pmcamera.h"
#include "pmscene.h"
#include "pmdatachangecommand.h"
#include "pmdebug.h"
#include "pmdefaults.h"

#include <math.h>
#include <qpopupmenu.h>
#include <qpainter.h>
#include <qapplication.h>
#include <qcursor.h>
#include <qcolor.h>
#include <qglobal.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qdom.h>

#include <kxmlguifactory.h>
#include <kaction.h>
#include <kconfig.h>
#include <kstaticdeleter.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kdialog.h>

#include <GL/glx.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/Xmu/StdCmap.h>

const double c_sizeFactor = log( 2.0 ) / 100.0;
const int c_mouseChangeDelayPixel = 3;
const int c_mouseChangeDelayMs = 300;
const int c_multipleSelectDelayPixel = 3;
const double c_defaultAutoScrollSpeed = 200; // pixels per second
const int c_minAutoScrollUpdateTime = 30; //ms

const double keyMoveSpeed = 40.0;
const double keyScaleFactor = 1.4;

static int glxAttributeList[] = { GLX_LEVEL, 0,
                                  GLX_DOUBLEBUFFER,
                                  GLX_RGBA,
                                  GLX_RED_SIZE, 1,
                                  GLX_GREEN_SIZE, 1,
                                  GLX_BLUE_SIZE, 1,
                                  //GLX_ALPHA_SIZE, 1,
                                  GLX_DEPTH_SIZE, 1,
                                  None };

class PMGLViewStatic
{
public:
   PMGLViewStatic( )
   {
      m_colormap = 0;
      m_context = NULL;
      m_colormapAllocated = false;
      m_display = 0;
      m_visualInfo = 0;
   }
   ~PMGLViewStatic( )
   {
      if( m_colormapAllocated )
         XFreeColormap( m_display, m_colormap );
      if( m_context != NULL )
         glXDestroyContext( m_display, m_context );
      if( m_visualInfo )
         XFree( m_visualInfo );
   }

   Colormap m_colormap;
   GLXContext m_context;
   bool m_colormapAllocated;
   Display* m_display;
   XVisualInfo* m_visualInfo;
};

static PMGLViewStatic* s_pSharedData = 0;
static KStaticDeleter<PMGLViewStatic> s_staticDeleter;
bool PMGLView::s_bDirect = true;


PMGLView::PMGLView( PMPart* part, PMViewType t,
                    QWidget* parent, const char* name, WFlags f )
      : PMViewBase( parent, name, f | Qt::WWinOwnDC | Qt::WRepaintNoErase )
{
   m_pPart = part;
   m_type = t;
   m_bScaleMode = false;
   m_scaleIntX = 0.0;
   m_scaleIntY = 0.0;
   m_bTranslateMode = false;
   m_bGraphicalChangeMode = false;
   m_bMousePressed = false;
   m_bMidMousePressed = false;
   m_dTransX = 0.0;
   m_dTransY = 0.0;
   m_dScale = 30.0;
   m_bInverseValid = false;
   m_pActiveObject = part->activeObject( );
   m_bMementoCreated = false;
   m_bSelectUnderMouse = false;
   m_bDeselectUnderMouse = false;
   m_bMultipleSelectionMode = false;
   m_bSelectionStarted = false;
   m_bAutoScroll = false;
   m_autoScrollSpeed = c_defaultAutoScrollSpeed;
   m_bAboutToUpdate = false;
   m_projectionUpToDate = false;
   m_contextClickPosition = PMVector( 0.0, 0.0 );
   m_objectActions.setAutoDelete( true );

   m_controlPointsPosition.setAutoDelete( true );
   m_pUnderMouse = 0;

   setCamera( m_pPart->firstCamera( ) );

   initializeGL( );

   setMouseTracking( true );
   setFocusPolicy( WheelFocus );

   PMRenderManager* rm = PMRenderManager::theManager( );
   rm->viewCreated( );

   setMinimumSize( 50, 50 );

   connect( part, SIGNAL( refresh( ) ), SLOT( slotRefresh( ) ) );
   connect( part, SIGNAL( clear( ) ), SLOT( slotClear( ) ) );

   connect( this, SIGNAL( objectChanged( PMObject*, const int, QObject* ) ),
            part, SLOT( slotObjectChanged( PMObject*, const int, QObject* ) ) );
   connect( part, SIGNAL( objectChanged( PMObject*, const int, QObject* ) ),
            SLOT( slotObjectChanged( PMObject*, const int, QObject* ) ) );

   connect( part, SIGNAL( activeRenderModeChanged( ) ),
            SLOT( slotActiveRenderModeChanged( ) ) );

   connect( &m_startTimer, SIGNAL( timeout( ) ),
            SLOT( slotMouseChangeTimer( ) ) );
   connect( &m_autoScrollTimer, SIGNAL( timeout( ) ),
            SLOT( slotAutoScroll( ) ) );

   connect( rm, SIGNAL( renderingStarted( PMGLView* ) ),
            SLOT( slotRenderingStarted( PMGLView* ) ) );
   connect( rm, SIGNAL( aboutToUpdate( PMGLView* ) ),
            SLOT( slotAboutToUpdate( PMGLView* ) ) );
   connect( rm, SIGNAL( renderingFinished( PMGLView* ) ),
            SLOT( slotRenderingFinished( PMGLView* ) ) );
   connect( rm, SIGNAL( renderingSettingsChanged( ) ),
            SLOT( slotRefresh( ) ) );

   connect( this, SIGNAL( controlPointMessage( const QString& ) ),
            m_pPart, SIGNAL( controlPointMessage( const QString& ) ) );

   updateControlPoints( );
}

void PMGLView::initializeGL( )
{
   Display* display = x11Display( );
   int screen = x11Screen( );
   int i;

   if( !s_pSharedData )
   {
      s_staticDeleter.setObject( s_pSharedData, new PMGLViewStatic );
      s_pSharedData->m_display = display;

      if( PMRenderManager::hasOpenGL( ) )
      {
         // get an appropriate visual
         XVisualInfo* vi = glXChooseVisual( display, screen, glxAttributeList );
         s_pSharedData->m_visualInfo = vi;

         if( vi )
         {
            kdDebug( PMArea ) << "PMGLView: XVisual found\n";

            // create a color map (from qgl_x11.cpp)
            // check if we can use the global colormap
            // should be the default ?
            if( vi->visualid ==
                XVisualIDFromVisual( ( Visual* ) QPaintDevice::x11AppVisual( ) ) )
            {
               kdDebug( PMArea ) << "PMGLView: Default colormap used\n";
               s_pSharedData->m_colormap = QPaintDevice::x11AppColormap();
               s_pSharedData->m_colormapAllocated = false;
            }

            if( !s_pSharedData->m_colormap )
            {
               const char* v = glXQueryServerString( display, vi->screen,
                                                     GLX_VERSION );
               bool mesa_gl = false;
               if( v )
                  mesa_gl = strstr( v, "Mesa" ) != 0;
               if( mesa_gl )
               {
                  XStandardColormap* c;
                  int n;
                  Atom hp_cmaps = XInternAtom( display, "_HP_RGB_SMOOTH_MAP_LIST",
                                               TRUE );

                  if( hp_cmaps && ( vi->visual->c_class == TrueColor )
                      && ( vi->depth == 8 ) )
                  {
                     if( XGetRGBColormaps( display, RootWindow( display,vi->screen ),
                                           &c, &n, hp_cmaps ) )
                     {
                        i = 0;
                        while( ( i < n ) && ( s_pSharedData->m_colormap == 0 ) )
                        {
                           if( c[i].visualid == vi->visual->visualid )
                           {
                              s_pSharedData->m_colormap = c[i].colormap;
                              kdDebug( PMArea ) << "PMGLView: HP_RGB scmap used\n";
                           }
                           i++;
                        }
                        XFree( ( char* ) c );
                     }
                  }
               }
            }

#if !defined(Q_OS_SOLARIS)
            if( !s_pSharedData->m_colormap )
            {
               XStandardColormap* c;
               int n;

               if( XmuLookupStandardColormap( display, vi->screen, vi->visualid,
                                              vi->depth, XA_RGB_DEFAULT_MAP,
                                              FALSE, TRUE ) )
               {
                  if( XGetRGBColormaps( display, RootWindow( display, vi->screen ),
                                        &c, &n, XA_RGB_DEFAULT_MAP ) )
                  {
                     i = 0;
                     while( ( i < n ) && ( s_pSharedData->m_colormap == 0 ) )
                     {
                        if( c[i].visualid == vi->visualid )
                        {
                           s_pSharedData->m_colormap = c[i].colormap;
                           kdDebug( PMArea ) << "PMGLView: RGB_DEFAULT scmap used\n";
                        }
                        i++;
                     }
                     XFree( ( char* ) c );
                  }
               }
            }
#endif

            if( !s_pSharedData->m_colormap )
            {
               // create a new colormap otherwise
               kdDebug( PMArea ) << "PMGLView: New colormap created\n";
               s_pSharedData->m_colormap =
                  XCreateColormap( display,
                                   RootWindow( display, vi->screen ),
                                   vi->visual, AllocNone );
               s_pSharedData->m_colormapAllocated = true;
            }

            // create the context
            // this context is shared between all gl views!
            s_pSharedData->m_context = glXCreateContext( display, vi, 0, s_bDirect );

            if( s_pSharedData->m_context != NULL )
               kdDebug( PMArea ) << "PMGLView: glx context created\n";

         }
      }
   }

   if( s_pSharedData->m_context != NULL )
   {
      XVisualInfo* vi = s_pSharedData->m_visualInfo;

      // create the window
      XSetWindowAttributes swa;
      swa.colormap = s_pSharedData->m_colormap;
      swa.border_pixel = 0;
      swa.background_pixel = 0;

      Window p;
      p = RootWindow( display, vi->screen );
      QWidget* pw = parentWidget( );
      if( pw )
         p = pw->winId( );

      Window w = XCreateWindow( display, p, x( ), y( ), width( ),
                                height( ), 0, vi->depth, InputOutput,
                                vi->visual,
                                CWColormap | CWBorderPixel | CWBackPixel,
                                &swa );

      // tell the windowmanager to use the colormap
      Window* colorMapWindows = 0;
      Window* newWindows = 0;
      int num;
      if( XGetWMColormapWindows( display, topLevelWidget( )->winId( ),
                                 &colorMapWindows, &num ) )
      {
         // create a new list and append the new window
         bool replaced = false;
         newWindows = new Window[num+1];

         for( i = 0; i < num; i++ )
         {
            newWindows[i] = colorMapWindows[i];
            if( newWindows[i] == winId( ) ) // old window
            {
               newWindows[i] = w;
               replaced = true;
            }
         }
         if( !replaced )
         {
            newWindows[num] = w;
            num++;
         }
      }
      else
      {
         num = 1;
         newWindows = new Window[1];
         newWindows[0] = w;
      }
      // tell Qt to use this window
      create( w );

      XSetWMColormapWindows( display, topLevelWidget( )->winId( ),
                             newWindows, num );
      delete[] newWindows;

      XFlush( x11Display( ) );
   }
   else
   {
      QVBoxLayout* topLayout = new QVBoxLayout( this );
      QLabel* label = new QLabel( i18n( "No OpenGL support" ), this );
      label->setAlignment( Qt::AlignCenter );
      topLayout->addWidget( label );
   }

   //setProjection( );
}

PMGLView::~PMGLView( )
{
   PMRenderManager* rm = PMRenderManager::theManager( );
   rm->removeView( this );
   rm->viewDeleted( );
   emit destroyed( this );
}

bool PMGLView::isValid( ) const
{
   if( s_pSharedData && ( s_pSharedData->m_context != NULL ) )
      return true;
   return false;
}

void PMGLView::makeCurrent( )
{
   if( isValid( ) )
      glXMakeCurrent( x11Display( ), winId( ), s_pSharedData->m_context );
}

void PMGLView::swapBuffers( )
{
   if( isValid( ) )
      glXSwapBuffers( x11Display( ), winId( ) );
}

void PMGLView::setScale( double scale )
{
   if( m_dScale > 0 )
   {
      m_dScale = scale;
      invalidateProjection( );
   }
   else
      kdError( PMArea ) << "Scale <= 0 in PMGLView::setScale\n";
}

void PMGLView::setTranslationX( double d )
{
   m_dTransX = d;
   invalidateProjection( );
}

void PMGLView::setTranslationY( double d )
{
   m_dTransY = d;
   invalidateProjection( );
}

void PMGLView::resizeEvent( QResizeEvent* )
{
   invalidateProjection( );
}

void PMGLView::paintEvent( QPaintEvent* )
{
   repaint( );
}

void PMGLView::invalidateProjection( bool graphicalChange /*= true*/ )
{
   m_viewTransformation = PMMatrix::identity( );


   if( m_type != PMViewCamera )
   {
      m_viewTransformation = m_viewTransformation * PMMatrix::scale( m_dScale, m_dScale, m_dScale );
      m_viewTransformation = m_viewTransformation * PMMatrix::translation( m_dTransX, m_dTransY, 0 );

      switch( m_type )
      {
         case PMViewPosZ:
            m_normal = PMVector( 0.0, 0.0, 1.0 );
            break;
         case PMViewNegZ:
            m_viewTransformation = m_viewTransformation * PMMatrix::rotation( 0.0, M_PI, 0.0 );
            m_normal = PMVector( 0.0, 0.0, -1.0 );
            break;
         case PMViewNegY:
            m_viewTransformation = m_viewTransformation * PMMatrix::rotation( M_PI_2, 0.0, 0.0 );
            m_normal = PMVector( 0.0, -1.0, 0.0 );
            break;
         case PMViewPosY:
            m_normal = PMVector( 0.0, 1.0, 0.0 );
            m_viewTransformation = m_viewTransformation * PMMatrix::rotation( -M_PI_2, 0.0, 0.0 );
            break;
         case PMViewPosX:
            m_viewTransformation = m_viewTransformation * PMMatrix::rotation( 0.0, M_PI_2, 0.0 );
            m_normal = PMVector( 1.0, 0.0, 0.0 );
            break;
         case PMViewNegX:
            m_viewTransformation = m_viewTransformation * PMMatrix::rotation( 0.0, -M_PI_2, 0.0 );
            m_normal = PMVector( -1.0, 0.0, 0.0 );
            break;
         default:
            break;
      }

      m_viewTransformation = m_viewTransformation * PMMatrix::scale( 1.0, 1.0, -1.0 );

      if( m_controlPoints.count( ) > 0 )
         recalculateTransformations( );
      recalculateControlPointPosition( );
   }
   m_projectionUpToDate = false;
   repaint( graphicalChange );
}

void PMGLView::enableTranslateMode( bool yes )
{
   if( m_type != PMViewCamera )
   {
      m_bScaleMode = false;
      m_bTranslateMode = yes;
      setCursor( yes ? crossCursor : arrowCursor );
   }
}

void PMGLView::enableScaleMode( bool yes )
{
   if( m_type != PMViewCamera )
   {
      m_bScaleMode = yes;
      m_bTranslateMode = false;
      setCursor( yes ? crossCursor : arrowCursor );
   }
}

void PMGLView::mousePressEvent( QMouseEvent* e )
{
   if( m_bScaleMode || m_bTranslateMode )
   {
      if( ( e->button( ) & LeftButton ) && ( e->state( ) == 0 ) )
      {
         m_bMousePressed = true;
         m_mousePos = e->pos( );
         m_scaleIntX = screenToInternalX( e->x( ) );
         m_scaleIntY = screenToInternalY( e->y( ) );
      }
   }
   else if( m_type != PMViewCamera )
   {
      if( ( e->button( ) & LeftButton ) && m_bInverseValid
          && m_pActiveObject )
      {
         if( m_pUnderMouse )
         {
            // check the control point selection
            if( m_pActiveObject->multipleSelectControlPoints( ) )
            {
               if( m_pUnderMouse->selected( ) )
               {
                  if( e->state( ) & ControlButton )
                     m_bDeselectUnderMouse = true;
                  else
                     m_bSelectUnderMouse = true;
               }
               else
               {
                  if( e->state( ) & ControlButton )
                     selectControlPoint( m_pUnderMouse,
                                         !m_pUnderMouse->selected( ), false );
                  else
                     selectControlPoint( m_pUnderMouse, true );
               }
            }
            else
               selectControlPoint( m_pUnderMouse, true );

            // start the graphical change
            if( !m_bGraphicalChangeMode )
            {
               m_bGraphicalChangeMode = true;
               m_bMementoCreated = false;
               m_changeStartPos = e->pos( );
               m_changeStartTime = QTime::currentTime( );
               m_currentMousePos = m_changeStartPos;
               m_startTimer.start( c_mouseChangeDelayMs, true );
            }
         }
         else
         {
            if( m_pActiveObject->multipleSelectControlPoints( ) )
            {
               // multiple selection mode
               // start only when the view is rendered
               if( !PMRenderManager::theManager( )->containsTask( this ) )
               {
                  m_bMultipleSelectionMode = true;
                  m_bSelectionStarted = false;
                  m_selectionStart = e->pos( );
                  m_selectionEnd = m_selectionStart;
               }
            }
            else
               selectControlPoint( 0, false );
         }
      }
   }


   if( !( m_bGraphicalChangeMode || m_bMousePressed ) )
   {
      if( ( e->button( ) == RightButton ) && ( e->state( ) == 0 ) )
      {
         m_contextClickPosition = PMVector( screenToInternalX( e->x( ) ),
                                            screenToInternalY( e->y( ) ) );

         if( m_pUnderMouse )
         {
            // check the control point selection
            if( m_pActiveObject->multipleSelectControlPoints( ) )
            {
               if( !m_pUnderMouse->selected( ) )
                  selectControlPoint( m_pUnderMouse, true );
            }
            else
               selectControlPoint( m_pUnderMouse, true );
         }

         contextMenu( );
      }
   }

   if( e->button( ) == MidButton )
   {
      m_bMidMousePressed = true;
      m_mousePos = e->pos( );
   }
}

void PMGLView::mouseReleaseEvent( QMouseEvent* e )
{
   m_bMousePressed = false;
   if( m_bGraphicalChangeMode )
   {
      m_startTimer.stop( );

      if( m_bMementoCreated )
      {
         PMDataChangeCommand* cmd;
         cmd = new PMDataChangeCommand( m_pActiveObject->takeMemento( ) );
         m_pPart->executeCommand( cmd );

         checkUnderMouse( ( int ) screenToInternalX( e->x( ) ),
                          ( int ) screenToInternalY( e->y( ) ) );
      }
      else
      {
         if( m_pUnderMouse )
         {
            if( m_bSelectUnderMouse )
               selectControlPoint( m_pUnderMouse, true );
            else if( m_bDeselectUnderMouse )
               selectControlPoint( m_pUnderMouse, false, false );
         }
      }
      m_bGraphicalChangeMode = false;
   }
   else if( m_bMultipleSelectionMode )
   {
      if( m_bSelectionStarted )
      {
         int sx, sy, ex, ey, w, h;
         double isx, isy, iex, iey;
         QPtrListIterator<PMVector> pit( m_controlPointsPosition );
         PMControlPointListIterator cit( m_controlPoints );
         PMVector p;

         calculateSelectionBox( sx, sy, ex, ey, w, h );
         isx = screenToInternalX( sx );
         iex = screenToInternalX( ex );
         isy = screenToInternalY( ey );
         iey = screenToInternalY( sy );
         restoreSelectionBox( );

         while( pit.current( ) && cit.current( ) )
         {
            p = *( pit.current( ) );

            if( ( isx <= p[0] ) && ( iex >= p[0] )
                && ( isy <= p[1] ) && ( iey >= p[1] ) )
               selectControlPoint( cit.current( ), true, false );
            else if( !( e->state( ) & ControlButton ) )
               selectControlPoint( cit.current( ), false, false );

            ++cit;
            ++pit;
         }
      }
      else
         selectControlPoint( 0, false );

      m_bMultipleSelectionMode = false;
   }

   if( m_bAutoScroll )
   {
      m_bAutoScroll = false;
      m_autoScrollTimer.stop( );
   }

   if( e->button( ) & QEvent::MidButton )
      m_bMidMousePressed = false;

   m_bSelectUnderMouse = false;
   m_bDeselectUnderMouse = false;
}

void PMGLView::mouseMoveEvent( QMouseEvent* e )
{
   if( m_bMousePressed )
   {
      if( m_bScaleMode )
      {
         int d = e->x( ) - m_mousePos.x( );
         if( d != 0 )
         {
            double s = exp( d * c_sizeFactor );
            double c = 1.0 / ( m_dScale * s ) - 1.0 / m_dScale;
            m_dTransX += m_scaleIntX * c;
            m_dTransY += m_scaleIntY * c;
            m_dScale *= s;
            invalidateProjection( );
         }
      }
      else if( m_bTranslateMode )
      {
         m_dTransX += ( e->x( ) - m_mousePos.x( ) ) / m_dScale;
         m_dTransY -= ( e->y( ) - m_mousePos.y( ) ) / m_dScale;
         invalidateProjection( );
      }
      m_mousePos = e->pos( );
   }
   else if( m_bMidMousePressed )
   {
      m_dTransX += ( e->x( ) - m_mousePos.x( ) ) / m_dScale;
      m_dTransY -= ( e->y( ) - m_mousePos.y( ) ) / m_dScale;
      invalidateProjection( );

      m_mousePos = e->pos( );
   }
   else if( m_bGraphicalChangeMode )
   {
      m_currentMousePos = e->pos( );
      if( !m_bMementoCreated )
      {
         QPoint movement = e->pos( ) - m_changeStartPos;
         if( ( m_changeStartTime.msecsTo( QTime::currentTime( ) ) > c_mouseChangeDelayMs )
            || ( movement.manhattanLength( ) > c_mouseChangeDelayPixel ) )
         {
            m_startTimer.stop( );
            startChange( m_changeStartPos );
         }
      }

      if( m_bMementoCreated )
         graphicalChange( e->pos( ) );
   }
   else if( m_bMultipleSelectionMode )
   {
      if( !m_bSelectionStarted )
      {
         m_selectionEnd = e->pos( );
         startSelection( );
      }
      else
      {
         restoreSelectionBox( );
         m_selectionEnd = e->pos( );
         saveSelectionBox( );
         paintSelectionBox( );
      }
   }
   else if( !( m_bScaleMode || m_bTranslateMode ) )
   {
      checkUnderMouse( ( int ) screenToInternalX( e->x( ) ),
                       ( int ) screenToInternalY( e->y( ) ) );
   }

   if( m_bMultipleSelectionMode || m_bGraphicalChangeMode )
   {
      bool as = m_bAutoScroll;

      if( e->x( ) < 0 )
         m_autoScrollDirectionX = 1;
      else if( e->x( ) >= width( ) )
         m_autoScrollDirectionX = -1;
      else
         m_autoScrollDirectionX = 0;

      if( e->y( ) < 0 )
         m_autoScrollDirectionY = 1;
      else if( e->y( ) >= height( ) )
         m_autoScrollDirectionY = -1;
      else
         m_autoScrollDirectionY = 0;

      if( ( m_autoScrollDirectionX != 0 ) || ( m_autoScrollDirectionY != 0 ) )
         m_bAutoScroll = true;
      else
         m_bAutoScroll = false;

      if( m_bAutoScroll && !as )
      {
         m_lastAutoScrollUpdate = QTime::currentTime( );
         m_autoScrollTimer.start( c_minAutoScrollUpdateTime, true );
      }
      if( !m_bAutoScroll && as )
         m_autoScrollTimer.stop( );
   }
}

void PMGLView::wheelEvent( QWheelEvent* e )
{
   if( m_type != PMViewCamera )
   {
      double s = exp( e->delta( ) / 4 * c_sizeFactor );
      double deltaX = screenToInternalX( e->x( ) );
      double deltaY = screenToInternalY( e->y( ) );
      double c = 1.0 / ( m_dScale * s ) - 1.0 / m_dScale;
      m_dTransX += deltaX * c;
      m_dTransY += deltaY * c;
      m_dScale *= s;
      invalidateProjection( );
   }
}

void PMGLView::keyPressEvent( QKeyEvent* e )
{
   bool accept = true;

   if( e->state( ) == 0 )
   {
      if( m_type != PMViewCamera )
      {
         if( m_dScale > 0 )
         {
            switch( e->key( ) )
            {
               case Key_Left:
                  m_dTransX -= keyMoveSpeed / m_dScale;
                  break;
               case Key_Right:
                  m_dTransX += keyMoveSpeed / m_dScale;
                  break;
               case Key_Up:
                  m_dTransY += keyMoveSpeed / m_dScale;
                  break;
               case Key_Down:
                  m_dTransY -= keyMoveSpeed / m_dScale;
                  break;
               default:
                  accept = false;
            }
         }
         else
            kdError( PMArea ) << "scale <= 0 in PMGLView::keyPressEvent\n";
      }
   }
   else if( e->state( ) == ControlButton )
   {
      if( m_type != PMViewCamera )
      {
         switch( e->key( ) )
         {
            case Key_Left:
            case Key_Down:
               m_dScale /= keyScaleFactor;
               break;
            case Key_Right:
            case Key_Up:
               m_dScale *= keyScaleFactor;
               break;
            default:
               accept = false;
         }
      }
   }
   else
      accept = false;

   if( accept )
      invalidateProjection( );
   else
      e->ignore( );
}

void PMGLView::slotAutoScroll( )
{
   if( m_bAutoScroll )
   {
      QTime now = QTime::currentTime( );
      int msecs = m_lastAutoScrollUpdate.msecsTo( now );
      int pixels = ( int ) ( m_autoScrollSpeed * msecs / 1000.0 );

      if( pixels < 1 )
         pixels = 1;
      if( pixels > ( width( ) * 3 / 4 ) )
         pixels = width( ) * 3 / 4;
      if( pixels > ( height( ) * 3 / 4 ) )
         pixels = height( ) * 3 / 4;

      if( m_bGraphicalChangeMode && !m_bMementoCreated )
         startChange( m_changeStartPos );
      if( m_bMultipleSelectionMode )
         restoreSelectionBox( );

      m_dTransX += pixels * m_autoScrollDirectionX / m_dScale;
      m_dTransY -= pixels * m_autoScrollDirectionY / m_dScale;
      invalidateProjection( );

   if( m_bGraphicalChangeMode )
      if( m_bMultipleSelectionMode )
      {
         m_selectionStart += QPoint( pixels * m_autoScrollDirectionX,
                                     pixels * m_autoScrollDirectionY );

         saveSelectionBox( );
         paintSelectionBox( );
      }

      if( m_bGraphicalChangeMode )
         graphicalChange( mapFromGlobal( QCursor::pos( ) ) );
      else
         repaint( );

      m_lastAutoScrollUpdate = now;
   }
}

void PMGLView::slotMouseChangeTimer( )
{
   if( !m_bMementoCreated )
   {
      if( m_currentMousePos != m_changeStartPos )
      {
         startChange( m_changeStartPos );
         graphicalChange( m_currentMousePos );
      }
   }
}

void PMGLView::startChange( const QPoint& mousePos )
{
   m_pActiveObject->createMemento( );
   m_bMementoCreated = true;

   PMVector p = mousePosition( m_pUnderMouse, mousePos.x( ), mousePos.y( ) );
   p.transform( m_inversePointsTransformation );

   if( m_pActiveObject->multipleSelectControlPoints( ) )
   {
      PMControlPointListIterator it( m_controlPoints );
      for( ; it.current( ); ++it )
         if( it.current( )->selected( ) )
            it.current( )->startChange( p, m_normal );
   }
   else
      m_pUnderMouse->startChange( p, m_normal );
}

void PMGLView::graphicalChange( const QPoint& mousePos )
{
   PMVector p = mousePosition( m_pUnderMouse, mousePos.x( ), mousePos.y( ) );
   p.transform( m_inversePointsTransformation );
   if( m_pActiveObject->multipleSelectControlPoints( ) )
   {
      PMControlPointListIterator it( m_controlPoints );
      for( ; it.current( ); ++it )
         if( it.current( )->selected( ) )
            it.current( )->change( p );
   }
   else
      m_pUnderMouse->change( p );

   PMObjectList changedObjects;
   m_pActiveObject->controlPointsChangedList( m_controlPoints, changedObjects );

   if( changedObjects.isEmpty( ) )
      emit objectChanged( m_pActiveObject, PMCGraphicalChange, this );
   else
   {
      PMObjectListIterator it( changedObjects );
      for( ; it.current( ); ++it )
         emit objectChanged( it.current( ), PMCGraphicalChange, this );
   }
}

void PMGLView::checkUnderMouse( int x, int y )
{
   // is cursor over a control point?
   // double z;
   PMVector* v;
   PMControlPoint* p;
   PMControlPoint* old = m_pUnderMouse;

   if( m_bInverseValid && ( m_type != PMViewCamera ) )
   {
      m_pUnderMouse = 0;
//      z = -1e10;

      v = m_controlPointsPosition.first( );
      p = m_controlPoints.first( );

      while( p )
      {
         if( p->displayType( ) == PMControlPoint::CPCross )
         {
            if( !m_pUnderMouse )
               m_pUnderMouse = p;
         }
         else
         {
            if( ( fabs( x - (*v)[0] ) < ( controlPointSize / 2.0 + 0.1 ) )
                && ( fabs( y - (*v)[1] ) < ( controlPointSize / 2.0 + 0.1 ) ) )
            {
               if( m_pUnderMouse )
               {
                  if( p->selected( ) && !m_pUnderMouse->selected( ) )
                     m_pUnderMouse = p;
               }
               else
                  m_pUnderMouse = p;
            }
         }

         p = m_controlPoints.next( );
         v = m_controlPointsPosition.next( );
      }
   }
   else
      m_pUnderMouse = 0;

   setCursor( m_pUnderMouse ? crossCursor : arrowCursor );
   if( m_pUnderMouse != old )
   {
      if( m_pUnderMouse )
         emit controlPointMessage( m_pUnderMouse->description( ) );
      else
         emit controlPointMessage( "" );
   }
}

void PMGLView::updateControlPoints( )
{
   m_controlPoints.clear( );
   m_controlPoints = m_pPart->activeControlPoints( );

   if( ( m_controlPoints.count( ) > 0 ) && m_pActiveObject )
   {
      m_objectsTransformation = m_pActiveObject->transformedWith( );
      recalculateTransformations( );
   }

   if( m_bGraphicalChangeMode )
      m_bGraphicalChangeMode = false;

   recalculateControlPointPosition( );
}

void PMGLView::recalculateControlPointPosition( )
{
   PMControlPointListIterator it( m_controlPoints );
   m_controlPointsPosition.clear( );
   PMVector* v;

   for( ; it.current( ); ++it )
   {
      v = new PMVector( m_controlPointsTransformation * it.current( )->position( ) );
      m_controlPointsPosition.append( v );
   }
   if( !m_bGraphicalChangeMode )
   {
      m_pUnderMouse = 0;
      emit controlPointMessage( "" );
   }
}

PMVector PMGLView::mousePosition( PMControlPoint* cp, int x, int y )
{
   PMVector result;
   int index;
   PMVector* p;

   result[0] = screenToInternalX( x );
   result[1] = screenToInternalY( y );
   if( cp )
   {
      index = m_controlPoints.findRef( cp );
      if( index >= 0 )
      {
         p = m_controlPointsPosition.at( ( uint ) index );
         if( p )
            result[2] = p->z( );
      }
   }
   return result;
}

void PMGLView::recalculateTransformations( )
{
   int r, c;

   m_controlPointsTransformation = m_viewTransformation * m_objectsTransformation;

   if( m_controlPointsTransformation.canBuildInverse( ) )
   {
      m_inversePointsTransformation = m_controlPointsTransformation.inverse( );

      for( c = 0; c < 4; c++ )
         for( r = 0; r < 4; r++ )
            if( fabs( m_inversePointsTransformation[c][r] ) < 1e-8 )
               m_inversePointsTransformation[c][r] = 0.0;

      m_bInverseValid = true;
   }
   else
      m_bInverseValid = false;
}

void PMGLView::setType( PMViewType t )
{
   if( m_type != t )
      m_viewTransformation = PMMatrix::identity( );
   m_type = t;
   invalidateProjection( );

   emit viewTypeChanged( viewTypeAsString( t ) );
}

void PMGLView::setCamera( PMCamera* c )
{
   m_pCamera = c;
   invalidateProjection( );
}

void PMGLView::slotRefresh( )
{
   if( m_type == PMViewCamera )
      if( !m_pCamera )
         setCamera( m_pPart->firstCamera( ) );

   repaint( );
}

void PMGLView::slotClear( )
{
   m_controlPointsPosition.clear( );
   m_controlPoints.clear( );
   m_pUnderMouse = 0;
   m_pCamera = 0;
   m_pActiveObject = 0;

   slotStopRendering( );
}

void PMGLView::slotActiveRenderModeChanged( )
{
   if( ( m_type == PMViewCamera ) && m_pCamera )
      invalidateProjection( );
}

void PMGLView::slotStopRendering( )
{
   PMRenderManager* rm = PMRenderManager::theManager( );
   rm->removeView( this );
}

void PMGLView::slotObjectChanged( PMObject* obj, const int mode,
                                  QObject* sender )
{
   bool redraw = false;

   if( mode & PMCNewSelection )
   {
      if( obj )
      {
         if( obj != m_pActiveObject )
         {
            m_pActiveObject = obj;
            redraw = true;
         }
      }
      else
      {
         m_pActiveObject = 0;
         redraw = true;
      }
   }
   if( mode & ( PMCSelected | PMCDeselected ) )
   {
      m_pActiveObject = 0;
      redraw = true;
   }
   if( mode & ( PMCViewStructure | PMCGraphicalChange ) )
   {
      if( m_type == PMGLView::PMViewCamera )
      {
         if( obj->type( ) == "Camera" )
            if( m_pCamera == ( PMCamera* ) obj )
               invalidateProjection( );

         if( obj->parent( ) )
            if( obj->parent( )->type( ) == "Camera" )
               if( m_pCamera == ( PMCamera* ) obj->parent( ) )
                  if( obj->hasTransformationMatrix( ) )
                     invalidateProjection( );
      }

      redraw = true;
   }
   if( mode & PMCNewControlPoints )
   {
      updateControlPoints( );
      m_pActiveObject = m_pPart->activeObject( );
      redraw = true;
   }
   if( mode & PMCControlPointSelection )
   {
      redraw = true;
   }
   if( mode & PMCDescription )
   {
      if( m_type == PMGLView::PMViewCamera && obj && obj == m_pCamera )
         redraw = true;
   }
   if( mode & PMCAdd )
   {
      if( m_type == PMGLView::PMViewCamera )
      {
         if( obj->type( ) == "Camera" )
            if( !m_pCamera )
               setCamera( ( PMCamera* ) obj );
         if( obj->parent( ) )
            if( obj->parent( )->type( ) == "Camera" )
               if( m_pCamera == ( PMCamera* ) obj->parent( ) )
                  if( obj->hasTransformationMatrix( ) )
                     invalidateProjection( );
      }
      redraw = true;
   }

   if( mode & PMCRemove )
   {
      if( obj->type( ) == "Camera" )
         if( m_pCamera == ( PMCamera* ) obj )
            setCamera( 0 );

      if( m_type == PMGLView::PMViewCamera )
         if( obj->parent( ) )
            if( obj->parent( )->type( ) == "Camera" )
               if( m_pCamera == ( PMCamera* ) obj->parent( ) )
                  if( obj->hasTransformationMatrix( ) )
                     invalidateProjection( );

      redraw = true;
   }

   if( mode & PMCChildren )
      redraw = true;

   if( redraw )
      repaint( sender == this );
}

void PMGLView::repaint( bool graphicalChange )
{
   if( isValid( ) )
   {
      PMObject* obj = m_pActiveObject;

      if( obj )
         obj = topLevelRenderingObject( obj );
      else
      {
         const PMObjectList& selected = m_pPart->selectedObjects( );
         PMObjectListIterator it( selected );
         if( it.current( ) )
            obj = topLevelRenderingObject( it.current( ) );

         if( obj && ( obj->type( ) != "Scene" ) )
            for( ++it; it.current( ) && obj; ++it )
               if( topLevelRenderingObject( it.current( ) ) != obj )
                  obj = 0;
      }

      if( !obj )
         obj = m_pPart->scene( );

      if( obj )
      {
         double aspectRatio = 1.0;
         PMRenderMode* mode = m_pPart->scene( )->renderModes( )->current( );
         if( mode )
            if( mode->height( ) != 0 )
               aspectRatio = ( double ) mode->width( )
                             / ( double ) mode->height( );

         PMRenderManager* rm = PMRenderManager::theManager( );
         rm->addView( this, m_pActiveObject, obj,
                      &m_controlPoints, aspectRatio,
                      m_pPart->scene( )->visibilityLevel( ), graphicalChange );
      }
   }
}

PMObject* PMGLView::topLevelRenderingObject( PMObject* o ) const
{
   PMObject* obj = o;
   bool stop = false;

   if( obj )
   {
      do
      {
         if( !obj )
            stop = true;
         else if( obj->isA( "Scene" ) || obj->isA( "Declare" ) )
            stop = true;
         else
            obj = obj->parent( );
      }
      while( !stop );
   }
   else
      obj = m_pPart->scene( );

   return obj;
}

void PMGLView::selectControlPoint( PMControlPoint* cp, bool select, bool deselectOthers )
{
   bool selectionChanged = false;

   if( cp )
   {
      if( deselectOthers )
      {
         PMControlPointListIterator it( m_controlPoints );
         for( ; it.current( ); ++it )
         {
            bool s;
            if( it.current( ) == cp )
               s = select;
            else
               s = false;

            if( s != it.current( )->selected( ) )
            {
               selectionChanged = true;
               it.current( )->setSelected( s );
            }
         }
      }
      else
      {
         if( select != cp->selected( ) )
         {
            selectionChanged = true;
            cp->setSelected( select );
         }
      }
   }
   else
   {
      PMControlPointListIterator it( m_controlPoints );
      for( ; it.current( ); ++it )
      {
         if( select != it.current( )->selected( ) )
         {
            selectionChanged = true;
            it.current( )->setSelected( select );
         }
      }
   }

   if( selectionChanged )
      emit objectChanged( m_pActiveObject, PMCControlPointSelection, this );
}

void PMGLView::startSelection( )
{
   if( !m_bSelectionStarted )
   {
      saveSelectionBox( );
      paintSelectionBox( );

      m_bSelectionStarted = true;
   }
}

void PMGLView::restoreSelectionBox( )
{
   if( !m_bAboutToUpdate )
   {
      int sx, sy, ex, ey, w, h;
      calculateSelectionBox( sx, sy, ex, ey, w, h );

      if( !m_selectionPixmap[0].isNull( ) )
         bitBlt( this, sx, sy, &( m_selectionPixmap[0] ), 0, 0, w, 1, CopyROP );
      if( !m_selectionPixmap[1].isNull( ) )
         bitBlt( this, sx, ey, &( m_selectionPixmap[1] ), 0, 0, w, 1, CopyROP );
      if( !m_selectionPixmap[2].isNull( ) )
         bitBlt( this, sx, sy+1, &( m_selectionPixmap[2] ), 0, 0, 1, h-2, CopyROP );
      if( !m_selectionPixmap[3].isNull( ) )
         bitBlt( this, ex, sy+1, &( m_selectionPixmap[3] ), 0, 0, 1, h-2, CopyROP );
   }
}

void PMGLView::saveSelectionBox( )
{
   if( !m_bAboutToUpdate )
   {
      int sx, sy, ex, ey, w, h;
      calculateSelectionBox( sx, sy, ex, ey, w, h );

      m_selectionPixmap[0].resize( w, 1 );
      if( !m_selectionPixmap[0].isNull( ) )
         bitBlt( &( m_selectionPixmap[0] ), 0, 0, this, sx, sy, w, 1, CopyROP );
      m_selectionPixmap[1].resize( w, 1 );
      if( !m_selectionPixmap[1].isNull( ) )
         bitBlt( &( m_selectionPixmap[1] ), 0, 0, this, sx, ey, w, 1, CopyROP );

      m_selectionPixmap[2].resize( 1, h-2 );
      if( !m_selectionPixmap[2].isNull( ) )
         bitBlt( &( m_selectionPixmap[2] ), 0, 0, this, sx, sy+1, 1, h-2, CopyROP );
      m_selectionPixmap[3].resize( 1, h-2 );
      if( !m_selectionPixmap[3].isNull( ) )
         bitBlt( &( m_selectionPixmap[3] ), 0, 0, this, ex, sy+1, 1, h-2, CopyROP );
   }
}

void PMGLView::paintSelectionBox( )
{
   if( !m_bAboutToUpdate )
   {
      int sx, sy, ex, ey, w, h;
      calculateSelectionBox( sx, sy, ex, ey, w, h );
      QPainter p;
      p.begin( this );
      p.setPen( PMRenderManager::theManager( )->controlPointColor( 1 ) );
      p.drawRect( sx, sy, w, h );
      p.end( );
   }
}

void PMGLView::calculateSelectionBox( int& sx, int& sy, int& ex, int& ey,
                                      int& w, int& h )
{
   if( m_selectionStart.x( ) < m_selectionEnd.x( ) )
   {
      sx = m_selectionStart.x( );
      ex = m_selectionEnd.x( );
   }
   else
   {
      ex = m_selectionStart.x( );
      sx = m_selectionEnd.x( );
   }

   if( m_selectionStart.y( ) < m_selectionEnd.y( ) )
   {
      sy = m_selectionStart.y( );
      ey = m_selectionEnd.y( );
   }
   else
   {
      ey = m_selectionStart.y( );
      sy = m_selectionEnd.y( );
   }

   w = ex - sx + 1;
   h = ey - sy + 1;
}

double PMGLView::screenToInternalX( int x ) const
{
   return rint( x - width( ) / 2.0 + 0.1 );
}

double PMGLView::screenToInternalY( int y ) const
{
   return rint( height( ) / 2.0 - y - 0.1 );
}

void PMGLView::slotRenderingStarted( PMGLView* )
{
}

void PMGLView::slotAboutToUpdate( PMGLView* view )
{
   if( view == this )
      m_bAboutToUpdate = true;
}

void PMGLView::slotRenderingFinished( PMGLView* view )
{
   if( view == this )
   {
      m_bAboutToUpdate = false;
      if( m_bMultipleSelectionMode )
      {
         saveSelectionBox( );
         paintSelectionBox( );
      }

      if( m_bAutoScroll )
      {
         QTime now = QTime::currentTime( );
         int msecs = m_lastAutoScrollUpdate.msecsTo( now );

         if( msecs < c_minAutoScrollUpdateTime )
            m_autoScrollTimer.start( c_minAutoScrollUpdateTime - msecs, true );
         else
            m_autoScrollTimer.start( 0, true );
      }
   }
}

QString PMGLView::viewTypeAsString( PMViewType t )
{
   QString str;

   switch( t )
   {
      case PMViewPosX:
         str = i18n( "Left" );
         break;
      case PMViewNegX:
         str = i18n( "Right" );
         break;
      case PMViewPosY:
         str = i18n( "Bottom" );
         break;
      case PMViewNegY:
         str = i18n( "Top" );
         break;
      case PMViewPosZ:
         str = i18n( "Front" );
         break;
      case PMViewNegZ:
         str = i18n( "Back" );
         break;
      case PMViewCamera:
         str = i18n( "Camera" );
         break;
   }
   return str;
}

void PMGLView::saveConfig( KConfig* /*cfg*/ )
{
}

void PMGLView::restoreConfig( KConfig* /*cfg*/ )
{
}

void PMGLView::contextMenu( )
{
   QPopupMenu* m = new QPopupMenu( );
   m->insertItem( i18n( "Left View" ), this, SLOT( slotSetTypePosX( ) ) );
   m->insertItem( i18n( "Right View" ), this, SLOT( slotSetTypeNegX( ) ) );
   m->insertItem( i18n( "Top View" ), this, SLOT( slotSetTypeNegY( ) ) );
   m->insertItem( i18n( "Bottom View" ), this, SLOT( slotSetTypePosY( ) ) );
   m->insertItem( i18n( "Front View" ), this, SLOT( slotSetTypePosZ( ) ) );
   m->insertItem( i18n( "Back View" ), this, SLOT( slotSetTypeNegZ( ) ) );

   QPopupMenu* cm = new QPopupMenu( m );
   QPtrListIterator<PMCamera> it = m_pPart->cameras( );
   QString name;
   if( !it.current( ) )
      cm->insertItem( i18n( "No Cameras" ) );
   else
   {
      int cnr = 0;
      for( ; it.current( ); ++it, ++cnr )
      {
         name = it.current( )->name( );
         if( name.isEmpty( ) )
            name = i18n( "(unnamed)" );
         cm->insertItem( name, cnr );
      }
   }
   connect( cm, SIGNAL( activated( int ) ), SLOT( slotCameraView( int ) ) );

   m->insertItem( SmallIconSet( "pmcamera" ), i18n( "Camera" ), cm );

   m->insertSeparator( );

   m->insertItem( i18n( "Snap to Grid" ), this, SLOT( slotSnapToGrid( ) ) );
   m_objectActions.clear( );
   if( m_pActiveObject )
   {
      m_pActiveObject->addObjectActions( m_controlPoints, m_objectActions );
      if( !m_objectActions.isEmpty( ) )
      {
         PMObjectAction* oa = 0;
         QPtrListIterator<PMObjectAction> ait( m_objectActions );

         for( ; ait.current( ); ++ait )
         {
            oa = ait.current( );
            oa->setMenuID( m->insertItem( oa->description( ) ) );
         }
      }
   }
   connect( m, SIGNAL( activated( int ) ), SLOT( slotObjectAction( int ) ) );

   m->insertSeparator( );

   QPopupMenu* menu = new QPopupMenu( m );
   PMControlPointListIterator pit( m_controlPoints );

   if( !pit.current( ) )
      menu->insertItem( i18n( "No Control Points" ) );
   else
   {
      int cnr = 0;
      for( ; pit.current( ); ++pit, ++cnr )
         menu->insertItem( pit.current( )->description( ), cnr );
   }
   connect( menu, SIGNAL( activated( int ) ), SLOT( slotControlPoint( int ) ) );

   m->insertItem( i18n( "Control Points" ), menu );

   m->exec( QCursor::pos( ) );
   delete m;
}

void PMGLView::slotCameraView( int id )
{
   int i;
   QPtrListIterator<PMCamera> it = m_pPart->cameras( );

   for( i = 0; i < id; i++ )
      ++it;
   if( it.current( ) )
   {
      setCamera( it.current( ) );
      setType( PMGLView::PMViewCamera );
   }
}

void PMGLView::slotObjectAction( int id )
{
   QPtrListIterator<PMObjectAction> it( m_objectActions );
   PMObjectAction* oa = 0;

   for( ; it.current( ) && !oa; ++it )
      if( it.current( )->menuID( ) == id )
         oa = it.current( );

   if( oa && m_pActiveObject )
   {
      // otherwise no object action was selected in the context menu

      m_pActiveObject->createMemento( );
      m_pActiveObject->objectActionCalled( oa, m_controlPoints,
                                           m_controlPointsPosition,
                                           m_contextClickPosition );
      PMDataChangeCommand* cmd;
      cmd = new PMDataChangeCommand( m_pActiveObject->takeMemento( ) );
      cmd->setText( oa->description( ) );
      m_pPart->executeCommand( cmd );
   }
}

void PMGLView::slotControlPoint( int id )
{
   PMControlPoint* p = m_controlPoints.at( id );
   if( p )
   {
      PMControlPointListIterator cit( m_controlPoints );
      for( ; cit.current( ); ++cit )
         cit.current( )->setSelected( p == cit.current( ) );
      emit objectChanged( m_pActiveObject, PMCControlPointSelection, this );
   }
}

void PMGLView::slotSnapToGrid( )
{
   if( m_pActiveObject )
   {
      if( !m_pActiveObject->mementoCreated( ) )
         m_pActiveObject->createMemento( );

      PMControlPointListIterator it( m_controlPoints );
      for( ; it.current( ); ++it )
         if( it.current( )->selected( ) )
            it.current( )->snapToGrid( );

      m_pActiveObject->controlPointsChanged( m_controlPoints );

      PMDataChangeCommand* cmd;
      cmd = new PMDataChangeCommand( m_pActiveObject->takeMemento( ) );
      cmd->setText( i18n( "Snap to Grid" ) );
      m_pPart->executeCommand( cmd );
   }
}

QString PMGLView::description( ) const
{
   return viewTypeAsString( m_type );
}

void PMGLView::restoreViewConfig( PMViewOptions* vo )
{
   if( vo && vo->viewType( ) == "glview" )
   {
      PMGLViewOptions* o = ( PMGLViewOptions* ) vo;
      m_type = o->glViewType( );
   }
}

void PMGLView::saveViewConfig( PMViewOptions* vo ) const
{
   if( vo && vo->viewType( ) == "glview" )
   {
      PMGLViewOptions* o = ( PMGLViewOptions* ) vo;
      o->setGLViewType( m_type );
   }
}

void PMGLViewOptions::loadData( QDomElement& e )
{
   QString s = e.attribute( "type", "Camera" );
   if( s == "Camera" ) m_glViewType = PMGLView::PMViewCamera;
   else if( s == "X" ) m_glViewType = PMGLView::PMViewPosX;
   else if( s == "Y" ) m_glViewType = PMGLView::PMViewPosY;
   else if( s == "Z" ) m_glViewType = PMGLView::PMViewPosZ;
   else if( s == "NegX" ) m_glViewType = PMGLView::PMViewNegX;
   else if( s == "NegY" ) m_glViewType = PMGLView::PMViewNegY;
   else if( s == "NegZ" ) m_glViewType = PMGLView::PMViewNegZ;
}

void PMGLViewOptions::saveData( QDomElement& e )
{
   switch( m_glViewType )
   {
      case PMGLView::PMViewCamera:
         e.setAttribute( "type", "Camera" );
         break;
      case PMGLView::PMViewPosX:
         e.setAttribute( "type", "X" );
         break;
      case PMGLView::PMViewPosY:
         e.setAttribute( "type", "Y" );
         break;
      case PMGLView::PMViewPosZ:
         e.setAttribute( "type", "Z" );
         break;
      case PMGLView::PMViewNegX:
         e.setAttribute( "type", "NegX" );
         break;
      case PMGLView::PMViewNegY:
         e.setAttribute( "type", "NegY" );
         break;
      case PMGLView::PMViewNegZ:
         e.setAttribute( "type", "NegZ" );
         break;
      default:
         kdError( PMArea ) << i18n( "Unknown GL view type." )
                           << endl;
         break;
   }
}

QString PMGLViewFactory::description( ) const
{
   return i18n( "3D View" );
}

QString PMGLViewFactory::description( PMViewOptions* vo ) const
{
   if( vo && vo->viewType( ) == "glview" )
   {
      PMGLViewOptions* o = ( PMGLViewOptions* ) vo;
      return i18n( "3D View (%1)" ).arg(
         PMGLView::viewTypeAsString( o->glViewType( ) ) );
   }
   return description( );
}

PMViewOptionsWidget* PMGLViewFactory::newOptionsWidget( QWidget* parent,
                                                        PMViewOptions* o )
{
   return new PMGLViewOptionsWidget( parent, o );
}

PMViewOptions* PMGLViewFactory::newOptionsInstance( ) const
{
   PMGLViewOptions* o = new PMGLViewOptions( );
   return o;
}

PMGLViewOptionsWidget::PMGLViewOptionsWidget( QWidget* parent,
                                              PMViewOptions* o )
      : PMViewOptionsWidget( parent )
{
   m_pOptions = ( PMGLViewOptions* ) o;

   QHBoxLayout* hl = new QHBoxLayout( this, 0, KDialog::spacingHint( ) );
   QLabel* l = new QLabel( i18n( "3D view type:" ), this );
   hl->addWidget( l );

   m_pGLViewType = new QComboBox( false, this );
   m_pGLViewType->insertItem( i18n( "Top" ) );
   m_pGLViewType->insertItem( i18n( "Bottom" ) );
   m_pGLViewType->insertItem( i18n( "Left" ) );
   m_pGLViewType->insertItem( i18n( "Right" ) );
   m_pGLViewType->insertItem( i18n( "Front" ) );
   m_pGLViewType->insertItem( i18n( "Back" ) );
   m_pGLViewType->insertItem( i18n( "Camera" ) );

   switch( m_pOptions->glViewType( ) )
   {
      case PMGLView::PMViewNegY:
         m_pGLViewType->setCurrentItem( 0 );
         break;
      case PMGLView::PMViewPosY:
         m_pGLViewType->setCurrentItem( 1 );
         break;
      case PMGLView::PMViewPosX:
         m_pGLViewType->setCurrentItem( 2 );
         break;
      case PMGLView::PMViewNegX:
         m_pGLViewType->setCurrentItem( 3 );
         break;
      case PMGLView::PMViewPosZ:
         m_pGLViewType->setCurrentItem( 4 );
         break;
      case PMGLView::PMViewNegZ:
         m_pGLViewType->setCurrentItem( 5 );
         break;
      case PMGLView::PMViewCamera:
         m_pGLViewType->setCurrentItem( 6 );
         break;
   }

   connect( m_pGLViewType, SIGNAL( activated( int ) ),
            SLOT( slotGLViewTypeChanged( int ) ) );
   hl->addWidget( m_pGLViewType );
}

void PMGLViewOptionsWidget::slotGLViewTypeChanged( int index )
{
   switch( index )
   {
      case 0:
         m_pOptions->setGLViewType( PMGLView::PMViewNegY );
         break;
      case 1:
         m_pOptions->setGLViewType( PMGLView::PMViewPosY );
         break;
      case 2:
         m_pOptions->setGLViewType( PMGLView::PMViewPosX );
         break;
      case 3:
         m_pOptions->setGLViewType( PMGLView::PMViewNegX );
         break;
      case 4:
         m_pOptions->setGLViewType( PMGLView::PMViewPosZ );
         break;
      case 5:
         m_pOptions->setGLViewType( PMGLView::PMViewNegZ );
         break;
      case 6:
         m_pOptions->setGLViewType( PMGLView::PMViewCamera );
         break;
   }
   emit viewTypeDescriptionChanged( );
}

#include "pmglview.moc"
