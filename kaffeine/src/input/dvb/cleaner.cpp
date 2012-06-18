/*
 * cleaner.cpp
 *
 * Copyright (C) 2005-2007 Christophe Thommeret <hftom@free.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <sys/statvfs.h>

#include <qdir.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qfileinfo.h>

#include "cleaner.h"

Cleaner::Cleaner( const QString &lpath, const QString &rpath )
{
	setPaths( lpath, rpath );

	connect( &timer, SIGNAL(timeout()), this, SLOT(doClean()) );
	timer.start( 60*1000 );
}

Cleaner::~Cleaner()
{
	wait();
}

void Cleaner::doClean()
{
	start(IdlePriority);
}

void Cleaner::setPaths( const QString &lpath, const QString &rpath )
{
	livePath = lpath;
	recordPath = rpath;
}

void Cleaner::run()
{
	QStringList list;
	QDir d;
	double freemb;
	struct statvfs buf;

	d.setPath( livePath );
	list = d.entryList( "DVBLive-*", QDir::Files, QDir::Name );
	if ( list.count()>1 ) d.remove( list[0] );


	if ( statvfs( recordPath.local8Bit(), &buf ) ) {
		fprintf(stderr,"Couldn't get file system statistics\n");
		return;
	}

	freemb = (double)(((double)(buf.f_bavail)*(double)(buf.f_bsize))/(1024.0*1024.0));
	if ( freemb<1000 ) {
		d.setPath( recordPath );
		const QFileInfoList *infos = d.entryInfoList( "*.m2t", QDir::Files|QDir::NoSymLinks|QDir::Writable, QDir::Time|QDir::Reversed );
		QFileInfoListIterator it( *infos );
		QFileInfo *i;
		/*if ( ( i=it.current() )!=0 )
			d.remove( i->absFilePath() );*/
	}
}

#include "cleaner.moc"
