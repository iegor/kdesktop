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

#include "imagesettings.h"

#include <qcheckbox.h>
#include <qlayout.h>

#include <klocale.h>
#include <kdialog.h>

ImageSettings::ImageSettings( QWidget * parent, const char * name )
	: KPrintDialogPage( parent, name )
	, m_pFitImage( 0 )
{
	setTitle( i18n( "Image Settings" ) );

	QBoxLayout * layout = new QVBoxLayout( this, KDialog::marginHint(), KDialog::spacingHint() );
	m_pFitImage = new QCheckBox( i18n( "Fit image to page size" ), this );
	m_pFitImage->setChecked( true );
	layout->addWidget( m_pFitImage );
	m_pCenter = new QCheckBox( i18n( "Center image on page" ), this );
	m_pCenter->setChecked( true );
	layout->addWidget( m_pCenter );
	layout->insertStretch( -1, 0 );
}

ImageSettings::~ImageSettings()
{
}

void ImageSettings::setOptions( const QMap<QString, QString> & opts )
{
	m_pFitImage->setChecked( opts[ "app-kviewviewer-fitimage" ] == "1" );
	m_pCenter->setChecked( opts[ "app-kviewviewer-center" ] == "1" );
}

void ImageSettings::getOptions( QMap<QString, QString> & opts, bool include_def )
{
	if( m_pFitImage->isChecked() )
		opts[ "app-kviewviewer-fitimage" ] = "1";
	else if( include_def )
		opts[ "app-kviewviewer-fitimage" ] = "0";

	if( m_pCenter->isChecked() )
		opts[ "app-kviewviewer-center" ] = "1";
	else if( include_def )
		opts[ "app-kviewviewer-center" ] = "0";
}

bool ImageSettings::isValid( QString & /*msg*/ )
{
	return true;
}

#include "imagesettings.moc"

// vim:sw=4:ts=4
