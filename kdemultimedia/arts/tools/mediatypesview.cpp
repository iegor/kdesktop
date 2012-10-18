/*

    Copyright (C) 2000-2001 Stefan Westerfeld
                            <stefan@space.twc.de>
                       2003 Arnold Krille
                            <arnold@arnoldarts.de>

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

#include <qlayout.h>
#include <klistview.h>
#include <klocale.h>
#include <kartsserver.h>
#include <map>
#include <kiconloader.h>

#include "mediatypesview.h"

using namespace std;
using namespace Arts;

MediaTypesView::MediaTypesView( QWidget* parent, const char* name ) : Template_ArtsView( parent,name )
{
	this->setCaption( i18n( "Available Media Types" ) );
	this->setIcon( MainBarIcon( "artsmediatypes", 32 ) );
	QBoxLayout *l= new QHBoxLayout(this);
	l->setAutoAdd(true);

	KListView *listView = new KListView(this);
	listView->addColumn(i18n("Media Type"));

	Arts::TraderQuery q;
	std::vector<Arts::TraderOffer> *results = q.query();
	std::map<std::string, bool> done;
	QString str;

	for(std::vector<Arts::TraderOffer>::iterator i = results->begin(); i != results->end(); i++)
	{
		std::vector<string> *ext = (*i).getProperty("Extension");

		for(vector<string>::iterator it = ext->begin(); it != ext->end(); it++)
		{
			if(!(*it).length() || done[*it])
			    continue;

			done[*it] = true;
			(void) new QListViewItem(listView, (*it).c_str());
		}
		delete ext;
	}
	delete results;

	l->activate();
	show();
	setBaseSize(300,200);
}

MediaTypesView::~MediaTypesView()
{
}

#include "mediatypesview.moc"
