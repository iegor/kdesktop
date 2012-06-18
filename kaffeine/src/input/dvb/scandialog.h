/*
 * scandialog.h
 *
 * Copyright (C) 2004-2007 Christophe Thommeret <hftom@free.fr>
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

#ifndef SCANDIALOG_H
#define SCANDIALOG_H

#include <qtimer.h>
#include <qprogressbar.h>
#include <qlabel.h>

#include <kled.h>

#include "kgradprogress.h"
#include "scandialogui.h"
#include "channeldesc.h"

class DVBsi;
class DvbStream;



class DListViewItem : public QListViewItem
{

public:

	DListViewItem( QListView *parent, ChannelDesc *desc, QString label1, QString label2 );

	ChannelDesc *chan;
};



class ScanDialog : public ScanDialogUI
{
	Q_OBJECT

public:

	ScanDialog( QPtrList<DvbStream> *d, QPtrList<ChannelDesc> *ch, QSize size, QString src, const QString &charset );
	~ScanDialog();

	QPtrList<Transponder> transponders;
	QPtrList<ChannelDesc> *chandesc;

private slots:

	void scan( bool b );
	void checkNewChannel();
	void setLock( bool on );
	void siEnded( bool b );
	void addFiltered();
	void addSelected();
	void setProgress();
	void pop( QListViewItem *it, const QPoint &pos, int col );
	void deleteAll();
	void selectAll();
	void newChannel();
	void deleteChannel( QString name );
	void deleteChannel();
	void setDvb(int);
	void slotChannelChanged( QListViewItem*, const QPoint &, int );

private:

	bool getTransData();
	bool edit( QString &name, QPixmap &pix );
	void working( bool b );
	void parseTp( QString s, fe_type_t type, QString src );
	void addFound( ChannelDesc *chan, bool scan );
	void checkDuplicateName( ChannelDesc *chan );
	bool checkChannUpdate( ChannelDesc *chan );

	QPtrList<DvbStream> *dvb;
	DvbStream *ds;
	KGradProgress *snr, *signal;
	QProgressBar *progress;
	KLed *lock;
	DVBsi *dvbsi;
	QTimer checkTimer, progressTimer;
	int nChannels;
	QPixmap tvPix, raPix, tvcPix, racPix;
	int ntv, nradio;
	QLabel *progressLab;
	QString sourcesPath;
	QString defaultCharset;
};

#endif /* SCANDIALOG_H */
