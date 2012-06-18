/*
 * ktimereditor.cpp
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

#include <qlayout.h>
#include <qlabel.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>

#include "ktimereditor.h"



KTimerEditor::KTimerEditor( bool newone, QStringList &chanList, RecTimer t, QWidget *parent ) : QDialog( parent )
{
	int i;

	QGridLayout *grid = new QGridLayout( 0, 1, 1, 11, 6 );
	QLabel *lab = new QLabel( i18n("Name:"), this );
	grid->addWidget( lab, 0, 0 );
	nameLe = new QLineEdit( this );
	grid->addWidget( nameLe, 0, 1 );
	lab = new QLabel( i18n("Channel:"), this );
	grid->addWidget( lab, 1, 0 );
	channelComb = new QComboBox( this );
	grid->addWidget( channelComb, 1, 1 );
	lab = new QLabel( i18n("Begin:"), this );
	grid->addWidget( lab, 2, 0 );
	begin = new QDateTimeEdit( this );
	grid->addWidget( begin, 2, 1 );
	lab = new QLabel( i18n("Duration:"), this );
	grid->addWidget( lab, 3, 0 );
	duration = new QTimeEdit( this );
	grid->addWidget( duration, 3, 1 );
	lab = new QLabel( i18n("End:"), this );
	grid->addWidget( lab, 4, 0 );
	end = new QDateTimeEdit( this );
	grid->addWidget( end, 4, 1 );
	repeatBtn = new KPushButton( this );
	grid->addWidget( repeatBtn, 5, 0 );
	repeatLab = new QLabel( this );
	grid->addWidget( repeatLab, 5, 1 );

	QFrame *line = new QFrame( this, "line1" );
	line->setFrameStyle( QFrame::HLine );
	line->setFrameShadow( QFrame::Sunken );
	line->setFrameShape( QFrame::HLine );

	QHBoxLayout *hb = new QHBoxLayout( 0, 0, 6 );
	cancelBtn = new KPushButton( this );
	hb->addWidget( cancelBtn );
	hb->addItem( new QSpacerItem( 20, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum ) );
	okBtn = new KPushButton( this );
	okBtn->setDefault( true );
	hb->addWidget( okBtn );

	QVBoxLayout *vb = new QVBoxLayout( this, 6, 6 );
	vb->addLayout( grid );
	vb->addItem( new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding ) );
	vb->addWidget( line );
	vb->addLayout( hb );

	timer = t;


	//begin->dateEdit()->setOrder( QDateEdit::DMY );
	channelComb->insertStringList( chanList );

	if ( newone ) {
		begin->setDateTime( QDateTime::currentDateTime() );
		duration->setTime( QTime(2,0,0) );
	}
	else {
		nameLe->setText( timer.name );
		for ( i=0; i<channelComb->count(); i++ ) {
			if ( channelComb->text(i)==timer.channel ) {
				channelComb->setCurrentItem(i);
				break;
			}
		}
		begin->setDateTime( timer.begin );
		duration->setTime( timer.duration );
		if ( timer.running ) {
			nameLe->setEnabled( false );
			channelComb->setEnabled( false );
			begin->setEnabled( false );
			repeatBtn->setEnabled( false );
		}
	}
	switch ( timer.mode ) {
		case CronTimer::Noone : repeatLab->setText( i18n("None") ); break;
		case CronTimer::Daily : repeatLab->setText( i18n("Daily") ); break;
		case CronTimer::Weekly : repeatLab->setText( i18n("Weekly") ); break;
		case CronTimer::Monthly : repeatLab->setText( i18n("Monthly") ); break;
		default : repeatLab->setText( i18n("Custom") );
	}

	KIconLoader *icon = new KIconLoader();
	cancelBtn->setGuiItem( KStdGuiItem::cancel() );
	okBtn->setGuiItem( KStdGuiItem::ok() );
	repeatBtn->setGuiItem( KGuiItem(i18n("Repeat..."), icon->loadIconSet("reload", KIcon::Small) ) );

	setCaption( i18n("Timer Editor") );

	connect( okBtn, SIGNAL(clicked()), this, SLOT(accept()) );
	connect( cancelBtn, SIGNAL(clicked()), this, SLOT(reject()) );
	connect( repeatBtn, SIGNAL(clicked()), this, SLOT(setRepeat()) );
	connect( begin, SIGNAL(valueChanged(const QDateTime&)), this, SLOT(setMaxEnd(const QDateTime&)) );
	connect( end, SIGNAL(valueChanged(const QDateTime&)), this, SLOT(setDuration(const QDateTime&)) );
	connect( duration, SIGNAL(valueChanged(const QTime&)), this, SLOT(setEnd(const QTime&)) );
	setMaxEnd( begin->dateTime() );
        delete icon;
}



void KTimerEditor::setMaxEnd( const QDateTime &dt )
{
	QDateTime max = dt.addSecs( 23*3600+59*60+59 );
	end->dateEdit()->setMinValue( dt.date() );
	end->dateEdit()->setMaxValue( max.date() );
	setEnd( duration->time() );
}



void KTimerEditor::setDuration( const QDateTime &dt )
{
	disconnect( duration, SIGNAL(valueChanged(const QTime&)), this, SLOT(setEnd(const QTime&)) );
	duration->setTime( QTime().addSecs( begin->dateTime().secsTo( dt ) ) );
	connect( duration, SIGNAL(valueChanged(const QTime&)), this, SLOT(setEnd(const QTime&)) );
}



void KTimerEditor::setEnd( const QTime &t )
{
	disconnect( end, SIGNAL(valueChanged(const QDateTime&)), this, SLOT(setDuration(const QDateTime&)) );
	end->setDateTime( begin->dateTime().addSecs( QTime().secsTo( t ) ) );
	connect( end, SIGNAL(valueChanged(const QDateTime&)), this, SLOT(setDuration(const QDateTime&)) );
}



void KTimerEditor::setRepeat()
{
	CronTimer dlg( timer.mode, this );

	if ( dlg.exec()==CronTimer::Accepted )
		timer.mode = dlg.getMode();
	switch ( timer.mode ) {
		case CronTimer::Noone : repeatLab->setText( i18n("None") ); break;
		case CronTimer::Daily : repeatLab->setText( i18n("Daily") ); break;
		case CronTimer::Weekly : repeatLab->setText( i18n("Weekly") ); break;
		case CronTimer::Monthly : repeatLab->setText( i18n("Monthly") ); break;
		default : repeatLab->setText( i18n("Custom") );
	}
}



void KTimerEditor::accept()
{
	if ( nameLe->text().stripWhiteSpace().isEmpty() ) {
		KMessageBox::sorry( this, i18n("You must give it a name!") );
		nameLe->setFocus();
		return;
	}

	if ( nameLe->text().stripWhiteSpace().contains("/") )
		goto stop;
	if ( nameLe->text().stripWhiteSpace().contains(">") )
		goto stop;
	if ( nameLe->text().stripWhiteSpace().contains("<") )
		goto stop;
	if ( nameLe->text().stripWhiteSpace().contains("\\") )
		goto stop;
	if ( nameLe->text().stripWhiteSpace().contains(":") )
		goto stop;
	if ( nameLe->text().stripWhiteSpace().contains("\"") )
		goto stop;
	if ( nameLe->text().stripWhiteSpace().contains("|") )
		goto stop;

	if ( duration->time()<QTime(0,1) ) {
		KMessageBox::sorry( this, i18n("Duration must be at least 1 minute!") );
		duration->setFocus();
		return;
	}

	timer.duration = duration->time();
	if ( timer.running )
		done( Accepted );

	timer.name = nameLe->text().stripWhiteSpace();
	timer.channel = channelComb->currentText();
	timer.begin = begin->dateTime();
	done( Accepted );
	return;

stop:
	KMessageBox::sorry( this, i18n("Name must not contain any of the following characters: > < \\ / : \" |") );
	nameLe->setFocus();
	return;
}



KTimerEditor::~KTimerEditor()
{
}

#include "ktimereditor.moc"
