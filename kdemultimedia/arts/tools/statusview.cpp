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

#include <qlayout.h>
#include <klocale.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <qtimer.h>
#include <qlabel.h>

#include "statusview.h"

using namespace std;
using namespace Arts;

ArtsStatusView::ArtsStatusView(Arts::SoundServer a_server, QWidget* parent, const char* name )
	: Template_ArtsView( parent,name )
	, server(a_server)
{
	this->setCaption( i18n( "aRts Status" ) );
	QBoxLayout *l= new QVBoxLayout(this);
	//l->setAutoAdd(TRUE);

	RealtimeStatus rs= server.realtimeStatus();
	l->addWidget(new QLabel(rs==rtRealtime?
							i18n("Artsd is running with realtime scheduling."):
							rs==rtNoSupport?
							i18n("Your system does not support realtime scheduling."):
							rs==rtNoWrapper?
							i18n("Artsd is not configured for realtime scheduling\n "
								 "or was manually started without artswrapper."):
							i18n("Artsd should run with realtime scheduling,\n but it "
								 "does not (Is artswrapper suid root?)."),
							this, "realtimeLabel"));
	l->addSpacing(10);

	suspendLabel= new QLabel(i18n("Determining suspend status..."),
							 this, "suspendLabel");
	l->addWidget(suspendLabel);
	l->addSpacing(6);
	l->setMargin(6);

	suspendButton= new QPushButton(this, "suspendButton");
	suspendButton->setText(i18n("&Suspend Now"));
	l->addWidget(suspendButton);
	connect(suspendButton, SIGNAL(clicked()), this, SLOT(suspendButtonClicked()));

	artsPollStatusTimer= new QTimer(this);
	connect(artsPollStatusTimer, SIGNAL(timeout()), this, SLOT(updateStatus()));
	artsPollStatusTimer->start(1000);
	//l->activate();
	show();
}

void ArtsStatusView::suspendButtonClicked()
{
	(void)server.suspend(); // TODO: error msg if suspend not possible?
}

void ArtsStatusView::updateStatus()
{
	long seconds= server.secondsUntilSuspend();
	if (seconds<0)
		suspendLabel->setText(i18n("The aRts sound daemon will not autosuspend right\n"
								  "now since there are active modules."));
	else if (seconds==0)
		suspendLabel->setText(i18n("The aRts sound daemon is suspended. Legacy\n "
								  "applications can use the sound card now."));
	else
		suspendLabel->setText(i18n("Autosuspend will happen in %1 seconds.").
							 arg(seconds));
	suspendButton->setEnabled(seconds>0);
}

#include "statusview.moc"
