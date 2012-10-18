/*

    Copyright (  C ) 2003 Arnold Krille <arnold@arnoldarts.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation;
    version 2 of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

    */

#include <artsmoduleseffects.h>
#include <connect.h>
#include <debug.h>

namespace Arts {

class StereoVolumeControlGuiFactory_impl : virtual public StereoVolumeControlGuiFactory_skel
{
public:
	Widget createGui( Object object )
	{
		arts_return_val_if_fail( !object.isNull(), Arts::Widget::null() );
		StereoVolumeControl svc = DynamicCast( object );
		arts_return_val_if_fail( !svc.isNull(), Arts::Widget::null() );

		return StereoVolumeControlGui( svc );
	}
};

REGISTER_IMPLEMENTATION( StereoVolumeControlGuiFactory_impl );

}
// vim: sw=4 ts=4

