/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2001-2002 by Andreas Zehender
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

#ifndef PMPOVRAYRENDERWIDGET_H
#define PMPOVRAYRENDERWIDGET_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qwidget.h>
#include <qcstring.h>
#include <qimage.h>
#include <qbuffer.h>
#include <qstringlist.h>

#include "pmrendermode.h"
#include "pmdragwidget.h"

class KProcess;
class KConfig;
class KURL;
class KTempFile;

/**
 * Widget that calls povray to render a scene and
 * displays the output.
 */
class PMPovrayRenderWidget : public PMDragWidget
{
   Q_OBJECT
public:
   /**
    * Standard constructor
    */
   PMPovrayRenderWidget( QWidget* parent = 0, const char* name = 0 );
   /**
    * destructor
    */
   virtual ~PMPovrayRenderWidget( );

   /**
    * Starts rendering for the povray code in the byte array with
    * render mode m.
    * @see PMRenderMode
    */
   bool render( const QByteArray& scene, const PMRenderMode& m,
                const KURL& documentURL );

   /**
    * Returns the povray text output
    */
   QString povrayOutput( ) const { return m_povrayOutput; }
   /**
    * Returns the rendered image
    */
   QImage image( ) const { return m_image; }
   
   static void saveConfig( KConfig* cfg );
   static void restoreConfig( KConfig* cfg );

   /**
    * Returns the povray command
    */
   static QString povrayCommand( ) { return s_povrayCommand; }
   /**
    * Sets the povray command
    */
   static void setPovrayCommand( const QString& c ) { s_povrayCommand = c; }
   /**
    * Returns the library paths
    */
   static QStringList povrayLibraryPaths( ) { return s_libraryPaths; }
   /**
    * Sets the library paths
    */
   static void setPovrayLibraryPaths( const QStringList& slist )
   { s_libraryPaths = slist; }
   virtual QSize sizeHint( ) const;

   virtual void startDrag( );
signals:
   /**
    * Emitted when rendering has finished
    */
   void finished( int exitStatus );
   /**
    * Provides progress information
    */
   void progress( int percent );
   /**
    * Provides progress imformation
    */
   void lineFinished( int line );
   /**
    * The povray output text
    */
   void povrayMessage( const QString& msg );
   
public slots:
   /**
    * Kills rendering
    */
   void killRendering( );
   /**
    * Suspends rendering
    */
   void suspendRendering( );
   /**
    * Resumes rendering
    */
   void resumeRendering( );

protected slots:
   /**
    * Receive povray messages
    */
   void slotPovrayMessage( KProcess* proc, char* buffer, int buflen );
   /**
    * Receive rendered image
    */
   void slotPovrayImage( KProcess* proc, char* buffer, int buflen );
   /**
    * Called when output has been written to the povray process
    */
   //void slotWroteStdin( KProcess* proc );
   /**
    * Called when the process has finished
    */
   void slotRenderingFinished( KProcess* proc );
   
protected:
   virtual void paintEvent( QPaintEvent* );
      
private:
   void setPixel( int x, int y, uint c );
   void cleanup( );
   
   KProcess* m_pProcess;
   bool m_bSuspended;
   PMRenderMode m_renderMode;
   QImage m_image;
   bool m_rcvHeader;
   unsigned char m_header[18];
   int m_rcvHeaderBytes;
   int m_skipBytes;
   int m_bytespp;
   int m_rcvPixels;
   int m_progress;
   unsigned char m_restBytes[4];
   int m_numRestBytes;
   int m_line;
   int m_column;
   QPixmap m_pixmap;
   bool m_bPixmapUpToDate;
   QString m_povrayOutput;
   KTempFile* m_pTempFile;

   static QString s_povrayCommand;
   static QStringList s_libraryPaths;
};

#endif
