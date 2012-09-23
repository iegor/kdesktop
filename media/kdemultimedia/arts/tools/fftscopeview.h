/*

    Copyright ( C ) 2000-2001 Stefan Westerfeld
                              <stefan@space.twc.de>
                    2003 Arnold Krille <arnold@arnoldarts.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    ( at your option ) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    */

#ifndef ARTSCONTROLLIB_FFTSCOPEVIEW_H
#define ARTSCONTROLLIB_FFTSCOPEVIEW_H

#include <artsmodules.h>
#include <flowsystem.h>
#include <kartsserver.h>

#include "templateview.h"

#include <artsgui.h>
#include <kdelibs_export.h>

class QTimer;
class KPopupMenu;
class KAction;
class KArtsWidget;
class ArtsActions;

class KDE_EXPORT FFTScopeView : public Template_ArtsView {
   Q_OBJECT
protected:
	Arts::StereoFFTScope scopefx;
	Arts::SimpleSoundServer server;
	long effectID;

	std::vector<float> *scopeData;
	std::vector<float> scopeDraw;
	std::vector<Arts::LevelMeter> scopeScales;
	std::vector<KArtsWidget*> aw;
	QTimer *updatetimer;

	ArtsActions* _artsactions;
	KPopupMenu *_menu, *_stylemenu;
	KAction *_moreBars, *_lessBars;
	KAction *_styleNormalBars, *_styleFireBars, *_styleLineBars, *_styleLEDs, *_styleAnalog, *_styleSmall;
	KAction *_substyle;

	void mousePressEvent( QMouseEvent* );
public:
	void updateScopeData();
	FFTScopeView( Arts::SimpleSoundServer server, QWidget* =0 );
	~FFTScopeView();

public slots:
	void updateScope();
	void moreBars();
	void lessBars();
	void setStyle( Arts::LevelMeterStyle style );
	void styleNormalBars();
	void styleFireBars();
	void styleLineBars();
	void styleLEDs();
	void styleAnalog();
	void styleSmall();
	void substyle();
};

// vim: sw=4 ts=4
#endif
