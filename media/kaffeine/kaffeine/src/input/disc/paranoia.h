/*
 * paranoia.h
 *
 * Copyright (C) 2002-2006 Christophe Thommeret <hftom@free.fr>
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

#ifndef PARANOIA_H
#define PARANOIA_H

#include <qstringlist.h>
#include <qthread.h>
#include <qwidget.h>

#include <kconfig.h>

#include "kaffeineaudioencoder.h"
#include "paranoiasettings.h"

extern "C"
{
#include <cdio/cdda.h>
#include <cdio/paranoia.h>
}

class KiloConfig : public ParanoiaSettings
{
	Q_OBJECT

public:

	KiloConfig( QWidget *parent, KConfig *confile, const QStringList &encoders );
	~KiloConfig();

	QString getEncoder();
	bool getNormalize();
	QString getBaseDir();
	int getParanoiaMode();

public slots:

	virtual void accept();
	void setBaseDir();

private:

	KConfig *Conf;
};

class Paranoia : public QThread
{
public:
	Paranoia();
	bool init( QString dev );
	~Paranoia();
	bool encode( const QStringList&, QWidget* );
	long getTracks();
	QString trackTime( int t );
	int trackFirstSector( int t );
	int discFirstSector();
	int discLastSector();
	virtual void run();
	bool running() {return isRunning;}
	int getProgress() {return progress;}

private:

	bool findCdrom();
	bool procCdrom( QString name );
	bool initTrack( int t );
	void setMode( int mode );
	bool isAudio( int t );
	QString trackSize( int t );
	long trackSectorSize( int t );
	bool loadEncoder( QWidget* );
	void unloadEncoder();
	bool validPath( QString path );
	bool setPath( QString &path, const QString &artist, const QString &album );

	long nTracks;
	cdrom_drive_t *d;
	cdrom_paranoia_t *p;
	long currentSector, endOfTrack;
	bool isRunning;
	QStringList encodingList;
	QWidget *myParent;
	KaffeineAudioEncoder *currentEncoder;
	QString encoderDesktop;
	bool normalize;
	QString baseDir;
	int paraMode;
	int progress;
};

#endif /* PARANOIA_H */
