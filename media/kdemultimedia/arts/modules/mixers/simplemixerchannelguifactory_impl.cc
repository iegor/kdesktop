#include "artsmodulesmixers.h"
#include "debug.h"
#include "connect.h"

#include <kglobal.h>
#include <klocale.h>

namespace Arts {
	class SimpleMixerChannelGuiFactory_impl : virtual public SimpleMixerChannelGuiFactory_skel {
		public:
			Widget createGui(Object object)
			{
				KGlobal::locale()->insertCatalogue( "artsmodules" );
				arts_return_val_if_fail(!object.isNull(), Arts::Widget::null());
				SimpleMixerChannel ch = DynamicCast(object);
				arts_return_val_if_fail(!ch.isNull(), Arts::Widget::null());

				Arts::LayoutBox vbox;
				vbox.direction( Arts::TopToBottom );

				Poti high;
				high.caption(i18n("volume","high").utf8().data());
				high.color("blue"); high.min(-12); high.max(12);
				high.value(ch.equalizerLeft().high());
				connect(high,"value_changed", ch.equalizerLeft(), "high");
				connect(high,"value_changed", ch.equalizerRight(), "high");
				vbox.addWidget( high );

				Poti mid;
				mid.caption(i18n("volume","mid").utf8().data());
				mid.color("blue"); mid.min(-12); mid.max(12);
				mid.value(ch.equalizerLeft().mid());
				connect(mid,"value_changed", ch.equalizerLeft(), "mid");
				connect(mid,"value_changed", ch.equalizerRight(), "mid");
				vbox.addWidget( mid );

				Poti low;
				low.caption(i18n("volume","low").utf8().data());
				low.color("blue"); low.min(-12); low.max(12);
				low.value(ch.equalizerLeft().low());
				connect(low,"value_changed", ch.equalizerLeft(), "low");
				connect(low,"value_changed", ch.equalizerRight(), "low");
				vbox.addWidget( low );

				Poti frequency;
				frequency.caption(i18n("frequency").utf8().data());
				frequency.color("darkgreen"); frequency.min(200); frequency.max(10000);
				frequency.value(ch.equalizerLeft().frequency());
				frequency.logarithmic(2.0);
				connect(frequency,"value_changed", ch.equalizerLeft(), "frequency");
				connect(frequency,"value_changed", ch.equalizerRight(), "frequency");
				vbox.addWidget( frequency );

				Poti q;
				q.caption(i18n( "q" ).utf8().data());
				q.color("darkgreen"); q.min(0.01); q.max(10);
				q.value(ch.equalizerLeft().q());
				q.logarithmic(2.0);
				connect(q,"value_changed", ch.equalizerLeft(), "q");
				connect(q,"value_changed", ch.equalizerRight(), "q");
				vbox.addWidget( q );

				Poti pan;
				pan.caption(i18n("pan").utf8().data());
				pan.color("grey"); pan.min(-1.0); pan.max(1.0);
				pan.value(ch.pan());
				connect(pan,"value_changed",ch,"pan");
				vbox.addWidget( pan );

				Fader volume;
				volume.caption(i18n("volume").utf8().data());
				volume.color("red"); volume.min(0.01); volume.max(4);
				volume.value(ch.volumeLeft());
				connect(volume,"value_changed", ch, "volumeLeft");
				connect(volume,"value_changed", ch, "volumeRight");
				vbox.addWidget( volume );

				return vbox;
			}
	};
	REGISTER_IMPLEMENTATION(SimpleMixerChannelGuiFactory_impl);
}


// vim:ts=4:sw=4

