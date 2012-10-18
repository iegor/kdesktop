/*
 * kevents.h
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

#ifndef KEVENTS_H
#define KEVENTS_H

#include <qtimer.h>
#include <qdialog.h>
#include <qdatetime.h>
#include <qtextbrowser.h>

#include <klistview.h>
#include <kpushbutton.h>
#include <klineedit.h>

class EventDesc;
class DvbStream;
class EventTable;
class ChannelDesc;


class EListViewItem : public KListViewItem
{

public:

	EListViewItem( QListView *parent, QString chanName, QString eBegin, QString eDuration, QString eTitle, EventDesc *desc );
	virtual int compare( QListViewItem *i, int col, bool ascending ) const;

	EventDesc *event;
};



class KEvents : public QDialog
{
	Q_OBJECT

public:

	KEvents( QPtrList<ChannelDesc> *chans, QPtrList<DvbStream> *d, EventTable *t, QWidget *parent, QSize size );
	~KEvents();

private slots:

	void mouseClickedSlot( int btn, QListViewItem *it, const QPoint &p, int c );
	void reset();
	void setMode( int m, QString name="" );
	void setScheduled();
	void setCurrentNext();
	void setCurrentChannelEpg();
	void epgSearch();
	void resetSearch();
	void zap( QListViewItem* it, const QPoint &p, int col );

private:
	void checkEpgSearch(QString searchword);
	void checkNewEvent();

	QPtrList<DvbStream> *dvb;
	EventTable *events;
	KListView *listView;
	KPushButton *resetBtn, *currentNextBtn, *allBtn, *currentChannelEpgBtn;
	QToolButton *searchBtn;
	QCheckBox *titleCb,*tvradioCb,*ftaCb;
	QPtrList<ChannelDesc> *channels;
	QTextBrowser *textBrow;
	int mode;
	ChannelDesc *chan;

protected:

	KLineEdit *searchLineEdit;

signals:

	void addTimer( QString channel, QString name, QDateTime begin, QTime duration );
	void zapTo( const QString &channel );
};

#endif /* KEVENTS_H */
