/*
 *
 * KEduca - Interactive Tests and Exams for KDE
 *
 * Copyright (c)
 *
 * 2004: Henrique Pinto <henrique.pinto@kdemail.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
 
#include "keduca_part.h"
#include "keducaview.h"
#include "settings.h"
#include "configdialogbase.h"

#include <kaboutdata.h>
#include <kparts/genericfactory.h>
#include <kstdaction.h>
#include <kconfigdialog.h>

typedef KParts::GenericFactory<KEducaPart> KEducaPartFactory;
K_EXPORT_COMPONENT_FACTORY( libkeducapart, KEducaPartFactory )

KEducaPart::KEducaPart( QWidget *parentWidget, const char *widgetName,
                        QObject *parent, const char *name,
                        const QStringList &/*args*/ )
	: KParts::ReadOnlyPart( parent, name )
{
	setInstance( KEducaPartFactory::instance() );
	
	m_view = new KEducaView( parentWidget, widgetName );
	m_view->show();
	setWidget( m_view );
	
	setupActions();
	
	setXMLFile( "keduca_partui.rc" );
}

KEducaPart::~KEducaPart()
{
}

KAboutData* KEducaPart::createAboutData()
{
	KAboutData *data = new KAboutData( "keduca", I18N_NOOP( "KEducaPart" ),
	                                   "0.2", I18N_NOOP( "KEduca KParts Component" ),
	                                   KAboutData::License_GPL,
	                                   "(c) 2004, Henrique Pinto <henrique.pinto@kdemail.net>"
	                                 );
	return data;
}

bool KEducaPart::openFile()
{
	return m_view->openURL( KURL( m_file ) );
}

void KEducaPart::setupActions()
{
	KStdAction::preferences( this, SLOT( slotConfigure() ), actionCollection(), "configure" );
}

void KEducaPart::slotConfigure()
{
	if ( KConfigDialog::showDialog( "KEducaSettings" ) )
		return;
		
	KConfigDialog *dialog = new KConfigDialog( widget(), "KEducaSettings", Settings::self() );
	ConfigDialogBase *confDlg = new ConfigDialogBase( 0, "ConfigDialogBase" );
	dialog->addPage( confDlg, i18n("General"), "keduca" );
	
	connect( dialog, SIGNAL( settingsChanged() ), SLOT( updateConfiguration() ) );
	
	dialog->show();
}

void KEducaPart::updateConfiguration()
{
	Settings::writeConfig();
}

#include "keduca_part.moc"
