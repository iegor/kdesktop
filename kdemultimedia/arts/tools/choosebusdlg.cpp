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

#include "choosebusdlg.h"

#include <artsflow.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qpushbutton.h>

#include <kbuttonbox.h>
#include <kseparator.h>
#include <klineedit.h>
#include <klocale.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kstdguiitem.h>

using namespace std;

static void min_size(QWidget *w) {
  w->setMinimumSize(w->sizeHint());
}

ChooseBusDlg::ChooseBusDlg(QWidget *parent)
	: KDialog(parent,"X", TRUE)
	, _newbusitemindex( -1 )
{
	setCaption(i18n("Choose Bus"));

	QVBoxLayout *mainlayout = new QVBoxLayout(this);

// caption label: "Synthesis running..."

	mainlayout->addSpacing(5);
	QLabel *captionlabel = new QLabel(this);
	QFont labelfont(captionlabel->font());
	labelfont.setPointSize(labelfont.pointSize()*3/2);
	captionlabel->setFont(labelfont);
	captionlabel->setText(QString(" ")+i18n("Available busses:")+QString(" "));
	captionlabel->setAlignment(AlignCenter);
	min_size(captionlabel);
	mainlayout->addWidget(captionlabel);

// hruler

	mainlayout->addSpacing(5);
	KSeparator *ruler2 = new KSeparator( KSeparator::HLine, this);
	mainlayout->addWidget(ruler2);

	mainlayout->addSpacing(5);

// listwidget


	listbox = new QListBox(this);
    listbox->setMinimumSize(300,200);

	Arts::AudioManager aman = Arts::Reference("global:Arts_AudioManager");

	if(!aman.isNull())
	{
		vector<string> *destinations = aman.destinations();
		unsigned long i;
		for(i=0;i<destinations->size();i++)
			listbox->insertItem((*destinations)[i].c_str());
		delete destinations;
	}
	if( listbox->count() > 0 )
		listbox->setCurrentItem( 0 );

	mainlayout->addWidget(listbox);

// hruler

	mainlayout->addSpacing(5);
	KSeparator *ruler = new KSeparator( KSeparator::HLine, this);
	mainlayout->addWidget(ruler);
	mainlayout->addSpacing(5);

// new bus lineedit

	QBoxLayout * layout2 = new QHBoxLayout( mainlayout );
	//mainlayout->addLayout( layout2 );
	QLabel * newbuslabel = new QLabel( i18n( "New bus:" ), this );
	layout2->addWidget( newbuslabel );
	lineedit = new KLineEdit( this );
	connect( lineedit, SIGNAL( textChanged( const QString & ) ), SLOT( textChanged( const QString & ) ) );
	layout2->addWidget( lineedit );

// hruler

	mainlayout->addSpacing(5);
	KSeparator *ruler3 = new KSeparator( KSeparator::HLine, this);
	mainlayout->addWidget(ruler3);

	mainlayout->addSpacing(5);

// buttons

	mainlayout->addSpacing(5);
	QHBoxLayout *buttonlayout = new QHBoxLayout( mainlayout );
	//mainlayout->addLayout(buttonlayout);
	mainlayout->addSpacing(5);

	buttonlayout->addSpacing(5);
	KButtonBox *bbox = new KButtonBox(this);

	bbox->addButton(KStdGuiItem::help(), this, SLOT( help() ));
	bbox->addStretch(1);

	QPushButton * okbutton = bbox->addButton(KStdGuiItem::ok());
	okbutton->setDefault( true );
	connect( okbutton, SIGNAL( clicked() ), SLOT(accept() ) );

	QButton *cancelbutton = bbox->addButton(KStdGuiItem::cancel());
	connect( cancelbutton, SIGNAL( clicked() ), SLOT(reject() ) );

	bbox->layout();

	buttonlayout->addWidget(bbox);
	buttonlayout->addSpacing(5);

	mainlayout->freeze();
}

QString ChooseBusDlg::result()
{
	if(listbox->currentItem() != -1)
	{
		return(listbox->text(listbox->currentItem()));
	}
	return(0);
}

void ChooseBusDlg::help()
{
	KApplication::kApplication()->invokeHelp("", "artsbuilder");
}

void ChooseBusDlg::textChanged( const QString & busname )
{
	if( ! busname.isEmpty() )
	{
		if( _newbusitemindex > -1 )
			listbox->changeItem( busname, _newbusitemindex );
		else
		{
			_newbusitemindex = listbox->count();
			listbox->insertItem( busname, _newbusitemindex );
			listbox->setCurrentItem( _newbusitemindex );
		}
	}
	else
	{
		listbox->removeItem( _newbusitemindex );
		listbox->setCurrentItem( _newbusitemindex - 1 );
		_newbusitemindex = -1;
	}
}

#include "choosebusdlg.moc"

// vim: sw=4 ts=4
