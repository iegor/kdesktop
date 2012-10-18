/*  This file is part of the KDE project
    Copyright (C) 2002-2003 Matthias Kretz <kretz@kde.org>
                  2002 Arnold Krille <arnold@arnoldarts.de>

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
// $Id: env_effectrackitem_impl.cc 244022 2003-08-22 15:37:22Z mkretz $

#include "artsmodulescommon.h"
#include <debug.h>
#include "env_item_impl.h"
#include <connect.h>
#include <stdio.h>
#include <vector>
#include <map>


// Wether you are able to edit the name of the effectrack with an ugly LineInput or not.
//#define EFFECTRACK_NAME
// We should implement something like a ConfigWidget or at least a KLineInputBox or something...

namespace Arts {
namespace Environment {

class EffectRackItem_impl : virtual public EffectRackItem_skel,
                            virtual public Item_impl
{
protected:
	std::string _name;
	AudioManagerClient _amClient;

	struct RackWiring {
		RackWiring( const std::string & type, AudioManagerClient _amClient )
			: routedtomaster( false )
			, amClient( _amClient )
		{
			effect = SubClass( type );

			connect( input, effect );
			connect( effect, output );
		}

		inline void setName( const std::string & efname )
		{
			name = efname;
			input.busname( efname );
			if( ! routedtomaster )
			{
				output.title( efname );
				output.autoRestoreID( efname );
			}
		}

		inline void start()
		{
			input.start();
			effect.start();
			output.start();
		}

		inline void stop()
		{
			input.stop();
			effect.stop();
			output.stop();
		}

		inline void master( bool tomaster )
		{
			routedtomaster = tomaster;

			output.stop();
			output = tomaster ? Synth_AMAN_PLAY( amClient ) : Synth_AMAN_PLAY();
			connect( effect, output );
			if( ! tomaster )
			{
				output.title( name );
				output.autoRestoreID( name );
			}
			output.start();
		}

		bool routedtomaster;
		std::string name;
		std::string effectName;
		Synth_BUS_DOWNLINK input;
		Arts::StereoEffect effect;
		Synth_AMAN_PLAY output;
		AudioManagerClient amClient;
	};
	std::vector<RackWiring> _wirings;

public:
	EffectRackItem_impl()
		: _name( "effect rack" )
		, _amClient( amPlay, _name + " Master", "effectrack_" + _name )
	{
		// TODO: check if there's another effect rack with the same name already - if so prefix with 2./3./4.
	}

	// readonly attribute sequence<Arts::StereoEffect> effects;
	std::vector<Arts::StereoEffect> *effects()
	{
		std::vector<Arts::StereoEffect> * effects = new std::vector<Arts::StereoEffect>;
		for( std::vector<RackWiring>::iterator it = _wirings.begin(); it != _wirings.end(); ++it )
			effects->push_back( it->effect );
		return effects;
	}

	// attribute long effectCount;
	long effectCount() { return _wirings.size(); }

	// attribute string name;
	void name(const std::string& newName) {
		if(newName != _name)
		{
			_name = newName;
			_amClient.title( _name + " Master" );
			_amClient.autoRestoreID( "effectrack_" + _name );
			for( unsigned int i = 0; i < _wirings.size(); i++ )
				_wirings[i].setName( effectName( i, _wirings[ i ].effectName ) );
			name_changed( newName );
		}
	}
	std::string name() { return _name; }

	void loadFromList(const std::vector<std::string>& /*list*/)
	{
	}

	std::vector<std::string> *saveToList()
	{
		std::vector<std::string> *result = new std::vector<std::string>;
		return result;
	}

	std::string effectName( int n, const std::string & en )
	{
		char * efname = new char[ _name.length() + en.length() + 128 ];
		sprintf( efname, "%s%02d (%s)", _name.c_str(), n, en.c_str() );
		return efname;
	}

	Arts::StereoEffect createEffect( const std::string & type, const std::string & name )
	{
		RackWiring wiring( type, _amClient );
		wiring.setName( effectName( _wirings.size() + 1, name ) );
		wiring.start();
		_wirings.push_back( wiring );
		return wiring.effect;
	}

	void delEffect( long pos )
	{
		_wirings[ pos ].stop();
		_wirings.erase( _wirings.begin() + pos );
		for( unsigned int i = pos; i < _wirings.size(); ++i )
			_wirings[ i ].setName( effectName( i, _wirings[ i ].effectName ) );
	}

	void routeToMaster( long pos, bool tomaster )
	{
		_wirings[ pos ].master( tomaster );
	}
};
REGISTER_IMPLEMENTATION(EffectRackItem_impl);
}

using namespace Environment;

typedef WeakReference<VBox> VBox_wref;

class EffectRackItemGui_impl : virtual public EffectRackItemGui_skel {
private:
	bool _active;
	long _effectCount;
	std::string _type;
	EffectRackItem _effectRack;

	/* widgets */
	VBox_wref _widget;
	HBox hbox;
	VBox effect_vbox;
#ifdef EFFECTRACK_NAME
	LineEdit name;
#endif
	ComboBox typebox;
	Button addbutton;
	GenericGuiFactory guiFactory;
	std::vector<EffectRackSlot> _slots;
	std::map<std::string, std::string> typeforname;
	std::map<std::string, std::string> namefortype;

public:
	EffectRackItemGui self() { return EffectRackItemGui::_from_base(_copy()); }

	void redoGui()
	{
		VBox vbox = _widget;
		if(vbox.isNull())
			arts_warning("update with vbox null");
		if(_effectRack.isNull())
			arts_warning("update with _effectRack null");
		if(!_effectRack.isNull() && !vbox.isNull())
		{
			vbox.spacing( 0 );
			vbox.margin( 10 );
			hbox = HBox( vbox );
			hbox.spacing( 5 );
			hbox.margin( 0 );

#ifdef EFFECTRACK_NAME
			name = LineEdit();
			name.caption("name");
			name.text(_effectRack.name());
			name.parent(hbox);
			connect(name,"text_changed", _effectRack, "name");
#endif

			std::vector<std::string> choices;
			TraderQuery query;
			query.supports( "Interface", "Arts::StereoEffect" );
			query.supports( "Features", "RackGUI" );
			std::vector<TraderOffer> *queryResults = query.query();
			for(std::vector<TraderOffer>::iterator it = queryResults->begin(); it != queryResults->end(); ++it)
			{
				std::vector<std::string> * names = it->getProperty( "Name" );
				std::string name = names->empty() ? it->interfaceName() : names->front();
				delete names;
				choices.push_back( name );
				typeforname[ name ] = it->interfaceName();
				namefortype[ it->interfaceName() ] = name;
			}
			delete queryResults;
			typebox = ComboBox();
			typebox.choices(choices);
			typebox.value(_type);
			typebox.parent(hbox);
			connect(typebox,"value_changed", self(), "type");

			addbutton = Button( "add", hbox );
			connect( addbutton, "clicked_changed", self(), "addeffect" );

			effect_vbox = VBox( vbox );
			effect_vbox.margin( 0 );
			effect_vbox.spacing( 5 );
			effect_vbox.show();

			Frame spacer;
			spacer.parent( effect_vbox );
			spacer.vSizePolicy( spExpanding );
			spacer.show();
			effect_vbox._addChild( spacer, "spacer" );

			_slots.clear();

			// add Arts::StereoEffect widgets
			std::vector<Arts::StereoEffect> * effects = _effectRack.effects();
			for( std::vector<Arts::StereoEffect>::iterator it = effects->begin(); it != effects->end(); ++it )
				createEffectGui( *it );
			delete effects;
		}
		else
		{
			/* FIXME: maybe insert a "dead" label here */
			if(!vbox.isNull())
				vbox.show();
			effect_vbox = VBox::null();
			hbox = HBox::null();
			// name = LineEdit::null();
			typebox = ComboBox::null();
			_slots.clear();
		}
	}

	void createEffectGui( Arts::StereoEffect effect )
	{
		Widget w = guiFactory.createGui( effect );
		if( ! w.isNull() )
		{
			// insert effect GUI into the "Rack"
			EffectRackSlot slot( effect_vbox, w, self() );
			_slots.push_back( slot );
		}
	}

	void removeSlot( EffectRackSlot slot )
	{
		unsigned int i;
		for( i = 0; i < _slots.size() && ! _slots[ i ]._isEqual( slot ) ; ++i );
		if( i < _slots.size() )
		{
			_slots.erase( _slots.begin() + i );
			_effectRack.delEffect( i );
		}
		else
			arts_warning( "WARNING: Trying to remove an unknown slot" );
	}

	void routeToMaster( EffectRackSlot slot, bool tomaster )
	{
		unsigned int i;
		for( i = 0; i < _slots.size() && ! _slots[ i ]._isEqual( slot ) ; ++i );
		if( i < _slots.size() )
			_effectRack.routeToMaster( i, tomaster );
		else
			arts_warning( "WARNING: Trying to route an unknown slot" );
	}

	bool active() { return _active; }
	void active(bool newActive)
	{
		if(newActive != _active)
		{
			_active = newActive;
			if(!newActive)
				_effectRack = EffectRackItem::null();
			redoGui();
		}
	}

	std::string type()
	{
		return _type;
	}
	void type(const std::string& t)
	{
		_type = typeforname[ t ];
	}

	bool addeffect() { return false; } //unused
	void addeffect( bool clicked )
	{
		if( ! addbutton.clicked() || ! clicked )
			return;

		Arts::StereoEffect effect = _effectRack.createEffect( _type, namefortype[ _type ] );
		createEffectGui( effect );
	}

	Widget initialize(EffectRackItem item)
	{
		VBox vbox;
		vbox._addChild(self(),"the_gui_updating_widget");

		_widget = vbox;
		_effectRack = item;
		_active = item.active();
		_type = "Arts::Synth_VOICE_REMOVAL";
		_effectCount = item.effectCount();

		if(!_effectRack.isNull())
		{
			connect(_effectRack, "active_changed", self(), "active");
		}
		redoGui();

		return vbox;
	}
};

REGISTER_IMPLEMENTATION(EffectRackItemGui_impl);

class EffectRackGuiFactory_impl : virtual public EffectRackGuiFactory_skel
{
public:
	Widget createGui(Object object) 
	{
		arts_return_val_if_fail(!object.isNull(), Arts::Widget::null());

		std::string iface = object._interfaceName();
		arts_return_val_if_fail(iface == "Arts::Environment::EffectRackItem",
				Arts::Widget::null());
		if(iface == "Arts::Environment::EffectRackItem")
		{
			EffectRackItem effectRack = DynamicCast(object);
			arts_return_val_if_fail(!effectRack.isNull(), Arts::Widget::null());

			EffectRackItemGui gui;
			return gui.initialize(effectRack);
		}
		return Arts::Widget::null();
	}
};
REGISTER_IMPLEMENTATION(EffectRackGuiFactory_impl);
}
// vim:ts=4:sw=4
