    /*

    Copyright ( C ) 2002, 2003 Arnold Krille <arnold@arnoldarts.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or ( at your option ) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

    */

#ifndef KPOPUPBOX_PRIVATE_H
#define KPOPUPBOX_PRIVATE_H


class HandleDrag;
class ShowButton;
class OwnButton;
class KArtsWidget;
class OwnWidget;
class QBoxLayout;

#include <qframe.h>

class KPopupBox_widget : public QFrame
{
   Q_OBJECT
public:
	KPopupBox_widget( QWidget* =0, const char* =0);
	~KPopupBox_widget();

	Arts::Direction direction();
	void direction( Arts::Direction );

	void setWidget( Arts::Widget widget );
	Arts::Widget getWidget();

	void name( std::string );
private slots:
	void hide( bool );
	void own( bool );
private:
	QBoxLayout *_layout;

	QFrame *_titlebar;
	QBoxLayout *_titlebarlayout;
	HandleDrag *_drag;
	ShowButton *_showbutton;
	OwnButton *_ownbutton;

	OwnWidget *_artswidget;
};

// See kpopupbox_impl.h - The eventmapper isn't needed at present, but perhaps in the future...

/*class KPopupBoxEventMapper : public QObject {
   Q_OBJECT
public:
	KPopupBoxEventMapper(  KPopupBox_widget *widget, Arts::KPopupBox_impl *impl )
	  : QObject(  widget,"" ), _widget(  widget ), _impl(  impl )
	{}
private:
	KPopupBox_widget *_widget;
	Arts::KPopupBox_impl *_impl;
};*/

#include <qpainter.h>
#include <qstyle.h>

class HandleDrag : public QWidget {
   Q_OBJECT
public:
	HandleDrag( QWidget *parent, const char* name=0 ) : QWidget( parent,name ) {}
	void paintEvent( QPaintEvent * ) {
		QPainter p( this );
		QStyle::SFlags flags = QStyle::Style_Default;
		if( width() < height() ) flags |= QStyle::Style_Horizontal;
		style().drawPrimitive( QStyle::PE_DockWindowHandle, &p, rect(), colorGroup(), flags );
	}
signals:
	void clicked();
protected:
	virtual void mouseDoubleClickEvent( QMouseEvent * ) {
		emit clicked();
	}
};

static const char* const close_xpm[]  = { "5 5 2 1", "# c black", ". c None", "#...#", ".#.#.", "..#..", ".#.#.", "#...#"};
static const char* const up_xpm[]     = { "5 5 2 1", "# c black", ". c None", ".....", "..#..", ".###.", "#####", "....."};
static const char* const down_xpm[]   = { "5 5 2 1", "# c black", ". c None", ".....", "#####", ".###.", "..#..", "....."};
static const char* const left_xpm[]   = { "5 5 2 1", "# c black", ". c None", "...#.", "..##.", ".###.", "..##.", "...#."};
static const char* const right_xpm[]  = { "5 5 2 1", "# c black", ". c None", ".#...", ".##..", ".###.", ".##..", ".#..."};
static const char* const inside_xpm[] = { "5 5 2 1", "# c black", ". c None", "#####", "#...#", "#...#", "#...#", "#####"};
static const char* const own_xpm[]    = { "5 5 2 1", "# c black", ". c None", "###..", "#.###", "###.#", ".#..#", ".####"};

#include <qpushbutton.h>
#include <qlayout.h>

class ShowButton : public QPushButton {
   Q_OBJECT
private:
	QBoxLayout::Direction _dir;
	QPixmap _pmleft, _pmright, _pmup, _pmdown;
public:
	ShowButton( QWidget *parent, const char* name=0 ) : QPushButton( parent,name ), _dir( QBoxLayout::LeftToRight )
	{
		connect( this, SIGNAL( toggled( bool ) ), this, SLOT( owntoggle( bool ) ) );
		setToggleButton( true );
		_pmleft = QPixmap( const_cast<const char**>( left_xpm ) );
		_pmright = QPixmap( const_cast<const char**>( right_xpm ) );
		_pmup = QPixmap( const_cast<const char**>( up_xpm ) );
		_pmdown = QPixmap( const_cast<const char**>( down_xpm ) );
		setPixmap( _pmright );
	}

	void direction( QBoxLayout::Direction n ) { _dir=n; }
public slots:
	void owntoggle( bool b ) {
		switch( _dir )
		{
			case QBoxLayout::BottomToTop:
				if( b ) setPixmap( _pmdown );
				else setPixmap( _pmup );
				break;
			case QBoxLayout::TopToBottom:
				if( b ) setPixmap( _pmup );
				else setPixmap( _pmdown );
				break;
			case QBoxLayout::LeftToRight:
				if( b ) setPixmap( _pmright );
				else setPixmap( _pmleft );
				break;
			case QBoxLayout::RightToLeft:
				if( b ) setPixmap( _pmleft );
				else setPixmap( _pmright );
				break;
		}
	}
public:
	QSize minimumSizeHint() const {
		int wh = style().pixelMetric( QStyle::PM_DockWindowHandleExtent, this );
		return QSize( wh, wh );
	}
	QSizePolicy sizePolicy() const { return QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ); }
	QSize minimumSize() const { return minimumSizeHint(); }
	QSize sizeHint() const { return minimumSize(); }

	void drawButton(  QPainter * p )
	{
		p->fillRect( 0,0, width(), height(), QBrush( colorGroup().brush( QColorGroup::Background ) ) );
		p->drawPixmap( ( width() - pixmap()->width() ) / 2, ( height() - pixmap()->height() ) / 2, *pixmap() );
	}
};

class OwnButton : public QPushButton {
   Q_OBJECT
private:
	QPixmap _pmown, _pminside;
public:
	OwnButton( QWidget *parent, const char* name=0 ) : QPushButton( parent,name )
	{
		connect( this, SIGNAL( toggled( bool ) ), this, SLOT( toggle( bool ) ) );
		setToggleButton( true );
		_pmown = QPixmap( const_cast<const char**>( own_xpm ) );
		_pminside = QPixmap( const_cast<const char**>( inside_xpm ) );
		setPixmap( _pmown );
	}

public slots:
	void toggle( bool b ) {
			if( b ) setPixmap( _pminside );
			else setPixmap(  _pmown );
	}
public:
	QSize minimumSizeHint() const {
		int wh = style().pixelMetric( QStyle::PM_DockWindowHandleExtent, this );
		return QSize( wh, wh );
	}
	QSizePolicy sizePolicy() const { return QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ); }
	QSize minimumSize() const { return minimumSizeHint(); }
	QSize sizeHint() const { return minimumSize(); }

	void drawButton( QPainter * p )
	{
		p->fillRect( 0,0, width(), height(), QBrush( colorGroup().brush( QColorGroup::Background ) ) );
		p->drawPixmap( ( width() - pixmap()->width() ) / 2, ( height() - pixmap()->height() ) / 2, *pixmap() );
	}
};

#include <kartswidget.h>

class OwnWidget : public KArtsWidget
{
   Q_OBJECT
	ShowButton *_b;
public:
	OwnWidget( ShowButton* b, QWidget* p, const char* n=0, WFlags f=0 ) : KArtsWidget( p,n,f ) { _b = b; }
	~OwnWidget() {}
public slots:
	void closeEvent( QCloseEvent * ) { _b->toggle(); }
};

#endif

// vim: sw=4 ts=4

