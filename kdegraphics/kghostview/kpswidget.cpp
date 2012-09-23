/** 
 * Copyright (C) 2000-2003 the KGhostView authors. See file AUTHORS.
 * 	
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "kpswidget.h"

#include <stdlib.h>
#include <math.h>

#include <qstringlist.h>

#include <kapplication.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprocess.h>

#include "configuration.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>

int handler( Display* d, XErrorEvent* e )
{
    char msg[80], req[80], number[80];

    XGetErrorText( d, e->error_code, msg, sizeof( msg ) );
    sprintf( number, "%d", e->request_code );
    XGetErrorDatabaseText( d, "XRequest", number, "<unknown>", 
                           req, sizeof( req ) );
    return 0;
}

int orientation2angle( CDSC_ORIENTATION_ENUM orientation )
{
    Q_ASSERT( orientation != CDSC_ORIENT_UNKNOWN );

    int angle = 0;

    switch( orientation ) 
    {
    case CDSC_ORIENT_UNKNOWN:		    break; // Catched by Q_ASSERT
    case CDSC_PORTRAIT:	    angle = 0;	    break;
    case CDSC_LANDSCAPE:    angle = 90;	    break;
    case CDSC_UPSIDEDOWN:   angle = 180;    break;
    case CDSC_SEASCAPE:	    angle = 270;    break;
    }

    return angle;
}

QCString palette2String( Configuration::EnumPalette::type palette )
{
    QCString str;

    switch( palette )
    {
	case Configuration::EnumPalette::Color:      str = "Color";      break;
	case Configuration::EnumPalette::Grayscale:  str = "Grayscale";  break;
	case Configuration::EnumPalette::Monochrome: str = "Monochrome"; break;
	default: kdWarning( 4500 ) << "palette2String(): unkown palette" << endl;
	     str = "Color";
    }

    return str;
}


KPSWidget::KPSWidget( QWidget* parent, const char* name ) : 
    QWidget           ( parent, name ),
    _gsWindow         ( None ),
    _usePipe          ( false ),
    _doubleBuffer     ( false ),
    _ghostscriptDirty ( false ),
    _orientation      ( CDSC_PORTRAIT ),
    _magnification    ( 1 ),
    _palette          ( Configuration::EnumPalette::Color ),
    _widgetDirty      ( true ),
    _process          ( 0 ),
    _buffer           ( 0 ),
    _stdinReady       ( false ),
    _interpreterBusy  ( false ),
    _interpreterReady ( false )
{
    XSetErrorHandler( handler );

    // Create the Atoms used to communicate with Ghostscript.
    const char* const atomNames[] = { "GHOSTVIEW", "GHOSTVIEW_COLORS", 
                                      "NEXT", "PAGE", "DONE" };
    XInternAtoms( x11Display(), const_cast<char**>( atomNames ), 
                  5, false, _atoms );

    // readSettings() TODO
}

KPSWidget::~KPSWidget()
{
    if ( _buffer ) operator delete( _buffer );
    stopInterpreter();
}

bool KPSWidget::isInterpreterReady() const
{
    return isInterpreterRunning() && _interpreterReady;
}

bool KPSWidget::isInterpreterBusy() const
{
    return _interpreterBusy;
}

bool KPSWidget::isInterpreterRunning() const
{
    return ( _process && _process->isRunning() );
}

bool KPSWidget::nextPage()
{
    if( !isInterpreterReady() )
	return false;

    if( _gsWindow == None ) {
	kdDebug(4500) << "communication window unknown!" << endl;
	return false;
    }

    _interpreterReady = false;
    _interpreterBusy = true;
    setCursor( waitCursor );

    XEvent e;
    e.xclient.type = ClientMessage;
    e.xclient.display = x11Display();
    e.xclient.window = _gsWindow;
    e.xclient.message_type = _atoms[NEXT];
    e.xclient.format = 32;

    XSendEvent( x11Display(), _gsWindow, false, 0, &e );
    XFlush( x11Display() );
    return true;
}


void KPSWidget::clear()
{
    //_backgroundPixmap.fill();
}


bool KPSWidget::sendPS( FILE* fp, unsigned int begin, unsigned int end )
{
    kdDebug(4500) << "KPSWidget::sendPS" << endl;

    if( !isInterpreterRunning() )
	return false;

    // Create a new record to add to the queue.
    _inputQueue.push( Record( fp, begin, end - begin ) );

    // Start processing the queue.
    if( _stdinReady )
	gs_input(_process);

    return true;
}

void KPSWidget::setGhostscriptPath( const QString& path )
{
    kdDebug() << "KPSWidget::setGhostscriptPath( " << path << " )" << endl;
    if( _ghostscriptPath != path )
    {
	_ghostscriptPath = path;
	stopInterpreter();
	_ghostscriptDirty = true;
    }
}

void KPSWidget::setGhostscriptArguments( const QStringList& arguments )
{
    if( _ghostscriptArguments != arguments )
    {
	_ghostscriptArguments = arguments;
	stopInterpreter();
	_ghostscriptDirty = true;
    }
}

void KPSWidget::setFileName( const QString& fileName, bool usePipe )
{
    if(( _fileName != fileName ) || (_usePipe != usePipe))
    {
        _usePipe = usePipe;
	_fileName = fileName;
	stopInterpreter();
	_ghostscriptDirty = true;
    }
}

void KPSWidget::setOrientation( CDSC_ORIENTATION_ENUM orientation )
{
    if( _orientation != orientation ) 
    {
	_orientation = orientation;
	stopInterpreter();
	_widgetDirty = true;
    }
}

void KPSWidget::setBoundingBox( const KDSCBBOX& boundingBox )
{
    if( _boundingBox != boundingBox ) 
    {
	_boundingBox = boundingBox;
	stopInterpreter();
	_widgetDirty = true;
    }
}

void KPSWidget::setMagnification( double magnification )
{
    if( kAbs( magnification - _magnification ) > 0.0001 )
    {
	_magnification = magnification;
	stopInterpreter();
	_widgetDirty = true;
    }
}

void KPSWidget::setPalette( Configuration::EnumPalette::type palette )
{
    if( _palette != palette )
    {
	_palette = palette;
	stopInterpreter();
	_widgetDirty = true;
    }
}

void KPSWidget::setDoubleBuffering( bool db )
{
    if( _doubleBuffer != db )
    {
	_doubleBuffer = db;
	stopInterpreter();
	_widgetDirty = true;
    }
}

namespace {
    /* Rounding up is better than normal rounding because it is better to have a pixel too many than one too little.
     * If we have one too many, no one will notice. If we have one too little, gs complains.
     *
     * I have a file which isn't displayed (gs error) without this fix.
     */
    inline int round_up( double x )
    {
	return static_cast<int>( ceil( x ) );
    }
}

void KPSWidget::setupWidget()
{
    if( !_widgetDirty )
	return;

    Q_ASSERT( orientation() != CDSC_ORIENT_UNKNOWN );

    const float dpiX = _magnification * x11AppDpiX();
    const float dpiY = _magnification * x11AppDpiY();

    int newWidth = 0, newHeight = 0;
    if( orientation() == CDSC_PORTRAIT || orientation() == CDSC_UPSIDEDOWN )
    {
	newWidth  = round_up( boundingBox().width()  * dpiX / 72.0 );
	newHeight = round_up( boundingBox().height() * dpiY / 72.0 );
    }
    else
    {
	newWidth  = round_up( boundingBox().height() * dpiX / 72.0 );
	newHeight = round_up( boundingBox().width()  * dpiY / 72.0 );
    }

    if( newWidth != width() || newHeight != height() )
    {
	setEraseColor( white );
	setFixedSize( newWidth, newHeight );
	kapp->processEvents();

	_backgroundPixmap.resize( size() );
	_backgroundPixmap.fill( white );
	// The line below is needed to work around certain "features" of styles such as liquid
	// see bug:61711 for more info (LPC, 20 Aug '03)
	setBackgroundOrigin( QWidget::WidgetOrigin );
	setErasePixmap( _backgroundPixmap );
    }

    char data[512];

    sprintf( data, "%ld %d %d %d %d %d %g %g",
             ( _doubleBuffer ? 0 : _backgroundPixmap.handle() ),
             orientation2angle( orientation() ),
             boundingBox().llx(), boundingBox().lly(), 
             boundingBox().urx(), boundingBox().ury(),
             dpiX, dpiY );
    XChangeProperty( x11Display(), winId(),
                     _atoms[GHOSTVIEW],
                     XA_STRING, 8, PropModeReplace,
                     (unsigned char*) data, strlen( data ) );

    sprintf( data, "%s %d %d",
             palette2String( _palette ).data(),
             (int)BlackPixel( x11Display(), DefaultScreen( x11Display() ) ),
             (int)WhitePixel( x11Display(), DefaultScreen( x11Display() ) ) );
    XChangeProperty( x11Display(), winId(),
                     _atoms[GHOSTVIEW_COLORS],
                     XA_STRING, 8, PropModeReplace,
                     (unsigned char*) data, strlen( data ) );

    // Make sure the properties are updated immediately.
    XSync( x11Display(), false );

    repaint();

    _widgetDirty = false;
}

bool KPSWidget::startInterpreter()
{
    setupWidget();

    _process = new KProcess;
    if ( _doubleBuffer ) _process->setEnvironment( "GHOSTVIEW", QString(  "%1 %2" ).arg( winId() ).arg( _backgroundPixmap.handle() ) );
    else _process->setEnvironment( "GHOSTVIEW", QString::number( winId() ) );

    *_process << _ghostscriptPath.local8Bit();
    *_process << _ghostscriptArguments;

    if( _usePipe )
	*_process << 
	// The following two lines are their to ensure that we are allowed to read _fileName
	"-dDELAYSAFER" << "-sInputFile="+_fileName << "-c" << 
	"<< /PermitFileReading [ InputFile ] /PermitFileWriting [] /PermitFileControl [] >> setuserparams .locksafe" <<
	"-";
    else
	*_process << _fileName << "-c" << "quit";

    connect( _process, SIGNAL( processExited( KProcess* ) ),
             this, SLOT( slotProcessExited( KProcess* ) ) );
    connect( _process, SIGNAL( receivedStdout( KProcess*, char*, int ) ),
             this, SLOT( gs_output( KProcess*, char*, int ) ) );
    connect( _process, SIGNAL( receivedStderr( KProcess*, char*, int ) ),
             this, SLOT( gs_output( KProcess*, char*, int ) ) );
    connect( _process, SIGNAL( wroteStdin( KProcess*) ),
             this, SLOT( gs_input( KProcess* ) ) );

    kapp->flushX();

    // Finally fire up the interpreter.
    kdDebug(4500) << "KPSWidget: starting interpreter" << endl;
    if( _process->start( KProcess::NotifyOnExit, 
              _usePipe ? KProcess::All : KProcess::AllOutput ) ) 
    {
	_interpreterBusy = true;
	setCursor( waitCursor );

	_stdinReady = true;
	_interpreterReady = false;
	_ghostscriptDirty = false;

	return true;
    }
    else
    {
	KMessageBox::error( this,
	        i18n( "Could not start Ghostscript. This is most likely "
		      "caused by an incorrectly specified interpreter." ) );
	return false;
    }
}

void KPSWidget::stopInterpreter()
{
    kdDebug(4500) << "KPSWidget::stopInterpreter()" << endl;
    // if( !_interpreterBusy ) return;

    if( isInterpreterRunning() )
	_process->kill( SIGHUP );

    _process = 0;
    while ( !_inputQueue.empty() ) _inputQueue.pop();

    _interpreterBusy = false;
    unsetCursor();
}

void KPSWidget::interpreterFailed()
{
    stopInterpreter();
}

void KPSWidget::slotProcessExited( KProcess* process )
{
    kdDebug(4500) << "KPSWidget: process exited" << endl;

    if ( process == _process )
    {
	kdDebug( 4500 ) << "KPSWidget::slotProcessExited(): looks like it was not a clean exit." << endl;
	if ( process->normalExit() ) {
	    emit ghostscriptError( QString( i18n( "Exited with error code %1." ).arg( process->exitStatus() ) ) );
	} else {
	    emit ghostscriptError( QString( i18n( "Process killed or crashed." ) ) );
	}
	_process = 0;
	stopInterpreter();
	unsetCursor();
    }
}

void KPSWidget::gs_output( KProcess*, char* buffer, int len )
{
    emit output( buffer, len );
}

void KPSWidget::gs_input( KProcess* process )
{
    kdDebug(4500) << "KPSWidget::gs_input" << endl;

    if (process != _process)
    {
      kdDebug(4500) << "KPSWidget::gs_input(): process != _process" << endl;
      return;
    }
    _stdinReady = true;

    while( ! _inputQueue.empty() && _inputQueue.front().len == 0 ) _inputQueue.pop();
    if( _inputQueue.empty() ) {
	_interpreterReady = true;
	return;
    }

    Record& current = _inputQueue.front();

    if ( fseek( current.fp, current.begin, SEEK_SET ) ) {
	kdDebug(4500) << "KPSWidget::gs_input(): seek failed!" << endl;
	interpreterFailed();
	return;
    }
    Q_ASSERT( current.len > 0 );

    const unsigned buffer_size = 4096;
    if ( !_buffer ) _buffer = static_cast<char*>( operator new( buffer_size ) );
    const int bytesRead = fread( _buffer, sizeof (char), 
	    QMIN( buffer_size, current.len ),
	    current.fp );
    if( bytesRead > 0 ) 
    {
	current.begin += bytesRead;
	current.len -= bytesRead;
	if( process && process->writeStdin( _buffer, bytesRead ) )
	    _stdinReady = false;
	else
	    interpreterFailed();
    }
    else
	interpreterFailed();
}

void KPSWidget::readSettings()
{
    setGhostscriptPath( Configuration::interpreter() );

    QStringList arguments;

    if( Configuration::antialiasing() )
	arguments = QStringList::split( " ", Configuration::antialiasingArguments() );
    else
	arguments = QStringList::split( " ", Configuration::nonAntialiasingArguments() );

    if( !Configuration::platformFonts() )
	arguments << "-dNOPLATFONTS";

    arguments << "-dNOPAUSE" << "-dQUIET" << "-dSAFER" << "-dPARANOIDSAFER";

    setGhostscriptArguments( arguments );

    setPalette( static_cast<Configuration::EnumPalette::type>( Configuration::palette() ) );
}

bool KPSWidget::x11Event( XEvent* e )
{
    if( e->type == ClientMessage )
    {
	_gsWindow = e->xclient.data.l[0];
	
	if( e->xclient.message_type == _atoms[PAGE] )
	{
	    kdDebug(4500) << "KPSWidget: received PAGE" << endl;
	    _interpreterBusy = false;
	    unsetCursor();
	    emit newPageImage( _backgroundPixmap );
	    if ( _doubleBuffer ) setErasePixmap( _backgroundPixmap );
	    return true;
	}
	else if( e->xclient.message_type == _atoms[DONE] )
	{
	    kdDebug(4500) << "KPSWidget: received DONE" << endl;
	    stopInterpreter();
	    return true;
	}
    }
    return QWidget::x11Event( e );
}

#include "kpswidget.moc"

// vim:sw=4:sts=4:ts=8:noet
