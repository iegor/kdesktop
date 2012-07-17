//////////////////////////////////////////////////////////////////////////////
// kwinqinx.h
// -------------------
// Qinx window decoration for KDE
// -------------------
// Copyright (c) 2002-2004 David Johnson <david@usermode.org>
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

#ifndef KWINQINX_H
#define KWINQINX_H

#include <qbutton.h>
#include <kdecoration.h>
#include <kdecorationfactory.h>
#include <kpixmap.h>

class QPoint;
class QSpacerItem;

namespace Qinx {

enum ButtonType {
    ButtonHelp=0,
    ButtonMax,
    ButtonMin,
    ButtonClose, 
    ButtonMenu,
    ButtonSticky,
    ButtonTypeCount
};

enum PixmapType {
    Button=0,
    HiliteButton,
    TitleLeft,
    TitleRight,
    PixmapTypeCount
};

struct ButtonDeco {
    bool small;
    const unsigned char *shadow;
    const unsigned char *dark;
    const unsigned char *mid;
    const unsigned char *light;
};

class KwinQinxClient;

// KwinQinxFactory ///////////////////////////////////////////////////////////

class KwinQinxFactory: public KDecorationFactory
{
public:
    KwinQinxFactory();
    ~KwinQinxFactory();
    virtual KDecoration *createDecoration(KDecorationBridge *b);
    virtual bool reset(unsigned long changed);

    static bool initialized();
    static KPixmap &pix(PixmapType p, bool active=true, bool small=false);
    static unsigned contrast();
    static bool mouseOver();
    static bool cornerDetail();
    static bool flipGradient();
    static Qt::AlignmentFlags titleAlign();

private:
    unsigned long readConfig();
    void createPixmaps();

private:
    static bool initialized_;
    static unsigned contrast_;
    static Qt::AlignmentFlags titlealign_;
    static KPixmap pix_[PixmapTypeCount][2][2];
    static bool mouseover_;
    static bool cornerdetail_;
    static bool flipgradient_;
};

inline bool KwinQinxFactory::initialized()  { return initialized_; }

inline KPixmap &KwinQinxFactory::pix(PixmapType p, bool active, bool small)
    { return pix_[p][active][small]; }

inline unsigned KwinQinxFactory::contrast() { return contrast_; }

inline bool KwinQinxFactory::mouseOver() { return mouseover_; }

inline bool KwinQinxFactory::cornerDetail() { return cornerdetail_; }

inline bool KwinQinxFactory::flipGradient() { return flipgradient_; }

inline Qt::AlignmentFlags KwinQinxFactory::titleAlign() { return titlealign_; }

// QinxButton ////////////////////////////////////////////////////////////////

class QinxButton : public QButton
{
public:
    QinxButton(KwinQinxClient *parent=0, const char *name=0,
               const QString &tip=NULL, bool small=false, bool left=false,
               ButtonType type=ButtonMax, const ButtonDeco *deco=0);
    ~QinxButton();

    void setDeco(const ButtonDeco *deco);
    QSize sizeHint() const;
    ButtonState lastMousePress() const;
    void reset();

private:
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void drawButton(QPainter *painter);

private:
    KwinQinxClient *client_;
    ButtonType type_;
    ButtonDeco const *deco_;
    ButtonState lastmouse_;
    bool mouseover_;
    bool small_;
    bool left_;
};

inline Qt::ButtonState QinxButton::lastMousePress() const
    { return lastmouse_; }

inline void QinxButton::reset() { repaint(false); }

// KwinQinxClient ////////////////////////////////////////////////////////////

class KwinQinxClient : public KDecoration
{
    Q_OBJECT
public:
    KwinQinxClient(KDecorationBridge *b, KDecorationFactory *f);
    virtual ~KwinQinxClient();

    virtual void init();

    virtual void activeChange();
    virtual void desktopChange();
    virtual void captionChange();
    virtual void iconChange();
    virtual void maximizeChange();
    virtual void shadeChange();

    virtual void borders(int &l, int &r, int &t, int &b) const;
    virtual void resize(const QSize &size);
    virtual QSize minimumSize() const;
    virtual Position mousePosition(const QPoint &point) const;

private:
    void addButtons(QHBoxLayout* layout, const QString &s, bool left);

    bool eventFilter(QObject *obj, QEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);
    void showEvent(QShowEvent*);

    bool isTool() const;

private slots:
    void maxButtonPressed();
    void menuButtonPressed();

private:
    QinxButton *button[ButtonTypeCount];
    QSpacerItem *titlebar_, *closespacer_;
    int  titleheight_;
};

} // namespace Qinx

#endif // KWINQINX_H
