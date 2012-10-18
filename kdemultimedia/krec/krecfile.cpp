/***************************************************************************
    copyright            : (C) 2003 by Arnold Krille
    email                : arnold@arnoldarts.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 ***************************************************************************/

#include "krecfile.h"
#include "krecfile.moc"

#include "krecglobal.h"

#include "krecnewproperties.h"

#include <kdebug.h>
#include <kapplication.h>
#include <ktempdir.h>
#include <ktempfile.h>
#include <ktar.h>
#include <kio/job.h>
#include <klocale.h>
#include <ksimpleconfig.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <kmessagebox.h>
#include <qtimer.h>

#include <qdatastream.h>
#include <math.h>

void KRecFile::init() {
	_pos = 0;
	_size = 0;
	_filename = QString::null;
	_currentBuffer = 0;
	_dir = new KTempDir();
	_config = new KSimpleConfig( _dir->name()+"project.rc", false );
}

KRecFile::KRecFile( QObject* p, const char* n )
  : QObject( p,n )
  , _saved( false )
{
	init();
	kdDebug( 60005 ) << k_funcinfo << "_dir->name(): " << _dir->name() << endl;
	_dir->setAutoDelete( true );
	KRecNewProperties* dialog = new KRecNewProperties( KRecGlobal::the()->mainWidget() );

	if ( dialog->usedefaults() )
		KRecGlobal::the()->message( i18n( "Using default properties for the new file" ) );
	else
		dialog->exec();

	_samplerate = dialog->samplerate();
	_channels = dialog->channels();
	_bits = dialog->bits();

	saveProps();

	delete dialog;
}
KRecFile::KRecFile( const QString &filename, QObject* p, const char* n )
  : QObject( p,n )
  , _saved( true )
{
	init();
	_filename = filename;
	kdDebug( 60005 ) << k_funcinfo << "_dir->name(): " << _dir->name() << endl;
	_dir->setAutoDelete( true );

	KTar *tar = new KTar( _filename, "application/x-gzip" );
	tar->open( IO_ReadOnly );
	int i=0; while ( _filename.find( '/', i ) != -1 ) i++; // Find last '/'
	QString basename = _filename.right( _filename.length()-i );
	basename = basename.left( basename.length()-5 );

	const KArchiveDirectory *dir = dynamic_cast<const KArchiveDirectory*>( tar->directory()->entry( basename ) );

	dir->copyTo( _dir->name() );

	delete _config;
	_config = new KSimpleConfig( _dir->name()+"project.rc", false );
	loadProps();
	int c = _config->readNumEntry( "Files" );
	//kdDebug( 60005 ) << c << " Files to load" << endl;
	for ( int i=0; i<c; i++ ) {
		//kdDebug( 60005 ) << "Loading file " << i << endl;
		_config->setGroup( "File-" + QString::number( i ) );
		newBuffer( KRecBuffer::fromConfig( _config, _dir->qDir(), this ) );
	}
	KRecGlobal::the()->message( i18n( "\'%1\' loaded." ).arg( filename ) );

	delete tar;

	_saved = true;
	//kdDebug( 60005 ) << "Buffers opened: " << _buffers.count() << endl;
}
KRecFile::~KRecFile() {
	kdDebug( 60005 ) << k_funcinfo << endl;
	QValueList<KRecBuffer*>::iterator it;
	for ( it = _buffers.begin(); it != _buffers.end(); ++it )
		delete ( *it );
	_buffers.clear();
	delete _dir;
        delete _config;
	//kdDebug( 60005 ) << k_funcinfo << "done." << endl;
}

QString KRecFile::filename() { return _filename; }
void KRecFile::filename( const QString &n ) {
	if ( _filename!=n ) {
		_filename = n;
		emit filenameChanged( _filename );
	}
}

void KRecFile::writeData( Arts::mcopbyte* /*data*/, uint /*length*/ ) {
	kdDebug( 60005 ) << k_funcinfo << endl;
}
void KRecFile::writeData( QByteArray* data ) {
	kdDebug( 60005 ) << k_funcinfo << endl;
	if ( _currentBuffer!=-1 ) _buffers[ _currentBuffer ]->writeData( data );
	_saved = false;
}
void KRecFile::writeData( QByteArray& data ) {
	if ( _currentBuffer!=-1 ) _buffers[ _currentBuffer ]->writeData( data );
	_saved = false;
}

void KRecFile::save( const QString &fname ) {
	QString filetosave = fname;
	
	//kdDebug( 60005 ) << k_funcinfo << filename << endl;
	if ( saved() ) {
		KRecGlobal::the()->message( i18n( "No need to save." ) );
		return;
	}
	
	KRecGlobal::the()->message( i18n( "Saving in progress..." ) );
	filename( fname );
	QString tmpname;
	{
		KTempFile *tmp = new KTempFile();
		tmp->setAutoDelete( true );
		tmpname = tmp->name();
		delete tmp;
	}
	saveProps();

	KTar *tar = new KTar( tmpname, "application/x-gzip" );
	tar->open( IO_WriteOnly );
	int i=0; while ( fname.find( '/', i ) != -1 ) i++; // Find last '/'
	QString basename = fname.right( fname.length()-i );
	if ( basename.endsWith( ".krec" ) )
		basename = basename.left( basename.length()-5 );
	else {
		filetosave = fname + ".krec";
		filename( filetosave );
	}
	tar->addLocalDirectory( _dir->name(), basename );

	delete tar;
	KIO::file_move( tmpname, filetosave, -1, true, false, true );

	KRecGlobal::the()->message( i18n( "Saving \"%1\" was successful." ).arg( filename() ) );
	_saved = true;
}

void KRecFile::exportwave( const QString &filename ) {
	kdDebug( 60005 ) << k_funcinfo << filename << endl;
}

QByteArray* KRecFile::getData( int ) {
	kdDebug( 60005 ) << k_funcinfo << endl;
	return 0;
}

void KRecFile::getData( QByteArray& data ) {
	KRecBuffer* current = getTopBuffer_buffer( _pos );
	if ( current ) {
	//kdDebug( 60005 ) << "_pos=" << _pos << "(" << samplesToOffset( _pos ) << ") current->startpos()=" << current->startpos() << "(" << samplesToOffset( current->startpos() ) << ") that is " << samplesToOffset( _pos - current->startpos() ) << endl;
		current->setPos( samplesToOffset( _pos - current->startpos() ) );
		current->getData( data );
	} else {
		for ( uint i=0; i<data.size(); i++ )
			data[ i ] = 0;
	}
	newPos( _pos + offsetToSamples( data.size() ) );
	if ( position() >= size() ) emit endReached();
}


// * * * Position * * *
void KRecFile::newPos( int p ) {
	if ( _pos != p ) {
		_pos = p;
		emit posChanged( _pos );
	}
}
void KRecFile::newPos( KRecBuffer* buffer, QIODevice::Offset p ) {
	newPos( buffer->startpos() + offsetToSamples( p ) );
}
void KRecFile::newSize( KRecBuffer* buffer, QIODevice::Offset p ) {
	if ( buffer->startpos() + offsetToSamples( p ) > _size ) {
		_size = buffer->startpos() + offsetToSamples( p );
	}
	emit sizeChanged( _size );
}


/// * * * Frames <-> Offset and more * * *
int KRecFile::offsetToSamples( QIODevice::Offset n ) const {
	int out = n / channels();
	if ( bits() == 16 ) out /= 2;
	return out;
}
QIODevice::Offset KRecFile::samplesToOffset( int n ) const {
	QIODevice::Offset out = n * channels();
	if ( bits() == 16 ) out *= 2;
	return out;
}

/// * * * Properties <-> KConfig * * *
void KRecFile::saveProps() {
	kdDebug( 60005 ) << k_funcinfo << endl;
	_config->setGroup( "General" );
	_config->writeEntry( "Samplerate", _samplerate );
	_config->writeEntry( "Bits", _bits );
	_config->writeEntry( "Channels", _channels );

	_config->writeEntry( "Files", _buffers.count() );
	for ( uint i=0; i<_buffers.count(); i++ ) {
		//kdDebug( 60005 ) << "Writing config " << i << endl;
		_config->setGroup( "File-" + QString::number( i ) );
		_buffers[ i ]->writeConfig( _config );
	}
	_config->sync();
}
void KRecFile::loadProps() {
	kdDebug( 60005 ) << k_funcinfo << endl;
	_config->setGroup( "General" );
	_samplerate = _config->readNumEntry( "Samplerate", 44100 );
	_bits = _config->readNumEntry( "Bits", 16 );
	_channels = _config->readNumEntry( "Channels", 2 );
}


/// * * * NewBuffer * * *
void KRecFile::newBuffer( const QString &filename ) {
	kdDebug( 60005 ) << k_funcinfo << filename << endl;
	newBuffer( new KRecBuffer( filename, _pos, true, this ) );
}
void KRecFile::newBuffer( KRecBuffer* buffer ) {
	kdDebug( 60005 ) << k_funcinfo << endl;
	connect( buffer, SIGNAL( posChanged( KRecBuffer*, QIODevice::Offset ) ), this, SLOT( newPos( KRecBuffer*, QIODevice::Offset ) ) );
	connect( buffer, SIGNAL( sizeChanged( KRecBuffer*, QIODevice::Offset ) ), this, SLOT( newSize( KRecBuffer*, QIODevice::Offset ) ) );
	connect( buffer, SIGNAL( deleteSelf( KRecBuffer* ) ), this, SLOT( deleteBuffer( KRecBuffer* ) ) );
	_buffers.append( buffer );
	newSize( buffer, buffer->size() );
	_currentBuffer = _buffers.findIndex( buffer );
	emit sNewBuffer( buffer );
	_saved = false;
}
void KRecFile::newBuffer() {
	kdDebug( 60005 ) << k_funcinfo << endl;
	newBuffer( _dir->name() + "file" + QString::number( _buffers.count() ) + ".raw" );
}

void KRecFile::deleteBuffer( KRecBuffer* b ) {
	kdDebug( 60005 ) << k_funcinfo << b << endl;
	emit sDeleteBuffer( b );
	delete b;
	if ( _buffers.remove( b ) )
		_currentBuffer = -1;
	KRecGlobal::the()->message( i18n( "Part deleted." ) );
	_saved = false;
}

/// * * * helper * * *
KRecBuffer* KRecFile::getTopBuffer_buffer( int pos ) {
	//kdDebug( 60005 ) << k_funcinfo << pos << endl;
	QValueList<KRecBuffer*>::iterator it = _buffers.begin();
	KRecBuffer* out = 0;
	while ( it != _buffers.end() ) {
		if ( ( *it )->startpos() <= pos && offsetToSamples( ( *it )->size() ) + ( *it )->startpos() > pos && ( *it )->active() )
			out = ( *it );
		++it;
	}
	return out;
}
int KRecFile::getTopBuffer_int( int pos ) {
	return _buffers.findIndex( getTopBuffer_buffer( pos ) );
}


/// * * * KRecBuffer * * *
KRecBuffer::KRecBuffer( const QString &filename, int startpos, bool a, KRecFile* p, const char* n )
  : QObject( p,n )
  , _krecfile( p )
  , _file( new QFile( filename ) )
  , _stream( new QDataStream( _file ) )
  , _fileinfo( new QFileInfo( filename ) )
  , _active( a )
  , _pos( 0 ), _start( startpos )
  , _title( _fileinfo->fileName() )
  , _comment( QString::null )
{
	kdDebug( 60005 ) << k_funcinfo << filename << " " << startpos << endl;
	_open = _file->open( IO_Raw | IO_ReadWrite );
	setPos( _file->at() );
	if ( _open ) kdDebug( 60005 ) << k_funcinfo << "Open successfull!" << endl;
		else kdDebug( 60005 ) << endl << k_funcinfo << "Could not open file!" << endl << endl;
}
KRecBuffer::~KRecBuffer() {
	kdDebug( 60005 ) << k_funcinfo << endl;
	if ( _open ) {
		_file->close();
		_open = false;
		_file->remove();
	}
	//kdDebug( 60005 ) << k_funcinfo << "done." << endl;
}

void KRecBuffer::writeConfig( KConfig* config ) {
	//kdDebug( 60005 ) << k_funcinfo << config << endl;
	config->writeEntry( "Filename", _fileinfo->fileName() );
	config->writeEntry( "StartPos", _start );
	config->writeEntry( "Activated", _active );
	config->writeEntry( "Title", _title );
	config->writeEntry( "Comment", _comment );
}

KRecBuffer* KRecBuffer::fromConfig( KConfig* config, QDir* dir, KRecFile* p, const char* n ) {
	kdDebug( 60005 ) << k_funcinfo << config << endl;
	KRecBuffer* tmp = new KRecBuffer( dir->path() + "/" + config->readEntry( "Filename" ),
	                       config->readNumEntry( "StartPos" ),
	                       config->readBoolEntry( "Activated", true ),
	                       p, n );
	tmp->setTitle( config->readEntry( "Title", tmp->filename() ) );
	tmp->setComment( config->readEntry( "Comment", QString::null ) );
	return tmp;
}

void KRecBuffer::writeData( Arts::mcopbyte* /*data*/, uint /*length*/ ) {
	kdDebug( 60005 ) << k_funcinfo << endl;
}
void KRecBuffer::writeData( QByteArray* data ) {
kdDebug( 60005 ) << k_funcinfo << endl;
	writeData( *data );
/*	if ( _open ) {
		_file->at( _pos );
		_file->writeBlock( *data );
		setPos( _file->at() );
		emit sizeChanged( this, size() );
	}*/
}
void KRecBuffer::writeData( QByteArray& data ) {
	//kdDebug( 60005 ) << k_funcinfo << endl;
	if ( _open ) {
		_file->at( _pos );
		_file->writeBlock( data );
		setPos( _file->at() );
		emit sizeChanged( this, size() );
	}
}

void KRecBuffer::getData( QByteArray& data ) {
	//kdDebug( 60005 ) << k_funcinfo << "data.size()" << data.size() << " _pos" << _pos << endl;
	if ( _pos > _file->size() )
		kdWarning() << "Trying to access behind file!" << endl;
	else {
		if ( _open ) {
			_file->at( _pos );
			//kdDebug( 60005 ) << "Reading " << _file->readBlock( ( char* )data.data(), data.size() ) << "bytes into data." << endl;
			//kdDebug( 60005 ) << "data.size()" << data.size() << endl;
			for ( uint i=0; i<data.size(); i++ ) {
				if ( !_file->atEnd() )
					data.data()[ i ] = _file->getch();
				else
					data.data()[ i ] = 0;
			}
			//setPos( _file->at() );
		}
	}
}

void KRecBuffer::setPos( QIODevice::Offset p ) {
	if ( p!=_pos ) {
		_pos = p;
		emit posChanged( this, _pos );
//kdDebug( 60005 ) << k_funcinfo << _pos << endl;
	}
}

int KRecBuffer::startpos() const { return _start; }
QIODevice::Offset KRecBuffer::size() const { return _file->size(); }

QString KRecBuffer::filename() const { return _fileinfo->fileName(); }
QString KRecBuffer::title() const { return _title; }
QString KRecBuffer::comment() const { return _comment; }

void KRecBuffer::setTitle( const QString &n ) {
	if ( _title != n ) {
		_title = n;
		emit somethingChanged();
	}
}

void KRecBuffer::setComment( const QString &n ) {
	if ( _comment != n ) {
		_comment = n;
		emit somethingChanged();
	}
}

bool KRecBuffer::active() const { return _active; }
void KRecBuffer::setActive( bool n ) {
	if ( _active != n ) {
		_active = n;
		emit activeChanged( _active );
		emit somethingChanged();
	}
}

void KRecBuffer::deleteBuffer() {
	if ( KMessageBox::warningContinueCancel( KRecGlobal::the()->mainWidget(), i18n( "Do you really want to delete the selected part '%1'?" ).arg( filename() ), i18n("Delete Part?"), KStdGuiItem::del() ) == KMessageBox::Continue )
		_krecfile->deleteBuffer( this );
}

float KRecBuffer::getSample( int pos, int /*channel*/ ) {
	Q_INT16 tmp16;
	Q_INT8  tmp8;
	float out;
	_file->at( _krecfile->samplesToOffset( pos ) );
	if ( _krecfile->bits() == 16 ) {
		*_stream >> tmp16;
		out = tmp16 / 65535.0;
	}
	else {
		*_stream >> tmp8;
		out = tmp8 / 65535.0;
	}
	return out;
}
float* KRecBuffer::getsamples( int start, int end, int channel ) {
	float* tmp = new float[ end-start ];
	for ( int i=start; i<end; ++i )
		tmp[ i ] = getSample( i, channel );
	return tmp;
}

