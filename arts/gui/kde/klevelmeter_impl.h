/*
    Copyright ( C ) 2003 Arnold Krille <arnold@arnoldarts.de>

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

#ifndef ARTS_KLEVELMETER_IMPL_H
#define ARTS_KLEVELMETER_IMPL_H

#include "artsgui.h"

#include "kframe_impl.h"

class QFrame;

class KLevelMeter_Private;

namespace Arts { // namespace Arts

class KLevelMeter_impl : virtual public Arts::LevelMeter_skel,
                         virtual public Arts::KFrame_impl
{
private:
	KLevelMeter_Private *p;
public:
	KLevelMeter_impl( QFrame* =0 );

	LevelMeterStyle style();
	void style( LevelMeterStyle );

	long substyle();
	void substyle( long );

	long count();
	void count( long );

	long peakfalloff();
	void peakfalloff( long );

	float mindB();
	void mindB( float );
	float maxdB();
	void maxdB( float );

	float invalue();
	void invalue( float );

	Arts::Direction direction();
	void direction( Arts::Direction );
}; //class KLevelMeter_impl

} // namespace Arts

#endif
// vim: sw=4 ts=4

