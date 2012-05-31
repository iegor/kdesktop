    /*

    Copyright (C) 2001 Stefan Westerfeld
                       <stefan@space.twc.de>
                  2003 Matthias Kretz <kretz@kde.org>
		  2003 Arnold Krille <arnold@arnoldarts.de

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

#ifndef ARTS_ENVIRONMENT_VIEW_H
#define ARTS_ENVIRONMENT_VIEW_H

#include "artsmodules.h"

#include "templateview.h"

class QListBoxItem;
class KListBox;

class EnvironmentView : public Template_ArtsView {
	Q_OBJECT
protected:
	Arts::Environment::Container container;
	KListBox *listBox;
	QString defaultEnvFileName;

public:
	EnvironmentView( Arts::Environment::Container container, QWidget* =0, const char* =0 );

public slots:
	void view(QListBoxItem *i);
	void addMixer();
	void addEffectRack();
	void delItem();
	void update();
	void load();
	void save();

};

Arts::Environment::Container defaultEnvironment();

#endif /* ARTS_ENVIRONMENT_VIEW_H */
