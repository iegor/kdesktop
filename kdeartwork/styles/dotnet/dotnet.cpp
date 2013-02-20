/*
 *	KDE3 dotNET Style (version 1.5)
 *	Copyright (C) 2001-2002, Chris Lee <clee@kde.org>
 *	                         Carsten Pfeiffer <pfeiffer@kde.org>
 *                           Karol Szwed <gallium@kde.org>
 *
 *	Drawing routines completely reimplemented from KDE3 HighColor, which was
 *	originally based on some stuff from the KDE2 HighColor.
 *	Includes portions of framework code from KDE3 HighColor,
 *          (C) 2001-2002 Karol Szwed <gallium@kde.org>,
 *          (C) 2001-2002 Fredrik H�lund  <fredrik@kde.org>
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
#include <qstyleplugin.h>
#include <qstylefactory.h>
#include <qpointarray.h>
#include <qpainter.h>
#include <qtabbar.h>
#include <qtabwidget.h>
#include <qprogressbar.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qscrollbar.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qtoolbar.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qdrawutil.h>
#include <qapplication.h>
#include <qvariant.h>
#include <qpixmapcache.h>
#include <qslider.h>
#include <qsettings.h>
#include <kpixmap.h>

#include "dotnet.h"
#include "dotnet.moc"

/*	This code registers the style with QStyleFactory, which makes it possible
 *	for any code that uses QStyle to find out about it. */
// -- Style Plugin Interface -------------------------
class dotNETstylePlugin : public QStylePlugin
{
	public:
		dotNETstylePlugin() {}
		~dotNETstylePlugin() {}

		QStringList keys() const {
			return QStringList() << "dotNET";
		}

		QStyle* create( const QString& key ) {
			if (key == "dotnet")
				return new dotNETstyle;
			return 0;
		}
};

KDE_Q_EXPORT_PLUGIN( dotNETstylePlugin )

// ---------------------------------------------------

dotNETstyle::dotNETstyle() : KStyle( AllowMenuTransparency ), kickerMode(false)
{
	winstyle = 0L;
	winstyle = QStyleFactory::create("Windows");
	if (winstyle == 0L) {
	//	We don't have the Windows style, neither builtin nor as a plugin.
	//	Use any style rather than crashing.
		winstyle = QStyleFactory::create("B3");
	}

	if (qApp->inherits("KApplication")) {
		connect( qApp, SIGNAL( kdisplayPaletteChanged() ), SLOT( paletteChanged() ));
	}

	QSettings settings;
	pseudo3D = settings.readBoolEntry("/KStyle/Settings/Pseudo3D", true);
	roundedCorners = settings.readBoolEntry("/KStyle/Settings/RoundedCorners", true);
	useTextShadows = settings.readBoolEntry("/KStyle/Settings/UseTextShadows", false);
	reverseLayout = QApplication::QApplication::reverseLayout();
}


dotNETstyle::~dotNETstyle()
{
}

bool dotNETstyle::inheritsKHTML(const QWidget* w) const
{
	if (w->parentWidget(true) && w->parentWidget(true)->parentWidget(true) && w->parentWidget(true)->parentWidget(true)->parentWidget(true) &&  w->parentWidget(true)->parentWidget(true)->parentWidget(true)->inherits("KHTMLView")) {
		return true;
	} else {
		return false;
	}
}

void dotNETstyle::polish(QWidget* widget)
{
	if (!qstrcmp(qApp->argv()[0], "kicker") || widget->inherits("Kicker"))
		kickerMode = true;

	if (widget->isTopLevel())
		return;

// we can't simply set a palette -- upon color-theme changes, we have
// to update the palette again.
	bool extraPalette = false;

	if (widget->inherits("QComboBox") && !inheritsKHTML(widget)) {
		widget->installEventFilter (this);
		updatePalette( (QComboBox*) widget );
		extraPalette = true;
	} else {
		winstyle->polish(widget);
	}

//	This code is disabled until Carsten explains to me why it's supposed to
//	be here. It breaks dynamically changing the color from KControl and does
//	other bad things (see bug #54569)
/*
	if (!widget->ownPalette()) {
		if (widget->inherits("QToolBar")) {
			updatePalette( (QToolBar*) widget );
			extraPalette = true;
		} else if (widget->inherits("QMenuBar")) {
			updatePalette( (QMenuBar*) widget );
			extraPalette = true;
		}
	}
*/
}

void dotNETstyle::unPolish(QWidget* widget)
{
	winstyle->unPolish(widget);

	if (widget->inherits("QComboBox") && !inheritsKHTML(widget)) {
		widget->removeEventFilter (this);
	}
}

void dotNETstyle::renderMenuBlendPixmap(KPixmap &pix,
                                        const QColorGroup &cg,
                                        const QPopupMenu *popup) const
{
	QPainter p( &pix );
	if (QApplication::reverseLayout()) {
		p.fillRect( popup->frameRect().width()-22, 0, 22, pix.height(), cg.mid() );
		p.fillRect( 0, 0, popup->frameRect().width()-22, pix.height(), cg.background().light() );
	} else {
		p.fillRect( 0, 0, pix.width(), pix.height(), cg.background().light() );
		p.fillRect( popup->frameRect().left() + 1, 0, 22, pix.height(), cg.mid() );
	}
}

/*	This is the code that renders 90+% of the buttons in dotNET.
 *	Changing the code here will most likely affect EVERYTHING.
 *	If you want to modify the style, this is a good place to start.
 *	Also, take a look at the sizeMetric function and play with the
 *	widths that things return for different Frame elements.
 */
void dotNETstyle::renderButton(QPainter *p,
                               const QRect &r,
                               const QColorGroup &g,
                               bool sunken,
                               bool corners) const
{
	if (sunken) {
		p->setPen(g.highlight().dark());
		p->setBrush(g.highlight());
		p->drawRect(r);
	} else {
		int x, y, w, h;
		r.rect(&x, &y, &w, &h);

		if (pseudo3D) {
			if (roundedCorners) {
				const QCOORD iCorners[] = { x, y + h - 2, x, y + 1, x + 1, y, x + w - 2, y, x + w - 1, y + 1, x + w - 1, y + h - 2, x + w - 2, y + h - 1, x + 1, y + h - 1 };
				p->fillRect(x+1, y+1, w-2, h-2, g.button());
				p->setPen(g.button().dark());
				p->drawLineSegments(QPointArray(8, iCorners));
				if (corners) {
					const QCOORD cPixels[] = { x, y, x + w - 1, y, x + w - 1, y + h - 1, x, y + h - 1 };
					p->setPen(g.button());
					p->drawPoints(QPointArray(4, cPixels));
				}
			} else {
				p->setPen(g.button().dark());
				p->setBrush(g.button());
				p->drawRect(r);
			}

			const QCOORD oCorners[] = { x + 1, y + h - 2, x + 1, y + 1, x + w - 2, y + 1, x + w - 2, y + h - 2 };
			const QPointArray outline(4, oCorners);
			p->setPen(g.button().dark(115));
			p->setBrush(QBrush::NoBrush);
			p->drawConvexPolygon(outline);
			p->setPen(g.button().light());
			p->drawPolyline(outline, 0, 3);
		} else {
			p->setPen(g.button().dark());
			p->setBrush(g.button());
			p->drawRect(r);
			p->setPen(g.button().light());
			p->drawLine(x, y + h - 1, x, y);
			p->drawLine(x, y, x + w - 1, y);
		}
	}
}

/*	This is the code that renders the slider handles on scrollbars. It draws
 *	them just like normal buttons, by calling renderButton, but it also adds
 *	very subtle grips to the slider handle.
 */
void dotNETstyle::renderSlider(QPainter *p,
                               const QRect &r,
                               const QColorGroup &g) const
{
	int x, y, w, h;
	int offset = 6;
	int llx, lly, urx, ury;

	r.rect(&x,&y,&w,&h);

	llx = (x + (w/2) - 4);
	lly = (y + (h/2) + 3);
	urx = (x + (w/2) + 2);
	ury = (y + (h/2) - 3);

	renderButton(p, r, g, false, true);

	p->save();

	if (h > 15 && w > 15) {
		p->setPen(g.background().dark(120));
		p->drawLine(llx, lly, urx, ury);
		if (h > 35) {
			p->translate(0, offset);
			p->drawLine(llx, lly, urx, ury);
			p->translate(0, -(offset * 2));
			p->drawLine(llx, lly, urx, ury);
			p->translate(0, offset);
		}

		if (w > 35) {
			p->translate(offset, 0);
			p->drawLine(llx, lly, urx, ury);
			p->translate(-(offset * 2), 0);
			p->drawLine(llx, lly, urx, ury);
			p->translate(offset, 0);
		}

		p->setPen(g.background().light());
		p->translate(0, 1);
		p->drawLine(llx, lly, urx, ury);
		if (h > 35) {
			p->translate(0, offset);
			p->drawLine(llx, lly, urx, ury);
			p->translate(0, -(offset * 2));
			p->drawLine(llx, lly, urx, ury);
			p->translate(0, offset);
		}

		if (w > 35) {
			p->translate(offset, 0);
			p->drawLine(llx, lly, urx, ury);
			p->translate(-(offset * 2), 0);
			p->drawLine(llx, lly, urx, ury);
			p->translate(offset, 0);
		}
	}

	p->restore();
}

/*	This function does the rendering for most of the panels in dotNET.
 *	The QCOORD stuff is used to provide the slightly-rounded corners.
 *	Changing this function will have pretty widespread effects. Also a good
 *	place to start if you're looking to make your own style.
 */
void dotNETstyle::renderPanel(QPainter *p,
                              const QRect &r,
                              const QColorGroup &g,
                              bool sunken,
                              bool thick) const
{
	int x, x2, y, y2, w, h;
	r.rect(&x,&y,&w,&h);
	r.coords(&x, &y, &x2, &y2);

	p->save();

	if (pseudo3D) {
		const QCOORD oCorners[] = { x, y2, x, y, x2, y, x2, y2 };
		const QCOORD iCorners[] = { x+1, y2-1, x+1, y+1, x2-1, y+1, x2-1, y2-1 };
		p->setPen(g.background().dark());
		p->drawConvexPolygon(QPointArray(4, oCorners));

		if (thick) {
			p->setPen(g.background().dark(115));
			p->drawConvexPolygon(QPointArray(4, iCorners));
			p->setPen(g.background().light());

			if (sunken) {
				p->drawLine(w-2, h-2, x+1, h-2);
				p->drawLine(w-2, h-2, w-2, y+1);
			} else {
				p->drawLine(x+1, y+1, x+1, h-2);
				p->drawLine(x+1, y+1, w-2, y+1);
			}
		}

		if (roundedCorners) {
			p->setPen(g.background());
			p->drawPoints(QPointArray(4, oCorners));
		}
	} else {
		if (sunken) {
			const QCOORD corners[] = { x2, y, x2, y2, x, y2, x, y };
			p->setPen(g.background().dark());
			p->drawConvexPolygon(QPointArray(4, corners));
			p->setPen(g.background().light());
			p->drawPolyline(QPointArray(4, corners), 0, 3);
		} else {
			const QCOORD corners[] = { x, y2, x, y, x2, y, x2, y2 };
			p->setPen(g.background().dark());
			p->drawPolygon(QPointArray(4, corners));
			p->setPen(g.background().light());
			p->drawPolyline(QPointArray(4, corners), 0, 3);
		}
	}

	if (kickerMode) {
	//	Stolen wholesale from Keramik. I don't like it, but oh well.
		if (!thick) {
			if (sunken) {
				const QCOORD corners[] = { x2, y, x2, y2, x, y2, x, y };
				p->setPen(g.background().dark());
				p->drawConvexPolygon(QPointArray(4, corners));
				p->setPen(g.background().light());
				p->drawPolyline(QPointArray(4, corners), 0, 3);
			} else {
				const QCOORD corners[] = { x, y2, x, y, x2, y, x2, y2 };
				p->setPen(g.background().dark());
				p->drawPolygon(QPointArray(4, corners));
				p->setPen(g.background().light());
				p->drawPolyline(QPointArray(4, corners), 0, 3);
			}
		}
	}

	p->restore();
}


void dotNETstyle::drawKStylePrimitive(KStylePrimitive kpe,
                                      QPainter *p,
                                      const QWidget* widget,
                                      const QRect &r,
                                      const QColorGroup &cg,
                                      SFlags flags,
                                      const QStyleOption& opt) const
{
//  SLIDER
//  ------
	switch( kpe ) {
		case KPE_SliderGroove: {
			const QSlider* slider = (const QSlider*)widget;
			int x, y, w, h;
			r.rect(&x, &y, &w, &h);
			bool horizontal = slider->orientation() == Horizontal;
			int gcenter = (horizontal ? h : w) / 2;

			if (horizontal) {
				gcenter += y;
				p->setPen (cg.background().dark());
				p->drawLine(x, gcenter, x+w, gcenter);
				gcenter++;
				p->setPen (cg.background().light());
				p->drawLine(x, gcenter, x + w, gcenter);
			} else {
				gcenter += x;
				p->setPen (cg.background().dark());
				p->drawLine(gcenter, y, gcenter, y + h);
				gcenter++;
				p->setPen (cg.background().light());
				p->drawLine(gcenter, y, gcenter, y + h);
			}
			break;
		}

		case KPE_SliderHandle: {
			renderButton(p, r, cg);
			break;
		}

		default:
			KStyle::drawKStylePrimitive(kpe, p, widget, r, cg, flags, opt);
	}
}


// This function draws primitive elements as well as their masks.
void dotNETstyle::drawPrimitive(PrimitiveElement pe,
                                QPainter *p,
                                const QRect &r,
                                const QColorGroup &cg,
                                SFlags flags,
                                const QStyleOption &opt ) const
{
	bool down   = flags & Style_Down;
	bool on     = flags & Style_On;
	bool sunken = flags & Style_Sunken;
	bool horiz  = flags & Style_Horizontal;

	int x, x2, y, y2, w, h;
	r.rect(&x, &y, &w, &h);
	r.coords(&x, &y, &x2, &y2);

	switch(pe) {
	//	BUTTONS
	//	-------
		case PE_ButtonBevel:
		case PE_ButtonTool:
		case PE_ButtonDropDown:
		case PE_HeaderSection:
		case PE_ButtonCommand: {

			if (on || down) {
				renderButton(p, r, cg, true);
				p->setPen( cg.highlightedText() );
			} else {
				renderButton(p, r, cg, false, true);
			}

			break;
		}

		case PE_ButtonDefault: {
			QRect sr = r;
			p->setPen(QPen::NoPen);
			p->setBrush(cg.background().dark(105));
			p->drawRoundRect(sr, 25, 25);
			p->setBrush(cg.background().dark(110));
			sr.setCoords(sr.x() + 1, sr.y() + 1, sr.width() - 2, sr.height() - 2);
			p->drawRoundRect(sr, 25, 25);
			p->setBrush(cg.background().dark(115));
			sr.setCoords(sr.x() + 1, sr.y() + 1, sr.width() - 2, sr.height() - 2);
			p->drawRoundRect(sr, 25, 25);
			break;
		}

		case PE_ScrollBarSlider: {
			renderSlider(p, r, cg);
			break;
		}

		case PE_ScrollBarAddPage:
		case PE_ScrollBarSubPage: {
			// draw double buffered to avoid flicker...
			QPixmap buffer(2,2);
			QRect br(buffer.rect() );
			QPainter bp(&buffer);

			if (on || down) {
				bp.fillRect(br, QBrush(cg.mid().dark()));
			} else {
				bp.fillRect(br, QBrush(cg.background()));
			}
			bp.fillRect(br, QBrush(cg.background().light(), Dense4Pattern));

			bp.end();
			p->drawTiledPixmap(r, buffer);

			break;
		}

	//	SCROLLBAR BUTTONS
	//	-----------------
		case PE_ScrollBarSubLine: {
			drawPrimitive(PE_ButtonBevel, p, r, cg, flags);
			p->setPen(down ? cg.highlightedText() : cg.foreground());
			drawPrimitive((horiz ? PE_ArrowLeft : PE_ArrowUp), p, r, cg, flags);
			break;
		}

		case PE_ScrollBarAddLine: {
			drawPrimitive(PE_ButtonBevel, p, r, cg, flags);
			p->setPen(down ? cg.highlightedText() : cg.foreground());
			drawPrimitive((horiz ? PE_ArrowRight : PE_ArrowDown), p, r, cg, flags);
			break;
		}

	//	CHECKBOXES
	//	----------
		case PE_Indicator: {
			p->setPen(cg.background().dark());
			p->setBrush(flags & Style_Enabled ? cg.light() : cg.background());
			p->drawRect(r);
			p->setPen(cg.background().dark(115));
			p->drawLine(x + 1, y + 1, w - 2, y + 1);
			p->drawLine(x + 1, y + 1, x + 1, h - 2);

			if (pseudo3D && roundedCorners) {
				const QCOORD corners[] = { x, y, x, y2, x2, y, x2, y2 };
				p->setPen(cg.background());
				p->drawPoints(QPointArray(4, corners));
			}

			break;
		}

		case PE_IndicatorMask: {
			p->fillRect (r, color1);

			if (roundedCorners) {
				p->setPen (color0);
				p->drawPoint(r.topLeft());
				p->drawPoint(r.topRight());
				p->drawPoint(r.bottomLeft());
				p->drawPoint(r.bottomRight());
			}
			break;
		}

	//	RADIOBUTTONS
	//	------------
		case PE_ExclusiveIndicator: {
			const QCOORD outline[] = { 0, 7, 0, 5, 1, 4, 1, 3, 2, 2, 3, 1, 4, 1, 5, 0, 7, 0, 8, 1, 9, 1, 10, 2, 11, 3, 11, 4, 12, 5, 12, 7, 11, 8, 11, 9, 10, 10, 9, 11, 8, 11, 7, 12, 5, 12, 4, 11, 3, 11, 2, 10, 1, 9, 1, 8 };
			const QCOORD indark[] = { 1, 7, 1, 6, 1, 5, 2, 4, 2, 3, 3, 2, 4, 2, 5, 1, 6, 1, 7, 1 };

			p->fillRect(r, cg.background());
			p->setPen(cg.background().dark());
			p->setBrush(flags & Style_Enabled ? cg.light() : cg.background());
			p->drawPolygon(QPointArray(28, outline));
			p->setPen(cg.background().dark(115));
			p->drawPoints(QPointArray(10, indark));
			break;
		}

		case PE_ExclusiveIndicatorMask: {
			const QCOORD outline[] = { 0, 7, 0, 5, 1, 4, 1, 3, 2, 2, 3, 1, 4, 1, 5, 0, 7, 0, 8, 1, 9, 1, 10, 2, 11, 3, 11, 4, 12, 5, 12, 7, 11, 8, 11, 9, 10, 10, 9, 11, 8, 11, 7, 12, 5, 12, 4, 11, 3, 11, 2, 10, 1, 9, 1, 8 };
			p->fillRect(r, color0);
			p->setPen(color1);
			p->setBrush(color1);
			p->translate(r.x(), r.y());
			p->drawPolygon(QPointArray(28, outline));
			p->translate(-r.x(), -r.y());
			break;
		}

	//	GENERAL PANELS
	//	--------------
		case PE_Splitter: {
			int k;
			p->fillRect(r, cg.background());
			p->setBrush(cg.background().dark());

			if (horiz) {
				for (k = r.center().y() - 9; k < r.center().y() + 9; k += 3) {
					p->drawLine(x + 2, k, w - 2, k);
				}
			} else {
				for (k = r.center().x() - 9; k < r.center().x() + 9; k += 3) {
					p->drawLine(k, y + 2, k, h - 2);
				}
			}

			break;
		}

		case PE_PanelGroupBox:
		case PE_GroupBoxFrame: {
			p->setPen(cg.background().dark());
			p->setBrush(cg.background());
			p->drawRect(r);

			if (pseudo3D && roundedCorners) {
				const QCOORD corners[] = { x, y, x, y2, x2, y, x2, y2 };
				p->setPen(cg.background());
				p->drawPoints(QPointArray(4, corners));
			}
			break;
		}

		case PE_WindowFrame:
		case PE_Panel: {
			renderPanel(p, r, cg, sunken, false);
			break;
		}

		case PE_PanelLineEdit: {
			renderPanel(p, r, cg, true, true);
			break;
		}

		case PE_StatusBarSection: {
			renderPanel(p, r, cg, true, false);
			break;
		}

		case PE_TabBarBase:	// Still not sure what this one does
		case PE_PanelTabWidget: {
			renderPanel(p, r, cg, sunken);
			break;
		}

		case PE_PanelPopup: {
			p->setPen(cg.shadow());
			p->drawRect(r);
			break;
		}

	//	MENU / TOOLBAR PANEL
	//	--------------------
		case PE_PanelMenuBar: 			// Menu
		case PE_PanelDockWindow: {		// Toolbar
			p->setPen(cg.button());
			p->drawRect(r);
			break;
		}

	//	TOOLBAR/DOCK WINDOW HANDLE
	//	--------------------------
		case PE_DockWindowResizeHandle: {
			renderButton(p, r, cg);
			break;
		}

		case PE_DockWindowHandle: {
			int k;

			p->fillRect(r, cg.button());
			p->setPen(cg.button().dark());

			if (horiz) {
				for (k = r.center().y() - 9; k < r.center().y() + 9; k += 3) {
					p->drawLine(x + 2, k, w - 2, k);
				}
			} else {
				for (k = r.center().x() - 9; k < r.center().x() + 9; k += 3) {
					p->drawLine(k, y + 2, k, h - 2);
				}
			}

			break;
		}

	//	TOOLBAR SEPARATOR
	//	-----------------
		case PE_DockWindowSeparator: {
			break;
		}

		case PE_CheckMark: {
			int x = r.center().x() - 3, y = r.center().y() - 3;
			const QCOORD check[] = { x, y + 2, x, y + 4, x + 2, y + 6, x + 6, y + 2, x + 6, y, x + 2, y + 4 };
			const QPointArray a(6, check);

			p->setPen(flags & Style_Down ? cg.highlight() : p->pen());
			p->setBrush(flags & Style_Down ? cg.highlight() : p->pen().color());
			p->drawPolygon(a);

			break;
		}

		case PE_SpinWidgetUp:
		case PE_SpinWidgetDown:
		case PE_HeaderArrow:
		case PE_ArrowUp:
		case PE_ArrowDown:
		case PE_ArrowLeft:
		case PE_ArrowRight: {
			QPointArray a;

			switch (pe) {
				case PE_SpinWidgetUp:
				case PE_ArrowUp: {
					a.setPoints(7, u_arrow);
					break;
				}
				case PE_SpinWidgetDown:
				case PE_ArrowDown: {
					a.setPoints(7, d_arrow);
					break;
				}
				case PE_ArrowLeft: {
					a.setPoints(7, l_arrow);
					break;
				}
				case PE_ArrowRight: {
					a.setPoints(7, r_arrow);
					break;
				}
				default: {
					if (flags & Style_Up) {
						a.setPoints(7, u_arrow);
					} else {
						a.setPoints(7, d_arrow);
					}
				}
			}

			p->save();

			if (flags & Style_Down) {
				p->translate(pixelMetric(PM_ButtonShiftHorizontal),
				             pixelMetric(PM_ButtonShiftVertical));
			}

			if (flags & Style_Enabled) {
				a.translate((r.x() + r.width()/2), (r.y() + r.height()/2));
				if (p->pen() == QPen::NoPen) {
					p->setPen(cg.buttonText());
				}
				p->drawLineSegments(a, 0, 3);
				p->drawPoint(a[6]);
			} else {
				a.translate((r.x() + r.width()/2)+1, (r.y() + r.height()/2)+1);
				if (p->pen() == QPen::NoPen) {
					p->setPen(cg.highlightedText());
				}
				p->drawLineSegments(a, 0, 3);
				p->drawPoint(a[6]);

				a.translate(-1,-1);
				if (p->pen() == QPen::NoPen) {
					p->setPen(cg.mid());
				}
				p->drawLineSegments(a, 0, 3);
				p->drawPoint(a[6]);
			}

			p->restore();
			break;
		}

		default: {
			return KStyle::drawPrimitive(pe, p, r, cg, flags, opt);
		}
	}
}


void dotNETstyle::drawControl(ControlElement element,
                              QPainter *p,
                              const QWidget *widget,
                              const QRect &r,
                              const QColorGroup &cg,
                              SFlags flags,
                              const QStyleOption& opt) const
{
	switch (element) {
	//	CHECKBOXES
	//	----------
		case CE_CheckBox: {
			drawPrimitive(PE_Indicator, p, r, cg, flags);
			if (flags & Style_On || flags & Style_Down) {
				p->setPen(cg.foreground());
				drawPrimitive(PE_CheckMark, p, r, cg, flags);
			}
			break;
		}

	//	PROGRESSBAR
	//	-----------
		case CE_ProgressBarGroove: {
			renderPanel(p, r, cg, true, false);
			break;
		}

		case CE_ProgressBarContents: {
			const QProgressBar *pb = (const QProgressBar *)widget;

			if (pb->totalSteps()) {
				int x, y, w, h;
				double percent = (double)pb->progress() / (double)pb->totalSteps();

				r.rect(&x, &y, &w, &h);

				p->fillRect(x + 1, y + 1, w - 2, h - 2, cg.background().light());

				if (QApplication::reverseLayout()) {
					x += w - ((int)(w * percent));
					y += 1; h -= 2;
					w = ((int)(w * percent)) - 2;
				} else {
					x += 1; y += 1; h -= 2;
					w = (int)(w * percent) - 2;
				}

				p->fillRect(x, y, w, h, cg.highlight());

				if (pb->progress() < pb->totalSteps()) {
					p->setPen(cg.background().dark(115));
					if (QApplication::reverseLayout()) {
						p->drawLine(x - 1, y, x - 1, h);
					} else {
						p->drawLine(x + w, y, x + w, h);
					}
				}

				if (pseudo3D) {
					QPointArray corners;
					if (QApplication::reverseLayout()) {
						const QCOORD c[] = { x, h, x, y, x + w, y, x + w, h };
						corners = QPointArray(4, c);
					} else {
						const QCOORD c[] = { x, h, x, y, w, y, w, h };
						corners = QPointArray(4, c);
					}
					p->setPen(cg.highlight().dark(120));
					p->drawConvexPolygon(corners);
					p->setPen(cg.highlight().light(120));
					p->drawPolyline(corners, 0, 3);
					if (roundedCorners) {
						p->setPen(cg.background().dark());
						p->drawPoints(corners);
					}
				}
			}
			break;
		}

	//	RADIOBUTTONS
	//	------------
		case CE_RadioButton: {
			drawPrimitive(PE_ExclusiveIndicator, p, r, cg, flags);

			if (flags & Style_On || flags & Style_Down) {
				QCOORD center[] = { 4, 5, 4, 7, 5, 8, 7, 8, 8, 7, 8, 5, 7, 4, 5, 4 };
				QPointArray c(8, center);
				p->setPen(flags & Style_Down ? cg.highlight() : cg.text());
				p->setBrush(flags & Style_Down ? cg.highlight() : cg.text());
				p->drawPolygon(c);
			}
			break;
		}

	//	TABS
	//	----
		case CE_TabBarTab: {
			const QTabBar *tb = (const QTabBar *) widget;
			bool cornerWidget = false;
			QTabBar::Shape tbs = tb->shape();
			bool selected = flags & Style_Selected;
			int x, x2, y, y2, w, h;
			r.rect(&x, &y, &w, &h);
			r.coords(&x, &y, &x2, &y2);

			if (tb->parent()->inherits("QTabWidget")) {
				const QTabWidget *tw = (const QTabWidget *)tb->parent();
				QWidget *cw = tw->cornerWidget(Qt::TopLeft);
				if (cw) {
					cornerWidget = true;
				}
			}

			switch (tbs) {
				case QTabBar::RoundedAbove: {
					y2 -= 1;
					if (pseudo3D) {
						p->setPen(cg.background().dark());
						if (selected) {
							const QCOORD oCorners[] = { x, y2, x, y, x2, y, x2, y2 };
							p->drawPolyline(QPointArray(4, oCorners));

							if (roundedCorners) {
								p->setPen(cg.background());
								p->drawPoints(QPointArray(4, oCorners), 1, 2);
							}

							p->setPen(cg.background().dark(115));
						/*	Right inner border */
							p->drawLine(x + w - 2, y + 1, x + w - 2, y + h - 2);
							p->setPen(cg.background().light());
						/* Top inner border */
							p->drawLine(x + 1, y + 1, x + w - 2, y + 1);
						/* Left inner border */
							p->drawLine(x + 1, y + 1, x + 1, y + h - 1);
						} else {
							QRect r2(x+1, y+3, w-2, h-5);
							p->fillRect(r2, cg.mid().light(105));

							const QCOORD oCorners[] = { x, y2, x, y + 2, x2, y + 2, x2, y2 };
							p->drawPolyline(QPointArray(4, oCorners));

							p->setPen(cg.background());

						/*	hack where the rounded corners sometimes go away */
							if (roundedCorners) {
								p->drawPoint(x + w - 1, y + 2);
							}

							p->setPen(cg.background().light());
						/* Top of underlying tabbar-widget */
							p->drawLine(x, y + h - 1, x + w - 1, y + h - 1);
							p->setPen(cg.background().dark());
						/*	Top of underlying tabbar-widget */
							p->drawLine(x, y + h - 2, x + w - 1, y + h - 2);
						/*	Outer right border */
							p->drawLine(x + w - 1, y + 3, x + w - 1, y + h - 2);
    					}

						if (opt.tab()->identifier() == 0 && !cornerWidget) {
							p->setPen(cg.background().dark());
							p->drawPoint(x, y + h - 1);
						}
					} else {
						p->setPen(cg.background().dark());
						if (selected) {
							p->drawLine(x + w - 1, y, x + w - 1, y + h - 2);
							p->setPen(cg.background().light());
							p->drawLine(x, y, x + w - 1, y);
							p->drawLine(x, y, x, y + h - 1);
							p->setPen(cg.background().dark(115));
							p->drawLine(x + w - 2, y + 1, x + w - 2, y + h - 3);
						} else {
							QRect r2(x + 1, y + 3, w - 2, h - 4);
							p->fillRect(r2, cg.mid());

							p->setPen(cg.background().light());
							p->drawLine(x, y + 2, x + w - 1, y + 2);
							p->drawLine(x, y + 2, x, y + h - 1);
							p->drawLine(x, y + h - 1, x + w - 1, y + h - 1);

							p->setPen(cg.mid().dark(115));
							p->drawLine(x + w - 2, y + 3, x + w - 2, y + h - 3);
						}
					}

					break;
				}

				case QTabBar::RoundedBelow: {
					if (pseudo3D) {
						p->setPen(cg.background().dark());
						if (selected) {
							const QCOORD oCorners[] = { x, y + 1, x, y2, x2, y2, x2, y + 1 };
							p->drawPolyline(QPointArray(4, oCorners));

							p->setPen(cg.background().dark(115));
							p->drawLine(x + 1, y + h - 2, x + w - 2, y + h - 2);
						/*	Right inner border */
							p->drawLine(x + w - 2, y + 1, x + w - 2, y + h - 2);
							p->setPen(cg.background().light());
						/* Left inner border */
							p->drawLine(x + 1, y, x + 1, y + h - 2);
						} else {
							y2 -= 2;
							const QCOORD oCorners[] = { x, y, x, y2, x2, y2, x2, y };
							QRect r2(x + 1, y + 2, w - 2, h - 5);
							p->fillRect(r2, cg.mid().light(105));

							p->drawPolyline(QPointArray(4, oCorners));

							p->setPen(cg.mid().dark(115));
						/*	Inner right border */
							p->drawLine(x + w - 2, y + 2, x + w - 2, y + h - 4);
							p->drawLine(x + 1, y + h - 4, x + w - 2, y + h - 4);

							p->setPen(cg.background().dark(115));
						/* Top of underlying tabbar-widget */
							p->drawLine(x, y, x + w - 1, y);
							p->setPen(cg.background().dark());
						/*	Top of underlying tabbar-widget */
							p->drawLine(x, y + 1, x + w - 1, y + 1);
						/*	Outer right border */
							p->drawLine(x + w - 1, y + 3, x + w - 1, y + h - 2);

						/*	hack where the rounded corners sometimes go away */
							if (roundedCorners) {
								p->setPen(cg.background());
								p->drawPoint(x + w - 1, y + h - 3);
								p->drawPoint(x + w - 1, y + h - 2);
							}
    					}

						if (opt.tab()->identifier() == 0 && !cornerWidget) {
							p->setPen(cg.background().dark());
							p->drawPoint(x, y);
						}
					} else {
						if (selected) {
							p->setPen(cg.background().light());
							p->drawLine(x, y, x, y + h - 1);

							p->setPen(cg.background().dark());
							p->drawLine(x + w - 1, y, x + w - 1, y + h - 1);
							p->drawLine(x, y + h-1, x + w - 1, y + h - 1);
						} else {
							QRect r2(x, y + 1, w - 1, h - 4);
							p->fillRect(r2, cg.mid());

							p->setPen(cg.background().dark());
							p->drawLine(x, y, x + w - 1, y);

							p->setPen(cg.mid().dark(115));
							p->drawLine(x + w - 2, y + 1, x + w - 2, y + h - 5);
							p->drawLine(x, y + h - 4, x + w - 1, y + h - 4);
						}
					}
					break;
				}

				default:
					winstyle->drawControl(element, p, widget, r, cg, flags, opt);
			}

			break;
		}

		case CE_TabBarLabel: {
			const int text_flags = AlignVCenter | AlignHCenter | ShowPrefix |
			                       DontClip | SingleLine;

			p->save();

			if (useTextShadows) {
				p->setPen(cg.background().dark(115));
				p->drawText(r.x()+1, r.y()+1, r.width(), r.height(), text_flags, opt.tab()->text());
			}

			p->setPen(flags & Style_Enabled ? cg.foreground() : cg.mid().dark());

			if (!flags & Style_Selected) {
				p->translate(pixelMetric(PM_ButtonShiftHorizontal),
				             pixelMetric(PM_ButtonShiftVertical));
			}

			p->drawText(r, text_flags, opt.tab()->text());

			if (flags & Style_HasFocus)
				drawPrimitive(PE_FocusRect, p, r, cg);

			p->restore();
			break;
		}

		case CE_PushButton: {
			QPushButton *button = (QPushButton *)widget;
			QRect br = r;
			bool btnDefault = button->isDefault();

			static int di = pixelMetric(PM_ButtonDefaultIndicator);
			br.addCoords(di, di, -di, -di);

			if (btnDefault) {
				drawPrimitive(PE_ButtonDefault, p, r, cg, flags);
			}

			drawPrimitive(PE_ButtonBevel, p, br, cg, flags);
			break;
		}

		case CE_PushButtonLabel: {
			const QPushButton *pb = (const QPushButton *)widget;
			const bool enabled = flags & Style_Enabled;
			const int text_flags = AlignVCenter | AlignHCenter | ShowPrefix |
			                       DontClip | SingleLine;
			QRect ur(r);

			if (flags & Style_Down) {
				p->translate(pixelMetric(PM_ButtonShiftHorizontal),
				             pixelMetric(PM_ButtonShiftVertical));
			}

			if (!pb->text().isEmpty() && (flags & Style_ButtonDefault)) {
				p->setFont(QFont(p->font().family(), p->font().pointSize(), 75));
			}

			if (pb->iconSet() && !pb->iconSet()->isNull()) {
				QIconSet::Mode mode = enabled ? QIconSet::Normal : QIconSet::Disabled;
				QPixmap pixmap = pb->iconSet()->pixmap(QIconSet::Small, mode);

				if (!pb->text().isEmpty())
				{
					const int TextToIconMargin = 3;
					int length = pixmap.width() + TextToIconMargin
					              + p->fontMetrics().size(ShowPrefix, pb->text()).width();
					int offset = (r.width() - length)/2;

					p->drawPixmap( r.x() + offset, r.y() + r.height() / 2 - pixmap.height() / 2, pixmap );
					ur.addCoords(offset + pixmap.width() + TextToIconMargin, 0, -offset, 0);
				}
				else
				{
					if (!pb->pixmap())
						p->drawPixmap(r.x() + r.width()/2 - pixmap.width()/2, r.y() + r.height() / 2 - pixmap.height() / 2,
										pixmap);
					else  //icon + pixmap. Ugh.
						p->drawPixmap(r.x() + pb->isDefault() ? 8 : 4 , r.y() + r.height() / 2 - pixmap.height() / 2, pixmap);
				}
			}

			if (pb->pixmap() && !pb->text()) {
				QRect pr(0, 0, pb->pixmap()->width(), pb->pixmap()->height());
				pr.moveCenter(r.center());
				p->drawPixmap(pr.topLeft(), *pb->pixmap());
			}

			if (useTextShadows && !pb->text().isEmpty() && enabled) {
				p->setPen((flags & Style_Down ? cg.highlight().dark(135) : cg.background().dark(115)));
				p->drawText(ur.x()+1, ur.y()+1, ur.width(), ur.height(), text_flags, pb->text());
			}

			if (!pb->text().isEmpty()) {
				p->setPen(enabled ? cg.foreground() : pb->palette().disabled().buttonText());
				p->drawText(ur, text_flags, pb->text());
			}

			if ( flags & Style_HasFocus )
				drawPrimitive( PE_FocusRect, p,
				               visualRect( subRect( SR_PushButtonFocusRect, widget ), widget ),
				               cg, flags );

			break;
		}

	//	MENUBAR ITEM (sunken panel on mouse over)
	//	-----------------------------------------
		case CE_MenuBarItem: {
			QMenuItem *mi = opt.menuItem();
			bool active  = flags & Style_Active;
			bool focused = flags & Style_HasFocus;
			bool down = flags & Style_Down;
			const int text_flags =
				AlignVCenter | AlignHCenter | ShowPrefix | DontClip | SingleLine;
			int x, x2, y, y2;
			r.coords(&x, &y, &x2, &y2);

			if (active && focused) {
				p->setBrush(cg.highlight());
				p->setPen(down ? cg.highlight().light() : cg.highlight().dark());
				p->drawRect(r);
				if (pseudo3D && roundedCorners && !down) {
					const QCOORD corners[] = { x, y2, x, y, x2, y, x2, y2 };
					p->setPen(cg.background());
					p->drawPoints(QPointArray(4, corners));
				}

				if (down) {
					p->setBrush(cg.background().light());
					p->setPen(cg.background().dark());
					p->drawRect(r);
					p->setPen(cg.background().light());
					p->drawLine(x+1, y2, x2-1, y2);
					if (pseudo3D && roundedCorners) {
						p->setPen(cg.background());
						p->drawPoint(x, y);
						p->drawPoint(x2, y);
					}
				}
			}

			if (useTextShadows) {
				p->setPen((active && focused && !down ? cg.highlight().dark(135) : cg.background().dark(115)));
				p->drawText(r.x() + 1, r.y() + 1, r.width(), r.height(), text_flags, mi->text());
			}
			p->setPen((active && focused && !down ? cg.highlightedText() : cg.foreground()));
			p->drawText(r, text_flags, mi->text());
			break;
		}

	//	POPUPMENU ITEM (highlighted on mouseover)
	//	------------------------------------------
		case CE_PopupMenuItem: {
			QMenuItem *mi = opt.menuItem();

			if (!mi) {
				return;
			}

			const QPopupMenu *pum = (const QPopupMenu *) widget;
			static const int itemFrame = 2;
			static const int itemHMargin = 3;
			static const int itemVMargin = 3;
			static const int arrowHMargin = 6;
			static const int rightBorder = 20;
			const int tab = opt.tabWidth();

			int checkcol = QMAX(opt.maxIconWidth(), 26);
			int x, x2, y, y2, w, h;
			r.rect(&x, &y, &w, &h);
			r.coords(&x, &y, &x2, &y2);

			bool active = flags & Style_Active;
			bool disabled = !mi->isEnabled();
			bool checkable = pum->isCheckable();

			int xpos = x;
			int xm = itemFrame + checkcol + itemHMargin;

			if (pum->erasePixmap() && !pum->erasePixmap()->isNull()) {
				p->drawPixmap( x, y, *pum->erasePixmap(), x, y, w, h );
			} else {
				if (!QApplication::reverseLayout()) {
					p->fillRect( x, y, 22, h, cg.mid() );
					p->fillRect( x + 22, y, w - 22, h, cg.background().light() );
				} else { // i wonder why exactly +1 (diego)
					p->fillRect( w-22, y, 22+1, h, cg.mid() );
					p->fillRect( x, y, w - 22, h, cg.background().light() );
				}
			}

			if (mi->isSeparator()) {
				p->setPen(cg.mid());
				if (!QApplication::reverseLayout())
					p->drawLine(x + opt.maxIconWidth() + 6, y, x + w, y);
				else
					p->drawLine(w - (x + opt.maxIconWidth() + 6), y,
					            x, y);
				return;
			}

			if (active && !disabled) {
				p->setBrush(cg.highlight());
				p->fillRect(x + 2, y + 2, w - 4, h - 4, cg.highlight());
				p->setPen(cg.highlight().dark());
				if (pseudo3D && roundedCorners) {
					const QCOORD segments[] = { x+2, y+1, x2-2, y+1, x2-1, y+2, x2-1, y2-2, x2-2, y2-1, x+2, y2-1, x+1, y2-2, x+1, y+2 };
					const QCOORD icorners[] = { x+2, y+2, x2-2, y+2, x2-2,y2-2, x+2, y2-2 };
					p->drawLineSegments(QPointArray(8, segments));
					p->drawPoints(QPointArray(4, icorners));
				} else {
					const QCOORD corners[] = { x+1, y2-1, x+1, y+1, x2-1, y+1, x2-1, y2-1 };
					p->drawConvexPolygon(QPointArray(4, corners));
				}
				p->setPen(cg.highlightedText());
			}

			if (mi->iconSet()) {
				p->save();
				QIconSet::Mode mode =
					disabled ? QIconSet::Disabled : QIconSet::Normal;
				QPixmap pixmap = mi->iconSet()->pixmap(QIconSet::Small, mode);
				int pixw = pixmap.width();
				int pixh = pixmap.height();

				QRect cr(xpos, y, opt.maxIconWidth(), h);
				QRect pmr(0, 0, pixw, pixh);
				pmr.moveCenter(cr.center());

				if (QApplication::reverseLayout())
					pmr = visualRect( pmr, r );

				p->setPen(cg.highlightedText());
				p->drawPixmap(pmr.topLeft(), pixmap);
				p->restore();
			}

			p->setPen(active ? cg.highlightedText() : cg.buttonText());

			xpos += xm;

			if (mi->custom()) {
				int m = itemVMargin;
				p->setPen(cg.foreground());
				mi->custom()->paint(p, cg, active, !disabled, x+xm, y+m, w-xm-tab+1, h-2*m);
				return;
			} else {
				QString s = mi->text();
				if(!s.isNull()) {
					int t = s.find('\t');
					int m = itemVMargin;

					int text_flags = AlignVCenter | ShowPrefix | DontClip | SingleLine;

					if (active && !disabled) {
						p->setPen(cg.highlightedText());
					} else if (disabled) {
						p->setPen(cg.mid().light(110));
					} else {
						p->setPen(cg.text());
					}

					if (t >= 0) {
						int xp;
						xp = x + w - tab - rightBorder - itemHMargin - itemFrame + 1;

						QRect rr = QRect(xp, y+m, tab, h-(2*m));
						if (QApplication::reverseLayout())
							rr = visualRect(rr, r);

						if (useTextShadows) {
							QPen op = p->pen();
							p->setPen(active && !disabled ? cg.highlight().dark(130) : cg.background().dark(115));
							p->drawText(rr.x() + 1, rr.y() + 1, rr.width(), rr.height(), text_flags, s.mid(t+1));
							p->setPen(op);
						}

						p->drawText(rr, text_flags, s.mid(t+1));
						s = s.left(t);
					}

					QRect rr = QRect(xpos, y+m, w-xm-tab+1, h-(2*m));
					if (QApplication::reverseLayout()) {
						rr = visualRect(rr, r);
						text_flags |= AlignRight;
					}

					if (useTextShadows) {
						QPen op = p->pen();
						p->setPen(active && !disabled ? cg.highlight().dark(130) : cg.background().dark(115));
						p->drawText(rr.x() + 1, rr.y() + 1, rr.width(), rr.height(), text_flags, s);
						p->setPen(op);
					}

					p->drawText(rr, text_flags, s);
				} else if (mi->pixmap()) {
					QPixmap *pixmap = mi->pixmap();
					if (pixmap->depth() == 1) {
						p->setBackgroundMode(OpaqueMode);
					}
					p->drawPixmap(xpos, y, *pixmap);
					if (pixmap->depth() == 1) {
						p->setBackgroundMode(TransparentMode);
					}
				}
			}

			if (mi->popup()) {
				int dim = pixelMetric(PM_MenuButtonIndicator);

				xpos = x+w - arrowHMargin - 2*itemFrame - dim;

				if (active && !disabled)
					p->setPen(cg.highlightedText());
				else
					p->setPen(cg.text());


				QRect rr = QRect(xpos, y + h/2 - dim/2, dim, dim);
				if (QApplication::reverseLayout())
					rr = visualRect( rr, r );
				drawPrimitive((QApplication::reverseLayout() ? PE_ArrowLeft : PE_ArrowRight), p, rr, cg, Style_Enabled);
			}

			if (checkable) {
				if (mi->isChecked()) {
					int xp = x;

					SFlags cflags = Style_Default;

					if (!disabled)
						cflags |= Style_Enabled;
					else
						cflags |= Style_On;

					QRect rr = QRect( xp + 6, y + 6, 16, h - 6 );
					if (QApplication::reverseLayout())
						rr = visualRect( rr, r );

					drawPrimitive(PE_CheckMark, p, rr, cg, cflags);
				}
			}

			break;
		}

	// Menu and dockwindow empty space
	//
		case CE_DockWindowEmptyArea:
		case CE_MenuBarEmptyArea:
			p->fillRect(r, cg.button());
			break;
		default:
			winstyle->drawControl(element, p, widget, r, cg, flags, opt);
	}
}

void dotNETstyle::drawControlMask(ControlElement element,
                                  QPainter *p,
                                  const QWidget *w,
                                  const QRect &r,
                                  const QStyleOption &opt) const
{
	switch (element) {
		case CE_PushButton: {
			if (pseudo3D && roundedCorners) {
				int x1, y1, x2, y2;
				r.coords (&x1, &y1, &x2, &y2);
				QCOORD corners[] = { x1, y1, x2, y1, x1, y2, x2, y2 };
				p->fillRect (r, color1);
				p->setPen (color0);
				p->drawPoints (QPointArray (4, corners));
			} else {
			//	Just fill the entire thing
				p->fillRect (r, color1);
			}
			break;
		}

		default: {
			KStyle::drawControlMask (element, p, w, r, opt);
		}
	}
}

void dotNETstyle::drawComplexControlMask(ComplexControl c,
                                         QPainter *p,
                                         const QWidget *w,
                                         const QRect &r,
                                         const QStyleOption &o) const
{
	switch (c) {
		case CC_ComboBox: {
			if (pseudo3D && roundedCorners) {
				int x1, y1, x2, y2;
				r.coords (&x1, &y1, &x2, &y2);
				QCOORD corners[] = { x1, y1, x2, y1, x1, y2, x2, y2 };
				p->fillRect (r, color1);
				p->setPen (color0);
				p->drawPoints (QPointArray (4, corners));
			} else {
			//	Just fill the entire thing
				p->fillRect (r, color1);
			}
			break;
		}
		default: {
			KStyle::drawComplexControlMask (c, p, w, r, o);
		}
	}
}

void dotNETstyle::drawComplexControl(ComplexControl control,
                                     QPainter *p,
                                     const QWidget *widget,
                                     const QRect &r,
                                     const QColorGroup &cg,
                                     SFlags flags,
                                     SCFlags controls,
                                     SCFlags active,
                                     const QStyleOption& opt) const
{
	switch(control) {
	//	COMBOBOX
	//	--------
		case CC_ComboBox: {
			int x, y, w, h;
			const QComboBox *cb = (const QComboBox *)widget;
			r.rect(&x, &y, &w, &h);

			if (active & Style_Sunken)
				flags |= Style_Sunken;

			static const unsigned char downarrow_bits[] = {
				0x7f, 0xbe, 0x9c, 0x08, 0x00, 0x00, 0x00, 0x28,
				0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0xb8
			};

			static const unsigned int handle_width = 15;
			static const unsigned int handle_offset = handle_width + 1;

			QBitmap downArrow = QBitmap(7, 4, downarrow_bits, true);
			downArrow.setMask(downArrow);

			cb->editable() ? p->fillRect(x + 1, y + 1, w - 2, h - 2, cg.base()) : p->fillRect(x + 1, y + 1, w - 2, h - 2, cg.light());

			renderPanel(p, r, cg, true, true);

			p->save();
		//	Draw the box on the right.
			if (cb->listBox() && cb->listBox()->isVisible()) {
				p->setPen(cg.highlight());
				p->setBrush(cg.highlight().light());
			} else if (flags & Style_HasFocus) {
				p->setPen(cg.highlight().dark());
				p->setBrush(cg.highlight());
			} else {
				p->setPen(cg.dark());
				p->setBrush(cg.midlight());
			}

			QRect hr = (pseudo3D ? QRect(w - handle_offset - 1, y, handle_width + 2, h) : QRect(w - handle_offset - 1, y + 1, handle_width + 2, h - 1));
			if (QApplication::reverseLayout()) { hr = visualRect(hr, r); }

			p->drawRect(hr);
			p->setBrush(NoBrush);
			QRect rr = (pseudo3D ? QRect(x + 1, y + 1, w - 2, h - 2) : QRect(x + 1, y + 1, w - 1, h - 1));

			if (QApplication::reverseLayout()) { rr = visualRect( rr, r ); }

			if (flags & Style_HasFocus || cg.highlight() == cg.midlight() ||
			    (cb->listBox() && cb->listBox()->isVisible())) {
				p->drawRect(rr);
			}

			if (pseudo3D && !((active & Style_Sunken) ||
			   (cb->listBox() && cb->listBox()->isVisible()))) {
				p->save();
				p->setBrush(NoBrush);
				QColor test = ((flags & Style_HasFocus) ? cg.highlight() : cg.midlight());
				p->setPen(test.dark());
				p->drawRect(hr);
				hr.moveBy(1,1);
				hr.setSize(QSize(hr.width() -2, hr.height() -2));
				QPointArray ca(3);
				ca.setPoint(0, hr.topRight());
				ca.setPoint(1, hr.bottomRight());
				ca.setPoint(2, hr.bottomLeft());
				p->setPen(test.dark(120));
				p->drawPolyline(ca);
				p->setPen(test.light(120));
				ca.setPoint(1, hr.topLeft());
				p->drawPolyline(ca);
				p->restore();
			}

			if (pseudo3D && roundedCorners) {
				p->setPen(cg.background());
				p->drawPoint(r.topRight());
				p->drawPoint(r.bottomRight());
				p->setPen(cg.background().dark());
				hr.moveBy(-1,-1);
				hr.setSize(QSize(hr.width() + 2, hr.height() + 2));
				p->drawPoint(hr.topLeft());
				p->drawPoint(hr.bottomLeft());
			}

			if ((active && cb->hasFocus()) || (cb->listBox() && cb->listBox()->isVisible())) {
				p->setPen(cg.highlightedText());
			} else {
				p->setPen(cg.text());
			}

			QRect cr(w - handle_offset, y, handle_width, h - 2);
			QRect pmr(0, 0, 7, 4);
			pmr.moveCenter(cr.center());
			if (QApplication::reverseLayout()) {
				pmr = visualRect( pmr, r );
			}

			p->drawPixmap(pmr.topLeft(), downArrow);

			p->restore();
			break;
		}

	//	TOOLBUTTON
	//	----------
		case CC_ToolButton: {
			const QToolButton *tb = (const QToolButton *) widget;

			QRect button, menuarea;
			button   = querySubControlMetrics(control, widget, SC_ToolButton, opt);
			menuarea = querySubControlMetrics(control, widget, SC_ToolButtonMenu, opt);

			SFlags bflags = flags,
			       mflags = flags;

			if (active & SC_ToolButton)
				bflags |= Style_Down;

			if (active & SC_ToolButtonMenu)
				mflags |= Style_Down;

			if (controls & SC_ToolButton) {
			//	If we're pressed, on, or raised...
				if (bflags & (Style_Down | Style_On | Style_Raised)) {
					drawPrimitive(PE_ButtonTool, p, button, cg, bflags, opt);
				} else if (tb->parentWidget() &&
				           tb->parentWidget()->backgroundPixmap() &&
				           !tb->parentWidget()->backgroundPixmap()->isNull()) {
					QPixmap pixmap = *(tb->parentWidget()->backgroundPixmap());
					p->drawTiledPixmap( r, pixmap, tb->pos() );
				}
			}

			// Draw a toolbutton menu indicator if required
			if (controls & SC_ToolButtonMenu) {
				if (mflags & (Style_Down | Style_On | Style_Raised)) {
					drawPrimitive(PE_ButtonDropDown, p, menuarea, cg, mflags, opt);
				}
				drawPrimitive(PE_ArrowDown, p, menuarea, cg, mflags, opt);
			}

			if (tb->hasFocus() && !tb->focusProxy()) {
				QRect fr = tb->rect();
				fr.addCoords(2, 2, -2, -2);
				drawPrimitive(PE_FocusRect, p, fr, cg);
			}

			break;
		}

	//	SPINWIDGETS
	//	-----------
		case CC_SpinWidget: {
			const QSpinWidget *sw = (const QSpinWidget *) widget;
			SFlags flags;
			PrimitiveElement pe;

			QRect swf = querySubControlMetrics(control, sw, SC_SpinWidgetFrame);
			swf = visualRect( swf, sw );

			if (controls & SC_SpinWidgetFrame) {
			// Don't draw the WindowsStyle thick frame.
				renderPanel(p, swf, cg, true, false);
			}

			if (controls & SC_SpinWidgetUp) {
				flags = Style_Default | Style_Enabled;
				if (active == SC_SpinWidgetUp) {
					flags |= Style_On;
					flags |= Style_Sunken;
				} else
					flags |= Style_Raised;

				if (sw->buttonSymbols() == QSpinWidget::PlusMinus)
					pe = PE_SpinWidgetPlus;
				else
					pe = PE_SpinWidgetUp;

				QRect re = sw->upRect();
				QColorGroup ucg = sw->isUpEnabled() ? cg : sw->palette().disabled();
				p->fillRect(re.x() + 1, re.y() + 1, re.width() - 2, re.height() - 1, flags & Style_Raised ? ucg.background() : ucg.highlight());
				if (pseudo3D) {
					const QCOORD corners[] = { re.x(), re.bottom(), re.x(), re.y() + 1, re.x() + 1, re.y(), re.right() - 1, re.y(), re.right(), re.y() + 1, re.right(), re.bottom() };
					p->setPen(flags & Style_Raised ? ucg.background().dark() : ucg.highlight());
					p->drawLineSegments(QPointArray(6, corners));
					p->setPen(flags & Style_Raised ? ucg.background().light() : ucg.highlight());
					p->drawLine(re.x() + 1, re.y() + 1, re.x() + 1, re.bottom());
					p->drawLine(re.x() + 1, re.y() + 1, re.right()-1, re.y() + 1);
					p->setPen(flags & Style_Raised ? ucg.background().dark(115) : ucg.highlight());
					p->drawLine(re.right()-1, re.y() + 2, re.right()-1, re.bottom());
				} else {
				}
				re.setX(re.x() + 1);
				p->setPen(flags & Style_Raised ? ucg.foreground() : ucg.highlightedText());
				drawPrimitive(pe, p, re, ucg, flags);
			}

			if (controls & SC_SpinWidgetDown) {
				flags = Style_Default | Style_Enabled;
				if (active == SC_SpinWidgetDown) {
					flags |= Style_On;
					flags |= Style_Sunken;
				} else
					flags |= Style_Raised;

				if (sw->buttonSymbols() == QSpinWidget::PlusMinus)
					pe = PE_SpinWidgetMinus;
				else
					pe = PE_SpinWidgetDown;

				QRect re = sw->downRect();
				QColorGroup dcg = sw->isDownEnabled() ? cg : sw->palette().disabled();
				p->fillRect(re.x() + 1, re.y(), re.width() - 2, re.height() - 1, flags & Style_Raised ? dcg.background() : dcg.highlight());
				if (pseudo3D) {
					const QCOORD corners[] = {
						re.x(), re.top(),
						re.x(), re.bottom(),
						re.x() + 1, re.y() + re.height(),
						re.right() - 1, re.y() + re.height(),
						re.right(), re.bottom(),
						re.right(), re.y()
					};
					p->setPen(flags & Style_Raised ? dcg.background().dark() : dcg.highlight());
					p->drawLineSegments(QPointArray(6, corners));
					p->setPen(flags & Style_Raised ? dcg.background().light() : dcg.highlight());
					p->drawLine(re.x() + 1, re.y(), re.x() + 1, re.bottom());
					p->setPen(flags & Style_Raised ? dcg.background().dark(115) : dcg.highlight());
					p->drawLine(re.x() + 2, re.bottom(), re.right()-1, re.bottom());
					p->drawLine(re.right()-1, re.y(), re.right()-1, re.bottom());
				} else {
				}
				p->setPen(flags & Style_Raised ? dcg.foreground() : dcg.highlightedText());
				re.setX(re.x() + 1);
				drawPrimitive(pe, p, re, dcg, flags);
			}

			break;
		}

		default:
			KStyle::drawComplexControl(control, p, widget,
			                           r, cg, flags, controls,
			                           active, opt);
			break;
	}
}


QRect dotNETstyle::subRect(SubRect r, const QWidget *widget) const
{
// Don't use KStyles progressbar subrect
	switch (r) {
		case SR_ComboBoxFocusRect: {
			return querySubControlMetrics( CC_ComboBox, widget, SC_ComboBoxEditField );
		}

		case SR_PushButtonFocusRect: {
			QRect rect = widget->rect();
			int margin = pixelMetric(PM_ButtonDefaultIndicator, widget) + pixelMetric(PM_DefaultFrameWidth, widget) + 2;

			rect.addCoords(margin, margin, -margin, -margin);
			return rect;
		}

		case SR_ProgressBarGroove:
		case SR_ProgressBarContents:
		case SR_ProgressBarLabel: {
			return winstyle->subRect(r, widget);
		}
		default: {
			return KStyle::subRect(r, widget);
		}
	}
}

QRect dotNETstyle::querySubControlMetrics(ComplexControl control,
                                          const QWidget *widget,
                                          SubControl subcontrol,
                                          const QStyleOption &opt) const
{
	if (!widget) {
		return QRect();
	}

	QRect r(widget->rect());
	switch (control) {
		case CC_ComboBox: {
			switch (subcontrol) {
				case SC_ComboBoxEditField: {
					return QRect(r.x() + 2, r.y() + 2, r.width() - 19, r.height() - 4);
				}
				default: {
					return KStyle::querySubControlMetrics(control, widget, subcontrol, opt);
				}
			}
			break;
		}

		case CC_SpinWidget: {
			int fw = pixelMetric(PM_SpinBoxFrameWidth, widget);
			QSize bs;
			bs.setHeight(QMAX(8, widget->height()/2));
			bs.setWidth(QMIN(bs.height() * 8 / 5, widget->width() / 4));

			int y = fw;
			int x, lx, rx;
			x = widget->width() - y - bs.width();
			lx = fw;
			rx = x - fw;

			switch (subcontrol) {
				case SC_SpinWidgetUp: {
					return QRect(x, y-1, bs.width(), bs.height());
				}
				case SC_SpinWidgetDown: {
					return QRect(x, y + bs.height()-1, bs.width(), bs.height());
				}
				case SC_SpinWidgetFrame: {
					return QRect(0, 0, widget->width() - (bs.width() + 2), widget->height());
				}
				case SC_SpinWidgetEditField: {
					return QRect(lx, fw, widget->width() - (bs.width() + 4), widget->height() - 2 * fw);
				}
				case SC_SpinWidgetButtonField: {
					return QRect(x, y, bs.width(), widget->height() - 2 * fw);
				}
				default: {
				}
			}
			break;
		}
		default: {
		}
	}
	return KStyle::querySubControlMetrics(control, widget, subcontrol, opt);
}

int dotNETstyle::pixelMetric(PixelMetric m, const QWidget *widget) const
{
	switch(m) {
	//	TABS
	//	----
		case PM_TabBarTabVSpace: {
			const QTabBar * tb = (const QTabBar *) widget;
			if (tb->shape() == QTabBar::RoundedAbove ||
			    tb->shape() == QTabBar::RoundedBelow)
				return 12;
			else
				return 0;
		}

		case PM_TabBarTabOverlap: {
			const QTabBar* tb = (const QTabBar*)widget;
			if (tb->shape() != QTabBar::RoundedAbove &&
			    tb->shape() != QTabBar::RoundedBelow)
				return 3;                   // Leave standard size alone
			else
				return 1;                   // Change size for our tabs only
		}

	//	SPLITTERS
	//	---------
		case PM_SplitterWidth: {
			return 6;
		}

	//	PROGRESSBARS
	//	------------
		case PM_ProgressBarChunkWidth:
			return 9;

	//	SLIDER
	//	------
		case PM_SliderLength:
			return 11;

	//	MENU INDICATOR
	//	--------------
		case PM_MenuButtonIndicator:
			return 8;

	//	HEADER INDICATOR
	//	----------------
		case PM_HeaderMarkSize:
			return 1;

	//	CHECKBOXES / RADIO BUTTONS
	//	--------------------------
		case PM_ExclusiveIndicatorWidth:    // Radiobutton size
		case PM_ExclusiveIndicatorHeight:   // 13x13
		case PM_IndicatorWidth:             // Checkbox size
		case PM_IndicatorHeight:            // 13x13
			return 13;

	//	FRAMES
	//	------
		case PM_SpinBoxFrameWidth:
			return 1;

		case PM_MenuBarFrameWidth:
			return 0;

		case PM_DefaultFrameWidth: {
			if (!pseudo3D) {
				return 1;
			} else {
				if (widget &&
				   (widget->inherits("QPopupMenu") ||
				    widget->inherits("QMenuBar") ||
				    widget->inherits("QRangeControl") ||
				    widget->inherits("QScrollView"))) {
					return 1;
				} else {
					return 2;
				}
			}
		}

		case PM_ButtonDefaultIndicator: {
			return 3;
		}

		case PM_ButtonShiftVertical:
		case PM_ButtonShiftHorizontal: {
			return 1;
		}

		default:
			return KStyle::pixelMetric(m, widget);
	}
}


QSize dotNETstyle::sizeFromContents(ContentsType t,
                                    const QWidget *widget,
                                    const QSize &s,
                                    const QStyleOption &opt) const
{
	switch (t) {

		case CT_PopupMenuItem: {
			if (!widget || opt.isDefault())
				break;

			const QPopupMenu *popup = (const QPopupMenu *)widget;
			QMenuItem *mi = opt.menuItem();
			int maxpmw = opt.maxIconWidth();
			int w = s.width(), h = s.height();
			bool checkable = popup->isCheckable();

			if (mi->custom()) {
				w = mi->custom()->sizeHint().width();
				h = mi->custom()->sizeHint().height();
			} else if (mi->widget()) {
			// don't change the size in this case.
			} else if (mi->isSeparator()) {
				w = 10;
				h = 1;
			} else {
				if (mi->pixmap()) {
					h = QMAX(h, mi->pixmap()->height() + 8);
				}

				h = QMAX(h, widget->fontMetrics().height() + 10);

				if (mi->iconSet()) {
					h = QMAX(h, mi->iconSet()->pixmap(QIconSet::Small, QIconSet::Normal).height() + 8);
				}
			}

			if (!mi->text().isNull()) {
				if ((mi->text().find('\t') >= 0) || mi->popup()) {
					w += 20;
				}
			}

			if (maxpmw) {
				w += maxpmw;
			}
			if (checkable && maxpmw < 20) {
				w += 20 - maxpmw;
			}
			if (checkable || maxpmw > 0) {
				w += 2;
			}

			w += 20;

			return QSize(w, h);
		}

		case CT_PushButton: {
			const QPushButton* button = (const QPushButton*) widget;
			int w  = s.width();
			int h  = s.height();
			int bm = pixelMetric( PM_ButtonMargin, widget );
			int fw = pixelMetric( PM_DefaultFrameWidth, widget ) * 2;

			//We use bold font sometimes, so that make take more space than Qt allocated.
			QFontMetrics origFM = button->fontMetrics();
			QFontMetrics boldFM = QFontMetrics(QFont(button->font().family(), button->font().pointSize(), 75));
			int origTextWidth = origFM.size(ShowPrefix, button->text()).width();
			int boldTextWidth = boldFM.size(ShowPrefix, button->text()).width();

			if (boldTextWidth > origTextWidth) //this might not happen in particular when text is empty
				w += boldTextWidth - origTextWidth;

			w += bm + fw;
			h += bm + fw;

			// Ensure we stick to standard width and heights.
			if ( button->isDefault() || button->autoDefault() ) {
				if ( w < 80 && !button->pixmap() )
					w = 80;
			}

			// Compensate for default indicator. Doing it for all widgets lets us
			// workaround changes in auto-default behavior in KDialogBase in 3.3
			int di = pixelMetric( PM_ButtonDefaultIndicator );
			w += di * 2;
			h += di * 2;

			if ( h < 22 )
				h = 22;
			return QSize( w, h );
		}

		case CT_ComboBox: {
			int arrow = 21;
			return QSize(s.width() + arrow, QMAX(s.height() + 4, 16));
		}

		default:
			return KStyle::sizeFromContents (t, widget, s, opt);
	}

	return KStyle::sizeFromContents (t, widget, s, opt);
//	return winstyle->sizeFromContents(t, widget, s, opt);
}

void dotNETstyle::paletteChanged()
{
	QComboBox *combo  = 0L;
	QMenuBar *menuBar = 0L;
	QToolBar *toolBar = 0L;
   QWidget  *widget  = 0L;

	QValueListIterator<QWidget*> it = m_widgets.begin();
	for ( ; it != m_widgets.end(); ++it ) {
		widget = *it;
		if ((combo = dynamic_cast<QComboBox*>( widget )))
			updatePalette( combo );
		else if ((toolBar = dynamic_cast<QToolBar*>( widget )))
			updatePalette( toolBar );
		else if ((menuBar = dynamic_cast<QMenuBar*>( widget )))
			updatePalette( menuBar );
	}
}

void dotNETstyle::updatePalette( QComboBox *combo )
{
	QPalette pal = QApplication::palette();
	pal.setColor(QColorGroup::Dark,
	             pal.active().color(QColorGroup::Base));
	pal.setColor(QColorGroup::Midlight,
	             pal.active().color(QColorGroup::Background));
	combo->setPalette(pal);
}

void dotNETstyle::updatePalette( QToolBar *bar )
{
	QPalette pal = QApplication::palette();
	pal.setColor(QColorGroup::Button,
	             pal.active().color(QColorGroup::Background));
	bar->setPalette(pal);
}

void dotNETstyle::updatePalette( QMenuBar *bar )
{
	QPalette pal = QApplication::palette();
	pal.setColor(QColorGroup::Button,
	             pal.active().color(QColorGroup::Background));
	bar->setPalette(pal);
}

void dotNETstyle::slotDestroyed()
{
	m_widgets.remove( (QWidget*) sender() );
}

bool dotNETstyle::eventFilter(QObject *obj, QEvent *ev)
{
	if (obj->inherits("QComboBox")) {
		if (ev->type() == QEvent::Enter) {
			QWidget *btn = (QWidget *)obj;
			if (btn->isEnabled()) {
				QPalette pal = btn->palette();
				pal.setColor(QColorGroup::Dark,
				             pal.active().color(QColorGroup::Highlight).dark());
				pal.setColor(QColorGroup::Midlight,
				             pal.active().color(QColorGroup::Highlight));
				btn->setPalette(pal);
			}
		} else if (ev->type() == QEvent::Leave) {
			QWidget *btn = (QWidget *)obj;
			QPalette pal = btn->palette();
			pal.setColor(QColorGroup::Dark,
			             pal.active().color(QColorGroup::Base));
			pal.setColor(QColorGroup::Midlight,
			             pal.active().color(QColorGroup::Background));
			btn->setPalette(pal);
		}
	} else if (obj->inherits("QButton")) {
		QWidget *btn = (QWidget *)obj;
		QPalette pal = btn->palette();
		pal.setColor(QColorGroup::Button,
		             pal.active().color(QColorGroup::Background));
		btn->setPalette(pal);
	}

	return false;
}

// vim: noet ts=4 sw=4
