/*  This file is part of the KDE project
    Copyright (C) 2002-2003 Matthias Kretz <kretz@kde.org>

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

#include "kviewconfmodules.h"

#include <qlayout.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>

#include <klocale.h>
#include <kdialog.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kgenericfactory.h>
#include <qwhatsthis.h>

typedef KGenericFactory<KViewGeneralConfig, QWidget> KViewGeneralConfigFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_kviewgeneralconfig, KViewGeneralConfigFactory( "kcm_kviewgeneralconfig" ) )

KViewGeneralConfig::KViewGeneralConfig( QWidget * parent, const char * /*name*/, const QStringList & args )
	: KCModule( KViewGeneralConfigFactory::instance(), parent, args )
{
	QBoxLayout * layout = new QVBoxLayout( this, KDialog::marginHint(), KDialog::spacingHint() );

	m_pResizeGroup = new QVButtonGroup( i18n( "Resizing" ), this );
	m_pResizeGroup->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );
	connect( m_pResizeGroup, SIGNAL( clicked( int ) ), this, SLOT( resizeChanged( int ) ) );
	layout->addWidget( m_pResizeGroup );

	( void )new QRadioButton( i18n( "Only resize window" ), m_pResizeGroup );
	( void )new QRadioButton( i18n( "Resize image to fit window" ), m_pResizeGroup );
	( void )new QRadioButton( i18n( "Don't resize anything" ), m_pResizeGroup );
	QWhatsThis::add( new QRadioButton( i18n( "Best fit" ), m_pResizeGroup ),
			i18n( "<p>KView will resize the window to fit the image. The image will never be scaled up but if it is too large for the screen the image will be scaled down.</p>" ) );

	load();
}

KViewGeneralConfig::~KViewGeneralConfig()
{
}

void KViewGeneralConfig::save()
{
	KConfigGroup cfgGroup( KGlobal::config(), "KView General" );
	m_savedPosition = m_pResizeGroup->id( m_pResizeGroup->selected() );
	cfgGroup.writeEntry( "Resize Mode", m_savedPosition );
	emit changed( false );
	KGlobal::config()->sync();
}

void KViewGeneralConfig::load()
{
	KConfigGroup cfgGroup( KGlobal::config(), "KView General" );
	int m_savedPosition = cfgGroup.readNumEntry( "Resize Mode", 2 );
	if( m_savedPosition < 0 || m_savedPosition > 3 )
		m_savedPosition = 2;
	m_pResizeGroup->setButton( m_savedPosition );
	emit changed( false );
}

void KViewGeneralConfig::defaults()
{
	m_pResizeGroup->setButton( 2 );
	emit changed( ( 2 != m_savedPosition ) );
}

void KViewGeneralConfig::resizeChanged( int pos )
{
	emit changed( ( pos != m_savedPosition ) );
}

// vim:sw=4:ts=4

#include "kviewconfmodules.moc"
