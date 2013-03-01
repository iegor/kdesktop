#ifndef __QINDUSTRIAL_H__
#define __QINDUSTRIAL_H__

/*
  QIndustrial (C) 2004 Novell, Inc.
	      (C) Craig Drummond, 2003 Craig.Drummond@lycos.co.uk

  ----

  Based upon B???Curve style (Copyright (c) 2002 R?d H?t, Inc)
      Bernhard Rosenkränzer <bero@r?dh?t.com>
      Preston Brown <pbrown@r?dh?t.com>
      Than Ngo <than@r?dh?t.com>

  Released under the GNU General Public License (GPL) v2.

  ----

  B???Curve is based on the KDE Light style, 2nd revision:
  Copyright (c) 2000-2001 Trolltech AS (info@trolltech.com)

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
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
#include <kstyle.h>
#include <qcolor.h>
#include <qpoint.h>
#include "config.h"
#include "common.h"

#define SHADE_FG   0 
#define SHADE_DARK 1 
#define SHADE_0x9E 2 
#define SHADE_0xB3 3 
#define SHADE_0xBB 4 
#define SHADE_0xBE 5 
#define SHADE_0xC7 6 
#define SHADE_0xC8 7 
#define SHADE_0xCA 8 
#define SHADE_0xD1 9 
#define SHADE_0xD5 10
#define SHADE_0xD9 11
#define SHADE_0xDA 12
#define SHADE_0xE7 13
#define SHADE_0xEA 14
#define SHADE_0xEB 15
#define SHADE_0xEC 16
#define SHADE_0xF2 17
#define SHADE_0xF3 18
#define SHADE_0xF4 19
#define SHADE_0xF6 20
#define SHADE_0xF7 21
#define SHADE_0xF8 22	
#define SHADE_0xFA 23	
#define SHADE_0xFB 24	
#define SHADE_0xFC 25
#define SHADE_0xFD 26
#define SHADE_0xFF 27
#define SHADE_HALF 28
#define SHADE_NEG  29

#define SHADE_COUNT 30

#define MENUITEM_BORDER_SHADE SHADE_NEG

#define HANDLE_SHADE SHADE_0x9E
#define HANDLE_SHADOW_SHADE SHADE_0xDA
#define LINE_SHADE SHADE_0x9E
#define STANDARD_BORDER_SHADE SHADE_0x9E
#define STANDARD_BEVEL_TOP_SHADE SHADE_0xFF
#define STANDARD_BEVEL_BOTTOM_SHADE SHADE_0xF3
#define TROUGH_BORDER_SHADE SHADE_0xC7
#define TROUGH_BG_SHADE SHADE_0xD9

#define BUTTON_BORDER_SHADE SHADE_0x9E

#define MENUBAR_BORDER_SHADE SHADE_0xEA

#define BUTTON_DEPRESSED_SHADOW_SHADE_1 SHADE_0xDA
#define BUTTON_DEPRESSED_SHADOW_SHADE_2 SHADE_0xEC
#define BUTTON_DEPRESSED_SHADOW_SHADE_3 SHADE_0xF7

#define SHADE_BG SHADE_0xFD

namespace Industrial {

const double contrast_center = 0.5;

typedef enum
{
  GTK_STATE_NORMAL,
  GTK_STATE_ACTIVE,
  GTK_STATE_PRELIGHT,
  GTK_STATE_SELECTED,
  GTK_STATE_INSENSITIVE,
  GTK_STATE_MAX
} GtkStateType;

typedef enum
{
	WDM_STANDARD,
	WDM_HIGHLIGHTED,
	WDM_LINK,
	WDM_WINDOW,
	WDM_BUTTON,
} widgetDrawMode;


class QIndustrialStyle : public KStyle
{
  Q_OBJECT

    public:

  enum EHover
    {
      HOVER_NONE,
      HOVER_CHECK,
      HOVER_RADIO,
      HOVER_SB_ADD,
      HOVER_SB_SUB,
      HOVER_SB_SLIDER,
      HOVER_HEADER
    };


  typedef enum {
    ROTATION_CORNER_TL = 0,
    ROTATION_CORNER_TR = 1,
    ROTATION_CORNER_BR = 2,
    ROTATION_CORNER_BL = 3
  } RotationCorner;

  typedef enum
    {
      GTK_POS_LEFT,
      GTK_POS_RIGHT,
      GTK_POS_TOP,
      GTK_POS_BOTTOM
    } GtkPositionType;

  QIndustrialStyle();
  virtual ~QIndustrialStyle() {}

  void polish(QApplication *app);
  void polish(QPalette &pal);
  void polish(QWidget *widget);
  void unPolish(QWidget *widget);
  void drawArrow(QPainter *p, const QRect &r, const QColor &col, QStyle::PrimitiveElement pe, bool small=false) const;
  void drawPrimitive(PrimitiveElement, QPainter *, const QRect &, const QColorGroup &, SFlags = Style_Default,
		     const QStyleOption & = QStyleOption::Default) const;
  void drawKStylePrimitive(KStylePrimitive kpe, QPainter* p, const QWidget* widget, const QRect &r, const QColorGroup &cg,
			   SFlags flags, const QStyleOption &opt ) const;
  void drawControl(ControlElement, QPainter *, const QWidget *, const QRect &, const QColorGroup &, SFlags = Style_Default,
		   const QStyleOption & = QStyleOption::Default) const;
  void drawControlMask(ControlElement, QPainter *, const QWidget *, const QRect &, const QStyleOption & = QStyleOption::Default) const;
  void drawComplexControlMask(ComplexControl control, QPainter *p, const QWidget *widget, const QRect &r, const QStyleOption &data) const;
  QRect subRect(SubRect, const QWidget *) const;
  void drawComplexControl(ComplexControl, QPainter *, const QWidget *, const QRect &, const QColorGroup &, SFlags = Style_Default,
			  SCFlags = SC_All, SCFlags = SC_None, const QStyleOption & = QStyleOption::Default) const;
  QRect querySubControlMetrics(ComplexControl, const QWidget *, SubControl, const QStyleOption & = QStyleOption::Default) const;
  int pixelMetric(PixelMetric, const QWidget *widget= 0) const;
  int kPixelMetric(KStylePixelMetric kpm, const QWidget *widget) const;
  QSize sizeFromContents(ContentsType, const QWidget *, const QSize &, const QStyleOption & = QStyleOption::Default) const;
  int styleHint(StyleHint, const QWidget *widget= 0, const QStyleOption & = QStyleOption::Default, QStyleHintReturn *returnData= 0) const;

 protected:

  bool eventFilter(QObject *object, QEvent *event);
  void drawPBarOrMenu(QPainter *p, QRect const &r, QColorGroup const &cg) const;
  void drawMenuItem(QPainter *p, QRect const &r, const QColorGroup &cg) const;
  void drawSliderHandle(QPainter *p, const QRect &r, QStyle::SFlags flags, const QColorGroup &cg) const;
  void drawSliderGroove(QPainter *p, const QRect &r, QStyle::SFlags flags, const QColorGroup &cg, const QWidget *widget) const;
  void drawSliderBevel(QPainter *p, const QRect &r, const QColorGroup &cg) const;
  void drawLightBevel(QPainter *p, const QRect &r, QStyle::SFlags flags, const QColorGroup &cg) const;
  void setupColors();

 private:
  // Rendering helpers:
  void drawKaleidoscopePoint( QPainter *p, const QColor &col,
			      const QRect *clip, const QRect &rect,
			      int offset_x, int offset_y ) const;
  void drawKaleidoscopeLine( QPainter *p, const QColor &col,
			     const QRect *clip, const QRect &rect,
			     int edge_distance_offset,
			     int directional_offset ) const;
  void rotatePoint( GtkPositionType pos, 
		    int x, int y,
		    int width, int height,
		    RotationCorner corner,
		    int offset_x, int offset_y,
		    int *x_result, int *y_result) const;

  void drawRotatedPoint( QPainter *p, const QColor &col,
			 GtkPositionType pos, const QRect &rect,
			 RotationCorner corner, int offset_x, int offset_y) const;
  void drawRotatedLine( QPainter *p, const QColor &col,
			GtkPositionType pos, const QRect &rect,
			RotationCorner corner_1, int offset_x_1, int offset_y_1,
			RotationCorner corner_2, int offset_x_2, int offset_y_2 ) const;
  void realDrawGrid(QPainter *p, const QRect &r, const QColorGroup &cg, int widgetClass) const;
  void drawGrid(QPainter *p, const QRect &region, bool horiz, const QColorGroup &cg) const;
  void drawBox(QPainter *p, const QRect &region, const QColorGroup &cg, int widgetClass, int flags, bool fill=false) const;
  void drawExtension(QPainter *p, const QRect &region, int state, const QColorGroup &cg, GtkPositionType gap_side=GTK_POS_BOTTOM) const;
  void drawRoundedRectTwoPixel( QPainter *p, const QColor &bg,
				const QColor &fg, const QColor &corner,
				const QRect &rect, bool fill=true) const;
  void drawRoundedRectOnePixel( QPainter *p, const QColor &bg,
				const QColor &fg, const QColor &corner,
				const QRect &rect, bool fill=true) const;
  void redrawHoverWidget();

  QColor shadeColorState(int state, const QColorGroup &cg, int shade) const;
  QColor shadeColorFull(const QColor &bg, const QColor &fg, int shade) const;
  QColor shadeColorFromCG(const QColorGroup &cg, int widgetClass, int shade) const;

 private:

  EDefBtnIndicator defBtnIndicator;
  EHover           hover;
  double           contrast;
  QPoint           oldCursor;
  QWidget         *hoverWidget;
  int              hoverSect;
};

};

#endif
