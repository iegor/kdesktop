//////////////////////////////////////////////////////////////////////////////
// fahrenheitclient.h
// -------------------
// Fahrenheit window decoration for KDE
// -------------------
// Copyright (c) 2003, 2004 David Johnson <david@usermode.org>
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

#ifndef FAHRENHEITCLIENT_H
#define FAHRENHEITCLIENT_H

#include <qbutton.h>
#include <kdecoration.h>
#include <kdecorationfactory.h>

class QSpacerItem;
class QPoint;

namespace Fahrenheit {

class FahrenheitClient;

enum ButtonType {
    ButtonHelp=0,
    ButtonMax,
    ButtonMin,
    ButtonClose, 
    ButtonMenu,
    ButtonSticky,
    ButtonTypeCount
};

// FahrenheitFactory /////////////////////////////////////////////////////////////

class FahrenheitFactory: public KDecorationFactory
{
public:
    FahrenheitFactory();
    virtual ~FahrenheitFactory();
    virtual KDecoration *createDecoration(KDecorationBridge *b);
    virtual bool reset(unsigned long changed);

    static bool initialized();
    static Qt::AlignmentFlags titleAlign();

private:
    bool readConfig();

private:
    static bool initialized_;
    static Qt::AlignmentFlags titlealign_;
};

inline bool FahrenheitFactory::initialized()
    { return initialized_; }

inline Qt::AlignmentFlags FahrenheitFactory::titleAlign()
    { return titlealign_; }

// FahrenheitButton //////////////////////////////////////////////////////////////

class FahrenheitButton : public QButton
{
public:
    FahrenheitButton(FahrenheitClient *parent=0, const char *name=0,
                  const QString &tip=NULL,
                  ButtonType type=ButtonHelp,
                  QString pixmap=NULL);
    ~FahrenheitButton();

    void setPixmap(QString pixmap);
    QSize sizeHint() const;
    int lastMousePress() const;
    void reset();

private:
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void drawButton(QPainter *painter);

private:
    FahrenheitClient *client_;
    ButtonType type_;
    QPixmap *deco_;
    int lastmouse_;
};

inline int FahrenheitButton::lastMousePress() const
    { return lastmouse_; }

inline void FahrenheitButton::reset()
    { repaint(false); }

// FahrenheitClient //////////////////////////////////////////////////////////////

class FahrenheitClient : public KDecoration
{
    Q_OBJECT
public:
    FahrenheitClient(KDecorationBridge *b, KDecorationFactory *f);
    virtual ~FahrenheitClient();

    virtual void init();

    virtual void recalcTitlebar();
    virtual void activeChange();
    virtual void desktopChange();
    virtual void captionChange();
    virtual void iconChange();
    virtual void maximizeChange();
    virtual void shadeChange();
    virtual void doShape();

    virtual void borders(int &l, int &r, int &t, int &b) const;
    virtual void resize(const QSize &size);
    virtual QSize minimumSize() const;
    virtual Position mousePosition(const QPoint &point) const;

private:
    void addButtons(QBoxLayout* layout, const QString& buttons);

    bool isTool();
    bool eventFilter(QObject *obj, QEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *);
    void showEvent(QShowEvent *);

private slots:
    void maxButtonPressed();
    void menuButtonPressed();

private:
    int buttonWidth_;
    FahrenheitButton *button[ButtonTypeCount];
    QBoxLayout *mainLayout_;
    QBoxLayout *topLayout_;
    QBoxLayout *menuLayout_;
    QBoxLayout *buttonLayout_;
    QSpacerItem *titleSpacer_;
    QSpacerItem *barSpacer_;
};

} // namespace Fahrenheit

#endif // FAHRENHEITCLIENT_H
