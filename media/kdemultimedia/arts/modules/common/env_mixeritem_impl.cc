#include "artsmodulescommon.h"
#include "debug.h"
#include "env_item_impl.h"
#include "connect.h"
#include "../runtime/sequenceutils.h"
#include <stdio.h>
#include <kglobal.h>
#include <klocale.h>

#include <vector>

namespace Arts {
namespace Environment {

class MixerItem_impl : virtual public MixerItem_skel,
					   virtual public Item_impl
{
protected:
	std::vector<Synth_BUS_DOWNLINK> _inputs;
	std::vector<MixerChannel> _channels;
	std::vector<Synth_AMAN_PLAY> _outputs;
	std::string _name;
	std::string _type;
	AudioManagerClient amClient;

public:
	MixerItem_impl()
		: _name("mixer"), _type("Arts::SimpleMixerChannel"),
		  amClient(amPlay, "Mixer (mixer)","mixer_mixer")
	{
	}
	// readonly attribute sequence<MixerChannel> channels;
	std::vector<MixerChannel> *channels() {
		return new std::vector<MixerChannel>(_channels);
	}
	// attribute long channelCount;
	void channelCount(long newChannelCount)
	{
		if((unsigned long)newChannelCount != _channels.size())
		{
			while(_channels.size() < (unsigned long)newChannelCount) addChannel();
			while(_channels.size() > (unsigned long)newChannelCount) delChannel();
			channelCount_changed(newChannelCount);
		}
	}
	long channelCount() { return _channels.size(); }
	// attribute string name;
	void name(const std::string& newName) {
		if(newName != _name)
		{
			_name = newName;
		  	amClient.title(i18n("Mixer (\"%1\")").arg(QString::fromUtf8(_name.c_str())).utf8().data());
		  	amClient.autoRestoreID("mixer_"+_name);
			for(unsigned int i = 0; i < _inputs.size(); i++)
				_inputs[i].busname(channelName(i));
			name_changed(newName);
		}
	}
	std::string name() { return _name; }
	// attribute string type;
	void type(const std::string& newType) {
		if(newType != _type)
		{
			_type = newType;
			type_changed(newType);
		}
	}
	std::string type() { return _type; }
	void loadFromList(const std::vector<std::string>& /*list*/)
	{
	/*
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
		*/
	}
	std::vector<std::string> *saveToList()
	{
		std::vector<std::string> *result = new std::vector<std::string>;
	/*
		sqprintf(result,"filename=%s",filename().c_str());
		*/
		return result;
	}
	std::string channelName(int n)
	{
		char chname[1024];
		sprintf(chname, "%s%02d", _name.c_str(), n);
		return chname;
	}
	void addChannel()
	{
		Synth_BUS_DOWNLINK input;
		MixerChannel channel = SubClass(_type);
		Synth_AMAN_PLAY output(amClient);

		std::string chname = channelName(_channels.size()+1);
		input.busname(chname);
		channel.name(chname);

		input.start();
		channel.start();
		output.start();

		connect(input, channel);
		connect(channel, output);

		_inputs.push_back(input);
		_channels.push_back(channel);
		_outputs.push_back(output);
	}

	void delChannel()
	{
		unsigned long cc = _channels.size()-1;

		_inputs.resize(cc);
		_channels.resize(cc);
		_outputs.resize(cc);
	}
};
REGISTER_IMPLEMENTATION(MixerItem_impl);
}

using namespace Environment;

typedef WeakReference<VBox> VBox_wref;

class MixerItemGui_impl : virtual public MixerItemGui_skel {
private:
	bool _active;
	long _channelCount;
	std::string _type;
	MixerItem _item;

	/* widgets */
	VBox_wref _widget;
	HBox hbox, channel_hbox;
	SpinBox spinbox;
	LineEdit name;
	ComboBox typebox;
	GenericGuiFactory guiFactory;
	std::vector<Widget> channelWidgets;

public:
	MixerItemGui self() { return MixerItemGui::_from_base(_copy()); }

	void updateChannelGui()
	{
		if(channelWidgets.size() > (unsigned)_item.channelCount())
			channelWidgets.resize(_item.channelCount());
		else
		{
			std::vector<MixerChannel> *channels = _item.channels();
			for(unsigned int i = channelWidgets.size(); i < channels->size(); ++i)
			{
				Widget w = guiFactory.createGui((*channels)[i]);
				if(!w.isNull())
				{
					w.parent(channel_hbox);
					w.show();
					channelWidgets.push_back(w);
				}
			}
		}
	}

	void redoGui()
	{
		VBox vbox = _widget;
		if(vbox.isNull())
			arts_warning("update with vbox null");
		if(_item.isNull())
			arts_warning("update with _item null");
		if(!_item.isNull() && !vbox.isNull())
		{
			hbox = HBox();
			hbox.parent(vbox);
			hbox.show();

			spinbox = SpinBox();
			spinbox.caption(i18n("channels").utf8().data());
			spinbox.min(0); spinbox.max(32);
			spinbox.value(_item.channelCount());
			spinbox.parent(hbox);
			spinbox.show();
			connect(spinbox,"value_changed", _item, "channelCount");

			name = LineEdit();
			name.caption(i18n("name").utf8().data());
			name.text(_item.name());
			name.parent(hbox);
			name.show();
			connect(name,"text_changed", _item, "name");

			typebox = ComboBox();
			typebox.caption(i18n("type").utf8().data());
			std::vector<std::string> choices;
			TraderQuery query;
			query.supports("Interface", "Arts::Environment::MixerChannel");
			std::vector<TraderOffer> *queryResults = query.query();
			for(std::vector<TraderOffer>::iterator it = queryResults->begin(); it != queryResults->end(); ++it)
				choices.push_back(it->interfaceName());
			delete queryResults;
			typebox.choices(choices);
			typebox.value(_type);
			typebox.parent(hbox);
			typebox.show();
			connect(typebox,"value_changed", _item, "type");

			channel_hbox = HBox();
			channel_hbox.parent(vbox);
			channel_hbox.show();

			channelWidgets.clear();
			updateChannelGui();

			vbox.show();
		}
		else
		{
			/* FIXME: maybe insert a "dead" label here */
			if(!vbox.isNull())
				vbox.show();
			channel_hbox = HBox::null();
			hbox = HBox::null();
			spinbox = SpinBox::null();
			name = LineEdit::null();
			typebox = ComboBox::null();
			channelWidgets.clear();
		}
	}

	bool active() { return _active; }
	void active(bool newActive)
	{
		if(newActive != _active)
		{
			_active = newActive;
			if(!newActive)
				_item = MixerItem::null();
			redoGui();
		}
	}
	long channelCount() { return _channelCount; }
	void channelCount(long ch)
	{
		if(_channelCount != ch)
		{
			_channelCount = ch;
			updateChannelGui();
		}
	}
	std::string type() {
		return _type;
	}
	void type(const std::string& t)
	{
		if(_type != t)
		{
			_type = t;
			//redoGui();
		}
	}
	Widget initialize(MixerItem item)
	{
		KGlobal::locale()->insertCatalogue( "artsmodules" );
		VBox vbox;
		vbox._addChild(self(),"the_gui_updating_widget");

		_widget = vbox;
		_item = item;
		_active = item.active();
		_type = item.type();
		_channelCount = item.channelCount();

		if(!_item.isNull())
		{
			connect(_item, "channelCount_changed", self(), "channelCount");
			connect(_item, "type_changed", self(), "type");
			connect(_item, "active_changed", self(), "active");
		}
		redoGui();

		return vbox;
	}
};

REGISTER_IMPLEMENTATION(MixerItemGui_impl);

class MixerGuiFactory_impl : virtual public MixerGuiFactory_skel
{
public:
	Widget createGui(Object object)
	{
		KGlobal::locale()->insertCatalogue( "artsmodules" );
		arts_return_val_if_fail(!object.isNull(), Arts::Widget::null());

		std::string iface = object._interfaceName();
		arts_return_val_if_fail(iface == "Arts::Environment::MixerItem",
												  Arts::Widget::null());
		if(iface == "Arts::Environment::MixerItem")
		{
			MixerItem mixerItem = DynamicCast(object);
			arts_return_val_if_fail(!mixerItem.isNull(), Arts::Widget::null());

#if 0
			VBox vbox;
			vbox.show();
			vbox.width(330); vbox.height(500);

			HBox hbox;
			hbox.show();
			hbox.width(330); hbox.height(50);
			hbox.parent(vbox);
			vbox._addChild(hbox,"hbox");

			SpinBox spinbox;
			spinbox.caption(i18n("channels").utf8().data());
			spinbox.min(0); spinbox.max(32);
			spinbox.value(mixerItem.channelCount());
			spinbox.parent(hbox);
			spinbox.show();
			connect(spinbox,"value_changed", mixerItem, "channelCount");
			hbox._addChild(spinbox,"channelsWidget");

			LineEdit name;
			name.caption(i18n("name").utf8().data());
			name.caption(mixerItem.name());
			name.parent(hbox);
			name.show();
			connect(name,"caption_changed", mixerItem, "name");
			hbox._addChild(name,"nameWidget");

			HBox channel_hbox;
			channel_hbox.show();
			channel_hbox.width(330); hbox.height(450);
			channel_hbox.parent(vbox);
			vbox._addChild(channel_hbox,"channel_hbox");

			GenericGuiFactory gf;

			std::vector<MixerChannel> *channels = mixerItem.channels();
			std::vector<MixerChannel>::iterator i;
			for(i = channels->begin(); i != channels->end(); i++)
			{
				Widget w = gf.createGui(*i);
				w.parent(channel_hbox);
				channel_hbox._addChild(w,"channel");
			}
#endif
			MixerItemGui gui;
			return gui.initialize(mixerItem);
		}
		return Arts::Widget::null();
	}
};
REGISTER_IMPLEMENTATION(MixerGuiFactory_impl);
}
// vim:ts=4:sw=4
