/*
 * broadcasteditor.cpp
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

#include <qdir.h>
#include <qvaluelist.h>

#include <kpushbutton.h>
#include <kiconloader.h>
#include <klocale.h>

#include "broadcasteditor.h"

BroadcastEditor::BroadcastEditor( QWidget *parent, QPtrList<ChannelDesc> *ch, QPtrList<ChannelDesc> *ret ) : BroadcastEditorUI( parent )
{
	KIconLoader *icon = new KIconLoader();

	tvPix = icon->loadIcon( "kdvbtv", KIcon::Small );
	tvcPix = icon->loadIcon( "kdvbtvc", KIcon::Small );
	raPix = icon->loadIcon( "kdvbra", KIcon::Small );
	racPix = icon->loadIcon( "kdvbrac", KIcon::Small );
	addBtn->setGuiItem( KGuiItem(i18n("Add"), icon->loadIconSet("forward", KIcon::Small) ) );
	resetBtn->setGuiItem( KGuiItem(i18n("Reset"), icon->loadIconSet("reload", KIcon::Small) ) );
	cancelBtn->setGuiItem( KGuiItem(i18n("Cancel"), icon->loadIconSet("cancel", KIcon::Small) ) );
	okBtn->setGuiItem( KGuiItem(i18n("OK"), icon->loadIconSet("ok", KIcon::Small) ) );
	
	connect( channelLb, SIGNAL(doubleClicked(QListBoxItem*)), this, SLOT(slotAddChannel(QListBoxItem*)) );

	chan = ch;
	list = ret;

	resetList();
        delete icon;
}

BroadcastEditor::~BroadcastEditor()
{
}

ChannelDesc* BroadcastEditor::getChannel( const QString &name )
{
	int i;

	for ( i=0; i<(int)chan->count(); i++ ) {
		if ( chan->at(i)->name==name ) return chan->at(i);
	}
	return 0;
}

void BroadcastEditor::slotAddChannel(QListBoxItem*)
{
	addToList();
}

void BroadcastEditor::addToList()
{
	int i;
	QString curName;
	QValueList<QListBoxItem*> qvl;
	ChannelDesc *c=0, *d=0;

	for ( i=0; i<(int)channelLb->count(); i++ ) {
		if ( channelLb->isSelected(i) ) {
			curName = channelLb->text(i);
			c = getChannel( curName );
			if ( !c ) continue;
			broadcastLb->insertItem( *channelLb->pixmap(i), channelLb->text(i) );
			list->append( c );
			qvl.append(channelLb->item(i));
		}
	}
	if ( !c ) return;
	for ( i=0; i<(int)qvl.count(); i++ ) channelLb->takeItem( qvl[i] );

	qvl.clear();
	for ( i=0; i<(int)channelLb->count(); i++ ) {
		d = getChannel( channelLb->text(i) );
		if ( !d ) continue;
		if ( d->tp!=c->tp ) qvl.append(channelLb->item(i));
	}
	for ( i=0; i<(int)qvl.count(); i++ ) channelLb->takeItem( qvl[i] );

	channelLb->setSelectionMode( QListBox::Extended );
}

bool BroadcastEditor::getChannelList()
{
	int i;
	ChannelDesc *c;

	for ( i=0; i<(int)chan->count(); i++ ) {
		c = chan->at(i);
		if ( c->fta ) {
			if ( c->type==1 ) channelLb->insertItem( tvcPix, c->name );
			else channelLb->insertItem( racPix, c->name );
		}
		else {
			if ( c->type==1 ) channelLb->insertItem( tvPix, c->name );
			else channelLb->insertItem( raPix, c->name );
		}
	}
	return true;
}

void BroadcastEditor::resetList()
{
	addBtn->setEnabled( true );
	channelLb->clear();
	broadcastLb->clear();
	list->clear();
	getChannelList();
	channelLb->sort();
	channelLb->setSelectionMode( QListBox::Single );
}

#include "broadcasteditor.moc"
