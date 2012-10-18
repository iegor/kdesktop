/*
 * cdcleaner.cpp
 *
 * Copyright (C) 2005 Christophe Thommeret <hftom@free.fr>
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

#include <qdir.h>
#include <qstringlist.h>

#include "cdcleaner.h"



CdCleaner::CdCleaner( const QString &path )
{
	livePath = path;

	connect( &timer, SIGNAL(timeout()), this, SLOT(doClean()) );
	timer.start( 60*1000 );
}



CdCleaner::~CdCleaner()
{
	wait();
}



void CdCleaner::doClean()
{
	start( QThread::LowestPriority );
}



void CdCleaner::setPath( const QString &path )
{
	livePath = path;
}



void CdCleaner::run()
{
	QStringList list;
	QDir d;

	d.setPath( livePath );
	list = d.entryList( "DVBClient-*.ts", QDir::Files, QDir::Name );
	if ( list.count()>1 ) d.remove( list[0] );
}

#include "cdcleaner.moc"
