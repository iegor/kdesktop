//////////////////////////////////////////////////////////////////////////////
// polyester.h
// -------------------
// Polyester window decoration for KDE
// Copyright (c) 2005 Marco Martin <notmart@gmail.com>
// -------------------
// derived from Smooth Blend
// Copyright (c) 2005 Ryan Nickell <p0z3r@users.sourceforge.net>
// -------------------
// Shadow engine from Plastik decoration
// Copyright (C) 2003 Sandro Giessl <ceebx@users.sourceforge.net>
// -------------------
//
//   This program is free software; you can redistribute it and/or modify 
//   it under the terms of the GNU General Public License as published by 
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the
//   Free Software Foundation, Inc.,
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//////////////////////////////////////////////////////////////////////////////

#ifndef polyester_H
#define polyester_H

#include <qbutton.h>
#include <kdecoration.h>
#include <kdecorationfactory.h>

#define TITLEBAR_GLASS     1

#define BUTTON_GRADIENT    0
#define BUTTON_GLASS       1
#define BUTTON_REVGRADIENT 2
#define BUTTON_FLAT        3

class QSpacerItem;
class QPoint;

namespace polyester {

class polyesterClient;

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

// polyesterFactory /////////////////////////////////////////////////////////
//
// add variables and flags for config like...
//
// public:  type function()
// private: type var_
// inline type function(){ return var_ };
//

class polyesterFactory: public KDecorationFactory {
public:
    polyesterFactory();
    virtual ~polyesterFactory();
    virtual KDecoration *createDecoration(KDecorationBridge *b);
    virtual bool reset(unsigned long changed);
    static bool initialized();
    static Qt::AlignmentFlags titleAlign();
    static int contrast();
    static bool roundedCorners();
    static int titleSize();
    static int buttonSize();
    static bool squareButton();
    static int frameSize();
    static int roundSize();
    static QFont titleFontTool() { return m_titleFontTool; }
    static bool titleShadow();
    static int titleBarStyle() { return titleBarStyle_;}
    static int buttonStyle() { return buttonStyle_;}
    static bool animateButtons() { return animatebuttons; }
    static bool noModalButtons() { return nomodalbuttons; }
    static bool lightBorder() { return lightBorder_; }
    static int getBtnComboBox() { return btnComboBox; }
    static bool menuClosed() { return menuClose; }

private:
    bool readConfig();

private:
    static bool initialized_;
    static Qt::AlignmentFlags titlealign_;
    static int contrast_;
    static bool cornerflags_;
    static int titlesize_;
    static int buttonsize_;
    static bool squareButton_;
    static bool lightBorder_;
    static int framesize_;
    static int titleBarStyle_;
    static int buttonStyle_;
    static int roundsize_;
    static QFont m_titleFontTool;
    static bool titleshadow_;
    static bool animatebuttons;
    static bool nomodalbuttons;
    static int btnComboBox;
    static bool menuClose;
};

inline bool polyesterFactory::initialized() {
    return initialized_;
}
inline Qt::AlignmentFlags polyesterFactory::titleAlign() {
    return titlealign_;
}

inline int polyesterFactory::contrast(){
   return contrast_;
}

inline bool polyesterFactory::roundedCorners() {
    return cornerflags_;
}
inline int polyesterFactory::titleSize() {
    return titlesize_;
};
inline int polyesterFactory::buttonSize() {
    return buttonsize_;
};
inline bool polyesterFactory::squareButton() {
  return squareButton_;
};
inline int polyesterFactory::frameSize() {
    return framesize_;
};
inline int polyesterFactory::roundSize() {
    return roundsize_;
};
inline bool polyesterFactory::titleShadow() {
    return titleshadow_;
};

// polyesterButton //////////////////////////////////////////////////////////

class polyesterButton : public QButton {
    Q_OBJECT
public:
    polyesterButton(polyesterClient *parent=0, const char *name=0,
                  const QString &tip=NULL,
                  ButtonType type=ButtonHelp,
                  int button_size=18,
                  bool squareButton = true,
                  bool toggle=false);
                  //const unsigned char *bitmap=0);
    ~polyesterButton();

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
    void genButtonPix( bool active );

private:
    polyesterClient *client_;
    ButtonType type_;
    int size_;
    double buttonProportions_;
    QBitmap *deco_;
    QPixmap *pixmap[2][4];
    QImage *buttonImgActive;
    QImage *buttonImgInactive;
    ButtonState lastmouse_;
    bool buttonImgActive_created;
    bool buttonImgInactive_created;
    bool hover_;
    bool m_clicked;
    QTimer *animTmr;
    uint animProgress;
};

inline Qt::ButtonState polyesterButton::lastMousePress() const {
    return lastmouse_;
}
inline void polyesterButton::reset() {
    repaint(false);
}

// polyesterClient //////////////////////////////////////////////////////////

class polyesterClient : public KDecoration {
    Q_OBJECT
public:
    polyesterClient(KDecorationBridge *b, KDecorationFactory *f);
    virtual ~polyesterClient();

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
    int frameSize, titleSize, buttonSize;
    bool roundedCorners;

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
    polyesterButton *button[ButtonTypeCount];
    QSpacerItem *titlebar_;
    bool pixmaps_created;
    bool closing;
    int s_titleHeight;
    QFont s_titleFont;
    int handlebar;
    bool maskDirty, aDoubleBufferDirty, iDoubleBufferDirty;
    QPixmap activeBuff, inactiveBuff;

    void create_pixmaps();
    void delete_pixmaps();
};

} // namespace polyester

#endif // polyester_H
