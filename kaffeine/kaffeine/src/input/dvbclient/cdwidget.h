/*
 * cdwidget.h
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

#ifndef CDWIDGET_H
#define CDWIDGET_H

#include <qlistbox.h>
#include <qpixmap.h>
#include <qptrlist.h>
#include <qvbox.h>
#include <qsplitter.h>

#include <kconfig.h>

#include "cddump.h"
#include "cdlisten.h"
#include "cdcleaner.h"
#include "kaffeineinput.h"



class CdWidget : public KaffeineInput
{
	Q_OBJECT

public:

	CdWidget( const QString &ad, int port, int info, const QString &tspath, QWidget *parent, QObject *objParent, const char *name );
	~CdWidget();
	void setParam( const QString &ad, int port, int info, const QString &tspath );
	void enableLive( bool b );
	void playNumChannel( int num );
	void next();
	void previous();

	// Reimplemented from KaffeineInput
	QWidget *wantPlayerWindow();
	QWidget *inputMainWidget();
	bool nextTrack( MRL& );
	bool previousTrack( MRL& );
	bool currentTrack( MRL& );
	bool trackNumber( int, MRL& );
	bool playbackFinished( MRL& );
	void getTargets( QStringList &uiNames, QStringList &iconNames, QStringList &targetNames );
	void togglePanel();
	bool execTarget( const QString& );
	void saveConfig();
	//***************************************

	QListBox *channelsLb;
	QVBox *mainWidget;
	QVBox *playerBox;

public slots:

	void playLastChannel();
	void stopLive();
	void pauseLiveTV();

private:
	void loadConfig( KConfig* config );
	void saveConfig( KConfig* config );

	QSplitter *split;
	CdDump *dump;
	CdListen *listen;
	QString cdAddress;
	int cdPort;
	int cdInfo;
	QString cdShiftDir;
	QPixmap tvPix, raPix;
	QPtrList<CdChannel> chan;
	QString fifoName;
	QString timeShiftFileName;
	CdCleaner *cleaner;
	int lastChannel;

private slots:

	void updateList( const QString &list );
	void channelSelected( const QString &name );

signals:

	void dvbOpen(const QString&, const QString&, int);
	void dvbStop();
	void setTimeShiftFilename( const QString& );
};

#endif /* CDWIDGET_H */
