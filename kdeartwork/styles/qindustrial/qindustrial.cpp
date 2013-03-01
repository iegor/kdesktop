/*
 * GTK Industrial Theme
 * Designed by Tigert and Jimmac
 * Implemented by Christopher James Lahey
 *
 * Based on GTK Bluecurve Theme
 * Designed by Garrett LeSage
 * Implemented by Alexander Larsson
 *
 * Parts of the code based on Metal theme, Copyright:
 * Randy Gordon, randy@integrand.com
 * Owen Taylor <otaylor@redhat.com>
 *
 * Changes:
 *
 *   xx.02.2005: menubar changes, minor spacing tweaks. <clemens@endorphin.org>
 *   10.04.2005: reworked color code completely to use KDE colours <clemens@endorphin.org>
 *
 * License: GPL (GNU Public License)
 */


/*

  This theme is based on:

  QtCurve (C) Craig Drummond, 2003 Craig.Drummond@lycos.co.uk

----

  Based upon B???Curve style (Copyright (c) 2002 R?d H?t, Inc)
      Bernhard Rosenkr�nzer <bero@r?dh?t.com>
      Preston Brown <pbrown@r?dh?t.com>
      Than Ngo <than@r?dh?t.com>

  Released under the GNU General Public License (GPL) v2.

----

  B???Curve is based on the KDE Light style, 2nd revision:
  Copyright(c)2000-2001 Trolltech AS (info@trolltech.com)

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files(the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
*/


#include <kdeversion.h>
#include <qmenubar.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qdrawutil.h>
#include <qscrollbar.h>
#include <qtabbar.h>
#include <qtabwidget.h>
#include <qguardedptr.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qbitmap.h>
#include <qimage.h>
#include <qcombobox.h>
#include <qslider.h>
#include <qstylefactory.h>
#include <qcleanuphandler.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qtoolbutton.h>
#include <qtoolbar.h>
#include <qprogressbar.h>
#include <qcursor.h>
#include <qheader.h>
#include <qwidgetstack.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "qindustrial.h"
#include "common.h"
#include <qsettings.h>

#include <qfile.h>
#include <qdir.h>

// Nice ...
#undef  MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#undef  MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#undef  CLAMP
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

/* Images */

#define PIXMAP_CHECK_ACTIVE 0
#define PIXMAP_CHECK_INACTIVE 1
#define PIXMAP_CHECK_MIXED 2
#define PIXMAP_MEDIUM_CHECK_ACTIVE 3
#define PIXMAP_MEDIUM_CHECK_INACTIVE 4
#define PIXMAP_MEDIUM_CHECK_MIXED 5
#define PIXMAP_SMALL_CHECK_ACTIVE 6
#define PIXMAP_SMALL_CHECK_INACTIVE 7
#define PIXMAP_SMALL_CHECK_MIXED 8

#define PIXMAP_RADIO_ACTIVE 9
#define PIXMAP_RADIO_INACTIVE 10
#define PIXMAP_RADIO_MIXED 11
#define PIXMAP_SMALL_RADIO_ACTIVE 12
#define PIXMAP_SMALL_RADIO_INACTIVE 13
#define PIXMAP_SMALL_RADIO_MIXED 14

#define PIXMAP_COUNT 15

#include "checkbox-on.xpm"
#include "checkbox-off.xpm"
#include "checkbox-mixed.xpm"
#include "table-checkbox-on.xpm"
#include "table-checkbox-off.xpm"
#include "table-checkbox-mixed.xpm"
#include "menu-checkbox-on.xpm"
#include "menu-checkbox-off.xpm"
#include "menu-checkbox-mixed.xpm"
#include "radio-on.xpm"
#include "radio-off.xpm"
#include "radio-mixed.xpm"
#include "menu-radio-on.xpm"
#include "menu-radio-off.xpm"
#include "menu-radio-mixed.xpm"

#include "menu-check-8.xpm"
#include "menu-check-11.xpm"
#include "menu-check-13.xpm"

using namespace Industrial;

const QColorGroup _cg;


static const double shade_data[SHADE_COUNT] = { 0.0   / 253.0,   /* 0  - SHADE_FG   */
    100.0 / 253.0,   /* 1  - SHADE_DARK 0x64 */
    158.0 / 253.0,   /* 2  - SHADE_0x9E */
    179.0 / 253.0,   /* 3  - SHADE_0xB3 */
    187.0 / 253.0,   /* 4  - SHADE_0xBB */
    190.0 / 253.0,   /* 5  - SHADE_0xBE */
    199.0 / 253.0,   /* 6  - SHADE_0xC7 */
    200.0 / 253.0,   /* 7  - SHADE_0xC8 */
    202.0 / 253.0,   /* 8  - SHADE_0xCA */
    209.0 / 253.0,   /* 9  - SHADE_0xD1 */
    213.0 / 253.0,   /* 10 - SHADE_0xD5 */
    217.0 / 253.0,   /* 11 - SHADE_0xD9 */
    218.0 / 253.0,   /* 12 - SHADE_0xDA */
    231.0 / 253.0,   /* 13 - SHADE_0xE7 */
    234.0 / 253.0,   /* 14 - SHADE_0xEA */
    235.0 / 253.0,   /* 15 - SHADE_0xEB */
    236.0 / 253.0,   /* 16 - SHADE_0xEC */
    242.0 / 253.0,   /* 17 - SHADE_0xF2 */
    243.0 / 253.0,   /* 18 - SHADE_0xF3 */
    244.0 / 253.0,   /* 19 - SHADE_0xF4 */
    246.0 / 253.0,   /* 20 - SHADE_0xF6 */
    247.0 / 253.0,   /* 21 - SHADE_0xF7 */
    248.0 / 253.0,   /* 22 - SHADE_0xF8 */
    250.0 / 253.0,   /* 23 - SHADE_0xFA */
    251.0 / 253.0,   /* 24 - SHADE_0xFB */
    252.0 / 253.0,   /* 25 - SHADE_0xFC */
    253.0 / 253.0,   /* 26 - SHADE_0xFD */
    255.0 / 253.0,   /* 27 - SHADE_0xFF */
    50.0  / 100.0,   /* 28 - SHADE_HALF */
    52.0  / 83.0
                                              }; /* 29 - SHADE_NEG 0x34 / 0x53 */
/* 135.0 /  83.0 26 - SHADE_NEG (0x53 + 0x34) / 0x53 */

static const char ** pixmaps[PIXMAP_COUNT] = { checkbox_on_xpm,
    checkbox_off_xpm,
    checkbox_mixed_xpm,
    table_checkbox_on_xpm,
    table_checkbox_off_xpm,
    table_checkbox_mixed_xpm,
    menu_checkbox_on_xpm,
    menu_checkbox_off_xpm,
    menu_checkbox_mixed_xpm,
    radio_on_xpm,
    radio_off_xpm,
    radio_mixed_xpm,
    menu_radio_on_xpm,
    menu_radio_off_xpm,
    menu_radio_mixed_xpm
                                             };

#define QTC_CHECK_SIZE 13
#define QTC_RADIO_SIZE 13
#define QTC_HIGHLIGHT_FACTOR 105
#define QTC_BORDERED_FRAME_WIDTH 2
#define QTC_DEF_FRAME_WIDTH 1
#define QTC_MIN_BTN_SIZE 16    // was 12
#define QTC_NO_SECT -1
#define QTC_ICON_INSET 10
#define QTC_BUTTON_WIDTH_EXTRA QTC_ICON_INSET
#define QTC_MIN_BTN_WIDTH 84

static void rgb2hls(double *r, double *g, double *b)
{
  double min,
  max,
  red=*r,
      green=*g,
            blue=*b,
                 h=0,
                   l,
                   s=0,
                     delta;

  if (red > green)
  {
    max=QMAX(red, blue);
    min=QMIN(green, blue);
  }
  else
  {
    max=QMAX(green, blue);
    min=QMIN(red, blue);
  }

  l=(max +min)/ 2;
  s=0;
  h=0;

  if (max !=min)
  {
    if (l <=0.5)
      s=(max-min)/(max +min);
    else
      s=(max-min)/(2-max-min);

    delta=max-min;

    if (red==max)
      h=(green-blue)/ delta;
    else
      if (green==max)
        h=2 +(blue-red)/ delta;
      else
        if (blue==max)
          h=4 +(red-green)/ delta;

    h *=60;
    if (h < 0.0)
      h +=360;
  }

  *r=h;
  *g=l;
  *b=s;
}

static void hls2rgb(double *h, double *l, double *s)
{
  double hue,
  lightness=*l,
            saturation=*s,
                       m1,
                       m2,
                       r,
                       g,
                       b;

  if (lightness <=0.5)
    m2=lightness *(1 +saturation);
  else
    m2=lightness +saturation-lightness * saturation;
  m1=2 * lightness-m2;

  if (0==saturation)
  {
    *h=lightness;
    *l=lightness;
    *s=lightness;
  }
  else
  {
    hue=*h +120;
    while (hue > 360)
      hue-=360;
    while (hue < 0)
      hue +=360;

    if (hue < 60)
      r=m1 +(m2-m1)* hue / 60;
    else
      if (hue < 180)
        r=m2;
      else
        if (hue < 240)
          r=m1 +(m2-m1)*(240-hue)/ 60;
        else
          r=m1;

    hue=*h;
    while (hue > 360)
      hue-=360;
    while (hue < 0)
      hue +=360;

    if (hue < 60)
      g=m1 +(m2-m1)* hue / 60;
    else
      if (hue < 180)
        g=m2;
      else
        if (hue < 240)
          g=m1 +(m2-m1)*(240-hue)/ 60;
        else
          g=m1;

    hue=*h-120;
    while (hue > 360)
      hue-=360;
    while (hue < 0)
      hue +=360;

    if (hue < 60)
      b=m1 +(m2-m1)* hue / 60;
    else
      if (hue < 180)
        b=m2;
      else
        if (hue < 240)
          b=m1 +(m2-m1)*(240-hue)/ 60;
        else
          b=m1;

    *h=r;
    *l=g;
    *s=b;
  }
}

inline int limit(double c)
{
  return c < 0
         ? 0
         : c > 255
         ? 255
         : (int)c;
}

static void shadeBetween(const QColor &a, const QColor &fg, QColor *b, float k)
{
  double red=a.red()/256.0,
             green=a.green()/256.0,
                   blue=a.blue()/256.0;
  double red_fg=fg.red()/256.0,
                green_fg=fg.green()/256.0,
                         blue_fg=fg.blue()/256.0;

  rgb2hls(&red, &green, &blue);
  rgb2hls(&red_fg, &green_fg, &blue_fg);

  green = (green - green_fg) * k + green_fg;
  if (green > 1.0)
    green = 1.0;
  else
    if (green < 0.0)
      green = 0.0;

  hls2rgb(&red, &green, &blue);

  b->setRgb(limit(red*256), limit(green*256), limit(blue*256));
}

#if 0
QColor
QIndustrialStyle::shadeColor(int flags, int shade) const
{
  return shadeColorFull(getBg(flags), getFg(flags), shade);
}
#endif

QColor QIndustrialStyle::shadeColorState(int state, const QColorGroup &cg, int shade) const
{
  //QColor tfg = fg[state];
  //QColor tbg = bg[state];
  //    return shadeColorFull(tbg, tfg, shade);
  return shadeColorFull(cg.background(), cg.foreground(), shade);

}

QColor
QIndustrialStyle::shadeColorFromCG(const QColorGroup &cg, int widgetClass, int shade) const
{
  switch (widgetClass) {
  case WDM_STANDARD:
    return shadeColorFull(cg.background(), cg.foreground(), shade);
  case WDM_HIGHLIGHTED:
    return shadeColorFull(cg.background(), cg.highlight(), shade);
  case WDM_BUTTON:
    return shadeColorFull(cg.background(), cg.button(), shade);
  case WDM_LINK:
    return shadeColorFull(cg.background(), cg.link(), shade);
  case WDM_WINDOW:
    return shadeColorFull(cg.background(), cg.foreground(), shade);

  }
  //     return black;
  //return shadeColorFull(getBg( flags ), getFg( flags ), shade);
  return black;
}

QColor
QIndustrialStyle::shadeColorFull(const QColor &bg, const QColor &fg, int shade) const
{
  double k;
  QColor result;

  if (shade == SHADE_HALF ||
      shade == SHADE_NEG)
    k = shade_data[shade];
  else
    k = CLAMP((shade_data[shade] - contrast_center) *
              contrast + contrast_center, 0.0, 1.0);

  shadeBetween(bg, fg, &result, k);

  return result;
}

static GtkStateType stateFromFlags(int flags)
{
  GtkStateType t = GTK_STATE_NORMAL;

  if (flags & (QStyle::Style_Selected))
    t = GTK_STATE_SELECTED;
  else
    if (flags & QStyle::Style_MouseOver)
      t = GTK_STATE_PRELIGHT;
    else
      if (flags & (QStyle::Style_Down | QStyle::Style_On))
        t = GTK_STATE_ACTIVE;
      else
        if (!flags & (QStyle::Style_Enabled | QStyle::Style_Active))
          t = GTK_STATE_INSENSITIVE;

  return t;
}

#if 0
QColor QIndustrialStyle::getBg(int flags) const
{
  return bg[ stateFromFlags(flags)];
}


QColor QIndustrialStyle::getFg(int flags) const
{
  return fg[ stateFromFlags(flags)];
}
#endif

#define SAVE_CLIP(_p, _clip) \
  QRegion _oldClip; \
  if (_clip) \
  { \
    _oldClip = (_p)->clipRegion(); \
    (_p)->setClipRect(*_clip); \
  }

#define RESTORE_CLIP(_p,_clip) \
  if (_clip) \
    (_p)->setClipRegion(_oldClip);

void
QIndustrialStyle::drawKaleidoscopePoint(QPainter *p, const QColor &col,
                                        const QRect *clip, const QRect &rect,
                                        int offset_x, int offset_y) const
{
  SAVE_CLIP(p, clip)

  p->setPen(col);

  /* No mirror */
  p->drawPoint(rect.x() + offset_x, rect.y() + offset_y);
  /* Mirror x */
  p->drawPoint(rect.x() + rect.width() - offset_x - 1, rect.y() + offset_y);
  /* Mirror y */
  p->drawPoint(rect.x() + offset_x, rect.y() + rect.height() - offset_y - 1);
  /* Mirror both */
  p->drawPoint(rect.x() + rect.width() - offset_x - 1, rect.y() + rect.height() - offset_y - 1);
  /* Mirror diagonal */
  if (offset_x != offset_y) {
    p->drawPoint(rect.x() + offset_y, rect.y() + offset_x);
    p->drawPoint(rect.x() + rect.width() - offset_y - 1, rect.y() + offset_x);
    p->drawPoint(rect.x() + offset_y, rect.y() + rect.height() - offset_x - 1);
    p->drawPoint(rect.x() + rect.width() - offset_y - 1, rect.y() + rect.height() - offset_x - 1);
  }

  RESTORE_CLIP(p, clip)
}

void
QIndustrialStyle::drawKaleidoscopeLine(QPainter *p, const QColor &col,
                                       const QRect *clip, const QRect &rect,
                                       int edge_distance_offset,
                                       int directional_offset) const
{
  SAVE_CLIP(p, clip);

  p->setPen(col);

  /* Top line */
  p->drawLine(rect.x() + directional_offset,
              rect.y() + edge_distance_offset,
              rect.x() + rect.width() - directional_offset - 1,
              rect.y() + edge_distance_offset);

  /* Bottom line */
  p->drawLine(rect.x() + directional_offset,
              rect.y() + rect.height() - edge_distance_offset - 1,
              rect.x() + rect.width() - directional_offset - 1,
              rect.y() + rect.height() - edge_distance_offset - 1);

  /* Left line */
  p->drawLine(rect.x() + edge_distance_offset,
              rect.y() + directional_offset,
              rect.x() + edge_distance_offset,
              rect.y() + rect.height() - directional_offset - 1);

  /* Right line */
  p->drawLine(rect.x() + rect.width() - edge_distance_offset - 1,
              rect.y() + directional_offset,
              rect.x() + rect.width() - edge_distance_offset - 1,
              rect.y() + rect.height() - directional_offset - 1);

  RESTORE_CLIP(p, clip);
}

void
QIndustrialStyle::drawRoundedRectOnePixel(QPainter *p, const QColor &bg,
    const QColor &fg, const QColor &corner,
    const QRect &rect, bool fill) const
{
  /*
    ... ...
    .+@ @+.
    .@. .@.

    .@. .@.
    .+@ @+.
    ... ...

    The spaces indicate repeat the line on either side (if they don't match, it's a bug in the drawing.)
    Set of . around the edge not part of width & height.
  */
  if (fill)
    p->fillRect(rect, bg);

  /* Draw @s */
  drawKaleidoscopeLine(p, fg, NULL, rect, 0, 0);

  /* Draw +s */
  drawKaleidoscopePoint(p, corner, NULL, rect, 0, 0);
}

void QIndustrialStyle::drawRoundedRectTwoPixel(QPainter *p, const QColor &bg,
    const QColor &fg, const QColor &corner,
    const QRect &rect, bool fill) const
{
  /*
    .... ....
    ..+@ @+..
    .+@. .@+.
    .@.. ..@.

    .@.. ..@.
    .+@. .@+.
    ..+@ @+..
    .... ....

    The spaces indicate repeat the line on either side (if they don't match, it's a bug in the drawing.)
    Set of . around the edge not part of width & height.
  */
  if (fill)
    p->fillRect(rect, bg);

  /* Draw @s */
  drawKaleidoscopeLine(p, fg, NULL, rect, 0, 2);
  drawKaleidoscopePoint(p, fg, NULL, rect, 1, 1);

  /* Draw +s */
  drawKaleidoscopePoint(p, corner, NULL, rect, 0, 1);
}

void QIndustrialStyle::realDrawGrid(QPainter *p, const QRect &r, const QColorGroup &cg, int widgetClass) const
{
  int i, j, k, l;
  int i2, j2;
  int xoffset = 0;
  int yoffset = 0;
  struct {
    int xoff;
    int yoff;
    int color;
  } offsets[5] = {
    {0, 1, 0},
    {1, 0, 0},
    {2, 1, 0},
    {1, 2, 0},
    {1, 1, 1}
  };

  /*
    .............
    ..*...*...*..
    .*@*.*@*.*@*.
    ..*.*.*.*.*..
    ...*+*.*+*...
    ..*.*.*.*.*..
    .*@*.*@*.*@*.
    ..*.*.*.*.*..
    ...*+*.*+*...
    ..*.*.*.*.*..
    .*@*.*@*.*@*.
    ..*...*...*..
    .............

    Repeated pattern.

    + and @ are the same color.  They're just used in comments later.

    * is color 1.

    Possibly offset by -1 either up or to the left.

    Set of . around the edge not part of width & height.
  */

  QColor col1 = shadeColorFromCG(cg, widgetClass, HANDLE_SHADE);
  QColor col2 = shadeColorFromCG(cg, widgetClass, HANDLE_SHADOW_SHADE);

  if (r.width() % 4 == 1)
    xoffset = -1;
  if (r.height() % 4 == 1)
    yoffset = -1;
  for (l = 0; l < 5; l++) { /* Draw the pattern 5 times in different colors. */
    for (k = 0; k <= 2; k += 2) { /* draw @ -> k = 0 ... draw + -> k = 2 */
      for (i = k + offsets[l].xoff + xoffset, i2 = k + 1 + xoffset; i < r.width() && i2 < r.width(); i += 4, i2 += 4) {
        if (i < 0)
          continue;
        for (j = k + offsets[l].yoff + yoffset, j2 = k + 1 + yoffset; j < r.height() && j2 < r.height(); j += 4, j2 += 4) {
          if (j < 0)
            continue;
          p->setPen(offsets[l].color ? col1 : col2);
          p->drawPoint(r.x()+i,r.y()+j);
        }
      }
    }
  }
}


#define HANDLE_HEIGHT (4 * 4 + 1 + 2)
#define HANDLE_WIDTH (4 + 1 + 2)

void QIndustrialStyle::drawGrid(QPainter *p, const QRect &region, bool horiz, const QColorGroup &cg) const
{
  int maxwidth, maxheight;
  int handle_width, handle_height;

  QRect r = region;
#if 0
  Shrink the region ?
  if (DETAIL("paned")) {
  } else {
    x += 2;
    y += 2;
    width -= 4;
    height -= 4;
  }
#endif

  //  if (shadow_type == GTK_SHADOW_NONE) {
  maxwidth = r.width();
  maxheight = r.height();
  //  } else {
  //    maxwidth = width - 2;
  //    maxheight = height - 2;
  //  }

  if (horiz)
  {
    handle_width = MIN(maxwidth, HANDLE_HEIGHT);
    handle_height = MIN(maxheight, HANDLE_WIDTH);
  }
  else
  {
    handle_width = MIN(maxwidth, HANDLE_WIDTH);
    handle_height = MIN(maxheight, HANDLE_HEIGHT);
  }

  if (handle_width <= 0 || handle_height <= 0)
    return;

  r.moveBy((r.width() - handle_width) / 2,
           (r.height() - handle_height) / 2);
  r.setWidth(handle_width);
  r.setHeight(handle_height);

  realDrawGrid(p, r, cg, WDM_STANDARD);
}

void QIndustrialStyle::drawBox(QPainter *p, const QRect &r, const QColorGroup &cg, int widgetClass,
                               int flags, bool fill) const
{
  QColor col, fg;

  int x,y,width,height;
  r.rect(&x,&y,&width,&height);

  if (fill)
    p->fillRect(x + 1, y + 1, width - 2, height - 2,
                shadeColorFromCG(cg, widgetClass, SHADE_BG));

  /* Draw +s */
  col = shadeColorFromCG(cg, widgetClass, STANDARD_BORDER_SHADE);
  // fg = shadeColorFull(getBg(flags), col, SHADE_HALF);
  fg = shadeColorFull(cg.background(), col, SHADE_HALF);
  drawKaleidoscopePoint(p, fg, NULL, r, 0, 0);

  fg = shadeColorFromCG(cg, widgetClass, STANDARD_BORDER_SHADE);
  drawRotatedLine(p, fg, GTK_POS_TOP, r, ROTATION_CORNER_TL, 1, 0, ROTATION_CORNER_TR, 1, 0);
  drawRotatedLine(p, fg, GTK_POS_TOP, r, ROTATION_CORNER_TR, 0, 1, ROTATION_CORNER_BR, 0, 1);
  drawRotatedLine(p, fg, GTK_POS_TOP, r, ROTATION_CORNER_BR, 1, 0, ROTATION_CORNER_BL, 1, 0);
  drawRotatedLine(p, fg, GTK_POS_TOP, r, ROTATION_CORNER_BL, 0, 1, ROTATION_CORNER_TL, 0, 1);
}

void QIndustrialStyle::setupColors()
{
  int  c=QSettings().readNumEntry("/Qt/KDE/contrast", 7);
  contrast = CLAMP(c * 10.0, 0.0, 1.0);

  qApp->palette().active();
}

QIndustrialStyle::QIndustrialStyle()
    : KStyle(Default, WindowsStyleScrollBar),
    hover(HOVER_NONE),
    oldCursor(-1, -1),
    hoverWidget(NULL),
    hoverSect(QTC_NO_SECT)
{
  QSettings s;

  setupColors();

  //  fprintf (stderr, "Created new Industrial Style (2)\n" );
}

void QIndustrialStyle::polish(QApplication *app)
{
}

void QIndustrialStyle::polish(QPalette &p)
{
  setupColors();
}

static const char * kdeToolbarWidget="kde toolbar widget";

void QIndustrialStyle::polish(QWidget *widget)
{
  if (::qt_cast<QRadioButton *>(widget) || ::qt_cast<QCheckBox *>(widget))
  {
    widget->setMouseTracking(true);
    widget->installEventFilter(this);
  }
  else
    if (::qt_cast<QButton *>(widget) || ::qt_cast<QComboBox *>(widget) || widget->inherits("QToolBarExtensionWidget"))
    {
      widget->setBackgroundMode(QWidget::PaletteBackground);
      widget->installEventFilter(this);
    }
    else
      if (::qt_cast<QMenuBar *>(widget) || ::qt_cast<QToolBar *>(widget) || ::qt_cast<QPopupMenu *>(widget))
        widget->setBackgroundMode(QWidget::PaletteBackground);
  /*    else if ()
  widget->setBackgroundMode(QWidget::NoBackground); */
      else
        if (widget->inherits("KToolBarSeparator"))
        {
          widget->setBackgroundMode(QWidget::NoBackground);
          widget->installEventFilter(this);
        }
        else
          if (::qt_cast<QScrollBar *>(widget))
          {
            widget->setMouseTracking(true);
            widget->installEventFilter(this);
            widget->setBackgroundMode(QWidget::NoBackground);
          }
          else
            if (::qt_cast<QSlider *>(widget) || ::qt_cast<QHeader *>(widget))
            {
              widget->setMouseTracking(true);
              widget->installEventFilter(this);
            }
            else
              if (0==qstrcmp(widget->name(), kdeToolbarWidget))
              {
                widget->installEventFilter(this);
                widget->setBackgroundMode(QWidget::NoBackground);  // We paint the whole background.
              }

  KStyle::polish(widget);
}

void QIndustrialStyle::unPolish(QWidget *widget)
{
  if (::qt_cast<QRadioButton *>(widget) || ::qt_cast<QCheckBox *>(widget))
  {
    widget->setMouseTracking(false);
    widget->removeEventFilter(this);
  }
  else
    if (::qt_cast<QButton *>(widget) || ::qt_cast<QComboBox *>(widget) || widget->inherits("QToolBarExtensionWidget"))
    {
      widget->setBackgroundMode(QWidget::PaletteButton);
      widget->removeEventFilter(this);
    }
    else
      if (::qt_cast<QMenuBar *>(widget) || ::qt_cast<QToolBar *>(widget) || ::qt_cast<QPopupMenu *>(widget))
        widget->setBackgroundMode(QWidget::PaletteBackground);
      else
        if (widget->inherits("KToolBarSeparator"))
        {
          widget->setBackgroundMode(PaletteBackground);
          widget->removeEventFilter(this);
        }
        else
          if (::qt_cast<QScrollBar *>(widget))
          {
            widget->setMouseTracking(false);
            widget->removeEventFilter(this);
            widget->setBackgroundMode(QWidget::PaletteButton);
          }
          else
            if (::qt_cast<QSlider *>(widget) || ::qt_cast<QHeader *>(widget))
            {
              widget->setMouseTracking(false);
              widget->removeEventFilter(this);
            }
            else
              if (0==qstrcmp(widget->name(), kdeToolbarWidget))
              {
                widget->removeEventFilter(this);
                widget->setBackgroundMode(PaletteBackground);
              }
  KStyle::unPolish(widget);
}

bool QIndustrialStyle::eventFilter(QObject *object, QEvent *event)
{
  switch (event->type()) {
  case QEvent::Enter:
  {
    if (!object || !object->isWidgetType())
      break;

    hoverWidget = (QWidget *) object;
    if (!hoverWidget->isEnabled())
      hoverWidget = NULL;
    else
      redrawHoverWidget();
    break;
  }

  case QEvent::Leave:
  {
    if (object != hoverWidget)
      break;
    QWidget *w = hoverWidget;
    hoverWidget = NULL;
    if (w)
      w->repaint(FALSE);
    break;
  }
  default:
    break;
  }
  return KStyle::eventFilter(object, event);
}

/*

    enum PrimitiveElement {
    PE_ButtonCommand,
    PE_ButtonDefault,
    PE_ButtonBevel,
    PE_ButtonTool,
    PE_ButtonDropDown,

    PE_FocusRect,

    PE_ArrowUp,
    PE_ArrowDown,
    PE_ArrowRight,
    PE_ArrowLeft,

    PE_SpinWidgetUp,
    PE_SpinWidgetDown,
    PE_SpinWidgetPlus,
    PE_SpinWidgetMinus,

    PE_Indicator,
    PE_IndicatorMask,
    PE_ExclusiveIndicator,
    PE_ExclusiveIndicatorMask,

    PE_DockWindowHandle,
    PE_DockWindowSeparator,
    PE_DockWindowResizeHandle,

    PE_Splitter,

    PE_Panel,
    PE_PanelPopup,
    PE_PanelMenuBar,
    PE_PanelDockWindow,

    PE_TabBarBase,

    PE_HeaderSection,
    PE_HeaderArrow,
    PE_StatusBarSection,

    PE_GroupBoxFrame,

    PE_Separator,

    PE_SizeGrip,

    PE_CheckMark,

    PE_ScrollBarAddLine,
    PE_ScrollBarSubLine,
    PE_ScrollBarAdPBarage,
    PE_ScrollBarSubPage,
    PE_ScrollBarSlider,
    PE_ScrollBarFirst,
    PE_ScrollBarLast,

    PE_ProgressBarChunk,


    PE_WindowFrame,

    PE_CheckListController,
    PE_CheckListIndicator,
    PE_CheckListExclusiveIndicator,

    PE_PanelGroupBox,
    PE_RubberBand,

    // do not add any values below/greater this
    PE_CustomBase =         0xf000000
}
*/

void QIndustrialStyle::drawLightBevel(QPainter *p, const QRect &r,
                                      QStyle::SFlags flags, const QColorGroup &cg) const
{
  QColor fg, bg, corner;

  p->save();

  fg = shadeColorFromCG(cg, WDM_STANDARD, TROUGH_BORDER_SHADE);
  bg = shadeColorFromCG(cg, WDM_STANDARD, TROUGH_BG_SHADE);
  corner = shadeColorFromCG(cg, WDM_STANDARD, SHADE_BG);
  drawRoundedRectOnePixel(p, bg, fg, corner, r);

  p->restore();
}

void QIndustrialStyle::drawArrow(QPainter *p, const QRect &r, const QColor &col, QStyle::PrimitiveElement pe, bool small) const
{
  QPointArray a;

  if (small)
    switch (pe)
    {
    case QStyle::PE_ArrowUp:
      a.setPoints(4,  2,0,  0,-2,  -2,0,  2,0);
      break;
    case QStyle::PE_ArrowDown:
      a.setPoints(4,  2,0,  0,2,  -2,0,   2,0);
      break;
    case QStyle::PE_ArrowRight:
      a.setPoints(4,  0,-2,  -2,0,  0,2,  0,-2);
      break;
    case QStyle::PE_ArrowLeft:
      a.setPoints(4,  0,-2,  2,0,  0,2,   0,-2);
      break;
    default:
      return;
    }
  else
    switch (pe)
    {
    case QStyle::PE_ArrowUp:
      a.setPoints(4,  3,1,  0,-2,  -3,1,  3,1);
      break;
    case QStyle::PE_ArrowDown:
      a.setPoints(4,  3,-1,  0,2,  -3,-1,  3,-1);
      break;
    case QStyle::PE_ArrowRight:
      a.setPoints(4,  -1,-3,  2,0,  -1,3,  -1,-3);
      break;
    case QStyle::PE_ArrowLeft:
      a.setPoints(4,  1,-3,  -2,0,  1,3,  1,-3);
      break;
    default:
      return;
    }

  if (a.isNull())
    return;

  p->save();
  a.translate((r.x()+r.width()/ 2),(r.y()+r.height()/ 2));
  p->setBrush(col);
  p->setPen(col);
  p->drawPolygon(a);
  p->restore();
}

void QIndustrialStyle::drawPrimitive(PrimitiveElement pe, QPainter *p, const QRect &r,
                                     const QColorGroup &cg,
                                     SFlags flags, const QStyleOption &data) const
{
  int x = r.x(); // Makes cut/paste easier
  int y = r.y();
  int width = r.width();
  int height = r.height();

  switch (pe)
  {
  case PE_ButtonCommand:  // complete
  case PE_ButtonBevel:    // complete
  case PE_ButtonTool:     // complete
  case PE_ButtonDropDown: // complete
  {
    QColor fg, col;
    p->save();

    /*
      .......... ...........
      ..+@@@@@@@ @@@@@@@@+..
      .+@....... ........@+.
      .@........ .......#.@.
      .@........ ......$%.@.
      .@........ .....$&%.@.
      .@........ ....*=&%.@.
      .@........ ...*#=&%.@.
      .@........ ..*$#=&%.@.
      .@........ .*-$#=&%.@.

      .@........ .*-$#=&%.@.
      .@.......* **-$#=&%.@.
      .@......*- --$$#=&%.@.
      .@.....*$$ $$$##=&%.@.
      .@....*### ####==&%.@.
      .@...$==== =====&&%.@.
      .@..$&&&&& &&&&&&%%.@.
      .@.#%%%%%% %%%%%%%#.@.
      .+@....... ........@+.
      ..+@@@@@@@ @@@@@@@@+..
      .......... ...........

      The spaces indicate repeat the line on either side (if they don't match, it's a bug in the drawing.)
      Set of . around the edge not part of width & height.
    */

    if (!(flags & Style_ButtonDefault))
    {
      p->fillRect(r.x() + 1, r.y() + 1, r.width() - 2, r.height() - 2, shadeColorFromCG(cg, WDM_STANDARD, SHADE_BG));

      /* Draw @s */
      fg = shadeColorFromCG(cg, WDM_STANDARD, BUTTON_BORDER_SHADE);

      drawKaleidoscopePoint(p, fg, NULL, r, 1, 1);
      drawKaleidoscopeLine(p, fg, NULL, r, 0, 2);

      /* Draw +s */
      col = shadeColorFromCG(cg, WDM_STANDARD, BUTTON_BORDER_SHADE);
      // fg = shadeColorFull(getBg(flags), col, SHADE_HALF);
      fg = shadeColorFull(cg.background(), col, SHADE_HALF);
      drawKaleidoscopePoint(p, fg, NULL, r, 0, 1);
    }

    /*
      Pushed button.
      ........... ..........
      ..+@@@@@@@@ @@@@@@@+..
      .+@'''''''' '''''''@+.
      .@'<<<<<<<< <<<<<<<<@.
      .@'<<"..... ....."<#@.
      .@'<"...... ......$%@.
      .@'<....... .....$&%@.
      .@'<....... ....*=&%@.
      .@'<....... ...*#=&%@.
      .@'<....... ..*$#=&%@.
      .@'<....... .*-$#=&%@.

      .@'<....... .*-$#=&%@.
      .@'<......* **-$#=&%@.
      .@'<.....*- --$$#=&%@.
      .@'<....*$$ $$$##=&%@.
      .@'<...*### ####==&%@.
      .@'<".$==== =====&&%@.
      .@'<<$&&&&& &&&&&&%%@.
      .+@<#%%%%%% %%%%%%%@+.
      ..+@@@@@@@@ @@@@@@@+..
      ......................
      The spaces indicate repeat the line on either side (if they don't match, it's a bug in the drawing.)
      Set of . around the edge not part of width & height.
    */
    if (flags & Style_Active)
    {
      /* Draw 's */
      p->setPen(shadeColorFromCG(cg, WDM_STANDARD, BUTTON_DEPRESSED_SHADOW_SHADE_1));
      p->drawLine(x + 2, y + 1, x + width - 3, y + 1);
      p->drawLine(x + 1, y + 2, x + 1, y + height - 3);

      /* Draw <s */
      p->setPen(shadeColorFromCG(cg, WDM_STANDARD, BUTTON_DEPRESSED_SHADOW_SHADE_2));
      p->drawLine(x + 2, y + 2, x + width - 2, y + 2);
      p->drawLine(x + 2, y + 3, x + 2, y + height - 2);
      p->drawPoint(x + 3, y + 3);
      p->drawPoint(x + width - 3, y + 3);
      p->drawPoint(x + 3, y + height - 3);

      /* Draw #s */
      p->setPen(shadeColorFromCG(cg, WDM_STANDARD, BUTTON_DEPRESSED_SHADOW_SHADE_3));
      p->drawPoint(x + 3, y + 4);
      p->drawPoint(x + 4, y + 3);
      p->drawPoint(x + 3, y + height - 4);
      p->drawPoint(x + width - 4, y + 3);
    }
    else
    {
      p->setPen(shadeColorFromCG(cg, WDM_STANDARD, SHADE_0xF8));
      p->drawPoint(x + width - 3, y + height - 3);
    }

    /* Only draw shadow if button is large enough to fit it. */
    if (width > 18 && height > 18)
    {
      /* Offset shadow if depressed. */
      if (flags & Style_Active)
      {
        x++;
        y++;
      }

      /* Draw *s */
      p->setPen(shadeColorFromCG(cg, WDM_STANDARD, SHADE_0xFC));
      p->drawLine(x + 8, y + height - 9, x + width - 9, y + height - 9);
      p->drawLine(x + width - 9, y + 8, x + width - 9, y + height - 9);
      p->drawPoint(x + 5, y + height - 6);
      p->drawPoint(x + 6, y + height - 7);
      p->drawPoint(x + 7, y + height - 8);
      p->drawPoint(x + width - 6, y + 5);
      p->drawPoint(x + width - 7, y + 6);
      p->drawPoint(x + width - 8, y + 7);

      /* Draw -s */
      p->setPen(shadeColorFromCG(cg, WDM_STANDARD, SHADE_0xFB));
      p->drawLine(x + 8, y + height - 8, x + width - 9, y + height - 8);
      p->drawLine(x + width - 8, y + 8, x + width - 8, y + height - 9);

      /* Draw $s */
      p->setPen(shadeColorFromCG(cg, WDM_STANDARD, SHADE_0xFA));
      p->drawLine(x + 7, y + height - 7, x + width - 8, y + height - 7);
      p->drawLine(x + width - 7, y + 7, x + width - 7, y + height - 8);
      p->drawPoint(x + width - 8, y + height - 8);
      p->drawPoint(x + 3, y + height - 4);
      p->drawPoint(x + 4, y + height - 5);
      p->drawPoint(x + width - 4, y + 3);
      p->drawPoint(x + width - 5, y + 4);

      /* Draw #s */
      p->setPen(shadeColorFromCG(cg, WDM_STANDARD, SHADE_0xF8));
      p->drawLine(x + 6, y + height - 6, x + width - 7, y + height - 6);
      p->drawLine(x + width - 6, y + 6, x + width - 6, y + height - 7);
      p->drawPoint(x + width - 7, y + height - 7);
      p->drawPoint(x + 2, y + height - 3);
      p->drawPoint(x + width - 3, y + 2);

      /* Draw =s */
      p->setPen(shadeColorFromCG(cg, WDM_STANDARD, SHADE_0xF6));
      p->drawLine(x + 5, y + height - 5, x + width - 6, y + height - 5);
      p->drawLine(x + width - 5, y + 5, x + width - 5, y + height - 6);
      p->drawPoint(x + width - 6, y + height - 6);

      /* Draw &s */
      p->setPen(shadeColorFromCG(cg, WDM_STANDARD, SHADE_0xF4));
      p->drawLine(x + 4, y + height - 4, x + width - 5, y + height - 4);
      p->drawLine(x + width - 4, y + 4, x + width - 4, y + height - 5);
      p->drawPoint(x + width - 5, y + height - 5);

      /* Draw %s */
      p->setPen(shadeColorFromCG(cg, WDM_STANDARD, SHADE_0xF3));
      p->drawLine(x + 3, y + height - 3, x + width - 4, y + height - 3);
      p->drawLine(x + width - 3, y + 3, x + width - 3, y + height - 4);
      p->drawPoint(x + width - 4, y + height - 4);
    }

    p->restore();
    break;
  }
  case PE_ButtonDefault: // complete
  {
    /*
      .   c #FDFDFD
      +   c #FCFCFC
      @   c #FBFBFB
      #   c #FAFAFA
      0   c #F8F8F8
      -   c #F7F7F7
      a   c #F2F2F2
      3   c #EBEBEB
      6   c #EAEAEA
      =   c #E7E7E7
      2   c #D9D9D9
      4   c #D1D1D1
      <   c #C8C8C8
      '   c #BEBEBE
      )   c #BBBBBB
      {   c #B3B3B3
      ]   c #000000
      ......... .........
      .....+@## ##@+.....
      ...+0a3== ==3a0+...
      ..+-62<') )'<26-+..
      ..064{]]] ]]]{460..
      .+a2{]... ...]{2a+.
      .@3<].... ....]<3@.
      .#='].... ....]'=#.
      .#=)].... ....])=#.

      .#=)].... ....])=#.
      .#='].... ....]'=#.
      .@3<].... ....]<3@.
      .+a2{]... ...]{2a+.
      ..064{]]] ]]]{460..
      ..+-62<') )'<26-+..
      ...+0a3== ==3a0+...
      .....+@## ##@+.....
      ......... .........
      The spaces indicate repeat the line on either side (if they don't match, it's a bug in the drawing.)
      Set of . around the edge not part of width & height.
    */
    QColor fg;
    p->fillRect(x + 4, y + 4, width - 8, height - 8, shadeColorFromCG(cg, WDM_STANDARD, SHADE_BG));

    /* Draw ]s */
    fg = shadeColorFromCG(cg, WDM_STANDARD, SHADE_FG);
    drawKaleidoscopePoint(p, fg, NULL, r, 4, 4);
    drawKaleidoscopeLine(p, fg, NULL, r, 3, 5);

    /* Draw {s */
    drawKaleidoscopePoint(p, shadeColorFromCG(cg, WDM_STANDARD, SHADE_0xB3), NULL, r, 3, 4);

    /* Draw )s */
    drawKaleidoscopeLine(p, shadeColorFromCG(cg, WDM_STANDARD, SHADE_0xBB), NULL, r, 2, 7);

    /* Draw 's */
    drawKaleidoscopePoint(p, shadeColorFromCG(cg, WDM_STANDARD, SHADE_0xBE), NULL, r, 2, 6);

    /* Draw <s */
    drawKaleidoscopePoint(p, shadeColorFromCG(cg, WDM_STANDARD, SHADE_0xC8), NULL, r, 2, 5);

    /* Draw 4s */
    drawKaleidoscopePoint(p, shadeColorFromCG(cg, WDM_STANDARD, SHADE_0xD1), NULL, r, 3, 3);

    /* Draw 2s */
    drawKaleidoscopePoint(p, shadeColorFromCG(cg, WDM_STANDARD, SHADE_0xD9), NULL, r, 2, 4);

    /* Draw =s */
    drawKaleidoscopeLine(p, shadeColorFromCG(cg, WDM_STANDARD, SHADE_0xE7), NULL, r, 1, 6);

    /* Draw 6s */
    drawKaleidoscopePoint(p, shadeColorFromCG(cg, WDM_STANDARD, SHADE_0xEA), NULL, r, 2, 3);

    /* Draw 3s */
    drawKaleidoscopePoint(p, shadeColorFromCG(cg, WDM_STANDARD, SHADE_0xEB), NULL, r, 1, 5);

    /* Draw as */
    drawKaleidoscopePoint(p, shadeColorFromCG(cg, WDM_STANDARD, SHADE_0xF2), NULL, r, 1, 4);

    /* Draw -s */
    drawKaleidoscopePoint(p, shadeColorFromCG(cg, WDM_STANDARD, SHADE_0xF7), NULL, r, 2, 2);

    /* Draw 0s */
    drawKaleidoscopePoint(p, shadeColorFromCG(cg, WDM_STANDARD, SHADE_0xF8), NULL, r, 1, 3);

    /* Draw #s */
    drawKaleidoscopeLine(p, shadeColorFromCG(cg, WDM_STANDARD, SHADE_0xFA), NULL, r, 0, 6);

    /* Draw @s */
    drawKaleidoscopePoint(p, shadeColorFromCG(cg, WDM_STANDARD, SHADE_0xFB), NULL, r, 0, 5);

    /* Draw +s */
    drawKaleidoscopePoint(p, shadeColorFromCG(cg, WDM_STANDARD, SHADE_0xFC), NULL, r, 0, 4);
    drawKaleidoscopePoint(p, shadeColorFromCG(cg, WDM_STANDARD, SHADE_0xFC), NULL, r, 1, 2);
    break;
  }
  case PE_IndicatorMask: // complete
  {
    p->fillRect(r, color0);
    p->fillRect(r.x()+1, r.y()+1, r.width()-2, r.height()-2, color1);

    p->setPen(shadeColorFromCG(cg, WDM_STANDARD, STANDARD_BORDER_SHADE));
    p->drawLine(r.x()+1, r.y(), r.x()+r.width()-2, r.y());
    p->drawLine(r.x()+1, r.y()+r.height()-1, r.x()+r.width()-2, r.y()+r.height()-1);
    p->drawLine(r.x(), r.y()+1, r.x(), r.y()+r.height()-2);
    p->drawLine(r.x()+r.width()-1, r.y()+1, r.x()+r.width()-1, r.y()+r.height()-2);
  }
  break;
  case PE_ExclusiveIndicatorMask: // complete
    p->fillRect(r, color0);
    break;
  case PE_ExclusiveIndicator: // complete
  {
    int which;
    if (r.width() <= 8)
      which = PIXMAP_SMALL_RADIO_ACTIVE;
    else
      which = PIXMAP_RADIO_ACTIVE;

    if (flags & QStyle::Style_On)
      which += 0;
    else
      if (flags & QStyle::Style_Off)
        which += 1;
      else
        which += 2;

    QPixmap pixmap(pixmaps[ which ]);
    p->drawPixmap(r.x() + r.width()/2 - pixmap.width() / 2,
                  r.y() + r.height() / 2 - pixmap.height() / 2, pixmap);

    break;
  }
  case PE_CheckMark:    // complete
  case PE_Indicator:    // missing XPM re-shading code
  {
#warning cut/paste the XPM shading code ...
    int which;

    if (r.width() <= 8)
      which = PIXMAP_SMALL_CHECK_ACTIVE;
    else
      if (r.width() <= 11)
        which = PIXMAP_MEDIUM_CHECK_ACTIVE;
      else
        which = PIXMAP_CHECK_ACTIVE;

    if (flags & QStyle::Style_On)
      which += 0;
    else
      if (flags & QStyle::Style_Off)
        which += 1;
      else
        which += 2;

    QPixmap pixmap(pixmaps[ which ]);
    p->drawPixmap(r.x() + r.width()/2 - pixmap.width() / 2,
                  r.y() + r.height() / 2 - pixmap.height() / 2, pixmap);
    break;
  }
  case PE_ArrowUp:     // complete
  case PE_ArrowDown:   // complete
  case PE_ArrowRight:  // complete
  case PE_ArrowLeft:   // complete
    drawArrow(p, r, flags & Style_Enabled ? cg.buttonText(): cg.mid(), pe);
    break;
  case PE_ScrollBarSlider:  // ~complete
    drawSliderHandle(p, r, flags, cg);
    break;
  case PE_ScrollBarAddLine: // complete
  case PE_ScrollBarSubLine: // complete
  { // Arrows at top / bottom of the scroll-bar
    pe=flags&Style_Horizontal
       ? PE_ScrollBarAddLine==pe
       ? PE_ArrowRight
       : PE_ArrowLeft
 : PE_ScrollBarAddLine==pe
       ? PE_ArrowDown
       : PE_ArrowUp;

    drawSliderBevel(p, r, cg);
    drawPrimitive(pe, p, r, cg, flags);
    break;
  }
  case PE_ScrollBarSubPage: // complete
  case PE_ScrollBarAddPage: // complete
  case PE_ScrollBarFirst:   // complete
  case PE_ScrollBarLast:    // complete
    // These are in fact the groove.
    drawSliderGroove(p, r, flags, cg, NULL);
    break;
  case PE_Separator:        // complete
    p->setPen(shadeColorFromCG(qApp->palette().active(), WDM_STANDARD, STANDARD_BORDER_SHADE));
    p->drawLine(r.topLeft(), r.topRight());
    break;
  case PE_GroupBoxFrame:    // complete
    drawBox(p, r, cg, WDM_STANDARD, 0);
    break;
  case PE_FocusRect:        // incomplete but unfixable
    p->drawWinFocusRect(r, shadeColorFromCG(qApp->palette().active(), WDM_STANDARD, STANDARD_BORDER_SHADE));
    break;
    // cf. QFrame::drawFrame - all these are frame contents
  case PE_PanelTabWidget: // complete
  {
    QRect shifted(r);
    shifted.addCoords(1,0,0,0); // to meet button bevel on tab.
    drawPrimitive(PE_Panel, p, shifted, cg, flags, data);
    break;
  }
  case PE_PanelLineEdit:  // complete
    drawPrimitive(PE_Panel, p, r, cg, flags, data);
    break;
  case PE_Panel: // complete
  {
    QColor bg = shadeColorFromCG(cg, WDM_STANDARD, SHADE_BG);
    QColor fg = shadeColorFromCG(cg, WDM_STANDARD, LINE_SHADE);
    QColor corner = shadeColorFull(bg,fg,SHADE_HALF);
    drawRoundedRectOnePixel(p, bg, fg, corner, r, false);
    break;
  }
  case PE_Splitter: // ~complete but ugly.
    // draggable divider thing
    //      p->fillRect(r, getBg(flags));
    p->fillRect(r, cg.background());
    p->setPen(shadeColorFromCG(cg, WDM_STANDARD, LINE_SHADE));
    p->drawRect(r); // unfortunate.
    drawGrid(p, r, r.width() > r.height(), cg);
    break;

  case PE_PanelMenuBar:
  {
#warning soft-color
    QColor fg(247,247,247);
    QColor bg(231,231,231);
    //QColor bg(0,255,0);
    QColor corner(239,239,239);
    // QRect in(r);

    // p->fillRect(r, fg);
    // in.addCoords(2,2,-2,-2);
    drawRoundedRectOnePixel(p, fg, bg, corner, r, true);
  }
  break;

  case PE_PanelPopup:
  case PE_PanelGroupBox:
  case PE_PanelDockWindow:
    drawPrimitive(PE_Panel, p, r, cg, flags, data);
    break;
  case PE_HeaderSection:
  {
    flags=((flags | Style_Sunken)^ Style_Sunken)| Style_Raised;
    drawLightBevel(p, r, flags|Style_Horizontal, cg);
    break;
  }
  case PE_HeaderArrow:
    drawArrow(p, r, flags & Style_Enabled ? cg.buttonText() : cg.mid(), flags & Style_Up ? PE_ArrowUp : PE_ArrowDown);
    break;
  case PE_DockWindowSeparator:
  {
    QPoint p1, p2;

    if (flags & Style_Horizontal)
    {
      int offset=r.height()>18 ? 6 : r.height()>12 ? 4 : r.height()>6 ? 2 : 0;

      p1=QPoint(r.width()/2, 0+offset);
      p2=QPoint(p1.x(), r.height()-offset);
    }
    else
    {
      int offset=r.width()>18 ? 6 : r.width()>12 ? 4 : r.width()>6 ? 2 : 0;

      p1=QPoint(0+offset, r.height()/2);
      p2=QPoint(r.width()-offset, p1.y());
    }
    p->fillRect(r, cg.background());
    qDrawShadeLine(p, p1, p2, cg, 1, 1, 0);
    break;
  }

  case PE_SpinWidgetUp:
  case PE_SpinWidgetDown:
  {
    QRect sr(r);

    drawLightBevel(p, sr, flags|Style_Horizontal, cg);

    sr.setY(sr.y()+(PE_SpinWidgetDown==pe ? -2 : 1));

    drawArrow(p, sr, flags&Style_Enabled ? cg.buttonText(): cg.mid(),(pe==PE_SpinWidgetUp ? PE_ArrowUp : PE_ArrowDown), true);
    break;
  }
  default:
    KStyle::drawPrimitive(pe, p, r, cg, flags, data);
    break;
  }
}

void QIndustrialStyle::drawKStylePrimitive(KStylePrimitive kpe, QPainter *p, const QWidget *widget, const QRect &r,
    const QColorGroup &cg, SFlags flags, const QStyleOption &opt) const
{
  if (widget==hoverWidget)
    flags|=Style_MouseOver;

  switch (kpe)
  {
  case KPE_ToolBarHandle:
  case KPE_GeneralHandle:
    drawGrid(p, r, !(flags & Style_Horizontal), cg);
    break;
  case KPE_SliderGroove: {
    const QSlider* slider = (const QSlider*)widget;
    int center = r.y()+r.height()/2;
    //double coloredWidth = ((double)r.width()/slider->maxValue())*slider->value();
    double sliderLength = pixelMetric(PM_SliderLength, widget);
    double halfSliderLength = pixelMetric(PM_SliderLength, widget)/2;
    double coloredWidth = ((((double)r.width()-sliderLength)/(slider->maxValue()-slider->minValue()))*(slider->value() - slider->minValue()))+halfSliderLength;
    //drawSliderGroove(p, r, flags, cg, widget);
    p->setPen(QColor(255, 0, 0));
    p->drawLine(r.left(), center-3, r.left()+(int)coloredWidth, center-5);
    break;
  }
  case KPE_SliderHandle:
    drawSliderHandle(p, r, flags, cg);
    break;
  default:
    KStyle::drawKStylePrimitive(kpe, p, widget, r, cg, flags, opt);
  }
}

void QIndustrialStyle::drawExtension(QPainter *p, const QRect &r,
                                     int state, const QColorGroup &cg, GtkPositionType gap_side) const
{
  QColor bg, fg, corner;
  bool selected = (state == GTK_STATE_NORMAL);

  bg = shadeColorState(state, cg, SHADE_BG);
  fg = shadeColorState(state, cg, STANDARD_BORDER_SHADE);
  corner = shadeColorFull(cg.background(), fg, SHADE_HALF);

  int x,y,width,height;
  r.rect(&x,&y,&width,&height);

  p->fillRect(x + 1, y + 1, width - 1, height - 1, bg);

  /* Draw @s */
  drawKaleidoscopeLine(p, fg, NULL, r, 0, 2);
  drawKaleidoscopePoint(p, corner, NULL, r, 0, 1);

  /* Draw shadow */
  if (!selected)
  {
    drawRotatedLine(p, shadeColorState(state, cg, SHADE_0xFB), gap_side, r, ROTATION_CORNER_TL, 2, 6, ROTATION_CORNER_TR, 2, 6);
    drawRotatedLine(p, shadeColorState(state, cg, SHADE_0xFA), gap_side, r, ROTATION_CORNER_TL, 2, 5, ROTATION_CORNER_TR, 2, 5);
    drawRotatedLine(p, shadeColorState(state, cg, SHADE_0xF8), gap_side, r, ROTATION_CORNER_TL, 2, 4, ROTATION_CORNER_TR, 2, 4);
    drawRotatedLine(p, shadeColorState(state, cg, SHADE_0xF6), gap_side, r, ROTATION_CORNER_TL, 2, 3, ROTATION_CORNER_TR, 2, 3);
    drawRotatedLine(p, shadeColorState(state, cg, SHADE_0xF6), gap_side, r, ROTATION_CORNER_TR, 1, 0, ROTATION_CORNER_BR, 1, 2);
    drawRotatedLine(p, shadeColorState(state, cg, SHADE_0xF4), gap_side, r, ROTATION_CORNER_TL, 2, 2, ROTATION_CORNER_TR, 2, 2);
    drawRotatedLine(p, shadeColorState(state, cg, SHADE_0xF3), gap_side, r, ROTATION_CORNER_TL, 2, 1, ROTATION_CORNER_TR, 2, 1);
    drawRotatedLine(p, shadeColorFull(cg.background(), cg.foreground(), SHADE_0xF2), gap_side, r, ROTATION_CORNER_TL, 2, 0, ROTATION_CORNER_TR, 2, 0);
    QColor col = shadeColorFull(cg.background(), qApp->palette().active().background(), SHADE_HALF);
    drawRotatedLine(p, col, gap_side, r, ROTATION_CORNER_BL, 2, 1, ROTATION_CORNER_BR, 2, 1);
    drawRotatedLine(p, col, gap_side, r, ROTATION_CORNER_TL, 1, 0, ROTATION_CORNER_BL, 1, 2);
  }
  else
    drawRotatedLine(p, bg, gap_side, r, ROTATION_CORNER_TL, 1, 0, ROTATION_CORNER_TR, 1, 0);

#warning reset color here [!]
  drawRotatedPoint(p, fg, gap_side, r, ROTATION_CORNER_TL, 0, 0);
  drawRotatedPoint(p, fg, gap_side, r, ROTATION_CORNER_TL, 0, 1);

  drawRotatedPoint(p, fg, gap_side, r, ROTATION_CORNER_TR, 0, 0);
  drawRotatedPoint(p, fg, gap_side, r, ROTATION_CORNER_TR, 0, 1);

  drawRotatedPoint(p, fg, gap_side, r, ROTATION_CORNER_BL, 1, 1);

#warning check page ? what does this do ? - first one ? LHS ?
  //  if (check_page (widget, x, y, width, height))
  //    drawRotatedPoint(p, fg, gap_side, r, ROTATION_CORNER_BL, 1, 0);
  //  else
  drawRotatedPoint(p, corner, gap_side, r, ROTATION_CORNER_BL, 1, 0);
  drawRotatedPoint(p, corner, gap_side, r, ROTATION_CORNER_BL, 0, 1);
  drawRotatedPoint(p, bg, gap_side, r, ROTATION_CORNER_BL, 0, 0);

  drawRotatedPoint(p, fg, gap_side, r, ROTATION_CORNER_BR, 1, 1);
  drawRotatedPoint(p, corner, gap_side, r, ROTATION_CORNER_BR, 1, 0);
  drawRotatedPoint(p, corner, gap_side, r, ROTATION_CORNER_BR, 0, 1);
  drawRotatedPoint(p, bg, gap_side, r, ROTATION_CORNER_BR, 0, 0);
}

void QIndustrialStyle::drawControl(ControlElement control, QPainter *p, const QWidget *widget, const QRect &r,
                                   const QColorGroup &cg, SFlags flags, const QStyleOption &data) const
{
  if (widget==hoverWidget)
    flags|=Style_MouseOver;

  switch (control)
  {
  case CE_TabBarTab:
  {
    const QTabBar *tb=(const QTabBar *)widget;
    bool firstTab=(0 == tb->indexOf(data.tab()->identifier()));
    bool cornerWidget = false;

    if (tb->shape() == QTabBar::TriangularAbove ||
        tb->shape() == QTabBar::TriangularBelow) // uninteresting - bail
    {
      KStyle::drawControl(control, p, widget, r, cg, flags, data);
      break;
    }

    if (::qt_cast<const QTabWidget *>(tb->parent()))
    {
      const QTabWidget *tw = (const QTabWidget*)tb->parent();

      // is there a corner widget in the (top) left edge?
      if (tw->cornerWidget(Qt::TopLeft))
        cornerWidget=true;
    }

    QRect space(r);
    QPoint joina, joinb;
    switch (tb->shape()) {
    case QTabBar::RoundedAbove:
      if (!(flags & Style_Selected))
        space.addCoords(0,2,0,0);
      space.addCoords(1,0,0,-1);
      joina = joinb = r.bottomLeft();
      joina.rx()++;
      joinb.ry()--;
      break;
    case QTabBar::RoundedBelow:
      if (!(flags & Style_Selected))
        space.addCoords(0,0,0,-2);
      space.addCoords(1,1,0,0);
      joina = joinb = r.topLeft();
      joina.rx()++;
      joinb.ry()++;
      break;
    default:
      break;
    }

    int state = flags & Style_Selected ? GTK_STATE_NORMAL : GTK_STATE_ACTIVE;
    drawExtension(p, space, state, cg, tb->shape() == QTabBar::RoundedAbove ? GTK_POS_BOTTOM : GTK_POS_TOP);

    //    p->setPen(shadeColor(flags & ~Style_Selected, STANDARD_BORDER_SHADE));
    p->setPen(shadeColorFromCG(cg, WDM_STANDARD, STANDARD_BORDER_SHADE));
    if (!(flags & Style_Selected))
      p->drawLine(space.bottomLeft(), space.bottomRight());
    // Nastily sucky.
    if (!cornerWidget && firstTab)
      p->drawPoint(joina);
    if (cornerWidget)
      p->drawPoint(joinb);
    break;
  }
  case CE_TabBarLabel:
  {
#warning the TabBarLabel has a nasty hard-coded 3 pixel offset problem.
    QRect sr = r;
    //    sr.moveRight(2); // counteract QTabBar::paint
    KStyle::drawControl(control, p, widget, sr, cg, flags, data);
    break;
  }
  case CE_ProgressBarGroove:
//      p->setBrush(bg[GTK_STATE_NORMAL]);
    p->setBrush(qApp->palette().active().background());
    p->drawRect(r); // FIXME: just the outline ?
    qDrawShadePanel(p, r, cg, true, 1);
    break;
  case CE_ProgressBarContents:
  {
    // ### Take into account totalSteps()for busy indicator
    const QProgressBar *pb=(const QProgressBar*)widget;
    QRect              cr=subRect(SR_ProgressBarContents, widget);
    double             progress=pb->progress();
    bool               reverse=QApplication::reverseLayout();
    int                steps=pb->totalSteps();

    if (cr.isValid()&&(progress > 0 || steps==0))
    {
      double pg=(steps==0)? 0.1 : progress / steps;
      int    width=QMIN(cr.width(),(int)(pg * cr.width()));

      if (0==steps)//Busy indicator
      {
        if (width<1)
          width=1; //A busy indicator with width 0 is kind of useless

        int remWidth=cr.width()-width; //Never disappear completely

        if (remWidth<=0)
          remWidth=1; //Do something non-crashy when too small...

        int pstep=int(progress)%(2 *  remWidth);

        if (pstep > remWidth)
        {
          //Bounce about.. We're remWidth +some delta, we want to be remWidth-delta...
          //-((remWidth +some delta)-2* remWidth)=-(some deleta-remWidth)=remWidth-some delta..
          pstep=-(pstep-2 * remWidth);
        }

        if (reverse)
          drawPBarOrMenu(p, QRect(cr.x()+cr.width()-width-pstep, cr.y(), width, cr.height()), cg);
        else
          drawPBarOrMenu(p, QRect(cr.x()+pstep, cr.y(), width, cr.height()), cg);
      }
      else
        if (reverse)
          drawPBarOrMenu(p, QRect(cr.x()+(cr.width()-width), cr.y(), width, cr.height()), cg);
        else
          drawPBarOrMenu(p, QRect(cr.x(), cr.y(), width, cr.height()), cg);
    }
    break;
  }
  case CE_PushButton:
  {
    const QPushButton *button = static_cast<const QPushButton *>(widget);
    QRect             br(r);
    int               dbi = pixelMetric(PM_ButtonDefaultIndicator, widget);

    if (widget == hoverWidget)
      flags|=Style_MouseOver;

    br.setCoords(br.left()+dbi, br.top()+dbi, br.right()-dbi, br.bottom()-dbi);

    p->save();
    p->setBrushOrigin(-widget->backgroundOffset().x(), -widget->backgroundOffset().y());
    drawPrimitive(PE_ButtonCommand, p, br, cg, flags);
    if (button->isDefault())
      drawPrimitive(PE_ButtonDefault, p, r, cg, flags);
    p->restore();
    break;
  }
  case CE_PushButtonLabel:  // Taken from Highcolour and Plastik...
  {
    int x, y, w, h;
    r.rect(&x, &y, &w, &h);

    const QPushButton *button=static_cast<const QPushButton *>(widget);
    bool              active = button->isOn() || button->isDown(),
                               cornArrow = false;

    // Shift button contents if pushed.
    if (active)
    {
      x += pixelMetric(PM_ButtonShiftHorizontal, widget);
      y += pixelMetric(PM_ButtonShiftVertical, widget);
      flags |= Style_Sunken;
    }

    // Does the button have a popup menu?
    if (button->isMenuButton())
    {
      int dx = pixelMetric(PM_MenuButtonIndicator, widget);

      if (button->iconSet() && !button->iconSet()->isNull() &&
          (dx+button->iconSet()->pixmap(QIconSet::Small, QIconSet::Normal, QIconSet::Off).width()) >= w)
        cornArrow = true; //To little room. Draw the arrow in the corner, don't adjust the widget
      else
      {
        drawPrimitive(PE_ArrowDown, p, visualRect(QRect(x + w - dx - 8, y + 2, dx, h - 4), r),
                      cg, flags, data);
        w-=dx;
      }
    }

    // Draw the icon if there is one
    if (button->iconSet() && !button->iconSet()->isNull())
    {
      QIconSet::Mode  mode  = QIconSet::Disabled;
      QIconSet::State state = QIconSet::Off;

      if (button->isEnabled())
        mode = button->hasFocus() ? QIconSet::Active : QIconSet::Normal;
      if (button->isToggleButton() && button->isOn())
        state = QIconSet::On;

      QPixmap pixmap = button->iconSet()->pixmap(QIconSet::Small, mode, state);

      if (button->text().isEmpty() && !button->pixmap())
        p->drawPixmap(x + w/2 - pixmap.width()/2, y + h / 2 - pixmap.height() / 2, pixmap);
      else
        p->drawPixmap(x + QTC_ICON_INSET, y + h / 2 - pixmap.height() / 2, pixmap);

      if (cornArrow) //Draw over the icon
        drawPrimitive(PE_ArrowDown, p, visualRect(QRect(x + w - 6, x + h - 6, 7, 7), r),
                      cg, flags, data);

      int pw = pixmap.width();
      x+= pw + QTC_ICON_INSET;
      w-= pw + QTC_ICON_INSET;
    }

    drawItem(p, QRect(x, y, w, h), AlignCenter|ShowPrefix, button->colorGroup(), button->isEnabled(), button->pixmap(),
             button->text(), -1, &button->colorGroup().buttonText());

    // Draw a focus rect if the button has focus
    if (flags & Style_HasFocus)
      drawPrimitive(PE_FocusRect, p, QStyle::visualRect(subRect(SR_PushButtonFocusRect, widget), widget), cg, flags);
    break;
  }
  case CE_CheckBox:
    drawPrimitive(PE_Indicator, p, r, cg, flags, data);
    break;
  case CE_CheckBoxLabel:
  {
    const QCheckBox *checkbox=(const QCheckBox *)widget;

    if (flags&Style_MouseOver &&
        HOVER_CHECK==hover && hoverWidget==widget)
    {
      QRect   cr(checkbox->rect());
      QRegion r(QRect(cr.x(), cr.y(), visualRect(subRect(SR_CheckBoxFocusRect, widget), widget).width()+
                      pixelMetric(PM_IndicatorWidth)+4, cr.height()));

      r-=visualRect(subRect(SR_CheckBoxIndicator, widget), widget);
      p->setClipRegion(r);
      p->fillRect(checkbox->rect(), cg.background().light(QTC_HIGHLIGHT_FACTOR)); // cg.brush(QColorGroup::Midlight));
      p->setClipping(false);
    }
    int alignment=QApplication::reverseLayout() ? AlignRight : AlignLeft;

    drawItem(p, r, alignment | AlignVCenter | ShowPrefix, cg, flags & Style_Enabled, checkbox->pixmap(), checkbox->text());

    if (checkbox->hasFocus())
      drawPrimitive(PE_FocusRect, p, visualRect(subRect(SR_CheckBoxFocusRect, widget), widget), cg, flags);
  }
  break;
  case CE_RadioButtonLabel:
  {
    const QRadioButton *radiobutton=(const QRadioButton *)widget;

    if (flags&Style_MouseOver &&
        HOVER_RADIO==hover && hoverWidget==widget)
    {
      QRect   rb(radiobutton->rect());
      QRegion r(QRect(rb.x(), rb.y(),
                      visualRect(subRect(SR_RadioButtonFocusRect, widget), widget).width()+
                      pixelMetric(PM_ExclusiveIndicatorWidth)+4, rb.height()));
      r-=visualRect(subRect(SR_RadioButtonIndicator, widget), widget);
      p->setClipRegion(r);
      p->fillRect(radiobutton->rect(), cg.background().light(QTC_HIGHLIGHT_FACTOR)); // cg.brush(QColorGroup::Midlight));
      p->setClipping(false);
    }

    int alignment=QApplication::reverseLayout() ? AlignRight : AlignLeft;

    drawItem(p, r, alignment | AlignVCenter | ShowPrefix, cg, flags & Style_Enabled, radiobutton->pixmap(), radiobutton->text());

    if (radiobutton->hasFocus())
      drawPrimitive(PE_FocusRect, p, visualRect(subRect(SR_RadioButtonFocusRect, widget), widget), cg, flags);
  }
  break;

  case CE_PopupMenuItem:
  {
    if (!widget || data.isDefault())
      break;

    const QPopupMenu *popupmenu = (const QPopupMenu *)widget;
    QMenuItem        *mi=data.menuItem();
    int               tab=data.tabWidth(),
                          maxpmw=data.maxIconWidth(),
                                 x,
                                 y,
                                 w,
                                 h;

    r.rect(&x, &y, &w, &h);

    if ((flags & Style_Active)&&(flags & Style_Enabled)) {
      drawMenuItem(p, r, cg);
    }
    else
      if (widget->erasePixmap()&& !widget->erasePixmap()->isNull())
        p->drawPixmap(x, y, *widget->erasePixmap(), x, y, w, h);
      else
        p->fillRect(r, qApp->palette().active().background());
    //        p->fillRect(r, bg[GTK_STATE_NORMAL]);

    if (!mi)
      break;

    if (mi->isSeparator())
    {
      p->setPen(shadeColorFromCG(cg, WDM_STANDARD, LINE_SHADE));
      p->drawLine(r.left()+5, r.top()+4, r.right()-5, r.top()+4);
      break;
    }

    maxpmw=QMAX(maxpmw, 16);

    QRect cr,
    ir,
    tr,
    sr;
    // check column
    cr.setRect(r.left(), r.top(), maxpmw, r.height());
    // submenu indicator column
    sr.setCoords(r.right()-maxpmw, r.top(), r.right(), r.bottom());
    // tab/accelerator column
    tr.setCoords(sr.left()-tab-4, r.top(), sr.left(), r.bottom());
    // item column
    ir.setCoords(cr.right()+4, r.top(), tr.right()-4, r.bottom());

    bool reverse=QApplication::reverseLayout();

    if (reverse)
    {
      cr=visualRect(cr, r);
      sr=visualRect(sr, r);
      tr=visualRect(tr, r);
      ir=visualRect(ir, r);
    }

    if (mi->iconSet())
    {
      // Select the correct icon from the iconset
      QIconSet::Mode mode=flags & Style_Active
                          ? (mi->isEnabled() ? QIconSet::Active : QIconSet::Disabled)
                              : (mi->isEnabled() ? QIconSet::Normal : QIconSet::Disabled);
      cr=visualRect(QRect(x, y, maxpmw, h), r);

      // Do we have an icon and are checked at the same time?
      // Then draw a "pressed" background behind the icon
      if (popupmenu->isCheckable()&& mi->isChecked())
      {
        QBrush brush(shadeColorFromCG(cg, WDM_STANDARD, SHADE_NEG));

        qDrawShadePanel(p, cr.x()+1, cr.y()+2, cr.width()-2, cr.height()-4, cg, true, 1, &brush);
      }
      // Draw the icon
      QPixmap pixmap=mi->iconSet()->pixmap(QIconSet::Small, mode);
      QRect   pmr(0, 0, pixmap.width(), pixmap.height());

      pmr.moveCenter(cr.center());
      p->drawPixmap(pmr.topLeft(), pixmap);
    }
    else
      if (popupmenu->isCheckable()&& mi->isChecked())
      {
        QPixmap pixmap;

#ifdef JIMMAC_MADE_THESE_PRETTY
        if (cr.height() <= 8)
          pixmap = QPixmap(menu_check_8_xpm);
        else
          if (cr.height() <= 11)
            pixmap = QPixmap(menu_check_11_xpm);
          else
            pixmap = QPixmap(menu_check_13_xpm);
#else
        pixmap = QPixmap(menu_check_8_xpm); // the only nice one
#endif

        p->drawPixmap(cr.x() + cr.width()/2 - pixmap.width() / 2,
                      cr.y() + cr.height() / 2 - pixmap.height() / 2, pixmap);
      }

    QColor textcolor,
    embosscolor;

    if (flags & Style_Active)
    {
      if (!(flags & Style_Enabled))
      {
        textcolor=cg.text();
        embosscolor=cg.light();
      }
      else
      {
        textcolor=cg.highlightedText();
        embosscolor=cg.midlight().light();
      }
    }
    else
      if (!(flags & Style_Enabled))
      {
        textcolor=cg.text();
        embosscolor=cg.light();
      }
      else
        textcolor=embosscolor=cg.buttonText();
    p->setPen(textcolor);

    if (mi->custom())
    {
      p->save();
      if (!(flags & Style_Enabled))
      {
        p->setPen(cg.light());
        mi->custom()->paint(p, cg,(flags & Style_Enabled)?(flags & Style_Active): 0,
                            flags & Style_Enabled, ir.x()+1, ir.y()+1, ir.width()-1, ir.height()-1);
        p->setPen(textcolor);
      }
      mi->custom()->paint(p, cg,(flags & Style_Enabled)?(flags & Style_Active): 0,
                          flags & Style_Enabled, ir.x(), ir.y(), ir.width(), ir.height());
      p->restore();
    }

    QString text=mi->text();

    if (!text.isNull())
    {
      int t=text.find('\t');

      // draw accelerator/tab-text
      if (t>=0)
      {
        int alignFlag=AlignVCenter | ShowPrefix | DontClip | SingleLine;

        alignFlag |=(reverse ? AlignLeft : AlignRight);

        if (!(flags & Style_Enabled))
        {
          p->setPen(embosscolor);
          tr.moveBy(1, 1);
          p->drawText(tr, alignFlag, text.mid(t +1));
          tr.moveBy(-1,-1);
          p->setPen(textcolor);
        }

        p->drawText(tr, alignFlag, text.mid(t +1));
      }

      int alignFlag=AlignVCenter | ShowPrefix | DontClip | SingleLine;

      alignFlag |=(reverse ? AlignRight : AlignLeft);

      if (!(flags & Style_Enabled))
      {
        p->setPen(embosscolor);
        ir.moveBy(1, 1);
        p->drawText(ir, alignFlag, text, t);
        ir.moveBy(-1,-1);
        p->setPen(textcolor);
      }

      p->drawText(ir, alignFlag, text, t);
    }
    else
      if (mi->pixmap())
      {
        QPixmap pixmap=*mi->pixmap();

        if (1==pixmap.depth())
          p->setBackgroundMode(OpaqueMode);
        p->drawPixmap(ir.x(),(ir.height()-pixmap.height())/ 2, pixmap);
        if (pixmap.depth()==1)
          p->setBackgroundMode(TransparentMode);
      }

    if (mi->popup())
      drawArrow(p, sr, textcolor, reverse ? PE_ArrowLeft : PE_ArrowRight);
    break;
  }
  case CE_MenuBarItem:
  {
    if ((flags & Style_Enabled)&&(flags & Style_Active)&&(flags & Style_Down)) {
      drawMenuItem(p, r, cg);
    }
    else
      p->fillRect(r, cg.background());

    if (data.isDefault())
      break;

    QMenuItem *mi=data.menuItem();

    if (flags & Style_Active &&(flags & Style_Down))
      drawItem(p, r, AlignCenter | ShowPrefix | DontClip | SingleLine, cg, flags & Style_Enabled, mi->pixmap(), mi->text(),-1,
               &cg.highlightedText());
    else
      drawItem(p, r, AlignCenter | ShowPrefix | DontClip | SingleLine, cg, flags & Style_Enabled, mi->pixmap(), mi->text(),-1,
               &cg.buttonText());
    break;
  }
  default:
    KStyle::drawControl(control, p, widget, r, cg, flags, data);
  }
}

void QIndustrialStyle::drawControlMask(ControlElement control, QPainter *p, const QWidget *widget, const QRect &r, const QStyleOption &data) const
{
  switch (control)
  {
  case CE_PushButton:
  {
    int offset=r.width()<QTC_MIN_BTN_SIZE || r.height()<QTC_MIN_BTN_SIZE ? 1 : 2;

    p->fillRect(r, color0);
    p->fillRect(r.x()+1, r.y()+1, r.width()-2, r.height()-2, color1);
    p->setPen(color1);
    p->drawLine(r.x()+offset, r.y(), r.x()+r.width()-(offset+1), r.y());
    p->drawLine(r.x()+offset, r.y()+r.height()-1, r.x()+r.width()-(offset+1), r.y()+r.height()-1);
    p->drawLine(r.x(), r.y()+offset, r.x(), r.y()+r.height()-(offset+1));
    p->drawLine(r.x()+r.width()-1, r.y()+offset, r.x()+r.width()-1, r.y()+r.height()-(offset+1));
  }
  break;
  default:
    KStyle::drawControlMask(control, p, widget, r, data);
  }
}

void QIndustrialStyle::drawComplexControlMask(ComplexControl control, QPainter *p, const QWidget *widget, const QRect &r, const QStyleOption &data) const
{
  switch (control)
  {
  case CC_ToolButton:
  case CC_ComboBox:
    drawControlMask(CE_PushButton, p, widget, r, data);
    break;
  default:
    KStyle::drawComplexControlMask(control, p, widget, r, data);
  }
}

QRect QIndustrialStyle::subRect(SubRect subrect, const QWidget *widget)const
{
  QRect rect,
  wrect(widget->rect());

  switch (subrect)
  {
    /*    case SR_PushButtonContents:
    {
    //  rect=QCommonStyle::subRect(subrect, widget);
    break;
    } */
  case SR_PushButtonFocusRect:
  {
    const QPushButton *button=(const QPushButton *)widget;
    int dlx = 0, dly = 0;
    int margin = pixelMetric(PM_ButtonMargin, widget) + 1;

    if (button->isDefault() || button->autoDefault())
      dlx=dly=2;

    rect.setRect(wrect.x()+margin+dlx, wrect.y()+margin+dly,
                 wrect.width()-margin*2-dlx*2,
                 wrect.height()-margin*2-dly*2);
    break;
  }
  case SR_CheckBoxIndicator:
  {
    int h=pixelMetric(PM_IndicatorHeight);

    rect.setRect((widget->rect().height()-h)/ 2, (widget->rect().height()-h)/ 2, pixelMetric(PM_IndicatorWidth), h);
    break;
  }
  case SR_RadioButtonIndicator:
  {
    int h=pixelMetric(PM_ExclusiveIndicatorHeight);

    rect.setRect((widget->rect().height()-h)/ 2, (widget->rect().height()-h)/ 2, pixelMetric(PM_ExclusiveIndicatorWidth), h);
    break;
  }
  case SR_ProgressBarContents:
    rect=QRect(wrect.x()+1, wrect.y()+1, wrect.width()-2, wrect.height()-2);
    break;
  default:
    rect=KStyle::subRect(subrect, widget);
  }

  return rect;
}

void QIndustrialStyle::drawComplexControl(ComplexControl control, QPainter *p, const QWidget *widget,
    const QRect &r, const QColorGroup &cg, SFlags flags,
    SCFlags controls, SCFlags active, const QStyleOption &data) const
{
  if (widget==hoverWidget)
    flags |=Style_MouseOver;

  switch (control)
  {
  case CC_ToolButton:
  {
    const QToolButton *toolbutton=(const QToolButton *)widget;
    QRect             button(querySubControlMetrics(control, widget, SC_ToolButton, data)),
    menuarea(querySubControlMetrics(control, widget, SC_ToolButtonMenu, data));
    SFlags            bflags=flags,
                             mflags=flags;

    bool onControlButtons = false,
                            onToolbar = widget->parentWidget() && ::qt_cast<QToolBar *>(widget->parentWidget()),
                                        onExtender = !onToolbar &&
                                                     widget->parentWidget() && widget->parentWidget()->inherits("QToolBarExtensionWidget") &&
                                                     ::qt_cast<QToolBar *>(widget->parentWidget()->parentWidget());

    if (!onToolbar && !onExtender && widget->parentWidget() &&
        !qstrcmp(widget->parentWidget()->name(),"qt_maxcontrols"))
      onControlButtons = true;

    if (active & SC_ToolButton)
      bflags |=Style_Down;
    if (active & SC_ToolButtonMenu)
      mflags |=Style_Down;

    if (controls & SC_ToolButton)
    {
      // If we're pressed, on, or raised...
      if (bflags &(Style_Down | Style_On | Style_Raised) || onControlButtons)
      {
        //Make sure the standalone toolbuttons have a gradient in the right direction
        if (!onToolbar && !onControlButtons)
          bflags |= Style_Horizontal;

        drawPrimitive(PE_ButtonTool, p, button, cg, bflags, data);
      }

      // Check whether to draw a background pixmap
      else
        if (toolbutton->parentWidget() &&
            toolbutton->parentWidget()->backgroundPixmap() &&
            !toolbutton->parentWidget()->backgroundPixmap()->isNull())
        {
          p->drawTiledPixmap(r, *(toolbutton->parentWidget()->backgroundPixmap()), toolbutton->pos());
        }
        else
          if (widget->parent())
          {
            if (::qt_cast<const QToolBar *>(widget->parent()))
            {
              QToolBar *parent=(QToolBar*)widget->parent();

              p->fillRect(parent->rect(), cg.background());
            }
            else
              if (widget->parent()->inherits("QToolBarExtensionWidget"))
              {
                QWidget  *parent=(QWidget*)widget->parent();
                QToolBar *toolbar=(QToolBar*)parent->parent();

                p->fillRect(toolbar->rect(), cg.background());
              }
          }
    }

    if (controls & SC_ToolButtonMenu)
    {
      if (mflags &(Style_Down | Style_On | Style_Raised))
        drawPrimitive(PE_ButtonDropDown, p, menuarea, cg, mflags, data);
      drawPrimitive(PE_ArrowDown, p, menuarea, cg, mflags, data);
    }

    if (toolbutton->hasFocus()&& !toolbutton->focusProxy())
    {
      QRect fr=toolbutton->rect();
      fr.addCoords(3, 3,-3,-3);
      drawPrimitive(PE_FocusRect, p, fr, cg);
    }
    break;
  }
  case CC_ComboBox:
  {
    const QComboBox *combobox=(const QComboBox *)widget;
    QRect           frame(QStyle::visualRect(querySubControlMetrics(CC_ComboBox, widget, SC_ComboBoxFrame, data), widget)),
    arrow(QStyle::visualRect(querySubControlMetrics(CC_ComboBox, widget, SC_ComboBoxArrow, data), widget)),
    field(QStyle::visualRect(querySubControlMetrics(CC_ComboBox, widget, SC_ComboBoxEditField, data), widget));

    QColor bg = shadeColorFromCG(cg, WDM_STANDARD, SHADE_BG);
    QColor fg = shadeColorFromCG(cg, WDM_STANDARD, BUTTON_BORDER_SHADE);
    QColor corner = shadeColorFull(bg, fg, SHADE_HALF);

    if ((controls & SC_ComboBoxFrame) && frame.isValid())
      drawRoundedRectTwoPixel(p, bg, fg, corner, frame);

    if ((controls & SC_ComboBoxArrow) && arrow.isValid())
    {
      QRect arrow_up(arrow), arrow_down(arrow);
      const int arrow_size = 4;
      const int arrow_separation = 4;
      const int offset = (arrow_size + arrow_separation/2 + arrow_size / 2);
      arrow_up.addCoords(0,0,0,-offset + 1);
      arrow_down.addCoords(0,+offset - 1,0,0);
      drawPrimitive(PE_ArrowUp, p, arrow_up, cg, flags & ~Style_MouseOver);
      drawPrimitive(PE_ArrowDown, p, arrow_down, cg, flags & ~Style_MouseOver);
    }

    if ((controls & SC_ComboBoxEditField) && field.isValid())
    {
      if (combobox->editable())
      {
        field.addCoords(-1,-1, 1, 1);
        drawRoundedRectOnePixel(p, bg, fg, corner, field, true);
      }
      else
      {
        p->setPen(shadeColorFromCG(cg, WDM_STANDARD, BUTTON_BORDER_SHADE));
        p->drawLine(field.right(), field.top(), field.right(), field.bottom());
      }

      if ((flags & Style_HasFocus) && (! combobox->editable()))
      {
        QRect fr=QStyle::visualRect(subRect(SR_ComboBoxFocusRect, widget), widget);

        fr.addCoords(0, 0,-3,0);
        drawPrimitive(PE_FocusRect, p, fr, cg, flags | Style_FocusAtBorder,
                      QStyleOption(cg.highlight()));
      }
      if (flags & Style_Enabled)
        p->setPen(cg.buttonText());
      else
        p->setPen(cg.mid());
    }
    break;
  }
  case CC_SpinWidget:
  {
    const QSpinWidget *spinwidget=(const QSpinWidget *)widget;
    QRect             frame(querySubControlMetrics(CC_SpinWidget, widget, SC_SpinWidgetFrame, data)),
    up(spinwidget->upRect()),
    down(spinwidget->downRect());

    if ((controls & SC_SpinWidgetFrame)&& frame.isValid())
      qDrawShadePanel(p, r, cg, true, pixelMetric(PM_SpinBoxFrameWidth));

    if ((controls & SC_SpinWidgetUp)&& up.isValid())
    {
      PrimitiveElement pe=PE_SpinWidgetUp;
      SFlags           upflags=flags;

      if (spinwidget->buttonSymbols()==QSpinWidget::PlusMinus)
        pe=PE_SpinWidgetPlus;
      if (!spinwidget->isUpEnabled())
        upflags^=Style_Enabled;
      drawPrimitive(pe, p, up, cg, upflags |((active==SC_SpinWidgetUp)? Style_On | Style_Sunken : Style_Raised));
    }

    if ((controls & SC_SpinWidgetDown)&& down.isValid())
    {
      PrimitiveElement pe=PE_SpinWidgetDown;
      SFlags           downflags=flags;

      if (spinwidget->buttonSymbols()==QSpinWidget::PlusMinus)
        pe=PE_SpinWidgetMinus;
      if (!spinwidget->isDownEnabled())
        downflags^=Style_Enabled;
      drawPrimitive(pe, p, down, cg, downflags |((active==SC_SpinWidgetDown)? Style_On | Style_Sunken : Style_Raised));
    }
    break;
  }
  case CC_ScrollBar:
  {
    const QScrollBar *scrollbar=(const QScrollBar *)widget;
    bool             hw=hoverWidget==scrollbar;
    QRect            subline(querySubControlMetrics(control, widget, SC_ScrollBarSubLine, data)),
    addline(querySubControlMetrics(control, widget, SC_ScrollBarAddLine, data)),
    subpage(querySubControlMetrics(control, widget, SC_ScrollBarSubPage, data)),
    addpage(querySubControlMetrics(control, widget, SC_ScrollBarAddPage, data)),
    groove(querySubControlMetrics(control, widget, SC_ScrollBarGroove,  data)),
    slider(querySubControlMetrics(control, widget, SC_ScrollBarSlider,  data)),
    first(querySubControlMetrics(control, widget, SC_ScrollBarFirst,   data)),
    last(querySubControlMetrics(control, widget, SC_ScrollBarLast,    data));

    if ((controls & SC_ScrollBarSubLine)&& subline.isValid())
      drawPrimitive(PE_ScrollBarSubLine, p, subline, cg,
                    (hw && HOVER_SB_SUB==hover ? Style_MouseOver : Style_Default) |
                    Style_Enabled |
                    ((active==SC_ScrollBarSubLine) ? Style_Down : Style_Default) |
                    ((scrollbar->orientation()==Qt::Horizontal) ? Style_Horizontal : Style_Default));
    if ((controls & SC_ScrollBarAddLine)&& addline.isValid())
      drawPrimitive(PE_ScrollBarAddLine, p, addline, cg,
                    (hw && HOVER_SB_ADD==hover ? Style_MouseOver : Style_Default) |
                    Style_Enabled |
                    ((active==SC_ScrollBarAddLine) ? Style_Down : Style_Default) |
                    ((scrollbar->orientation()==Qt::Horizontal) ? Style_Horizontal : Style_Default));
    if ((controls & SC_ScrollBarSubPage)&& subpage.isValid())
      drawPrimitive(PE_ScrollBarSubPage, p, subpage, cg,
                    Style_Enabled |
                    ((active==SC_ScrollBarSubPage) ? Style_Down : Style_Default) |
                    ((scrollbar->orientation()==Qt::Horizontal) ? Style_Horizontal : Style_Default));
    if ((controls & SC_ScrollBarAddPage)&& addpage.isValid())
      drawPrimitive(PE_ScrollBarAddPage, p, addpage, cg,
                    ((scrollbar->minValue()==scrollbar->maxValue()) ? Style_Default : Style_Enabled) |
                    ((active==SC_ScrollBarAddPage) ? Style_Down : Style_Default) |
                    ((scrollbar->orientation()==Qt::Horizontal) ? Style_Horizontal : Style_Default));
    if ((controls & SC_ScrollBarFirst)&& first.isValid())
      drawPrimitive(PE_ScrollBarFirst, p, first, cg,
                    Style_Enabled |
                    ((active==SC_ScrollBarFirst) ? Style_Down : Style_Default) |
                    ((scrollbar->orientation()==Qt::Horizontal) ? Style_Horizontal : Style_Default));
    if ((controls & SC_ScrollBarLast)&& last.isValid())
      drawPrimitive(PE_ScrollBarLast, p, last, cg,
                    Style_Enabled |
                    ((active==SC_ScrollBarLast) ? Style_Down : Style_Default) |
                    ((scrollbar->orientation()==Qt::Horizontal) ? Style_Horizontal : Style_Default));
    if ((controls & SC_ScrollBarGroove) && groove.isValid())
      drawSliderGroove(p, r, Style_Enabled |
                       ((scrollbar->orientation()==Qt::Horizontal) ? Style_Horizontal : Style_Default), cg,
                       widget);

    if ((controls & SC_ScrollBarSlider)&& slider.isValid())
    {
      drawPrimitive(PE_ScrollBarSlider, p, slider, cg,
                    (hw && HOVER_SB_SLIDER==hover ? Style_MouseOver : Style_Default) |
                    Style_Enabled |
                    ((active==SC_ScrollBarSlider) ? Style_Down : Style_Default) |
                    ((scrollbar->orientation()==Qt::Horizontal) ? Style_Horizontal : Style_Default));

      // ### perhaps this should not be able to accept focus if maxedOut?
      if (scrollbar->hasFocus())
        drawPrimitive(PE_FocusRect, p, QRect(slider.x()+2, slider.y()+2, slider.width()-5, slider.height()-5), cg,
                      Style_Default);
    }
    break;
  }
  case CC_Slider:
  {
    QRect groove=querySubControlMetrics(CC_Slider, widget, SC_SliderGroove, data),
                 handle=querySubControlMetrics(CC_Slider, widget, SC_SliderHandle, data);

    if ((controls & SC_SliderGroove)&& groove.isValid())
      drawSliderGroove(p, groove, flags, cg, widget);
    if ((controls & SC_SliderHandle)&& handle.isValid())
      drawSliderHandle(p, handle, flags, cg);
    if (controls & SC_SliderTickmarks)
      QCommonStyle::drawComplexControl(control, p, widget, r, cg, flags, SC_SliderTickmarks, active, data);
    break;
  }
  default:
    KStyle::drawComplexControl(control, p, widget, r, cg, flags, controls, active, data);
  }
}

QRect QIndustrialStyle::querySubControlMetrics(ComplexControl control, const QWidget *widget, SubControl sc, const QStyleOption &data) const
{
  switch (control)
  {
  case CC_SpinWidget:
  {
    if (!widget)
      return QRect();

    int   fw=pixelMetric(PM_SpinBoxFrameWidth, 0);
    QSize bs;

    bs.setHeight(widget->height()/2);
    if (bs.height()< 8)
      bs.setHeight(8);
    bs.setWidth(QMIN(bs.height()*8/6, widget->width()/4));
    bs=bs.expandedTo(QApplication::globalStrut());

    if (!(bs.width()%2))
      bs.setWidth(bs.width()+1);

    int extra=bs.height()*2==widget->height()? 0 : 1;
    int y=0,
          x=widget->width()-y-bs.width(),
            lx=fw,
               rx=x-fw*2;

    switch (sc)
    {
    case SC_SpinWidgetUp:
      return QRect(x, y, bs.width(), bs.height());
    case SC_SpinWidgetDown:
      return QRect(x, y+bs.height(), bs.width(), bs.height()+extra);
    case SC_SpinWidgetButtonField:
      return QRect(x, y, bs.width(), widget->height()-2*fw);
    case SC_SpinWidgetEditField:
      return QRect(lx, fw, rx, widget->height()-2*fw);
    case SC_SpinWidgetFrame:
      return QRect(widget->x(), widget->y(), widget->width()-bs.width(), widget->height());
    }
  }
  default:
    return KStyle::querySubControlMetrics(control, widget, sc, data);
  }
}

int QIndustrialStyle::pixelMetric(PixelMetric metric, const QWidget *widget) const
{
  switch (metric)
  {
  case PM_TabBarTabShiftVertical:
  {
    const QTabBar *tb=::qt_cast<const QTabBar *>(widget);
    return QTabBar::RoundedAbove==tb->shape() || QTabBar::TriangularAbove==tb->shape()
           ? 1
           : -1;
  }
  case PM_TabBarTabShiftHorizontal:
    return 0;
  case PM_ButtonMargin:
    // was unconditional 5
    if (widget && widget->inherits("KDialogBaseButton"))
      // It seems KDialogBaseButton has some bloating issues.
      return 3;
    else
      return 5;
  case PM_ButtonShiftHorizontal:
  case PM_ButtonShiftVertical:
    return 1;
  case PM_ButtonDefaultIndicator:
    return 3; // was 1
  case PM_DefaultFrameWidth:
    if (widget) {
      if (::qt_cast<const QTabBar *>(widget) ||
          ::qt_cast<const QWidgetStack *>(widget) ||
          ::qt_cast<const QPopupMenu *>(widget))
        return QTC_BORDERED_FRAME_WIDTH;
    }
    return QTC_DEF_FRAME_WIDTH;
    break;
  case PM_SpinBoxFrameWidth:
    return 1;
  case PM_IndicatorWidth:
  case PM_IndicatorHeight:
    return QTC_CHECK_SIZE;
  case PM_ExclusiveIndicatorWidth:
  case PM_ExclusiveIndicatorHeight:
    return QTC_RADIO_SIZE;
  case PM_TabBarTabOverlap:
    return 1;
  case PM_ProgressBarChunkWidth:
    return 2;
  case PM_DockWindowSeparatorExtent:
    return 4;
  case PM_DockWindowHandleExtent:
    return 10;
  case PM_SplitterWidth:
    return 9;
  case PM_ScrollBarSliderMin:
    return 32;
  case PM_ScrollBarExtent:
  case PM_SliderControlThickness:
  case PM_SliderThickness:
    return 15;
  case PM_SliderLength:
    return 24;
  case PM_MaximumDragDistance:
    return -1;
  case PM_MenuBarItemSpacing:
    return 5;
  default:
    return KStyle::pixelMetric(metric, widget);
  }
}

int QIndustrialStyle::kPixelMetric(KStylePixelMetric kpm, const QWidget *widget) const
{
  switch (kpm)
  {
  case KPM_MenuItemSeparatorHeight:
    return 2;
  default:
    return KStyle::kPixelMetric(kpm, widget);
  }
}


QSize QIndustrialStyle::sizeFromContents(ContentsType contents, const QWidget *widget, const QSize &contentsSize, const QStyleOption &data)const
{
  switch (contents)
  {
  case CT_PushButton:
  {
    const QPushButton *button=static_cast<const QPushButton *>(widget);

    if (button && !button->text().isEmpty())
    {
      QSize ret = KStyle::sizeFromContents(contents, widget, contentsSize, data);
      int margin=2*pixelMetric(PM_ButtonMargin, widget);
      int w = ret.width() + margin;
      int h = ret.height() + margin;
      int minH = 28;

      w += QTC_BUTTON_WIDTH_EXTRA;

      if (button->text()=="...")
        w += 24;
      else
      {
        if (w<QTC_MIN_BTN_WIDTH)
          w=QTC_MIN_BTN_WIDTH;
      }
      return QSize(w, h < minH ? minH : h);
    }
    break;
  }
  case CT_DialogButtons:
  {
    QSize ret = KStyle::sizeFromContents(contents, widget, contentsSize, data);
    int w = ret.width();
    int h = ret.height();
    return QSize(w + QTC_BUTTON_WIDTH_EXTRA, h);
    break;
  }
  case CT_ComboBox:
  {
    QSize sz(KStyle::sizeFromContents(contents, widget, contentsSize, data));

    int h=sz.height();

    return QSize(sz.width(), h<26 ? 26 : h);
  }
  case CT_PopupMenuItem:
  {
    if (! widget || data.isDefault())
      break;

    QMenuItem        *mi = data.menuItem();
    const QPopupMenu *popupmenu = static_cast<const QPopupMenu *>(widget);
    int              maxpmw = data.maxIconWidth(),
                              w=contentsSize.width(),
                                h=contentsSize.height();

    if (mi->custom())
    {
      w = mi->custom()->sizeHint().width();
      h = mi->custom()->sizeHint().height();

      if (!mi->custom()->fullSpan() && h < 27)
        h = 27;
    }
    else
      if (mi->widget())
        ;
      else
        if (mi->isSeparator())
        {
          w = 10;
          h = 8;
        }
        else
        {
          // check is at least 16x16
          if (h < 16)
            h = 16;
          if (mi->pixmap())
            h = QMAX(h, mi->pixmap()->height());
          else
            if (!mi->text().isNull())
              h = QMAX(h, popupmenu->fontMetrics().height() + 2);
          if (mi->iconSet()!= 0)
            h = QMAX(h, mi->iconSet()->pixmap(QIconSet::Small, QIconSet::Normal).height());
          h+=5;
        }

    // check | 4 pixels | item | 8 pixels | accel | 4 pixels | check

    // check is at least 16x16
    maxpmw=QMAX(maxpmw, 16);
    w += (maxpmw * 2) + 8;

    if (! mi->text().isNull() && mi->text().find('\t') >= 0)
      w += 8;

    return QSize(w, h);
    break;
  }

  }

  return KStyle::sizeFromContents(contents, widget, contentsSize, data);
}

int QIndustrialStyle::styleHint(StyleHint stylehint, const QWidget *widget, const QStyleOption &option, QStyleHintReturn* returnData) const
{
  switch (stylehint) {
  case SH_EtchDisabledText:
  case SH_Slider_SnapToValue:
  case SH_PrintDialog_RightAlignButtons:
  case SH_FontDialog_SelectAssociatedText:
  case SH_MenuBar_AltKeyNavigation:
  case SH_MenuBar_MouseTracking:
  case SH_PopupMenu_MouseTracking:
  case SH_PopupMenu_SpaceActivatesItem:
  case SH_ComboBox_ListMouseTracking:
  case SH_ScrollBar_MiddleClickAbsolutePosition:
    return 1;
  case SH_MainWindow_SpaceBelowMenuBar:
    return 0;
  case SH_PopupMenu_AllowActiveAndDisabled:
    return 0;
  case SH_ScrollBar_BackgroundMode:
    return Qt::PaletteLink;
  default:
    return KStyle::styleHint(stylehint, widget, option, returnData);
  }
}

void QIndustrialStyle::drawPBarOrMenu(QPainter *p, QRect const &r, QColorGroup const &cg) const
{
  //  QColor bg(110, 124, 140); // #6e7c8c
  //  QColor bg(165, 178, 198); // #a5b2c6
  QColor bg = cg.highlight();
  //QColor fg(255, 255, 255);
  //  p->fillRect(r, shadeColorFull(0, bg, fg, SHADE_BG));
  p->fillRect(r, bg);
}

void QIndustrialStyle::drawMenuItem(QPainter *p, QRect const &r, const QColorGroup &cg) const
{
  QColor bg = cg.highlight();
  QColor fg = shadeColorFull(cg.background(), cg.foreground(), SHADE_HALF);
  QColor corner = shadeColorFull(bg, fg, SHADE_NEG);
  drawRoundedRectOnePixel(p,
                          bg,  // bg
                          fg,  // fg
                          corner,

                          r, true);
}


void
QIndustrialStyle::rotatePoint(GtkPositionType pos,
                              int x, int y,
                              int width, int height,
                              RotationCorner corner_enum,
                              int offset_x, int offset_y,
                              int *x_result, int *y_result) const
{
  int x_value = 0, y_value = 0;
  bool invert = false;
  int corner = (int) corner_enum;

  if (!x_result && !y_result)
    return;

  if (corner == ROTATION_CORNER_TR ||
      corner == ROTATION_CORNER_BL) {
    /* Swap values */
    offset_x ^= offset_y;
    offset_y ^= offset_x;
    offset_x ^= offset_y;
  }

  switch (pos) {
  case GTK_POS_TOP:
    /* corner += 0; */
    break;
  case GTK_POS_RIGHT:
    corner += 1;
    break;
  case GTK_POS_BOTTOM:
    corner = 3 - corner;
    invert = true;
    break;
  case GTK_POS_LEFT:
    corner = 4 - corner;
    invert = true;
    break;
  }
  corner %= 4;

  if (invert) {
    offset_x ^= offset_y;
    offset_y ^= offset_x;
    offset_x ^= offset_y;
  }

  switch (corner) {
  case ROTATION_CORNER_TL:
    x_value = x + offset_x;
    y_value = y + offset_y;
    break;
  case ROTATION_CORNER_TR:
    x_value = x + width - offset_y - 1;
    y_value = y + offset_x;
    break;
  case ROTATION_CORNER_BR:
    x_value = x + width - offset_x - 1;
    y_value = y + height - offset_y - 1;
    break;
  case ROTATION_CORNER_BL:
    x_value = x + offset_y;
    y_value = y + height - offset_x - 1;
    break;
  }

  if (x_result)
    *x_result = x_value;
  if (y_result)
    *y_result = y_value;
}

void QIndustrialStyle::drawRotatedPoint
(QPainter *p, const QColor &col,
 GtkPositionType pos, const QRect &r,
 RotationCorner corner, int offset_x, int offset_y) const
{
  int x_result, y_result;

  rotatePoint(pos, r.x(), r.y(), r.width(), r.height(),
              corner, offset_x, offset_y, &x_result, &y_result);
  p->setPen(col);
  p->drawPoint(x_result, y_result);
}

void QIndustrialStyle::drawRotatedLine
(QPainter *p, const QColor &col,
 GtkPositionType pos, const QRect &r,
 RotationCorner corner_1, int offset_x_1, int offset_y_1,
 RotationCorner corner_2, int offset_x_2, int offset_y_2) const
{
  int x,y,width,height;
  r.rect(&x,&y,&width,&height);
  int x_result_1, y_result_1, x_result_2, y_result_2;

  rotatePoint(pos, x, y, width, height, corner_1, offset_x_1, offset_y_1, &x_result_1, &y_result_1);
  rotatePoint(pos, x, y, width, height, corner_2, offset_x_2, offset_y_2, &x_result_2, &y_result_2);

  /* No mirror */
  p->setPen(col);
  p->drawLine(x_result_1, y_result_1, x_result_2, y_result_2);
}

void QIndustrialStyle::drawSliderBevel(QPainter *p, const QRect &r, const QColorGroup &cg) const
{
  QColor bg = shadeColorFromCG(cg, WDM_STANDARD, SHADE_BG);
  QColor bg_color = shadeColorFromCG(cg, WDM_STANDARD, TROUGH_BORDER_SHADE);
  QColor fg = shadeColorFromCG(cg, WDM_STANDARD, STANDARD_BORDER_SHADE);
  QColor corner = shadeColorFull(bg_color, fg, SHADE_HALF);
  QColor bevel_top = shadeColorFromCG(cg, WDM_STANDARD, STANDARD_BEVEL_TOP_SHADE);
  QColor bevel_bottom = shadeColorFromCG(cg, WDM_STANDARD, STANDARD_BEVEL_BOTTOM_SHADE);

  drawRoundedRectTwoPixel(p, bg, fg, corner, r);

  drawRotatedLine(p, bevel_bottom, GTK_POS_TOP, r, ROTATION_CORNER_TR, 1, 2, ROTATION_CORNER_BR, 1, 2);
  drawRotatedLine(p, bevel_bottom, GTK_POS_TOP, r, ROTATION_CORNER_BR, 2, 1, ROTATION_CORNER_BL, 2, 1);
  drawRotatedLine(p, bevel_top, GTK_POS_TOP, r, ROTATION_CORNER_BL, 1, 2, ROTATION_CORNER_TL, 1, 2);
  drawRotatedLine(p, bevel_top, GTK_POS_TOP, r, ROTATION_CORNER_TL, 2, 1, ROTATION_CORNER_TR, 2, 1);
}

void QIndustrialStyle::drawSliderHandle(QPainter *p, const QRect &r, QStyle::SFlags flags, const QColorGroup &cg) const
{
  flags &= ~(Style_Selected | Style_Down | Style_On);

  drawSliderBevel(p, r, cg);
  drawGrid(p, r, r.width() > r.height(), cg);

  p->setPen(shadeColorFromCG(cg, WDM_STANDARD, TROUGH_BORDER_SHADE));
  p->drawPoint(r.topLeft());
  p->drawPoint(r.topRight());
  p->drawPoint(r.bottomLeft());
  p->drawPoint(r.bottomRight());
}

void QIndustrialStyle::drawSliderGroove(QPainter *p, const QRect &r,
                                        QStyle::SFlags flags, const QColorGroup &cg, const QWidget *widget) const
{
  flags &= ~(Style_Active | Style_On);

  p->fillRect(r, shadeColorFromCG(cg, WDM_STANDARD, TROUGH_BG_SHADE));

  p->setPen(shadeColorFromCG(cg, WDM_STANDARD, TROUGH_BORDER_SHADE));
  if (flags & Style_Horizontal)
  {
    p->drawLine(r.topLeft(), r.topRight());
    p->drawLine(r.bottomLeft(), r.bottomRight());
  }
  else
  {
    p->drawLine(r.topLeft(), r.bottomLeft());
    p->drawLine(r.topRight(), r.bottomRight());
  }
}

// Vile hack piled on vile hack
void QIndustrialStyle::redrawHoverWidget()
{
  bool redraw = false;
  QPoint cursor(QCursor::pos()),
  widgetZero(hoverWidget->mapToGlobal(QPoint(0, 0)));

  //
  // Qt>=3.2 sets the sensitive part of a check/radio to the image + label -> anything else is not sensitive. But,
  // the widget can ocupy a larger area - and this whole are will react to mouse over. This needs to be coounteracted so that
  // it looks as if only the sensitive area mouse-overs...
  QRadioButton *rb=dynamic_cast<QRadioButton *>(hoverWidget);

  if (rb)
  {
    QRect rect(widgetZero.x(), widgetZero.y(), visualRect(subRect(SR_RadioButtonFocusRect, rb), rb).width()+
               pixelMetric(PM_ExclusiveIndicatorWidth)+4, hoverWidget->height());

    hover=rect.contains(cursor) ? HOVER_RADIO : HOVER_NONE;
    redraw = (HOVER_NONE!=hover && !rect.contains(oldCursor)) || (HOVER_NONE==hover && rect.contains(oldCursor));
  }
  else
  {
    QCheckBox *cb=dynamic_cast<QCheckBox *>(hoverWidget);

    if (cb)
    {
      QRect rect(widgetZero.x(), widgetZero.y(), visualRect(subRect(SR_CheckBoxFocusRect, cb), cb).width()+
                 pixelMetric(PM_IndicatorWidth)+4, hoverWidget->height());

      hover=rect.contains(cursor) ? HOVER_CHECK : HOVER_NONE;
      redraw= (HOVER_NONE!=hover && !rect.contains(oldCursor)) || (HOVER_NONE==hover && rect.contains(oldCursor));
    }
    else
    {
      QScrollBar *sb=dynamic_cast<QScrollBar *>(hoverWidget);

      if (sb) // So, are we over add button, sub button, slider, or none?
      {
        QRect subline(querySubControlMetrics(CC_ScrollBar, hoverWidget, SC_ScrollBarSubLine)),
        addline(querySubControlMetrics(CC_ScrollBar, hoverWidget, SC_ScrollBarAddLine)),
        slider(querySubControlMetrics(CC_ScrollBar, hoverWidget, SC_ScrollBarSlider));

        subline.moveLeft(subline.x()+widgetZero.x());
        subline.moveTop(subline.y()+widgetZero.y());
        addline.moveLeft(addline.x()+widgetZero.x());
        addline.moveTop(addline.y()+widgetZero.y());
        slider.moveLeft(slider.x()+widgetZero.x());
        slider.moveTop(slider.y()+widgetZero.y());

        if (slider.contains(cursor))
          hover=HOVER_SB_SLIDER;
        else
          if (subline.contains(cursor))
            hover=HOVER_SB_SUB;
          else
            if (addline.contains(cursor))
              hover=HOVER_SB_ADD;
            else
              hover=HOVER_NONE;

        redraw = (HOVER_SB_SLIDER==hover && !slider.contains(oldCursor)) || (HOVER_SB_SLIDER!=hover && slider.contains(oldCursor)) ||
                 (HOVER_SB_SUB==hover && !subline.contains(oldCursor)) || (HOVER_SB_SUB!=hover && subline.contains(oldCursor)) ||
                 (HOVER_SB_ADD==hover && !addline.contains(oldCursor)) || (HOVER_SB_ADD!=hover && addline.contains(oldCursor));
      }
      else
      {
        QHeader *hd=dynamic_cast<QHeader *>(hoverWidget);

        if (hd)
        {
          // Hmm... this ones tricky, as there's only 1 widget - but it has different sections...
          // and the ones that aren't clickable should not highlight on mouse over!

          QRect rect(widgetZero.x(), widgetZero.y(), hoverWidget->width(), hoverWidget->height());
          int  s=0;
          bool redraw=false;

          hover=rect.contains(cursor) ? HOVER_HEADER : HOVER_NONE;
          hoverSect=QTC_NO_SECT;

          for (s=0; s<hd->count() && (QTC_NO_SECT==hoverSect || !redraw); ++s)
          {
            QRect r(hd->sectionRect(s));

            r.moveLeft(r.x()+widgetZero.x());
            r.moveTop(r.y()+widgetZero.y());

            bool hasNew=r.contains(cursor);

            if (hasNew)
              hoverSect=s;

            if (!redraw)
            {
              bool hasOld=r.contains(oldCursor);

              if ((hasNew && !hasOld) || (!hasNew && hasOld))
                redraw=true;
            }
          }
        }
        else
          redraw = oldCursor==QPoint(-1, -1);
      }
    }
  }

  if (redraw)
    hoverWidget->repaint(FALSE);
}

#include "qindustrial.moc"
