/*
 * cdlisten.cpp
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

#include <unistd.h>

#include <kmessagebox.h>
#include <klocale.h>

#include "cdlisten.h"



CdListen::CdListen()
{
	connect( &timer, SIGNAL(timeout()), this, SLOT(updateList()) );
}



CdListen::~CdListen()
{
	stop();
}



void CdListen::updateList()
{
	if ( newList==currentList ) return;

	currentList = newList;
	emit listChanged( currentList );
}



void CdListen::stop()
{
	isRunning = false;
	timer.stop();
	if ( !wait(100) ) {
		terminate();
		wait();
	}
	close( sock );
	sock = 0;
}



bool CdListen::go( const QString &ad, int port )
{
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		KMessageBox::information( 0, i18n("Can't open info socket."), i18n("DVB Client") );
		sock = 0;
		return false;
	}

	addr.sin_family = AF_INET;         // host byte order
	addr.sin_port = htons( port );     // short, network byte order
	addr.sin_addr.s_addr = inet_addr( ad.ascii() );
	memset( &( addr.sin_zero ), '\0', 8 ); // zero the rest of the struct

	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		KMessageBox::information( 0, i18n("Can't bind info socket!!!"), i18n("DVB Client") );
		close( sock );
		sock = 0;
		return false;
       	}

	currentList = "";
	newList = currentList;
	isRunning = true;
	timer.start( 500, false );
	start();
	return true;
}



void CdListen::run()
{
	char buf[1500];
	int n;
	struct sockaddr_in a;
	socklen_t len = sizeof(struct sockaddr);

	while ( isRunning ) {
		memset( buf, '\0', 1500 );
		n = recvfrom( sock, buf, 1500, 0, (struct sockaddr *)&a, &len );
		if ( n>0 ) newList = buf;
		else msleep(500);
	}
}

#include "cdlisten.moc"
