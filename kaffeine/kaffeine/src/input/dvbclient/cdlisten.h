/*
 * cdlisten.h
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

#ifndef CDLISTEN_H
#define CDLISTEN_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <qobject.h>
#include <qthread.h>
#include <qtimer.h>
#include <qstring.h>

class CdListen : public QObject, public QThread
{
	Q_OBJECT

public:

	CdListen();
	~CdListen();
	virtual void run();
	bool go( const QString &ad, int port );
	void stop();

private slots:

	void updateList();

private:

	int sock;
	struct sockaddr_in addr;
	bool isRunning;
	QTimer timer;
	QString currentList, newList;

signals:

	void listChanged( const QString& );
};

#endif /* CDLISTEN_H */
