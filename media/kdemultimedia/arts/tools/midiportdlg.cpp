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

#include "midiportdlg.h"
#include <klocale.h>

#include <kapplication.h>
#include <qlayout.h>
#include <qlabel.h>
#include <kseparator.h>
#include <kbuttonbox.h>
#include <qlineedit.h>
#include <qbutton.h>
#include <qpushbutton.h>
#include <kstdguiitem.h>

MidiPortDlg::MidiPortDlg(QWidget *parent, const char *oldname, const char *title) :QDialog(parent,title,TRUE)
{
	QVBoxLayout *mainlayout = new QVBoxLayout(this);

// caption label: title

	mainlayout->addSpacing(5);
	QLabel *captionlabel = new QLabel(this);
	QFont labelfont(captionlabel->font());
	labelfont.setPointSize(labelfont.pointSize()*3/2);
	captionlabel->setFont(labelfont);
	captionlabel->setText(i18n("OSS MIDI Port"));
	captionlabel->setAlignment(AlignCenter);
	//min_size(captionlabel);
	mainlayout->addWidget(captionlabel);

// hruler

	mainlayout->addSpacing(5);
	KSeparator *ruler2 = new KSeparator( KSeparator::HLine, this);
	mainlayout->addWidget(ruler2);
	mainlayout->addSpacing(5);

// editwidget

	edit = new QLineEdit(this);
	edit->setText(oldname);
	//min_size(edit);

	mainlayout->addWidget(edit);

// hruler

	mainlayout->addSpacing(5);
	KSeparator *ruler = new KSeparator( KSeparator::HLine, this);
	mainlayout->addWidget(ruler);
	mainlayout->addSpacing(5);

// buttons

	QHBoxLayout *buttonlayout = new QHBoxLayout;
	mainlayout->addSpacing(5);
	mainlayout->addLayout(buttonlayout);
	mainlayout->addSpacing(5);

	buttonlayout->addSpacing(5);
	KButtonBox *bbox = new KButtonBox(this);

	QPushButton *helpbutton = bbox->addButton(KStdGuiItem::help(), this, SLOT( help() ));
	bbox->addStretch(1);
	helpbutton->setAutoDefault( true );
	helpbutton->setDefault( true );

	QPushButton *okbutton = bbox->addButton(KStdGuiItem::ok());
	connect( okbutton, SIGNAL( clicked() ), SLOT(accept() ) );
	okbutton->setAutoDefault( true );
	okbutton->setDefault( true );

	bbox->layout();

	buttonlayout->addWidget(bbox);
	buttonlayout->addSpacing(5);

	mainlayout->freeze();
}

const char *MidiPortDlg::device()
{
	return edit->text().ascii();
}

void MidiPortDlg::help()
{
	KApplication::kApplication()->invokeHelp("", "artsbuilder");
}

#include "midiportdlg.moc"
