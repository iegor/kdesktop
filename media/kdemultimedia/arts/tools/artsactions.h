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

#ifndef ARTS_ACTIONS_H
#define ARTS_ACTIONS_H

#include <qwidget.h>
#include <kdelibs_export.h>
class KArtsServer;
class KAction;
class KActionCollection;
class KPopupMenu;
class FFTScopeView;
class Gui_AUDIO_MANAGER;
class ArtsStatusView;
class MidiManagerView;
class EnvironmentView;
class MediaTypesView;

class KDE_EXPORT ArtsActions : public QObject
{
	Q_OBJECT
public:
	/**
		Constructs a ArtsActions-object.

		Use the KActions you get from it to obtain a unique Style for all menus.

		It also provides an easy way to have FFT-ScopeView, Audiomanager and other things available.

		@param artsserver a pointer to a existing KArtsServer. If 0 a new is created.
		@param actioncollection the KActionCollection all the actions should belong to. Names of the actions are then: artssupport_*
		@param qwidget the parent QWidget
		@param name the name of the object
	*/
	ArtsActions( KArtsServer* artsserver, KActionCollection* actioncollection, QWidget* qwidget, const char* name=0 );
	/** Destructor */
	~ArtsActions();

	/** Returns an Action for showing the ScopeView. Unless otherwise connected it also toggles a ScopeView.  */
	KAction* actionScopeView();
	/** Returns an Action for showing the Audiomanager. Unless otherwise connected it also toggles a Audiomanager.  */
	KAction* actionAudioManager();
	/** Returns an Action for showing the StatusView. Unless otherwise connected it also toggles a StatusView.  */
	KAction* actionArtsStatusView();
	/** Returns an Action for showing the MidiManager. Unless otherwise connected it also toggles a MidiManager.  */
	KAction* actionMidiManagerView();
	/** Returns an Action for showing the EnvironmentView. Unless otherwise connected it also toggles a EnvironmentView.  */
	KAction* actionEnvironmentView();
	/** Returns an Action for showing the MediaTypesView. Unless otherwise connected it also toggles a MediaTypesView.  */
	KAction* actionMediaTypesView();

	KAction* actionStyleNormal();
	KAction* actionStyleFire();
	KAction* actionStyleLine();
	KAction* actionStyleLED();
	KAction* actionStyleAnalog();
	KAction* actionStyleSmall();
	KPopupMenu* stylemenu();

	static KAction* actionMoreBars( const QObject*, const char*, KActionCollection* );
	static KAction* actionLessBars( const QObject*, const char*, KActionCollection* );

public slots:
	void viewScopeView();
	void viewAudioManager();
	void viewArtsStatusView();
	void viewMidiManagerView();
	void viewEnvironmentView();
	void viewMediaTypesView();
private slots:
	void _p_style_normal() { emit styleNormal(); }
	void _p_style_fire() { emit styleFire(); }
	void _p_style_line() { emit styleLine(); }
	void _p_style_led() { emit styleLED(); }
	void _p_style_analog() { emit styleAnalog(); }
	void _p_style_small() { emit styleSmall(); }
signals:
	void styleNormal();
	void styleFire();
	void styleLine();
	void styleLED();
	void styleAnalog();
	void styleSmall();
private:
	KArtsServer* _kartsserver;
	KActionCollection* _actioncollection;
	KAction *_a_sv, *_a_am, *_a_asv, *_a_mmv, *_a_ev, *_a_mtv;
	//KAction *_a_morebars, *_a_lessbars;
	KAction *_a_style_normal, *_a_style_fire, *_a_style_line, *_a_style_led, *_a_style_analog, *_a_style_small;
	KPopupMenu* _stylemenu;
	FFTScopeView *_sv;
	Gui_AUDIO_MANAGER *_am;
	ArtsStatusView *_asv;
	MidiManagerView *_mmv;
	EnvironmentView *_ev;
	MediaTypesView *_mtv;
};

#endif
