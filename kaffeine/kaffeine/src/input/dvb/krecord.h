/*
 * krecord.h
 *
 * Copyright (C) 2004-2005 Christophe Thommeret <hftom@free.fr>
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

#ifndef KRECORD_H
#define KRECORD_H

#include <qdialog.h>
#include <qpixmap.h>

#include <kpushbutton.h>
#include <klistview.h>

#include "channeldesc.h"
#include "ktimereditor.h"



class KRecord : public QDialog
{
	Q_OBJECT

public:

	KRecord( QStringList chanList, QPtrList<RecTimer> *t, QWidget *parent, QSize size );
	~KRecord();

	QStringList channelsList;

protected slots:

	virtual void accept();
	void newTimer();
	void editTimer();
	void deleteTimer();
	void refresh();

private:

	QListViewItem* where( RecTimer *rt, bool add=false );

	KPushButton *newBtn, *editBtn, *deleteBtn, *okBtn;
	KListView *list;
	QPtrList<RecTimer> *timers;
	QPixmap isRecording, yesRepeat;

signals:

	void updateTimer(RecTimer*, int);

};

#endif /* KRECORD_H */
