/*
 * sender.cpp
 *
 * Copyright (C) 2003-2007 Christophe Thommeret <hftom@free.fr>
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

#include <kmessagebox.h>
#include <klocale.h>

#include "sender.h"



Sender::Sender()
{
	isRunning = false;
	senderSocket = 0;
	sendList = "\n";
}



bool Sender::makeSenderSocket( const QString &addr, int m_senderPort )
{
	int sockopt=1;

	if ( senderSocket ) return true;

	if ( ( senderSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		KMessageBox::error( 0, i18n("Can't open DVB info socket.") );
		senderSocket = 0;
		return false;
	}

	senderAddr.sin_family = AF_INET;         // host byte order
	senderAddr.sin_port = htons( m_senderPort );     // short, network byte order
	senderAddr.sin_addr.s_addr = inet_addr( addr.ascii() );

	memset( &( senderAddr.sin_zero ), '\0', 8 ); // zero the rest of the struct

	if ( setsockopt( senderSocket, SOL_SOCKET, SO_BROADCAST, &sockopt, sizeof(sockopt) ) < 0 ) {
		KMessageBox::error( 0, i18n("Can't open DVB info socket.") );
		close( senderSocket );
		senderSocket = 0;
		return false;
	}
	fprintf( stderr, "Sender socket opened\n" );
	return true;
}



Sender::~Sender()
{
	stop();
}



void Sender::closeSender()
{
	if ( senderSocket ) {
		sendto( senderSocket, "quit\n", 5, 0, (struct sockaddr *)&senderAddr, sizeof(senderAddr) );
		close( senderSocket );
		senderSocket = 0;
		fprintf( stderr, "Sender socket closed\n" );
	}
}



void Sender::run()
{
	int i;

	while( isRunning ) {
		sendto( senderSocket, sendList.latin1(), sendList.length(), 0, (struct sockaddr *)&senderAddr, sizeof(senderAddr) );
		msleep(500);
	}
}



void Sender::go()
{
	isRunning = true;
	start();
}



void Sender::stop()
{
	isRunning = false;
	if ( !wait(10000) ) {
		terminate();
		wait();
	}
	closeSender();
}
