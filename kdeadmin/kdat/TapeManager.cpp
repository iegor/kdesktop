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

#include <stdlib.h>
#include <unistd.h>

#include <qdir.h>
#include <qregexp.h>
#include <qfile.h>

#include <kglobal.h>
#include <kstandarddirs.h>

#include "TapeManager.h"

#include "TapeManager.moc"

TapeManager::TapeManager()
        : _mountedTape( 0 )
{
    _tapes.setAutoDelete( TRUE );

    // Get a list of all available tape indexes.
    QStringList relList;

    // Fix 2002-01-24 c/o RG. The problem was: "Tape Index is empty, but I did 
    // just a few minutes ago a 1st backup onto the the tape."
    // (void) KGlobal::dirs()->findAllResources( "appdata", ".*:[0-9]+", false, true, relList);
    (void) KGlobal::dirs()->findAllResources( "appdata", "*:[0-9]*", false, true, relList);
    
    for(QStringList::Iterator it = relList.begin();
        it != relList.end();
        it++)
    {
        QString fn = *it;
        // Convert to outdated QStringList :-)
        _tapeIDs.append( QFile::encodeName(fn) );
    }
}

TapeManager::~TapeManager()
{
}

TapeManager* TapeManager::_instance = 0;

TapeManager* TapeManager::instance()
{
    if ( _instance == 0 ) {
        _instance = new TapeManager();
    }

    return _instance;
}

const QStringList& TapeManager::getTapeIDs()
{
    return _tapeIDs;
}

Tape* TapeManager::findTape( const QString & id )
{
  /* 2002-01-26 LEW */
  //  printf("Contents of _tapeIDs: %d entries\n", _tapes.count());  
  //  for ( QStringList::Iterator it = _tapeIDs.begin(); it != _tapeIDs.end(); ++it ) {
  //    printf("%s\n", (*it).latin1());
  //  }
  //  printf("\n");

  //  QDictIterator<Tape> it( _tapes );
  //  printf("Contents of _tapes: %d entries\n", _tapes.count());
  //  for( ; it.current(); ++it )
  //    printf("index ?: %s (%d)\n", it.current()->getName().latin1(), 
  //                            it.current()->getCTime());
  //  printf("\n");
  /* 2002-01-26 LEW */

    Tape* tape = _tapes[ id ];

    if ( !tape ) {
        tape = new Tape( id.ascii() );
        _tapes.insert( tape->getID(), tape );
    }

    return tape;
}

void TapeManager::addTape( Tape* tape )
{
    Tape* old = _tapes[ tape->getID() ];
    if ( old ) {
        removeTape( old );
    }

    _tapeIDs.append( tape->getID() );
    _tapes.insert( tape->getID(), tape );

    emit sigTapeAdded( tape );
}

void TapeManager::removeTape( Tape* tape )
{
    emit sigTapeRemoved( tape );

    // Remove the index file.
    QString filename = locateLocal( "appdata", tape->getID() );
    
    unlink( QFile::encodeName(filename) );

    _tapeIDs.remove( tape->getID() );
    _tapes.remove( tape->getID() );
}

void TapeManager::tapeModified( Tape* tape )
{
    emit sigTapeModified( tape );
}

void TapeManager::mountTape( Tape* tape )
{
    _mountedTape = tape;
    emit sigTapeMounted();
}

void TapeManager::unmountTape()
{
    emit sigTapeUnmounted();
    _mountedTape = 0;
}

Tape* TapeManager::getMountedTape()
{
    return _mountedTape;
}


