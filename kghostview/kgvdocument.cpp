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

#include <algorithm>
#include <memory>

#include <qfileinfo.h>

#include <kconfig.h>
#include <kfiledialog.h>
#include <kfilterdev.h>
#include <kinstance.h>
#include <kmessagebox.h>
#include <kmimetype.h>
#include <kprinter.h>
#include <kprocess.h>
#include <ktempfile.h>
#include <kio/netaccess.h>
#include <klocale.h>
#include <kdebug.h>

#include "configuration.h"
#include "kdscerrordialog.h"
#include "kgv_miniwidget.h"
#include "marklist.h"
#include "kgvfactory.h"

extern "C" {
#include "ps.h"
}

#include "kgvdocument.h"

using namespace std;
using namespace KGV;

KGVDocument::KGVDocument( KGVPart* part, const char* name ) : 
    QObject( part, name ),
    _psFile( 0 ),
    _part( part ),
    _tmpUnzipped( 0 ),
    _tmpFromPDF( 0 ),
    _tmpDSC( 0 ),
    _isFileOpen( false ),
    _dsc( 0 )

{
    readSettings();

    _pdf2dsc = new Pdf2dsc( _interpreterPath, this );
    connect( _pdf2dsc, SIGNAL( finished( bool ) ),
             SLOT( openPDFFileContinue( bool ) ) );
}

KGVDocument::~KGVDocument()
{
    close();
}

void KGVDocument::readSettings()
{
    _interpreterPath = Configuration::interpreter();
}

/*- OPENING and READING ---------------------------------------------------*/

void KGVDocument::openFile( const QString& name, const QString& mimetype )
{
    kdDebug(4500) << "KGVDocument::openFile" << endl;

    close();
    _fileName = name;
    _mimetype = mimetype;

    QTimer::singleShot( 0, this, SLOT( doOpenFile() ) );
}

void KGVDocument::doOpenFile()
{
    QFileInfo fileInfo( _fileName );
    if( !fileInfo.exists() ) 
    {
	KMessageBox::sorry( _part->widget(),
		i18n( "<qt>Could not open <nobr><strong>%1</strong></nobr>: "
		      "File does not exist.</qt>" )
		.arg( _fileName ) );
	emit canceled( QString() );
	return;
    }
    if( !fileInfo.isReadable() )
    {
    	KMessageBox::sorry( _part->widget(),
		i18n( "<qt>Could not open <nobr><strong>%1</strong></nobr>: "
		      "Permission denied.</qt>" )
		.arg( _fileName ) );
	emit canceled( QString() );
	return;
    }
   
    if( uncompressFile() )
    {
	kdDebug( 4500 ) << "FILENAME: " << _fileName << endl;
	KMimeType::Ptr mimetype = KMimeType::findByPath( _fileName );
	kdDebug(4500) << "KGVDocument::mimetype: " << mimetype->name() 
	              << endl;
	_mimetype = mimetype->name();
    }	
    
    // If the file contains a PDF document, create a DSC description file
    // of the PDF document. This can be passed to Ghostscript just like
    // an ordinary PS file.
    if( _mimetype == "application/pdf"
     || _mimetype == "application/x-pdf" ) // see bug:67474
    {
	_tmpDSC = new KTempFile( QString::null, ".ps" );
	Q_CHECK_PTR( _tmpDSC );
	if( _tmpDSC->status() != 0 ) {
	    KMessageBox::error( _part->widget(),
		    i18n( "Could not create temporary file: %1" )
		    .arg( strerror( _tmpDSC->status() ) ) );
	    emit canceled( QString() );
	    return;
	}
	
	// When pdf2dsc has finished the program will continue with 
	// openPDFFileContinue.
	_pdf2dsc->run( _fileName, _tmpDSC->name() );
	return;
    }
    else if( _mimetype == "application/postscript"
	  || _mimetype == "application/x-postscript" // see bug:71546
          || _mimetype == "application/illustrator"
          || _mimetype == "image/x-eps"
	  || _mimetype == "text/plain" )
    {
	_format = PS;
	openPSFile();
	return;
    }
    else 
    {
	KMessageBox::sorry( _part->widget(),
	        i18n( "<qt>Could not open <nobr><strong>%1</strong></nobr> "
	              "which has type <strong>%2</strong>. KGhostview can "
	              "only load PostScript (.ps, .eps) and Portable "
	              "Document Format (.pdf) files.</qt>" )
		      .arg( _fileName )
		      .arg( _mimetype ) );
	emit canceled( QString() );
	return;
    }
}

bool KGVDocument::uncompressFile()
{
    // If the file is gzipped, gunzip it to the temporary file _tmpUnzipped.
    kdDebug(4500) << "KGVDocument::uncompressFile()" << endl;

    auto_ptr<QIODevice> filterDev( KFilterDev::deviceForFile( _fileName, _mimetype, true ) );
    if ( !filterDev.get() ) {
        KMimeType::Ptr mt = KMimeType::mimeType(_mimetype);
	if ( (_fileName.right( 3 ) == ".gz") || mt->is("application/x-gzip") ) {
	    kdDebug(4500) << "KGVDocument::uncompressFile(): manually guessing gzip" << endl;
	    filterDev.reset( KFilterDev::deviceForFile( _fileName, "application/x-gzip", true ) );
	} else if ( (_fileName.right( 4 ) == ".bz2") || mt->is("application/x-bzip2") ) {
	    kdDebug(4500) << "KGVDocument::uncompressFile(): manually guessing bzip2" << endl;
	    filterDev.reset( KFilterDev::deviceForFile( _fileName, "application/x-bzip2", true ) );
	} else {
	    kdDebug( 4500 ) << "KGVDocument::uncompressFile(): Unable to guess " << _fileName << endl;
	}
	if ( !filterDev.get() )
	    return false;
    }
    if( !filterDev->open( IO_ReadOnly ) ) 
    {
	KMessageBox::error( _part->widget(),
		i18n( "<qt>Could not uncompress <nobr><strong>%1</strong></nobr>.</qt>" )
		.arg( _fileName ) );
	emit canceled( QString() );
	return false;
    }

    _tmpUnzipped = new KTempFile;
    Q_CHECK_PTR( _tmpUnzipped );
    if( _tmpUnzipped->status() != 0 ) 
    {
	KMessageBox::error( _part->widget(),
		i18n( "Could not create temporary file: %2" )
		.arg( strerror( _tmpUnzipped->status() ) ) );
	emit canceled( QString() );
	return false;
    }


    QByteArray buf( 8192 );
    int read = 0, wrtn = 0;
    while( ( read = filterDev->readBlock( buf.data(), buf.size() ) )
	    > 0 ) 
    {
	wrtn = _tmpUnzipped->file()->writeBlock( buf.data(), read );
	if( read != wrtn )
	    break;
    }

    if( read != 0 ) 
    {
	KMessageBox::error( _part->widget(),
	    i18n( "<qt>Could not uncompress <nobr><strong>%1</strong></nobr>.</qt>" )
	    .arg( _fileName ) );
	emit canceled( QString() );
	return false;
    }

    _tmpUnzipped->close();
    _fileName = _tmpUnzipped->name();
    return true;
}

void KGVDocument::openPDFFileContinue( bool pdf2dscResult )
{	
    kdDebug(4500) << "KGVDocument::openPDFFileContinue" << endl;
    
    if( !pdf2dscResult ) 
    {
	KMessageBox::error( _part->widget(),
		i18n( "<qt>Could not open file <nobr><strong>%1</strong></nobr>.</qt>" ) 
		.arg( _part->url().url() ) );
	emit canceled( QString() );
	return;
    }

    _tmpDSC->close();
    _format = PDF;

    openPSFile(_tmpDSC->name());
}

void KGVDocument::openPSFile(const QString &file)
{
    QString fileName = file.isEmpty() ? _fileName : file;
    kdDebug(4500) << "KGVDocument::openPSFile (" << fileName << ")" << endl;
    
    FILE* fp = fopen( QFile::encodeName( fileName ), "r");
    if( fp == 0 ) 
    {
	KMessageBox::error( _part->widget(),
		i18n( "<qt>Error opening file <nobr><strong>%1</strong></nobr>: %2</qt>" )
		.arg( _part->url().url() )
		.arg( strerror( errno ) ) );
	emit canceled( "" );
	return;
    }
    else 
    {
	_psFile = fp;
	_isFileOpen = true;
	scanDSC();
	emit completed();
    }
}

void KGVDocument::fileChanged( const QString& name )
{
    kdDebug(4500) << "KGVDocument: fileChanged " << name << endl;

    if( !_psFile )
	return;

    // unsigned int savepage = _currentPage;

    /*
    if( openFile( name ) )
	goToPage( savepage );
    else
	emit fileChangeFailed();
    */
}

void KGVDocument::scanDSC()
{
    _dsc = new KDSC();

    // Disable errorDialog for now while KDSCErrorDialog isn't fully 
    // implemented 
    // KDSCErrorDialog errorDialog;
    // KDSCErrorThreshold errorHandler( 3, &errorDialog );
    // _dsc->setErrorHandler( &errorHandler );

    char buf[4096];
    int  count;
    /*
    QTime clock;
    clock.start();
    */
    while( ( count = fread( buf, sizeof(char), sizeof( buf ), _psFile ) ) != 0 )
    {
	_dsc->scanData( buf, count );
	/*
	if( clock.elapsed() > 10 )
	{
	    kapp->processEvents();
	    clock.start();
	}
	*/
    }
    _dsc->fixup();
    // _dsc->setErrorHandler( 0 );
}

void KGVDocument::close()
{
    _pdf2dsc->kill();
    _isFileOpen = false;

    delete _dsc; 
    _dsc = 0;

    if( _psFile )
    {
	fclose( _psFile );
	_psFile = 0;
    }

    clearTemporaryFiles();
}

bool KGVDocument::convertFromPDF( const QString& saveFileName, 
                                    unsigned int firstPage,
                                    unsigned int lastPage )
{
    // TODO -- timeout/fail on this conversion (it can hang on a bad pdf)
    // TODO -- use output from gs (leave out -q) to drive a progress bar
    KProcess process;
    process << _interpreterPath
            << "-q"
            << "-dNOPAUSE"
            << "-dBATCH"
            << "-dSAFER"
            << "-dPARANOIDSAFER"
            << "-sDEVICE=pswrite"
            << ( QCString("-sOutputFile=")+QFile::encodeName(saveFileName) )
            << ( QString("-dFirstPage=")+QString::number( firstPage ) )
            << ( QString("-dLastPage=")+QString::number( lastPage ) )
            << "-c"
            << "save"
            << "pop"
            << "-f"
            << QFile::encodeName(_fileName);

    /*QValueList<QCString> args = process.args();
    QValueList<QCString>::Iterator it = args.begin();
    for ( ; it != args.end() ; ++it )
        kdDebug(4500) << ( *it ) << endl;*/

    if( !process.start( KProcess::Block ) )
    {
	kdError() << "convertFromPDF: Couldn't start gs process" << endl;
	// TODO -- error message (gs not found?)
	return false;
    }
    if ( !process.normalExit() || process.exitStatus() != 0 )
    {
	kdError() << "convertFromPDF: normalExit=" << process.normalExit() << " exitStatus=" << process.exitStatus() << endl;
	// TODO -- error message (can't open, strerr())
	return false;
    }

    return true;
}

void KGVDocument::clearTemporaryFiles()
{
    if( _tmpUnzipped ) {
	_tmpUnzipped->setAutoDelete( true );
	delete _tmpUnzipped;
	_tmpUnzipped = 0;
    }
    if( _tmpFromPDF ) {
	_tmpFromPDF->setAutoDelete( true );
	delete _tmpFromPDF;
	_tmpFromPDF = 0;
    }
    if( _tmpDSC ) {
	_tmpDSC->setAutoDelete( true );
	delete _tmpDSC;
	_tmpDSC = 0;
    }
}


/*- DOCUMENT --------------------------------------------------------------*/

QStringList KGVDocument::mediaNames() const
{   
    QStringList names;

    const CDSCMEDIA* m = dsc_known_media;
    while( m->name ) {
	names << m->name;
	m++;
    }

    if( isOpen() && dsc()->media() ) {
	for( unsigned int i = 0; i < dsc()->media_count(); i++ ) {
	    if( dsc()->media()[i] && dsc()->media()[i]->name )
		names << dsc()->media()[i]->name;
	}
    }

    return names;
}

const CDSCMEDIA* KGVDocument::findMediaByName( const QString& mediaName ) const
{
    if( !isOpen() )
	return 0;
    
    if( dsc()->media() ) {
	for( unsigned int i = 0; i < dsc()->media_count(); i++ ) {
	    if( dsc()->media()[i] && dsc()->media()[i]->name
	     && qstricmp( mediaName.local8Bit(), 
	                  dsc()->media()[i]->name ) == 0 ) {
		return dsc()->media()[i];
	    }
	}
    }
    /* It didn't match %%DocumentPaperSizes: */
    /* Try our known media */
    const CDSCMEDIA *m = dsc_known_media;
    while( m->name ) {
        if( qstricmp( mediaName.local8Bit(), m->name ) == 0 ) {
	    return m;
	}
	m++;
    }

    return 0;
}

QSize KGVDocument::computePageSize( const QString& mediaName ) const
{
    kdDebug(4500) << "KGVDocument::computePageSize( " << mediaName << " )" << endl;

    if( mediaName == "BoundingBox" ) {
	if( dsc()->bbox().get() != 0 )
	    return dsc()->bbox()->size();
	else
	    return QSize( 0, 0 );
    }

    const CDSCMEDIA* m = findMediaByName( mediaName );
    Q_ASSERT( m );
    return QSize( static_cast<int>( m->width ), static_cast<int>( m->height ) );
}


/*- PRINTING and SAVING ---------------------------------------------------*/

QString KGVDocument::pageListToRange( const PageList& pageList )
{
    QString range;

    // Iterators marking the begin and end of a successive sequence 
    // of pages.
    PageList::const_iterator bss( pageList.begin() );
    PageList::const_iterator ess;
       
    PageList::const_iterator it ( pageList.begin() );
    
    while( it != pageList.end() )
    {
	ess = it++;

	// If ess points to the end of a successive sequence of pages,
	// add the stringrepresentation of the sequence to range and
	// update bss.
	if( it == pageList.end() || *it != (*ess) + 1 )
	{
	    if( !range.isEmpty() )
		range += ",";

	    if( bss == ess )
		range += QString::number( *ess );
	    else
		range += QString( "%1-%2" ).arg( *bss ).arg( *ess );

	    bss = it;
	}
    }

    return range;
}

void KGVDocument::print()
{
    if( !dsc() ) return;

    KPrinter printer;
    
    if( dsc()->isStructured() ) 
    {
	printer.setPageSelection( KPrinter::ApplicationSide );

	printer.setCurrentPage( _part->miniWidget()->displayOptions().page() + 1 );
	printer.setMinMax( 1, dsc()->page_count() );
	printer.setOption( "kde-range", 
	                    pageListToRange( _part->markList()->markList() ) );

	if( printer.setup( _part->widget(), i18n("Print %1").arg(_part->url().fileName()) ) ) 
	{
	    KTempFile tf( QString::null, ".ps" );
	    if( tf.status() == 0 ) 
	    {
		if ( printer.pageList().empty() ) { 
		    KMessageBox::sorry( 0,
			    i18n( "Printing failed because the list of "
				"pages to be printed was empty." ),
			    i18n( "Error Printing" ) );
		} else if ( savePages( tf.name(), printer.pageList() ) ) {
		    printer.printFiles( QStringList( tf.name() ), true );
		} else {
		    KMessageBox::error( 0, i18n( "<qt><strong>Printing failure:</strong><br>Could not convert to PostScript</qt>" ) );
		}
	    }
	    else 
	    {
	        // TODO: Proper error handling
	        ;
	    }
	}
    }
    else 
    {
	printer.setPageSelection( KPrinter::SystemSide );
	
	if( printer.setup( _part->widget(), i18n("Print %1").arg(_part->url().fileName()) ) )
	    printer.printFiles( _fileName );
    }
}

void KGVDocument::saveAs()
{
    if( !isOpen() )
	return;
    
    KURL saveURL = KFileDialog::getSaveURL( 
                          _part->url().isLocalFile() 
                              ? _part->url().url() 
                              : _part->url().fileName(), 
                          QString::null, 
                          _part->widget(), 
                          QString::null );
    if( !KIO::NetAccess::upload( _fileName,
				 saveURL,
				 static_cast<QWidget*>( 0 ) ) ) {
	// TODO: Proper error dialog
    }
}

bool KGVDocument::savePages( const QString& saveFileName,
                               const PageList& pageList )
{
    if( pageList.empty() )
	return true;
    
    if( _format == PDF ) 
    {
	KTempFile psSaveFile( QString::null, ".ps" );
	psSaveFile.setAutoDelete( true );
	if( psSaveFile.status() != 0 )
	    return false;

	// Find the minimum and maximum pagenumber in pageList.
	int minPage = pageList.first(), maxPage = pageList.first();
	for( PageList::const_iterator ci = pageList.begin();
	     ci != pageList.end(); ++ci )
	{
	    minPage = QMIN( *ci, minPage );
	    maxPage = QMAX( *ci, maxPage );
	}
	

	// TODO: Optimize "print whole document" case
	//
	// The convertion below from PDF to PS creates a temporary file which, in
	// the case where we are printing the whole document will then be
	// completelly copied to another temporary file.
	//
	// In very large files, the inefficiency is visible (measured in
	// seconds).
	//
	// luis_pedro 4 Jun 2003
	
	
	
	// Convert the pages in the range [minPage,maxPage] from PDF to
	// PostScript.
	if( !convertFromPDF( psSaveFile.name(), 
	                     minPage, maxPage ) )
	    return false;

	// The page minPage in the original file becomes page 1 in the 
	// converted file. We still have to select the desired pages from
	// this file, so we need to take into account that difference.
	PageList normedPageList;
	transform( pageList.begin(), pageList.end(),
	           back_inserter( normedPageList ),
	           bind2nd( minus<int>(), minPage - 1 ) );
	
	// Finally select the desired pages from the converted file.
	psCopyDoc( psSaveFile.name(), saveFileName, normedPageList );
    }
    else
    {
	psCopyDoc( _fileName, saveFileName, pageList );
    }

    return true;
}

// length calculates string length at compile time
// can only be used with character constants
#define length( a ) ( sizeof( a ) - 1 )

// Copy the headers, marked pages, and trailer to fp

bool KGVDocument::psCopyDoc( const QString& inputFile,
	const QString& outputFile, const PageList& pageList )
{
    FILE* from;
    FILE* to;
    char text[ PSLINELENGTH ];
    char* comment;
    bool pages_written = false;
    bool pages_atend = false;
    unsigned int i = 0;
    unsigned int pages = 0;
    long here;

    kdDebug(4500) << "KGVDocument: Copying pages from " << inputFile << " to "
    << outputFile << endl;

    pages = pageList.size();

    if( pages == 0 ) {
	KMessageBox::sorry( 0,
		i18n( "Printing failed because the list of "
				"pages to be printed was empty." ),
			  i18n( "Error Printing" ) );
	return false;
    }

    from = fopen( QFile::encodeName( inputFile ), "r" );
    to = fopen( QFile::encodeName( outputFile ), "w" );

    // Hack in order to make printing of PDF files work. FIXME
    CDSC* dsc;

    if( _format == PS )
	dsc = _dsc->cdsc();
    else {
	FILE* fp = fopen( QFile::encodeName( inputFile ), "r");
	char buf[256];
	int count;
	dsc = dsc_init( 0 );
	while( ( count = fread( buf, 1, sizeof( buf ), fp ) ) != 0 )
	    dsc_scan_data( dsc, buf, count );
	fclose( fp );
        if( !dsc )
            return false;

	dsc_fixup( dsc );
    }

    here = dsc->begincomments;
    while( ( comment = pscopyuntil( from, to, here,
	     dsc->endcomments, "%%Pages:" ) ) ) {
	here = ftell( from );
	if( pages_written || pages_atend ) {
	    free( comment );
	    continue;
	}
	sscanf( comment + length("%%Pages:" ), "%256s", text );
	text[256] = 0; // Just in case of an overflow
	if( strcmp( text, "(atend)" ) == 0 ) {
	    fputs( comment, to );
	    pages_atend = true;
	}
	else {
	    switch ( sscanf( comment + length( "%%Pages:" ), "%*d %u", &i ) )  {
	    case 1:
		fprintf( to, "%%%%Pages: %d %d\n", pages, i );
		break;
	    default:
		fprintf( to, "%%%%Pages: %d\n", pages );
		break;
	    }
	    pages_written = true;
	}
	free(comment);
    }
    pscopy( from, to, dsc->beginpreview, dsc->endpreview );
    pscopy( from, to, dsc->begindefaults, dsc->enddefaults );
    pscopy( from, to, dsc->beginprolog, dsc->endprolog );
    pscopy( from, to, dsc->beginsetup, dsc->endsetup );

    //TODO -- Check that a all dsc attributes are copied

    unsigned int count = 1;
    PageList::const_iterator it;
    for( it = pageList.begin(); it != pageList.end(); ++it ) {
	i = (*it) - 1;
	comment = pscopyuntil( from, to, dsc->page[i].begin,
			       dsc->page[i].end, "%%Page:" );
	if ( comment ) free( comment );
	fprintf( to, "%%%%Page: %s %d\n", dsc->page[i].label,
		 count++ );
	pscopy( from, to, -1, dsc->page[i].end );
    }

    here = dsc->begintrailer;
    while( ( comment = pscopyuntil( from, to, here,
				  dsc->endtrailer, "%%Pages:" ) ) ) {
	here = ftell( from );
	if ( pages_written ) {
	    free( comment );
	    continue;
	}
	switch ( sscanf( comment + length( "%%Pages:" ), "%*d %u", &i ) ) {
	case 1:
	    fprintf( to, "%%%%Pages: %d %d\n", pages, i );
	    break;
	default:
	    fprintf( to, "%%%%Pages: %d\n", pages );
	    break;
	}
	pages_written = true;
	free( comment );
    }

    fclose( from );
    fclose( to );

    if( _format == PDF )
	dsc_free( dsc );

    return true;
}

#undef length


/*- Conversion stuff ------------------------------------------------------*/

/*
void KGVDocument::runPdf2ps( const QString& pdfName, 
                               const QString& dscName )
{
    KProcess process;
    process << _interpreterPath
	    << "-dNODISPLAY"
	    << "-dQUIET"
	    << QString( "-sPDFname=%1" ).arg( pdfName )
	    << QString( "-sDSCnamale locale( "kghostview" );
    _fallBackPageMedia = pageSizeToString( 
              static_cast< QPrinter::PageSize >( locale.pageSize() ) );
    
    _usePageLabels = false;
e=%1" ).arg( dscName )
	    << "pdf2dsc.ps"
	    << "-c"
	    << "quit";

    connect( &process, SIGNAL( processExited( KProcess* ) ),
	     this, SLOT( pdf2psExited( KProcess* ) ) );

    kdDebug(4500) << "KGVDocument: pdf2ps started" << endl;
    process.start( KProcess::NotifyOnExit );
}

void KGVDocument::pdf2psExited( KProcess* process )
{
    kdDebug(4500) << "KGVDocument: pdf2ps exited" << endl;
    
    emit pdf2psFinished( process.normalExit() && process.exitStatus() != 0 );
}
*/

Pdf2dsc::Pdf2dsc( const QString& ghostscriptPath, QObject* parent, const char* name ) :
    QObject( parent, name ),
    _process( 0 ),
    _ghostscriptPath( ghostscriptPath )
{}

Pdf2dsc::~Pdf2dsc()
{
    kill();
}

void Pdf2dsc::run( const QString& pdfName, const QString& dscName )
{
    kill();

    _process = new KProcess;
    *_process << _ghostscriptPath
              << "-dSAFER"
              << "-dPARANOIDSAFER"
              << "-dDELAYSAFER"
              << "-dNODISPLAY"
              << "-dQUIET"
              << QString( "-sPDFname=%1" ).arg( pdfName )
              << QString( "-sDSCname=%1" ).arg( dscName )
              << "-c"
              << "<< /PermitFileReading [ PDFname ] /PermitFileWriting [ DSCname ] /PermitFileControl [] >> setuserparams .locksafe"
              << "-f"
              << "pdf2dsc.ps"
              << "-c"
              << "quit";

    connect( _process, SIGNAL( processExited( KProcess* ) ),
	     this, SLOT( processExited() ) );

    kdDebug(4500) << "Pdf2dsc: started" << endl;
    _process->start( KProcess::NotifyOnExit );
}

void Pdf2dsc::kill()
{
    if( _process != 0 )
    {
	kdDebug(4500) << "Pdf2dsc: killing current process" << endl;
	delete _process;
	_process = 0;
    }
}

void Pdf2dsc::processExited()
{
    kdDebug(4500) << "Pdf2dsc: process exited" << endl;
    
    emit finished( _process->normalExit() && _process->exitStatus() == 0 );
    delete _process;
    _process = 0;
}

#include "kgvdocument.moc"


// vim:sw=4:sts=4:ts=8:sta:tw=78:noet
