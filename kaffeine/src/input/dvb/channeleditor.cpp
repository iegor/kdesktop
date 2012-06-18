/*
 * channeleditor.cpp
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

#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include <klocale.h>
#include <kmessagebox.h>

#include "channeleditor.h"
#include "audioeditor.h"
#include "subeditor.h"

ChannelEditor::ChannelEditor( QStringList src, bool m, ChannelDesc *chan, QPtrList<ChannelDesc> *cdesc, QWidget *parent ) : ChannelEditorUI( parent )
{
	int i;
	channel = chan;
	mode = m;
	chandesc = cdesc;
	orgName = channel->name;

	sourceComb->insertStringList( src );
	for ( i=0; i<(int)sourceComb->count(); i++ ) {
		if ( sourceComb->text(i)==channel->tp.source ) {
			sourceComb->setCurrentItem(i);
			break;
		}
	}

	if ( channel->tp.type==FE_QAM ) initC();
	else if ( channel->tp.type==FE_OFDM ) initT();
	else if ( channel->tp.type==FE_QPSK ) initS();
	else initA();

	if ( mode ) {
		ftaCb->setEnabled( false );
		pidsGroup->setEnabled( false );
		numSpin->setEnabled( false );
		setCaption( i18n("Initial Transponder Settings") );
	}
	else {
		numSpin->setMinValue( 1 );
		numSpin->setMaxValue( chandesc->count() );
		chanNum = channel->num;
		numSpin->setValue( channel->num );
		sidSpin->setValue( channel->sid );
		vpidSpin->setValue( channel->vpid );
		ttpidSpin->setValue( channel->ttpid );
		tsidSpin->setValue( channel->tp.tsid );
		nidSpin->setValue( channel->tp.nid );
		ftaCb->setChecked( channel->fta );
	}
	nameLe->setText( channel->name );

	connect( apidBtn, SIGNAL(clicked()), this, SLOT(editAudio()) );
	connect( subpidBtn, SIGNAL(clicked()), this, SLOT(editSubtitle()) );
}

void ChannelEditor::editAudio()
{
	AudioEditor dlg( channel, this );

	dlg.exec();
}

void ChannelEditor::editSubtitle()
{
	SubEditor dlg( channel, this );

	dlg.exec();
}

void ChannelEditor::accept()
{
	int ret, i;
	QString name;

	name = nameLe->text().stripWhiteSpace();
	if ( name.isEmpty() ) {
		KMessageBox::sorry( this, i18n("You must give it a name!") );
		return;
	}

	if ( !mode ) {
		for ( ret=0; ret<(int)chandesc->count(); ret++ ) {
			if ( (channel->name==chandesc->at(ret)->name) && (channel->name!=orgName) ) {
				KMessageBox::sorry( this, i18n("This name is not unique.") );
				return;
			}
		}
		if ( ftaCb->isChecked() ) channel->fta = 1;
		else channel->fta = 0;
		channel->vpid = vpidSpin->value();
		channel->sid = sidSpin->value();
		channel->ttpid = ttpidSpin->value();
		channel->tp.tsid = tsidSpin->value();
		channel->tp.nid = nidSpin->value();
		if ( !channel->napid ) {
			KMessageBox::sorry( this, i18n("Missing audio pid(s)!") );
			return;
		}
		if ( numSpin->value()!=chanNum ) {
			for ( i=0; i<(int)chandesc->count(); i++ ) {
				if ( (int)chandesc->at(i)->num==numSpin->value() ) {
					chandesc->at(i)->num = chanNum;
					break;
				}
			}
		}
		channel->num = numSpin->value();
		if ( channel->vpid ) channel->type = 1;
		else channel->type = 2;
	}
	channel->name = name;
	channel->tp.source = sourceComb->currentText();
	if ( channel->tp.type==FE_QPSK ) {
		channel->tp.freq = freqSpin->value();
		channel->tp.sr = srSpin->value();
		if ( verticalRadio->isChecked() ) channel->tp.pol = 'v';
		else channel->tp.pol = 'h';
		channel->tp.coderateH = (fe_code_rate_t)(FEC_NONE+coderateHComb->currentItem());
		channel->tp.inversion = (fe_spectral_inversion_t)(INVERSION_OFF+inversionComb->currentItem());
		channel->tp.modulation = (fe_modulation_t)(QPSK+modulationComb->currentItem());
		channel->tp.S2 = stypeComb->currentItem();
		channel->tp.rolloff = (fe_rolloff_t)(ROLLOFF_35+rolloffComb->currentItem() );
	}
	else if ( channel->tp.type==FE_QAM ) {
		channel->tp.freq = freqSpin->value();
		channel->tp.sr = srSpin->value();
		channel->tp.coderateH = (fe_code_rate_t)(FEC_NONE+coderateHComb->currentItem());
		channel->tp.inversion = (fe_spectral_inversion_t)(INVERSION_OFF+inversionComb->currentItem());
		channel->tp.modulation = (fe_modulation_t)(QPSK+modulationComb->currentItem());
	}
	else if ( channel->tp.type==FE_OFDM ) {
		channel->tp.freq = freqSpin->value();
		channel->tp.inversion = (fe_spectral_inversion_t)(INVERSION_OFF+inversionComb->currentItem());
		channel->tp.coderateH = (fe_code_rate_t)(FEC_NONE+coderateHComb->currentItem());
		channel->tp.coderateL = (fe_code_rate_t)(FEC_NONE+coderateLComb->currentItem());
		channel->tp.modulation = (fe_modulation_t)(QPSK+modulationComb->currentItem());
		channel->tp.transmission = (fe_transmit_mode_t)(TRANSMISSION_MODE_2K+transmissionComb->currentItem());
		channel->tp.bandwidth = (fe_bandwidth_t)(BANDWIDTH_8_MHZ+bandwidthComb->currentItem());
		channel->tp.hierarchy = (fe_hierarchy_t)(HIERARCHY_NONE+hierarchyComb->currentItem());
		channel->tp.guard = (fe_guard_interval_t)(GUARD_INTERVAL_1_32+guardComb->currentItem());
	}
	else {
		channel->tp.freq = freqSpin->value();
		channel->tp.inversion = (fe_spectral_inversion_t)(INVERSION_OFF+inversionComb->currentItem());
		channel->tp.modulation = (fe_modulation_t)(QPSK+modulationComb->currentItem());
	}

	done( Accepted );
}

void ChannelEditor::initS()
{
	freqSpin->setValue( channel->tp.freq );
	srSpin->setValue( channel->tp.sr );
	if ( channel->tp.pol=='v' ) polGroup->setButton( 0 ) ;
	else polGroup->setButton( 1 ) ;
	inversionComb->insertStringList( inversionList() );
	inversionComb->setCurrentItem( INVERSION_OFF+channel->tp.inversion );
	coderateHComb->insertStringList( coderateList() );
	coderateHComb->setCurrentItem( FEC_NONE+channel->tp.coderateH );
	modulationComb->insertStringList( modulationList() );
	modulationComb->setCurrentItem( QPSK+channel->tp.modulation );
	stypeComb->insertStringList( stypeList() );
	stypeComb->setCurrentItem( channel->tp.S2 );
	rolloffComb->insertStringList( rolloffList() );
	rolloffComb->setCurrentItem( ROLLOFF_35+channel->tp.rolloff );
	transmissionComb->setEnabled( false );
	coderateLComb->setEnabled( false );
	bandwidthComb->setEnabled( false );
	hierarchyComb->setEnabled( false );
	guardComb->setEnabled( false );
}

void ChannelEditor::initC()
{
	freqSpin->setValue( channel->tp.freq );
	srSpin->setValue( channel->tp.sr );
	inversionComb->insertStringList( inversionList() );
	inversionComb->setCurrentItem( INVERSION_OFF+channel->tp.inversion );
	coderateHComb->insertStringList( coderateList() );
	coderateHComb->setCurrentItem( FEC_NONE+channel->tp.coderateH );
	modulationComb->insertStringList( modulationList() );
	modulationComb->setCurrentItem( QPSK+channel->tp.modulation );
	polGroup->setEnabled( false );
	transmissionComb->setEnabled( false );
	coderateLComb->setEnabled( false );
	bandwidthComb->setEnabled( false );
	hierarchyComb->setEnabled( false );
	guardComb->setEnabled( false );
	stypeComb->setEnabled( false );
	rolloffComb->setEnabled( false );
}

void ChannelEditor::initT()
{
	freqSpin->setValue( channel->tp.freq );
	inversionComb->insertStringList( inversionList() );
	inversionComb->setCurrentItem( INVERSION_OFF+channel->tp.inversion );
	coderateHComb->insertStringList( coderateList() );
	coderateHComb->setCurrentItem( FEC_NONE+channel->tp.coderateH );
	coderateLComb->insertStringList( coderateList() );
	coderateLComb->setCurrentItem( FEC_NONE+channel->tp.coderateL );
	modulationComb->insertStringList( modulationList() );
	modulationComb->setCurrentItem( QPSK+channel->tp.modulation );
	transmissionComb->insertStringList( transmissionList() );
	transmissionComb->setCurrentItem( TRANSMISSION_MODE_2K+channel->tp.transmission );
	bandwidthComb->insertStringList( bandwidthList() );
	bandwidthComb->setCurrentItem( BANDWIDTH_8_MHZ+channel->tp.bandwidth );
	hierarchyComb->insertStringList( hierarchyList() );
	hierarchyComb->setCurrentItem( HIERARCHY_NONE+channel->tp.hierarchy );
	guardComb->insertStringList( guardList() );
	guardComb->setCurrentItem( GUARD_INTERVAL_1_32+channel->tp.guard );
	srSpin->setEnabled( false );
	polGroup->setEnabled( false );
	stypeComb->setEnabled( false );
	rolloffComb->setEnabled( false );
}

void ChannelEditor::initA()
{
	freqSpin->setValue( channel->tp.freq );
	inversionComb->insertStringList( inversionList() );
	inversionComb->setCurrentItem( INVERSION_OFF+channel->tp.inversion );
	modulationComb->insertStringList( modulationList() );
	modulationComb->setCurrentItem( QPSK+channel->tp.modulation );
	srSpin->setEnabled( false );
	polGroup->setEnabled( false );
	transmissionComb->setEnabled( false );
	coderateHComb->setEnabled( false );
	coderateLComb->setEnabled( false );
	bandwidthComb->setEnabled( false );
	hierarchyComb->setEnabled( false );
	guardComb->setEnabled( false );
	stypeComb->setEnabled( false );
	rolloffComb->setEnabled( false );
}

QStringList ChannelEditor::inversionList()
{
	QStringList list;

	list<<"OFF"<<"ON"<<"AUTO";
	return list;
}

QStringList ChannelEditor::coderateList()
{
	QStringList list;

	list<<"NONE"<<"1/2"<<"2/3"<<"3/4"<<"4/5"<<"5/6"<<"6/7"<<"7/8"<<"8/9"<<"AUTO"<<"3/5"<<"9/10";
	return list;
}

QStringList ChannelEditor::modulationList()
{
	QStringList list;

	list<<"QPSK"<<"QAM 16"<<"QAM 32"<<"QAM 64"<<"QAM 128"<<"QAM 256"<<"AUTO"<<"VSB-8"<<"VSB-16"<<"8PSK"<<"16APSK"<<"DQPSK";
	return list;
}

QStringList ChannelEditor::transmissionList()
{
	QStringList list;

	list<<"2K"<<"8K"<<"AUTO";
	return list;
}

QStringList ChannelEditor::bandwidthList()
{
	QStringList list;

	list<<"8 MHz"<<"7 MHz"<<"6 MHz"<<"AUTO";
	return list;
}

QStringList ChannelEditor::hierarchyList()
{
	QStringList list;

	list<<"NONE"<<"1"<<"2"<<"4"<<"AUTO";
	return list;
}

QStringList ChannelEditor::guardList()
{
	QStringList list;

	list<<"1/32"<<"1/16"<<"1/8"<<"1/4"<<"AUTO";
	return list;
}

QStringList ChannelEditor::stypeList()
{
	QStringList list;

	list<<"DVB-S"<<"DVB-S2";
	return list;
}

QStringList ChannelEditor::rolloffList()
{
	QStringList list;

	list<<"35"<<"20"<<"25"<<"AUTO";
	return list;
}

ChannelEditor::~ChannelEditor()
{
}

#include "channeleditor.moc"
