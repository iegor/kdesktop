/***************************************************************************
                          mediawidget.h - The main widget
                             -------------------
    begin                : Sat June 23 13:35:30 CEST 2001
    copyright            : (C) 2001 Joseph Wenninger
    email                : jowenn@kde.org
 ***************************************************************************/
 
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _MEDIAWIDGET_H_
#define _MEDIAWIDGET_H_

#include <kurl.h>
#include "mediawidget_skel.h"

class KSB_MediaWidget: public KSB_MediaWidget_skel
{
	Q_OBJECT
public:
	KSB_MediaWidget(QWidget *parent);
	~KSB_MediaWidget(){;}

private:
	class Player *player;
	QString pretty;
	bool needLengthUpdate;
	KURL::List m_kuri_list;

protected:
	virtual void dragEnterEvent ( QDragEnterEvent * );
	virtual void dropEvent ( QDropEvent * );

private slots:
	void playerTimeout();
	void playerFinished();
	void playing();
	void paused();
	void stopped();
	void empty();

public slots:
        void skipToWrapper(int);

signals:
        void skipTo(unsigned long);

};

#endif
