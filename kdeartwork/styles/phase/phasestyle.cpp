//////////////////////////////////////////////////////////////////////////////
// phasestyle.cpp
// -------------------
// Qt/KDE widget style
// -------------------
// Copyright (c) 2004 David Johnson
// Please see the header file for copyright and license information.
//////////////////////////////////////////////////////////////////////////////
//
// Some miscellaneous notes
//
// Reimplemented scrollbar metric and drawing routines from KStyle to allow
// better placement of subcontrols. This is because the subcontrols slightly
// overlap to share part of their border.
//
// Menu and toolbars are painted with the background color by default. This
// differs from the Qt default of giving them PaletteButton backgrounds.
// Menubars have normal gradients, toolbars have reverse.
//
// Some toolbars are not part of a QMainWindows, such as in a KDE file dialog.
// In these cases we treat the toolbar as "floating" and paint it flat.
//
//////////////////////////////////////////////////////////////////////////////

#include <kdrawutil.h>
#include <kpixmap.h>
#include <kpixmapeffect.h>

#include <qapplication.h>
#include <qintdict.h>
#include <qpainter.h>
#include <qpointarray.h>
#include <qsettings.h>
#include <qstyleplugin.h>

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qheader.h>
#include <qmainwindow.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qscrollbar.h>
#include <qslider.h>
#include <qsplitter.h>
#include <qtabbar.h>
#include <qtabwidget.h>
#include <qtoolbar.h>
#include <qtoolbox.h>
#include <qtoolbutton.h>

#include "phasestyle.h"
#include "bitmaps.h"

static const char* QSPLITTERHANDLE    = "QSplitterHandle";
static const char* QTOOLBAREXTENSION  = "QToolBarExtensionWidget";
static const char* KTOOLBARWIDGET     = "kde toolbar widget";

// some convenient constants
static const int ITEMFRAME       = 1; // menu stuff
static const int ITEMHMARGIN     = 3;
static const int ITEMVMARGIN     = 0;
static const int ARROWMARGIN     = 6;
static const int RIGHTBORDER     = 6;
static const int MINICONSIZE     = 16;
static const int CHECKSIZE       = 9;
static const int MAXGRADIENTSIZE = 64;

static unsigned contrast = 110;


//////////////////////////////////////////////////////////////////////////////
// Construction, Destruction, Initialization                                //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PhaseStyle()
// -----------
// Constructor

PhaseStyle::PhaseStyle()
    : KStyle(FilledFrameWorkaround | AllowMenuTransparency,
             ThreeButtonScrollBar), hover_(0), hovertab_(0),
      gradients_(QPixmap::defaultDepth() > 8), kicker_(false)
{
    QSettings settings;
    if (gradients_) { // don't bother setting if already false
        gradients_ =
            settings.readBoolEntry("/phasestyle/Settings/gradients", true);
        contrast = 100 + settings.readNumEntry("/Qt/KDE/contrast", 5);
    }
    highlights_ =
        settings.readBoolEntry("/phasestyle/Settings/highlights", true);

    gradients = new QMap<unsigned int, QIntDict<GradientSet> >;

    reverse_ = QApplication::reverseLayout();

    // create bitmaps
    uarrow = QBitmap(6, 6, uarrow_bits, true);
    uarrow.setMask(uarrow);
    darrow = QBitmap(6, 6, darrow_bits, true);
    darrow.setMask(darrow);
    larrow = QBitmap(6, 6, larrow_bits, true);
    larrow.setMask(larrow);
    rarrow = QBitmap(6, 6, rarrow_bits, true);
    rarrow.setMask(rarrow);
    bplus = QBitmap(6, 6, bplus_bits, true);
    bplus.setMask(bplus);
    bminus = QBitmap(6, 6, bminus_bits, true);
    bminus.setMask(bminus);
    bcheck = QBitmap(9, 9, bcheck_bits, true);
    bcheck.setMask(bcheck);
    dexpand = QBitmap(9, 9, dexpand_bits, true);
    dexpand.setMask(dexpand);
    rexpand = QBitmap(9, 9, rexpand_bits, true);
    rexpand.setMask(rexpand);
    doodad_mid = QBitmap(4, 4, doodad_mid_bits, true);
    doodad_light = QBitmap(4, 4, doodad_light_bits, true);
}

PhaseStyle::~PhaseStyle() 
{
    delete gradients;
    gradients = 0;
}

//////////////////////////////////////////////////////////////////////////////
// Polishing                                                                //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// polish()
// --------
// Initialize application specific

void PhaseStyle::polish(QApplication* app)
{
    if (!qstrcmp(app->argv()[0], "kicker")) kicker_ = true;
}

//////////////////////////////////////////////////////////////////////////////
// polish()
// --------
// Initialize the appearance of a widget

void PhaseStyle::polish(QWidget *widget)
{
    if (::qt_cast<QMenuBar*>(widget) ||
        ::qt_cast<QPopupMenu*>(widget)) {
        // anti-flicker optimization
        widget->setBackgroundMode(NoBackground);
    } else if (::qt_cast<QFrame*>(widget) ||
               widget->inherits(QTOOLBAREXTENSION) ||
               (!qstrcmp(widget->name(), KTOOLBARWIDGET))) {
        // needs special handling on paint events
        widget->installEventFilter(this);
    } else if (highlights_ &&
               (::qt_cast<QPushButton*>(widget) ||
                ::qt_cast<QComboBox*>(widget) ||
                ::qt_cast<QSpinWidget*>(widget) ||
                ::qt_cast<QCheckBox*>(widget) ||
                ::qt_cast<QRadioButton*>(widget) ||
                ::qt_cast<QSlider*>(widget) ||
                widget->inherits(QSPLITTERHANDLE))) {
        // mouseover highlighting
        widget->installEventFilter(this);
    } else if (highlights_ && ::qt_cast<QTabBar*>(widget)) {
        // highlighting needing mouse tracking
        widget->setMouseTracking(true);
        widget->installEventFilter(this);
    }

    KStyle::polish(widget);
}

//////////////////////////////////////////////////////////////////////////////
// polish()
// --------
// Initialize the palette

void PhaseStyle::polish(QPalette &pal)
{
    // clear out gradients on a color change
    gradients->clear();

    // lighten up a bit, so the look is not so "crisp"
    if (QPixmap::defaultDepth() > 8) { // but not on low color displays
        pal.setColor(QPalette::Disabled, QColorGroup::Dark,
            pal.color(QPalette::Disabled, QColorGroup::Dark).light(contrast));
        pal.setColor(QPalette::Active, QColorGroup::Dark,
            pal.color(QPalette::Active, QColorGroup::Dark).light(contrast));
        pal.setColor(QPalette::Inactive, QColorGroup::Dark,
            pal.color(QPalette::Inactive, QColorGroup::Dark).light(contrast));
    }

    QStyle::polish(pal);
}

//////////////////////////////////////////////////////////////////////////////
// unPolish()
// ----------
// Undo the initialization of a widget's appearance

void PhaseStyle::unPolish(QWidget *widget)
{
    if (::qt_cast<QMenuBar*>(widget) ||
        ::qt_cast<QPopupMenu*>(widget)) {
        widget->setBackgroundMode(PaletteBackground);
    } else if (::qt_cast<QFrame*>(widget) ||
               widget->inherits(QTOOLBAREXTENSION) ||
               (!qstrcmp(widget->name(), KTOOLBARWIDGET))) {
        widget->removeEventFilter(this);
    } else if (highlights_ && // highlighting
               (::qt_cast<QPushButton*>(widget) ||
                ::qt_cast<QComboBox*>(widget) ||
                ::qt_cast<QSpinWidget*>(widget) ||
                ::qt_cast<QCheckBox*>(widget) ||
                ::qt_cast<QRadioButton*>(widget) ||
                ::qt_cast<QSlider*>(widget) ||
                widget->inherits(QSPLITTERHANDLE))) {
        widget->removeEventFilter(this);
    } else if (highlights_ && ::qt_cast<QTabBar*>(widget)) {
        widget->setMouseTracking(false);
        widget->removeEventFilter(this);
    }

    KStyle::unPolish(widget);
}

//////////////////////////////////////////////////////////////////////////////
// Drawing                                                                  //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// drawPhaseGradient()
// ------------------
// Draw gradient

void PhaseStyle::drawPhaseGradient(QPainter *painter,
                                   const QRect &rect,
                                   QColor color,
                                   bool horizontal,
                                   int px, int py,
                                   int pw, int ph,
                                   bool reverse) const
{
    if (!gradients_) {
        painter->fillRect(rect, color);
        return;
    }

    // px, py, pw, ph used for parent-relative pixmaps
    int size;
    if (horizontal)
        size = (pw > 0) ? pw : rect.width();
    else
        size = (ph > 0) ? ph : rect.height();

    if (size > MAXGRADIENTSIZE) { // keep it sensible
        painter->fillRect(rect, color);
    } else {
        // lazy allocation
        GradientSet *set = (*gradients)[color.rgb()][size];
        if (!set) {
            set = new GradientSet(color, size);
            (*gradients)[color.rgb()].setAutoDelete(true);
            (*gradients)[color.rgb()].insert(size, set);
        }
        painter->drawTiledPixmap(rect, *set->gradient(horizontal, reverse),
                                 QPoint(px, py));
    }
}

//////////////////////////////////////////////////////////////////////////////
// drawPhaseBevel()
// ----------------
// Draw the basic Phase bevel

void PhaseStyle::drawPhaseBevel(QPainter *painter,
				int x, int y, int w, int h,
				const QColorGroup &group,
				const QColor &fill,
				bool sunken,
				bool horizontal,
				bool reverse) const
{
    int x2 = x + w - 1;
    int y2 = y + h - 1;
    painter->save();

    painter->setPen(group.dark());
    painter->drawRect(x, y, w, h);

    painter->setPen(sunken ? group.mid() : group.midlight());
    painter->drawLine(x+1, y+1, x2-2, y+1);
    painter->drawLine(x+1, y+2, x+1, y2-2);

    painter->setPen(sunken ? group.midlight() : group.mid());
    painter->drawLine(x+2, y2-1, x2-1, y2-1);
    painter->drawLine(x2-1, y+2, x2-1, y2-2);

    painter->setPen(group.button());
    painter->drawPoint(x+1, y2-1);
    painter->drawPoint(x2-1, y+1);

    if (sunken) {
        // sunken bevels don't get gradients
        painter->fillRect(x+2, y+2, w-4, h-4, fill);
    } else {
        drawPhaseGradient(painter, QRect(x+2, y+2, w-4, h-4), fill,
                          horizontal, 0, 0, w-4, h-4, reverse);
    }
    painter->restore();
}

//////////////////////////////////////////////////////////////////////////////
// drawPhaseButton()
// ----------------
// Draw the basic Phase button

void PhaseStyle::drawPhaseButton(QPainter *painter,
                                 int x, int y, int w, int h,
                                 const QColorGroup &group,
                                 const QColor &fill,
                                 bool sunken) const
{
    int x2 = x + w - 1;
    int y2 = y + h - 1;

    painter->setPen(group.midlight());
    painter->drawLine(x+1, y2, x2, y2);
    painter->drawLine(x2, y+1, x2, y2-1);

    painter->setPen(group.mid());
    painter->drawLine(x, y,  x2-1, y);
    painter->drawLine(x, y+1, x, y2-1);

    painter->setPen(group.button());
    painter->drawPoint(x, y2);
    painter->drawPoint(x2, y);

    drawPhaseBevel(painter, x+1, y+1, w-2, h-2, group, fill,
                   sunken, false, false);
}

//////////////////////////////////////////////////////////////////////////////
// drawPhasePanel()
// ----------------
// Draw the basic Phase panel

void PhaseStyle::drawPhasePanel(QPainter *painter,
                                int x, int y, int w, int h,
                                const QColorGroup &group,
                                bool sunken,
                                const QBrush *fill) const
{
    int x2 = x + w - 1;
    int y2 = y + h - 1;
    painter->save();

    if (sunken) {
        painter->setPen(group.dark());
        painter->drawRect(x+1, y+1, w-2, h-2);
        painter->setPen(group.midlight());
        painter->drawLine(x+1, y2, x2, y2);
        painter->drawLine(x2, y+1, x2, y2-1);
        painter->setPen(group.mid());
        painter->drawLine(x, y, x, y2-1);
        painter->drawLine(x+1, y, x2-1, y);
        painter->setPen(group.background());
        painter->drawPoint(x, y2);
        painter->drawPoint(x2, y);
    } else {
        painter->setPen(group.dark());
        painter->drawRect(x, y, w, h);
        painter->setPen(group.midlight());
        painter->drawLine(x+1, y+1, x2-2, y+1);
        painter->drawLine(x+1, y+2, x+1, y2-2);
        painter->setPen(group.mid());
        painter->drawLine(x+2, y2-1, x2-1, y2-1);
        painter->drawLine(x2-1, y+2, x2-1, y2-2);
        painter->setPen(group.background());
        painter->drawPoint(x+1, y2-1);
        painter->drawPoint(x2-1, y+1);
    }

    if (fill) {
        painter->fillRect(x+2, y+2, w-4, h-4, fill->color());
    }
    painter->restore();
}

//////////////////////////////////////////////////////////////////////////////
// drawPhaseTab()
// -------------
// Draw a Phase style tab

void PhaseStyle::drawPhaseTab(QPainter *painter,
                              int x, int y, int w, int h,
                              const QColorGroup &group,
                              const QTabBar *bar,
                              const QStyleOption &option,
                              SFlags flags) const
{
    const QTabWidget *tabwidget;
    bool selected = (flags & Style_Selected);
    bool edge; // tab is at edge of bar
    const int x2 = x + w - 1;
    const int y2 = y + h - 1;

    painter->save();

    // what position is the tab?
    if ((bar->count() == 1)
	|| (bar->indexOf(option.tab()->identifier()) == 0)) {
	edge = true;
    } else {
	edge = false;
    }

    switch (QTabBar::Shape(bar->shape())) {
      case QTabBar::RoundedAbove:
      case QTabBar::TriangularAbove: {
	  // is there a corner widget?
	  tabwidget = ::qt_cast<QTabWidget*>(bar->parent());
	  if (edge && tabwidget
	      && tabwidget->cornerWidget(reverse_ ?
					 Qt::TopRight : Qt::TopLeft)) {
	      edge = false;
	  }

          if (!selected) { // shorten
              y += 2; h -= 2;
          }
          if (selected) {
              painter->setPen(Qt::NoPen);
              painter->fillRect(x+1, y+1, w-1, h-1,
                                group.brush(QColorGroup::Background));
          } else {
              drawPhaseGradient(painter, QRect(x+1, y+1, w-1, h-2),
                                (flags & Style_MouseOver)
                                ? group.background()
                                : group.background().dark(contrast),
                                false, 0, 0, 0, h*2, false);
          }

          // draw tab
          painter->setPen(group.dark());
          painter->drawLine(x, y, x, y2-2);
          painter->drawLine(x+1, y, x2, y);
          painter->drawLine(x2, y+1, x2, y2-2);

          painter->setPen(group.mid());
          painter->drawLine(x2-1, y+2, x2-1, y2-2);

          painter->setPen(group.midlight());
          painter->drawLine(x+1, y+1, x2-2, y+1);
          if ((selected) || edge) painter->drawLine(x+1, y+2, x+1, y2-2);

          // finish off bottom
          if (selected) {
              painter->setPen(group.dark());
              painter->drawPoint(x, y2-1);
              painter->drawPoint(x2, y2-1);

              painter->setPen(group.midlight());
              painter->drawPoint(x, y2);
              painter->drawLine(x+1, y2-1, x+1, y2);
              painter->drawPoint(x2, y2);

              painter->setPen(group.mid());
              painter->drawPoint(x2-1, y2-1);

              if (!reverse_ && edge) {
                  painter->setPen(group.dark());
                  painter->drawLine(x, y2-1, x, y2);
                  painter->setPen(group.midlight());
                  painter->drawPoint(x+1, y2);
              }
          } else {
              painter->setPen(group.dark());
              painter->drawLine(x, y2-1, x2, y2-1);

              painter->setPen(group.midlight());
              painter->drawLine(x, y2, x2, y2);

              if (!reverse_ && edge) {
                  painter->setPen(group.dark());
                  painter->drawLine(x, y2-1, x, y2);
              }
          }
          if (reverse_ && edge) {
              painter->setPen(group.dark());
              painter->drawPoint(x2, y2);
              painter->setPen(selected ? group.mid() : group.background());
              painter->drawPoint(x2-1, y2);
          }
          break;
      }

      case QTabBar::RoundedBelow:
      case QTabBar::TriangularBelow: {
	  // is there a corner widget?
	  tabwidget = ::qt_cast<QTabWidget*>(bar->parent());
	  if (edge && tabwidget
	      && tabwidget->cornerWidget(reverse_ ?
					 Qt::BottomRight : Qt::BottomLeft)) {
	      edge = false;
	  }

          painter->setBrush((selected || (flags & Style_MouseOver))
                            ? group.background()
                            : group.background().dark(contrast));
          painter->setPen(Qt::NoPen);
          painter->fillRect(x+1, y+1, w-1, h-1, painter->brush());

          // draw tab
          painter->setPen(group.dark());
          painter->drawLine(x, y+1, x, y2);
          painter->drawLine(x+1, y2, x2, y2);
          painter->drawLine(x2, y+1, x2, y2-1);

          painter->setPen(group.mid());
          painter->drawLine(x2-1, y+1, x2-1, y2-1);
          painter->drawLine(x+2, y2-1, x2-1, y2-1);
          painter->drawPoint(x, y);
          painter->drawPoint(x2, y);

          if ((selected) || edge) {
              painter->setPen(group.midlight());
              painter->drawLine(x+1, y+1, x+1, y2-2);
          }

          // finish off top
          if (selected) {
              if (!reverse_ && edge) {
                  painter->setPen(group.dark());
                  painter->drawPoint(x, y);
                  painter->setPen(group.midlight());
                  painter->drawPoint(x+1, y);
              }
          } else {
              painter->setPen(group.dark());
              painter->drawLine(x, y+1, x2, y+1);

              painter->setPen(group.mid());
              painter->drawLine(x, y, x2, y);

              if (!reverse_ && edge) {
                  painter->setPen(group.dark());
                  painter->drawPoint(x, y);
              }
          }
          if (reverse_ && edge) {
              painter->setPen(group.dark());
              painter->drawPoint(x2, y);
              painter->setPen(group.mid());
              painter->drawPoint(x2-1, y);
          }
          break;
      }
    }
    painter->restore();
}

//////////////////////////////////////////////////////////////////////////////
// drawPrimitive()
// ---------------
// Draw the primitive element

void PhaseStyle::drawPrimitive(PrimitiveElement element,
                               QPainter *painter,
                               const QRect &rect,
                               const QColorGroup &group,
                               SFlags flags,
                               const QStyleOption &option) const
{
    // common locals
    bool down      = flags & Style_Down;
    bool on        = flags & Style_On;
    bool depress   = (down || on);
    bool enabled   = flags & Style_Enabled;
    bool horiz     = flags & Style_Horizontal;
    bool mouseover = highlights_ && (flags & Style_MouseOver);
    int x, y, w, h, x2, y2, n, cx, cy;
    QPointArray parray;
    QWidget* widget;

    rect.rect(&x, &y, &w, &h);
    x2 = rect.right();
    y2 = rect.bottom();

    switch(element) {
      case PE_ButtonBevel:
      case PE_ButtonDefault:
      case PE_ButtonDropDown:
      case PE_ButtonTool:
          drawPhaseBevel(painter, x,y,w,h, group, group.button(),
                         depress, false, false);
          break;

      case PE_ButtonCommand:
              drawPhaseButton(painter, x, y, w, h, group,
                              mouseover ?
                              group.button().light(contrast) :
                              group.button(), depress);
          break;

      case PE_FocusRect: {
          QPen old = painter->pen();
          painter->setPen(group.highlight().dark(contrast));

          painter->drawRect(rect);

          painter->setPen(old);
          break;
      }

      case PE_HeaderSection: {
          // covers kicker taskbar buttons and menu titles
          QHeader* header = dynamic_cast<QHeader*>(painter->device());
          widget =dynamic_cast<QWidget*>(painter->device());

          if (header) {
              horiz = (header->orientation() == Horizontal);
          } else {
              horiz = true;
          }

          if ((widget) && ((widget->inherits("QPopupMenu")) ||
                           (widget->inherits("KPopupTitle")))) {
              // kicker/kdesktop menu titles
              drawPhaseBevel(painter, x,y,w,h,
                             group, group.background(), depress, !horiz);
          } else if (kicker_) {
              // taskbar buttons (assuming no normal headers used in kicker)
              if (depress) {
                  painter->setPen(group.dark());
                  painter->setBrush(group.brush(QColorGroup::Mid));
                  painter->drawRect(x-1, y-1, w+1, h+1);
              }
              else {
                  drawPhaseBevel(painter, x-1, y-1, w+1, h+1,
                                 group, group.button(), false, !horiz, true);
              }
          } else {
              // other headers
              if (depress) {
                  painter->setPen(group.dark());
                  painter->setBrush(group.brush(QColorGroup::Mid));
                  painter->drawRect(x-1, y-1, w+1, h+1);
              }
              else {
                  drawPhaseBevel(painter, x-1, y-1, w+1, h+1, group,
                                 group.background(), false, !horiz, true);
              }
          }
          break;
      }

      case PE_HeaderArrow:
          if (flags & Style_Up)
              drawPrimitive(PE_ArrowUp, painter, rect, group, Style_Enabled);
          else
              drawPrimitive(PE_ArrowDown, painter, rect, group, Style_Enabled);
          break;

      case PE_ScrollBarAddPage:
      case PE_ScrollBarSubPage:
          if (h) { // has a height, thus visible
              painter->fillRect(rect, group.mid());
              painter->setPen(group.dark());
              if (horiz) { // vertical
                  painter->drawLine(x, y, x2, y);
                  painter->drawLine(x, y2, x2, y2);
              } else { // horizontal
                  painter->drawLine(x, y, x, y2);
                  painter->drawLine(x2, y, x2, y2);
              }
          }
          break;

      case PE_ScrollBarAddLine:
      case PE_ScrollBarSubLine: {
          drawPhaseBevel(painter, x, y, w, h,
                         group, group.button(), down, !horiz, true);

          PrimitiveElement arrow = ((horiz) ?
                                    ((element == PE_ScrollBarAddLine) ?
                                     PE_ArrowRight : PE_ArrowLeft) :
                                    ((element == PE_ScrollBarAddLine) ?
                                     PE_ArrowDown : PE_ArrowUp));
          if (down) { // shift arrow
              switch (arrow) {
                case PE_ArrowRight: x++; break;
                case PE_ArrowLeft:  x--; break;
                case PE_ArrowDown:  y++; break;
                case PE_ArrowUp:    y--; break;
                default: break;
              }
          }

          drawPrimitive(arrow, painter, QRect(x,y,h,w), group, flags);
          break;
      }

      case PE_ScrollBarSlider:
          drawPhaseBevel(painter, x, y, w, h, group, group.button(),
                         false, !horiz, true);
          // draw doodads
          cx = x + w/2 - 2; cy = y + h/2 - 2;
          if (horiz && (w >=20)) {
              for (n = -5; n <= 5; n += 5) {
                  kColorBitmaps(painter, group, cx+n, cy,
                                0, &doodad_mid, &doodad_light, 0, 0, 0);
              }
          } else if (!horiz && (h >= 20)) {
              for (n = -5; n <= 5; n += 5) {
                  kColorBitmaps(painter, group, cx, cy+n,
                                0, &doodad_mid, &doodad_light, 0, 0, 0);
              }
          }
          break;

      case PE_Indicator:
          drawPhasePanel(painter, x+1, y+1, w-2, h-2, group, true, enabled ?
                         &group.brush(QColorGroup::Base) :
                         &group.brush(QColorGroup::Background));

          if (on) {
              painter->setPen(mouseover
                              ? group.highlight().dark(contrast)
                              : group.dark());
              painter->drawRect(x+4, y+4, w-8, h-8);
              painter->fillRect(x+5, y+5, w-10, h-10,
                                group.brush(QColorGroup::Highlight));
          } else if (mouseover) {
              painter->setPen(group.highlight().dark(contrast));
              painter->drawRect(x+4, y+4, w-8, h-8);
          }
          break;

      case PE_IndicatorMask:
          painter->fillRect(x+1, y+1, w-2, h-2, Qt::color1);
          painter->setPen(Qt::color0);
          break;

      case PE_ExclusiveIndicator: {
          // note that this requires an even size from pixelMetric
          cx = (x + x2) / 2;
          cy = (y + y2) / 2;

          painter->setBrush(enabled
                            ? group.brush(QColorGroup::Base)
                            : group.brush(QColorGroup::Background));

          painter->setPen(group.dark());
          parray.putPoints(0, 8,
                           x+1,cy+1, x+1,cy,    cx,y+1,    cx+1,y+1,
                           x2-1,cy,  x2-1,cy+1, cx+1,y2-1, cx,y2-1);
          painter->drawConvexPolygon(parray, 0, 8);

          painter->setPen(group.mid());
          parray.putPoints(0, 4, x,cy, cx,y, cx+1,y, x2,cy);
          painter->drawPolyline(parray, 0, 4);
          painter->setPen(group.midlight());
          parray.putPoints(0, 4, x2,cy+1, cx+1,y2, cx,y2, x,cy+1);
          painter->drawPolyline(parray, 0, 4);

          if (on) {
              painter->setBrush(group.brush(QColorGroup::Highlight));
              painter->setPen(mouseover
                              ? group.highlight().dark(contrast)
                              : group.dark());
              parray.putPoints(0, 8,
                               x+4,cy+1, x+4,cy,    cx,y+4,    cx+1,y+4,
                               x2-4,cy,  x2-4,cy+1, cx+1,y2-4, cx,y2-4);
              painter->drawConvexPolygon(parray, 0, 8);
          } else if (mouseover) {
              painter->setPen(group.highlight().dark(contrast));
              parray.putPoints(0, 9,
                               x+4,cy+1, x+4,cy,    cx,y+4,    cx+1,y+4,
                               x2-4,cy,  x2-4,cy+1, cx+1,y2-4, cx,y2-4,
                               x+4,cy+1);
              painter->drawPolyline(parray, 0, 9);
          }
          break;
      }

      case PE_ExclusiveIndicatorMask:
          cx = (x + x2) / 2;
          cy = (y + y2) / 2;
          painter->setBrush(Qt::color1);
          painter->setPen(Qt::color1);
          parray.putPoints(0, 8,
                           x,cy+1, x,cy,    cx,y,    cx+1,y,
                           x2,cy,  x2,cy+1, cx+1,y2, cx,y2);
          painter->drawConvexPolygon(parray, 0, 8);
          painter->setPen(Qt::color0);
          break;

      case PE_DockWindowResizeHandle:
          drawPhasePanel(painter, x, y, w, h, group, false,
                         &group.brush(QColorGroup::Background));
          break;

      case PE_Splitter:
          cx = x + w/2 - 2; cy = y + h/2 - 2;
          painter->fillRect(rect,
                            (hover_ == painter->device())
                            ? group.background().light(contrast)
                            : group.background());

          if (!horiz && (w >=20)) {
              for (n = -5; n <= 5; n += 5) {
                  kColorBitmaps(painter, group, cx+n, cy,
                                0, &doodad_mid, &doodad_light, 0, 0, 0);
              }
          } else if (horiz && (h >= 20)) {
              for (n = -5; n <= 5; n += 5) {
                  kColorBitmaps(painter, group, cx, cy+n,
                                0, &doodad_mid, &doodad_light, 0, 0, 0);
              }
          }
          break;

      case PE_Panel:
      case PE_PanelLineEdit:
      case PE_PanelTabWidget:
      case PE_TabBarBase:
          drawPhasePanel(painter, x, y, w, h, group, flags & Style_Sunken);
          break;

      case PE_PanelPopup:
      case PE_WindowFrame:
          drawPhasePanel(painter, x, y, w, h, group, false);
          break;

      case PE_GroupBoxFrame:
      case PE_PanelGroupBox:
          painter->setPen(group.dark());
          painter->drawRect(rect);
          break;

      case PE_Separator:
          painter->setPen(group.dark());
          if (w < h)
              painter->drawLine(w/2, y, w/2, y2);
          else
              painter->drawLine(x, h/2, x2, h/2);
          break;

      case PE_StatusBarSection:
          painter->setPen(group.mid());
          painter->drawLine(x, y,  x2-1, y);
          painter->drawLine(x, y+1, x, y2-1);
          painter->setPen(group.midlight());
          painter->drawLine(x+1, y2, x2, y2);
          painter->drawLine(x2, y+1, x2, y2-1);
          break;

      case PE_PanelMenuBar:
      case PE_PanelDockWindow:
          if (kicker_ && (w == 2)) { // kicker handle separator
              painter->setPen(group.mid());
              painter->drawLine(x, y,  x, y2);
              painter->setPen(group.midlight());
              painter->drawLine(x+1, y,  x+1, y2);
          } else if (kicker_ && (h == 2)) { // kicker handle separator
              painter->setPen(group.mid());
              painter->drawLine(x, y,  x2, y);
              painter->setPen(group.midlight());
              painter->drawLine(x, y+1,  x2, y+1);
          } else {
              --x; --y; ++w; ++h; // adjust rect so we can use bevel
              drawPhaseBevel(painter, x, y, w, h,
                             group, group.background(), false, (w < h),
                             (element==PE_PanelDockWindow) ? true : false);
          }
          break;

      case PE_DockWindowSeparator: {
          widget = dynamic_cast<QWidget*>(painter->device());
          bool flat = true;

          if (widget && widget->parent() &&
              widget->parent()->inherits("QToolBar")) {
              QToolBar *toolbar = ::qt_cast<QToolBar*>(widget->parent());
              if (toolbar) {
                  // toolbar not floating or in a QMainWindow
                  flat = flatToolbar(toolbar);
              }
          }

          if (flat)
              painter->fillRect(rect, group.background());
          else
              drawPhaseGradient(painter, rect, group.background(),
                                !(horiz), 0, 0, -1, -1, true);

          if (horiz) {
              int cx = w/2 - 1;
              painter->setPen(group.mid());
              painter->drawLine(cx, 0, cx, h-2);
              if (!flat) painter->drawLine(0, h-1, w-1, h-1);

              painter->setPen(group.midlight());
              painter->drawLine(cx+1, 0, cx+1, h-2);
          } else {
              int cy = h/2 - 1;
              painter->setPen(group.mid());
              painter->drawLine(0, cy, w-2, cy);
              if (!flat) painter->drawLine(w-1, 0, w-1, h-1);

              painter->setPen(group.midlight());
              painter->drawLine(0, cy+1, w-2, cy+1);
          }
          break;
      }

      case PE_SizeGrip: {
          int sw = QMIN(h, w) - 1;
          y = y2 - sw;

          if (reverse_) {
              x2 = x + sw;
              for (int n = 0; n < 4; ++n) {
                  painter->setPen(group.dark());
                  painter->drawLine(x, y, x2, y2);
                  painter->setPen(group.midlight());
                  painter->drawLine(x, y+1, x2-1, y2);
                  y += 3;;
                  x2 -= 3;;
              }
          } else {
              x = x2 - sw;
              for (int n = 0; n < 4; ++n) {
                  painter->setPen(group.dark());
                  painter->drawLine(x, y2, x2, y);
                  painter->setPen(group.midlight());
                  painter->drawLine(x+1, y2, x2, y+1);
                  x += 3;
                  y += 3;
              }
          }

          break;
      }

      case PE_CheckMark:
          painter->setPen(group.text());
          painter->drawPixmap(x+w/2-4, y+h/2-4, bcheck);
          break;

      case PE_SpinWidgetPlus:
      case PE_SpinWidgetMinus:
          if (enabled)
              painter->setPen((flags & Style_Sunken)
                              ? group.midlight() : group.dark());
          else
              painter->setPen(group.mid());
          painter->drawPixmap(x+w/2-3, y+h/2-3,
                              (element==PE_SpinWidgetPlus) ? bplus : bminus);
          break;

      case PE_SpinWidgetUp:
      case PE_ArrowUp:
          if (flags & Style_Enabled)
              painter->setPen((flags & Style_Sunken)
                              ? group.midlight() : group.dark());
          else
              painter->setPen(group.mid());
          painter->drawPixmap(x+w/2-3, y+h/2-3, uarrow);
          break;

      case PE_SpinWidgetDown:
      case PE_ArrowDown:
          if (enabled) painter->setPen((flags & Style_Sunken)
                                       ? group.midlight() : group.dark());
          else painter->setPen(group.mid());
          painter->drawPixmap(x+w/2-3, y+h/2-3, darrow);
          break;

      case PE_ArrowLeft:
          if (enabled) painter->setPen((flags & Style_Sunken)
                                       ? group.midlight() : group.dark());
          else painter->setPen(group.mid());
          painter->drawPixmap(x+w/2-3, y+h/2-3, larrow);
          break;

      case PE_ArrowRight:
          if (enabled) painter->setPen((flags & Style_Sunken)
                                       ? group.midlight() : group.dark());
          else painter->setPen(group.mid());
          painter->drawPixmap(x+w/2-3, y+h/2-3, rarrow);
          break;

      default:
          KStyle::drawPrimitive(element, painter, rect, group, flags, option);
    }
}

//////////////////////////////////////////////////////////////////////////////
// drawKStylePrimitive()
// ---------------------
// Draw the element

void PhaseStyle::drawKStylePrimitive(KStylePrimitive element,
                                     QPainter *painter,
                                     const QWidget *widget,
                                     const QRect &rect,
                                     const QColorGroup &group,
                                     SFlags flags,
                                     const QStyleOption &option) const
{
    bool horiz = flags & Style_Horizontal;
    int x, y, w, h, x2, y2, n, cx, cy;

    rect.rect(&x, &y, &w, &h);
    x2 = rect.right();
    y2 = rect.bottom();
    cx = x + w/2;
    cy = y + h/2;

    switch (element) {
      case KPE_ToolBarHandle:
          cx-=2; cy-=2;
          drawPhaseGradient(painter, rect, group.background(),
                            !horiz, 0, 0, w-1, h-1, true);
          if (horiz) {
              for (n = -5; n <= 5; n += 5) {
                  kColorBitmaps(painter, group, cx, cy+n,
                                0, &doodad_mid, &doodad_light, 0, 0, 0);
              }
              painter->setPen(group.mid());
              painter->drawLine(x, y2, x2, y2);
          } else {
              for (n = -5; n <= 5; n += 5) {
                  kColorBitmaps(painter, group, cx+n, cy,
                                0, &doodad_mid, &doodad_light, 0, 0, 0);
              }
              painter->setPen(group.mid());
              painter->drawLine(x2, y, x2, y2);
          }
          break;

      //case KPE_DockWindowHandle:
      case KPE_GeneralHandle:
          cx-=2; cy-=2;
          painter->fillRect(rect, group.brush(QColorGroup::Background));

          if (horiz) {
              for (n = -5; n <= 5; n += 5) {
                  kColorBitmaps(painter, group, cx, cy+n,
                                0, &doodad_mid, &doodad_light, 0, 0, 0);
              }
          } else {
              for (n = -5; n <= 5; n += 5) {
                  kColorBitmaps(painter, group, cx+n, cy,
                                0, &doodad_mid, &doodad_light, 0, 0, 0);
              }
          }
          break;

      case KPE_ListViewExpander:
          painter->setPen(group.mid());
          if (flags & Style_On) {
              painter->drawPixmap(x+w/2-4, y+h/2-4, rexpand);
          } else {
              painter->drawPixmap(x+w/2-4, y+h/2-4, dexpand);
          }
          break;

      case KPE_ListViewBranch:
          painter->setPen(group.mid());
          if (flags & Style_Horizontal) {
              painter->drawLine(x, cy, x2, cy);
          } else {
              painter->drawLine(cx, y, cx, y2);
          }
          break;

      case KPE_SliderGroove: {
          const QSlider* slider = ::qt_cast<const QSlider*>(widget);
          if (slider) {
              if (slider->orientation() == Horizontal) {
                  y = cy - 3;
                  h = 7;
              } else {
                  x = cx - 3;
                  w = 7;
              }
          }
          drawPhasePanel(painter, x, y, w, h, group, true,
                         &group.brush(QColorGroup::Mid));
          break;
      }

      case KPE_SliderHandle: {
          const QSlider* slider = ::qt_cast<const QSlider*>(widget);
          if (slider) {
              QColor color = (widget==hover_)
                  ? group.button().light(contrast)
                  : group.button();
              if (slider->orientation() == Horizontal) {
                  drawPhaseBevel(painter, cx-5, y, 6, h, group, color,
                                 false, false,  false);
                  drawPhaseBevel(painter, cx, y, 6, h, group, color,
                                 false, false, false);
              } else {
                  drawPhaseBevel(painter, x, cy-5, w, 6, group, color,
                                 false, true, false);
                  drawPhaseBevel(painter, x, cy, w, 6, group, color,
                                 false, true, false);
              }
          }
          break;
      }

      default:
          KStyle::drawKStylePrimitive(element, painter, widget, rect,
                                      group, flags, option);
    }
}

//////////////////////////////////////////////////////////////////////////////
// drawControl()
// -------------
// Draw the control

void PhaseStyle::drawControl(ControlElement element,
                             QPainter *painter,
                             const QWidget *widget,
                             const QRect &rect,
                             const QColorGroup &group,
                             SFlags flags,
                             const QStyleOption &option) const
{
    bool active, enabled, depress;
    int x, y, w, h, x2, y2, dx;
    QMenuItem *mi;
    QIconSet::Mode mode;
    QIconSet::State state;
    QPixmap pixmap;

    rect.rect(&x, &y, &w, &h);
    x2 = rect.right();
    y2 = rect.bottom();

    switch (element) {
      case CE_PushButton: {
          depress = flags & (Style_Down | Style_On);
          int bd = pixelMetric(PM_ButtonDefaultIndicator, widget) + 1;

          if ((flags & Style_ButtonDefault) && !depress) {
              drawPhasePanel(painter, x, y, w, h, group, true,
                             &group.brush(QColorGroup::Mid));
              drawPhaseBevel(painter, x+bd, y+bd, w-bd*2, h-bd*2, group,
                             (widget==hover_)
                             ? group.button().light(contrast)
                             : group.button(),
                             false, false, false);
          } else {
              drawPhaseButton(painter, x, y, w, h, group,
                              (widget==hover_)
                              ? group.button().light(contrast)
                              : group.button(), depress);
          }

          if (flags & Style_HasFocus) { // draw focus
              drawPrimitive(PE_FocusRect, painter,
                            subRect(SR_PushButtonFocusRect, widget),
                            group, flags);
          }
          break;
      }

      case CE_PushButtonLabel: {
          const QPushButton* button = ::qt_cast<const QPushButton*>(widget);
          if (!button) {
              KStyle::drawControl(element, painter, widget, rect, group,
                                  flags, option);
              return;
          }
          active = button->isOn() || button->isDown();

          if (active) { // shift contents
              x++; y++;
              flags |= Style_Sunken;
          }

          if (button->isMenuButton()) { // menu indicator
              int dx = pixelMetric(PM_MenuButtonIndicator, widget);
              drawPrimitive(PE_ArrowDown, painter,
                            QRect(x+w-dx-2, y+2, dx, h-4),
                            group, flags, option);
              w -= dx;
          }

          if (button->iconSet() && !button->iconSet()->isNull()) { // draw icon
              if (button->isEnabled()) {
                  if (button->hasFocus()) {
                      mode = QIconSet::Active;
                  } else {
                      mode = QIconSet::Normal;
                  }
              } else {
                  mode = QIconSet::Disabled;
              }

              if (button->isToggleButton() && button->isOn()) {
                  state = QIconSet::On;
              } else {
                  state = QIconSet::Off;
              }

              pixmap = button->iconSet()->pixmap(QIconSet::Small, mode, state);
              if (button->text().isEmpty() && !button->pixmap()) {
                  painter->drawPixmap(x+w/2 - pixmap.width()/2,
                                      y+h/2 - pixmap.height()/2, pixmap);
              } else {
                  painter->drawPixmap(x+4, y+h/2 - pixmap.height()/2, pixmap);
              }
              x += pixmap.width() + 4;
              w -= pixmap.width() + 4;
          }

          if (active || button->isDefault()) { // default button
              for(int n=0; n<2; n++) {
                  drawItem(painter, QRect(x+n, y, w, h),
                           AlignCenter | ShowPrefix,
                           button->colorGroup(),
                           button->isEnabled(),
                           button->pixmap(),
                           button->text(), -1,
                           (button->isEnabled()) ?
                           &button->colorGroup().buttonText() :
                           &button->colorGroup().mid());
              }
          } else { // normal button
              drawItem(painter, QRect(x, y, w, h),
                       AlignCenter | ShowPrefix,
                       button->colorGroup(),
                       button->isEnabled(),
                       button->pixmap(),
                       button->text(), -1,
                       (button->isEnabled()) ?
                       &button->colorGroup().buttonText() :
                       &button->colorGroup().mid());
          }
          break;
      }

      case CE_CheckBoxLabel:
      case CE_RadioButtonLabel: {
          const QButton *b = ::qt_cast<const QButton*>(widget);
          if (!b) return;

          int alignment = reverse_ ? AlignRight : AlignLeft;
          drawItem(painter, rect, alignment | AlignVCenter | ShowPrefix,
                   group, flags & Style_Enabled, b->pixmap(), b->text());

          // only draw focus if content (forms on html won't)
          if ((flags & Style_HasFocus) && ((!b->text().isNull()) || b->pixmap())) {
              drawPrimitive(PE_FocusRect, painter,
                            visualRect(subRect(SR_RadioButtonFocusRect,
                                               widget), widget),
                            group, flags);
          }
          break;
      }

      case CE_DockWindowEmptyArea:  {
          const QToolBar *tb = ::qt_cast<const QToolBar*>(widget);
          if (tb) {
              // toolbar not floating or in a QMainWindow
              if (flatToolbar(tb)) {
                  if (tb->backgroundMode() == PaletteButton)
                      // force default button color to background color
                      painter->fillRect(rect, group.background());
                  else
                      painter->fillRect(rect, tb->paletteBackgroundColor());
              }
          }
          break;
      }

      case CE_MenuBarEmptyArea:
          drawPhaseGradient(painter, QRect(x, y, w, h), group.background(),
                            (w<h), 0, 0, 0, 0, false);
          break;

      case CE_MenuBarItem: {
          const QMenuBar *mbar = ::qt_cast<const QMenuBar*>(widget);
          if (!mbar) {
              KStyle::drawControl(element, painter, widget, rect, group,
                                  flags, option);
              return;
          }
          mi = option.menuItem();
          QRect prect = mbar->rect();

          if ((flags & Style_Active) && (flags & Style_HasFocus)) {
              if (flags & Style_Down) {
                  drawPhasePanel(painter, x, y, w, h, group, true,
                                 &group.brush(QColorGroup::Background));
              } else {
                  drawPhaseBevel(painter, x, y, w, h,
                                 group, group.background(),
                                 false, false, false);
              }
          } else {
              drawPhaseGradient(painter, rect, group.background(), false, x, y,
                                prect.width()-2, prect.height()-2, false);
          }
          drawItem(painter, rect,
                   AlignCenter | AlignVCenter |
                   DontClip | ShowPrefix | SingleLine,
                   group, flags & Style_Enabled,
                   mi->pixmap(), mi->text());
          break;
      }

      case CE_PopupMenuItem: {
          const QPopupMenu *popup = ::qt_cast<const QPopupMenu*>(widget);
          if (!popup) {
              KStyle::drawControl(element, painter, widget, rect, group,
                                  flags, option);
              return;
          }

          mi = option.menuItem();
          if (!mi) {
              painter->fillRect(rect, group.button());
              break;
          }

          int tabwidth   = option.tabWidth();
          int checkwidth = option.maxIconWidth();
          bool checkable = popup->isCheckable();
          bool etchtext  = styleHint(SH_EtchDisabledText);
          active         = flags & Style_Active;
          enabled        = mi->isEnabled();
          QRect vrect;

          if (checkable) checkwidth = QMAX(checkwidth, 20);

          // draw background
          if (active && enabled) {
              painter->fillRect(x, y, w, h, group.highlight());
          } else if (widget->erasePixmap() &&
                     !widget->erasePixmap()->isNull()) {
              painter->drawPixmap(x, y, *widget->erasePixmap(), x, y, w, h);
          } else {
              painter->fillRect(x, y, w, h, group.background());
          }

          // draw separator
          if (mi->isSeparator()) {
              painter->setPen(group.dark());
              painter->drawLine(x+checkwidth+1, y+1, x2-checkwidth-1, y+1);
              painter->setPen(group.mid());
              painter->drawLine(x+checkwidth, y, x2-checkwidth-1, y);
              painter->drawPoint(x+checkwidth, y+1);
              painter->setPen(group.midlight());
              painter->drawLine(x+checkwidth+1, y2, x2-checkwidth, y2);
              painter->drawPoint(x2-checkwidth, y2-1);
              break;
          }

          // draw icon
          if (mi->iconSet() && !mi->isChecked()) {
              if (active)
                  mode = enabled ? QIconSet::Active : QIconSet::Disabled;
              else
                  mode = enabled ? QIconSet::Normal : QIconSet::Disabled;

              pixmap = mi->iconSet()->pixmap(QIconSet::Small, mode);
              QRect pmrect(0, 0, pixmap.width(), pixmap.height());
              vrect = visualRect(QRect(x, y, checkwidth, h), rect);
              pmrect.moveCenter(vrect.center());
              painter->drawPixmap(pmrect.topLeft(), pixmap);
          }

          // draw check
          if (mi->isChecked()) {
              int cx = reverse_ ? x+w - checkwidth : x;
              drawPrimitive(PE_CheckMark, painter,
                            QRect(cx + ITEMFRAME, y + ITEMFRAME,
                                  checkwidth - ITEMFRAME*2, h - ITEMFRAME*2),
                            group, Style_Default |
                            (active ? Style_Enabled : Style_On));
          }

          // draw text
          int xm = ITEMFRAME + checkwidth + ITEMHMARGIN;
          int xp = reverse_ ?
              x + tabwidth + RIGHTBORDER + ITEMHMARGIN + ITEMFRAME - 1 :
              x + xm;
          int offset = reverse_ ? -1 : 1;
          int tw = w - xm - tabwidth - ARROWMARGIN - ITEMHMARGIN * 3
              - ITEMFRAME + 1;

          painter->setPen(enabled ? (active ? group.highlightedText() :
                                     group.buttonText()) : group.mid());

          if (mi->custom()) { // draws own label
              painter->save();
              if (etchtext && !enabled && !active) {
                  painter->setPen(group.light());
                  mi->custom()->paint(painter, group, active, enabled,
                                      xp+offset, y+ITEMVMARGIN+1,
                                      tw, h-2*ITEMVMARGIN);
                  painter->setPen(group.mid());
              }
              mi->custom()->paint(painter, group, active, enabled,
                                  xp, y+ITEMVMARGIN, tw, h-2*ITEMVMARGIN);
              painter->restore();
          }
          else { // draw label
              QString text = mi->text();

              if (!text.isNull()) {
                  int t = text.find('\t');
                  int tflags = AlignVCenter | DontClip |
                      ShowPrefix |  SingleLine |
                      (reverse_ ? AlignRight : AlignLeft);

                  if (t >= 0) {
                      int tabx = reverse_ ?
                          x + RIGHTBORDER + ITEMHMARGIN + ITEMFRAME :
                          x + w - tabwidth - RIGHTBORDER - ITEMHMARGIN
                          - ITEMFRAME;

                      // draw right label (accerator)
                      if (etchtext && !enabled) { // etched
                          painter->setPen(group.light());
                          painter->drawText(tabx+offset, y+ITEMVMARGIN+1,
                                            tabwidth, h-2*ITEMVMARGIN,
                                            tflags, text.mid(t+1));
                          painter->setPen(group.mid());
                      }
                      painter->drawText(tabx, y+ITEMVMARGIN,
                                        tabwidth, h-2*ITEMVMARGIN,
                                        tflags, text.mid(t+1));
                      text = text.left(t);
                  }

                  // draw left label
                  if (etchtext && !enabled) { // etched
                      painter->setPen(group.light());
                      painter->drawText(xp+offset, y+ITEMVMARGIN+1,
                                        tw, h-2*ITEMVMARGIN,
                                        tflags, text, t);
                      painter->setPen(group.mid());
                  }
                  painter->drawText(xp, y+ITEMVMARGIN,
                                    tw, h-2*ITEMVMARGIN,
                                    tflags, text, t);
              }
              else if (mi->pixmap()) { // pixmap as label
                  pixmap = *mi->pixmap();
                  if (pixmap.depth() == 1)
                      painter->setBackgroundMode(OpaqueMode);

                  dx = ((w - pixmap.width()) / 2) + ((w - pixmap.width()) % 2);
                  painter->drawPixmap(x+dx, y+ITEMFRAME, pixmap);

                  if (pixmap.depth() == 1)
                      painter->setBackgroundMode(TransparentMode);
              }
          }

          if (mi->popup()) { // draw submenu arrow
              PrimitiveElement arrow = reverse_ ? PE_ArrowLeft : PE_ArrowRight;
              int dim = (h-2*ITEMFRAME) / 2;
              vrect = visualRect(QRect(x + w - ARROWMARGIN - ITEMFRAME - dim,
                                       y + h / 2 - dim / 2, dim, dim), rect);
              drawPrimitive(arrow, painter, vrect, group,
                            enabled ? Style_Enabled : Style_Default);
          }
          break;
      }

      case CE_TabBarTab: {
          const QTabBar* tab = ::qt_cast<const QTabBar*>(widget);
          if (tab) {
              if ((widget == hover_) && (option.tab() == hovertab_)) {
                  flags |= Style_MouseOver;
              }
              // this guy can get complicated, we we do it elsewhere
              drawPhaseTab(painter, x, y, w, h, group, tab, option,
                           flags);
          } else { // not a tabbar
              KStyle::drawControl(element, painter, widget, rect, group,
                                  flags, option);
              return;
          }
          break;
      }

      case CE_ProgressBarGroove: {
          drawPhasePanel(painter, x, y, w, h, group, true,
                         &group.brush(QColorGroup::Base));
          break;
      }

      case CE_ProgressBarContents: {
          const QProgressBar* pbar = ::qt_cast<const QProgressBar*>(widget);
          if (!pbar) {
              KStyle::drawControl(element, painter, widget, rect, group,
                                  flags, option);
              return;
          }
          subRect(SR_ProgressBarContents, widget).rect(&x, &y, &w, &h);

          painter->setBrush(group.brush(QColorGroup::Highlight));
          painter->setPen(group.dark());

          if (!pbar->totalSteps()) {
              // busy indicator
              int bar = pixelMetric(PM_ProgressBarChunkWidth, widget) + 2;
              int progress = pbar->progress() % ((w-bar) * 2);
              if (progress > (w-bar)) progress = 2 * (w-bar) - progress;
              painter->drawRect(x+progress+1, y+1, bar-2, h-2);
          } else {
              double progress = static_cast<double>(pbar->progress()) /
                  static_cast<double>(pbar->totalSteps());
              dx = static_cast<int>(w * progress);
              if (dx < 4) break;
              if (reverse_) x += w - dx;
              painter->drawRect(x+1, y+1, dx-2, h-2);
          }
          break;
      }

#if (QT_VERSION >= 0x030200)
      case CE_ToolBoxTab: {
          const QToolBox *box = ::qt_cast<const QToolBox*>(widget);
          if (!box) {
              KStyle::drawControl(element, painter, widget, rect, group,
                                  flags, option);
              return;
          }

          const int rx = x2 - 20;
          const int cx = rx - h + 1;

          QPointArray parray(6);
          parray.putPoints(0, 6,
                           x-1,y, cx,y, rx-2,y2-2, x2+1,y2-2,
                           x2+1,y2+2, x-1,y2+2);

          if (box->currentItem() && (flags & Style_Selected)) {
              painter->setPen(group.dark());
              painter->setBrush(box->currentItem()->paletteBackgroundColor());
              painter->drawConvexPolygon(parray, 0, 6);
              painter->setBrush(NoBrush);
          } else {
              painter->setClipRegion(parray, QPainter::CoordPainter);
              drawPhaseGradient(painter, rect,
                                group.background(),
                                false, 0, 0, 0, h*2, false);
              painter->setClipping(false);
              painter->drawPolyline(parray, 0, 4);
          }

          parray.putPoints(0, 4, x,y+1, cx,y+1, rx-2,y2-1, x2,y2-1);
          painter->setPen(group.midlight());
          painter->drawPolyline(parray, 0, 4);

          break;
      }
#endif

      default:
          KStyle::drawControl(element, painter, widget, rect, group,
                              flags, option);
    }
}

//////////////////////////////////////////////////////////////////////////////
// drawControlMask()
// -----------------
// Draw a bitmask for the element

void PhaseStyle::drawControlMask(ControlElement element,
                                 QPainter *painter,
                                 const QWidget *widget,
                                 const QRect &rect,
                                 const QStyleOption &option) const
{
    switch (element) {
      case CE_PushButton:
          painter->fillRect(rect, Qt::color1);
          painter->setPen(Qt::color0);
          break;

      default:
          KStyle::drawControlMask(element, painter, widget, rect, option);
    }
}

//////////////////////////////////////////////////////////////////////////////
// drawComplexControl()
// --------------------
// Draw a complex control

void PhaseStyle::drawComplexControl(ComplexControl control,
                                    QPainter *painter,
                                    const QWidget *widget,
                                    const QRect &rect,
                                    const QColorGroup &group,
                                    SFlags flags,
                                    SCFlags controls,
                                    SCFlags active,
                                    const QStyleOption &option) const
{
    bool down = flags & Style_Down;
    bool on = flags & Style_On;
    bool raised = flags & Style_Raised;
    bool sunken = flags & Style_Sunken;
    QRect subrect;
    int x, y, w, h, x2, y2;
    rect.rect(&x, &y, &w, &h);

    switch (control) {
      case CC_ComboBox: {
          const QComboBox * combo = ::qt_cast<const QComboBox*>(widget);
          if (!combo) {
              KStyle::drawComplexControl(control, painter, widget, rect, group,
                                         flags, controls, active, option);
              return;
          }

          sunken = (active == SC_ComboBoxArrow);
          drawPhaseButton(painter, x, y, w, h, group,
                          (widget==hover_)
                          ? group.button().light(contrast)
                          : group.button(), sunken);

          if (controls & SC_ComboBoxArrow) { // draw arrow box
              subrect = visualRect(querySubControlMetrics(CC_ComboBox, widget,
                            SC_ComboBoxArrow), widget);

              subrect.rect(&x, &y, &w, &h);
              int slot = QMAX(h/4, 6) + (h % 2);
              drawPhasePanel(painter, x+3, y+(h/2)-(slot/2), w-6,
                             slot, group, true,
                             sunken ? &group.brush(QColorGroup::Midlight)
                             : &group.brush(QColorGroup::Mid));
          }

          if (controls & SC_ComboBoxEditField) { // draw edit box
              if (combo->editable()) { // editable box
                  subrect = visualRect(querySubControlMetrics(CC_ComboBox,
                                widget, SC_ComboBoxEditField), widget);
                  x2 = subrect.right(); y2 = subrect.bottom();
                  painter->setPen(group.dark());
                  painter->drawLine(x2+1, y, x2+1, y2);
                  painter->setPen(group.midlight());
                  painter->drawLine(x2+2, y, x2+2, y2-1);
                  painter->setPen(group.button());
                  painter->drawPoint(x2+2, y2);
              } else if (combo->hasFocus()) { // non editable box
                  subrect = visualRect(subRect(SR_ComboBoxFocusRect,
                                               combo), widget);
                  drawPrimitive(PE_FocusRect, painter, subrect, group,
                                Style_FocusAtBorder,
                                QStyleOption(group.highlight()));
              }
          }

          painter->setPen(group.buttonText()); // for subsequent text
          break;
      }

      case CC_ScrollBar: {
          // always a three button scrollbar
          const QScrollBar *sb = ::qt_cast<const QScrollBar*>(widget);
          if (!sb) {
              KStyle::drawComplexControl(control, painter, widget, rect, group,
                                         flags, controls, active, option);
              return;
          }

          QRect srect;
          bool horizontal = (sb->orientation() == Qt::Horizontal);
          SFlags scrollflags = (horizontal ? Style_Horizontal : Style_Default);

          if (sb->minValue() == sb->maxValue()) scrollflags |= Style_Default;
          else scrollflags |= Style_Enabled;

          // addline
          if (controls & SC_ScrollBarAddLine) {
              srect = querySubControlMetrics(control, widget,
                                             SC_ScrollBarAddLine, option);
              if (srect.isValid())
                  drawPrimitive(PE_ScrollBarAddLine, painter, srect, group,
                                scrollflags | ((active == SC_ScrollBarAddLine)
                                               ? Style_Down : Style_Default));
          }

          // subline (two of them)
          if (controls & SC_ScrollBarSubLine) {
              // top/left subline
              srect = querySubControlMetrics(control, widget,
                                             SC_ScrollBarSubLine, option);
              if (srect.isValid())
                  drawPrimitive(PE_ScrollBarSubLine, painter, srect, group,
                                scrollflags | ((active == SC_ScrollBarSubLine)
                                               ? Style_Down : Style_Default));
              // bottom/right subline
              srect = querySubControlMetrics(control, widget,
                                             SC_ScrollBarAddLine, option);
              if (srect.isValid()) {
                  if (horizontal) srect.moveBy(-srect.width()+1, 0);
                  else srect.moveBy(0, -srect.height()+1);
                  drawPrimitive(PE_ScrollBarSubLine, painter, srect, group,
                                scrollflags | ((active == SC_ScrollBarSubLine)
                                               ? Style_Down : Style_Default));
              }
          }

          // addpage
          if (controls & SC_ScrollBarAddPage) {
              srect = querySubControlMetrics(control, widget,
                                             SC_ScrollBarAddPage, option);
              if (srect.isValid()) {
                  if (horizontal) srect.addCoords(1, 0, 1, 0);
                  else srect.addCoords(0, 1, 0, 1);
                  drawPrimitive(PE_ScrollBarAddPage, painter, srect, group,
                                scrollflags | ((active == SC_ScrollBarAddPage)
                                               ? Style_Down : Style_Default));
              }
          }

          // subpage
          if (controls & SC_ScrollBarSubPage) {
              srect = querySubControlMetrics(control, widget,
                                             SC_ScrollBarSubPage, option);
              if (srect.isValid()) {
                  drawPrimitive(PE_ScrollBarSubPage, painter, srect, group,
                                scrollflags | ((active == SC_ScrollBarSubPage)
                                               ? Style_Down : Style_Default));
              }
          }

          // slider
          if (controls & SC_ScrollBarSlider) {
              if (sb->minValue() == sb->maxValue()) {
                  // maxed out
                  srect = querySubControlMetrics(control, widget,
                                                 SC_ScrollBarGroove, option);
              } else {
                  srect = querySubControlMetrics(control, widget,
                                                 SC_ScrollBarSlider, option);
              }
              if (srect.isValid()) {
                  if (horizontal) srect.addCoords(0, 0, 1, 0);
                  else srect.addCoords(0, 0, 0, 1);
                  drawPrimitive(PE_ScrollBarSlider, painter, srect, group,
                                scrollflags | ((active == SC_ScrollBarSlider)
                                               ? Style_Down : Style_Default));
                  // focus
                  if (sb->hasFocus()) {
                      srect.addCoords(2, 2, -2, -2);
                      drawPrimitive(PE_FocusRect, painter, srect, group,
                                    Style_Default);
                  }
              }
          }
          break;
      }

      case CC_SpinWidget: {
          const QSpinWidget *spin = ::qt_cast<const QSpinWidget*>(widget);
          if (!spin) {
              KStyle::drawComplexControl(control, painter, widget, rect, group,
                                         flags, controls, active, option);
              return;
          }

          PrimitiveElement element;

          // draw frame
          if (controls & SC_SpinWidgetFrame) {
              drawPhasePanel(painter, x, y, w, h, group, true, NULL);
          }

          // draw button field
          if (controls & SC_SpinWidgetButtonField) {
              subrect = querySubControlMetrics(CC_SpinWidget, widget,
                                               SC_SpinWidgetButtonField,
                                               option);
              if (reverse_) subrect.moveLeft(spin->upRect().left());
              drawPhaseBevel(painter, subrect.x(), subrect.y(),
                             subrect.width(), subrect.height(), group,
                             (widget==hover_)
                             ? group.button().light(contrast)
                             : group.button(), false, false, false);
          }

          // draw up arrow
          if (controls & SC_SpinWidgetUp) {
              subrect = spin->upRect();

              sunken = (active == SC_SpinWidgetUp);
              if (spin->buttonSymbols() == QSpinWidget::PlusMinus)
                  element = PE_SpinWidgetPlus;
              else
                  element = PE_SpinWidgetUp;

              drawPrimitive(element, painter, subrect, group, flags
                            | ((active == SC_SpinWidgetUp)
                               ? Style_On | Style_Sunken : Style_Raised));
          }

          // draw down button
          if (controls & SC_SpinWidgetDown) {
              subrect = spin->downRect();

              sunken = (active == SC_SpinWidgetDown);
              if (spin->buttonSymbols() == QSpinWidget::PlusMinus)
                  element = PE_SpinWidgetMinus;
              else
                  element = PE_SpinWidgetDown;

              drawPrimitive(element, painter, subrect, group, flags
                            | ((active == SC_SpinWidgetDown)
                               ? Style_On | Style_Sunken : Style_Raised));
          }
          break;
      }

      case CC_ToolButton: {
          const QToolButton *btn = ::qt_cast<const QToolButton*>(widget);
          if (!btn) {
              KStyle::drawComplexControl(control, painter, widget, rect, group,
                                         flags, controls, active, option);
              return;
          }

          QToolBar *toolbar;
          bool horiz = true;
          bool normal = !(down || on || raised); // normal button state

          x2 = rect.right();
          y2 = rect.bottom();

          // check for QToolBar parent
          if (btn->parent() && btn->parent()->inherits("QToolBar")) {
              toolbar = ::qt_cast<QToolBar*>(btn->parent());
              if (toolbar) {
                  horiz = (toolbar->orientation() == Qt::Horizontal);
                  if (normal) { // draw background
                      if (flatToolbar(toolbar)) {
                          // toolbar not floating or in a QMainWindow
                          painter->fillRect(rect, group.background());
                      } else {
                          drawPhaseGradient(painter, rect, group.background(),
                                            !horiz, 0, 0,
                                            toolbar->width()-3,
                                            toolbar->height()-3, true);
                          painter->setPen(group.mid());
                          if (horiz) {
                              painter->drawLine(x, y2, x2, y2);
                          } else {
                              painter->drawLine(x2, y, x2, y2);
                          }
                      }
                  }
              }
          }
          // check for QToolBarExtensionWidget parent
          else if (btn->parent() &&
                   btn->parent()->inherits(QTOOLBAREXTENSION)) {
              QWidget *extension;
              if ((extension = ::qt_cast<QWidget*>(btn->parent()))) {
                  toolbar = ::qt_cast<QToolBar*>(extension->parent());
                  if (toolbar) {
                      horiz = (toolbar->orientation() == Qt::Horizontal);
                      if (normal) { // draw background
                          drawPhaseGradient(painter, rect, group.background(),
                                            !horiz, 0, 0, toolbar->width()-3,
                                            toolbar->height()-3, true);
                      }
                  }
              }
          }
          // check for background pixmap
          else if (normal && btn->parentWidget() &&
                   btn->parentWidget()->backgroundPixmap() &&
                   !btn->parentWidget()->backgroundPixmap()->isNull()) {
              QPixmap pixmap = *(btn->parentWidget()->backgroundPixmap());
              painter->drawTiledPixmap(rect, pixmap, btn->pos());
          }
          // everything else
          else if (normal) {
              // toolbutton not on a toolbar
              painter->fillRect(rect, group.background());
          }

          // now draw active buttons
          if (down || on) {
              drawPhasePanel(painter, x, y, w, h, group, true,
                             &group.brush(QColorGroup::Button));
          } else if (raised) {
              drawPhaseBevel(painter, x, y, w, h, group, group.button(),
                             false, !horiz, true);
          }
          painter->setPen(group.text());
          break;
      }

      default:
          KStyle::drawComplexControl(control, painter, widget, rect, group,
                                     flags, controls, active, option);
          break;
    }
}

//////////////////////////////////////////////////////////////////////////////
// drawComplexControlMask()
// ------------------------
// Draw a bitmask for the control

void PhaseStyle::drawComplexControlMask(ComplexControl control,
                                        QPainter *painter,
                                        const QWidget *widget,
                                        const QRect &rect,
                                        const QStyleOption &option) const
{
    switch (control) {
      case CC_ComboBox:
      case CC_ToolButton: {
          painter->fillRect(rect, Qt::color1);
          painter->setPen(Qt::color0);
          break;
      }

      default:
          KStyle::drawComplexControlMask(control,painter,widget,rect,option);
    }
}

//////////////////////////////////////////////////////////////////////////////
// pixelMetric()
// -------------
// Get the pixel metric for metric

int PhaseStyle::pixelMetric(PixelMetric metric, const QWidget *widget) const
{
    // not using widget's font, so that all metrics are uniform
    int em = QMAX(QApplication::fontMetrics().strikeOutPos() * 3, 17);

    switch (metric) {
      case PM_DefaultFrameWidth:
          return 2;

      case PM_ButtonDefaultIndicator:   // size of default indicator
          return 2;

      case PM_ButtonMargin:             // Space betweeen frame and label
          return 3;

      case PM_TabBarTabOverlap:         // Amount of tab overlap
          return 1;

      case PM_TabBarTabHSpace:          // extra tab spacing
          return 24;

      case PM_TabBarTabVSpace:
          if (const QTabBar *tb = ::qt_cast<const QTabBar*>(widget)) {
              if (tb->shape() == QTabBar::RoundedAbove) {
                  return 10;
              } else {
                  return 6;
              }
          }
          return 0;

      case PM_ExclusiveIndicatorWidth:  // radiobutton size
      case PM_ExclusiveIndicatorHeight:
      case PM_IndicatorWidth:           // checkbox size
      case PM_IndicatorHeight:
      case PM_CheckListButtonSize:	// checkbox size in qlistview items
      case PM_ScrollBarExtent:          // base width of a vertical scrollbar
          return em & 0xfffe;

      case PM_SplitterWidth:            // width of spitter
          return (em / 3) & 0xfffe;

      case PM_ScrollBarSliderMin:       // minimum length of slider
          return  em * 2;

      case PM_SliderThickness:          // slider thickness
      case PM_SliderControlThickness:
          return em;

      default:
          return KStyle::pixelMetric(metric, widget);
    }
}

//////////////////////////////////////////////////////////////////////////////
// subRect()
// ---------
// Return subrect for the widget in logical coordinates

QRect PhaseStyle::subRect(SubRect rect, const QWidget *widget) const
{
    switch (rect) {
      case SR_ComboBoxFocusRect: {
          QRect r = querySubControlMetrics(CC_ComboBox, widget,
                                           SC_ComboBoxEditField);
          r.addCoords(1, 1,-1,-1);
          return r;
      }

      default:
          return KStyle::subRect(rect, widget);
    }
}

//////////////////////////////////////////////////////////////////////////////
// querySubControlMetrics()
// ------------------------
// Get metrics for subcontrols of complex controls

QRect PhaseStyle::querySubControlMetrics(ComplexControl control,
                                         const QWidget *widget,
                                         SubControl subcontrol,
                                         const QStyleOption &option) const
{
    QRect rect;

    const int fw = pixelMetric(PM_DefaultFrameWidth, widget);
    int w = widget->width(), h = widget->height();
    int xc;

    switch (control) {

      case CC_ComboBox: {
          xc = h; // position between edit and arrow

          switch (subcontrol) {
            case SC_ComboBoxFrame: // total combobox area
                rect = widget->rect();
                break;

            case SC_ComboBoxArrow: // the right side
                rect.setRect(w-xc, fw, xc-fw, h-(fw*2));
                break;

            case SC_ComboBoxEditField: // the left side
                rect.setRect(fw, fw, w-xc-fw-1, h-(fw*2));
                break;

            case SC_ComboBoxListBoxPopup: // the list popup box
                rect = option.rect();
                break;

            default:
                break;
          }
          break;
      }

      case CC_ScrollBar: {
          const QScrollBar *sb = ::qt_cast<const QScrollBar*>(widget);
          if (!sb) break;

          bool horizontal = (sb->orientation() == Qt::Horizontal);
          rect = KStyle::querySubControlMetrics(control, widget,
                                                subcontrol, option);

          // adjust the standard metrics so controls can "overlap"
          if (subcontrol == SC_ScrollBarGroove) {
              if (horizontal) rect.addCoords(-1, 0, 1, 0);
              else            rect.addCoords(0, -1, 0, 1);
          }
          break;
      }

      case CC_SpinWidget: {
          bool odd = widget->height() % 2;
          xc = (h * 3 / 4) + odd; // position between edit and arrows

          switch (subcontrol) {
            case SC_SpinWidgetButtonField:
                rect.setRect(w-xc, 1, xc-1, h-2);
                break;

            case SC_SpinWidgetEditField:
                rect.setRect(fw, fw, w-xc-fw, h-(fw*2));
                break;

            case SC_SpinWidgetFrame:
                rect = widget->rect();
                break;

            case SC_SpinWidgetUp:
                rect.setRect(w-xc, (h/2)-(odd ? 6 : 7), xc-1, 6);
                break;

            case SC_SpinWidgetDown:
                rect.setRect(w-xc, (h/2)+1, xc-1, odd ? 7 : 6);
                break;

            default:
                break;
          }
          break;
      }

      default:
          rect = KStyle::querySubControlMetrics(control, widget, subcontrol,
                                                option);
    }

    return rect;
}

//////////////////////////////////////////////////////////////////////////////
// sizeFromContents()
// ------------------
// Returns the size of widget based on the contentsize

QSize PhaseStyle::sizeFromContents(ContentsType contents,
                                   const QWidget* widget,
                                   const QSize &contentsize,
                                   const QStyleOption &option ) const
{
    int w = contentsize.width();
    int h = contentsize.height();

    switch (contents) {
      case CT_PushButton: {
          const QPushButton* button = ::qt_cast<const QPushButton*>(widget);
          if (!button) {
              return KStyle::sizeFromContents(contents, widget, contentsize,
                                              option);
          }
          int margin = pixelMetric(PM_ButtonMargin, widget)
              + pixelMetric(PM_DefaultFrameWidth, widget) + 4;

          w += margin + 6; // add room for bold font
          h += margin;

          // standard width and heights
          if (button->isDefault() || button->autoDefault()) {
              if (w < 80 && !button->pixmap()) w = 80;
          }
          if (h < 22) h = 22;
          return QSize(w, h);
      }

      case CT_PopupMenuItem: {
          if (!widget || option.isDefault()) return contentsize;
          const QPopupMenu *popup = ::qt_cast<const QPopupMenu*>(widget);
          if (!popup) {
              return KStyle::sizeFromContents(contents, widget, contentsize,
                                              option);
          }
          QMenuItem *item = option.menuItem();

          if (item->custom()) {
              w = item->custom()->sizeHint().width();
              h = item->custom()->sizeHint().height();
              if (!item->custom()->fullSpan())
                  h += ITEMVMARGIN*2 + ITEMFRAME*2;
          } else if (item->widget()) { // a menu item that is a widget
              w = contentsize.width();
              h = contentsize.height();
          } else if (item->isSeparator()) {
              w = h = 3;
          } else {
              if (item->pixmap()) {
                  h = QMAX(h, item->pixmap()->height() + ITEMFRAME*2);
              } else {
                  h = QMAX(h, MINICONSIZE + ITEMFRAME*2);
                  h = QMAX(h, popup->fontMetrics().height()
                           + ITEMVMARGIN*2 + ITEMFRAME*2);
              }
              if (item->iconSet())
                  h = QMAX(h, item->iconSet()->
                           pixmap(QIconSet::Small, QIconSet::Normal).height()
                           + ITEMFRAME*2);
          }

          if (!item->text().isNull() && item->text().find('\t') >= 0)
              w += 12;
          else if (item->popup())
              w += 2 * ARROWMARGIN;

          if (option.maxIconWidth() || popup->isCheckable()) {
              w += QMAX(option.maxIconWidth(),
                        QIconSet::iconSize(QIconSet::Small).width())
                  + ITEMHMARGIN*2;
          }
          w += RIGHTBORDER;
          return QSize(w, h);
      }

      default:
          return KStyle::sizeFromContents(contents, widget, contentsize,
                                          option);
    }
}

//////////////////////////////////////////////////////////////////////////////
// Miscellaneous                                                            //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// flatToolbar()
// -------------
// Is the toolbar "flat"

bool PhaseStyle::flatToolbar(const QToolBar *toolbar) const
{
    if (!toolbar) return true; // not on a toolbar
    if (!toolbar->isMovingEnabled()) return true; // immobile toolbars are flat
    if (!toolbar->area()) return true; // not docked
    if (toolbar->place() == QDockWindow::OutsideDock) return true; // ditto
    if (!toolbar->mainWindow()) return true; // not in a main window
    return false;
}

//////////////////////////////////////////////////////////////////////////////
// eventFilter()
// -------------
// Grab events we are interested in. Most of this routine is to handle the
// exceptions to the normal styling rules.

bool PhaseStyle::eventFilter(QObject *object, QEvent *event)
{
    if (KStyle::eventFilter(object, event)) return true;
    if (!object->isWidgetType()) return false;

    bool horiz;
    int x, y, w, h;
    QFrame *frame;
    QToolBar *toolbar;
    QWidget *widget;

    // painting events
    if (event->type() == QEvent::Paint) {
        // make sure we do the most specific stuff first

        // KDE Toolbar Widget
        // patch by Daniel Brownlees <dbrownlees@paradise.net.nz>
        if (object->parent() && !qstrcmp(object->name(), KTOOLBARWIDGET)) {
            if (0 == (widget = ::qt_cast<QWidget*>(object))) return false;
            QWidget *parent = ::qt_cast<QWidget*>(object->parent());
            int px = widget->x(), py = widget->y();
            // find the toolbar
            while (parent && parent->parent()
                   && !::qt_cast<QToolBar*>(parent)) {
                px += parent->x();
                py += parent->y();
                parent = ::qt_cast<QWidget*>(parent->parent());
            }
            if (!parent) return false;
            widget->rect().rect(&x, &y, &w, &h);
            QRect prect = parent->rect();

            toolbar = ::qt_cast<QToolBar*>(parent);
            horiz = (toolbar) ? (toolbar->orientation() == Qt::Horizontal)
                : (prect.height() < prect.width());
            QPainter painter(widget);
            if (flatToolbar(toolbar)) {
                painter.fillRect(widget->rect(),
                                  parent->colorGroup().background());
            } else {
                drawPhaseGradient(&painter, widget->rect(),
                                  parent->colorGroup().background(),
                                  !horiz, px, py,
                                  prect.width(), prect.height(), true);
                if (horiz && (h==prect.height()-2)) {
                    painter.setPen(parent->colorGroup().mid());
                    painter.drawLine(x, h-1, w-1, h-1);
                } else if (!horiz && (w==prect.width()-2)) {
                    painter.setPen(parent->colorGroup().mid());
                    painter.drawLine(w-1, y, w-1, h-1);
                }
            }
        }

        // QToolBarExtensionWidget
        else if (object && object->isWidgetType() && object->parent() &&
                 (toolbar = ::qt_cast<QToolBar*>(object->parent()))) {
            if (0 == (widget = ::qt_cast<QWidget*>(object))) return false;
            horiz = (toolbar->orientation() == Qt::Horizontal);
            QPainter painter(widget);
            widget->rect().rect(&x, &y, &w, &h);
            // draw the extension
            drawPhaseGradient(&painter, widget->rect(),
                              toolbar->colorGroup().background(),
                              !horiz, x, y, w-1, h-1, true);
            if (horiz) {
                painter.setPen(toolbar->colorGroup().dark());
                painter.drawLine(w-1, 0, w-1, h-1);
                painter.setPen(toolbar->colorGroup().mid());
                painter.drawLine(w-2, 0, w-2, h-2);
                painter.drawLine(x, h-1, w-2, h-1);
                painter.drawLine(x, y, x, h-2);
                painter.setPen(toolbar->colorGroup().midlight());
                painter.drawLine(x+1, y, x+1, h-2);
            } else {
                painter.setPen(toolbar->colorGroup().dark());
                painter.drawLine(0, h-1, w-1, h-1);
                painter.setPen(toolbar->colorGroup().mid());
                painter.drawLine(0, h-2, w-2, h-2);
                painter.drawLine(w-1, y, w-1, h-2);
                painter.drawLine(x, y, w-2, y);
                painter.setPen(toolbar->colorGroup().midlight());
                painter.drawLine(x, y+1, w-2, y+1);
            }
        }

        // QFrame lines (do this guy last)
        else if (0 != (frame = ::qt_cast<QFrame*>(object))) {
            QFrame::Shape shape = frame->frameShape();
            switch (shape) {
              case QFrame::HLine:
              case QFrame::VLine: {
                  // NOTE: assuming lines have no content
                  QPainter painter(frame);
                  frame->rect().rect(&x, &y, &w, &h);
                  painter.setPen(frame->colorGroup().dark());
                  if (shape == QFrame::HLine) {
                      painter.drawLine(0, h/2, w, h/2);
                  } else if (shape == QFrame::VLine) {
                      painter.drawLine(w/2, 0, w/2, h);
                  }
                  return true;
              }
              default:
                  break;
            }
        }

    } else if (highlights_) { // "mouseover" events
        if (::qt_cast<QPushButton*>(object) ||
            ::qt_cast<QComboBox*>(object) ||
            ::qt_cast<QSpinWidget*>(object) ||
            ::qt_cast<QCheckBox*>(object) ||
            ::qt_cast<QRadioButton*>(object) ||
            ::qt_cast<QSlider*>(object) ||
            object->inherits(QSPLITTERHANDLE)) {
            if (event->type() == QEvent::Enter) {
                if (0 != (widget = ::qt_cast<QWidget*>(object)) &&
                    widget->isEnabled()) {
                    hover_ = widget;
                    widget->repaint(false);
                }
            } else if (event->type() == QEvent::Leave) {
                if (0 != (widget = ::qt_cast<QWidget*>(object))) {
                    hover_ = 0;
                    widget->repaint(false);
                }
            }
        } else if (::qt_cast<QTabBar*>(object)) { // special case for qtabbar
            if (event->type() == QEvent::Enter) {
                if (0 != (widget = ::qt_cast<QWidget*>(object)) &&
                    widget->isEnabled()) {
                    hover_ = widget;
                    hovertab_ = 0;;
                    widget->repaint(false);
                }
            } else if (event->type() == QEvent::Leave) {
                if (0 != (widget = ::qt_cast<QWidget*>(object))) {
                    hover_ = 0;
                    hovertab_ = 0;;
                    widget->repaint(false);
                }
            } else if (event->type() == QEvent::MouseMove) {
                QTabBar *tabbar;
                if (0 != (tabbar = ::qt_cast<QTabBar*>(object))) {
                    QMouseEvent *me;
                    if (0 != (me = dynamic_cast<QMouseEvent*>(event))) {
                        QTab *tab = tabbar->selectTab(me->pos());
                        if (hovertab_ != tab) {
                            hovertab_ = tab;
                            tabbar->repaint(false);
                        }
                    }
                }
            }
        }
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////////
// GradientSet                                                              //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// GradientSet()
// -------------
// Constructor

GradientSet::GradientSet(const QColor &color, int size)
    : color_(color), size_(size)
{
    for (int n=0; n<GradientTypeCount; ++n)  set[n] = 0;
}

//////////////////////////////////////////////////////////////////////////////
// ~GradientSet()
// --------------
// Destructor

GradientSet::~GradientSet()
{
    for (int n=0; n<GradientTypeCount; ++n) if (set[n]) delete set[n];
}

//////////////////////////////////////////////////////////////////////////////
// gradient()
// ----------
// Return the appropriate gradient pixmap

KPixmap* GradientSet::gradient(bool horizontal, bool reverse)
{
    GradientType type;

    if (horizontal) {
        type = (reverse) ? HorizontalReverse : Horizontal;
    } else {
        type = (reverse) ? VerticalReverse : Vertical;
    }

    // lazy allocate
    if (!set[type]) {
        set[type] = new KPixmap();
        switch (type) {
          case Horizontal:
              set[type]->resize(size_, 16);
              KPixmapEffect::gradient(*set[type],
                                      color_.light(contrast),
                                      color_.dark(contrast),
                                      KPixmapEffect::HorizontalGradient);
              break;

          case HorizontalReverse:
              set[type]->resize(size_, 16);
              KPixmapEffect::gradient(*set[type],
                                      color_.dark(contrast),
                                      color_.light(contrast),
                                      KPixmapEffect::HorizontalGradient);
              break;

          case Vertical:
              set[type]->resize(16, size_);
              KPixmapEffect::gradient(*set[type],
                                      color_.light(contrast),
                                      color_.dark(contrast),
                                      KPixmapEffect::VerticalGradient);
              break;

          case VerticalReverse:
              set[type]->resize(16, size_);
              KPixmapEffect::gradient(*set[type],
                                      color_.dark(contrast),
                                      color_.light(contrast),
                                      KPixmapEffect::VerticalGradient);
              break;

          default:
              break;
        }
    }
    return (set[type]);
}

//////////////////////////////////////////////////////////////////////////////
// Plugin Stuff                                                             //
//////////////////////////////////////////////////////////////////////////////

class PhaseStylePlugin : public QStylePlugin
{
 public:
    PhaseStylePlugin();
    QStringList keys() const;
    QStyle *create(const QString &key);
};

PhaseStylePlugin::PhaseStylePlugin() : QStylePlugin() { ; }

QStringList PhaseStylePlugin::keys() const
{
    return QStringList() << "Phase";
}

QStyle* PhaseStylePlugin::create(const QString& key)
{
    if (key.lower() == "phase")
        return new PhaseStyle();
    return 0;
}

KDE_Q_EXPORT_PLUGIN(PhaseStylePlugin)

#include "phasestyle.moc"
