#include <vector>
#include "artsmodulescommon.h"
#include "debug.h"
#include "env_item_impl.h"
#include "connect.h"
#include "../runtime/sequenceutils.h"

namespace Arts {
namespace Environment {

class InstrumentItem_impl : virtual public InstrumentItem_skel,
							virtual public Item_impl
{
protected:
	Synth_MIDI_TEST instrument;
	bool running;

public:
	InstrumentItem_impl() : running(false)
	{
	}
	~InstrumentItem_impl()
	{
		/* this will allow freeing the instrument */
		if(running)
			instrument.stop();
	}
	void filename(const std::string& newFilename)
	{
		if(newFilename != instrument.filename())
		{
			instrument.filename(newFilename);
			filename_changed(newFilename);

			if(!running) {
				instrument.start();
				running = true;
			}
		}
	}
	std::string filename()
	{
		return instrument.filename();
	}
	void busname(const std::string& newBusname)
	{
		if(newBusname != instrument.busname())
		{
			instrument.busname(newBusname);
			busname_changed(newBusname);
		}
	}
	std::string busname()
	{
		return instrument.busname();
	}
	MidiPort port()
	{
		return instrument;
	}
	void loadFromList(const std::vector<std::string>& list)
	{
		unsigned long i;
		std::string cmd,param;
		for(i=0;i<list.size();i++)
		{
			if(parse_line(list[i],cmd,param))	// otherwise: empty or comment
			{
				if(cmd == "filename") {
					filename(param.c_str());
				}
			}
		}
	}
	std::vector<std::string> *saveToList()
	{
		std::vector<std::string> *result = new std::vector<std::string>;
		sqprintf(result,"filename=%s",filename().c_str());
		return result;
	}
};
REGISTER_IMPLEMENTATION(InstrumentItem_impl);

class InstrumentItemGuiFactory_impl
	: virtual public InstrumentItemGuiFactory_skel
{
public:
	Widget createGui(Object object)
	{
		arts_return_val_if_fail(!object.isNull(), Arts::Widget::null());

		InstrumentItem instrument = DynamicCast(object);
		arts_return_val_if_fail(!instrument.isNull(), Arts::Widget::null());

		Widget panel;
		panel.width(150); panel.height(60); panel.show();

		LineEdit edit;
		edit.x(20); edit.y(10); edit.width(120); edit.height(40);
		edit.text(instrument.filename());
		edit.parent(panel);
		edit.show();
		connect(edit,"text_changed", instrument, "filename");
		panel._addChild(edit,"editWidget");

		return panel;
	}
};

REGISTER_IMPLEMENTATION(InstrumentItemGuiFactory_impl);

}
}
