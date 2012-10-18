    /*

    Copyright (C) 2001 Stefan Westerfeld
                       stefan@space.twc.de

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

#include "artsgui.h"
#include "debug.h"

using namespace Arts;
using namespace std;

namespace Arts {

class GenericGuiFactory_impl : public GenericGuiFactory_skel {
public:
	Widget createGui(Object runningObject);
};
REGISTER_IMPLEMENTATION(GenericGuiFactory_impl);

}

Widget GenericGuiFactory_impl::createGui(Object runningObject)
{
	Arts::Widget result = Arts::Widget::null();
	arts_return_val_if_fail(!runningObject.isNull(), result);

	TraderQuery query;
	query.supports("Interface","Arts::GuiFactory");
	query.supports("CanCreate",runningObject._interfaceName());

	vector<TraderOffer> *queryResults = query.query();
	if(queryResults->size())
	{
		Arts::GuiFactory factory = SubClass((*queryResults)[0].interfaceName());
		result = factory.createGui(runningObject);
	}
	delete queryResults;
	return result;
}
