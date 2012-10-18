/*  This file is part of the KDE project
    Copyright (C) 2002 Arnold Krille <arnold@arnoldarts.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

*/

#include <kglobal.h>
#include <klocale.h>

#include "artsmoduleseffects.h"
#include "connect.h"
#include "debug.h"

using namespace Arts;

namespace Arts {

class StereoCompressorGuiFactory_impl : virtual public StereoCompressorGuiFactory_skel
{
public:
	Widget createGui( Object object )
	{
		KGlobal::locale()->insertCatalogue( "artsmodules" );

		arts_return_val_if_fail(!object.isNull(), Arts::Widget::null() );

		Synth_STEREO_COMPRESSOR comp = DynamicCast(object);
		arts_return_val_if_fail(!comp.isNull(), Arts::Widget::null());

		Poti attack;
		attack.caption(i18n("attack").utf8().data());
		attack.color("blue");
		attack.min(0.1); attack.max(250);
		attack.value( comp.attack() );
		attack.range(250);
		connect( attack, "value_changed", comp, "attack" );

		Poti release;
		release.caption(i18n("release").utf8().data());
		release.color("blue");
		release.min(0.1); release.max(250);
		release.value( comp.release() );
		release.range(250);
		connect( release, "value_changed", comp, "release" );

		Poti threshold;
		threshold.caption(i18n("thresh.").utf8().data());
		threshold.min(0.00001); threshold.max(1);
		threshold.value( comp.threshold() );
		threshold.logarithmic( 2.0 );
		threshold.range(200);
		connect( threshold, "value_changed", comp, "threshold" );

		Poti ratio;
		ratio.caption(i18n("ratio").utf8().data());
		ratio.min(0); ratio.max(1);
		ratio.value( comp.ratio() );
		ratio.range(200);
		connect( ratio, "value_changed", comp, "ratio" );

		Poti output;
		output.caption(i18n("output").utf8().data());
		output.min(0.1); output.max(10.0);
		output.value( comp.output() );
		output.logarithmic( 2.0 );
		output.range(200);
		connect( output, "value_changed", comp, "output" );

		Button bon;
		bon.text(i18n("Bypass").utf8().data());
		bon.toggle( true );
		connect( bon, "pressed_changed", comp, "thru" );

		LayoutBox hbox;
		hbox.direction( LeftToRight ); hbox.layoutmargin( 5 ); hbox.spacing( 5 );
		PopupBox timesbox;
		timesbox.name( "Timings" ); timesbox.direction( LeftToRight );
		LayoutBox times;
		times.direction( LeftToRight ); times.spacing( 5 );

		hbox.addWidget( timesbox );
			times.addSpace( 5 );
			times.addWidget( attack );
			times.addWidget( release );
			times.addSpace( 5 );
		timesbox.widget( times );
		hbox.addWidget( threshold );
		hbox.addWidget( ratio );
		hbox.addWidget( output );
		hbox.addWidget( bon );
		hbox.addStretch( 10 );

		return hbox;
	}
};

// vim:sw=4:ts=4

REGISTER_IMPLEMENTATION(StereoCompressorGuiFactory_impl);

}
