//////////////////////////////////////////////////////////////////////////////
// phasestyle.h
// -------------------
// Qt/KDE widget style
// -------------------
// Copyright (c) 2004 David Johnson <david@usermode.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////

#ifndef PHASESTYLE_H
#define PHASESTYLE_H

#include <kstyle.h>
#include <qcolor.h>

class KPixmap;

class GradientSet
{
public:
    enum GradientType {
        Horizontal,
        Vertical,
        HorizontalReverse,
        VerticalReverse,
        GradientTypeCount
    };

    GradientSet(const QColor &color, int size);
    ~GradientSet();

    KPixmap* gradient(bool horizontal, bool reverse);

private:
    KPixmap *set[GradientTypeCount];
    QColor color_;
    int size_;
};

class PhaseStyle : public KStyle
{
    Q_OBJECT
public:
    PhaseStyle();
    virtual ~PhaseStyle();

    void polish(QApplication* app);
    void polish(QWidget *widget);
    void polish(QPalette &pal);
    void unPolish(QWidget *widget);

    void drawPrimitive(PrimitiveElement element,
            QPainter *painter,
            const QRect &rect,
            const QColorGroup &group,
            SFlags flags = Style_Default,
            const QStyleOption &option = QStyleOption::Default) const;

    void drawKStylePrimitive(KStylePrimitive element,
            QPainter *painter,
            const QWidget *widget,
            const QRect &rect,
            const QColorGroup &group,
            SFlags flags = Style_Default,
            const QStyleOption &option = QStyleOption::Default) const;

    void drawControl(ControlElement element,
            QPainter *painter,
            const QWidget *widget,
            const QRect &rect,
            const QColorGroup &group,
            SFlags flags = Style_Default,
            const QStyleOption &option = QStyleOption::Default) const;

    void drawControlMask(ControlElement element,
            QPainter *painter,
            const QWidget *widget,
            const QRect &rect,
            const QStyleOption &option = QStyleOption::Default) const;

    void drawComplexControl(ComplexControl control,
            QPainter *painter,
            const QWidget *widget,
            const QRect &rect,
            const QColorGroup &group,
            SFlags flags = Style_Default,
            SCFlags controls = SC_All,
            SCFlags active = SC_None,
            const QStyleOption &option = QStyleOption::Default) const;

    void drawComplexControlMask(ComplexControl control,
            QPainter *painter,
            const QWidget *widget,
            const QRect &rect,
            const QStyleOption &option = QStyleOption::Default) const;

    int pixelMetric(PixelMetric metric,
            const QWidget *widget = 0) const;

    QRect subRect(SubRect rect, const QWidget *widget) const;

    QRect querySubControlMetrics(ComplexControl control,
            const QWidget *widget,
	    SubControl subcontrol,
            const QStyleOption &option = QStyleOption::Default) const;

    QSize sizeFromContents(ContentsType contents,
            const QWidget *widget,
            const QSize &contentsize,
            const QStyleOption& option = QStyleOption::Default) const;

private:
    PhaseStyle(const PhaseStyle &);
    PhaseStyle& operator=(const PhaseStyle &);

    void drawPhaseBevel(QPainter *painter,
            int x, int y, int w, int h,
            const QColorGroup &group,
	    const QColor &fill,
            bool sunken=false,
            bool horizontal=true,
            bool reverse=false) const;

    void drawPhaseButton(QPainter *painter,
            int x, int y, int w, int h,
            const QColorGroup &group,
	    const QColor &fill,
            bool sunken=false) const;

    void drawPhasePanel(QPainter *painter,
            int x, int y, int w, int h,
            const QColorGroup &group,
            bool sunken=false,
            const QBrush *fill=NULL) const;

    void drawPhaseTab(QPainter *painter,
            int x, int y, int w, int h,
            const QColorGroup &group,
            const QTabBar *bar,
	    const QStyleOption &option,
            const SFlags flags) const;

    void drawPhaseGradient(QPainter *painter,
            const QRect &rect,
            QColor color,
            bool horizontal,
            int px=0, int py=0,
            int pw=-1, int ph=-1,
            bool reverse=false) const;

    bool flatToolbar(const QToolBar *toolbar) const;

    bool eventFilter(QObject *object, QEvent *event);

private:
    QWidget *hover_;
    QTab *hovertab_;
    QMap<unsigned int, QIntDict<GradientSet> > * gradients;
    bool gradients_;
    bool highlights_;
    bool reverse_;
    bool kicker_;
};

#endif // PHASESTYLE_H
