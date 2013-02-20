/*
    Copyright (c) 2003 Alexander Kellett <lypanov@kde.org>

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

#include <qfile.h>

#include <kdebug.h>
#include <kaction.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kinstance.h>
#include <khtml_part.h>
#include <kgenericfactory.h>

#include <kstandarddirs.h>

#include <krun.h>
#include <kservice.h>

#include <kpopupmenu.h>
#include <kbookmarkimporter.h>
#include <kbookmarkmanager.h>

#include "minitoolsplugin.h"

typedef KGenericFactory<MinitoolsPlugin> MinitoolsPluginFactory;
K_EXPORT_COMPONENT_FACTORY( libminitoolsplugin, MinitoolsPluginFactory("minitoolsplugin") )

MinitoolsPlugin::MinitoolsPlugin(QObject* parent, const char* name, const QStringList &)
   : KParts::Plugin(parent, name) {
  m_part = (parent && parent->inherits( "KHTMLPart" )) ? static_cast<KHTMLPart*>(parent) : 0L;

  m_pMinitoolsMenu = new KActionMenu(i18n("&Minitools"), "minitools", actionCollection(), "minitools");

  m_pMinitoolsMenu->setDelayed(false);
  m_pMinitoolsMenu->setEnabled(true);

  connect(m_pMinitoolsMenu->popupMenu(), SIGNAL( aboutToShow() ),
                                   this, SLOT( slotAboutToShow() ));
}

MinitoolsPlugin::~MinitoolsPlugin() {
   ;
}

void MinitoolsPlugin::slotAboutToShow() {

  m_minitoolsList.clear();
  KXBELBookmarkImporterImpl importer;
  connect(&importer, SIGNAL( newBookmark( const QString &, const QCString &, const QString &) ),
                     SLOT( newBookmarkCallback( const QString &, const QCString &, const QString & ) ));
  connect(&importer, SIGNAL( endFolder() ), 
                     SLOT( endFolderCallback() ));
  QString filename = minitoolsFilename(true);
  if (!filename.isEmpty() && QFile::exists(filename)) {
     importer.setFilename(filename);
     importer.parse();
  }
  filename = minitoolsFilename(false);
  if (!filename.isEmpty() && QFile::exists(filename)) {
     importer.setFilename(filename);
     importer.parse();
  }

  m_pMinitoolsMenu->popupMenu()->clear();

  int count = m_pMinitoolsMenu->popupMenu()->count(); // why not 0???
  bool gotSep = true; // don't start with a sep

  if (m_minitoolsList.count() > 0) {
     MinitoolsList::ConstIterator e = m_minitoolsList.begin();
     for( ; e != m_minitoolsList.end(); ++e ) {
        if ( ((*e).first  == "-") 
          && ((*e).second == "-")
        ) {
           if (!gotSep)
              m_pMinitoolsMenu->popupMenu()->insertSeparator();
           gotSep = true;
           count++;
        } else {
           QString str = (*e).first;
           // emsquieezzy thingy?
           if (str.length() > 48) {
              str.truncate(48);
              str.append("...");
           }
           m_pMinitoolsMenu->popupMenu()->insertItem( 
              str, this,
              SLOT(slotItemSelected(int)),
              0, ++count );
           gotSep = false;
        }
     }
  }

  if (!gotSep) {
     // don't have an extra sep
     m_pMinitoolsMenu->popupMenu()->insertSeparator();
  }

  m_pMinitoolsMenu->popupMenu()
                       ->insertItem(i18n("&Edit Minitools"), 
                                    this, SLOT(slotEditBookmarks()),
                                    0, ++count );  
}

void MinitoolsPlugin::newBookmarkCallback(
   const QString & text, const QCString & url, const QString & 
) {
  kdDebug(90150) << "MinitoolsPlugin::newBookmarkCallback" << text << url << endl;
  m_minitoolsList.prepend(qMakePair(text,url));
}

void MinitoolsPlugin::endFolderCallback() {
  kdDebug(90150) << "MinitoolsPlugin::endFolderCallback" << endl;
  m_minitoolsList.prepend(qMakePair(QString("-"),QCString("-")));
}

QString MinitoolsPlugin::minitoolsFilename(bool local) {
  return local ? locateLocal("data", QString::fromLatin1("konqueror/minitools.xml"))
               : locateLocal("data", QString::fromLatin1("konqueror/minitools-global.xml"));
}

void MinitoolsPlugin::slotEditBookmarks() {
   KBookmarkManager *manager = KBookmarkManager::managerForFile(minitoolsFilename(true));
   manager->slotEditBookmarks();
}

void MinitoolsPlugin::slotItemSelected(int id) {
  if (m_minitoolsList.count() == 0)
     return;
  QString tmp = m_minitoolsList[id-1].second;
  QString script = KURL::decode_string(tmp.right(tmp.length() - 11)); // sizeof("javascript:")
  connect(this, SIGNAL( executeScript(const QString &) ),
          m_part, SLOT( executeScript(const QString &) ));
  emit executeScript(script);
  disconnect(this, SIGNAL( executeScript(const QString &) ), 0, 0);
}

#include "minitoolsplugin.moc"
