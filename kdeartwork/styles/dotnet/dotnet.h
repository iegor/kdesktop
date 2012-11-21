/*
 * $Id: dotnet.h 465369 2005-09-29 14:33:08Z mueller $
 *
 *	Copyright 2001, Chris Lee <lee@azsites.com>
 *	Originally copied from the KDE3 HighColor style, modified to fit mine.
 *
 *	This library is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Library General Public
 *	License version 2 as published by the Free Software Foundation.
 *
 *	This library is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *	Library General Public License for more details.
 *
 *	You should have received a copy of the GNU Library General Public License
 *	along with this library; see the file COPYING.LIB.  If not, write to
 *	the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *	Boston, MA 02110-1301, USA.
 */

#ifndef __DOTNET_H
#define __DOTNET_H

#include <kstyle.h>
#include <qbitmap.h>

#define u_arrow -4,1, 2,1, -3,0, 1,0, -2,-1, 0,-1, -1,-2
#define d_arrow -4,-2, 2,-2, -3,-1, 1,-1, -2,0, 0,0, -1,1
#define l_arrow 0,-3, 0,3,-1,-2,-1,2,-2,-1,-2,1,-3,0
#define r_arrow -2,-3,-2,3,-1,-2, -1,2,0,-1,0,1,1,0

#define QCOORDARRLEN(x) sizeof(x)/(sizeof(QCOORD)*2)

class dotNETstyle : public KStyle
{
	Q_OBJECT

public:
	dotNETstyle();
	virtual ~dotNETstyle();

	bool inheritsKHTML( const QWidget* widget ) const;
	void polish( QWidget* widget );
	void unPolish( QWidget* widget );

	void renderMenuBlendPixmap( KPixmap&, const QColorGroup&, const QPopupMenu * ) const;

	void drawKStylePrimitive( KStylePrimitive kpe,
		QPainter* p,
		const QWidget* widget,
		const QRect &r,
		const QColorGroup &cg,
		SFlags flags = Style_Default,
		const QStyleOption& = QStyleOption::Default ) const;

	void drawPrimitive( PrimitiveElement pe,
		QPainter *p,
		const QRect &r,
		const QColorGroup &cg,
		SFlags flags = Style_Default,
		const QStyleOption &opt = QStyleOption::Default ) const;

	void drawControl( ControlElement element,
		QPainter *p,
		const QWidget *widget,
		const QRect &r,
		const QColorGroup &cg,
		SFlags flags = Style_Default,
		const QStyleOption& = QStyleOption::Default ) const;

	void drawControlMask( ControlElement, QPainter *, const QWidget *, const QRect &, const QStyleOption &) const;

	void drawComplexControl( ComplexControl control,
		QPainter *p,
		const QWidget *widget,
		const QRect &r,
		const QColorGroup &cg,
		SFlags flags = Style_Default,
		SCFlags controls = SC_All,
		SCFlags active = SC_None,
		const QStyleOption& = QStyleOption::Default ) const;

	int pixelMetric( PixelMetric m,
		const QWidget *widget = 0 ) const;

	QRect subRect( SubRect r,
		const QWidget *widget ) const;

	QRect querySubControlMetrics( ComplexControl control,
		const QWidget *widget,
		SubControl subcontrol,
		const QStyleOption &opt = QStyleOption::Default ) const;

	void drawComplexControlMask(QStyle::ComplexControl c,
	                            QPainter *p,
	                            const QWidget *w,
	                            const QRect &r,
	                            const QStyleOption &o=QStyleOption::Default) const;

	QSize sizeFromContents(QStyle::ContentsType t,
	                       const QWidget *w,
	                       const QSize &s,
	                       const QStyleOption &o) const;

protected:
	void renderButton(QPainter *p,
	                  const QRect &r,
	                  const QColorGroup &g,
	                  bool sunken = false,
	                  bool corners = false) const;

	void renderPanel(QPainter *p,
	                 const QRect &r,
	                 const QColorGroup &g,
	                 bool sunken = true,
	                 bool thick = true) const;

	void renderSlider(QPainter *p,
	                  const QRect &r,
	                  const QColorGroup &g) const;
	bool eventFilter(QObject *, QEvent *);

	void updatePalette( QComboBox * );
	void updatePalette( QToolBar * );
	void updatePalette( QMenuBar * );

protected slots:
	void slotDestroyed();
	void paletteChanged();

private:
// Disable copy constructor and = operator
	dotNETstyle( const dotNETstyle & );
	dotNETstyle& operator=( const dotNETstyle & );
	QStyle *winstyle;

	bool pseudo3D, useTextShadows, roundedCorners, reverseLayout, kickerMode;

	QValueList<QWidget*> m_widgets;
};

#endif

// vim: set noet ts=4 sw=4:
