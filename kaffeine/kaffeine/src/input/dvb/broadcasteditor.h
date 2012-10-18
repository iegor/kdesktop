/*
 * broadcasteditor.h
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

#ifndef BROADCASTEDITOR_H
#define BROADCASTEDITOR_H

#include <qpixmap.h>
#include <qfile.h>
#include <qlistbox.h>

#include "broadcasteditorui.h"
#include "channeldesc.h"

class BroadcastEditor : public BroadcastEditorUI
{
	Q_OBJECT

public:

	BroadcastEditor( QWidget *parent, QPtrList<ChannelDesc> *ch, QPtrList<ChannelDesc> *ret );
	~BroadcastEditor();

	QPtrList<ChannelDesc> *chan;
	QPtrList<ChannelDesc> *list;

public slots:

	virtual void addToList();
	virtual void resetList();
	void slotAddChannel(QListBoxItem*);

private:

	bool getChannelList();
	ChannelDesc* getChannel( const QString &name );

	QPixmap tvPix, raPix, tvcPix, racPix;

};

#endif /* BROADCASTEDITOR_H */
