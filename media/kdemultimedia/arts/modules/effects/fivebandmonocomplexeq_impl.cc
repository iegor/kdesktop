/*

    Copyright ( C ) 2002 Arnold Krille <arnold@arnoldarts.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (  at your option ) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

    */

#include <artsflow.h>
#include <flowsystem.h>
#include <stdsynthmodule.h>
#include <debug.h>
#include <artsmoduleseffects.h>
#include <connect.h>

#include <kglobal.h>
#include <klocale.h>

namespace Arts {

class FiveBandMonoComplexEQ_impl : virtual public FiveBandMonoComplexEQ_skel,
                                   virtual public StdSynthModule
{
private:
	Arts::StereoToMono _s2m;
	Arts::MonoToStereo _m2s;
	Arts::Synth_STD_EQUALIZER _low, _mid1, _mid2, _mid3, _high;
public:
	FiveBandMonoComplexEQ_impl() {}

	Arts::StereoToMono s2m() { return _s2m; }
	Arts::MonoToStereo m2s() { return _m2s; }

	float lowfreq() { return _low.frequency(); }
	void lowfreq( float n ) { _low.frequency( n ); };
	float lowq() { return _low.q(); }
	void lowq( float n ) { _low.q( n ); };
	float lowgain() { return _low.low(); }
	void lowgain( float n ) { _low.low( n ); };

	float mid1freq() { return _mid1.frequency(); }
	void mid1freq( float n ) { _mid1.frequency( n ); };
	float mid1q() { return _mid1.q(); }
	void mid1q( float n ) { _mid1.q( n ); };
	float mid1gain() { return _mid1.mid(); }
	void mid1gain( float n ) { _mid1.mid( n ); };

	float mid2freq() { return _mid2.frequency(); }
	void mid2freq( float n ) { _mid2.frequency( n ); };
	float mid2q() { return _mid2.q(); }
	void mid2q( float n ) { _mid2.q( n ); };
	float mid2gain() { return _mid2.mid(); }
	void mid2gain( float n ) { _mid2.mid( n ); };

	float mid3freq() { return _mid3.frequency(); }
	void mid3freq( float n ) { _mid3.frequency( n ); };
	float mid3q() { return _mid3.q(); }
	void mid3q( float n ) { _mid3.q( n ); };
	float mid3gain() { return _mid3.mid(); }
	void mid3gain( float n ) { _mid3.mid( n ); };

	float highfreq() { return _high.frequency(); }
	void highfreq( float n ) { _high.frequency( n ); };
	float highq() { return _high.q(); }
	void highq( float n ) { _high.q( n ); };
	float highgain() { return _high.high(); }
	void highgain( float n ) { _high.high( n ); };

	void streamInit()
	{
		_s2m.start(); _low.start(); _mid1.start(); _mid2.start(); _mid3.start(); _high.start(); _m2s.start();

		_node()->virtualize( "inleft", _s2m._node(), "inleft" );
		_node()->virtualize( "inright", _s2m._node(), "inright" );
		connect( _s2m, "outmono", _low, "invalue" );
		connect( _low, "outvalue", _mid1, "invalue" );
		connect( _mid1, "outvalue", _mid2, "invalue" );
		connect( _mid2, "outvalue", _mid3, "invalue" );
		connect( _mid3, "outvalue", _high, "invalue" );
		connect( _high, "outvalue", _m2s, "inmono" );
		_node()->virtualize( "outleft", _m2s._node(), "outleft" );
		_node()->virtualize( "outright", _m2s._node(), "outright" );
	}
};
REGISTER_IMPLEMENTATION( FiveBandMonoComplexEQ_impl );

class FiveBandMonoComplexEQGuiFactory_impl : virtual public FiveBandMonoComplexEQGuiFactory_skel
{
public:
	Arts::Widget createGui( Arts::Object object )
	{
		KGlobal::locale()->insertCatalogue( "artsmodules" );

		arts_return_val_if_fail(  !object.isNull(), Arts::Widget::null() );
		FiveBandMonoComplexEQ ch = DynamicCast(  object );
		arts_return_val_if_fail(  !ch.isNull(), Arts::Widget::null() );

		Arts::LayoutBox hbox;
		hbox.direction( Arts::LeftToRight );
		hbox.layoutmargin( 5 ); hbox.spacing( 5 );

		Arts::Poti lowgain; lowgain.caption( i18n( "Low Gain" ).utf8().data() );
		lowgain.min( -24 ); lowgain.max( 24 );
		lowgain.value( ch.lowgain() ); connect( lowgain, "value_changed", ch, "lowgain" );
		hbox.addWidget( lowgain );
		PopupBox low;
		low.height( 100 ); low.direction( LeftToRight );
		hbox.addWidget( low );
		Arts::VBox lowbox; low.widget( lowbox );
		Arts::Poti lowfreq; lowfreq.color( "grey" ); lowfreq.caption( i18n( "Low Freq" ).utf8().data() );
		lowfreq.min( 20 ); lowfreq.max( 1000 );
		lowfreq.value( ch.lowfreq() ); connect( lowfreq, "value_changed", ch, "lowfreq" );
		lowfreq.parent( lowbox ); lowbox._addChild( lowfreq , "" );
		Arts::Poti lowq; lowq.color( "grey" ); lowq.caption( i18n( "Low Q" ).utf8().data() );
		lowq.min( 0.01 ); lowq.max( 10 );
		lowq.value( ch.lowq() ); connect( lowq, "value_changed", ch, "lowq" );
		lowq.parent( lowbox ); lowbox._addChild( lowq , "" );

		Arts::Poti mid1gain; mid1gain.caption( i18n( "Mid1 Gain" ).utf8().data() );
		mid1gain.min( -24 ); mid1gain.max( 24 );
		mid1gain.value( ch.mid1gain() ); connect( mid1gain, "value_changed", ch, "mid1gain" );
		hbox.addWidget( mid1gain );
		PopupBox mid1;
		mid1.height( 100 ); mid1.direction( LeftToRight );
		hbox.addWidget( mid1 );
		Arts::VBox mid1box; mid1.widget( mid1box );
		Arts::Poti mid1freq; mid1freq.color( "grey" ); mid1freq.caption( i18n( "Mid1 Freq" ).utf8().data() );
		mid1freq.min( 20 ); mid1freq.max( 5000 );
		mid1freq.value( ch.mid1freq() ); connect( mid1freq, "value_changed", ch, "mid1freq" );
		mid1freq.parent( mid1box ); mid1box._addChild( mid1freq , "" );
		Arts::Poti mid1q; mid1q.color(  "grey" ); mid1q.caption( i18n( "Mid1 Q" ).utf8().data() );
		mid1q.min( 0.01 ); mid1q.max( 10 );
		mid1q.value( ch.mid1q() ); connect( mid1q, "value_changed", ch, "mid1q" );
		mid1q.parent( mid1box ); mid1box._addChild( mid1q , "" );

		Arts::Poti mid2gain; mid2gain.caption( i18n( "Mid2 Gain" ).utf8().data() );
		mid2gain.min( -24 ); mid2gain.max( 24 );
		mid2gain.value( ch.mid2gain() ); connect( mid2gain, "value_changed", ch, "mid2gain" );
		hbox.addWidget( mid2gain );
		PopupBox mid2;
		mid2.height( 100 ); mid2.direction( LeftToRight );
		hbox.addWidget( mid2 );
		Arts::VBox mid2box; mid2.widget( mid2box );
		Arts::Poti mid2freq; mid2freq.color( "grey" ); mid2freq.caption( i18n( "Mid2 Freq" ).utf8().data() );
		mid2freq.min( 20 ); mid2freq.max( 10000 );
		mid2freq.value( ch.mid2freq() ); connect( mid2freq, "value_changed", ch, "mid2freq" );
		mid2freq.parent( mid2box ); mid2box._addChild( mid2freq , "" );
		Arts::Poti mid2q; mid2q.color( "grey" ); mid2q.caption( i18n( "Mid2 Q" ).utf8().data() );
		mid2q.min( 0.01 ); mid2q.max( 10 );
		mid2q.value( ch.mid2q() ); connect( mid2q, "value_changed", ch, "mid2q" );
		mid2q.parent( mid2box ); mid2box._addChild( mid2q , "" );

		Arts::Poti mid3gain; mid3gain.caption( i18n( "Mid3 Gain" ).utf8().data() );
		mid3gain.min( -24 ); mid3gain.max( 24 );
		mid3gain.value( ch.mid3gain() ); connect( mid3gain, "value_changed", ch, "mid3gain" );
		hbox.addWidget( mid3gain );
		PopupBox mid3;
		mid3.height( 100 ); mid3.direction( LeftToRight );
		hbox.addWidget( mid3 );
		Arts::VBox mid3box; mid3.widget( mid3box );
		Arts::Poti mid3freq; mid3freq.color( "grey" ); mid3freq.caption( i18n( "Mid3 Freq" ).utf8().data() );
		mid3freq.min( 1000 ); mid3freq.max( 10000 );
		mid3freq.value( ch.mid3freq() ); connect( mid3freq, "value_changed", ch, "mid3freq" );
		mid3freq.parent( mid3box ); mid3box._addChild( mid3freq , "" );
		Arts::Poti mid3q; mid3q.color( "grey" ); mid3q.caption( i18n( "Mid3 Q" ).utf8().data() );
		mid3q.min( 0.01 ); mid3q.max( 10 );
		mid3q.value( ch.mid3q() ); connect( mid3q, "value_changed", ch, "mid3q" );
		mid3q.parent( mid3box ); mid3box._addChild( mid3q , "" );

		Arts::Poti highgain; highgain.caption( i18n( "High Gain" ).utf8().data() );
		highgain.min( -24 ); highgain.max( 24 );
		highgain.value( ch.highgain() ); connect( highgain, "value_changed", ch, "highgain" );
		hbox.addWidget( highgain );
		PopupBox high;
		high.height( 100 ); high.direction( LeftToRight );
		hbox.addWidget( high );
		Arts::VBox highbox; high.widget( highbox );
		Arts::Poti highfreq; highfreq.color( "grey" ); highfreq.caption( i18n( "High Freq" ).utf8().data() );
		highfreq.min( 5000 ); highfreq.max( 16000 );
		highfreq.value( ch.highfreq() ); connect( highfreq, "value_changed", ch, "highfreq" );
		highfreq.parent( highbox ); highbox._addChild( highfreq , "" );
		Arts::Poti highq; highq.color( "grey" ); highq.caption( i18n( "High Q" ).utf8().data() );
		highq.min( 0.01 ); highq.max( 10 );
		highq.value( ch.highq() ); connect( highq, "value_changed", ch, "highq" );
		highq.parent( highbox ); highbox._addChild( highq , "" );

		hbox.addStretch( 100 );

		return hbox;
	}
};
REGISTER_IMPLEMENTATION( FiveBandMonoComplexEQGuiFactory_impl );

}

