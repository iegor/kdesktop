/*
 *	CDE KWin client - emulates the look and feel
 *	of dtwm, the CDE window manager.
 *
 *	Copyright (c) 2000-2001, 2002
 *		Chris Lee       <lee@azsites.com>
 *		Lennart Kudling <kudling@kde.org>
 *      	Fredrik Höglund <fredrik@kde.org>
 *
 *	Copyright (c) 2003
 *		Luciano Montanaro <mikelima@cirulla.net>
 *
 *	Originally based on the KStep client.
 *
 *	Distributed under the terms of the BSD license.
 */

#ifndef __CDECLIENT_H
#define __CDECLIENT_H

#include <qbutton.h>
#include <qbitmap.h>
#include <kpixmap.h>
#include <kdecoration.h>
#include <kdecorationfactory.h>

class QLabel;
class QBoxLayout;
class QVBoxLayout;
class QSpacerItem;

namespace CDE {

class CdeClient;

enum Buttons { BtnMenu=0, BtnHelp, BtnIconify, BtnMax, BtnClose, BtnCount };

class CdeButton : public QButton
{
public:
    CdeButton( CdeClient* parent=0, const char* name=0, int btnType=0,
               const QString& tip=NULL, int realize_btns = LeftButton );
    void reset();
    ButtonState lastButton() { return last_button; }

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    virtual void drawButton(QPainter *p);

private:
    CdeClient *m_parent;
    int m_btnType;
    int m_realize_buttons;
    ButtonState last_button;
};

class CdeClient : public KDecoration
{
    Q_OBJECT
public:
    CdeClient(KDecorationBridge *b, KDecorationFactory *f);
    ~CdeClient() {};
    void init();

protected:
    bool eventFilter(QObject *o, QEvent *e);
    void resizeEvent( QResizeEvent* );
    void paintEvent( QPaintEvent* );

    void showEvent(QShowEvent *);
    void addClientButtons( const QString& );
    void mouseDoubleClickEvent( QMouseEvent* );
    void wheelEvent( QWheelEvent * );
    void captionChange();
    void desktopChange();
    void activeChange();
    void shadeChange();
    void iconChange();
    QSize minimumSize() const;
    void resize(const QSize &size);
    void borders(int &left, int &right, int &top, int &bottom) const;
    void mousePressEvent( QMouseEvent* );
    void mouseReleaseEvent( QMouseEvent* );
    void maximizeChange();
    Position mousePosition( const QPoint& p ) const;

protected slots:
    void menuButtonPressed();
    void menuButtonReleased();
    void maximizeButtonClicked();

private:
    CdeButton* button[BtnCount];
    QVBoxLayout* mainLayout;
    QBoxLayout*  titleLayout;
    QSpacerItem* titlebar;
    bool titlebarPressed;
    bool closing;
};

class CdeClientFactory: public QObject, public KDecorationFactory
{
public:
    CdeClientFactory();
    virtual ~CdeClientFactory();
    virtual KDecoration *createDecoration(KDecorationBridge *);
    virtual bool supports( Ability ability );
    virtual bool reset(unsigned long changed);

    QValueList< CdeClientFactory::BorderSize > borderSizes() const;

};

}

#endif

