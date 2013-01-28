/*
    Copyright (c) 2001 Dawit Alemayehu <adawit@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License (LGPL) as published by the Free Software Foundation;
    either version 2 of the License, or (at your option) any later
    version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <sys/utsname.h>

#include <qregexp.h>

#include <krun.h>
#include <kdebug.h>
#include <kaction.h>
#include <klocale.h>
#include <kglobal.h>
#include <ktrader.h>
#include <kconfig.h>
#include <kio/job.h>
#include <kservice.h>
#include <kinstance.h>
#include <kpopupmenu.h>
#include <dcopref.h>
#include <khtml_part.h>
#include <kgenericfactory.h>
#include <kprotocolmanager.h>
#include <kaboutdata.h>

#include "uachangerplugin.h"

typedef KGenericFactory<UAChangerPlugin> UAChangerPluginFactory;
static const KAboutData aboutdata("uachangerplugin", I18N_NOOP("Change Browser Identification") , "1.0" );
K_EXPORT_COMPONENT_FACTORY (libuachangerplugin, UAChangerPluginFactory (&aboutdata))


#define UA_PTOS(x) (*it)->property(x).toString()
#define QFL1(x) QString::fromLatin1(x)


UAChangerPlugin::UAChangerPlugin( QObject* parent, const char* name,
                                  const QStringList & )
                :KParts::Plugin( parent, name ),
                  m_bSettingsLoaded(false), m_part(0L), m_config(0L)
{
  setInstance(UAChangerPlugin::instance());

  m_pUAMenu = new KActionMenu( i18n("Change Browser &Identification"), "agent",
                               actionCollection(), "changeuseragent" );
  m_pUAMenu->setDelayed( false );
  connect( m_pUAMenu->popupMenu(), SIGNAL( aboutToShow() ),
           this, SLOT( slotAboutToShow() ) );

  m_pUAMenu->setEnabled ( false );

  if ( parent && parent->inherits( "KHTMLPart" ) )
  {
    m_part = static_cast<KHTMLPart*>(parent);
    connect( m_part, SIGNAL(started(KIO::Job*)), this,
             SLOT(slotStarted(KIO::Job*)) );
  }
}

UAChangerPlugin::~UAChangerPlugin()
{
  saveSettings();
  slotReloadDescriptions();
}

void UAChangerPlugin::slotReloadDescriptions()
{
  delete m_config;
  m_config = 0L;
}

void UAChangerPlugin::parseDescFiles()
{
  KTrader::OfferList list = KTrader::self()->query("UserAgentStrings");
  if ( list.count() == 0 )
    return;

  m_mapAlias.clear();
  m_lstAlias.clear();
  m_lstIdentity.clear();

  struct utsname utsn;
  uname( &utsn );

  QStringList languageList = KGlobal::locale()->languageList();
  if ( languageList.count() )
  {
     QStringList::Iterator it = languageList.find(QFL1("C"));
     if( it != languageList.end() )
     {
       if( languageList.contains( QFL1("en") ) > 0 )
         languageList.remove( it );
       else
         (*it) = QFL1("en");
     }
  }

  KTrader::OfferList::ConstIterator it = list.begin();
  KTrader::OfferList::ConstIterator lastItem = list.end();

  for ( ; it != lastItem; ++it )
  {
    QString tmp = UA_PTOS("X-KDE-UA-FULL");
    QString tag = UA_PTOS("X-KDE-UA-TAG");

    if(tag != "IE" && tag != "NN" && tag != "MOZ")
      tag = "OTHER";

    if ( (*it)->property("X-KDE-UA-DYNAMIC-ENTRY").toBool() )
    {
      tmp.replace( QFL1("appSysName"), QFL1(utsn.sysname) );
      tmp.replace( QFL1("appSysRelease"), QFL1(utsn.release) );
      tmp.replace( QFL1("appMachineType"), QFL1(utsn.machine) );
      tmp.replace( QFL1("appLanguage"), languageList.join(QFL1(", ")) );
      tmp.replace( QFL1("appPlatform"), QFL1("X11") );
    }

    if ( m_lstIdentity.contains(tmp) )
      continue; // Ignore dups!

    m_lstIdentity << tmp;
    tmp = QString("%1 %2").arg(UA_PTOS("X-KDE-UA-SYSNAME")).arg(UA_PTOS("X-KDE-UA-SYSRELEASE"));

    if ( tmp.stripWhiteSpace().isEmpty() )
    {
      if(tag == "NN" || tag == "IE" || tag == "MOZ")
         tmp = i18n("Version %1").arg(UA_PTOS("X-KDE-UA-VERSION"));
      else
         tmp = QString("%1 %2").arg(UA_PTOS("X-KDE-UA-NAME")).arg(UA_PTOS("X-KDE-UA-VERSION"));
    }
    else
    {
      if(tag == "NN" || tag == "IE" || tag == "MOZ")
         tmp = i18n("Version %1 on %2").arg(UA_PTOS("X-KDE-UA-VERSION")).arg(tmp);
      else
        tmp = i18n("%1 %2 on %3").arg(UA_PTOS("X-KDE-UA-NAME")).arg(UA_PTOS("X-KDE-UA-VERSION")).arg(tmp);
    }

    m_lstAlias << tmp;

    /* sort in this UA Alias alphabetically */
    BrowserGroup ualist = m_mapAlias[tag];
    BrowserGroup::Iterator e = ualist.begin();
    while ( !tmp.isEmpty() && e != ualist.end() )
    {
      if ( m_lstAlias[(*e)] > tmp ) {
         ualist.insert( e, m_lstAlias.count()-1 );
         tmp = QString::null;
      }
      ++e;
    }

    if ( !tmp.isEmpty() )
      ualist.append( m_lstAlias.count()-1 );

    m_mapAlias[tag] = ualist;

    if(tag == "OTHER")
       m_mapBrowser[tag] = i18n("Other");
    else
       m_mapBrowser[tag] = UA_PTOS("X-KDE-UA-NAME");

  }
}

void UAChangerPlugin::slotStarted( KIO::Job* )
{
  m_currentURL = m_part->url();

  // This plugin works on local files, http[s], and webdav[s].
  QString proto = m_currentURL.protocol();
  if (m_currentURL.isLocalFile() ||
      proto.startsWith("http") || proto.startsWith("webdav"))
  {
    if (!m_pUAMenu->isEnabled())
      m_pUAMenu->setEnabled ( true );
  }
  else
  {
    m_pUAMenu->setEnabled ( false );
  }
}

void UAChangerPlugin::slotAboutToShow()
{
  if (!m_config)
  {
    m_config = new KConfig( "kio_httprc" );
    parseDescFiles();
  }

  if (!m_bSettingsLoaded)
    loadSettings();

  int count = 0;
  m_pUAMenu->popupMenu()->clear();
  m_pUAMenu->popupMenu()->insertTitle(i18n("Identify As")); // imho title doesn't need colon..

  QString host = m_currentURL.isLocalFile() ? QFL1("localhost") : m_currentURL.host();
  m_currentUserAgent = KProtocolManager::userAgentForHost(host);
  //kdDebug(90130) << "User Agent: " << m_currentUserAgent << endl;

  int id = m_pUAMenu->popupMenu()->insertItem( i18n("Default Identification"), this,
                                      SLOT(slotDefault()), 0, ++count );
  if( m_currentUserAgent == KProtocolManager::defaultUserAgent() )
     m_pUAMenu->popupMenu()->setItemChecked(id, true);

  m_pUAMenu->popupMenu()->insertSeparator();

  AliasConstIterator map = m_mapAlias.begin();
  for( ; map != m_mapAlias.end(); ++map )
  {
    KPopupMenu *browserMenu = new KPopupMenu;
    BrowserGroup::ConstIterator e = map.data().begin();
    for( ; e != map.data().end(); ++e )
    {
       int id = browserMenu->insertItem( m_lstAlias[*e], this, SLOT(slotItemSelected(int)), 0, *e );
       if (m_lstIdentity[(*e)] == m_currentUserAgent)
         browserMenu->setItemChecked(id, true);
    }
    m_pUAMenu->popupMenu()->insertItem( m_mapBrowser[map.key()], browserMenu );
  }

  m_pUAMenu->popupMenu()->insertSeparator();

  /* useless here, imho..
  m_pUAMenu->popupMenu()->insertItem( i18n("Reload Identifications"), this,
                                      SLOT(slotReloadDescriptions()),
                                      0, ++count );*/

  m_pUAMenu->popupMenu()->insertItem( i18n("Apply to Entire Site"), this,
                                      SLOT(slotApplyToDomain()),
                                      0, ++count );
  m_pUAMenu->popupMenu()->setItemChecked(count, m_bApplyToDomain);

  m_pUAMenu->popupMenu()->insertItem( i18n("Configure..."), this,
                                      SLOT(slotConfigure()));

}

void UAChangerPlugin::slotConfigure()
{
  KService::Ptr service = KService::serviceByDesktopName ("useragent");
  if (service)
    KRun::runCommand (service->exec ());
}

void UAChangerPlugin::slotItemSelected( int id )
{
  if (m_lstIdentity[id] == m_currentUserAgent) return;

  QString host;
  m_currentUserAgent = m_lstIdentity[id];
  host = m_currentURL.isLocalFile() ? QFL1("localhost") : filterHost( m_currentURL.host() );

  m_config->setGroup( host.lower() );
  m_config->writeEntry( "UserAgent", m_currentUserAgent );
  m_config->sync();

  // Update the io-slaves...
  updateIOSlaves ();

  // Reload the page with the new user-agent string
  m_part->openURL( m_currentURL );
}

void UAChangerPlugin::slotDefault()
{
  if( m_currentUserAgent == KProtocolManager::defaultUserAgent() ) return; // don't flicker!
  // We have no choice but delete all higher domain level settings here since it
  // affects what will be matched.
  QStringList partList = QStringList::split('.', m_currentURL.host(), false);

  if ( !partList.isEmpty() )
  {
    partList.remove(partList.begin());

    QStringList domains;
    // Remove the exact name match...
    domains << m_currentURL.host ();

    while (partList.count())
    {
      if (partList.count() == 2)
        if (partList[0].length() <=2 && partList[1].length() ==2)
          break;

      if (partList.count() == 1)
        break;

      domains << partList.join(QFL1("."));
      partList.remove(partList.begin());
    }

    for (QStringList::Iterator it = domains.begin(); it != domains.end(); it++)
    {
      //kdDebug () << "Domain to remove: " << *it << endl;
      if ( m_config->hasGroup(*it) )
        m_config->deleteGroup(*it);
      else if( m_config->hasKey(*it) )
        m_config->deleteEntry(*it);
    }
  }
  else
      if ( m_currentURL.isLocalFile() && m_config->hasGroup( "localhost" ) )
          m_config->deleteGroup( "localhost" );

  m_config->sync();

  // Reset some internal variables and inform the http io-slaves of the changes.
  m_currentUserAgent = KProtocolManager::defaultUserAgent();

  // Update the http io-slaves.
  updateIOSlaves();

  // Reload the page with the default user-agent
  m_part->openURL( m_currentURL );
}

void UAChangerPlugin::updateIOSlaves ()
{
  // Inform running http(s) io-slaves about the change...
  if (!DCOPRef("*", "KIO::Scheduler").send("reparseSlaveConfiguration", QString::null))
    kdWarning() << "UAChangerPlugin::updateIOSlaves: Unable to update running application!" << endl;
}

QString UAChangerPlugin::filterHost(const QString &hostname)
{
  QRegExp rx;

  // Check for IPv4 address
  rx.setPattern ("[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}");
  if (rx.exactMatch (hostname))
    return hostname;

  // Check for IPv6 address here...
  rx.setPattern ("^\\[.*\\]$");
  if (rx.exactMatch (hostname))
    return hostname;

  // Return the TLD if apply to domain or
  return (m_bApplyToDomain ? findTLD(hostname): hostname);
}

QString UAChangerPlugin::findTLD (const QString &hostname)
{
  QStringList domains;
  QStringList partList = QStringList::split('.', hostname, false);

  if (partList.count())
      partList.remove(partList.begin()); // Remove hostname

  while(partList.count())
  {
    // We only have a TLD left.
    if (partList.count() == 1)
        break;

    if( partList.count() == 2 )
    {
        // The .name domain uses <name>.<surname>.name
        // Although the TLD is striclty speaking .name, for our purpose
        // it should be <surname>.name since people should not be able
        // to set cookies for everyone with the same surname.
        // Matches <surname>.name
        if( partList[1].lower() == QFL1("name") )
        {
          break;
        }
        else if( partList[1].length() == 2 )
        {
          // If this is a TLD, we should stop. (e.g. co.uk)
          // We assume this is a TLD if it ends with .xx.yy or .x.yy
          if (partList[0].length() <= 2)
              break; // This is a TLD.

          // Catch some TLDs that we miss with the previous check
          // e.g. com.au, org.uk, mil.co
          QCString t = partList[0].lower().utf8();
          if ((t == "com") || (t == "net") || (t == "org") || (t == "gov") ||
              (t == "edu") || (t == "mil") || (t == "int"))
              break;
        }
    }

    domains.append(partList.join(QFL1(".")));
    partList.remove(partList.begin()); // Remove part
  }

  if( domains.isEmpty() )
    return hostname;

  return domains[0];
}

void UAChangerPlugin::saveSettings()
{
  if(!m_bSettingsLoaded) return;

  KConfig cfg ("uachangerrc", false, false);
  cfg.setGroup ("General");

  cfg.writeEntry ("applyToDomain", m_bApplyToDomain);
}

void UAChangerPlugin::loadSettings()
{
  KConfig cfg ("uachangerrc", false, false);
  cfg.setGroup ("General");

  m_bApplyToDomain = cfg.readBoolEntry ("applyToDomain", true);
  m_bSettingsLoaded = true;
}

void UAChangerPlugin::slotApplyToDomain()
{
  m_bApplyToDomain = !m_bApplyToDomain;
}

#include "uachangerplugin.moc"
