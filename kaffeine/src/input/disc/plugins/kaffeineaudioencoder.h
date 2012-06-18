/*
 * kaffeineaudioencoder.h
 *
 * Copyright (C) 2006 Christophe Thommeret <hftom@free.fr>
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

#ifndef KAFFEINEAUDIOENCODER_H
#define KAFFEINEAUDIOENCODER_H

#include <kparts/part.h>
#include <kconfig.h>

#include <qstring.h>
#include <qwidget.h>

/*
 * Base-Class for Kaffeine audio encoder plugins.
 */

class KDE_EXPORT KaffeineAudioEncoder : public KParts::Part
{
	Q_OBJECT
public:
	KaffeineAudioEncoder(QObject* parent, const char* name);
	virtual ~KaffeineAudioEncoder();

	// return false if the user's canceled.
	virtual bool options( QWidget*, KConfig* ) {return false;}

	// your file extension, e.g. ".ogg"
	virtual QString getExtension() {return QString();}

	virtual void start( QString/*title*/=0, QString/*artist*/=0, QString/*album*/=0, QString/*tracknumber*/=0, QString/*genre*/=0 ) {}
	virtual char* getHeader( int&/*len*/ ) {return NULL;}
	virtual char* encode( char*/*data*/, int /*datalen*/, int&/*len*/ ) {return NULL;}
	virtual char* stop( int& /*len*/) {return NULL;}
};

#endif /* KAFFEINEAUDIOENCODER_H */
