/* This file is part of the KDE project
   Copyright (C) 2001 Carsten Pfeiffer <pfeiffer@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <dcopclient.h>

#include <kapplication.h>
#include <kaction.h>
#include <kconfig.h>
#include <kglobal.h>
#include <khtml_part.h>
#include <kinstance.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpopupmenu.h>
#include <kprotocolmanager.h>
#include <kgenericfactory.h>
#include <kaboutdata.h>

#include "settingsplugin.h"

typedef KGenericFactory<SettingsPlugin> SettingsPluginFactory;
static const KAboutData aboutdata("khtmlsettingsplugin", I18N_NOOP("HTML Settings") , "1.0" );
K_EXPORT_COMPONENT_FACTORY( libkhtmlsettingsplugin,
                            SettingsPluginFactory( &aboutdata ) )

SettingsPlugin::SettingsPlugin( QObject* parent, const char* name,
                                const QStringList & )
    : KParts::Plugin( parent, name ), mConfig(0)
{

    setInstance(SettingsPluginFactory::instance());

    if ( !kapp->dcopClient()->isAttached() )
	kapp->dcopClient()->attach();

    KActionMenu *menu = new KActionMenu( i18n("HTML Settings"), "configure",
					 actionCollection(), "action menu" );
    menu->setDelayed( false );

    KToggleAction *action;


    action = new KToggleAction( i18n("Java&Script"), 0,
				this, SLOT(toggleJavascript()),
				actionCollection(), "javascript" );
    menu->insert( action );

    action = new KToggleAction( i18n("&Java"), 0,
				this, SLOT(toggleJava()),
				actionCollection(), "java" );
    menu->insert( action );

    action = new KToggleAction( i18n("&Cookies"), 0,
				this, SLOT(toggleCookies()),
				actionCollection(), "cookies" );
    menu->insert( action );

    action = new KToggleAction( i18n("&Plugins"), 0,
				this, SLOT(togglePlugins()),
				actionCollection(), "plugins" );
    menu->insert( action );

    action = new KToggleAction( i18n("Autoload &Images"), 0,
				this, SLOT(toggleImageLoading()),
				actionCollection(), "imageloading" );
    menu->insert( action );

    menu->insert( new KActionSeparator(actionCollection()) );

    action = new KToggleAction( i18n("Enable Pro&xy"), 0,
				this, SLOT(toggleProxy()),
				actionCollection(), "useproxy" );
    action->setCheckedState(i18n("Disable Pro&xy"));
    menu->insert( action );

    action = new KToggleAction( i18n("Enable Cac&he"), 0,
				this, SLOT(toggleCache()),
				actionCollection(), "usecache" );
    action->setCheckedState(i18n("Disable Cac&he"));
    menu->insert( action );


    KSelectAction *sAction = new KSelectAction( i18n("Cache Po&licy"), 0,
						0, 0, actionCollection(),
                                                "cachepolicy" );
    QStringList policies;
    policies += i18n( "&Keep Cache in Sync" );
    policies += i18n( "&Use Cache if Possible" );
    policies += i18n( "&Offline Browsing Mode" );
    sAction->setItems( policies );
    connect( sAction, SIGNAL( activated( int ) ), SLOT( cachePolicyChanged(int) ) );

    menu->insert( sAction );

    connect( menu->popupMenu(), SIGNAL( aboutToShow() ), SLOT( showPopup() ));
}

SettingsPlugin::~SettingsPlugin()
{
  delete mConfig;
}

void SettingsPlugin::showPopup()
{
  if( !parent() || !parent()->inherits("KHTMLPart"))
    return;

  if (!mConfig)
    mConfig = new KConfig("settingspluginrc", false, false);
    
  KHTMLPart *part = static_cast<KHTMLPart *>( parent() );

  KProtocolManager::reparseConfiguration();
  bool cookies = cookiesEnabled( part->url().url() );

  ((KToggleAction*)actionCollection()->action("useproxy"))->setChecked(KProtocolManager::useProxy());
  ((KToggleAction*)actionCollection()->action("java"))->setChecked( part->javaEnabled() );
  ((KToggleAction*)actionCollection()->action("javascript"))->setChecked( part->jScriptEnabled() );
  ((KToggleAction*)actionCollection()->action("cookies"))->setChecked( cookies );
  ((KToggleAction*)actionCollection()->action("plugins"))->setChecked( part->pluginsEnabled() );
  ((KToggleAction*)actionCollection()->action("imageloading"))->setChecked( part->autoloadImages() );
  ((KToggleAction*)actionCollection()->action("usecache"))->setChecked(KProtocolManager::useCache());

  KIO::CacheControl cc = KProtocolManager::cacheControl();
  switch ( cc ) 
  {
      case KIO::CC_Verify:
          ((KSelectAction*)actionCollection()->action("cachepolicy"))->setCurrentItem( 0 );
          break;
      case KIO::CC_CacheOnly:
          ((KSelectAction*)actionCollection()->action("cachepolicy"))->setCurrentItem( 2 );
          break;
      case KIO::CC_Cache:
          ((KSelectAction*)actionCollection()->action("cachepolicy"))->setCurrentItem( 1 );
          break;
      case KIO::CC_Reload: // nothing for now
      case KIO::CC_Refresh:
      default:
          break;
      
  }
}

void SettingsPlugin::toggleJava()
{
  if( parent() && parent()->inherits("KHTMLPart"))
  {
      KHTMLPart *part = static_cast<KHTMLPart *>(parent());
      part->setJavaEnabled( ((KToggleAction*)actionCollection()->action("java"))->isChecked() );
  }
}

void SettingsPlugin::toggleJavascript()
{
  if( parent() && parent()->inherits("KHTMLPart"))
  {
      KHTMLPart *part = static_cast<KHTMLPart *>(parent());
      part->setJScriptEnabled( ((KToggleAction*)actionCollection()->action("javascript"))->isChecked() );
  }
}

void SettingsPlugin::toggleCookies()
{
  if( !parent() || !parent()->inherits("KHTMLPart"))
    return;
    
  KHTMLPart *part = static_cast<KHTMLPart *>( parent() );

  QString advice;
  bool enable = ((KToggleAction*)actionCollection()->action("cookies"))->isChecked();
  advice = enable ? "Accept" : "Reject";

  QCString replyType;
  QByteArray data, replyData;
  QDataStream stream( data, IO_WriteOnly );
  stream << part->url().url() << advice;
  bool ok = kapp->dcopClient()->call( "kded", "kcookiejar",
        "setDomainAdvice(QString,QString)",
        data, replyType, replyData, true );

  if ( !ok )
     KMessageBox::sorry( part->widget(),
          i18n("I can't enable cookies, because the "
                "cookie daemon could not be started."),
          i18n("Cookies Disabled"));
}

void SettingsPlugin::togglePlugins()
{
  if( parent() && parent()->inherits("KHTMLPart"))
  {
      KHTMLPart *part = static_cast<KHTMLPart *>(parent());
      part->setPluginsEnabled( ((KToggleAction*)actionCollection()->action("plugins"))->isChecked() );
  }
}

void SettingsPlugin::toggleImageLoading()
{
  if( parent() && parent()->inherits("KHTMLPart"))
  {
      KHTMLPart *part = static_cast<KHTMLPart *>(parent());
      part->setAutoloadImages( ((KToggleAction*)actionCollection()->action("imageloading"))->isChecked() );
  }
}

bool SettingsPlugin::cookiesEnabled( const QString& url )
{
    QByteArray data, reply;
    QCString replyType;
    QDataStream stream( data, IO_WriteOnly );
    stream << url;
    kapp->dcopClient()->call( "kcookiejar", "kcookiejar", "getDomainAdvice(QString)", data, replyType, reply, true );

    bool enabled = false;

    if ( replyType == "QString" ) 
    {
        QString advice;
        QDataStream s( reply, IO_ReadOnly );
        s >> advice;
        enabled = ( advice == "Accept" );
        if ( !enabled && advice == "Dunno" ) {
            // TODO, check the global setting via dcop
            KConfig kc( "kcookiejarrc", true, false );
            kc.setGroup( "Cookie Policy" );
            enabled =
          (kc.readEntry( "CookieGlobalAdvice", "Reject" ) == "Accept");
        }
    }

    return enabled;
}


//
// sync with kcontrol/kio/ksaveioconfig.* !
//

void SettingsPlugin::toggleProxy()
{
    bool enable = ((KToggleAction*)actionCollection()->action("useproxy"))->isChecked();   
    
    int type;
    
    if( enable )
      type = mConfig->readNumEntry( "SavedProxyType", KProtocolManager::ManualProxy );
    else
    {
      mConfig->writeEntry( "SavedProxyType", KProtocolManager::proxyType() );
      type = KProtocolManager::NoProxy;
    }
    
    KConfig config("kioslaverc", false, false);
    config.setGroup( "Proxy Settings" );    
    config.writeEntry( "ProxyType", type );

    ((KToggleAction*)actionCollection()->action("useproxy"))->setChecked(enable);
    updateIOSlaves();
}


void SettingsPlugin::toggleCache()
{
    bool usesCache = KProtocolManager::useCache();
    KConfig config( "kio_httprc", false, false );
    config.writeEntry( "UseCache", !usesCache );

    ((KToggleAction*)actionCollection()->action("usecache"))->setChecked( !usesCache );

    updateIOSlaves();
}

void SettingsPlugin::cachePolicyChanged( int p )
{
    QString policy;

    switch ( p ) {
        case 0:
            policy = KIO::getCacheControlString( KIO::CC_Verify );
            break;
        case 1:
            policy = KIO::getCacheControlString( KIO::CC_Cache );
            break;
        case 2:
            policy = KIO::getCacheControlString( KIO::CC_CacheOnly );
            break;
    };

    if ( !policy.isEmpty() ) {
        KConfig config("kio_httprc", false, false);
        config.writeEntry("cache", policy);

        updateIOSlaves();
    }
}

void SettingsPlugin::updateIOSlaves()
{
    QByteArray data;
    QDataStream stream( data, IO_WriteOnly );

    DCOPClient* client = kapp->dcopClient();
    if ( !client->isAttached() )
        client->attach();

    QString protocol; // null -> all of them
    stream << protocol;
    client->send( "*", "KIO::Scheduler",
                  "reparseSlaveConfiguration(QString)", data );
}

#include "settingsplugin.moc"
