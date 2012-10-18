/*

    Copyright (C) 2000-2001 Stefan Westerfeld
                            <stefan@space.twc.de>
                       2003 Arnold Krille
                            <arnold@arnoldarts.de>

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

#ifndef STATUSVIEW_H
#define STATUSVIEW_H

#include <qwidget.h>
#include <arts/soundserver.h>

#include "templateview.h"

class QButton;
class QTimer;
class QLabel;

class ArtsStatusView : public Template_ArtsView {
	Q_OBJECT
public:
	ArtsStatusView(Arts::SoundServer server, QWidget* =0, const char* =0 );
	
public slots:
	void updateStatus();
	void suspendButtonClicked();
	
protected:
	QTimer *artsPollStatusTimer;
	Arts::SoundServer server;
	QLabel *suspendLabel;
	QButton *suspendButton;
};

#endif
