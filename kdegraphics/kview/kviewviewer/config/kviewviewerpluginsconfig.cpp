/*  This file is part of the KDE project
    Copyright (C) 2003 Matthias Kretz <kretz@kde.org>

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

#include "kviewviewerpluginsconfig.h"

#include <kpluginselector.h>
#include <kgenericfactory.h>
#include <klocale.h>
#include <kglobal.h>
#include <ksimpleconfig.h>
#include <kplugininfo.h>

#include <qstring.h>

typedef KGenericFactory<KViewViewerPluginsConfig, QWidget> KViewViewerPluginsConfigFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_kviewviewerpluginsconfig, KViewViewerPluginsConfigFactory( "kcm_kviewviewerpluginsconfig" ) )

KViewViewerPluginsConfig::KViewViewerPluginsConfig( QWidget * parent, const char *, const QStringList & args )
	: KSettings::PluginPage( KViewViewerPluginsConfigFactory::instance(), parent, args )
{
	m_config = new KSimpleConfig( "kviewviewerrc" );
	pluginSelector()->addPlugins( QString::fromAscii( "kviewviewer" ), i18n( "Viewer" ), QString::null, m_config );
	pluginSelector()->setShowEmptyConfigPage( false );
}

KViewViewerPluginsConfig::~KViewViewerPluginsConfig()
{
	delete m_config;
}

// vim: sw=4 ts=4 noet

#include "kviewviewerpluginsconfig.moc"
