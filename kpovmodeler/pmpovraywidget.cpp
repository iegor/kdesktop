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

#include "pmpovraywidget.h"
#include "pmpovrayrenderwidget.h"
#include "pmpovrayoutputwidget.h"
#include "pmshell.h"

#include <klocale.h>
#include <kmessagebox.h>
#include <kimageio.h>
#include <kfiledialog.h>
#include <ktempfile.h>
#include <kio/netaccess.h>
#include <kglobalsettings.h>
#include <kpushbutton.h>
#include <kstdguiitem.h>

#include <qlayout.h>
#include <qprogressbar.h>
#include <qlabel.h>
#include <qscrollview.h>
#include <qtimer.h>
#include <qapplication.h>

const int timerIntervall = 1000;
bool PMPovrayWidget::s_imageFormatsRegistered = false;

PMPovrayWidget::PMPovrayWidget( QWidget* parent, const char* name )
      : KDialog( parent, name )
{
   QVBoxLayout* topLayout = new QVBoxLayout( this, KDialog::marginHint( ), KDialog::spacingHint( ) );
   topLayout->addStretch( );

   QHBoxLayout* renderLayout = new QHBoxLayout( );
   topLayout->addLayout( renderLayout, 2 );
   m_pScrollView = new QScrollView( this );
   m_pScrollView->setBackgroundMode( PaletteBase );
   renderLayout->addWidget( m_pScrollView, 2 );
   m_pRenderWidget = new PMPovrayRenderWidget( m_pScrollView->viewport( ) );
   m_pRenderWidget->setFixedSize( 200, 200 );
   m_pScrollView->addChild( m_pRenderWidget );
   topLayout->addStretch( );

   QHBoxLayout* progressLayout = new QHBoxLayout( topLayout );
   m_pProgressBar = new QProgressBar( this );
   m_pProgressBar->hide( );
   progressLayout->addWidget( m_pProgressBar, 1 );
   m_pProgressLabel = new QLabel( this );
   progressLayout->addWidget( m_pProgressLabel, 2 );

   QHBoxLayout* buttonLayout = new QHBoxLayout( topLayout );
   m_pStopButton = new QPushButton( i18n( "Stop" ), this );
   m_pStopButton->setEnabled( false );
   buttonLayout->addWidget( m_pStopButton );
   m_pSuspendButton = new QPushButton( i18n( "Suspend" ), this );
   m_pSuspendButton->setEnabled( false );
   buttonLayout->addWidget( m_pSuspendButton );
   m_pResumeButton = new QPushButton( i18n( "Resume" ), this );
   m_pResumeButton->setEnabled( false );
   buttonLayout->addWidget( m_pResumeButton );
   buttonLayout->addStretch( 1 );
   m_pPovrayOutputButton = new QPushButton( i18n( "Povray Output" ), this );
   buttonLayout->addWidget( m_pPovrayOutputButton );

   buttonLayout = new QHBoxLayout( topLayout );
   m_pSaveButton = new KPushButton( KStdGuiItem::saveAs(), this );
   m_pSaveButton->setEnabled( false );
   buttonLayout->addWidget( m_pSaveButton );
   buttonLayout->addStretch( 1 );
   QPushButton* closeButton = new KPushButton( KStdGuiItem::close(), this );
   buttonLayout->addWidget( closeButton );

   connect( m_pRenderWidget, SIGNAL( finished( int ) ),
            SLOT( slotRenderingFinished( int ) ) );
   connect( m_pRenderWidget, SIGNAL( progress( int ) ),
            SLOT( slotProgress( int ) ) );
   connect( m_pRenderWidget, SIGNAL( lineFinished( int ) ),
            SLOT( slotLineFinished( int ) ) );

   connect( m_pStopButton, SIGNAL( clicked( ) ), SLOT( slotStop( ) ) );
   connect( m_pSuspendButton, SIGNAL( clicked( ) ), SLOT( slotSuspend( ) ) );
   connect( m_pResumeButton, SIGNAL( clicked( ) ), SLOT( slotResume( ) ) );
   connect( m_pSaveButton, SIGNAL( clicked( ) ), SLOT( slotSave( ) ) );
   connect( closeButton, SIGNAL( clicked( ) ), SLOT( slotClose( ) ) );
   connect( m_pPovrayOutputButton, SIGNAL( clicked( ) ),
            SLOT( slotPovrayOutput( ) ) );

   m_bRunning = false;
   m_pProgressTimer = new QTimer( this );
   connect( m_pProgressTimer, SIGNAL( timeout( ) ),
            SLOT( slotUpdateSpeed( ) ) );

   setCaption( i18n( "Render Window" ) );

   m_height = m_width = 0;
   m_stopped = false;

   m_pPovrayOutputWidget = new PMPovrayOutputWidget( );
   connect( m_pRenderWidget, SIGNAL( povrayMessage( const QString& ) ),
            m_pPovrayOutputWidget, SLOT( slotText( const QString& ) ) );
}

PMPovrayWidget::~PMPovrayWidget( )
{
   delete m_pPovrayOutputWidget;
}

bool PMPovrayWidget::render( const QByteArray& scene, const PMRenderMode& m,
                             const KURL& documentURL )
{
   bool updateSize = ( m_height != m.height( ) ) || ( m_width != m.width( ) );
   m_height = m.height( );
   m_width = m.width( );
   m_bRunning = false;
   m_pPovrayOutputWidget->slotClear( );
   m_stopped = false;

   m_pRenderWidget->setFixedSize( m_width, m_height );
   QSize maxSize( m_width + m_pScrollView->frameWidth( ) * 2,
                  m_height + m_pScrollView->frameWidth( ) * 2 );
   m_pScrollView->setMaximumSize( maxSize );

   if( updateSize )
   {
      int w, h;

      w = maxSize.width( ) + KDialog::spacingHint( ) * 2;
      h = maxSize.height( ) + m_pSaveButton->sizeHint( ).height( ) * 2
         + KDialog::spacingHint( ) * 6;
      if( m_pProgressLabel->sizeHint( ).height( )
          > m_pProgressBar->sizeHint( ).height( ) )
         h += m_pProgressLabel->sizeHint( ).height( );
      else
         h += m_pProgressBar->sizeHint( ).height( );

      w += 16;
      h += 16;

#if ( ( KDE_VERSION_MAJOR == 3 ) && ( KDE_VERSION_MINOR <= 1 ) )
      QWidget* dw = QApplication::desktop( );
      if( w > dw->width( ) )
         w = dw->width( );
      if( h > dw->height( ) )
         h = dw->height( );
#else
      QRect dw = KGlobalSettings::desktopGeometry(this);
      if( w > dw.width() )
         w = dw.width();
      if( h > dw.height() )
         h = dw.height();
#endif
      resize( w, h );
   }

   if( m_pRenderWidget->render( scene, m, documentURL ) )
   {
      m_bRunning = true;
      m_pProgressBar->setProgress( 0 );
      m_pProgressBar->show( );
      m_pProgressLabel->setText( i18n( "running" ) );
      m_pStopButton->setEnabled( true );
      m_pSuspendButton->setEnabled( true );
      m_pResumeButton->setEnabled( false );
      m_pSaveButton->setEnabled( false );

      m_lastSpeedTime = QTime( );
      m_pProgressTimer->start( timerIntervall, true );
      m_speedInfo = false;
      m_speed = 0;
      m_line = 0;
      m_immediateUpdate = false;
      showSpeed( 0 );
   }

   return m_bRunning;
}

void PMPovrayWidget::slotStop( )
{
   m_stopped = true;
   m_pRenderWidget->killRendering( );
   m_pSaveButton->setEnabled( true );
}

void PMPovrayWidget::slotSuspend( )
{
   m_pRenderWidget->suspendRendering( );
   m_pSuspendButton->setEnabled( false );
   m_pResumeButton->setEnabled( true );
   m_pSaveButton->setEnabled( true );

   m_pProgressTimer->stop( );
   m_lastSpeedTime = QTime( );
   m_speedInfo = false;
   m_immediateUpdate = false;
   m_pProgressLabel->setText( i18n( "suspended" ) );
}

void PMPovrayWidget::slotResume( )
{
   m_pRenderWidget->resumeRendering( );
   m_pSuspendButton->setEnabled( true );
   m_pResumeButton->setEnabled( false );
   m_pSaveButton->setEnabled( false );

   m_pProgressTimer->start( timerIntervall, true );
   showSpeed( m_speed );
}

void PMPovrayWidget::slotClose( )
{
   hide( );
}

void PMPovrayWidget::slotSave( )
{
   KTempFile* tempFile = 0;
   QFile* file = 0;
   bool ok = true;

   if( !s_imageFormatsRegistered )
   {
      KImageIO::registerFormats( );
      s_imageFormatsRegistered = true;
   }

   KURL url = KFileDialog::getSaveURL( QString::null, KImageIO::pattern( KImageIO::Writing ) );
   if( url.isEmpty( ) )
      return;
   if( !PMShell::overwriteURL( url ) )
      return;

   if( !url.isValid( ) )
   {
      KMessageBox::error( this, i18n( "Malformed URL" ) );
      return;
   }

   QString format = KImageIO::type( url.fileName( ) );
   if( format.isEmpty( ) )
   {
      KMessageBox::error( this, i18n( "Unknown image format.\n"
                                      "Please enter a valid suffix." ) );
      return;
   }

   if( !KImageIO::canWrite( format ) )
   {
      KMessageBox::error( this, i18n( "Format is not supported for writing." ) );
      return;
   }

   if( url.isLocalFile( ) )
   {
      // Local file
      file = new QFile( url.path( ) );
      if( !file->open( IO_WriteOnly ) )
         ok = false;
   }
   else
   {
      // Remote file
      // provide a temp file
      tempFile = new KTempFile( );
      if( tempFile->status( ) != 0 )
         ok = false;
      else
         file = tempFile->file( );
   }

   if( ok )
   {
      QImageIO iio( file, format.latin1( ) );
      iio.setImage( m_pRenderWidget->image( ) );
      ok = iio.write( );

      if( ok )
      {
         if( tempFile )
         {
            tempFile->close( );
            ok = KIO::NetAccess::upload( tempFile->name( ), url );
            tempFile->unlink( );
            file = 0;
         }
         else
            file->close( );
      }
      else
         KMessageBox::error( this, i18n( "Couldn't correctly write the image.\n"
                                         "Wrong image format?" ) );
   }
   else
      KMessageBox::error( this, i18n( "Couldn't write the image.\n"
                                      "Permission denied." ) );


   delete file;
   delete tempFile;
}

void PMPovrayWidget::slotPovrayOutput( )
{
   m_pPovrayOutputWidget->show( );
}

void PMPovrayWidget::slotRenderingFinished( int returnStatus )
{
   kdDebug( PMArea ) << "Povray exited with status " << returnStatus << endl;
   m_bRunning = false;
   if( returnStatus == 0 )
      m_pSaveButton->setEnabled( true );
   m_pStopButton->setEnabled( false );
   m_pSuspendButton->setEnabled( false );
   m_pResumeButton->setEnabled( false );
   m_pProgressLabel->setText( i18n( "finished" ) );
   m_pProgressTimer->stop( );

   if( ( returnStatus != 0 ) && !m_stopped )
   {
      KMessageBox::error( this, i18n( "Povray exited abnormally.\n"
                                      "See the povray output for details." )
                          .arg( returnStatus ) );
   }
   else if( m_pRenderWidget->povrayOutput( ).contains( "ERROR" ) )
   {
      KMessageBox::error( this, i18n( "There were errors while rendering.\n"
                                      "See the povray output for details." ) );
   }
}

void PMPovrayWidget::slotProgress( int i )
{
   m_pProgressBar->setProgress( i );
}

void PMPovrayWidget::slotLineFinished( int line )
{
   m_speedInfo = true;
   QTime ct = QTime::currentTime( );

   if( !m_lastSpeedTime.isNull( ) )
   {
      int msecs = m_lastSpeedTime.msecsTo( ct );
      if( msecs < 1 )
         msecs = 1;

      double g = 1.0 / ( ( double ) msecs / 500.0 + 1.0 );
      m_speed = g * m_speed +
                1000 * ( 1.0 - g ) * m_width * ( line - m_line ) / msecs;
   }

   if( m_immediateUpdate )
   {
      m_immediateUpdate = false;
      showSpeed( m_speed );
      m_pProgressTimer->start( timerIntervall, true );
      m_speedInfo = false;
   }

   m_lastSpeedTime = ct;
   m_line = line;
}

void PMPovrayWidget::slotUpdateSpeed( )
{
   if( m_speedInfo )
   {
      showSpeed( m_speed );
      m_pProgressTimer->start( timerIntervall, true );
      m_speedInfo = false;
   }
   else
      m_immediateUpdate = true;
}

void PMPovrayWidget::showSpeed( double pps )
{
   QString num;
   if( pps >= 1000000 )
   {
      num.setNum( pps / 100000, 'g', 3 );
      num += 'M';
   }
   else if( pps >= 1000 )
   {
      num.setNum( pps / 1000, 'g', 3 );
      num += 'K';
   }
   else
      num.setNum( pps, 'g', 3 );

   m_pProgressLabel->setText( i18n( "running, %1 pixels/second" ).arg( num ) );
}

#include "pmpovraywidget.moc"
