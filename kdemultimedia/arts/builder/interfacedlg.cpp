	/*

	Copyright (C) 1998-2001 Stefan Westerfeld
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

#include "interfacedlg.h"
#include "structureport.h"
#include <qlayout.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <kbuttonbox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <kseparator.h>
#include <klocale.h>
#include <qlineedit.h>
#include <stdio.h>
#include <arts/debug.h>
#include <arts/core.h>
#include <arts/dispatcher.h>
#include <qpushbutton.h>
#include <kstdguiitem.h>

using namespace std;

InterfaceDlg::InterfaceDlg(QWidget *parent) :QDialog(parent,"Props", TRUE)
{
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

	listbox->setMinimumSize(340,400);
	mainlayout->addWidget(listbox);
        connect( listbox, SIGNAL( doubleClicked ( QListBoxItem *)), this,
                 SLOT(accept()));
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

	QButton *okbutton = bbox->addButton(KStdGuiItem::ok());
	connect( okbutton, SIGNAL( clicked() ), SLOT(accept() ) );

	QButton *cancelbutton = bbox->addButton(KStdGuiItem::cancel());
	connect( cancelbutton, SIGNAL( clicked() ), SLOT(reject() ) );

	bbox->layout();
	//min_size(bbox);

	buttonlayout->addWidget(bbox);
	buttonlayout->addSpacing(5);

	//mainlayout->activate();
	mainlayout->freeze();
}

string InterfaceDlg::interfaceName()
{
	if(listbox->currentItem() != -1)
	{
		string s = listbox->text(listbox->currentItem()).local8Bit().data();
		string::iterator j = s.begin();
		while(*j == ' ') j++;
			return string(j, s.end());
	}
	return "";
}

void InterfaceDlg::raise()
{
}

void InterfaceDlg::lower()
{
}

void InterfaceDlg::rename()
{
}

void InterfaceDlg::update(const string& interface, const string& indent)
{
	listbox->insertItem((indent + interface).c_str());

	vector<string> *children = Arts::Dispatcher::the()->interfaceRepo().queryChildren(interface);
	for (vector<string>::iterator ci = children->begin(); ci != children->end(); ++ci)
		update(ci->c_str(), indent+"  ");
	delete children;
}

void InterfaceDlg::update()
{
	update("Arts::Object", "");
}

void InterfaceDlg::help()
{
	KApplication::kApplication()->invokeHelp("", "karts");
}

#include "interfacedlg.moc"
