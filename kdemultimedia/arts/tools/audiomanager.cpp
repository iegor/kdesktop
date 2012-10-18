/*

    Copyright (C) 1999 Stefan Westerfeld
                       stefan@space.twc.de
                  2003 Arnold Krille
                       arnold@arnoldarts.de

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

#include "audiomanager.h"
#include "choosebusdlg.h"

#include <qtimer.h>
#include <qlayout.h>

#include <klocale.h>
#include <klistview.h>
#include <kiconloader.h>

using namespace std;

/*
 * as this is an 1:1 port of an old arts-0.3.4.1 artsbuilable visual widget,
 * you'll see some porting artefacts, and it's not elegance itself ;)
 */
Gui_AUDIO_MANAGER::Gui_AUDIO_MANAGER( QWidget* parent, const char* name ) : Template_ArtsView( parent,name )
{
	this->setCaption( i18n( "Audio Manager" ) );
	this->setIcon( MainBarIcon( "artsaudiomanager", 32 ) );
	//printf("constructor\n");
	ParentWidget = 0;
	listview = 0;
	inDialog = false;
	proxy = new GuiAudioManagerProxy(this);

	AudioManager = Arts::Reference("global:Arts_AudioManager");
	changes = AudioManager.changes()-1;
	setParent(this,0);
	tick();
	show();

	QTimer *updatetimer = new QTimer(this);
	updatetimer->start(500);
	QObject::connect(updatetimer,SIGNAL(timeout()),this,SLOT(tick()));
}

Gui_AUDIO_MANAGER::~Gui_AUDIO_MANAGER()
{
	if(listview) delete listview;
	delete proxy;
}

#if 0
void Gui_AUDIO_MANAGER::widgetDestroyed(QWidget *widget)
{
	assert(widget == listview);
	listview = 0;
}
#endif

void Gui_AUDIO_MANAGER::setParent(QWidget *parent, QBoxLayout * /*layout*/)
{
/************************************************************************
 * From Gui_INSTRUMENT_MAPPER:
 *
 * I am still not sure wether this kind of putting yourself into a parent
 * widget (with own layout etc.) is a good idea (there may not even be
 * a singe call to setParent, because there is no parent).
 *
 * But the "how to write aRts widgets"-stuff will need some experiments,
 * so lets try that method...
 *
 * ----
 *
 * But perhaps here (since only one widget Listview is used) something
 * else would be appropriate. Check that. FIXME
 ************************************************************************/

	QVBoxLayout *mainlayout = new QVBoxLayout(parent);
	/*QHBoxLayout *contentslayout = new QHBoxLayout;*/

// list

	listview = new KListView(parent);

	listview->addColumn(i18n("Title"),175);
	listview->addColumn(i18n("Type"),50);
	listview->addColumn(i18n("Bus"),75);

	listview->setMinimumSize(300,100);

	QObject::connect(listview,SIGNAL(executed(QListViewItem *)),proxy,
								SLOT(edit(QListViewItem *)));

	mainlayout->addWidget(listview);

	mainlayout->activate();
	//mainlayout->freeze();
	ParentWidget = parent;
}

void Gui_AUDIO_MANAGER::tick()
{
	unsigned long newChanges = AudioManager.changes();
	if(inDialog) return;
	if(changes == newChanges) return;

	changes = newChanges;

	listview->clear();
	vector<Arts::AudioManagerInfo> *acs = AudioManager.clients();

	unsigned long c;
	for(c=0;c<acs->size();c++)
	{
		//char status[2][10] = {"init","running"};
		QString description = QString::fromUtf8( (*acs)[c].title.c_str() );
		QString type;
		if((*acs)[c].direction == Arts::amPlay)
			type = i18n("play");
		else
			type = i18n("record");
		QString destination = QString::fromUtf8( (*acs)[c].destination.c_str() );
		long ID = (*acs)[c].ID;

		(void)new AudioManagerItem(listview, description, type, destination, ID);
	}
	delete acs;
	//Widget->show();
}

void Gui_AUDIO_MANAGER::edit(QListViewItem *item)
{
	AudioManagerItem *ai = (AudioManagerItem *)item;
	ChooseBusDlg *cd = new ChooseBusDlg(0);
	assert(cd);

	inDialog = true;
	int accept = cd->exec();
	inDialog = false;

	if( accept == QDialog::Accepted )
	{
		QString result = cd->result();
		if(!result.isNull())
		{
			//lukas: I hope UTF-8 is OK here...
			AudioManager.setDestination(ai->ID(),result.utf8().data());
			// refresh:
			changes = 0;
			tick();
		}
	}
	delete cd;
}

GuiAudioManagerProxy::GuiAudioManagerProxy(Gui_AUDIO_MANAGER *gim)
{
	this->gim = gim;
}

void GuiAudioManagerProxy::edit(QListViewItem *item)
{
	gim->edit(item);
}

AudioManagerItem::AudioManagerItem(QListView *parent, QString a,
         QString b, QString c, long ID) :QListViewItem(parent,a,b,c)
{
	_ID = ID;
}

long AudioManagerItem::ID()
{
	return _ID;
}

AudioManagerItem::~AudioManagerItem()
{
	//
}
#include "audiomanager.moc"

// vim: sw=4 ts=4
