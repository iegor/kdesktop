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

#ifndef ARTS_KLAYOUTBOX_IMPL_H
#define ARTS_KLAYOUTBOX_IMPL_H

#include "kframe_impl.h"

#include <artsgui.h>
#include <kdelibs_export.h>

class KArtsWidget;
class QBoxLayout;

namespace Arts {

class KDE_EXPORT KLayoutBox_impl : virtual public Arts::LayoutBox_skel,
                        public Arts::KFrame_impl
{
protected:
	QFrame* _qframe;
	QBoxLayout* _layout;

	LayoutBox self() { return LayoutBox::_from_base( _copy() ); }
public:
	KLayoutBox_impl( QFrame* w=0 );
	~KLayoutBox_impl();

	Direction direction();
	void direction( Direction );

	void addWidget( Arts::Widget, long, long );
	void addWidget( Arts::Widget w, long n ) { addWidget( w,n,0 ); }
	void addWidget( Arts::Widget w ) { addWidget( w,0,0 ); }

	void insertWidget( long, Arts::Widget, long, long );
	void insertWidget( long i, Arts::Widget w, long n ) { insertWidget( i,w,n,0 ); }
	void insertWidget( long i, Arts::Widget w ) {insertWidget( i,w,0,0 ); }

	void addStretch( long );
	void addStretch() { addStretch( 0 ); }

	void addSpace( long );
	void addStrut( long );

	void addSeparator( long, long );
	void addSeparator( long ) { addSeparator( 0, 0 ); }
	void addSeparator() { addSeparator( 0, 0 ); }

	void addLine( long, long, long, long );
	void addLine( long width, long space, long stretch ) { addLine( width, space, stretch, 0 ); }
	void addLine( long width, long space ) { addLine( width, space, 0, 0 ); }

	long spacing();
	void spacing( long );

	long layoutmargin();
	void layoutmargin( long );
}; // class

} // namespace

class KDE_EXPORT KLayoutBox_Separator : public QWidget {
   Q_OBJECT
public:
	KLayoutBox_Separator( QWidget*, const char* =0 );
	void resizeEvent( QResizeEvent* );
	void paintEvent( QPaintEvent* );
	QSize minimumSizeHint() const;
};

class KLayoutBox_Line : public QWidget {
   Q_OBJECT
private:
	int _width, _space;
public:
	KLayoutBox_Line( int, int, QWidget*, const char* =0 );
	void paintEvent( QPaintEvent* );
	QSize minimumSizeHint() const;
};

#endif

// vim: sw=4 ts=4

