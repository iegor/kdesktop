/** 
 * Copyright (C) 1997-2003 the KGhostView authors. See file AUTHORS.
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

#include <math.h>

#include <qlistbox.h>

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kinstance.h>
#include <kdeversion.h>
#include <klocale.h>
#include <kmessagebox.h>

// KLineEditDlg is depricated as of 3.2. use KInputDialog instead
#if KDE_VERSION >= KDE_MAKE_VERSION(3,1,90)
  #include <kinputdialog.h>
#else
  #include <qvalidator.h>
  #include <qwidget.h>
  #include <klineeditdlg.h>
#endif

#include <config.h>
#include <stdlib.h>

#include "infodialog.h"
#include "marklist.h"
#include "kgvdocument.h"
#include "kgv_view.h"
#include "version.h"
#include "scrollbox.h"

#include "kgv_miniwidget.h"

#include "kpswidget.h"

using namespace KGV;

KGVMiniWidget::KGVMiniWidget( KGVPart* part, const char* name ) : 
    QObject( part, name ),
    _document( 0 ),
    _part( part ),
    _psWidget( 0 ),
    _usePageLabels( true ),
    _visiblePage( -1 )
{
    KLocale locale( "kghostview" );
    _fallBackPageMedia = pageSizeToString( 
              static_cast< QPrinter::PageSize >( locale.pageSize() ) );
    _thumbnailService = new ThumbnailService( this );

    connect( this, SIGNAL( newPageShown( int ) ), 
             SLOT( updateStatusBarText( int ) ) );
}

void KGVMiniWidget::setDocument( KGVDocument* document )
{
    _document = document;
    if( _document )
	connect( _document, SIGNAL( completed() ), 
	         SLOT( slotDocumentOpened() ) );
}

QString KGVMiniWidget::pageSizeToString( QPrinter::PageSize pageSize )
{
    switch( pageSize )
    {
    case QPrinter::A3:     return "A3";
    case QPrinter::A4:     return "A4";
    case QPrinter::A5:     return "A5";
    case QPrinter::B4:     return "B4";
    case QPrinter::Ledger: return "Ledger";
    case QPrinter::Legal:  return "Legal";
    case QPrinter::Letter: return "Letter";
    default:               return "Unknown";
    }
}

void KGVMiniWidget::reset()
{
    /*
    if( _psWidget )
	_psWidget->disableInterpreter();
    */

    // return to document defaults
    _options.reset();
    emit setStatusBarText( "" );
}

void KGVMiniWidget::setPSWidget( KPSWidget* psWidget )
{
    _psWidget = psWidget;
    // setMagnification( _magnification );
    connect( _psWidget, SIGNAL( newPageImage( QPixmap ) ), 
             this, SLOT( sendPage() ) );
}

void KGVMiniWidget::goToPage()
{
#if KDE_VERSION >= KDE_MAKE_VERSION(3,1,90)
	int num;
	bool ok = false;
	num = KInputDialog::getInteger(i18n("Go to Page"), i18n("Page:"), 1,
		1, dsc()->page_count(), 1, 10, &ok, _part->widget());
	if (ok) goToPage( num-1 );
#else
    QString num;
    bool b = false;
    num = KLineEditDlg::getText(i18n("Go to Page"), i18n("Page:"), QString::null, &b, _part->widget(), new QIntValidator(1, dsc()->page_count(), this));
    if (b) goToPage( num.toInt() - 1 );
#endif
}

void KGVMiniWidget::info()
{
    if( !document()->isOpen() ) 
	return;

    InfoDialog* infoDialog = new InfoDialog( _part->widget(), "info", true );
    infoDialog->setup( _part->url().prettyURL(),
                        dsc()->dsc_title(), dsc()->dsc_date() );
    infoDialog->exec();
    delete infoDialog;
}

void KGVMiniWidget::goToPage( int page )
{
    if( _options.page() != page ) {
	_options.setPage( page );
	showPage( _options.page() );
    }
}

void KGVMiniWidget::zoomIn()
{
    if ( _options.zoomIn() ) showPage( _options.page() );
}

void KGVMiniWidget::zoomOut()
{
    if ( _options.zoomOut() ) showPage( _options.page() );
}

bool KGVMiniWidget::atMaxZoom() const
{
    return !_options.canZoomIn();
}

bool KGVMiniWidget::atMinZoom() const
{
    return !_options.canZoomOut();
}

void KGVMiniWidget::fitWidth( unsigned int width )
{
    if ( (orientation() == CDSC_LANDSCAPE) || (orientation() == CDSC_SEASCAPE) )
        setMagnification( ( (double)width / QPaintDevice::x11AppDpiY()) /
                      ( (double)boundingBox().height() / 72) );
    else // default
        setMagnification( ( (double)width / QPaintDevice::x11AppDpiX() ) /
                      ( (double)boundingBox().width() / 72) );
}

void KGVMiniWidget::fitHeight( unsigned int height )
{
    if ( (orientation() == CDSC_LANDSCAPE) || (orientation() == CDSC_SEASCAPE) )
        setMagnification( ( (double)height / QPaintDevice::x11AppDpiY()) /
                      ( (double)boundingBox().width() / 72) );
    else  //default
        setMagnification( ( (double)height / QPaintDevice::x11AppDpiY()) /
                      ( (double)boundingBox().height() / 72) );
}

void KGVMiniWidget::fitWidthHeight( unsigned int w, unsigned int h )
{
    double magnification = std::min<double>( 
	    ( ( double )h / QPaintDevice::x11AppDpiY() ) /
	    ( ( double )boundingBox().height() / 72.0 ),
	    ( ( double )w / QPaintDevice::x11AppDpiX() ) /
	    ( ( double )boundingBox().width() / 72.0 ) );
    setMagnification( magnification );
}

void KGVMiniWidget::firstPage()
{
    goToPage( 0 );
}

void KGVMiniWidget::lastPage()
{
    if ( !dsc() ) return;

    goToPage( dsc()->page_count() - 1 );
}

bool KGVMiniWidget::prevPage()
{
    if ( !dsc() ) return false;

    int new_page = 0;

    if( dsc()->isStructured() ) {
	new_page = _options.page() - 1;
	if( new_page < 0 )
	    return false;
    }

    goToPage( new_page );
    return true;
}

bool KGVMiniWidget::nextPage()
{
    if ( !dsc() ) return false;

    int new_page = 0;

    if( dsc()->isStructured() ) {
	new_page = _options.page() + 1;
	if( (unsigned int)new_page >= dsc()->page_count() )
	    return false;
    }

    goToPage( new_page );
    return true;
}


void KGVMiniWidget::redisplay ()
{
    if( !document()->psFile() )
	return;

    _psWidget->stopInterpreter();
    showPage( _options.page() );
}

void KGVMiniWidget::restoreOverrideOrientation()
{
    _options.restoreOverrideOrientation();
    showPage( _options.page() );
}

void KGVMiniWidget::setOverrideOrientation( CDSC_ORIENTATION_ENUM orientation )
{
    _options.setOverrideOrientation( orientation );
    showPage( _options.page() );
}

CDSC_ORIENTATION_ENUM KGVMiniWidget::orientation() const
{
    if( _options.overrideOrientation() != CDSC_ORIENT_UNKNOWN )
	return _options.overrideOrientation();
    else if( dsc()->page_orientation() != CDSC_ORIENT_UNKNOWN )
	return static_cast< CDSC_ORIENTATION_ENUM >( dsc()->page_orientation());
    else if( dsc()->bbox().get() != 0 
	  && dsc()->bbox()->width() > dsc()->bbox()->height() )
        return CDSC_LANDSCAPE;
    else
	return CDSC_PORTRAIT;
}

CDSC_ORIENTATION_ENUM KGVMiniWidget::orientation( int pagenumber ) const
{
    if ( !dsc() ||  unsigned( pagenumber ) >= dsc()->page_count() ) {
	return orientation();
    }
    if( _options.overrideOrientation() != CDSC_ORIENT_UNKNOWN ) { 
	return _options.overrideOrientation();
    }
	
    if( dsc()->page()[ pagenumber ].orientation != CDSC_ORIENT_UNKNOWN ) { 
	return static_cast< CDSC_ORIENTATION_ENUM >( dsc()->page()[ pagenumber ].orientation );
    }
    if( dsc()->page_orientation() != CDSC_ORIENT_UNKNOWN ) { 
	return static_cast< CDSC_ORIENTATION_ENUM >( dsc()->page_orientation());
    }
    if( !dsc()->epsf() ) {
	return CDSC_PORTRAIT;
    }
    if( dsc()->bbox().get() != 0
          && dsc()->bbox()->width() > dsc()->bbox()->height() ) {
	return CDSC_LANDSCAPE;
    }
	return CDSC_PORTRAIT;
}

void KGVMiniWidget::restoreOverridePageMedia()
{
    _options.restoreOverridePageMedia();
    redisplay();
	showPage( _options.page() );
}

void KGVMiniWidget::setOverridePageMedia( const QString& mediaName )
{
    _options.setOverridePageMedia( mediaName );
	showPage( _options.page() );
}

QString KGVMiniWidget::pageMedia() const
{
    if( !_options.overridePageMedia().isNull() )
	return _options.overridePageMedia();
    else if( dsc()->page_media() != 0 )
	return QString( dsc()->page_media()->name );
    else if( dsc()->bbox().get() != 0 )
	return QString( "BoundingBox" );
    else
	return _fallBackPageMedia;
}

QString KGVMiniWidget::pageMedia( int pagenumber ) const
{
    kdDebug( 4500 ) << "KGVMiniWidget::pageMedia( " <<  pagenumber << " )" << endl;
    if ( !dsc() ) return pageMedia();
    if ( unsigned( pagenumber ) >= dsc()->page_count() ) return pageMedia();
    if( !_options.overridePageMedia().isNull() )
	return _options.overridePageMedia();
    else if( dsc()->page()[ pagenumber ].media != 0 )
	return QString( dsc()->page()[ pagenumber ].media->name );
    else if( dsc()->page_media() != 0 )
	return QString( dsc()->page_media()->name );
    else if( dsc()->bbox().get() != 0 )
	return QString( "BoundingBox" );
    else
	return _fallBackPageMedia;
}

KDSCBBOX KGVMiniWidget::boundingBox() const
{
    QString currentMedia = pageMedia();
    if( currentMedia == "BoundingBox" )
	return KDSCBBOX( *dsc()->bbox().get() );
    else {
	QSize size = document()->computePageSize( currentMedia );
	return KDSCBBOX( 0, 0, size.width(), size.height() );
    }
}

KDSCBBOX KGVMiniWidget::boundingBox( int pageNo ) const
{
    QString currentMedia = pageMedia( pageNo );
    if( currentMedia == "BoundingBox" )
	return KDSCBBOX( *dsc()->bbox().get() );
    else {
	QSize size = document()->computePageSize( currentMedia );
	return KDSCBBOX( 0, 0, size.width(), size.height() );
    }
}

bool KGVMiniWidget::atFirstPage() const
{
    return ( _options.page() == 0 );
}

bool KGVMiniWidget::atLastPage() const
{
    return ( _options.page() == static_cast<int>( dsc()->page_count() ) - 1 );
}

void KGVMiniWidget::showPage( int pagenumber )
{
    if( !document()->isOpen() )
	return;

    kdDebug(4500) << "KGVMiniWidget::showPage( " << pagenumber << " )" << endl;

    static_cast< QWidget* >( _psWidget->parent() )->show();

    _psWidget->setFileName(_document->fileName(), dsc()->isStructured() );
    _psWidget->clear();

    if( dsc()->isStructured() ) 
    {
	// Coerce page number to fall in range
	if( ( unsigned int)pagenumber >= dsc()->page_count() )
	    pagenumber = dsc()->page_count() - 1;
	if( pagenumber < 0 )
	    pagenumber = 0;

	_options.setPage( pagenumber );
	
	
        _psWidget->setOrientation( orientation( _options.page() ) );
	_psWidget->setBoundingBox( boundingBox( _options.page() ) );
	_psWidget->setMagnification( _options.magnification() );
   
	if( !_psWidget->isInterpreterRunning() )
	{
	    // Start interpreter, send preamble and send the current page.
	    if( _psWidget->startInterpreter() )
	    {
		_psWidget->sendPS( psFile(), dsc()->beginprolog(),
		                             dsc()->endprolog() );
		_psWidget->sendPS( psFile(), dsc()->beginsetup(),
		                             dsc()->endsetup() );
		_psWidget->sendPS( psFile(), dsc()->page()[ _options.page() ].begin,
		                             dsc()->page()[ _options.page() ].end );
		_visiblePage = _options.page();
	    }
	}
	else
	    sendPage();
    }
    else 
    {
	_psWidget->setOrientation( orientation() );
	_psWidget->setBoundingBox( boundingBox() );
	_psWidget->setMagnification( _options.magnification() );
	
	if( !_psWidget->isInterpreterRunning() ) 
	{
	    // This is not a structured document -- start interpreter
	    _psWidget->startInterpreter();
	    if( !dsc() )
		_psWidget->stopInterpreter();
	}
	else if( _psWidget->isInterpreterReady() )
	    _psWidget->nextPage();
	else 
	{
	    /*
	    KNotifyClient::userEvent
	      (i18n("KGhostview cannot load the document, \"%1\".\n"
		    "It appears to be broken.").arg( _fileName ),
	       KNotifyClient::Messagebox);
	    _psWidget->disableInterpreter();
	    _psFile=0;

	    //TODO: More to do to turn off display?
	    */
	    return;
	}
    }
    // Do this after ajusting pagenumber above
    _thumbnailService->cancelRequests( -1 , _part->scrollBox(), SLOT( setThumbnail( QPixmap ) ) );
    _thumbnailService->delayedGetThumbnail( pagenumber, _part->scrollBox(), SLOT( setThumbnail( QPixmap ) ), true );

    emit newPageShown( pagenumber );
}

void KGVMiniWidget::sendPage()
{
    // Send the page to the interpreter.
    if( !_psWidget->isInterpreterBusy() && _visiblePage != _options.page() )
    {
	// Interpreter ready - Fire off next page
	_psWidget->clear();
	_psWidget->nextPage();
	_psWidget->sendPS( psFile(), dsc()->page()[ _options.page() ].begin,
	                             dsc()->page()[ _options.page() ].end );
	_visiblePage = _options.page();
    }
}

void KGVMiniWidget::updateStatusBarText( int pageNumber )
{
    if( !dsc() )
	return;

    if( dsc()->isStructured() ) 
    {
	QString text;
	
	if( pageNumber == -1 )
	    text = i18n( "Page 1" );
	else
	    if( !_usePageLabels || document()->format() == KGVDocument::PDF )
		text = i18n( "Page %1 of %2" )
		       .arg( pageNumber + 1 )
		       .arg( dsc()->page_count() );
	    else
		text = i18n( "Page %1 (%2 of %3)" )
		       .arg( dsc()->page()[ _options.page() ].label )
		       .arg( pageNumber + 1 )
		       .arg( dsc()->page_count() );

	emit setStatusBarText( text );
    }
}

void KGVMiniWidget::buildTOC()
{
    if( !dsc() ) 
	return;

    // Build table of contents
    // Well, that's what it used to be called !!

    int last_page = 0;

    MarkList*  marklist = _part->markList();

    if( dsc()->isStructured() ) {
	if( _usePageLabels )
	    for( unsigned i = 0; i < dsc()->page_count(); ++i ) {
		unsigned j = i;
		if( dsc()->page_order() == CDSC_DESCEND )
		    j = ( dsc()->page_count() - 1 ) - i;
		last_page = atoi( dsc()->page()[j].label );
	    }

	// finally set marked list
	QString s;
	for( unsigned i = 0; i < dsc()->page_count(); ++i ) {
	    const char * label = dsc()->page()[ i ].label;
	    QString tip = QString::fromLocal8Bit( label ? label : "" );

	    if( !_usePageLabels )
		s.setNum( i + 1 );
	    else
		s = tip;

	    marklist->insertItem( s, i, tip );
	}
    }
    else {
	marklist->insertItem( QString::fromLatin1( "1" ), 0 );
    }
}

void KGVMiniWidget::setMagnification( double magnification )
{
    if ( magnification != _options.magnification() ) {
	_options.setMagnification( magnification );
	showPage( _options.page() );
    }
}

void KGVMiniWidget::enablePageLabels( bool b )
{
    if( _usePageLabels != b ) 
    {
	_usePageLabels = b;
	updateStatusBarText( _options.page() );
	buildTOC();
    }
}

void KGVMiniWidget::slotDocumentOpened()
{
    buildTOC();
    showPage( _options.page() );
}

void KGVMiniWidget::setDisplayOptions( const DisplayOptions& newOptions )
{
    _options = newOptions;
}

#include "kgv_miniwidget.moc"


// vim:sw=4:sts=4:ts=8:sta:tw=78:noet
