/*
 * audioeditor.cpp
 *
 * Copyright (C) 2004-2005 Christophe Thommeret <hftom@free.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <qlistbox.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qcheckbox.h>

#include <kmessagebox.h>
#include <klocale.h>

#include "audioeditor.h"

AudioEditor::AudioEditor( ChannelDesc *chan, QWidget *parent ) : AudioEditorUI( parent )
{
	QString s, t;
	int i;

	channel = chan;
	pidList->clear();

	for ( i=0; i<channel->napid; i++ ) insertItem( i );

	if ( channel->napid==channel->maxapid ) newBtn->setEnabled( false );

	connect( pidList, SIGNAL(highlighted(int)), this, SLOT(showProp(int)) );
	connect( upBtn, SIGNAL(clicked()), this, SLOT(itemUp()) );
	connect( downBtn, SIGNAL(clicked()), this, SLOT(itemDown()) );
	connect( delBtn, SIGNAL(clicked()), this, SLOT(itemDelete()) );
	connect( updateBtn, SIGNAL(clicked()), this, SLOT(itemUpdate()) );
	connect( newBtn, SIGNAL(clicked()), this, SLOT(itemNew()) );
}

void AudioEditor::insertItem( int index, bool updt )
{
	QString s, t;

	s = t.setNum( channel->apid[index].pid );
	if ( !channel->apid[index].lang.isEmpty() ) s = s+"("+channel->apid[index].lang+")";
	if ( channel->apid[index].ac3 ) s = s+"(ac3)";
	if ( updt ) pidList->changeItem( s, index );
	else pidList->insertItem( s, index );
}

void AudioEditor::showProp( int index )
{
	if ( index<0 ) {
		pidSpin->setValue( 0 );
		langLe->setText( "" );
		ac3Cb->setChecked( false );
	}
	else {
		pidSpin->setValue( channel->apid[index].pid );
		langLe->setText( channel->apid[index].lang );
		ac3Cb->setChecked( channel->apid[index].ac3 );
	}
}

void AudioEditor::itemUp()
{
	AudioPid a;

	int n = pidList->currentItem();

	if ( n<1 ) return;

	a = channel->apid[n-1];
	channel->apid[n-1] = channel->apid[n];
	channel->apid[n] = a;

	insertItem( n, true );
	insertItem( n-1, true );
}

void AudioEditor::itemDown()
{
	AudioPid a;

	int n = pidList->currentItem();

	if ( (n<0) || (n>(channel->napid-2)) ) return;

	a = channel->apid[n+1];
	channel->apid[n+1] = channel->apid[n];
	channel->apid[n] = a;

	insertItem( n, true );
	insertItem( n+1, true );
}

void AudioEditor::itemDelete()
{
	int n = pidList->currentItem();

	if ( channel->napid==0 || (n<0) ) return; //for sure

	for ( int i=n; i<channel->napid-1; i++ ) channel->apid[i] = channel->apid[i+1];
	channel->napid--;
	pidList->removeItem( n );

	newBtn->setEnabled( true );
}

void AudioEditor::itemUpdate()
{
	int n = pidList->currentItem();

	if ( n<0 ) return;

	channel->apid[n].pid = pidSpin->value();
	channel->apid[n].lang = langLe->text().stripWhiteSpace();
	if ( ac3Cb->isChecked() ) channel->apid[n].ac3 = 1;
	else channel->apid[n].ac3 = 0;

	insertItem( n, true );
}

void AudioEditor::itemNew()
{
	if ( channel->napid==channel->maxapid ) {   //for sure
		newBtn->setEnabled( false );
		return;
	}

	if ( !pidSpin->value() ) {
		KMessageBox::sorry( this, i18n("Pid must be non zero!") );
		return;
	}
	channel->napid++;
	channel->apid[channel->napid-1].pid = pidSpin->value();
	channel->apid[channel->napid-1].lang = langLe->text().stripWhiteSpace();
	if ( ac3Cb->isChecked() ) channel->apid[channel->napid-1].ac3 = 1;
	else channel->apid[channel->napid-1].ac3 = 0;
	insertItem( channel->napid-1 );
	if ( channel->napid==channel->maxapid ) newBtn->setEnabled( false );
}

AudioEditor::~AudioEditor()
{
}

#include "audioeditor.moc"
