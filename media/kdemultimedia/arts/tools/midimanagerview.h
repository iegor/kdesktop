/*

    Copyright (C) 2000 Stefan Westerfeld
                       stefan@space.twc.de

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

    Permission is also granted to link this program with the Qt
    library, treating Qt like a library that normally accompanies the
    operating system kernel, whether or not that is in fact the case.

    */

#ifndef ARTS_TOOLS_MIDIMANAGERVIEW_H
#define ARTS_TOOLS_MIDIMANAGERVIEW_H

#include <kmainwindow.h>
#include "artsmidi.h"
#include <map>

class ConnectionWidget;
class MidiManagerItem;
class MidiManagerWidget;

class MidiManagerView : public KMainWindow {
	Q_OBJECT
protected:
	friend class ConnectionWidget;
	Arts::MidiManager manager;
	MidiManagerWidget *widget;
	ConnectionWidget *connectionWidget;
	std::map<long, MidiManagerItem *> itemMap;

public:
	MidiManagerView();

	void closeEvent(QCloseEvent *e);
public slots:
	void updateLists();
	void slotConnect();
	void slotDisconnect();
	void addOSSMidiPort();
	void addArtsMidiOutput();

signals:
	void closed();
};

#endif
