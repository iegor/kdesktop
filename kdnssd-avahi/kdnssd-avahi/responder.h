/* This file is part of the KDE project
 *
 * Copyright (C) 2004 Jakub Stachowski <qbast@go2.pl>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef DNSSDRESPONDER_H
#define DNSSDRESPONDER_H

#include <qobject.h>
#include <qsocketnotifier.h>
#include <qsignal.h>
#include <config.h>
#include <avahi-client/client.h>
namespace DNSSD
{

/**
This class should not be used directly.
 
@author Jakub Stachowski
@short Internal class wrapping avahi client
 */
class Responder : public QObject
{
	Q_OBJECT

public:
	Responder();

	~Responder();

	static Responder& self();
	AvahiClientState state() const;
	AvahiClient* client() const { return m_client; }
	void process();
signals:
	void stateChanged(AvahiClientState);
private:
	AvahiClient* m_client;
	static Responder* m_self;
	friend void client_callback(AvahiClient*, AvahiClientState, void*);

};

/* Utils functions */

bool domainIsLocal(const QString& domain);
// Encodes domain name using utf8() or IDN 
QCString domainToDNS(const QString &domain);
QString DNSToDomain(const char* domain);


}

#endif
