/*

    Copyright (C) 1999 Stefan Westerfeld
                       stefan@space.twc.de
                  2003 Arnold Krille
                       arnold@arnoldarts.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    */

#ifndef _AUDIOMANAGER_H_
#define _AUDIOMANAGER_H_

#include <artsflow.h>
#include <qwidget.h>
#include <qlistview.h>

#include "templateview.h"

class KListView;
class GuiAudioManagerProxy;
class QBoxLayout;

class Gui_AUDIO_MANAGER : public Template_ArtsView
{
	Q_OBJECT
protected:
	QWidget *ParentWidget;
	KListView *listview;
	GuiAudioManagerProxy *proxy;
	Arts::AudioManager AudioManager;

	bool inDialog;
	unsigned long changes;
	int x,y;
public:
	Gui_AUDIO_MANAGER( QWidget* =0, const char* =0 );
	~Gui_AUDIO_MANAGER();

	void setParent(QWidget *Parent,QBoxLayout *layout);

	void edit(QListViewItem *item);
public slots:
	void tick();
};

class GuiAudioManagerProxy :public QObject {
	Q_OBJECT
	Gui_AUDIO_MANAGER *gim;
public:
	GuiAudioManagerProxy(Gui_AUDIO_MANAGER *gim);
public slots:
	void edit(QListViewItem *item);
};

class AudioManagerItem : public QListViewItem {
	long _ID;
public:
	AudioManagerItem(QListView *parent, QString a, QString b,
			              QString c, long ID);
	~AudioManagerItem();

	long ID();
};

#endif
