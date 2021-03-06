/*
 * audioeditor.h
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

#ifndef AUDIOEDITOR_H
#define AUDIOEDITOR_H

#include "channeldesc.h"
#include "audioeditorui.h"

class AudioEditor : public AudioEditorUI
{
	Q_OBJECT
	
public:

	AudioEditor( ChannelDesc *chan, QWidget *parent );
	~AudioEditor();

private slots:

	void showProp( int index );
	void itemUp();
	void itemDown();
	void itemDelete();
	void itemUpdate();
	void itemNew();

private:

	void insertItem( int index, bool updt=false );

	ChannelDesc *channel;
	
};

#endif /* AUDIOEDITOR_H */
