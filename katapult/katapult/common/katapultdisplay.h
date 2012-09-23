/***************************************************************************
 *   Copyright (C) 2005 by Joe Ferris                                      *
 *   jferris@optimistictech.com                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef KATAPULTDISPLAY_H
#define KATAPULTDISPLAY_H

#include <qwidget.h>
#include <qstring.h>
#include <qevent.h>

#include "katapultitem.h"
#include "katapultaction.h"

class Katapult;
class KConfigBase;

/**
@author Joe Ferris
*/
class KatapultDisplay : public QWidget
{
	Q_OBJECT

public:
	KatapultDisplay(const char *, WFlags);
	virtual ~KatapultDisplay();
	
	void setQuery(QString);
	void setSelected(unsigned int);
	void setItem(const KatapultItem *);
	void setAction(const KatapultAction *);
	void setStatus(int);

	virtual void readSettings(KConfigBase *);
	virtual void writeSettings(KConfigBase *);
	virtual QWidget * configure();
	
signals:
	void keyReleased(QKeyEvent *);
	void focusOut();
	
protected:
	virtual void keyPressEvent(QKeyEvent *);
	virtual void keyReleaseEvent(QKeyEvent *);
	virtual void focusOutEvent(QFocusEvent *);
	
	QString query() const;
	unsigned int selected() const;
	const KatapultItem * item() const;
	const KatapultAction * action() const;
	int status() const;
	
private:
	QString _query;
	unsigned int _selectionLength;
	const KatapultItem *_item;
	const KatapultAction *_action;
	int _status;
	Katapult *app;
};

#endif
