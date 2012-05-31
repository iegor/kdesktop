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

#include "confmodules.h"
#include "generalconfigwidget.h"
#include "defaults.h"

#include <qlayout.h>
#include <qcheckbox.h>
#include <qframe.h>

#include <klocale.h>
#include <ksimpleconfig.h>
#include <kcolorbutton.h>
#include <klistview.h>
#include <knuminput.h>
#include <kgenericfactory.h>

typedef KGenericFactory<KViewCanvasConfig, QWidget> KViewCanvasConfigFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_kviewcanvasconfig, KViewCanvasConfigFactory( "kcm_kviewcanvasconfig" ) )

KViewCanvasConfig::KViewCanvasConfig( QWidget * parent, const char *, const QStringList & args )
	: KCModule( KViewCanvasConfigFactory::instance(), parent, args )
	, m_config( new KConfig( "kviewcanvasrc" ) )
{
	QBoxLayout * layout = new QVBoxLayout( this );
	layout->setAutoAdd( true );

	m_pWidget = new GeneralConfigWidget( this );
	m_pWidget->m_pMinWidth ->setRange( 1, 200 );
	m_pWidget->m_pMinHeight->setRange( 1, 200 );
	m_pWidget->m_pMaxWidth ->setRange( 1, 10000 );
	m_pWidget->m_pMaxHeight->setRange( 1, 10000 );

	// clear m_items
	m_items.clear();

	for( unsigned int i = 1; i <= Defaults::numOfBlendEffects; ++i )
	{
		QCheckListItem * item = new QCheckListItem( m_pWidget->m_pListView, i18n( Defaults::blendEffectDescription[ i ] ), QCheckListItem::CheckBox );
		m_items.append( item );
	}

	connect( m_pWidget->m_pListView, SIGNAL( clicked( QListViewItem * ) ), this, SLOT( configChanged() ) );
	connect( m_pWidget->m_pListView, SIGNAL( spacePressed( QListViewItem * ) ), this, SLOT( configChanged() ) );

	connect( m_pWidget->m_pSmoothScaling, SIGNAL( toggled( bool ) ), this, SLOT( configChanged() ) );
	connect( m_pWidget->m_pKeepRatio, SIGNAL( toggled( bool ) ), this, SLOT( configChanged() ) );
	connect( m_pWidget->m_pCenterImage, SIGNAL( toggled( bool ) ), this, SLOT( configChanged() ) );
	connect( m_pWidget->m_bgColor, SIGNAL( changed( const QColor & ) ), this, SLOT( configChanged() ) );
	connect( m_pWidget->m_pMinWidth, SIGNAL( valueChanged( int ) ), this, SLOT( configChanged() ) );
	connect( m_pWidget->m_pMaxWidth, SIGNAL( valueChanged( int ) ), this, SLOT( configChanged() ) );
	connect( m_pWidget->m_pMinHeight, SIGNAL( valueChanged( int ) ), this, SLOT( configChanged() ) );
	connect( m_pWidget->m_pMaxHeight, SIGNAL( valueChanged( int ) ), this, SLOT( configChanged() ) );

	load();
}

KViewCanvasConfig::~KViewCanvasConfig()
{
}

void KViewCanvasConfig::save()
{
	KConfigGroup cfgGroup( m_config, "Settings" );
	cfgGroup.writeEntry( "Smooth Scaling", m_pWidget->m_pSmoothScaling->isChecked() );
	cfgGroup.writeEntry( "Keep Aspect Ratio", m_pWidget->m_pKeepRatio->isChecked() );
	cfgGroup.writeEntry( "Center Image", m_pWidget->m_pCenterImage->isChecked() );

	cfgGroup.writeEntry( "Background Color", m_pWidget->m_bgColor->color() );

	cfgGroup.writeEntry( "Minimum Width" , m_pWidget->m_pMinWidth->value() );
	cfgGroup.writeEntry( "Minimum Height", m_pWidget->m_pMinHeight->value() );
	cfgGroup.writeEntry( "Maximum Width" , m_pWidget->m_pMaxWidth->value() );
	cfgGroup.writeEntry( "Maximum Height", m_pWidget->m_pMaxHeight->value() );

	KConfigGroup cfgGroup2( m_config, "Blend Effects" );
	QCheckListItem *item = m_items.first();
	for( int i = 1; item; item = m_items.next(), ++i )
		cfgGroup2.writeEntry( QString::number( i ), item->isOn() );
	m_config->sync();
}

void KViewCanvasConfig::load()
{
	KConfigGroup cfgGroup( m_config, "Settings" );
	m_pWidget->m_pSmoothScaling->setChecked( cfgGroup.readBoolEntry( "Smooth Scaling", Defaults::smoothScaling ) );
	m_pWidget->m_pKeepRatio->setChecked( cfgGroup.readBoolEntry( "Keep Aspect Ratio", Defaults::keepAspectRatio ) );
	m_pWidget->m_pCenterImage->setChecked( cfgGroup.readBoolEntry( "Center Image", Defaults::centerImage ) );
	
	m_pWidget->m_bgColor->setColor( cfgGroup.readColorEntry( "Background Color", &Defaults::bgColor ) );

	m_pWidget->m_pMinWidth ->setValue( cfgGroup.readNumEntry( "Minimum Width" , Defaults::minSize.width() ) );
	m_pWidget->m_pMinHeight->setValue( cfgGroup.readNumEntry( "Minimum Height", Defaults::minSize.height() ) );
	m_pWidget->m_pMaxWidth ->setValue( cfgGroup.readNumEntry( "Maximum Width" , Defaults::maxSize.width() ) );
	m_pWidget->m_pMaxHeight->setValue( cfgGroup.readNumEntry( "Maximum Height", Defaults::maxSize.height() ) );

	KConfigGroup cfgGroup2( m_config, "Blend Effects" );
	QCheckListItem * item = m_items.first();
	for( int i = 1; item; item = m_items.next(), ++i )
		item->setOn( cfgGroup2.readBoolEntry( QString::number( i ), false ) );
}

void KViewCanvasConfig::defaults()
{
	m_pWidget->m_pSmoothScaling->setChecked( Defaults::smoothScaling );
	m_pWidget->m_pKeepRatio->setChecked( Defaults::keepAspectRatio );
	m_pWidget->m_pCenterImage->setChecked( Defaults::centerImage );
	
	m_pWidget->m_bgColor->setColor( Defaults::bgColor );

	m_pWidget->m_pMinWidth ->setValue( Defaults::minSize.width() );
	m_pWidget->m_pMinHeight->setValue( Defaults::minSize.height() );
	m_pWidget->m_pMaxWidth ->setValue( Defaults::maxSize.width() );
	m_pWidget->m_pMaxHeight->setValue( Defaults::maxSize.height() );

	QCheckListItem * item = m_items.first();
	for( int i = 1; item; item = m_items.next(), ++i )
		item->setOn( false );
	emit changed( true );
}

void KViewCanvasConfig::configChanged()
{
	emit changed( true );
}

// vim:sw=4:ts=4

#include "confmodules.moc"
