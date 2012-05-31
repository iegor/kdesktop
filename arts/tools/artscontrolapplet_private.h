/***************************************************************************
                          artscontrolapplet_private.h  -  description
                             -------------------
    begin                : Don Jan 30 2003
    copyright            : (C) 2003 by Arnold Krille
    email                : arnold@arnoldarts.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ARTSCONTROLAPPLET_PRIVATE_H
#define ARTSCONTROLAPPLET_PRIVATE_H

#include "artscontrolapplet.h"

#include <qobject.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qslider.h>
#include <qpushbutton.h>
#include <kpopupmenu.h>
#include <kaction.h>
#include <qfont.h>

#include <kartsserver.h>
#include <kartsdispatcher.h>
#include <kartsfloatwatch.h>
#include <kartswidget.h>
#include <artsflow.h>
#include <connect.h>
#include <artsmoduleseffects.h>
#include <kdelibs_export.h>

#include "artsactions.h"

#include "fftscopeview.h"

class VolumeSlider;

class KDE_EXPORT ArtsControlAppletPrivate : public QObject {
   Q_OBJECT
private:
	ArtsControlApplet *_parent;
	ArtsActions* _artsactions;
public:
	QBoxLayout *layout;
	KArtsServer *arts;
	KArtsDispatcher *dispatcher;
	Arts::StereoVolumeControl volume;
	bool barts, bInUpdate;
	FFTScopeView *svinline;

	KArtsWidget *vuw;
	Arts::StereoVolumeControlGui vu;

	KPopupMenu *menu;
	KAction *_showSV, *_showSVinline, *_showAM, *_showArtsStatus, *_showMidiManager, *_showEnvironment, *_showMediaTypes, *_moreBars, *_lessBars;
	KAction *_styleNormalBars, *_styleFireBars, *_styleLineBars, *_styleLEDs, *_styleAnalog, *_styleSmall;

	ArtsControlAppletPrivate( ArtsControlApplet *parent )
	    : QObject(parent)
	    , _parent( parent )
	    , barts( false )
	    , bInUpdate( false )
	{
		arts = new KArtsServer( 0 );
		dispatcher = new KArtsDispatcher( 0 );
		if( ! arts->server().isNull() ) barts = true;
		if( barts ) volume = arts->server().outVolume();
		svinline=0;

		_artsactions = new ArtsActions( arts, 0, parent );

		menu = new KPopupMenu( 0 );
		_showSV = _artsactions->actionScopeView();
		_showSV->plug( menu );
		_showSVinline = new KAction( i18n( "Toggle &Inline FFT Scope" ), "artscontrol", KShortcut(), this, SLOT( SVinline() ), this );
		_showSVinline->plug( menu );
		_showAM = _artsactions->actionAudioManager();
		_showAM->plug( menu );
		_showArtsStatus = _artsactions->actionArtsStatusView();
		_showArtsStatus->plug( menu );
		_showMidiManager = _artsactions->actionMidiManagerView();
		_showMidiManager->plug( menu );
		_showEnvironment = _artsactions->actionEnvironmentView();
		_showEnvironment->plug( menu );
		_showMediaTypes = _artsactions->actionMediaTypesView();
		_showMediaTypes->plug( menu );
		menu->insertSeparator();
		menu->insertItem( i18n( "VU-Style" ), _artsactions->stylemenu() );
		connect( _artsactions, SIGNAL( styleNormal() ), this, SLOT( styleNormalBars() ) );
		connect( _artsactions, SIGNAL( styleFire() ), this, SLOT( styleFireBars() ) );
		connect( _artsactions, SIGNAL( styleLine() ), this, SLOT( styleLineBars() ) );
		connect( _artsactions, SIGNAL( styleLED() ), this, SLOT( styleLEDs() ) );
		connect( _artsactions, SIGNAL( styleAnalog() ), this, SLOT( styleAnalog() ) );
		connect( _artsactions, SIGNAL( styleSmall() ), this, SLOT( styleSmall() ) );
	}
	~ArtsControlAppletPrivate() {
		if ( svinline ) SVinline();
	}
public slots:
	void SVinline(); // ScopeView inline
	void moreBars();
	void lessBars();
	void styleNormalBars();
	void styleFireBars();
	void styleLineBars();
	void styleLEDs();
	void styleAnalog();
	void styleSmall();
};

// vim: sw=4 ts=4
#endif
