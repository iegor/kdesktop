/*****************************************************************
kwin - the KDE window manager
								
Copyright (C) 1999, 2000 Matthias Ettrich <ettrich@kde.org>
******************************************************************/
#ifndef STDCLIENT_H
#define STDCLIENT_H
#include <qlayout.h>
#include <qvariant.h>
#include <kdecoration.h>
#include <kdecorationfactory.h>
#include <qtoolbutton.h>
class QLabel;
class QSpacerItem;

namespace KDE1 {

enum ButtonType {
    ButtonMenu=0,
    ButtonSticky,
    ButtonMinimize,
    ButtonMaximize,
    ButtonClose,
    ButtonHelp,
    ButtonTypeCount
};

class StdClient : public KDecoration
{
    Q_OBJECT
public:
    StdClient( KDecorationBridge* b, KDecorationFactory* f );
    ~StdClient();
    void init();
    QSize minimumSize() const;
    void borders( int& left, int& right, int& top, int& bottom ) const;
    void reset( unsigned long mask );
    void resize( const QSize& s );
    void shadeChange() {};
    Position mousePosition( const QPoint& p ) const { return KDecoration::mousePosition( p ); }
protected:
    bool eventFilter( QObject* o, QEvent* e );
    void resizeEvent( QResizeEvent* );
    void paintEvent( QPaintEvent* );

    void mouseDoubleClickEvent( QMouseEvent * );
    void wheelEvent( QWheelEvent * );
    void captionChange();
    void iconChange();
    void maximizeChange();
    void desktopChange();
    void activeChange();

private:
    void addButtons(QBoxLayout* hb, const QString& buttons);

private slots:
    void menuButtonPressed();
    void maxButtonClicked( ButtonState );

private:
    QToolButton* button[ButtonTypeCount];
    QSpacerItem* titlebar;
};

class StdToolClient : public KDecoration
{
    Q_OBJECT
public:
    StdToolClient( KDecorationBridge* b, KDecorationFactory* f );
    ~StdToolClient();
    void init();
    QSize minimumSize() const;
    void borders( int& left, int& right, int& top, int& bottom ) const;
    void reset( unsigned long mask );
    void resize( const QSize& s );
    void shadeChange() {};
    void activeChange() {};
    void iconChange() {};
    void maximizeChange() {};
    void desktopChange() {};
    Position mousePosition( const QPoint& p ) const { return KDecoration::mousePosition( p ); }
protected:
    bool eventFilter( QObject* o, QEvent* e );
    void resizeEvent( QResizeEvent* );
    void paintEvent( QPaintEvent* );

    void mouseDoubleClickEvent( QMouseEvent * );
    void wheelEvent( QWheelEvent * );
    void captionChange();

private:
    QToolButton* closeBtn;
    QSpacerItem* titlebar;
};



/*
  Like QToolButton, but provides a clicked(ButtonState) signals that
  has the last pressed mouse button as argument
 */
class ThreeButtonButton: public QToolButton
{
    Q_OBJECT
public:
  ThreeButtonButton ( QWidget *parent = 0, const char* name = 0 )
      : QToolButton( parent, name )
    {
	connect( this, SIGNAL( clicked() ), this, SLOT( handleClicked() ) );
        setCursor( arrowCursor );
    }
    ~ThreeButtonButton () {}

signals:
    void clicked( ButtonState );

protected:
    void mousePressEvent( QMouseEvent* e )
    {
	last_button = e->button();
	QMouseEvent me ( e->type(), e->pos(), e->globalPos(), LeftButton, e->state() );
	QToolButton::mousePressEvent( &me );
    }

    void mouseReleaseEvent( QMouseEvent* e )
    {
	QMouseEvent me ( e->type(), e->pos(), e->globalPos(), LeftButton, e->state() );
	QToolButton::mouseReleaseEvent( &me );
    }

private slots:
    void handleClicked()
    {
	emit clicked( last_button );
    }

private:
    ButtonState last_button;

};

class StdFactory : public KDecorationFactory
{
public:
    StdFactory();
    ~StdFactory();
    KDecoration* createDecoration( KDecorationBridge* b );
    bool reset( unsigned long mask );
    virtual bool supports( Ability ability );
};

}

#endif
