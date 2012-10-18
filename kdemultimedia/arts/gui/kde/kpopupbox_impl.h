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

#ifndef KPOPUPBOX_IMPL_H
#define KPOPUPBOX_IMPL_H

#include <kframe_impl.h>
#include <artsgui.h>

class QBoxLayout;
class KPopupBox_widget;
//class KPopupBoxEventMapper; // The EventMapper isn't needed at present, but perhaps in the future...

namespace Arts {

/// The PopupBox

class KPopupBox_impl : virtual public Arts::PopupBox_skel, public Arts::KFrame_impl
{
public:
	/// selfreference like 'this'
	PopupBox self() { return PopupBox::_from_base( _copy() ); }
	/// Constructor
	KPopupBox_impl( KPopupBox_widget *w=0 );
	~KPopupBox_impl();

	/// The name of the widget
	std::string name();
	void name( const std::string & );

	/// The direction. LeftToRight (true) or TopToBottom (false). May get other values in the future.
	Direction direction();
	void direction( Direction );

	/// Set and Get the Widget.
	void widget( Arts::Widget );
	Arts::Widget widget();

private:
	std::string _name;
	bool _lefttoright;
	KPopupBox_widget *_widget;
//	KPopupBoxEventMapper *_mapper;

}; // class KPopupBox

} // namespace Arts

#endif

// vim: sw=4 ts=4

