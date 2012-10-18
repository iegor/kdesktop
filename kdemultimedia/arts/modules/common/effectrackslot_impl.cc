/*  This file is part of the KDE project
    Copyright (C) 2002 Matthias Kretz <kretz@kde.org>

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
// $Id: effectrackslot_impl.cc 239435 2003-07-27 13:50:21Z mueller $

#include "artsmodulescommon.h"
#include <artsgui.h>
#include <debug.h>

namespace Arts {
class EffectRackSlot_impl : virtual public EffectRackSlot_skel
{
	private:
		HBox _hbox;
		VBox _buttonbox; // Buttons
		Button _removeButton;
		Button _masterButton;
		HBox _frame;
		Widget _effect;
		Frame _spacer;
		EffectRackItemGui _effectrackgui; //XXX: need a WeakReference here?

		EffectRackSlot self() { return EffectRackSlot::_from_base( _copy() ); }

	public:
		EffectRackSlot_impl()
		{
		}

		void constructor( Widget parent, Widget effect, EffectRackItemGui effectrackgui )
		{
			_effectrackgui = effectrackgui;

			_hbox.parent( parent );
			_hbox.margin( 0 );
			_hbox.spacing( 0 );
			_hbox.framestyle( Sunken | Panel );
			_hbox.linewidth( 1 );
			_hbox.show();

			_buttonbox.parent( _hbox );
			_buttonbox.margin( 0 );
			_buttonbox.spacing( 0 );
			_buttonbox.show();

			_removeButton.parent( _buttonbox );
			_removeButton.text( "x" );
			_removeButton.hSizePolicy( spFixed );
			_removeButton.width( 20 );
			_removeButton.height( 20 );
			connect( _removeButton, "clicked_changed", self(), "removeslot" );
			_removeButton.show();

			_masterButton.parent( _buttonbox );
			_masterButton.text( "MM" );
			_masterButton.toggle( true );
			_masterButton.hSizePolicy( spFixed );
			_masterButton.width( 20 );
			_masterButton.height( 20 );
			connect( _masterButton, "pressed_changed", self(), "tomaster" );
			_masterButton.show();

			_frame.parent( _hbox );
			_frame.margin( 5 );
			_frame.spacing( 0 );
			_frame.framestyle( Raised | Panel );
			_frame.linewidth( 2 );
			_frame.midlinewidth( 2 );
			_frame.hSizePolicy( spExpanding );
			_frame.show();

			_effect = effect;
			_effect.parent( _frame );
			_effect.show();

			_spacer.parent( _frame );
			_spacer.hSizePolicy( spExpanding );
			_spacer.show();
		}

		bool removeslot() { return false; } //unused
		void removeslot( bool clicked )
		{
			if( ! _removeButton.clicked() || ! clicked )
				return;

			// I need to be removed...
			_effectrackgui.removeSlot( self() );
			// I should be deleted by now
		}

		bool tomaster() { return false; } //unused
		void tomaster( bool toggled )
		{
			_effectrackgui.routeToMaster( self(), toggled );
		}
};
REGISTER_IMPLEMENTATION( EffectRackSlot_impl );
}

// vim: sw=4 ts=4
