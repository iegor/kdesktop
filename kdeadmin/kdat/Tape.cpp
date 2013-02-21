// KDat - a tar-based DAT archiver
// Copyright (C) 1998-2000  Sean Vyain, svyain@mail.tds.net
// Copyright (C) 2001-2002  Lawrence Widman, kdat@cardiothink.com
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#include <qfile.h>

#include <kmessagebox.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kapplication.h>

#include "IndexDlg.h"
#include "KDatMainWindow.h"
#include "Options.h"
#include "Tape.h"
#include "TapeDrive.h"
#include "TapeManager.h"
#include "kdat.h"

Tape::Tape()
        : _stubbed( FALSE ),
          _name( i18n( "New Tape" ) ),
          _size( Options::instance()->getDefaultTapeSize() ),
          _fptr( 0 )
{
    char buffer[1024];
    gethostname( buffer, 1024 );
    time_t tm = time( NULL );
    _ctime = tm;
    _mtime = tm;
    _id.sprintf("%s:%d", buffer, tm);

    write();
}

Tape::Tape( const char * id )
        : _stubbed( TRUE ),
          _id( id ),
          _ctime( -1 ),
          _mtime( -1 ),
          _name( "<unknown>" ),
          _size( -1 ),
          _fptr( 0 )
{
}

Tape::~Tape()
{
    clear();
}

void Tape::format()
{
    // Rewind tape.
    if ( !TapeDrive::instance()->rewind() ) {
        KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Rewinding tape failed." ), i18n("Format Failed"));
        return;
    }

    // Set block size for tape.
    if ( !TapeDrive::instance()->setBlockSize( Options::instance()->getTapeBlockSize() ) ) {
        KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Cannot set tape block size." ), i18n("Format Failed"));
        return;
    }

    // TAPE HEADER
    int iv;

    // KDat magic string.
    if ( TapeDrive::instance()->write( KDAT_MAGIC, KDAT_MAGIC_LENGTH ) < KDAT_MAGIC_LENGTH ) {
        KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Writing magic string failed." ), i18n("Format Failed"));
        return;
    }

    // Tape header version number.
    iv = KDAT_TAPE_HEADER_VERSION;
    if ( TapeDrive::instance()->write( (const char*)&iv, 4 ) < 4 ) {
        KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Writing version number failed." ), i18n("Format Failed") );
        return;
    }

    // Write tape ID.  Tape ID is machine name + current time.
    iv = _id.length() + 1;
    if ( TapeDrive::instance()->write( (const char*)&iv, 4 ) < 4 ) {
        KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Writing tape ID length failed." ), i18n("Format Failed") );
        return;
    }
    if ( TapeDrive::instance()->write( _id, iv ) < iv ) {
        KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Writing tape ID failed." ), i18n("Format Failed") );
        return;
    }

    // Write end of file marker.
    TapeDrive::instance()->close();
    TapeDrive::instance()->open();

    // Write new tape index.
    write();
}

void Tape::read()
{
    if ( !_stubbed ) {
      /* 2002-01-28 LEW */
      // printf("Can't read tape data because tape isn't stubbed\n" );
      /* 2002-01-28 LEW */
      return;
    }

    _stubbed = FALSE;

    /* 2002-01-28 LEW */
    // printf("Preparing to read tape data\n" );
    // QString filename1 = locateLocal( "appdata", _id);
    // printf("The tape data are in \"%s\"\n", filename1.ascii() );
    /* 2002-01-28 LEW */

    if ( !_fptr ) {
        QString filename = locateLocal( "appdata", _id);
        _fptr = fopen( QFile::encodeName(filename), "r" );

	/* 2002-01-28 LEW */
#ifdef DEBUG
	//	printf("Opened tape archive file \"%s\". %s %d\n", 
	//	       filename.ascii(), __FILE__, __LINE__ );
#endif /* DEBUG */
	/* 2002-01-28 LEW */

    }

    if ( !_fptr ) {
        // No tape index file was found.
        int result = KMessageBox::warningContinueCancel( KDatMainWindow::getInstance(), 
                 i18n( "No index file was found for this tape.\n"
                       "Recreate the index from tape?" ), 
                 i18n("Tape Index"), 
                 i18n("Recreate"));
        if (result == KMessageBox::Continue ) {
            write();

            IndexDlg dlg( this, KDatMainWindow::getInstance() );
            dlg.exec();
            TapeManager::instance()->addTape( this );
            return;
        } else {
            return;
        }
    }

    char buf[4096];

    fseek( _fptr, 0, SEEK_SET );

    // Read index file version number.
    if ( !fgets( buf, 4096, _fptr ) ) {
        fclose( _fptr );
        KMessageBox::error( KDatMainWindow::getInstance(), 
                              i18n( "Reading version number failed." ), 
                              i18n("Index File Error") );
        return;
    }
    int version = atoi( buf );

    switch ( version ) {
        case 1:
            readVersion1Index( _fptr );
            readAll( version );
            calcRanges();
            fclose( _fptr );
            _fptr = NULL;
            write();
            break;

        case 2:
            readVersion2Index( _fptr );
            readAll( version );
            calcRanges();
            fclose( _fptr );
            _fptr = NULL;
            write();
            break;

        case 3:
            readVersion3Index( _fptr );
            readAll( version );
            calcRanges();
            fclose( _fptr );
            _fptr = NULL;
            write();
            break;

        case 4:
            readVersion4Index( _fptr );
            break;

        default:
        {
            KMessageBox::sorry( KDatMainWindow::getInstance(), 
                i18n( "The tape index file format is version %d. The index cannot be read by this version of KDat. Perhaps the tape index file was created by a newer version of KDat?" ).arg(version ),
                i18n("Tape Index") );
        }
    }
}

void Tape::readAll( int version )
{
    read();

    QPtrListIterator<Archive> i( _children );
    for ( ; i.current(); ++i ) {
        i.current()->readAll( version );
    }
}

void Tape::write()
{
    QString filename = locateLocal( "appdata", _id);

    /* 2002-01-28 LEW */
#ifdef DEBUG
    //    printf("Preparing to write the archive data to \"%s\". %s %d\n", 
    //            filename.ascii(), __FILE__, __LINE__ );
#endif /* DEBUG */
    /* 2002-01-28 LEW */

    if ( !_fptr ) {
        _fptr = fopen( QFile::encodeName(filename), "w" );

    /* 2002-01-31 LEW */
#ifdef DEBUG
	//  printf("Opened new archive file \"%s\". %s %d\n", 
	//  filename.ascii(), __FILE__, __LINE__ );
#endif /* DEBUG */
    /* 2002-01-31 LEW */

        if ( !_fptr ) {
            // Suck!
            printf( "Tape::write() -- cannot open '%s' for writing!\n", filename.ascii() );
            return;
        }
    } else {
        freopen( QFile::encodeName(filename), "w", _fptr );
    /* 2002-01-31 LEW */
#ifdef DEBUG
	//	printf("Reopened new archive file \"%s\". %s %d\n", 
	//	       filename.ascii(), __FILE__, __LINE__ );
#endif /* DEBUG */
    /* 2002-01-31 LEW */
    }

    int zero = 0;

    //===== Write the tape data =====

    fprintf( _fptr, "%d\n", KDAT_INDEX_FILE_VERSION );
    fprintf( _fptr, "%s\n", _id.data() );

    fwrite( &_ctime, sizeof( _ctime ), 1, _fptr );
    fwrite( &_mtime, sizeof( _mtime ), 1, _fptr );
    fwrite( &_size , sizeof( _size  ), 1, _fptr );
    char buf[4096];
    memset( buf, 0, 4096 );
    memcpy( buf, _name.ascii(), _name.length() > 4095 ? 4095 : _name.length() );
    fwrite( buf, sizeof( char ), 4096, _fptr );
    int ival = _children.count();
    fwrite( &ival, sizeof( ival ), 1, _fptr );

    // Fill in the archive offsets later...
    int archiveTable = ftell( _fptr );
    for ( uint i = 0; i < MAX_NUM_ARCHIVES; i++ ) {
        fwrite( &zero, sizeof( zero ), 1, _fptr );
    }

    //===== Write archives =====
    QPtrListIterator<Archive> i( _children );
    int count = 0;
    for ( ; i.current(); ++i, count++ ) {
        // Fill in the file offset.
        int here = ftell( _fptr );
        fseek( _fptr, archiveTable + 4*count, SEEK_SET );
        fwrite( &here, sizeof( here ), 1, _fptr );
        fseek( _fptr, here, SEEK_SET );

        i.current()->write( _fptr );
    }

    freopen( QFile::encodeName(filename), "r+", _fptr );
}

QString Tape::getID()
{
    read();

    return _id;
}

QString Tape::getName()
{
    read();

    return _name;
}

int Tape::getCTime()
{
    read();

    return _ctime;
}

int Tape::getMTime()
{
    read();

    return _mtime;
}

int Tape::getSize()
{
    read();

    return _size;
}

const QPtrList<Archive>& Tape::getChildren()
{
    read();

    return _children;
}

void Tape::setName( const QString & name )
{
    /* 2002-01-31 LEW */
    int i;
    /* 2002-01-31 LEW */

    read();

    if ( !_fptr ) {
        // Create a new empty index file.
        write();
    }

    // change file to read-write so we can update it   2002-01-31 LEW
    QString filename = locateLocal( "appdata", _id);
    freopen( QFile::encodeName(filename), "r+", _fptr );

    _name = name;

    char buf[4096];
    if( fseek( _fptr, 0, SEEK_SET ) < 0 )
      {
	clearerr( _fptr );
#ifdef DEBUG    /* Can't create new i18n strings now, so we'll save this for KDE 3.1 */
       	        /* Note to translator:  this error checking was added while tracking
                   down a bug that turns out to be unrelated.  The errors I'm looking
                   for here haven't happened in real use, so there is no rush to add
                   these checks in KDE 3.0.  Thanks. - LW */
	QString msg = i18n("Error during fseek #1 while accessing archive: \"");
        msg.append( getID() );
        msg.append( "\": " );
	msg.append( i18n(strerror( errno )) );
	printf("%s\n", msg.latin1());      
	KMessageBox::error(NULL, msg, i18n("File Access Error"));
#endif /* DEBUG */
	return;      
      }
    if( fgets( buf, 4096, _fptr ) == NULL )
      {
	clearerr( _fptr );
#ifdef DEBUG    /* Can't create new i18n strings now, so we'll save this for KDE 3.1 */
	QString msg = i18n("Error while accessing string #1 in archive: \"");
        msg.append( getID() );
        msg.append( "\": " );
	msg.append( i18n(strerror( errno )) );
	printf("%s\n", msg.latin1());      
	KMessageBox::error(NULL, msg, i18n("File Access Error"));
#endif /* DEBUG */
	return;      
      }
    if( fgets( buf, 4096, _fptr ) == NULL )
      {
	clearerr( _fptr );
#ifdef DEBUG    /* Can't create new i18n strings now, so we'll save this for KDE 3.1 */
	QString msg = i18n("Error while accessing string #2 in archive: \"");
        msg.append( getID() );
        msg.append( "\": " );
	msg.append( i18n(strerror( errno )) );
	printf("%s\n", msg.latin1());      
	KMessageBox::error(NULL, msg, i18n("File Access Error"));
#endif /* DEBUG */
	return;      
      }
    if( fseek( _fptr, 12, SEEK_CUR ) < 0 )
      {
	clearerr( _fptr );
#ifdef DEBUG    /* Can't create new i18n strings now, so we'll save this for KDE 3.1 */
	QString msg = i18n("Error during fseek #2 while accessing archive: \"");
        msg.append( getID() );
        msg.append( "\": " );
	msg.append( i18n(strerror( errno )) );
	printf("%s\n", msg.latin1());      
	KMessageBox::error(NULL, msg, i18n("File Access Error"));
#endif /* DEBUG */
	return;      
      }
    memset( buf, 0, 4096 );
    memcpy( buf, _name.ascii(), _name.length() > 4095 ? 4095 : _name.length() );
    i = fwrite( buf, sizeof( char ), 4096, _fptr );
    /* 2002-01-31 LEW */
    /* Note to translator: I know it's past the deadline, so don't translate this.  I think
       I fixed the bug that caused this error to occur.  Thanks - LW */
    if( ( i < 4096 ) || ( ferror( _fptr ) != 0 )){
      clearerr( _fptr );
      QString msg = i18n("Error while updating archive name: ");
      msg.append( i18n(strerror( errno )) );
      printf("%s\n", msg.latin1());      
      KMessageBox::error(NULL, msg, i18n("File Access Error"));
      // return;      
    /* 2002-01-31 LEW */
    }
    fflush( _fptr );

    /* 2002-01-31 LEW */
#ifdef DEBUG
    //    printf("Tape::setName. wrote \"%s\" (%d bytes) into archive ID %s. %s %d\n", 
    //	   _name.ascii(), _name.length(), getID().latin1(), __FILE__, __LINE__);
    //    printf("  The buffer size is 4096.  %d bytes were written\n", i);
#endif /* DEBUG */
    /* 2002-01-31 LEW */

    // change back to read-only   2002-01-31 LEW
    freopen( QFile::encodeName(filename), "r", _fptr );

    TapeManager::instance()->tapeModified( this );
}

void Tape::setMTime( int mtime )
{
    read();

    if ( !_fptr ) {
        // Create a new empty index file.
        write();
    }

    // change file to read-write so we can update it   2002-01-31 LEW
    QString filename = locateLocal( "appdata", _id);
    freopen( QFile::encodeName(filename), "r+", _fptr );

    _mtime = mtime;

    char buf[4096];
    fseek( _fptr, 0, SEEK_SET );
    fgets( buf, 4096, _fptr );
    fgets( buf, 4096, _fptr );
    fseek( _fptr, 4, SEEK_CUR );
    fwrite( &_mtime, sizeof( _mtime ), 1, _fptr );
    fflush( _fptr );

    // change back to read-only   2002-01-31 LEW
    freopen( QFile::encodeName(filename), "r", _fptr );

    TapeManager::instance()->tapeModified( this );
}

void Tape::setSize( int size )
{
    read();

    if ( !_fptr ) {
        // Create a new empty index file.
        write();
    }

    _size = size;

    // change file to read-write so we can update it   2002-01-31 LEW
    QString filename = locateLocal( "appdata", _id);
    freopen( QFile::encodeName(filename), "r+", _fptr );

    char buf[4096];
    fseek( _fptr, 0, SEEK_SET );
    fgets( buf, 4096, _fptr );
    fgets( buf, 4096, _fptr );
    fseek( _fptr, 8, SEEK_CUR );
    fwrite( &_size, sizeof( _size ), 1, _fptr );
    fflush( _fptr );

    // change back to read-only   2002-01-31 LEW
    freopen( QFile::encodeName(filename), "r", _fptr );

    TapeManager::instance()->tapeModified( this );
}

void Tape::addChild( Archive* archive )
{
    /* 2002-01-28 LEW */
    // printf("Preparing to add archive to tape file\n" );
    /* 2002-01-28 LEW */

    read();

    // change file to read-write so we can update it   2002-01-31 LEW
    QString filename = locateLocal( "appdata", _id);
    freopen( QFile::encodeName(filename), "r+", _fptr );

    archive->calcRanges();

    _children.append( archive );

    char buf[4096];
    fseek( _fptr, 0, SEEK_END );
    int here = ftell( _fptr );
    fseek( _fptr, 0, SEEK_SET );
    fgets( buf, 4096, _fptr );
    fgets( buf, 4096, _fptr );
    fseek( _fptr, 12 + 4096, SEEK_CUR );
    int ival = _children.count();
    fwrite( &ival, sizeof( ival ), 1, _fptr );
    fseek( _fptr, ( _children.count() - 1 ) * 4, SEEK_CUR );
    fwrite( &here, sizeof( here ), 1, _fptr );
    fseek( _fptr, here, SEEK_SET );
    archive->write( _fptr );
    fflush( _fptr );

    setMTime( time( NULL ) );

    // change back to read-only   2002-01-31 LEW
    freopen( QFile::encodeName(filename), "r", _fptr );

    /* 2002-01-28 LEW */
    // printf("Done adding archive to tape file\n" );
    /* 2002-01-28 LEW */
}

void Tape::removeChild( Archive* archive )
{
    read();

    while ( _children.last() != archive ) {
        _children.removeLast();
    }
    _children.removeLast();

    char buf[4096];
    fseek( _fptr, 0, SEEK_SET );
    fgets( buf, 4096, _fptr );
    fgets( buf, 4096, _fptr );
    fseek( _fptr, 12 + 4096, SEEK_CUR );
    int ival = _children.count();
    fwrite( &ival, sizeof( ival ), 1, _fptr );
    int here;
    if ( ival > 0 ) {
        fseek( _fptr, _children.count() * 4, SEEK_CUR );
        fread( &here, sizeof( here ), 1, _fptr );
    } else {
        fseek( _fptr, MAX_NUM_ARCHIVES * 4, SEEK_CUR );
        here = ftell( _fptr );
    }

    QString filename = locateLocal( "appdata", _id);
    truncate( QFile::encodeName(filename), here );
    freopen( QFile::encodeName(filename), "r+", _fptr );
    fflush( _fptr );

    // change back to read-only   2002-01-31 LEW
    freopen( QFile::encodeName(filename), "r", _fptr );

    setMTime( time( NULL ) );
}

void Tape::clear()
{
    if ( _children.count() < 1 ) {
        return;
    }

    while ( _children.first() ) {
        delete _children.first();
        _children.removeFirst();
    }

    char buf[4096];
    fseek( _fptr, 0, SEEK_SET );
    fgets( buf, 4096, _fptr );
    fgets( buf, 4096, _fptr );
    fseek( _fptr, 12 + 4096, SEEK_CUR );
    int ival = _children.count();
    fwrite( &ival, sizeof( ival ), 1, _fptr );
    fseek( _fptr, MAX_NUM_ARCHIVES * 4, SEEK_CUR );
    int here = ftell( _fptr );

    QString filename = locateLocal( "appdata", _id);
    truncate( QFile::encodeName(filename), here );
    freopen( QFile::encodeName(filename), "r+", _fptr );
    fflush( _fptr );

    // change back to read-only   2002-01-31 LEW
    freopen( QFile::encodeName(filename), "r", _fptr );

    setMTime( time( NULL ) );
}

void Tape::readVersion1Index( FILE* fptr )
{
    fseek( fptr, 0, SEEK_CUR );

    char buf[4096];

    // Read the tapeID.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Reading tape ID failed." ), i18n("Index File Error") );
        return;
    }
    QCString tapeID = buf;
    tapeID.truncate( tapeID.length() - 1 );
    if ( tapeID !=_id ) {
        KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Tape ID on tape does not match tape ID in index file." ), i18n("Index File Error") );
        return;
    }

    // Read creation time.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Reading creation time failed." ), i18n("Index File Error") );
        return;
    }
    _ctime = atoi( buf );

    // Read modification time.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Reading modification time failed." ), i18n("Index File Error") );
        return;
    }
    _mtime = atoi( buf );

    // Read tape name.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Reading tape name failed." ), i18n("Index File Error") );
        return;
    }
    _name = buf;
    _name.truncate( _name.length() - 1 );

    // Read tape size.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Reading tape size failed." ), i18n("Index File Error") );
        return;
    }
    _size = atoi( buf );

    // Read number of archives.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Reading archive count failed." ), i18n("Index File Error") );
        return;
    }
    int numArchives = atoi( buf );

    for ( ; numArchives; numArchives-- ) {
        // Read archive name.
        if ( !fgets( buf, 4096, fptr ) ) {
            KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Reading archive name failed." ), i18n("Index File Error") );
            return;
        }
        QString archiveName = buf;
        archiveName.truncate( archiveName.length() - 1 );

        // Read archive time stamp.
        if ( !fgets( buf, 4096, fptr ) ) {
            KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Reading archive time stamp failed." ), i18n("Index File Error") );
            return;
        }
        int archiveTimeStamp = atoi( buf );

        // Read archive start block.
        if ( !fgets( buf, 4096, fptr ) ) {
            KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Reading archive start block failed." ), i18n("Index File Error") );
            return;
        }
        int archiveStartBlock = atoi( buf );

        // Read archive end block.
        if ( !fgets( buf, 4096, fptr ) ) {
            KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Reading archive end block failed." ), i18n("Index File Error") );
            return;
        }
        int archiveEndBlock = atoi( buf );

        // Create the archive.
        Archive* archive = new Archive( this, archiveTimeStamp, archiveName );
        archive->setEndBlock( archiveEndBlock - archiveStartBlock );

        _children.append( archive );

        // Read the number of files.
        if ( !fgets( buf, 4096, fptr ) ) {
            KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Reading archive file count failed." ), i18n("Index File Error") );
            return;
        }
        int archiveNumFiles = atoi( buf );

        QString tmpFileName;
        int tmpFileSize = -1;
        int tmpFileMTime = -1;
        int tmpFileStartRecord = -1;
        for ( ; archiveNumFiles; archiveNumFiles-- ) {
            // Read file name.
            if ( !fgets( buf, 4096, fptr ) ) {
                KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Reading file name failed." ), i18n("Index File Error") );
                return;
            }
            QString fileName = buf;
            fileName.truncate( fileName.length() - 1 );

            // Read file size.
            if ( !fgets( buf, 4096, fptr ) ) {
                KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Reading file size failed." ), i18n("Index File Error") );
                return;
            }
            int fileSize = atoi( buf );

            // Read file modification time.
            if ( !fgets( buf, 4096, fptr ) ) {
                KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Reading file modification time failed." ), i18n("Index File Error") );
                return;
            }
            int fileModTime = atoi( buf );

            // Read file record number.
            if ( !fgets( buf, 4096, fptr ) ) {
                KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Reading file record number failed." ), i18n("Index File Error") );
                return;
            }
            int fileRecord = atoi( buf );

            if ( tmpFileName.length() > 0 ) {
                archive->addFile( tmpFileSize, tmpFileMTime, tmpFileStartRecord, fileRecord, tmpFileName );
            }

            tmpFileName = fileName.copy();
            tmpFileSize = fileSize;
            tmpFileMTime = fileModTime;
            tmpFileStartRecord = fileRecord;
        }

        if ( tmpFileName.length() > 0 ) {
            archive->addFile( tmpFileSize, tmpFileMTime, tmpFileStartRecord, archive->getEndBlock() * ( Options::instance()->getTapeBlockSize() / 512 ), tmpFileName );
        }
    }
}

void Tape::readVersion2Index( FILE* fptr )
{
    fseek( fptr, 0, SEEK_CUR );

    char buf[4096];

    // Read the tapeID.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Reading tape ID failed." ), i18n("Index File Error") );
        return;
    }
    QCString tapeID = buf;
    tapeID.truncate( tapeID.length() - 1 );
    if ( tapeID != _id ) {
        KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Tape ID on tape does not match tape ID in index file." ), i18n("Index File Error") );
        return;
    }

    // Read creation time.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Reading creation time failed." ), i18n("Index File Error") );
        return;
    }
    _ctime = atoi( buf );

    // Read modification time.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Reading modification time failed." ), i18n("Index File Error") );
        return;
    }
    _mtime = atoi( buf );

    // Read tape name.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Reading tape name failed." ), i18n("Index File Error") );
        return;
    }
    _name = buf;
    _name.truncate( _name.length() - 1 );

    // Read tape size.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Reading tape size failed." ), i18n("Index File Error") );
        return;
    }
    _size = atoi( buf );

    // Read number of archives.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Reading archive count failed." ), i18n("Index File Error") );
        return;
    }
    int numArchives = atoi( buf );

    int ival;
    for ( ; numArchives; numArchives-- ) {
        fread( &ival, sizeof( ival ), 1, fptr );
        _children.append( new Archive( this, fptr, ival ) );
    }
}

void Tape::readVersion3Index( FILE* fptr )
{
    fseek( fptr, 0, SEEK_CUR );

    char buf[4097];
    buf[4096] = '\0';

    // Read the tapeID.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Reading tape ID failed." ), i18n("Index File Error") );
        return;
    }
    QCString tapeID = buf;
    tapeID.truncate( tapeID.length() - 1 );
    if ( tapeID != _id ) {
        KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Tape ID on tape does not match tape ID in index file." ), i18n("Index File Error") );
        return;
    }

    // Read creation time.
    fread( &_ctime, sizeof( _ctime ), 1, fptr );

    // Read modification time.
    fread( &_mtime, sizeof( _mtime ), 1, fptr );

    // Read tape size.
    fread( &_size, sizeof( _size ), 1, fptr );

    // Read tape name.
    fread( buf, sizeof( char ), 4096, fptr );
    _name = buf;

    // Read number of archives.
    int numArchives;
    fread( &numArchives, sizeof( numArchives ), 1, fptr );

    int ival;
    for ( ; numArchives; numArchives-- ) {
        fread( &ival, sizeof( ival ), 1, fptr );
        _children.append( new Archive( this, fptr, ival ) );
    }
}

void Tape::readVersion4Index( FILE* fptr )
{
    fseek( fptr, 0, SEEK_CUR );

    char buf[4097];
    buf[4096] = '\0';

    // Read the tapeID.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Reading tape ID failed." ), i18n("Index File Error") );
        return;
    }
    QCString tapeID = buf;
    tapeID.truncate( tapeID.length() - 1 );
    if ( tapeID != _id ) {
        KMessageBox::error( KDatMainWindow::getInstance(), i18n( "Tape ID on tape does not match tape ID in index file." ), i18n("Index File Error") );
        return;
    }

    // Read creation time.
    fread( &_ctime, sizeof( _ctime ), 1, fptr );

    // Read modification time.
    fread( &_mtime, sizeof( _mtime ), 1, fptr );

    // Read tape size.
    fread( &_size, sizeof( _size ), 1, fptr );

    // Read tape name.
    fread( buf, sizeof( char ), 4096, fptr );
    _name = buf;

    // Read number of archives.
    int numArchives;
    fread( &numArchives, sizeof( numArchives ), 1, fptr );

    int ival;
    for ( ; numArchives; numArchives-- ) {
        fread( &ival, sizeof( ival ), 1, fptr );
        _children.append( new Archive( this, fptr, ival ) );
    }

    /* 2002-01-31 LEW */
#ifdef DEBUG
    printf("Read contents of archive file \"%s\". %s %d\n", 
	   _name.latin1(), __FILE__, __LINE__ );
#endif /* DEBUG */
    /* 2002-01-31 LEW */
}

void Tape::calcRanges()
{
    QPtrListIterator<Archive> it( getChildren() );
    for ( ; it.current(); ++it ) {
        it.current()->calcRanges();
    }
}
