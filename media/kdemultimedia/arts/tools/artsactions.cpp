/*

    Copyright (C) 2003 Arnold Krille <arnold@arnoldarts.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    */

#include "artsactions.h"

#include <kaction.h>
#include <kactioncollection.h>
#include <kpopupmenu.h>
#include <klocale.h>
#include <kglobal.h>
//#include <kdebug.h>

#include <kartsserver.h>

#include "fftscopeview.h"
#include "audiomanager.h"
#include "statusview.h"
#include "midimanagerview.h"
#include "environmentview.h"
#include "mediatypesview.h"

ArtsActions::ArtsActions( KArtsServer* server, KActionCollection* col, QWidget* parent, const char* name )
  : QObject( parent,name )
  , _kartsserver( server )
  , _actioncollection( col )
  , _a_sv( 0 ), _a_am( 0 ), _a_asv( 0 ), _a_mmv( 0 ), _a_ev( 0 ), _a_mtv( 0 )
//  , _a_morebars( 0 ), _a_lessbars( 0 )
  , _a_style_normal( 0 ), _a_style_fire( 0 ), _a_style_line( 0 ), _a_style_led( 0 ), _a_style_analog( 0 ), _a_style_small( 0 )
  , _stylemenu( 0 )
  , _sv( 0 ), _am( 0 ), _asv( 0 ), _mmv( 0 ), _ev( 0 ), _mtv( 0 )
{
	//kdDebug()<<k_funcinfo<<endl;
	KGlobal::locale()->insertCatalogue( "artscontrol" );
	if ( !_kartsserver ) _kartsserver = new KArtsServer( this );
}

ArtsActions::~ArtsActions() {
	//kdDebug()<<k_funcinfo<<endl;
	if ( _sv ) viewScopeView();
	if ( _am ) viewAudioManager();
	if ( _asv ) viewArtsStatusView();
	if ( _mmv ) viewMidiManagerView();
	if ( _ev ) viewEnvironmentView();
	if ( _mtv ) viewMediaTypesView();
}

KAction* ArtsActions::actionScopeView() {
	if ( !_a_sv ) _a_sv = new KAction( i18n( "&FFT Scope" ), "artsfftscope", KShortcut(), this, SLOT( viewScopeView() ), _actioncollection, "artssupport_view_scopeview" );
	return _a_sv;
}
KAction* ArtsActions::actionAudioManager() {
	if ( !_a_am ) _a_am = new KAction( i18n( "&Audio Manager" ), "artsaudiomanager", KShortcut(), this, SLOT( viewAudioManager() ), _actioncollection, "artssupport_view_audiomanager" );
	return _a_am;
}
KAction* ArtsActions::actionArtsStatusView() {
	if ( !_a_asv ) _a_asv = new KAction( i18n( "aRts &Status" ), "artscontrol", KShortcut(), this, SLOT( viewArtsStatusView() ), _actioncollection, "artssupport_view_artsstatus" );
	return _a_asv;
}
KAction* ArtsActions::actionMidiManagerView() {
	if ( !_a_mmv ) _a_mmv = new KAction( i18n( "&MIDI Manager" ), "artsmidimanager", KShortcut(), this, SLOT( viewMidiManagerView() ), _actioncollection, "artssupport_view_midimanager" );
	return _a_mmv;
}
KAction* ArtsActions::actionEnvironmentView() {
	if ( !_a_ev ) _a_ev = new KAction( i18n( "&Environment" ), "artsenvironment", KShortcut(), this, SLOT( viewEnvironmentView() ), _actioncollection, "artssupport_view_environment" );
	return _a_ev;
}
KAction* ArtsActions::actionMediaTypesView() {
	if ( !_a_mtv ) _a_mtv = new KAction( i18n( "Available Media &Types" ), "artsmediatypes", KShortcut(), this, SLOT( viewMediaTypesView() ), _actioncollection, "artssupport_view_mediatypes" );
	return _a_mtv;
}

KAction* ArtsActions::actionStyleNormal() {
	if ( !_a_style_normal ) _a_style_normal = new KAction( i18n( "Style: NormalBars" ), "", KShortcut(), this, SLOT( _p_style_normal() ), _actioncollection, "artssupport_style_normal" );
	return _a_style_normal;
}
KAction* ArtsActions::actionStyleFire() {
	if ( !_a_style_fire ) _a_style_fire = new KAction( i18n( "Style: FireBars" ), "", KShortcut(), this, SLOT( _p_style_fire() ), _actioncollection, "artssupport_style_fire" );
	return _a_style_fire;
}
KAction* ArtsActions::actionStyleLine() {
	if ( !_a_style_line ) _a_style_line = new KAction( i18n( "Style: LineBars" ), "", KShortcut(), this, SLOT( _p_style_line() ), _actioncollection, "artssupport_style_line" );
	return _a_style_line;
}
KAction* ArtsActions::actionStyleLED() {
	if ( !_a_style_led ) _a_style_led = new KAction( i18n( "Style: LEDs" ), "", KShortcut(), this, SLOT( _p_style_led() ), _actioncollection, "artssupport_style_led" );
	return _a_style_led;
}
KAction* ArtsActions::actionStyleAnalog() {
	if ( !_a_style_analog ) _a_style_analog = new KAction( i18n( "Style: Analog" ), "", KShortcut(), this, SLOT( _p_style_analog() ), _actioncollection, "artssupport_style_analog" );
	return _a_style_analog;
}
KAction* ArtsActions::actionStyleSmall() {
	if ( !_a_style_small ) _a_style_small = new KAction( i18n( "Style: Small" ), "", KShortcut(), this, SLOT( _p_style_small() ), _actioncollection, "artssupport_style_small" );
	return _a_style_small;
}
KPopupMenu* ArtsActions::stylemenu() {
	if ( !_stylemenu ) {
		_stylemenu = new KPopupMenu();
		KAction *tmp;
		tmp = actionStyleNormal(); tmp->plug( _stylemenu );
		tmp = actionStyleFire(); tmp->plug( _stylemenu );
		tmp = actionStyleLine(); tmp->plug( _stylemenu );
		tmp = actionStyleLED(); tmp->plug( _stylemenu );
		tmp = actionStyleAnalog(); tmp->plug( _stylemenu );
		tmp = actionStyleSmall(); tmp->plug( _stylemenu );
	}
	return _stylemenu;
}

KAction* ArtsActions::actionMoreBars( const QObject* receiver, const char* slot, KActionCollection *actioncollection ) {
	static KAction *_a_morebars = new KAction( i18n( "More Bars in VU-Meters" ), "up", KShortcut(), receiver, slot, actioncollection, "artssupport_morebars" );
	return _a_morebars;
}
KAction* ArtsActions::actionLessBars( const QObject* receiver, const char* slot, KActionCollection *actioncollection ) {
	static KAction *_a_lessbars = new KAction( i18n( "Less Bars in VU-Meters" ), "down", KShortcut(), receiver, slot, actioncollection, "artssupport_lessbars" );
	return _a_lessbars;
}

void ArtsActions::viewScopeView() {
	if ( !_sv ) {
		_sv = new FFTScopeView( _kartsserver->server() );
		connect( _sv, SIGNAL( closed() ), this, SLOT( viewScopeView() ) );
	} else {
		delete _sv;
		_sv = 0;
	}
}
void ArtsActions::viewAudioManager() {
	if ( !_am ) {
		_am = new Gui_AUDIO_MANAGER();
		connect( _am, SIGNAL( closed() ), this, SLOT( viewAudioManager() ) );
	} else {
		delete _am;
		_am = 0;
	}
}
void ArtsActions::viewArtsStatusView() {
	if ( !_asv ) {
		_asv = new ArtsStatusView( _kartsserver->server() );
		connect( _asv, SIGNAL( closed() ), this, SLOT( viewArtsStatusView() ) );
	} else {
		delete _asv;
		_asv = 0;
	}
}
void ArtsActions::viewMidiManagerView() {
	if ( !_mmv ) {
		_mmv = new MidiManagerView();
		connect( _mmv, SIGNAL( closed() ), this, SLOT( viewMidiManagerView() ) );
	} else {
		delete _mmv;
		_mmv = 0;
	}
}
void ArtsActions::viewEnvironmentView() {
	if ( !_ev ) {
		_ev = new EnvironmentView( defaultEnvironment() );
		connect( _ev, SIGNAL( closed() ), this, SLOT( viewEnvironmentView() ) );
	} else {
		delete _ev;
		_ev = 0;
	}
}
void ArtsActions::viewMediaTypesView() {
	if ( !_mtv ) {
		_mtv = new MediaTypesView();
		connect( _mtv, SIGNAL( closed() ), this, SLOT( viewMediaTypesView() ) );
	} else {
		delete _mtv;
		_mtv = 0;
	}
}

#include "artsactions.moc"
