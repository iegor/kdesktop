/*  This file is part of the KDE project
    Copyright (C) 2001-2002 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/
// $Id: kimageholder.h 465369 2005-09-29 14:33:08Z mueller $

#ifndef _KIMAGEHOLDER_H
#define _KIMAGEHOLDER_H

#include <qwidget.h>
class QRect;
class QPainter;
class QPen;
class QPixmap;
class KPixmap;
class QPoint;

/**
 * @short Image widget
 * @version $Id: kimageholder.h 465369 2005-09-29 14:33:08Z mueller $
 */
class KImageHolder : public QWidget
{
	Q_OBJECT
	public:
		KImageHolder( QWidget *parent = 0, const char * name = 0 );

		virtual ~KImageHolder();

		void clearSelection();

		void setImage( const KPixmap & );
		void setImage( const QImage & );
		void setImage( const QMovie & );

		/**
		 * clears the ImageHolder
		 */
		void clear();

		/**
		 * the selected rect
		 */
		QRect selection() const;

		QSize sizeHint() const;

		void setDrawRect( const QRect & rect ) { m_drawRect = rect; }
		const QRect & drawRect() const { return m_drawRect; }

	signals:
		void contextPress( const QPoint& );
		void selected( const QRect & );
		void wannaScroll( int dx, int dy );
		void cursorPos( const QPoint & );

	protected:
		void mousePressEvent( QMouseEvent * );
		void mouseMoveEvent( QMouseEvent * );
		void mouseReleaseEvent( QMouseEvent * );
		void paintEvent( QPaintEvent * );
		virtual void timerEvent( QTimerEvent * );

	private:
		void drawSelect( QPainter & );
		void eraseSelect();

		void setPixmap( const KPixmap & );

	private:
		QRect m_selection;
		QRect m_drawRect;
		QPoint m_scrollpos;
		QPoint m_selectionStartPoint;

		const KPixmap & checkboardPixmap();

		bool m_selected;
		bool m_bSelecting;
		int m_scrollTimerId;
		int m_xOffset, m_yOffset;

		QPen *m_pen;

		KPixmap * m_pPixmap;
		KPixmap * m_pDoubleBuffer;
		KPixmap * m_pCheckboardPixmap;
};

// vim:sw=4:ts=4

#endif // _KIMAGEHOLDER_H
