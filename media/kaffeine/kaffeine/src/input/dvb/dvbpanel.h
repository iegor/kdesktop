/*
 * dvbpanel.h
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

#ifndef DVBPANEL_H
#define DVBPANEL_H

#include <qframe.h>
#include <qcombobox.h>
#include <qtoolbutton.h>
#include <qptrlist.h>
#include <qtimer.h>
#include <qsplitter.h>
#include <qvbox.h>

#include <kapp.h>
#include <kpushbutton.h>
#include <kled.h>
#include <klistview.h>
#include <kiconview.h>
#include <klineedit.h>

#include "scandialog.h"
#include "dvbconfig.h"
#include "krecord.h"
#include "ts2rtp.h"
#include "cleaner.h"
#include "dvbevents.h"
#include "kaffeineinput.h"



class ChannelDesc;
class DvbStream;
class DvbPanel;
class KaffeineDvbPlugin;



class DListView : public KListView
{
	Q_OBJECT
public:
	DListView( QWidget *parent );

	int visibleItems;

protected:
	virtual QDragObject* dragObject();
};



class DIconViewItem : public KIconViewItem
{
public:
	DIconViewItem( DvbPanel *pan, QIconView *parent, const QString &text, const QPixmap &icon );
protected:
	void dropped( QDropEvent *e, const QValueList<QIconDragItem> & );
private:
	DvbPanel *panel;
};



class DvbPanel : public KaffeineInput
{
	Q_OBJECT

public:

	DvbPanel( QWidget *parent, QObject *objParent, const char *name=0);
	~DvbPanel();
	bool getChannelList();
	bool saveChannelList();
	bool timeShiftMode();
	void setConfig();
	void enableLiveDvb( bool on );
	void checkFirstRun();
	void moveChannel( const QString &cat, const QString &name );

	// Reimplemented from KaffeineInput
	QWidget *wantPlayerWindow();
	QWidget *inputMainWidget();
	void toggleLayout( bool );
	bool close();
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

	QPtrList<ChannelDesc> channels;
	QPtrList<RecTimer> timers;
	QPtrList<DvbStream> dvb;
	QString fifoName, fifoName1, currentFifo;
	QString timeShiftFileName;
	Ts2Rtp *rtp;
	QVBox *mainWidget;
	QVBox *playerBox, *pbox;

public slots:

	void playLastChannel();
	void playNumChannel( int num );
	void stopLive();
	void setShiftLed( bool on );
	void setRecordLed( bool on );
	void setBroadcastLed( bool on );
	void setRecord();
	void setBroadcast();
	void pauseLiveTV();
	void killTimer( RecTimer *t );
	void showEvents();
	void showTimers();
	void scanner();
	void showConfigDialog();
	void next();
	void previous();
	void recallZap();
	void dvbOSD();

	void dvbOSDNext();
	void dvbOSDPrev();
	void dvbOSDSkip(int skip, int timeShift = 0);
	void dvbOSDZap();
	void dvbOSDHide();

	void dvbOSDAdvance();
	void dvbOSDRetreat();

	void dvbNewTimer( QString name, QString channel, QString datetime, QString duration );
	int getSNR( int device );
	void diskStatus();
	void camClicked( int devNum );

private:

	void setupActions();
	bool getTimerList();
	bool saveTimerList();
	void fillChannelList( ChannelDesc *ch=0 );
	QPtrList<Transponder> getSourcesStatus();
	void updateModeTimer( RecTimer *t );
	DvbStream* getWorkingDvb( int mode, ChannelDesc *chan );

	void dvbOSD(ChannelDesc liveChannel, DvbStream *d, int timeShift = 0);
	DvbStream* getLiveDVBStream();

	QPixmap tvPix, raPix, tvcPix, racPix;
	QSplitter *split;
	QFrame *panel;
	KIconView *iconView;
	DListView *channelsCb;
	QToolButton *broadcastBtn, *recordBtn;
	QToolButton *dateBtn, *infoBtn, *channelsBtn, *configBtn, *osdBtn, *recallBtn;
	KLed *shiftLed, *recordLed, *broadcastLed;
	QTimer timersTimer;
	DVBconfig *dvbConfig;
	KRecord *timersDialog;
	int updown;
	unsigned long autocount;
	int currentChannelNumber;
	Cleaner *cleaner;
	int osdMode;
	QTimer osdTimer;
	QTimer showOsdTimer;
	QString currentCategory;
	bool isTuning;
	QTimer tuningTimer, stopTuningTimer;
	QTimer diskTimer;
	KLineEdit *searchLE;
	QToolButton *searchBtn;
	int recallChannel;

	int browseDvbStream;
	int maxChannelNumber, minChannelNumber;
	int browseDvbTimeShift;

	KaffeineDvbPlugin *plug;
	QString plugName;

	EventTable events;

private slots:

	void searchChannel( const QString &text );
	void resetSearch();
	void checkTimers();
	bool editChannel( QString &name );
	void channelSelected( QListViewItem *it );
	void channelClicked( QListViewItem *it );
	void channelSelected( const QString &name );
	void channelPopup( QListViewItem *it, const QPoint &pos, int col );
	void dvbZap( ChannelDesc *chan );
	void finalZap( DvbStream *d, ChannelDesc *chan );
	void newTimer( QString channel, QString name, QDateTime begin, QTime duration, bool warn=true );
	void dumpEvents();
	void resetOSD();
	void catContextMenu( QIconViewItem*, const QPoint& );
	void catSelected( QIconViewItem* );
	void channelNumberChanged( QListViewItem* );
	void pipeOpened();
	void setTuning();

signals:

	void zap( ChannelDesc* );
	void playDvb();
	void timersChanged();
	void dvbOpen( const QString &filename, const QString &chanName, int haveVideo );
	void dvbStop();
	void dvbPause( bool pauseLive );
	void setTimeShiftFilename( const QString& );
	void showOSD( const QString &text, int duration, int priority );
	void showDvbOSD( const QStringList& );
	void showDvbOSD( const QString&, const QStringList& );

	void updateTimer(RecTimer*, int);
};

#endif /* DVBPANEL_H */
