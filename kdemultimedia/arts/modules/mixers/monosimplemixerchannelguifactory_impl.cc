#include "artsmodulesmixers.h"
#include "debug.h"
#include "connect.h"

#include <kglobal.h>
#include <klocale.h>

namespace Arts {

	class MonoSimpleMixerChannelGuiFactory_impl : virtual public MonoSimpleMixerChannelGuiFactory_skel
	{
		public:
			Widget createGui(Object object)
			{
				KGlobal::locale()->insertCatalogue( "artsmodules" );
				arts_return_val_if_fail(!object.isNull(), Arts::Widget::null());
				MonoSimpleMixerChannel ch= DynamicCast(object);
				arts_return_val_if_fail(!ch.isNull(), Arts::Widget::null());

				Arts::LayoutBox vbox;
				vbox.direction( Arts::TopToBottom );

				Poti gain;
				gain.caption(i18n("gain").utf8().data());
				gain.color("red"); gain.min(0.01); gain.max(4);
				gain.value(ch.gain());
				connect(gain,"value_changed", ch, "gain");
				vbox.addWidget( gain );

				Arts::PopupBox eqbox;
				eqbox.name( i18n( "EQ" ).utf8().data() );
				eqbox.direction( Arts::TopToBottom );
				vbox.addWidget( eqbox );

				Arts::LayoutBox eq;
				eq.direction( Arts::TopToBottom );
				eqbox.widget( eq );

				Poti high;
				high.caption(i18n("volume","high").utf8().data());
				high.color("blue"); high.min(-12); high.max(12);
				high.value(ch.equalizer().high());
				connect(high,"value_changed", ch.equalizer(), "high");
				eq.addWidget( high );

				Poti mid;
				mid.caption(i18n("volume","mid").utf8().data());
				mid.color("blue"); mid.min(-12); mid.max(12);
				mid.value(ch.equalizer().mid());
				connect(mid,"value_changed", ch.equalizer(), "mid");
				eq.addWidget( mid );

				Poti low;
				low.caption(i18n("volume","low").utf8().data());
				low.color("blue"); low.min(-12); low.max(12);
				low.value(ch.equalizer().low());
				connect(low,"value_changed", ch.equalizer(), "low");
				eq.addWidget( low );

				Poti frequency;
				frequency.caption(i18n("frequency").utf8().data());
				frequency.color("darkgreen"); frequency.min(20); frequency.max(10000);
				frequency.value(ch.equalizer().frequency());
				frequency.logarithmic(2.0);
				connect(frequency,"value_changed", ch.equalizer(), "frequency");
				eq.addWidget( frequency );

				Poti q;
				q.caption(i18n( "q" ).utf8().data());
				q.color("darkgreen"); q.min(0.01); q.max(10);
				q.value(ch.equalizer().q());
				q.logarithmic(2.0);
				connect(q,"value_changed", ch.equalizer(), "q");
				eq.addWidget( q );

				Poti pan;
				pan.caption(i18n("pan").utf8().data());
				pan.color("grey"); pan.min(-1.0); pan.max(1.0);
				pan.value(ch.pan());
				connect(pan,"value_changed",ch,"pan");
				vbox.addWidget( pan );

				Fader volume;
				volume.caption(i18n("volume").utf8().data());
				volume.color("red"); volume.min(0.01); volume.max(4);
				volume.value(ch.volume());
				connect(volume,"value_changed", ch, "volume");
				vbox.addWidget( volume );

				return vbox;
			}
	};
	REGISTER_IMPLEMENTATION(MonoSimpleMixerChannelGuiFactory_impl);
}

// vim:ts=4:sw=4
