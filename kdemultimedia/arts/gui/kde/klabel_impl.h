    /*

    Copyright ( C ) 2002 Arnold Krille <arnold@arnoldarts.de>

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

#ifndef ARTS_KLABEL_IMPL
#define ARTS_KLABEL_IMPL

#include <kframe_impl.h>
#include <qframe.h>
#include <qpainter.h>
#include <qfontmetrics.h>
#include <qfont.h>

#include <artsgui.h>
#include <kdelibs_export.h>

class RotateLabel;

namespace Arts {

class KDE_EXPORT KLabel_impl : virtual public Arts::Label_skel,
                    public Arts::KFrame_impl
{
protected:
	RotateLabel* _label;
public:
	KLabel_impl( QFrame* w=0 );
	void constructor( Widget p ) { parent( p ); }

	std::string text();
	void text( const std::string& newtext );

	long align();
	void align( long );

	long fontsize() { return -1; }
	void fontsize( long );

	std::string fontfamily() { return ""; }
	void fontfamily( const std::string& );

	Arts::TextBottom bottom();
	void bottom( Arts::TextBottom );

}; // class

} // namespace

class RotateLabel : public QFrame {
   Q_OBJECT
public:
	RotateLabel( QWidget*, const char* =0 );
	void paintEvent( QPaintEvent* );

	void fontfamily( QString );
	void fontsize( int );
	void title( QString );
	QString title() { return _title; }

	long align() { return _align; }
	void align( long );

	Arts::TextBottom bottom() { return _bottom; }
	void bottom( Arts::TextBottom );
private:
	long _align;
	Arts::TextBottom _bottom;
	QString _title;
};

#endif

// vim: sw=4 ts=4
