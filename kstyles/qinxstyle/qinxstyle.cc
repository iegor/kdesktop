//////////////////////////////////////////////////////////////////////////////
// qinxstyle.cc
// -------------------
// A style for KDE
// -------------------
// Copyright (c) 2002-2004 David Johnson
// Please see the header file for copyright and license information.
//////////////////////////////////////////////////////////////////////////////

#include <kdemacros.h>
#include <kdrawutil.h>
#include <kpixmap.h>
#include <kpixmapeffect.h>

#include <qapplication.h>
#include <qintdict.h>
#include <qpainter.h>
#include <qpointarray.h>
#include <qsettings.h>

#include <qcombobox.h>
#include <qheader.h>
#include <qmainwindow.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qscrollbar.h>
#include <qslider.h>
#include <qstyleplugin.h>
#include <qtabbar.h>
#include <qtabwidget.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>

#include "qinxstyle.h"

static const char* QSPLITTERHANDLE    = "QSplitterHandle";
static const char* QTOOLBAREXTENSION  = "QToolBarExtensionWidget";
static const char* KTOOLBARWIDGET     = "kde toolbar widget";

// some convenient constants
static const int ITEMFRAME       = 1;
static const int ITEMHMARGIN     = 3;
static const int ITEMVMARGIN     = 0;
static const int ARROWMARGIN     = 6;
static const int RIGHTBORDER     = 6;
static const int MINICONSIZE     = 16;
static const int CHECKSIZE       = 7;
static const int MAXGRADIENTSIZE = 64;

static unsigned contrast = 100;

static QBitmap radiooff_shadow;
static const unsigned char radiooff_shadow_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00};

static QBitmap radiooff_dark;
static const unsigned char radiooff_dark_bits[] = {
    0xf0, 0x01, 0x0c, 0x06, 0x02, 0x08, 0x02, 0x08, 0x01, 0x10, 0x01, 0x10,
    0x01, 0x10, 0x01, 0x10, 0x01, 0x10, 0x02, 0x08, 0x02, 0x08, 0x0c, 0x06,
    0xf0, 0x01};

static QBitmap radiooff_mid;
static const unsigned char radiooff_mid_bits[] = {
    0x08, 0x02, 0x10, 0x01, 0x04, 0x04, 0x01, 0x10, 0x02, 0x08, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x02, 0x08, 0x01, 0x10, 0x04, 0x04, 0x10, 0x01,
    0x08, 0x02};

static QBitmap radioon_shadow;
static const unsigned char radioon_shadow_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0xe0, 0x00,
   0xf0, 0x01, 0xe0, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00};

static QBitmap radioon_dark;
static const unsigned char radioon_dark_bits[] = {
   0xf0, 0x01, 0x0c, 0x06, 0x02, 0x08, 0x02, 0x08, 0xb1, 0x11, 0x11, 0x11,
   0x01, 0x10, 0x11, 0x11, 0xb1, 0x11, 0x02, 0x08, 0x02, 0x08, 0x0c, 0x06,
   0xf0, 0x01};

static QBitmap radioon_mid;
static const unsigned char radioon_mid_bits[] = {
   0x08, 0x02, 0x10, 0x01, 0x04, 0x04, 0xe1, 0x10, 0x02, 0x08, 0x08, 0x02,
   0x08, 0x02, 0x08, 0x02, 0x02, 0x08, 0xe1, 0x10, 0x04, 0x04, 0x10, 0x01,
   0x08, 0x02};

static QBitmap radiomask;
static const unsigned char radiomask_bits[] = {
    0xf8, 0x03, 0xfc, 0x07, 0xfe, 0x0f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f,
    0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xfe, 0x0f, 0xfc, 0x07,
    0xf8, 0x03};

static QBitmap dexpand;
static const unsigned char dexpand_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0xff, 0x01, 0xfe, 0x00, 0x7c, 0x00, 0x38, 0x00,
    0x10, 0x00, 0x00, 0x00, 0x00, 0x00};

static QBitmap rexpand;
static const unsigned char rexpand_bits[] = {
    0x04, 0x00, 0x0c, 0x00, 0x1c, 0x00, 0x3c, 0x00, 0x7c, 0x00, 0x3c, 0x00,
    0x1c, 0x00, 0x0c, 0x00, 0x04, 0x00};

QMap<unsigned int, QIntDict<GradientSet> > gradients; // gradients[color][size]

//////////////////////////////////////////////////////////////////////////////
// Construction, Destruction, Initialization                                //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// QinxStyle()
// -----------
// Constructor

QinxStyle::QinxStyle()
    : KStyle(FilledFrameWorkaround | AllowMenuTransparency,
             ThreeButtonScrollBar),
      hover_(0), gradients_(QPixmap::defaultDepth() > 8), kicker_(false)
{
    QSettings settings;
    photontabs_ =
        settings.readBoolEntry("/qinxstyle/Settings/photonTabs", false);
    highlights_ =
        settings.readBoolEntry("/qinxstyle/Settings/highlights", true);
    photonmenus_ =
        ((settings.readBoolEntry("/qinxstyle/Settings/photonMenus", true) &&
          (settings.readEntry("/kstyle/Settings/MenuTransparencyEngine",
                              "Disabled") == "Disabled")));

    if (gradients_) { // don't bother setting if already false
        gradients_ =
            settings.readBoolEntry("/qinxstyle/Settings/useGradients", true);
        contrast = 100 + settings.readNumEntry("/Qt/KDE/contrast", 5) * 2;
    }

    reverse_ = QApplication::reverseLayout();

    radiooff_shadow = QBitmap(13, 13, radiooff_shadow_bits, true);
    radiooff_dark = QBitmap(13, 13, radiooff_dark_bits, true);
    radiooff_mid = QBitmap(13, 13, radiooff_mid_bits, true);
    radioon_shadow = QBitmap(13, 13, radioon_shadow_bits, true);
    radioon_dark = QBitmap(13, 13, radioon_dark_bits, true);
    radioon_mid = QBitmap(13, 13, radioon_mid_bits, true);
    radiomask = QBitmap(13, 13, radiomask_bits, true);
    radiomask.setMask(radiomask);
    dexpand = QBitmap(9, 9, dexpand_bits, true);
    dexpand.setMask(dexpand);
    rexpand = QBitmap(9, 9, rexpand_bits, true);
    rexpand.setMask(rexpand);    
}

QinxStyle::~QinxStyle() { ; }

//////////////////////////////////////////////////////////////////////////////
// Polishing                                                                //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// polish()
// --------
// Initialize application specific

void QinxStyle::polish(QApplication* app)
{
    if (!qstrcmp(app->argv()[0], "kicker")) kicker_ = true;
}

//////////////////////////////////////////////////////////////////////////////
// polish()
// --------
// Initialize the appearance of a widget

void QinxStyle::polish(QWidget *widget)
{
    if (::qt_cast<QMenuBar*>(widget) ||
        ::qt_cast<QPopupMenu*>(widget)) {
        widget->setBackgroundMode(NoBackground);
    } else if (highlights_ && // highlighting
               (::qt_cast<QPushButton*>(widget) ||
                ::qt_cast<QComboBox*>(widget) ||
                ::qt_cast<QSpinWidget*>(widget) ||
                ::qt_cast<QSlider*>(widget) ||
                widget->inherits(QSPLITTERHANDLE))) {
        widget->installEventFilter(this);
    } else if (widget->inherits(QTOOLBAREXTENSION) ||
               (!qstrcmp(widget->name(), KTOOLBARWIDGET))) {
        widget->installEventFilter(this);
    }

    KStyle::polish(widget);
}

//////////////////////////////////////////////////////////////////////////////
// polish()
// --------
// Initialize the palette

void QinxStyle::polish(QPalette &pal)
{
    gradients.clear(); // clear out gradients on a color change
    QStyle::polish(pal);
}

//////////////////////////////////////////////////////////////////////////////
// unPolish()
// ----------
// Undo the initialization of a widget's appearance

void QinxStyle::unPolish(QWidget *widget)
{
    if (::qt_cast<QMenuBar*>(widget) ||
        ::qt_cast<QPopupMenu*>(widget)) {
        widget->setBackgroundMode(PaletteBackground);
    } else if (highlights_ && // highlighting
               (::qt_cast<QPushButton*>(widget) ||
                ::qt_cast<QComboBox*>(widget) ||
                ::qt_cast<QSpinWidget*>(widget) ||
                ::qt_cast<QSlider*>(widget) ||
                widget->inherits(QSPLITTERHANDLE))) {
        widget->removeEventFilter(this);
    } else if (widget->inherits(QTOOLBAREXTENSION) ||
               (!qstrcmp(widget->name(), KTOOLBARWIDGET))) {
        widget->removeEventFilter(this);
    }

    KStyle::unPolish(widget);
}

//////////////////////////////////////////////////////////////////////////////
// Drawing                                                                  //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// drawQinxGradient()
// ------------------
// Draw gradient

void QinxStyle::drawQinxGradient(QPainter *painter,
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

    // lazy allocation
    if (size > MAXGRADIENTSIZE) { // keep it sensible
        painter->fillRect(rect, color);
    } else {
        GradientSet *set = gradients[color.rgb()][size];
        if (!set) {
            set = new GradientSet(color, size);
            gradients[color.rgb()].setAutoDelete(true);
            gradients[color.rgb()].insert(size, set);
        }
        painter->drawTiledPixmap(rect, *set->gradient(horizontal, reverse),
                                 QPoint(px, py));
    }
}

//////////////////////////////////////////////////////////////////////////////
// drawQinxBevel()
// ----------------
// Draw the basic Qinx bevel

void QinxStyle::drawQinxBevel(QPainter *painter,
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

    painter->setPen(sunken ? group.mid() : group.light());
    painter->drawLine(x+1, y+1, x2-1, y+1);
    painter->drawLine(x+1, y+2, x+1, y2-1);

    painter->setPen(sunken ? group.light() : group.mid());
    painter->drawLine(x+1, y2-1, x2-1, y2-1);
    painter->drawLine(x2-1, y+1, x2-1, y2-2);

    if (sunken) {
        // sunken bevels don't get gradients
        painter->fillRect(x+2, y+2, w-4, h-4, fill);
    } else {
        drawQinxGradient(painter, QRect(x+2, y+2, w-4, h-4), fill,
                         horizontal, 0, 0, w-4, h-4, reverse);
    }
    painter->restore();
}

//////////////////////////////////////////////////////////////////////////////
// drawQinxButton()
// ----------------
// Draw the basic Qinx button

void QinxStyle::drawQinxButton(QPainter *painter,
                               int x, int y, int w, int h,
                               const QColorGroup &group,
                               const QColor &fill,
                               bool sunken) const
{
    int x2 = x + w - 1;
    int y2 = y + h - 1;

    painter->setPen(group.light());
    painter->drawLine(x, y2, x2, y2);
    painter->drawLine(x2, y, x2, y2-1);

    painter->setPen(group.mid());
    painter->drawLine(x, y,  x2-1, y);
    painter->drawLine(x, y+1, x, y2-1);

    drawQinxBevel(painter, x+1, y+1, w-2, h-2, group, fill,
                  sunken, false, false);
}

//////////////////////////////////////////////////////////////////////////////
// drawQinxPanel()
// ----------------
// Draw the basic Qinx panel

void QinxStyle::drawQinxPanel(QPainter *painter,
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
        painter->setPen(group.mid());
        painter->drawLine(x, y, x2-1, y);
        painter->drawLine(x, y+1, x, y2-1);
        painter->setPen(group.light());
        painter->drawLine(x, y2, x2, y2);
        painter->drawLine(x2, y, x2, y2-1);
    } else {
        painter->setPen(group.dark());
        painter->drawRect(x, y, w, h);
        painter->setPen(group.mid());
        painter->drawLine(x+1, y2-1, x2-1, y2-1);
        painter->drawLine(x2-1, y+1, x2-1, y2-2);
        painter->setPen(group.light());
        painter->drawLine(x+1, y+1, x+1, y2-2);
        painter->drawLine(x+2, y+1, x2-2, y+1);
    }
    if (fill) {
        painter->fillRect(x+2, y+2, w-4, h-4, fill->color());
    }
    painter->restore();
}

//////////////////////////////////////////////////////////////////////////////
// drawQinxTab()
// -------------
// Draw a Qinx style tab

void QinxStyle::drawQinxTab(QPainter *painter,
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
          if (photontabs_ && !reverse_) { // photon style angled tabs
              QPointArray parray;
              const int xc = x2 - h + 2;

              // is there a corner widget?
              tabwidget = dynamic_cast<QTabWidget*>(bar->parent());
              if (edge && tabwidget
                  && tabwidget->cornerWidget(reverse_ ?
                                             Qt::TopRight : Qt::TopLeft)) {
                  edge = false;
              }

              // draw tab
              painter->setPen((selected) ? group.background() : group.mid());
              painter->setBrush((selected) ? group.background() : group.mid());
              parray.putPoints(0, 5,
                               x+1,y+1, xc+1,y+1, x2,y2-1, x2,y2, x+1,y2);
              painter->drawPolygon(parray);
              
              painter->setPen(group.dark());
              painter->drawLine(x, y+1, x, y2-2);
              painter->drawLine(x+1, y, xc-1, y);
              painter->drawLine(xc, y+1, xc+1, y+1);
              painter->drawLine(x2-1, y2-2, x2, y2-2);
              painter->drawLine(xc+2, y+2, x2-2, y2-3);

              if (selected) painter->setPen(group.midlight());
              else  painter->setPen(group.midlight().dark(contrast));
              painter->drawLine(xc, y+2, xc+1, y+2);
              painter->drawLine(x2-1, y2-1, x2, y2-1);

              painter->setPen(group.mid());
              painter->drawLine(xc+2, y+3, x2-2, y2-2);

              if (selected) painter->setPen(group.light());
              else painter->setPen(group.midlight());
              painter->drawLine(x+1, y+1, x+1, y2-2);
              painter->drawLine(x+2, y+1, xc-1, y+1);

              // finish off bottom
              if (selected) {
                  painter->setPen(group.dark());
                  painter->drawPoint(x, y2-1);

                  painter->setPen(group.light());
                  painter->drawPoint(x, y2);
                  painter->drawLine(x+1, y2-1, x+1, y2);

                  if (!reverse_ && edge) {
                      painter->setPen(group.dark());
                      painter->drawLine(x, y2-1, x, y2);
                      painter->setPen(group.light());
                      painter->drawPoint(x+1, y2);
                  }
              } else {
                  painter->setPen(group.dark());
                  painter->drawLine(x, y2-1, x2, y2-1);

                  painter->setPen(group.light());
                  painter->drawLine(x, y2, x2, y2);

                  if (!reverse_ && edge) {
                      painter->setPen(group.dark());
                      painter->drawLine(x, y2-1, x, y2);
                  }
              }
              if (reverse_ && edge) {
                  painter->setPen(group.dark());
                  painter->drawPoint(x2, y2);
                  if (selected) painter->drawPoint(x2, y2-1);
                  painter->setPen(group.mid());
                  painter->drawPoint(x2-1, y2);
                  if (selected) painter->drawPoint(x2-1, y2-1);
              }
              break;
          } else {
              // not photontabs_
              // we fall though to TrianglularAbove below
          }

      case QTabBar::TriangularAbove: {
          // is there a corner widget?
          tabwidget = dynamic_cast<QTabWidget*>(bar->parent());
      if (edge && tabwidget
          && tabwidget->cornerWidget(reverse_ ?
                     Qt::TopRight : Qt::TopLeft)) {
          edge = false;
      }

          painter->setBrush((selected) ?
                            group.background() :
                            group.background().dark(contrast));
          painter->setPen(Qt::NoPen);
          painter->fillRect(x+1, y+1, w-1, h-1, painter->brush());

          // draw tab
          painter->setPen(group.dark());
          painter->drawLine(x, y+1, x, y2-2);
          painter->drawLine(x+1, y, x2, y);
          painter->drawLine(x2, y+1, x2, y2-2);

          painter->setPen(group.mid());
          painter->drawLine(x2-1, y+1, x2-1, y2-2);

          if (selected) painter->setPen(group.light());
          else painter->setPen(group.midlight());
          painter->drawLine(x+1, y+1, x+1, y2-2);
          painter->drawLine(x+2, y+1, x2-1, y+1);

          // finish off bottom
          if (selected) {
              painter->setPen(group.dark());
              painter->drawPoint(x, y2-1);
              painter->drawPoint(x2, y2-1);

              painter->setPen(group.light());
              painter->drawPoint(x, y2);
              painter->drawLine(x+1, y2-1, x+1, y2);
              painter->drawPoint(x2, y2);

              painter->setPen(group.mid());
              painter->drawPoint(x2-1, y2-1);

              if (!reverse_ && edge) {
                  painter->setPen(group.dark());
                  painter->drawLine(x, y2-1, x, y2);
                  painter->setPen(group.light());
                  painter->drawPoint(x+1, y2);
              }
          } else {
              painter->setPen(group.dark());
              painter->drawLine(x, y2-1, x2, y2-1);

              painter->setPen(group.light());
              painter->drawLine(x, y2, x2, y2);

              if (!reverse_ && edge) {
                  painter->setPen(group.dark());
                  painter->drawLine(x, y2-1, x, y2);
              }
          }
          if (reverse_ && edge) {
              painter->setPen(group.dark());
              painter->drawPoint(x2, y2);
              painter->setPen(group.mid());
              painter->drawPoint(x2-1, y2);
          }
          break;
      }

      case QTabBar::RoundedBelow:
      case QTabBar::TriangularBelow: {
      // is there a corner widget?
      tabwidget = dynamic_cast<QTabWidget*>(bar->parent());
      if (edge && tabwidget
          && tabwidget->cornerWidget(reverse_ ?
                     Qt::BottomRight : Qt::BottomLeft)) {
          edge = false;
      }

          painter->setBrush((selected) ?
                            group.background() :
                            group.background().dark(contrast));
          painter->setPen(Qt::NoPen);
          painter->fillRect(x+1, y+1, w-1, h-1, painter->brush());

          // draw tab
          painter->setPen(group.dark());
          painter->drawLine(x, y+1, x, y2-1);
          painter->drawLine(x+1, y2, x2, y2);
          painter->drawLine(x2, y+1, x2, y2-1);

          painter->setPen(group.mid());
          painter->drawLine(x2-1, y+1, x2-1, y2-1);
          painter->drawLine(x+1, y2-1, x2-1, y2-1);
          painter->drawPoint(x, y);
          painter->drawPoint(x2, y);

          if (selected) painter->setPen(group.light());
          else painter->setPen(group.midlight());
          painter->drawLine(x+1, y+1, x+1, y2-2);

          // finish off top
          if (selected) {
              if (!reverse_ && edge) {
                  painter->setPen(group.dark());
                  painter->drawPoint(x, y);
                  painter->setPen(group.light());
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
// Draw the element

void QinxStyle::drawPrimitive(PrimitiveElement element,
                              QPainter *painter,
                              const QRect &rect,
                              const QColorGroup &group,
                              SFlags flags,
                              const QStyleOption &option) const
{
    // common locals
    bool down    = flags & Style_Down;
    bool on      = flags & Style_On;
    bool depress = (down || on);
    bool sunken  = flags & Style_Sunken;
    bool enabled = flags & Style_Enabled;
    bool horiz   = flags & Style_Horizontal;
    int  x, y, w, h, x2, y2;

    rect.rect(&x, &y, &w, &h);
    x2 = rect.right();
    y2 = rect.bottom();

    switch(element) {
      case PE_ButtonDefault:
      case PE_ButtonDropDown:
      case PE_ButtonTool:
      case PE_ButtonBevel:
          drawQinxBevel(painter, x,y,w,h, group, group.button(),
                        depress, false, true);
          break;

      case PE_ButtonCommand:
          drawQinxButton(painter, x, y, w, h, group, (flags & Style_MouseOver)
                         ? group.button().light(contrast) : group.button(),
                         depress);
          break;

      case PE_HeaderSection: {
          // covers kicker taskbar buttons and menu titles
          QHeader* header = dynamic_cast<QHeader*>(painter->device());
          QWidget* widget = dynamic_cast<QWidget*>(painter->device());

          if (header) {
              horiz = (header->orientation() == Horizontal);
          } else {
              horiz = true;
          }

          if ((widget) && ((widget->inherits("QPopupMenu")) ||
                           (widget->inherits("KPopupTitle")))) {
              // kicker/kdesktop menu titles
              drawQinxBevel(painter, x,y,w,h, group,
                            group.background(), depress, !horiz, false);
          } else if (kicker_) {
              // taskbar buttons
              painter->save();
              ++x2; ++y2; ++w; ++h; // "pack" buttons
              painter->setPen(group.background());
              painter->drawRect(x, y, w, h);
              if (depress) {
                  painter->setPen(group.shadow());
                  painter->drawRect(x+2, y+2, w-4, h-4);
                  painter->setPen(group.mid().dark(contrast));
                  painter->drawLine(x+1, y+1, x+1, y2-2);
                  painter->drawLine(x+2, y+1, x2-2, y+1);
                  painter->setPen(group.light());
                  painter->drawLine(x+1, y2-1, x2-1, y2-1);
                  painter->drawLine(x2-1, y+1, x2-1, y2-2);
                  painter->fillRect(x+3, y+3, w-6, h-6,
                                    group.brush(QColorGroup::Midlight));
              } else {
                  painter->setPen(group.mid().dark(contrast));
                  painter->drawRect(x+1, y+1, w-3, h-3);
                  painter->setPen(group.light());
                  painter->drawLine(x+2, y+2, x+2, y2-3);
                  painter->drawLine(x+3, y+2, x2-3, y+2);
                  painter->drawLine(x+1, y2-1, x2-1, y2-1);
                  painter->drawLine(x2-1, y+1, x2-1, y2-2);
                  painter->fillRect(x+3, y+3, w-6, h-6,
                                    group.brush(QColorGroup::Background));
              }
              painter->restore();
          } else {
              // other headers
              drawQinxBevel(painter, x-1,y-1,w+1,h+1,
                            group, group.background(), depress, !horiz, true);
          }
          break;
      }

      case PE_HeaderArrow:
          if (flags & Style_Up)
              drawPrimitive(PE_ArrowUp, painter, rect, group, Style_Enabled);
          else
              drawPrimitive(PE_ArrowDown, painter, rect, group, Style_Enabled);
          break;

      case PE_FocusRect:
          painter->drawWinFocusRect(rect, group.button());
          break;

      case PE_ScrollBarSlider:
          drawQinxBevel(painter, x, y, w, h, group,
                        down ? group.midlight(): group.button(),
                        down, !horiz, true);
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
          drawQinxBevel(painter, x, y, w, h, group,
                        down ? group.midlight(): group.button(),
                        down, (horiz), false);

          PrimitiveElement arrow = ((horiz) ?
                                    ((element == PE_ScrollBarAddLine) ?
                                     PE_ArrowRight : PE_ArrowLeft) :
                                    ((element == PE_ScrollBarAddLine) ?
                                     PE_ArrowDown : PE_ArrowUp));
          drawPrimitive(arrow, painter, rect, group, flags);
          break;
      }

      case PE_Indicator:
          painter->setPen(group.mid());
          painter->drawLine(x, y, x2-1, y);
          painter->drawLine(x, y+1, x, y2-1);

          painter->setPen(group.light());
          painter->drawLine(x, y2, x2, y2);
          painter->drawLine(x2, y, x2, y2-1);

          painter->setPen(group.dark());
          painter->drawRect(x+1, y+1, w-2, h-2);

          painter->fillRect(x+2, y+2, w-4, h-4,
                            down ? group.brush(QColorGroup::Background) :
                            group.light());

          if (on) {
              painter->setPen(group.dark());
              painter->drawLine(x+3, y+4, x2-4, y2-3);
              painter->drawLine(x+4, y+3, x2-3, y2-4);
              painter->drawLine(x+3, y2-4, x2-4, y+3);
              painter->drawLine(x+4, y2-3, x2-3, y+4);

              painter->setPen(group.shadow());
              painter->drawLine(x+3, y+3, x2-3, y2-3);
              painter->drawLine(x+3, y2-3, x2-3, y+3);
          } else {
              painter->setPen(group.mid());
              painter->drawRect(x+3, y+3, w-6, h-6);
          }
          break;

      case PE_ExclusiveIndicator:
          if (enabled) {
              drawQinxGradient(painter, QRect(x+1, y+1, w-2, h-2),
                               group.midlight(), false, 0, 0, w-2, h-2, false);
          } else {
              painter->fillRect(QRect(x+1, y+1, w-2, h-2), group.midlight());
          }
          painter->setPen(group.background());
          painter->drawPoint(x+1, y+1);
          painter->drawPoint(x+1, y2-1);
          painter->drawPoint(x2-1, y+1);
          painter->drawPoint(x2-1, y2-1);

          if (on) {
              kColorBitmaps(painter, group, x, y,
                            0, &radioon_mid, 0,
                            &radioon_dark, &radioon_shadow, 0);
          } else {
              kColorBitmaps(painter, group, x, y,
                            0, &radiooff_mid, 0,
                            &radiooff_dark, &radiooff_shadow, 0);
          }
          break;

      case PE_ExclusiveIndicatorMask:
          painter->setPen(Qt::color1);
          painter->drawPixmap(x, y, radiomask);
          break;

      case PE_DockWindowResizeHandle:
      case PE_Splitter:
          drawQinxPanel(painter, x, y, w, h, group, false,
                        (hover_ == painter->device())
                        ? &group.brush(QColorGroup::Light)
                        : &group.brush(QColorGroup::Background));
          break;

      case PE_Panel:
      case PE_PanelLineEdit:
      case PE_PanelTabWidget:
      case PE_TabBarBase:
      case PE_PanelPopup:
          drawQinxPanel(painter, x, y, w, h, group, sunken, NULL);
          break;

      case PE_WindowFrame:
          drawQinxPanel(painter, x, y, w, h, group, false, NULL);
          break;

//       case PE_GroupBoxFrame:
//       case PE_PanelGroupBox:
//       case PE_Separator:
          // TODO: group box frames need to be lightened

      case PE_StatusBarSection:
          painter->setPen(group.mid());
          painter->drawLine(x, y,  x2-1, y);
          painter->drawLine(x, y+1, x, y2-1);
          painter->setPen(group.midlight());
          painter->drawLine(x, y2, x2, y2);
          painter->drawLine(x2, y, x2, y2-1);
          break;

      case PE_PanelMenuBar:
      case PE_PanelDockWindow:
          // adjust rect so we can use bevel
          --x; --y; ++w; ++h;
          drawQinxBevel(painter, x, y, w, h, group, group.button(),
                        false, (w < h), true);
          break;

      case PE_DockWindowSeparator: {
          QWidget *widget = dynamic_cast<QWidget*>(painter->device());
          bool flat = true;

          if (widget && widget->parent() &&
              widget->parent()->inherits("QToolBar")) {
              QToolBar *toolbar = dynamic_cast<QToolBar*>(widget->parent());
              if (toolbar) {
                  // toolbar not floating or in a QMainWindow
                  flat = flatToolbar(toolbar);
              }
          }

          if (flat)
              painter->fillRect(rect, group.button());
          else
              drawQinxGradient(painter, rect, group.button(),
                               !(horiz), 0, 0, -1, -1, true);

          if (horiz) {
              painter->setPen(group.mid());
              painter->drawLine(w/2-1, 0, w/2-1, h-1);
              if (!flat) painter->drawLine(0, h-1, w-1, h-1);

              painter->setPen(group.dark());
              painter->drawLine(w/2, 0, w/2, h-1);

              painter->setPen(group.light());
              painter->drawLine(w/2+1, 0, w/2+1, h-2);
          } else {
              painter->setPen(group.mid());
              painter->drawLine(0, h/2-1, w-1, h/2-1);
              if (!flat) painter->drawLine(w-1, 0, w-1, h-1);

              painter->setPen(group.dark());
              painter->drawLine(0, h/2, w-1, h/2);

              painter->setPen(group.light());
              painter->drawLine(0, h/2+1, w-2, h/2+1);
          }
          break;
      }
       
      case PE_ArrowUp:
      case PE_ArrowDown:
      case PE_ArrowLeft:
      case PE_ArrowRight:
      case PE_SpinWidgetDown:
      case PE_SpinWidgetUp:
      case PE_SpinWidgetPlus:
      case PE_SpinWidgetMinus: {
          QPointArray parray;
          switch (element) {
          case PE_SpinWidgetUp:
          case PE_ArrowUp:
              parray.putPoints(0, 8, +1,+2, +1,+0, +3,+0, +0,-3,
                               -1,-3, -4,+0, -2,+0, -2,+2);
              break;
          case PE_SpinWidgetDown:
          case PE_ArrowDown:
              parray.putPoints(0, 8, -2,-3, -2,-1, -4,-1, -1,+2,
                               +0,+2, +3,-1, +1,-1, +1,-3);
              break;
          case PE_ArrowLeft:
              parray.putPoints(0, 8, +2,-2, +0,-2, +0,-4, -3,-1,
                               -3,+0, +0,+3, +0,+1, +2,+1);
              break;
          case PE_ArrowRight:
              parray.putPoints(0, 8, -3,+1, -1,+1, -1,+3, +2,+0,
                               +2,-1, -1,-4, -1,-2, -3,-2);
              break;
          case PE_SpinWidgetPlus:
              parray.putPoints(0, 12, -3,-1, -1,-1, -1,-3, 0,-3, 0,-1, 2,-1,
                               2,0, 0,0, 0,2, -1,2, -1,0, -3,0);
              break;
          case PE_SpinWidgetMinus:
              parray.putPoints(0, 4, -3,-1, 2,-1, 2,0, -3,0);
              break;
          default:
              return;
          }

          painter->save();
          parray.translate(x+w/2, y+h/2);
          painter->setPen(enabled ? group.buttonText() : group.mid());
          painter->setBrush(enabled ? group.buttonText() : group.mid());
          painter->drawPolygon(parray);
          painter->restore();
          break;
      }

      default:
          KStyle::drawPrimitive(element, painter, rect, group, flags, option);
    }
}

//////////////////////////////////////////////////////////////////////////////
// drawKStylePrimitive()
// ---------------------
// Draw the element

void QinxStyle::drawKStylePrimitive(KStylePrimitive element,
                                    QPainter *painter,
                                    const QWidget *widget,
                                    const QRect &rect,
                                    const QColorGroup &group,
                                    SFlags flags,
                                    const QStyleOption &option) const
{
    bool horiz = flags & Style_Horizontal;
    int x, y, w, h, x2, y2;

    rect.rect(&x, &y, &w, &h);
    x2 = rect.right();
    y2 = rect.bottom();

    switch (element) {
      case KPE_ToolBarHandle:
          if (horiz) { // horizontal
              drawQinxGradient(painter, rect, group.button(), false,
                               0, 0, w, h, true);

              painter->setPen(group.light());
              painter->drawLine(x+2, y, x+2, y2);
              painter->drawLine(x+5, y, x+5, y2);
              painter->drawLine(x+8, y, x+8, y2);

              painter->setPen(group.mid().dark(120));
              painter->drawLine(x+1, y, x+1, y2);
              painter->drawLine(x+4, y, x+4, y2);
              painter->drawLine(x+7, y, x+7, y2);

              painter->setPen(group.mid());
              painter->drawLine(x, y2, x2, y2);

          } else { // vertical
              drawQinxGradient(painter, rect, group.button(), true,
                               0, 0, w, h, true);

              painter->setPen(group.light());
              painter->drawLine(x, y+2, x2, y+2);
              painter->drawLine(x, y+5, x2, y+5);
              painter->drawLine(x, y+8, x2, y+8);

              painter->setPen(group.mid().dark(120));
              painter->drawLine(x, y+1, x2, y+1);
              painter->drawLine(x, y+4, x2, y+4);
              painter->drawLine(x, y+7, x2, y+7);

              painter->setPen(group.mid());
              painter->drawLine(x2, y, x2, y2);
          }
          break;

      case KPE_GeneralHandle:
          if (horiz) { // horizontal
              painter->fillRect(rect, group.brush(QColorGroup::Background));

              painter->setPen(group.light());
              painter->drawLine(x+2, y, x+2, y2);
              painter->drawLine(x+5, y, x+5, y2);

              painter->setPen(group.mid().dark(120));
              painter->drawLine(x+1, y, x+1, y2);
              painter->drawLine(x+4, y, x+4, y2);

          } else { // vertical
              painter->fillRect(rect, group.brush(QColorGroup::Background));

              painter->setPen(group.light());
              painter->drawLine(x, y+2, x2, y+2);
              painter->drawLine(x, y+5, x2, y+5);

              painter->setPen(group.mid().dark(120));
              painter->drawLine(x, y+1, x2, y+1);
              painter->drawLine(x, y+4, x2, y+4);
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
          break; // nothing

      case KPE_SliderGroove: {
          const QSlider* slider = dynamic_cast<const QSlider*>(widget);
          if (slider) {
              if (slider->orientation() == Horizontal) {
                  y +=  (h / 2) - 2;
                  h = 5;
                  y2 = y + h - 1;
              } else {
                  x += (w / 2) - 2;
                  w = 5;
                  x2 = x + w - 1;
              }
          }

          painter->setPen(group.light());
          painter->drawLine(x+1, y2, x2, y2);
          painter->drawLine(x2, y+1, x2, y2);

          painter->setPen(group.dark());
          painter->fillRect(x+1, y+1, w-2, h-2,
                            slider->isEnabled() ? group.dark() : group.mid());
          painter->setPen(group.mid());
          painter->drawPoint(x, y2);
          painter->drawPoint(x2, y);

          painter->setPen(group.shadow());
          painter->drawLine(x, y, x2-1, y);
          painter->drawLine(x, y+1, x, y2-1);
          break;
      }

      case KPE_SliderHandle:
          painter->setPen(group.light());
          painter->drawLine(x+2, y+2, x2-3, y+2);
          painter->drawLine(x+2, y+3, x+2, y2-3);

          painter->setPen(group.midlight());
          painter->drawPoint(x+2, y2-2);
          painter->drawPoint(x2-2, y+2);

          painter->setPen(group.mid());
          painter->drawLine(x+3, y2-2, x2-2, y2-2);
          painter->drawLine(x2-2, y+3, x2-2, y2-3);

          painter->setPen(group.dark());
          painter->drawLine(x2, y+2, x2, y2);
          painter->drawLine(x+2, y2, x2-1, y2);

          painter->setPen(group.shadow());
          painter->drawRect(x+1, y+1, w-2, h-2);

          drawQinxGradient(painter, QRect(x+3, y+3, w-6, h-6),
                           (widget==hover_) ?
                           group.button().light(contrast) : group.button(),
                           false, 0, 0, w-6, h-6, false);
          break;

      default:
          KStyle::drawKStylePrimitive(element, painter, widget, rect,
                                      group, flags, option);
    }
}

//////////////////////////////////////////////////////////////////////////////
// drawControl()
// -------------
// Draw the control

void QinxStyle::drawControl(ControlElement element,
                            QPainter *painter,
                            const QWidget *widget,
                            const QRect &rect,
                            const QColorGroup &group,
                            SFlags flags,
                            const QStyleOption &option ) const
{
    bool active, enabled;
    int x, y, w, h, dx;
    QMenuItem *mi;
    QIconSet::Mode mode;
    QIconSet::State state;
    QPixmap pixmap;

    rect.rect(&x, &y, &w, &h);

    switch (element) {
      case CE_PushButton:
          if (widget == hover_) flags |= Style_MouseOver;
          drawPrimitive(PE_ButtonCommand, painter, rect, group, flags);

          if (flags & Style_HasFocus) { // draw focus
              drawPrimitive(PE_FocusRect, painter,
                  QStyle::visualRect(subRect(SR_PushButtonFocusRect, widget),
                                     widget), group, flags);
          }
          break;

      case CE_PushButtonLabel: {
          const QPushButton* button = dynamic_cast<const QPushButton*>(widget);
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
                  drawItem(painter, QRect(x+n+1, y+1, w, h),
                           AlignCenter | ShowPrefix,
                           button->colorGroup(),
                           button->isEnabled(),
                           button->pixmap(),
                           button->text(), -1,
                           (button->isEnabled()) ?
                               &button->colorGroup().mid() :
                               &button->colorGroup().midlight());
              }
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
          const QButton *b = dynamic_cast<const QButton*>(widget);
          if (!b) return;

          int alignment = reverse_ ? AlignRight : AlignLeft;
          drawItem(painter, rect, alignment | AlignVCenter | ShowPrefix,
                   group, flags & Style_Enabled, b->pixmap(), b->text());

          // only draw focus if content (forms on html won't)
          if ((flags & Style_HasFocus) && (b->text().isEmpty() || (b->pixmap() != 0))) {
              drawPrimitive(PE_FocusRect, painter,
                            visualRect(subRect(SR_RadioButtonFocusRect,
                                               widget), widget),
                            group, flags);
          }
          break;
      }

      case CE_DockWindowEmptyArea:  {
          const QToolBar *tb = dynamic_cast<const QToolBar*>(widget);
          if (tb) {
              // toolbar not floating or in a QMainWindow
              if (flatToolbar(tb)) {
                  painter->fillRect(rect, tb->paletteBackgroundColor());
              }
          }
          break;
      }

      case CE_MenuBarEmptyArea:
          drawQinxGradient(painter, QRect(x, y, w, h), group.button(),
                           (w<h), 0, 0, 0, 0, true);
          break;

      case CE_MenuBarItem: {
          const QMenuBar *mbar = dynamic_cast<const QMenuBar*>(widget);
          if (!mbar) {
              KStyle::drawControl(element, painter, widget, rect, group,
                                  flags, option);
              return;
          }
          mi = option.menuItem();
          QRect prect = mbar->rect();

          if ((flags & Style_Active) && (flags & Style_HasFocus)) {
              if (flags & Style_Down) {
                  drawQinxPanel(painter, x, y, w, h, group, true,
                                 &group.brush(QColorGroup::Button));
              } else {
                  drawQinxBevel(painter, x, y, w, h,
                                 group, group.button(),
                                 false, false, false);
              }
          } else {
              drawQinxGradient(painter, rect, group.button(),
                               false, x, y,
                               prect.width()-2, prect.height()-2, true);
          }
          drawItem(painter, rect,
                   AlignCenter | AlignVCenter |
                   DontClip | ShowPrefix | SingleLine,
                   group, flags & Style_Enabled,
                   mi->pixmap(), mi->text());
          break;
      }

      case CE_PopupMenuItem: {
          const QPopupMenu *popup = dynamic_cast<const QPopupMenu*>(widget);
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
              painter->fillRect(x, y, w, h, group.button());
          }
          // Photon style background
          if (photonmenus_) {
              if (enabled && !active) {
                  painter->fillRect(x+2, y+1, w-4, h-2,
                                    group.button().dark(106));
              }
          }

          // draw separator
          if (mi->isSeparator()) {
              painter->setPen(group.mid());
              painter->drawLine(x, y+1, x+w-1, y+1);
              painter->setPen(group.shadow());
              painter->drawLine(x, y+2, x+w-1, y+2);
              painter->setPen(group.midlight());
              painter->drawLine(x, y+3, x+w-1, y+3);
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
                          group.buttonText()) :
                          group.mid()); // fix by kretz@kde.org

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
          const QTabBar* tb = dynamic_cast<const QTabBar*>(widget);
          if (tb) {
              // this guy can get complicated, we we do it elsewhere
              drawQinxTab(painter, x, y, w, h, group, tb, option,
                          (flags & Style_Selected));
          } else { // not a tabbar
              KStyle::drawControl(element, painter, widget, rect, group,
                                  flags, option);
              return;
          }
          break;
      }

      case CE_TabBarLabel: {
          const QTabBar* tabbar = dynamic_cast<const QTabBar*>(widget);
          const QTab* tab = dynamic_cast<const QTab*>(option.tab());
          if (!tabbar || !tab) {
              KStyle::drawControl(element, painter, widget, rect, group,
                                  flags, option);
              return;
          }
          bool alignleft = (photontabs_ &&
                            (tabbar->shape() == QTabBar::RoundedAbove));

          QRect trect = rect;
          if ((alignleft) && (!tab->iconSet())) {
              trect.moveLeft(x-6); // adjust left
          }

          int alignment = (alignleft ? AlignLeft : AlignCenter) | ShowPrefix;
          drawItem(painter, trect, alignment, group,
                   flags & Style_Enabled, 0, tab->text(), -1, &group.text());

          if ((flags & Style_HasFocus) && !tab->text().isEmpty())
              drawPrimitive(PE_FocusRect, painter, trect, group);
          break;
      }

      case CE_ProgressBarGroove: {
          drawQinxPanel(painter, x+1, y+1, w-2, h-2, group, true,
                        &group.brush(QColorGroup::Mid));
          break;
      }

      case CE_ProgressBarContents: {
          const QProgressBar* pbar = dynamic_cast<const QProgressBar*>(widget);
          if (!pbar) {
              KStyle::drawControl(element, painter, widget, rect, group,
                                  flags, option);
              return;
          }
          subRect(SR_ProgressBarContents, widget).rect(&x, &y, &w, &h);

          if (!pbar->totalSteps()) {
              // busy indicator
              int bar = pixelMetric(PM_ProgressBarChunkWidth, widget) + 2;
              int progress = pbar->progress() % ((w-bar) * 2);
              if (progress > (w-bar)) progress = 2 * (w-bar) - progress;
              drawQinxBevel(painter, x+progress, y, bar, h,
                            group, group.button(),
                            false, false, true);
          } else {
              double progress = static_cast<double>(pbar->progress()) /
                  static_cast<double>(pbar->totalSteps());
              dx = static_cast<int>(w * progress);
              if (dx < 4) break;
              if (reverse_) x += w - dx;
              drawQinxBevel(painter, x, y, dx, h,
                            group, group.button(),
                            false, false, true);
          }
          break;
      }

      case CE_ProgressBarLabel: {
          const QProgressBar* pbar = dynamic_cast<const QProgressBar*>(widget);
          if (!pbar) {
              KStyle::drawControl(element, painter, widget, rect, group,
                                  flags, option);
              return;
          }
          enabled = pbar->isEnabled();
          if (!enabled) {
              painter->setPen(group.light());
              painter->drawText(x+1, y+1, w, h,
                                AlignCenter | SingleLine,
                                pbar->progressString());
              painter->setPen(group.mid()); // fix by kretz@kde.org
          }
          painter->drawText(rect, AlignCenter | SingleLine,
                            pbar->progressString());
          break;
      }

      default:
          KStyle::drawControl(element, painter, widget, rect, group,
                              flags, option);
    }
}

//////////////////////////////////////////////////////////////////////////////
// drawControlMask()
// -----------------
// Draw a bitmask for the element

void QinxStyle::drawControlMask(ControlElement element,
                                QPainter *painter,
                                const QWidget *widget,
                                const QRect &rect,
                                const QStyleOption &option) const
{
    switch (element) {
      case CE_PushButton: {
          painter->fillRect(rect, Qt::color1);
          painter->setPen(Qt::color0);
          break;
      }

      default:
          KStyle::drawControlMask(element, painter, widget, rect, option);
    }
}

//////////////////////////////////////////////////////////////////////////////
// drawComplexControl()
// --------------------
// Draw a complex control

void QinxStyle::drawComplexControl(ComplexControl control,
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
    bool sunken;
    QRect subrect;
    int x, y, w, h, x2, y2;
    rect.rect(&x, &y, &w, &h);

    switch (control) {
      case CC_ComboBox: {
          const QComboBox * combo = dynamic_cast<const QComboBox*>(widget);
          if (!combo) {
              KStyle::drawComplexControl(control, painter, widget, rect, group,
                                         flags, controls, active, option);
              return;
          }

          if (active == SC_ComboBoxArrow) flags |= Style_Sunken;
          if (widget == hover_) flags |= Style_MouseOver;
          drawPrimitive(PE_ButtonCommand, painter, rect, group, flags);

          if (controls & SC_ComboBoxArrow) { // draw arrow box
              subrect = visualRect(querySubControlMetrics(CC_ComboBox, widget,
                                   SC_ComboBoxArrow), widget);
              subrect.coords(&x, &y, &x2, &y2);
              drawPrimitive(PE_ArrowDown, painter, subrect, group, flags);
              if (reverse_) x = x2-1;
              painter->setPen(group.dark());
              painter->drawLine(x, y+2, x, y2-2);
              painter->setPen(group.light());
              painter->drawLine(x+1, y+2, x+1, y2-2);
          }

          if (controls & SC_ComboBoxEditField) { // draw edit box
              if (combo->editable()) { // editable box
                  subrect = visualRect(querySubControlMetrics(CC_ComboBox,
                                       widget, SC_ComboBoxEditField), widget);
                  subrect.addCoords(-1, -1, 0, 1);
                  painter->setPen(group.base());
                  painter->drawRect(subrect);
              } else if (combo->hasFocus()) { // non editable box
                  subrect = visualRect(subRect(SR_ComboBoxFocusRect,
                                               combo), widget);
                  drawPrimitive(PE_FocusRect, painter, subrect, group,
                                Style_FocusAtBorder,
                                QStyleOption(group.highlight()));
              }
          }
          painter->setPen(group.foreground()); // for subsequent text
          break;
      }

      case CC_SpinWidget: {
          const QSpinWidget *spin = dynamic_cast<const QSpinWidget*>(widget);
          if (!spin) {
              KStyle::drawComplexControl(control, painter, widget, rect, group,
                                         flags, controls, active, option);
              return;
          }

          PrimitiveElement element;
          QColor color = (widget==hover_)
              ? group.button().light(contrast)
              : group.button();

          // draw frame
          if (controls & SC_SpinWidgetFrame) {
              drawQinxPanel(painter, x, y, w, h,
                            group, true, NULL);
          }

          // draw up arrow
          if (controls & SC_SpinWidgetUp) {
              subrect = spin->upRect();
              subrect.coords(&x, &y, &x2, &y2);

              sunken = (active == SC_SpinWidgetUp);
              if (spin->buttonSymbols() == QSpinWidget::PlusMinus)
                  element = PE_SpinWidgetPlus;
              else
                  element = PE_SpinWidgetUp;

              drawQinxGradient(painter, subrect, color, false);
              painter->setPen(sunken ? group.mid() : group.light());
              painter->drawLine(x, y, x2, y);
              painter->drawLine(x, y+1, x, y2);

              painter->setPen(sunken ? group.light() : group.mid());
              painter->drawLine(x, y2, x2, y2);
              painter->drawLine(x2, y, x2, y2-1);

              painter->setPen(group.dark());
              if (reverse_)
                  painter->drawLine(x2+1, y, x2+1, y2);
              else
                  painter->drawLine(x-1, y, x-1, y2);

              drawPrimitive(element, painter, subrect, group, flags
                            | Style_Enabled
                            | ((active == SC_SpinWidgetUp) ?
                               Style_On | Style_Sunken : Style_Raised));
          }

          // draw down button
          if (controls & SC_SpinWidgetDown) {
              subrect = spin->downRect();
              subrect.coords(&x, &y, &x2, &y2);

              sunken = (active == SC_SpinWidgetDown);
              if (spin->buttonSymbols() == QSpinWidget::PlusMinus)
                  element = PE_SpinWidgetMinus;
              else
                  element = PE_SpinWidgetDown;

              drawQinxGradient(painter, subrect, color, false);
              painter->setPen(sunken ? group.mid() : group.light());
              painter->drawLine(x, y, x2, y);
              painter->drawLine(x, y+1, x, y2);

              painter->setPen(sunken ? group.light() : group.mid());
              painter->drawLine(x, y2, x2, y2);
              painter->drawLine(x2, y, x2, y2-1);

              painter->setPen(group.dark());
              if (reverse_)
                  painter->drawLine(x2+1, y, x2+1, y2);
              else
                  painter->drawLine(x-1, y, x-1, y2);

              drawPrimitive(element, painter, subrect, group, flags
                            | Style_Enabled
                            |((active == SC_SpinWidgetDown) ?
                              Style_On | Style_Sunken : Style_Raised));
          }
          break;
      }

      case CC_ToolButton: {
          const QToolButton *btn = dynamic_cast<const QToolButton*>(widget);
          if (!btn) {
              KStyle::drawComplexControl(control, painter, widget, rect, group,
                                         flags, controls, active, option);
              return;
          }

          QToolBar *toolbar;
          bool horiz = true;      // orientation of toolbar
          bool normal = !(down || on || raised); // normal button state

          x2 = rect.right();
          y2 = rect.bottom();

          // check for QToolBar parent
          if (btn->parent() && btn->parent()->inherits("QToolBar")) {
              toolbar = dynamic_cast<QToolBar*>(btn->parent());
              if (toolbar) {
                  horiz = (toolbar->orientation() == Qt::Horizontal);
                  if (normal) { // draw background
                      if (flatToolbar(toolbar)) {
                          // toolbar not floating or in a QMainWindow
                          painter->fillRect(rect, group.button());
                      } else {
                          drawQinxGradient(painter, rect, group.button(),
                                           !horiz, 0, 0,
                                           toolbar->width()-1,
                                           toolbar->height()-1, true);
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
          // QToolBarExtensionWidget parent
          else if (btn->parent() &&
                   btn->parent()->inherits(QTOOLBAREXTENSION)) {
              QWidget *extension;
              if ((extension = dynamic_cast<QWidget*>(btn->parent()))) {
                  toolbar = dynamic_cast<QToolBar*>(extension->parent());
                  if (toolbar){
                      horiz = (toolbar->orientation() == Qt::Horizontal);
                      if (normal) { // draw background
                          drawQinxGradient(painter, rect, group.button(),
                                           !horiz, 0, 0, toolbar->width()-1,
                                           toolbar->height()-1, true);
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
              painter->fillRect(rect, group.button());
          }

          // now draw active buttons
          if (down || on) {
              drawQinxPanel(painter, x, y, w, h, group, true,
                            &group.brush(QColorGroup::Button));
          } else if (raised) {
              drawQinxBevel(painter, x, y, w, h, group, group.button(),
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

void QinxStyle::drawComplexControlMask(ComplexControl control,
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

int QinxStyle::pixelMetric(PixelMetric metric, const QWidget *widget) const
{
    switch (metric) {
      case PM_ButtonDefaultIndicator:   // size of default indicator
          return 0;

      case PM_ButtonMargin:             // space betweeen frame and label
          return 4;

      case PM_ExclusiveIndicatorWidth:  // radiobutton size
      case PM_ExclusiveIndicatorHeight:
      case PM_IndicatorWidth:           // checkbox size
      case PM_IndicatorHeight:
          return 13;

      case PM_MenuButtonIndicator:      // arrow width
          return 7;

      case PM_TabBarTabOverlap:         // Amount of tab overlap
          if (photontabs_) {
              if (const QTabBar *tb = dynamic_cast<const QTabBar*>(widget)) {
                  if (tb->shape() == QTabBar::RoundedAbove)
                      return 12;
              }
          }
          return 2;

      case PM_TabBarTabHSpace:          // extra tab spacing
      if (photontabs_) {
          return 32;
      } else {
          return 24;
      }

      case PM_TabBarTabVSpace:
          if (const QTabBar *tb = dynamic_cast<const QTabBar*>(widget)) {
              if (tb->shape() == QTabBar::RoundedAbove)
                  return 9;
              if (tb->shape() == QTabBar::RoundedBelow)
                  return 6;
          }
          return 0;

      case PM_DockWindowHandleExtent:
          return 10;

      default:
          return KStyle::pixelMetric(metric, widget);
    }
}

//////////////////////////////////////////////////////////////////////////////
// querySubControlMetrics()
// ------------------------
// Get metrics for subcontrols of complex controls

QRect QinxStyle::querySubControlMetrics(ComplexControl control,
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
          xc = h/2 + ITEMHMARGIN*2;

          switch (subcontrol) {
            case SC_ComboBoxFrame: // total combobox area
                rect = widget->rect();
                break;

            case SC_ComboBoxArrow: // the right side
                rect.setRect(w-xc, 0, xc, h);
                break;

            case SC_ComboBoxEditField: // the left side
                rect.setRect(fw+1, fw+1, w-xc-fw-1, h-(fw*2)-2);
                break;

            case SC_ComboBoxListBoxPopup: // the list popup box
                rect = option.rect();
                break;

            default:
                break;
          }
          break;
      }

      case CC_SpinWidget: {
          bool odd = widget->height() % 2;
          xc =  h/2 + ITEMHMARGIN*2;

          switch (subcontrol) {
            case SC_SpinWidgetButtonField:
                rect.setRect(w-xc, fw, xc-fw, h-fw*2-2);
                break;

            case SC_SpinWidgetEditField:
                rect.setRect(fw, fw, w-xc-fw-1, h-(fw*2));
                break;

            case SC_SpinWidgetFrame:
                rect = widget->rect();
                break;

            case SC_SpinWidgetUp:
                rect.setRect(w-xc, fw, xc-fw, h/2-fw);
                break;

            case SC_SpinWidgetDown:
                rect.setRect(w-xc, h/2, xc-fw, h/2-fw+(odd?1:0));
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

QSize QinxStyle::sizeFromContents(ContentsType contents,
                                  const QWidget* widget,
                                  const QSize &contentsize,
                                  const QStyleOption &option ) const
{
    int w = contentsize.width();
    int h = contentsize.height();

    switch (contents) {
      case CT_PushButton: {
          const QPushButton* button = dynamic_cast<const QPushButton*>(widget);
          if (!button) {
              return KStyle::sizeFromContents(contents, widget, contentsize,
                                              option);
          }
          int margin = pixelMetric(PM_ButtonMargin, widget)
              + pixelMetric(PM_DefaultFrameWidth, widget) * 2;

          w += margin + 6; // add room for bold font
          h += margin;

          // standard width and heights.
          if (button->isDefault() || button->autoDefault()) {
              if (w < 80 && !button->pixmap()) w = 80;
          }
          if (h < 22) h = 22;
          return QSize(w, h);
      }

      case CT_PopupMenuItem: {
          if (!widget || option.isDefault()) return contentsize;
          const QPopupMenu *popup = dynamic_cast<const QPopupMenu*>(widget);
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
          } else if (item->widget()) {    // a menu item that is a widget
              w = contentsize.width();    // fix by sgehn@gmx.net
              h = contentsize.height();
          } else if (item->isSeparator()) {
              w = h = 5;
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

bool QinxStyle::flatToolbar(const QToolBar *toolbar) const
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

bool QinxStyle::eventFilter(QObject *object, QEvent *event)
{
    if (KStyle::eventFilter(object, event)) return true;
    if (!object->isWidgetType()) return false;

    bool horiz;
    int x, y, w, h;
    QToolBar *toolbar;
    QWidget *widget;

    // painting events
    if (event->type() == QEvent::Paint) {
        // make sure we do the most specific stuff first

        // KDE Toolbar Widget
        // patch by Daniel Brownlees <dbrownlees@paradise.net.nz>
        if (object->parent() && !qstrcmp(object->name(), KTOOLBARWIDGET)) {
            if (0 == (widget = dynamic_cast<QWidget*>(object))) return false;
            QWidget *parent = dynamic_cast<QWidget*>(object->parent());
            int px = widget->x(), py = widget->y();
            // find the toolbar
            while (parent && parent->parent()
                   && !dynamic_cast<QToolBar*>(parent)) {
                px += parent->x();
                py += parent->y();
                parent = dynamic_cast<QWidget*>(parent->parent());
            }
            if (!parent) return false;
            widget->rect().rect(&x, &y, &w, &h);
            QRect prect = parent->rect();

            toolbar = dynamic_cast<QToolBar*>(parent);
            horiz = (toolbar) ? (toolbar->orientation() == Qt::Horizontal)
                : (prect.height() < prect.width());

            QPainter painter(widget);
            if (flatToolbar(toolbar)) {
                painter.fillRect(widget->rect(),
                                  parent->colorGroup().background());
            } else {
                drawQinxGradient(&painter, widget->rect(),
                                 parent->colorGroup().button(),
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
        else if ((object->parent()) &&
                 ((toolbar = dynamic_cast<QToolBar*>(object->parent())))) {
            if (event->type() == QEvent::Paint) {
                widget = static_cast<QWidget*>(object);
                horiz = (toolbar->orientation() == Qt::Horizontal);
                QPainter painter(widget);
                QRect wrect = widget->rect();
                wrect.rect(&x, &y, &w, &h);
                // draw the extension
                drawQinxGradient(&painter, wrect,
                                 toolbar->colorGroup().button(),
                                 !horiz, x, y, w-1, h-1, true);
                if (horiz) {
                    painter.setPen(toolbar->colorGroup().dark());
                    painter.drawLine(w-1, 0, w-1, h-1);
                    painter.setPen(toolbar->colorGroup().mid());
                    painter.drawLine(w-2, 0, w-2, h-2);
                    painter.drawLine(x, h-1, w-2, h-1);
                    painter.drawLine(x, y, x, h-2);
                    painter.setPen(toolbar->colorGroup().light());
                    painter.drawLine(x+1, y, x+1, h-2);
                } else {
                    painter.setPen(toolbar->colorGroup().dark());
                    painter.drawLine(0, h-1, w-1, h-1);
                    painter.setPen(toolbar->colorGroup().mid());
                    painter.drawLine(0, h-2, w-2, h-2);
                    painter.drawLine(w-1, y, w-1, h-2);
                    painter.drawLine(x, y, w-2, y);
                    painter.setPen(toolbar->colorGroup().light());
                    painter.drawLine(x, y+1, w-2, y+1);
                }
            }
        }

    } else if (highlights_) { // "mouseover" events
        if (::qt_cast<QPushButton*>(object) ||
            ::qt_cast<QComboBox*>(object) ||
            ::qt_cast<QSpinWidget*>(object) ||
            ::qt_cast<QSlider*>(object) ||
            object->inherits(QSPLITTERHANDLE)) {
            if (event->type() == QEvent::Enter) {
                if ((widget = dynamic_cast<QWidget*>(object)) &&
                    widget->isEnabled()) {
                    hover_ = widget;
                    widget->repaint(false);
                }
            } else if (event->type() == QEvent::Leave) {
                if ((widget = dynamic_cast<QWidget*>(object)) &&
                    widget->isEnabled()) {
                    hover_ = 0;
                    widget->repaint(false);
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

class QinxStylePlugin : public QStylePlugin
{
public:
    QinxStylePlugin();
    QStringList keys() const;
    QStyle *create(const QString &key);
};

QinxStylePlugin::QinxStylePlugin() : QStylePlugin() { ; }

QStringList QinxStylePlugin::keys() const
{
    return QStringList() << "Qinx";
}

QStyle* QinxStylePlugin::create(const QString& key)
{
    if (key.lower() == "qinx")
        return new QinxStyle();
    return 0;
}

KDE_Q_EXPORT_PLUGIN(QinxStylePlugin);

#include "qinxstyle.moc"
