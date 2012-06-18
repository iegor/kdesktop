/*
 * ktimereditor.h
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

#ifndef KTIMEREDITOR_H
#define KTIMEREDITOR_H

#include <qdialog.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qdatetimeedit.h>
#include <qlabel.h>

#include <kpushbutton.h>

#include "channeldesc.h"
#include "gdvb.h"
#include "crontimer.h"



class KTimerEditor : public QDialog
{
	Q_OBJECT

public:

	KTimerEditor( bool newone, QStringList &chanList, RecTimer t, QWidget *parent );
	~KTimerEditor();

	KPushButton *okBtn, *cancelBtn, *repeatBtn;
	QLabel *repeatLab;
	QLineEdit *nameLe;
	QComboBox *channelComb;
	QDateTimeEdit *begin, *end;
	QTimeEdit *duration;
	RecTimer timer;

public slots:

	void setRepeat();

protected slots:

	virtual void accept();
	void setDuration( const QDateTime &dt );
	void setEnd( const QTime &t );
	void setMaxEnd( const QDateTime &dt );
};

#endif /* KTIMEREDITOR_H */
