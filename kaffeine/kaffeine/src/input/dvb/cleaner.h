/*
 * cleaner.h
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

#ifndef CLEANER_H
#define CLEANER_H

#include <qobject.h>
#include <qthread.h>
#include <qstring.h>
#include <qtimer.h>

class Cleaner : public QObject, public QThread
{

	Q_OBJECT

public:

	Cleaner( const QString &lpath, const QString &rpath );
	~Cleaner();
	void setPaths( const QString &lpath, const QString &rpath );

protected:

	virtual void run();

private slots:

	void doClean();

private:

	QTimer timer;
	QString livePath, recordPath;
};

#endif /* CLEANER_H */
