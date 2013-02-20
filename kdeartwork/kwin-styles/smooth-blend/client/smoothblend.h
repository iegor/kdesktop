//////////////////////////////////////////////////////////////////////////////
// smoothblend.h
// -------------------
// Smooth Blend window decoration for KDE
// -------------------
// Copyright (c) 2005 Ryan Nickell <p0z3r@users.sourceforge.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef SMOOTHBLEND_H
#define SMOOTHBLEND_H

#include <qbutton.h>
#include <kdecoration.h>
#include <kdecorationfactory.h>

class QSpacerItem;
class QPoint;

namespace smoothblend {

class smoothblendClient;

enum ButtonType {
    ButtonHelp=0,
    ButtonMax,
    ButtonMin,
    ButtonClose,
    ButtonMenu,
    ButtonSticky,
    ButtonAbove,
    ButtonBelow,
    ButtonShade,
    ButtonTypeCount
};

// smoothblendFactory /////////////////////////////////////////////////////////
//
// add variables and flags for config like...
//
// public:  type function()
// private: type var_
// inline type function(){ return var_ };
//

class smoothblendFactory: public KDecorationFactory {
public:
    smoothblendFactory();
    virtual ~smoothblendFactory();
    virtual KDecoration *createDecoration(KDecorationBridge *b);
    virtual bool reset(unsigned long changed);
    static bool initialized();
    static Qt::AlignmentFlags titleAlign();
    static bool roundedCorners();
    static int titleSize();
    static int buttonSize();
    static int frameSize();
    static int roundSize();
    static QFont titleFontTool() { return m_titleFontTool; }
    static bool titleShadow();
    static bool animateButtons() { return animatebuttons; }
    static int getBtnComboBox() { return btnComboBox; }
    static bool menuClosed() { return menuClose; }

private:
    bool readConfig();

private:
    static bool initialized_;
    static Qt::AlignmentFlags titlealign_;
    static bool cornerflags_;
    static int titlesize_;
    static int buttonsize_;
    static int framesize_;
    static int roundsize_;
    static QFont m_titleFontTool;
    static bool titleshadow_;
    static bool animatebuttons;
    static int btnComboBox;
    static bool menuClose;
};

inline bool smoothblendFactory::initialized() {
    return initialized_;
}
inline Qt::AlignmentFlags smoothblendFactory::titleAlign() {
    return titlealign_;
}
inline bool smoothblendFactory::roundedCorners() {
    return cornerflags_;
}
inline int smoothblendFactory::titleSize() {
    return titlesize_;
}
inline int smoothblendFactory::buttonSize() {
    return buttonsize_;
}
inline int smoothblendFactory::frameSize() {
    return framesize_;
}
inline int smoothblendFactory::roundSize() {
    return roundsize_;
}
inline bool smoothblendFactory::titleShadow() {
    return titleshadow_;
}

// smoothblendButton //////////////////////////////////////////////////////////

class smoothblendButton : public QButton {
    Q_OBJECT
public:
    smoothblendButton(smoothblendClient *parent=0, const char *name=0,
                  const QString &tip=NULL,
                  ButtonType type=ButtonHelp,
                  int button_size=18,
                  bool toggle=false);
                  //const unsigned char *bitmap=0);
    ~smoothblendButton();

    void setBitmap(const unsigned char *bitmap);
    QSize sizeHint() const;
    ButtonState lastMousePress() const;
    void reset();
    QImage getButtonImage(ButtonType type);
    virtual void setOn(bool on);
    virtual void setDown(bool on);

protected slots:
    void animate();
    void buttonClicked();
    void buttonReleased();

private:
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void drawButton(QPainter *painter);

private:
    smoothblendClient *client_;
    ButtonType type_;
    int size_;
    QBitmap *deco_;
    QPixmap *pixmap[2][4];
    ButtonState lastmouse_;
    bool hover_;
    bool m_clicked;
    QTimer *animTmr;
    uint animProgress;
};

inline Qt::ButtonState smoothblendButton::lastMousePress() const {
    return lastmouse_;
}
inline void smoothblendButton::reset() {
    repaint(false);
}

// smoothblendClient //////////////////////////////////////////////////////////

class smoothblendClient : public KDecoration {
    Q_OBJECT
public:
    smoothblendClient(KDecorationBridge *b, KDecorationFactory *f);
    virtual ~smoothblendClient();

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

    QPixmap getTitleBarTile(bool active) const
    {
        return active ? *aTitleBarTile : *iTitleBarTile;
    }

private:
    void addButtons(QBoxLayout* layout, const QString& buttons, int buttonSize = 18);
    bool eventFilter(QObject *obj, QEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *);
    void showEvent(QShowEvent *);
    void updateMask();
    void _resetLayout();
    QVBoxLayout *mainLayout_;
    QHBoxLayout *titleLayout_;
    QSpacerItem *topSpacer_,
                *titleSpacer_,
                *leftTitleSpacer_, *rightTitleSpacer_,
                *decoSpacer_,
                *leftSpacer_, *rightSpacer_,
                *bottomSpacer_, *windowSpacer_;
    QPixmap *aCaptionBuffer, *iCaptionBuffer;

private slots:
    void maxButtonPressed();
    void menuButtonPressed();
    void menuButtonReleased();
    void aboveButtonPressed();
    void belowButtonPressed();
    void shadeButtonPressed();
    void keepAboveChange(bool);
    void keepBelowChange(bool);

signals:
    void keepAboveChanged(bool);
    void keepBelowChanged(bool);

private:
    QPixmap *aTitleBarTile, *iTitleBarTile, *aTitleBarTopTile, *iTitleBarTopTile;
    smoothblendButton *button[ButtonTypeCount];
    QSpacerItem *titlebar_;
    bool pixmaps_created;
    int s_titleHeight;
    QFont s_titleFont;
    int handlebar;
    bool closing;

    void create_pixmaps();
    void delete_pixmaps();
};

} // namespace smoothblend

#endif // SMOOTHBLEND_H
