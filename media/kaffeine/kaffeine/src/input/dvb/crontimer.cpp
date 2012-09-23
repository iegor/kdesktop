/*
 * crontimer.cpp
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

#include <kmessagebox.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kpushbutton.h>

#include "crontimer.h"

CronTimer::CronTimer( int m, QWidget *parent ) : CronTimerUI( parent )
{
	connect( cronBtnGrp, SIGNAL(clicked(int)), this, SLOT(modeSelected(int)) );
	mode = m;
	switch ( mode ) {
		case Noone : cronBtnGrp->setButton( 0 ); break;
		case Daily : cronBtnGrp->setButton( 1 ); break;
		case Weekly : cronBtnGrp->setButton( 2 ); break;
		case Monthly : cronBtnGrp->setButton( 3 ); break;
		default : cronBtnGrp->setButton( 4 );
	}
	if ( mode>Monthly ) {
		daysGb->setEnabled( true );
		if ( mode&Monday ) monCb->setChecked( true );
		if ( mode&Tuesday ) tueCb->setChecked( true );
		if ( mode&Wednesday ) wedCb->setChecked( true );
		if ( mode&Thursday ) thuCb->setChecked( true );
		if ( mode&Friday ) friCb->setChecked( true );
		if ( mode&Saturday ) satCb->setChecked( true );
		if ( mode&Sunday ) sunCb->setChecked( true );
	}
	else
		daysGb->setEnabled( false );

	KIconLoader *icon = new KIconLoader();
	cancelBtn->setGuiItem( KGuiItem(i18n("Cancel"), icon->loadIconSet("cancel", KIcon::Small) ) );
	okBtn->setGuiItem( KGuiItem(i18n("OK"), icon->loadIconSet("ok", KIcon::Small) ) );
        delete icon;
}



CronTimer::~CronTimer()
{
}



int CronTimer::getMode() const
{
	return mode;
}



void CronTimer::modeSelected( int id )
{
	if ( id==4 )
		daysGb->setEnabled( true );
	else
		daysGb->setEnabled( false );
}



void CronTimer::accept()
{
	switch ( cronBtnGrp->selectedId() ) {
		case 0 : mode = Noone; break;
		case 1 : mode = Daily; break;
		case 2 : mode = Weekly; break;
		case 3 : mode = Monthly; break;
		case 4 : {
			mode = Custom;
			if ( monCb->isChecked() ) mode+=Monday;
			if ( tueCb->isChecked() ) mode+=Tuesday;
			if ( wedCb->isChecked() ) mode+=Wednesday;
			if ( thuCb->isChecked() ) mode+=Thursday;
			if ( friCb->isChecked() ) mode+=Friday;
			if ( satCb->isChecked() ) mode+=Saturday;
			if ( sunCb->isChecked() ) mode+=Sunday;
			if ( mode<Monday ) {
				KMessageBox::sorry( this, i18n("You have to choose some days.") );
				return;
			}
		}
	}

	done( Accepted );
}

#include "crontimer.moc"
