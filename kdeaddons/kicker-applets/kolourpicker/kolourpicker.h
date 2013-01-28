/* This file is part of Kolourpicker
   Copyright (c) 2001 Malte Starostik <malte@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.

*/

#ifndef _KOLOURPICKER_H_
#define _KOLOURPICKER_H_

#include <qmap.h>
#include <qvaluelist.h>

#include <kpanelapplet.h>

#include "simplebutton.h"

class KInstance;
class QPopupMenu;

class KolourPicker : public KPanelApplet
{
	Q_OBJECT
public:
	KolourPicker(const QString& configFile, Type t = Normal, int actions = 0,
		QWidget *parent = 0, const char *name = 0);
    ~KolourPicker();
	virtual int heightForWidth(int) const;
	virtual int widthForHeight(int) const;
	virtual void about();

protected:
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void keyPressEvent(QKeyEvent *);
	virtual void resizeEvent(QResizeEvent*);

private slots:
	void slotPick();
	void slotHistory();

private:
	QPopupMenu *copyPopup(const QColor &, bool title) const;
	QPixmap colorPixmap(const QColor &) const;
	const QStringList &colorNames(int r, int g, int b) const;
	void arrangeButtons();
	void setClipboard(const QString& text);

	KInstance *m_instance;
	bool m_picking;
	SimpleButton *m_historyButton, *m_colourButton;
	QValueList<QColor> m_history;
	QMap<int, QStringList> m_colorNames;
};

#endif

// vim: ts=4 sw=4 noet
