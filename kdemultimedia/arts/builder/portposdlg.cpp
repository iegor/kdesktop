	/*

	Copyright (C) 1998 Stefan Westerfeld
                       stefan@space.twc.de

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

#include "portposdlg.h"
#include "structureport.h"
#include <qlayout.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <kbuttonbox.h>
#include <klocale.h>
#include <kinputdialog.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <kseparator.h>
#include <qlineedit.h>
#include <stdio.h>
#include <arts/debug.h>
#include <qpushbutton.h>
#include <kstdguiitem.h>

using namespace std;

PortPosDlg::PortPosDlg(QWidget *parent, Structure *structure) :QDialog(parent,"Props", TRUE)
{
	this->structure = structure;

	setCaption(i18n("aRts: Structureport View"));

	QVBoxLayout *mainlayout = new QVBoxLayout(this);
	//QHBoxLayout *contentslayout = new QHBoxLayout;

// object type
/*
	mainlayout->addSpacing(5);
	QLabel *objectlabel = new QLabel(this);
	QFont labelfont(objectlabel->font());
	labelfont.setPointSize(labelfont.pointSize()*3/2);
	objectlabel->setFont(labelfont);
	objectlabel->setText(QString(" ")+i18n("Object type: ")+QString(port->owner->name())+QString(" "));
	objectlabel->setAlignment(AlignCenter);
	min_size(objectlabel);
	mainlayout->addWidget(objectlabel);
*/

// port description

/*
	mainlayout->addSpacing(5);
	QLabel *portlabel = new QLabel(this);
	labelfont.setPointSize(labelfont.pointSize()*4/5);
	portlabel->setFont(labelfont);
	portlabel->setText(i18n("Port description: ")+ port->description);
	min_size(portlabel);
	portlabel->setAlignment(AlignCenter);
	mainlayout->addWidget(portlabel);

	int labelwidth = imax(portlabel->sizeHint().width(),objectlabel->sizeHint().width());

	portlabel->setMinimumWidth(labelwidth);
	objectlabel->setMinimumWidth(labelwidth);

// hruler

	mainlayout->addSpacing(5);
	KSeparator *ruler = new KSeparator( KSeparator::HLine, this);
	mainlayout->addWidget(ruler);
	mainlayout->addSpacing(5);
	mainlayout->addLayout(contentslayout);
*/
// list

	listbox = new QListBox(this);

	update();

	listbox->setMinimumSize(100,200);
	mainlayout->addWidget(listbox);
// hruler

	mainlayout->addSpacing(5);
	KSeparator *ruler2 = new KSeparator( KSeparator::HLine, this);
	mainlayout->addWidget(ruler2);

// buttons

	QHBoxLayout *buttonlayout = new QHBoxLayout;
	mainlayout->addSpacing(5);
	mainlayout->addLayout(buttonlayout);
	mainlayout->addSpacing(5);

	buttonlayout->addSpacing(5);
	KButtonBox *bbox = new KButtonBox(this);

	bbox->addButton(KStdGuiItem::help(), this, SLOT( help() ));
	bbox->addStretch(1);

	KIconLoader iconloader;
	QButton *raise = bbox->addButton(i18n("&Raise"));
	raise->setPixmap(iconloader.loadIcon("up", KIcon::Small));
	connect( raise, SIGNAL( clicked() ), SLOT( raise() ));

	QButton *lower = bbox->addButton(i18n("&Lower"));
	lower->setPixmap(iconloader.loadIcon("down", KIcon::Small));
	connect( lower, SIGNAL( clicked() ), SLOT( lower() ));

	QButton *rename = bbox->addButton(i18n("R&ename..."));
	connect( rename, SIGNAL( clicked() ), SLOT( rename() ));

	QButton *okbutton = bbox->addButton(KStdGuiItem::ok());
	connect( okbutton, SIGNAL( clicked() ), SLOT(accept() ) );

/*
	QButton *cancelbutton = bbox->addButton(i18n("Cancel"));
	connect( cancelbutton, SIGNAL( clicked() ), SLOT(reject() ) );
*/
	bbox->layout();
	//min_size(bbox);

	buttonlayout->addWidget(bbox);
	buttonlayout->addSpacing(5);

	//mainlayout->activate();
	mainlayout->freeze();
}

void PortPosDlg::raise()
{
	int i = listbox->currentItem();
	arts_debug("selected %d",i);
	if(i < 0) return;

	StructurePort *port = listports[i];
	assert(port);

	// hmm ok this is ugly that the raise function calls lowerPosition
	port->lowerPosition();
	update();

	unsigned long l;
	for(l=0;l<listports.size();l++)
		if(listports[l]->id() == port->id())
			listbox->setCurrentItem(l);
}

void PortPosDlg::lower()
{
	int i = listbox->currentItem();
	arts_debug("selected %d",i);
	if(i < 0) return;
	StructurePort *port = listports[i];
	assert(port);

	port->raisePosition();
	update();

	unsigned long l;
	for(l=0;l<listports.size();l++)
		if(listports[l]->id() == port->id())
			listbox->setCurrentItem(l);
}

void PortPosDlg::rename()
{
	int i = listbox->currentItem();
	arts_debug("selected %d",i);
	if(i < 0) return;
	StructurePort *port = listports[i];
	assert(port);

    bool ok;
    QString name = KInputDialog::getText( i18n( "Rename Port" ),
      i18n( "Enter port name:" ), port->name(), &ok, this );
	if (ok)
	{
		arts_debug("rename OK...");
		port->rename(name.local8Bit());
	}
	update();

	unsigned long l;
	for(l=0;l<listports.size();l++)
		if(listports[l]->id() == port->id())
			listbox->setCurrentItem(l);
}
void PortPosDlg::update()
{
	list<StructureComponent *> &cl = *structure->getComponentList();
	list<StructureComponent *>::iterator ci;

	listports.erase(listports.begin(), listports.end());
	listbox->clear();

	// first incoming ports, then outgoing (which are represented by
	// the opposite directions inside the structure)
	for(int direction = 0; direction < 2; direction++)
	{
		map<long, StructurePort *> pmap;
		int finddirection = ModulePort::in;
		int pcount = 0;

		if(direction == 0) finddirection = ModulePort::out;

		for(ci = cl.begin(); ci != cl.end(); ++ci)
		{
			StructureComponent *component = *ci;
			if(component->type() == StructureComponent::ctPort)
			{
				StructurePort *port = (StructurePort *)component;
				if(port->port()->direction == finddirection)
				{
					arts_debug("port %s position %ld",
							port->name().local8Bit().data(), port->position());
					pmap[port->position()] = port;
					pcount++;
				}
			}
		}
		for(int i=0;i<pcount;i++)
		{
			StructurePort *port = pmap[i];
			if (port)
			{
				assert(port);
				listbox->insertItem(port->name(),listports.size());
				listports.push_back(port);
				arts_debug("listports.size() is now %d",listports.size());
			}
		}
	}
	listbox->repaint();
}

void PortPosDlg::help()
{
	KApplication::kApplication()->invokeHelp("", "karts");
}

#include "portposdlg.moc"
