/*
 * channeleditor.h
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

#ifndef CHANNELEDITOR_H
#define CHANNELEDITOR_H

#include <qstringlist.h>

#include "channeleditorui.h"
#include "channeldesc.h"

class ChannelEditor : public ChannelEditorUI
{
	Q_OBJECT

public:

	ChannelEditor( QStringList src, bool m, ChannelDesc *chan, QPtrList<ChannelDesc> *cdesc, QWidget *parent );
	~ChannelEditor();

protected:

	virtual void accept();

private slots:

	void editAudio();
	void editSubtitle();

private:

	void initS();
	void initC();
	void initT();
	void initA();
	QStringList inversionList();
	QStringList coderateList();
	QStringList modulationList();
	QStringList transmissionList();
	QStringList bandwidthList();
	QStringList hierarchyList();
	QStringList guardList();
	QStringList stypeList();
	QStringList rolloffList();

	ChannelDesc *channel;
	QPtrList<ChannelDesc> *chandesc;
	bool mode;
	QString orgName;
	int chanNum;

};

#endif /* CHANNELEDITOR_H */
