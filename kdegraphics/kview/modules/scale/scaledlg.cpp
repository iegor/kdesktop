/*  This file is part of the KDE project
    Copyright (C) 2002 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

// $Id: scaledlg.cpp 465369 2005-09-29 14:33:08Z mueller $

#include "scaledlg.h"

#include <qvbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qsize.h>

#include <kdebug.h>
#include <kcombobox.h>
#include <klocale.h>
#include <kdialog.h>
#include "kfloatspinbox.h"
#include <kglobal.h>

#define ONEINCHINMM 2.54

ScaleDlg::ScaleDlg( const QSize & origsize, QVBox * parent, const char * name )
	: QObject( parent, name )
	, m_origsize( origsize )
	, m_newsizeunit( 0 )
	, m_newsizeunit2( 0 )
	, m_resolutionunit( 0 )
	, m_newwidth( origsize.width() )
	, m_newheight( origsize.height() )
	, m_resx( 72 )
	, m_resy( 72 )
{
	QGroupBox * pixelgroup = new QGroupBox( i18n( "Pixel Dimensions" ), parent );
	QGroupBox * printgroup = new QGroupBox( i18n( "Print Size && Display Units" ), parent );

	QGridLayout * pixelgroupgrid = new QGridLayout( pixelgroup, 1, 1,
			KDialog::marginHint(), KDialog::spacingHint() );
	QGridLayout * printgroupgrid = new QGridLayout( printgroup, 1, 1,
			KDialog::marginHint(), KDialog::spacingHint() );

	QLabel * label;

	pixelgroupgrid->addRowSpacing( 0, KDialog::spacingHint() );

	label = new QLabel( i18n( "Original width:" ), pixelgroup );
    label->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
	pixelgroupgrid->addWidget( label, 1, 0 );
	label = new QLabel( i18n( "Height:" ), pixelgroup );
    label->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
	pixelgroupgrid->addWidget( label, 2, 0 );

	pixelgroupgrid->addRowSpacing( 3, KDialog::spacingHint() );

	label = new QLabel( i18n( "New width:" ), pixelgroup );
    label->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
	pixelgroupgrid->addWidget( label, 4, 0 );
	label = new QLabel( i18n( "Height:" ), pixelgroup );
    label->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
	pixelgroupgrid->addWidget( label, 5, 0 );

	pixelgroupgrid->addRowSpacing( 6, KDialog::spacingHint() );

	label = new QLabel( i18n( "Ratio X:" ), pixelgroup );
    label->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
	pixelgroupgrid->addWidget( label, 7, 0 );
	label = new QLabel( i18n( "Y:" ), pixelgroup );
    label->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
	pixelgroupgrid->addWidget( label, 8, 0 );

	printgroupgrid->addRowSpacing( 0, KDialog::spacingHint() );

	label = new QLabel( i18n( "New width:" ), printgroup );
    label->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
	printgroupgrid->addWidget( label, 1, 0 );
	label = new QLabel( i18n( "Height:" ), printgroup );
    label->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
	printgroupgrid->addWidget( label, 2, 0 );

	printgroupgrid->addRowSpacing( 3, KDialog::spacingHint() );

	label = new QLabel( i18n( "Resolution X:" ), printgroup );
    label->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
	printgroupgrid->addWidget( label, 4, 0 );
	label = new QLabel( i18n( "Y:" ), printgroup );
    label->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
	printgroupgrid->addWidget( label, 5, 0 );

	m_pOldWidth = new QLabel( QString::number( origsize.width() ), pixelgroup );
    m_pOldWidth->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
	pixelgroupgrid->addWidget( m_pOldWidth, 1, 1 );
	m_pOldHeight = new QLabel( QString::number( origsize.height() ), pixelgroup );
    m_pOldHeight->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
	pixelgroupgrid->addWidget( m_pOldHeight, 2, 1 );

	m_pNewWidth = new KFloatSpinBox( 1.0, 100000.0, 10.0, 0, pixelgroup );
	pixelgroupgrid->addWidget( m_pNewWidth, 4, 1 );
	m_pNewHeight = new KFloatSpinBox( 1.0, 100000.0, 10.0, 0, pixelgroup );
	pixelgroupgrid->addWidget( m_pNewHeight, 5, 1 );

	m_pNewSizeUnit = new KComboBox( pixelgroup );
	m_pNewSizeUnit->insertItem( i18n( "px" ) );
	m_pNewSizeUnit->insertItem( i18n( "%" ) );
	pixelgroupgrid->addMultiCellWidget( m_pNewSizeUnit, 4, 5, 2, 2, Qt::AlignVCenter );

	m_pRatioX = new KFloatSpinBox( 0.0001, 10000.0, 0.1, 4, pixelgroup );
	pixelgroupgrid->addWidget( m_pRatioX, 7, 1 );
	m_pRatioY = new KFloatSpinBox( 0.0001, 10000.0, 0.1, 4, pixelgroup );
	pixelgroupgrid->addWidget( m_pRatioY, 8, 1 );

	m_pLinkRatio = new QCheckBox( i18n( "Link" ), pixelgroup );
	pixelgroupgrid->addMultiCellWidget( m_pLinkRatio, 7, 8, 2, 2, Qt::AlignVCenter );

	m_pNewWidth2 = new KFloatSpinBox( 0.0001, 10000.0, 0.1, 4, printgroup );
	printgroupgrid->addWidget( m_pNewWidth2, 1, 1 );
	m_pNewHeight2 = new KFloatSpinBox( 0.0001, 10000.0, 0.1, 4, printgroup );
	printgroupgrid->addWidget( m_pNewHeight2, 2, 1 );

	m_pNewSizeUnit2 = new KComboBox( printgroup );
	m_pNewSizeUnit2->insertItem( i18n( "in" ) );
	m_pNewSizeUnit2->insertItem( i18n( "mm" ) );
	printgroupgrid->addMultiCellWidget( m_pNewSizeUnit2, 1, 2, 2, 2, Qt::AlignVCenter );

	m_pResolutionX = new KFloatSpinBox( 0.0001, 6000.0, 1.0, 4, printgroup );
	printgroupgrid->addWidget( m_pResolutionX, 4, 1 );
	m_pResolutionY = new KFloatSpinBox( 0.0001, 6000.0, 1.0, 4, printgroup );
	printgroupgrid->addWidget( m_pResolutionY, 5, 1 );

	m_pLinkResolution = new QCheckBox( i18n( "Link" ), printgroup );
	printgroupgrid->addMultiCellWidget( m_pLinkResolution, 4, 5, 2, 2, Qt::AlignVCenter );
	m_pResolutionUnit = new KComboBox( printgroup );
	m_pResolutionUnit->insertItem( i18n( "pixels/in" ) );
	m_pResolutionUnit->insertItem( i18n( "pixels/mm" ) );
	printgroupgrid->addMultiCellWidget( m_pResolutionUnit, 6, 6, 1, 2, Qt::AlignLeft );

	m_pNewWidth->setValue( m_origsize.width() );
	m_pNewHeight->setValue( m_origsize.height() );

	m_newsizeunit = 0;
	m_newsizeunit2 = 0;
	m_resolutionunit = 0;

	connect( m_pNewWidth, SIGNAL( valueChanged( float ) ), SLOT( slotNewWidth( float ) ) );
	connect( m_pNewHeight, SIGNAL( valueChanged( float ) ), SLOT( slotNewHeight( float ) ) );
	connect( m_pNewWidth2, SIGNAL( valueChanged( float ) ), SLOT( slotNewWidth2( float ) ) );
	connect( m_pNewHeight2, SIGNAL( valueChanged( float ) ), SLOT( slotNewHeight2( float ) ) );
	connect( m_pResolutionX, SIGNAL( valueChanged( float ) ), SLOT( slotResolutionX( float ) ) );
	connect( m_pResolutionY, SIGNAL( valueChanged( float ) ), SLOT( slotResolutionY( float ) ) );

	connect( m_pNewSizeUnit, SIGNAL( activated( int ) ), SLOT( slotChangeNewSizeUnit( int ) ) );
	connect( m_pNewSizeUnit2, SIGNAL( activated( int ) ), SLOT( slotChangeNewSizeUnit2( int ) ) );
	connect( m_pResolutionUnit, SIGNAL( activated( int ) ), SLOT( slotChangeResolutionUnit( int ) ) );
}

ScaleDlg::~ScaleDlg()
{
}

void ScaleDlg::slotNewWidth( float newval )
{
	float newwidth;
	switch( m_newsizeunit )
	{
		case 0: // Pixel
			newwidth = newval;
			break;
		case 1: // Percent
			newwidth = newval / 100.0 * m_origsize.width();
			break;
		default:
			kdError( 4630 ) << "unknown unit\n";
			break;
	}

	m_newwidth = newwidth;
	m_pNewWidth2->setValueBlocking( m_newwidth / m_resx / ( m_newsizeunit2 ? ONEINCHINMM : 1 ) );
	m_pRatioX->setValueBlocking( m_newwidth / m_origsize.width() );
	if( m_pLinkRatio->isChecked() )
	{
		m_newheight = m_newwidth / m_origsize.width() * m_origsize.height();
		m_pNewHeight->setValueBlocking( m_newheight * ( m_newsizeunit ? 100.0 / m_origsize.height() : 1 ) );
		m_pRatioY->setValueBlocking( m_newheight / m_origsize.height() );
		m_pNewHeight2->setValueBlocking( m_newheight / m_resy / ( m_newsizeunit2 ? ONEINCHINMM : 1 ) );
	}
}

void ScaleDlg::slotNewHeight( float newval )
{
}

void ScaleDlg::slotNewWidth2( float newval )
{
}

void ScaleDlg::slotNewHeight2( float newval )
{
}

void ScaleDlg::slotResolutionX( float newval )
{
}

void ScaleDlg::slotResolutionY( float newval )
{
}

void ScaleDlg::slotChangeNewSizeUnit( int index )
{
	if( m_newsizeunit == index )
		return;

	m_newsizeunit = index;

	switch( m_newsizeunit )
	{
		case 0:
			// Pixel
			m_pNewWidth->setRange( 1.0, 100000.0, 10.0, 0 );
			m_pNewHeight->setRange( 1.0, 100000.0, 10.0, 0 );
			m_pNewWidth->setValueBlocking( m_newwidth );
			m_pNewHeight->setValueBlocking( m_newheight );
			break;
		case 1:
			// Percent
			m_pNewWidth->setRange( 0.0001, 100000.0, 0.1, 4 );
			m_pNewHeight->setRange( 0.0001, 100000.0, 0.1, 4 );
			m_pNewWidth->setValueBlocking( m_newwidth * 100.0 / m_origsize.width() );
			m_pNewHeight->setValueBlocking( m_newheight * 100.0 / m_origsize.height() );
			break;
		default:
			kdError( 4630 ) << "change to unknown unit\n";
			break;
	}
}

void ScaleDlg::slotChangeNewSizeUnit2( int index )
{
	if( m_newsizeunit2 == index )
		return;

	m_newsizeunit2 = index;

	switch( m_newsizeunit2 )
	{
		case 0:
			// Inch
			m_pNewWidth2->setRange( 0.0001, 10000.0, 0.1, 4 );
			m_pNewHeight2->setRange( 0.0001, 10000.0, 0.1, 4 );
			m_pNewWidth2->setValueBlocking( m_newwidth / m_resx );
			m_pNewHeight2->setValueBlocking( m_newheight / m_resy );
			break;
		case 1:
			// Millimeter
			m_pNewWidth2->setRange( 0.0002, 25400.0, 0.1, 4 );
			m_pNewHeight2->setRange( 0.0002, 25400.0, 0.1, 4 );
			m_pNewWidth2->setValueBlocking( m_newwidth / m_resx / ONEINCHINMM );
			m_pNewHeight2->setValueBlocking( m_newheight / m_resy / ONEINCHINMM );
			break;
		default:
			kdError( 4630 ) << "change to unknown unit\n";
			break;
	}
}

void ScaleDlg::slotChangeResolutionUnit( int index )
{
	if( m_resolutionunit == index )
		return;

	m_resolutionunit = index;

	switch( m_resolutionunit )
	{
		case 0:
			// Pixels per Inch
			m_pResolutionX->setRange( 0.0002, 25400.0, 0.1, 4 );
			m_pResolutionY->setRange( 0.0002, 25400.0, 0.1, 4 );
			m_pResolutionX->setValueBlocking( m_resx );
			m_pResolutionY->setValueBlocking( m_resy );
			break;
		case 1:
			// Pixels per Millimeter
			m_pResolutionX->setRange( 0.0001, 10000.0, 0.1, 4 );
			m_pResolutionY->setRange( 0.0001, 10000.0, 0.1, 4 );
			m_pResolutionX->setValueBlocking( m_resx / ONEINCHINMM );
			m_pResolutionY->setValueBlocking( m_resy / ONEINCHINMM );
			break;
		default:
			kdError( 4630 ) << "change to unknown unit\n";
			break;
	}
}

#include "scaledlg.moc"
