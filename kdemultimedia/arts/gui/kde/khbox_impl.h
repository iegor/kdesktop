    /*

    Copyright (C) 2000,2001 Stefan Westerfeld
                            stefan@space.twc.de
                       2002 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

    */

#include "kframe_impl.h"

class QHBox;

namespace Arts {

class KHBox_impl : virtual public Arts::HBox_skel,
                   public Arts::KFrame_impl
{
private:
	long _spacing;

protected:
	QHBox * _qhbox;

public:
	KHBox_impl( QHBox * w = 0 );

	long spacing();
	void spacing( long );
};

}

// vim: sw=4 ts=4
