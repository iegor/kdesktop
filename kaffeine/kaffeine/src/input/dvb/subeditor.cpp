/*
 * subeditor.cpp
 *
 * Copyright (C) 2005 Christophe Thommeret <hftom@free.fr>
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

#include "subeditor.h"



SubEditor::SubEditor( ChannelDesc *chan, QWidget *parent ) : SubEditorUI( parent )
{
	QString s, t;
	int i;

	channel = chan;
	pidList->clear();

	for ( i=0; i<channel->nsubpid; i++ ) insertItem( i );

	if ( channel->nsubpid==channel->maxsubpid ) newBtn->setEnabled( false );

	connect( pidList, SIGNAL(highlighted(int)), this, SLOT(showProp(int)) );
	connect( upBtn, SIGNAL(clicked()), this, SLOT(itemUp()) );
	connect( downBtn, SIGNAL(clicked()), this, SLOT(itemDown()) );
	connect( delBtn, SIGNAL(clicked()), this, SLOT(itemDelete()) );
	connect( updateBtn, SIGNAL(clicked()), this, SLOT(itemUpdate()) );
	connect( newBtn, SIGNAL(clicked()), this, SLOT(itemNew()) );
}



void SubEditor::insertItem( int index, bool updt )
{
	QString s, t;

	s = t.setNum( channel->subpid[index].pid );
	s = s+"("+t.setNum( channel->subpid[index].page )+")";
	s = s+"("+t.setNum( channel->subpid[index].id )+")";
	if ( !channel->subpid[index].lang.isEmpty() ) s = s+"("+channel->subpid[index].lang+")";
	if ( updt ) pidList->changeItem( s, index );
	else pidList->insertItem( s, index );
}



void SubEditor::showProp( int index )
{
	if ( index<0 ) {
		pidSpin->setValue( 0 );
		pageSpin->setValue( 0 );
		ancSpin->setValue( 0 );
		langLe->setText( "" );
	}
	else {
		pidSpin->setValue( channel->subpid[index].pid );
		pageSpin->setValue( channel->subpid[index].page );
		ancSpin->setValue( channel->subpid[index].id );
		langLe->setText( channel->subpid[index].lang );
	}
}



void SubEditor::itemUp()
{
	SubPid a;

	int n = pidList->currentItem();

	if ( n<1 ) return;

	a = channel->subpid[n-1];
	channel->subpid[n-1] = channel->subpid[n];
	channel->subpid[n] = a;

	insertItem( n, true );
	insertItem( n-1, true );
}



void SubEditor::itemDown()
{
	SubPid a;

	int n = pidList->currentItem();

	if ( (n<0) || (n>(channel->nsubpid-2)) ) return;

	a = channel->subpid[n+1];
	channel->subpid[n+1] = channel->subpid[n];
	channel->subpid[n] = a;

	insertItem( n, true );
	insertItem( n+1, true );
}



void SubEditor::itemDelete()
{
	int n = pidList->currentItem();

	if ( channel->nsubpid==0 || (n<0) ) return; //for sure

	for ( int i=n; i<channel->nsubpid-1; i++ ) channel->subpid[i] = channel->subpid[i+1];
	channel->nsubpid--;
	pidList->removeItem( n );

	newBtn->setEnabled( true );
}



void SubEditor::itemUpdate()
{
	int n = pidList->currentItem();

	if ( n<0 ) return;

	channel->subpid[n].pid = pidSpin->value();
	channel->subpid[n].page = pageSpin->value();
	channel->subpid[n].id = ancSpin->value();
	channel->subpid[n].lang = langLe->text().stripWhiteSpace();

	insertItem( n, true );
}



void SubEditor::itemNew()
{
	if ( channel->nsubpid==channel->maxsubpid ) {   //for sure
		newBtn->setEnabled( false );
		return;
	}

	if ( !pidSpin->value() ) {
		KMessageBox::sorry( this, i18n("Pid must be non zero!") );
		return;
	}
	channel->nsubpid++;
	channel->subpid[channel->nsubpid-1].pid = pidSpin->value();
	channel->subpid[channel->nsubpid-1].page = pageSpin->value();
	channel->subpid[channel->nsubpid-1].id = ancSpin->value();
	channel->subpid[channel->nsubpid-1].lang = langLe->text().stripWhiteSpace();
	insertItem( channel->nsubpid-1 );
	if ( channel->nsubpid==channel->maxsubpid ) newBtn->setEnabled( false );
}



SubEditor::~SubEditor()
{

}

#include "subeditor.moc"
