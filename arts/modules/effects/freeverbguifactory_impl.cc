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

#include "artsmoduleseffects.h"
#include "debug.h"
#include "connect.h"

#include <kglobal.h>
#include <klocale.h>

using namespace std;
using namespace Arts;

namespace Arts {

class FreeverbGuiFactory_impl : public FreeverbGuiFactory_skel
{
public:
	Widget createGui(Object freeverb);
};

REGISTER_IMPLEMENTATION(FreeverbGuiFactory_impl);

}

Widget FreeverbGuiFactory_impl::createGui(Object object)
{
	KGlobal::locale()->insertCatalogue(  "artsmodules" );
	arts_return_val_if_fail(!object.isNull(), Arts::Widget::null());

	Synth_FREEVERB freeverb = DynamicCast(object);
	arts_return_val_if_fail(!freeverb.isNull(), Arts::Widget::null());

	HBox hbox;
	hbox.width(330); hbox.height(80); hbox.show();

	Poti roomsize;
	roomsize.x(20); roomsize.y(10); roomsize.caption(i18n("roomsize").utf8().data());
	roomsize.color("red"); roomsize.min(0); roomsize.max(1);
	roomsize.value(freeverb.roomsize());
	roomsize.range(100);
	roomsize.parent(hbox);
	roomsize.show();
	connect(roomsize,"value_changed", freeverb, "roomsize");
	hbox._addChild(roomsize,"roomsizeWidget");

	Poti damp;
	damp.x(80); damp.y(10); damp.caption(i18n("damp").utf8().data());
	damp.color("red"); damp.min(0); damp.max(1);
	damp.value(freeverb.damp());
	damp.range(100);
	damp.parent(hbox);
	damp.show();
	connect(damp,"value_changed", freeverb, "damp");
	hbox._addChild(damp,"dampWidget");

	Poti wet;
	wet.x(140); wet.y(10); wet.caption(i18n("wet").utf8().data());
	wet.color("red"); wet.min(0); wet.max(1);
	wet.value(freeverb.wet());
	wet.range(100);
	wet.parent(hbox);
	wet.show();
	connect(wet,"value_changed", freeverb, "wet");
	hbox._addChild(wet,"wetWidget");

	Poti dry;
	dry.x(200); dry.y(10); dry.caption(i18n("dry").utf8().data());
	dry.color("red"); dry.min(0); dry.max(1);
	dry.value(freeverb.dry());
	dry.range(100);
	dry.parent(hbox);
	dry.show();
	connect(dry,"value_changed", freeverb, "dry");
	hbox._addChild(dry,"dryWidget");

	Poti width;
	width.x(260); width.y(10); width.caption(i18n("width").utf8().data());
	width.color("red"); width.min(0); width.max(1);
	width.value(freeverb.width());
	width.range(100);
	width.parent(hbox);
	width.show();
	connect(width,"value_changed", freeverb, "width");
	hbox._addChild(width,"widthWidget");

	return hbox;
}
