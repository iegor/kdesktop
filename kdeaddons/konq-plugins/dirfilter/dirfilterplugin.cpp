/*
   Copyright (C) 2000, 2001, 2002 Dawit Alemayehu <adawit@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <unistd.h>
#include <sys/types.h>

#include <qtimer.h>
#include <qapplication.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qhbox.h>
#include <qwhatsthis.h>
#include <qiconview.h>
#include <klistview.h>

#include <kdebug.h>
#include <klocale.h>
#include <kinstance.h>

#include <kaction.h>
#include <kpopupmenu.h>
#include <kmessagebox.h>
#include <kiconloader.h>

#include <kdirlister.h>
#include <klistviewsearchline.h>
#include <kiconviewsearchline.h>
#include <konq_dirpart.h>
#include <konq_propsview.h>
#include <kstaticdeleter.h>
#include <kgenericfactory.h>
#include <kparts/browserextension.h>

#include "dirfilterplugin.h"

SessionManager* SessionManager::m_self = 0;
static KStaticDeleter<SessionManager> dirfiltersd;

SessionManager *SessionManager::self ()
{
  if (!m_self)
    m_self = dirfiltersd.setObject(m_self, new SessionManager);

  return m_self;
}

SessionManager::SessionManager()
{
  m_bSettingsLoaded = false;
  loadSettings ();
}

SessionManager::~SessionManager()
{
  saveSettings();
  m_self = 0;
}

QString SessionManager::generateKey (const KURL& url) const
{
  QString key;

  key = url.protocol ();
  key += ':';

  if (!url.host ().isEmpty ())
  {
    key += url.host ();
    key += ':';
  }

  key += url.path ();
  key += ':';
  key += QString::number (m_pid);

  return key;
}

QStringList SessionManager::restoreMimeFilters (const KURL& url) const
{
  QString key(generateKey(url));
  return m_filters[key];
}

QString SessionManager::restoreTypedFilter (const KURL& url) const
{
  QString key(generateKey(url));
  return m_typedFilter[key];
}

void SessionManager::save (const KURL& url, const QStringList& filters)
{
  QString key = generateKey(url);
  m_filters[key] = filters;
}

void SessionManager::save (const KURL& url, const QString& typedFilter)
{
  QString key = generateKey(url);
  m_typedFilter[key] = typedFilter;
}

void SessionManager::saveSettings()
{
  KConfig cfg ("dirfilterrc", false, false);
  cfg.setGroup ("General");

  cfg.writeEntry ("ShowCount", showCount);
  cfg.writeEntry ("UseMultipleFilters", useMultipleFilters);
  cfg.sync();
}

void SessionManager::loadSettings()
{
  if (m_bSettingsLoaded)
    return;

  KConfig cfg ("dirfilterrc", false, false);
  cfg.setGroup ("General");

  showCount = cfg.readBoolEntry ("ShowCount", false);
  useMultipleFilters = cfg.readBoolEntry ("UseMultipleFilters", true);
  m_pid = getpid ();
  m_bSettingsLoaded = true;
}



DirFilterPlugin::DirFilterPlugin (QObject* parent, const char* name,
                                  const QStringList&)
                :KParts::Plugin (parent, name),
                 m_pFilterMenu(0),
                 m_searchWidget(0)
{
  m_part = ::qt_cast<KonqDirPart*>(parent);

  if ( !m_part || !m_part->scrollWidget() )
    return;

  m_pFilterMenu = new KActionMenu (i18n("View F&ilter"), "filter",
                                   actionCollection(), "filterdir");
  m_pFilterMenu->setDelayed (false);
  m_pFilterMenu->setWhatsThis(i18n("Allow to filter the currently displayed items by filetype."));

  connect (m_pFilterMenu->popupMenu(), SIGNAL (aboutToShow()),
           SLOT (slotShowPopup()));

  connect (m_part, SIGNAL (itemRemoved(const KFileItem*)),
           SLOT( slotItemRemoved (const KFileItem*)));
  connect (m_part, SIGNAL (itemsAdded(const KFileItemList&)),
           SLOT (slotItemsAdded(const KFileItemList&)));
  connect (m_part, SIGNAL (itemsFilteredByMime(const KFileItemList&)),
           SLOT (slotItemsAdded(const KFileItemList&)));
  connect (m_part, SIGNAL(aboutToOpenURL()), SLOT(slotOpenURL()));

  // add a searchline filter for konqis icons/list views
  QHBox *hbox = new QHBox(m_part->widget());
  hbox->hide();

  KAction *clear = new KAction(i18n("Clear Filter Field"),
                               QApplication::reverseLayout() ? "clear_left" : "locationbar_erase",
                               0, 0, 0, actionCollection(), "clear_filter");

  clear->setWhatsThis(i18n("Clear filter field<p>Clears the content of the filter field."));

  if ( ::qt_cast<KListView*>(m_part->scrollWidget()) )
  {
    m_searchWidget = new KListViewSearchLine(hbox);
    static_cast<KListViewSearchLine*>(m_searchWidget)->setListView(static_cast<KListView*>(m_part->scrollWidget()));
  }
  else if ( ::qt_cast<QIconView*>(m_part->scrollWidget()) )
  {
    m_searchWidget = new KIconViewSearchLine(hbox);
    static_cast<KIconViewSearchLine*>(m_searchWidget)->setIconView(static_cast<QIconView*>(m_part->scrollWidget()));
  }


  if ( m_searchWidget )
  {
    QWhatsThis::add(m_searchWidget, i18n("Enter here a text which an item in the view must contain anywhere to be shown."));
    connect(clear, SIGNAL(activated()), m_searchWidget, SLOT(clear()));
  }

  KWidgetAction *filterAction = new KWidgetAction(hbox, i18n("Filter Field"),
                                                  0, 0, 0, actionCollection(), "toolbar_filter_field");
  filterAction->setShortcutConfigurable(false);
}

DirFilterPlugin::~DirFilterPlugin()
{
  delete m_pFilterMenu;
}

void DirFilterPlugin::slotOpenURL ()
{
  KURL url = m_part->url();

  //kdDebug(90190) << "DirFilterPlugin: New URL    : " << url.url() << endl;
  //kdDebug(90190) << "DirFilterPlugin: Current URL: " << m_pURL.url() << endl;

  if (m_pURL != url)
  {
    //Clears the hidden list which is by now outdated...
    if (m_searchWidget)
    {
      SessionManager::self()->save(m_pURL, m_searchWidget->text());
      m_searchWidget->clear();

      QString typedFilter(SessionManager::self()->restoreTypedFilter(url));
      m_searchWidget->completionObject()->addItem(typedFilter);
      m_searchWidget->setText(typedFilter);
    }

    m_pURL = url;
    m_pMimeInfo.clear();
    m_part->setMimeFilter (SessionManager::self()->restoreMimeFilters(url));
  }
}

void DirFilterPlugin::slotShowPopup()
{
  if (!m_part)
  {
    m_pFilterMenu->setEnabled (false);
    return;
  }

  int id = 0;
  uint enableReset = 0;

  QString label;
  QStringList inodes;

  m_pFilterMenu->popupMenu()->clear();
  m_pFilterMenu->popupMenu()->insertTitle (i18n("Only Show Items of Type"));

  MimeInfoIterator it = m_pMimeInfo.begin();
  const MimeInfoIterator end = m_pMimeInfo.end();
  for (; it != end ; ++it)
  {
    if (it.key().startsWith("inode"))
    {
      inodes << it.key();
      continue;
    }

    if (!SessionManager::self()->showCount)
      label = it.data().mimeComment;
    else
    {
      label = it.data().mimeComment;
      label += "  (";
      label += QString::number (it.data().filenames.size ());
      label += ")";
    }

    m_pMimeInfo[it.key()].id = m_pFilterMenu->popupMenu()->insertItem (
                               SmallIconSet(it.data().iconName), label,
                               this, SLOT(slotItemSelected(int)), 0, ++id);

    if (it.data().useAsFilter)
    {
      m_pFilterMenu->popupMenu()->setItemChecked (id, true);
      enableReset++;
    }
  }

  // Add all the items that have mime-type of "inode/*" here...
  if (!inodes.isEmpty())
  {
    m_pFilterMenu->popupMenu()->insertSeparator ();

    QStringList::Iterator it = inodes.begin();
    QStringList::Iterator end = inodes.end();

    for (; it != end; ++it)
    {
      if (!SessionManager::self()->showCount)
        label = m_pMimeInfo[(*it)].mimeComment;
      else
      {
        label = m_pMimeInfo[(*it)].mimeComment;
        label += "  (";
        label += QString::number (m_pMimeInfo[(*it)].filenames.size ());
        label += ")";
      }

      m_pMimeInfo[(*it)].id = m_pFilterMenu->popupMenu()->insertItem (
                              SmallIconSet(m_pMimeInfo[(*it)].iconName), label,
                              this, SLOT(slotItemSelected(int)), 0, ++id);

      if (m_pMimeInfo[(*it)].useAsFilter)
      {
        m_pFilterMenu->popupMenu()->setItemChecked (id, true);
        enableReset ++;
      }
    }
  }

  m_pFilterMenu->popupMenu()->insertSeparator ();
  id = m_pFilterMenu->popupMenu()->insertItem (i18n("Use Multiple Filters"),
                                               this, SLOT(slotMultipleFilters()));
  m_pFilterMenu->popupMenu()->setItemEnabled (id, enableReset <= 1);
  m_pFilterMenu->popupMenu()->setItemChecked (id, SessionManager::self()->useMultipleFilters);

  id = m_pFilterMenu->popupMenu()->insertItem (i18n("Show Count"), this,
                                               SLOT(slotShowCount()));
  m_pFilterMenu->popupMenu()->setItemChecked (id, SessionManager::self()->showCount);

  id = m_pFilterMenu->popupMenu()->insertItem (i18n("Reset"), this,
                                               SLOT(slotReset()));

  m_pFilterMenu->popupMenu()->setItemEnabled (id, enableReset);
}

void DirFilterPlugin::slotItemSelected (int id)
{
  if (!m_part)
    return;

  MimeInfoIterator it = m_pMimeInfo.begin();
  while (it != m_pMimeInfo.end () && id != it.data().id)
    it++;

  if (it != m_pMimeInfo.end())
  {
    QStringList filters;

    if (it.data().useAsFilter)
    {
      it.data().useAsFilter = false;
      filters = m_part->mimeFilter ();
      if (filters.remove (it.key()))
        m_part->setMimeFilter (filters);
    }
    else
    {
      m_pMimeInfo[it.key()].useAsFilter = true;

      if (SessionManager::self()->useMultipleFilters)
      {
        filters = m_part->mimeFilter ();
        filters << it.key();
      }
      else
      {
        filters << it.key();

        MimeInfoIterator item = m_pMimeInfo.begin();
        while ( item != m_pMimeInfo.end() )
        {
          if ( item != it )
            item.data().useAsFilter = false;
          item++;
        }
      }

      m_part->setMimeFilter (filters);
    }

    KURL url = m_part->url();
    m_part->openURL (url);
    SessionManager::self()->save (url, filters);
  }
}

void DirFilterPlugin::slotItemsAdded (const KFileItemList& list)
{
  KURL url = m_part->url();

  if (list.count() == 0 || !m_part || !m_part->nameFilter().isEmpty())
  {
    m_pFilterMenu->setEnabled (m_part->nameFilter().isEmpty());
    return;
  }

  // Make sure the filter menu is enabled once a named
  // filter is removed.
  if (!m_pFilterMenu->isEnabled())
    m_pFilterMenu->setEnabled (true);

  for (KFileItemListIterator it (list); it.current (); ++it)
  {
    QString name = it.current()->name();
    KMimeType::Ptr mime = it.current()->mimeTypePtr(); // don't resolve mimetype if unknown
    if (!mime)
      continue;
    QString mimeType = mime->name();

    if (!m_pMimeInfo.contains (mimeType))
    {
      MimeInfo& mimeInfo = m_pMimeInfo[mimeType];
      QStringList filters = m_part->mimeFilter ();
      mimeInfo.useAsFilter = (!filters.isEmpty () &&
                                           filters.contains (mimeType));
      mimeInfo.mimeComment = mime->comment();
      mimeInfo.iconName = mime->icon(KURL(), false);
      mimeInfo.filenames.insert(name, false);
    }
    else
    {
      m_pMimeInfo[mimeType].filenames.insert(name, false);
    }
  }
}

void DirFilterPlugin::slotItemRemoved (const KFileItem* item)
{
  if (!item || !m_part)
    return;

  QString mimeType = item->mimetype().stripWhiteSpace();

  if (m_pMimeInfo.contains (mimeType))
  {
    MimeInfo info = m_pMimeInfo [mimeType];

    if (info.filenames.size () > 1)
      m_pMimeInfo [mimeType].filenames.remove (item->name ());
    else
    {
      if (info.useAsFilter)
      {
        QStringList filters = m_part->mimeFilter ();
        filters.remove (mimeType);
        m_part->setMimeFilter (filters);
        SessionManager::self()->save (m_part->url(), filters);
        QTimer::singleShot( 0, this, SLOT(slotTimeout()) );
      }
      m_pMimeInfo.remove (mimeType);
    }
  }
}

void DirFilterPlugin::slotReset()
{
  if (!m_part)
    return;

  MimeInfoIterator it = m_pMimeInfo.begin();
  for (; it != m_pMimeInfo.end(); ++it)
    it.data().useAsFilter = false;

  QStringList filters;
  KURL url = m_part->url();

  m_part->setMimeFilter (filters);
  SessionManager::self()->save (url, filters);
  m_part->openURL (url);
}

void DirFilterPlugin::slotShowCount()
{
  if (SessionManager::self()->showCount)
    SessionManager::self()->showCount = false;
  else
    SessionManager::self()->showCount = true;
}

void DirFilterPlugin::slotMultipleFilters()
{
  if (SessionManager::self()->useMultipleFilters)
    SessionManager::self()->useMultipleFilters = false;
  else
    SessionManager::self()->useMultipleFilters = true;
}

void DirFilterPlugin::slotTimeout()
{
  if (m_part)
    m_part->openURL (m_part->url());
}

typedef KGenericFactory<DirFilterPlugin> DirFilterFactory;
K_EXPORT_COMPONENT_FACTORY (libdirfilterplugin, DirFilterFactory("dirfilterplugin"))

#include "dirfilterplugin.moc"
