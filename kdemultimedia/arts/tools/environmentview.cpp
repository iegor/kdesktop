/*

    Copyright (C) 2001 Stefan Westerfeld
                       <stefan@space.twc.de>
                  2003 Matthias Kretz <kretz@kde.org>
                  2003 Arnold Krille <arnold@arnoldarts.de>

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

#include "environmentview.h"
#include <qdir.h>
#include <qfile.h>
#include <qpushbutton.h>

#include <klistbox.h>
#include <kartswidget.h>
#include <klocale.h>
#include <kiconloader.h>
#include <stdio.h>
#include <fstream>
#include <vector>

#include <qlayout.h>

#define DEFAULT_ENV_FILENAME "~/default.arts-env"

using namespace Arts;
using Environment::Container;
using Environment::Item;

class ItemView : public QListBoxText {
public:
	Item item;
	KArtsWidget *widget;
	ItemView(QListBox *listBox, Item item)
		: QListBoxText(listBox), item(item), widget(0)
	{
	}
	~ItemView()
	{
		delete widget;
		widget = 0;
		printf("~ItemView()\n");
	}
	QString text() const {
		return QString::fromLatin1(item._interfaceName().c_str());
	}
};

EnvironmentView::EnvironmentView( Container container, QWidget* parent, const char* name ) : Template_ArtsView( parent,name ), container(container)
{
	this->setCaption( i18n( "Environment" ) );
	this->setIcon( MainBarIcon( "artsenvironment", 32 ) );
	QVBoxLayout* _layout = new QVBoxLayout( this );
	_layout->setAutoAdd( true );
        defaultEnvFileName = DEFAULT_ENV_FILENAME;
        defaultEnvFileName.replace('~', QDir::homeDirPath());
        listBox = new KListBox(this);
	update();
	connect(listBox,SIGNAL(executed(QListBoxItem*)),
			this,SLOT(view(QListBoxItem*)));
	
	QPushButton *mixerButton = new QPushButton(i18n("Add Mixer"), this);
	connect(mixerButton, SIGNAL(clicked()), this, SLOT(addMixer()));

	QPushButton *effectRackButton = new QPushButton(i18n("Add Effect Rack"), this);
	connect(effectRackButton, SIGNAL(clicked()), this, SLOT(addEffectRack()));

	QPushButton *delButton = new QPushButton(i18n("Delete Item"), this);
	connect(delButton, SIGNAL(clicked()), this, SLOT(delItem()));

	QPushButton *loadButton = new
		QPushButton(i18n("Load %1").arg(DEFAULT_ENV_FILENAME), this);
	connect(loadButton, SIGNAL(clicked()), this, SLOT(load()));

	QPushButton *saveButton = new
		QPushButton(i18n("Save %1").arg(DEFAULT_ENV_FILENAME), this);
	connect(saveButton, SIGNAL(clicked()), this, SLOT(save()));
	show();
}

void EnvironmentView::view(QListBoxItem *i)
{
	ItemView *iv = static_cast<ItemView*>(i);

	if(!iv->widget)
	{
		GenericGuiFactory gf;
		Widget w = gf.createGui(iv->item);
		if(!w.isNull())
		{
			iv->widget = new KArtsWidget(w);
		}
		else
		{
			printf("no gui for %s\n",iv->text().ascii());
		}
	}
	if(iv->widget)
		iv->widget->show();
}

void EnvironmentView::addMixer()
{
	container.createItem("Arts::Environment::MixerItem");
	update();
}

void EnvironmentView::addEffectRack()
{
	container.createItem("Arts::Environment::EffectRackItem");
	update();
}

void EnvironmentView::delItem()
{
	int i = listBox->currentItem();
	if(i < 0) return; /* nothing selected */

	ItemView *iv = static_cast<ItemView*>(listBox->item(i));
	container.removeItem(iv->item);
	update();
}

void EnvironmentView::update()
{
	listBox->clear();

	std::vector<Item> *items = container.items();
	for(std::vector<Item>::iterator i = items->begin(); i != items->end(); i++)
		(void)new ItemView(listBox, *i);
	delete items;
}

void EnvironmentView::load()
{
	std::ifstream infile(QFile::encodeName(defaultEnvFileName).data());
	std::string line;
	std::vector<std::string> strseq;

	while(getline(infile,line))
		strseq.push_back(line);

	defaultEnvironment().loadFromList(strseq);
}

void EnvironmentView::save()
{
	std::vector<std::string> *strseq;
	strseq = defaultEnvironment().saveToList();

	std::ofstream outfile(QFile::encodeName(defaultEnvFileName).data());
	for(std::vector<std::string>::iterator i = strseq->begin(); i != strseq->end(); i++)
		outfile << ( *i ) << std::endl;
	delete strseq;
}
#include "environmentview.moc"
