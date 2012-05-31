/*
    Copyright (C) 2001-2003 KSVG Team
    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KSVGWidget_H
#define KSVGWidget_H

#include <qwidget.h>

#include <kpopupmenu.h>

#include <SVGEventImpl.h>

class KSVGPlugin;
class KSVGWidget : public QWidget
{
Q_OBJECT
public:
	KSVGWidget(KSVGPlugin *part, QWidget *parent, const char *name);
	virtual ~KSVGWidget();

	KSVGPlugin *part() const;

	void reset();

protected:
	virtual void paintEvent(QPaintEvent *);
	
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);

	virtual void keyPressEvent(QKeyEvent *);
	virtual void keyReleaseEvent(QKeyEvent *);

	virtual void resizeEvent(QResizeEvent *);

signals:
	void browseURL(const QString &);
	void redraw(const QRect &);

private:
	KSVG::SVGMouseEventImpl *newMouseEvent(KSVG::SVGEvent::EventId id, QMouseEvent *event);

	QPoint m_panningPos;
	QPoint m_oldPanningPos;
	KSVGPlugin *m_part;
};

#endif

// vim:ts=4:noet
