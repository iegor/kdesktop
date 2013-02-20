/***************************************************************************
                          katefll_plugin.cpp  -  description
                             -------------------
    begin                : FRE July 12th 2002
    copyright            : (C) 2002 by Joseph Wenninger
    email                : jowenn@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "katefll_plugin.h"
#include "katefll_plugin.moc"

#include <kate/pluginmanager.h>

#include <qfileinfo.h>
#include <kgenericfactory.h>
#include <kaction.h>
#include <klocale.h>
#include <kdebug.h>
#include <kurl.h>
#include <kio/netaccess.h>
#include <kfiledialog.h>
#include <ktempfile.h>
#include <kmessagebox.h>

K_EXPORT_COMPONENT_FACTORY( katefll_plugin, KGenericFactory<PluginKateFileListLoader>( "katefll_loader" ) )

class PluginView : public KXMLGUIClient
{
  friend class PluginKateFileListLoader;

  public:
    Kate::MainWindow *win;
    KRecentFilesAction *recentFiles;
};

PluginKateFileListLoader::PluginKateFileListLoader (QObject * parent, const char *name,  const QStringList)
  : Plugin((Kate::Application*)parent,name), PluginViewInterface(),
    m_config( new KConfig("katefilelistpluginrc") )
{
  m_config->setGroup("General");
  updateInit();
}


PluginKateFileListLoader:: ~PluginKateFileListLoader()
{
  m_config->sync();
  delete m_config;
}


void PluginKateFileListLoader::updateInit()
{
  /*if (application()->initPluginManager()->initPlugin())
  {
    if (application()->initPluginManager()->initPlugin()->qt_cast("InitPluginKateFileListLoader")) //That's needed, if the plugin is loaded during a configuration change
    m_saveURL=application()->initPluginManager()->initScript();
  }
  else*/ m_saveURL=KURL();
  m_oldInitURL="";//application()->initPluginManager()->initScript();
  m_saveAs=false;
}

void PluginKateFileListLoader::addView(Kate::MainWindow *win)
{
  PluginView *view = new PluginView ();

  (void) new KAction( i18n("Open File List..."), 0,
    this, SLOT( slotOpenList() ),
    view->actionCollection(), "file_kfllopen" );

  view->recentFiles = new KRecentFilesAction( i18n("Open Recent"), KShortcut::null(),
    this, SLOT(slotOpenList(const KURL&)), view->actionCollection(),
    "file_kfllopenrecent");
  view->recentFiles->loadEntries(m_config, "Recent Files");

  (void) new KAction( i18n("Save File List"), 0,
    this, SLOT( slotSaveList() ),
    view->actionCollection(), "file_kfllsave" );

  (void) new KAction( i18n("Save File List As..."), 0,
    this, SLOT( slotSaveListAs() ),
    view->actionCollection(), "file_kfllsaveas" );

  view->setInstance (new KInstance("kate"));
  view->setXMLFile( "plugins/katefll/ui.rc" );
  win->guiFactory()->addClient (view);
  view->win = win;

  m_views.append (view);
}

void PluginKateFileListLoader::removeView(Kate::MainWindow *win)
{
  if (m_views.count() == 1) // yeah baby, last view! So save filelist
    m_views.at(0)->recentFiles->saveEntries(m_config, "Recent Files");

  for (uint z=0; z < m_views.count(); z++)
    if (m_views.at(z)->win == win)
    {
      PluginView *view = m_views.at(z);
      m_views.remove (view);
      win->guiFactory()->removeClient (view);
      delete view;
    }
}

void PluginKateFileListLoader::addURLToList(const KURL& url)
{
  // add url to all views to make sure all are synced
  for (uint a = 0; a < m_views.count(); ++a)
  {
    m_views.at(a)->recentFiles->addURL(url);
  }
}

void PluginKateFileListLoader::removeURLFromList(const KURL& url)
{
  // remove url from all views to make sure all are synced
  for (uint a = 0; a < m_views.count(); ++a) {
    m_views.at(a)->recentFiles->removeURL(url);
  }
}

void PluginKateFileListLoader::slotOpenList()
{
  KURL url = KFileDialog::getOpenURL(QString::null,
    "*.katefl|Kate File List (*.katefl)");
  if (url.isValid()) // cancel pressed?
    slotOpenList(url);
}

void PluginKateFileListLoader::slotOpenList(const KURL& url)
{
  if ( url.isValid() && KIO::NetAccess::exists(url, false, 0) )
  {
    addURLToList( url );
    if ( KMessageBox::questionYesNo (0,
	 i18n("Do you want to close all other files first?"),
	 i18n("Kate Filelist Loader"),
	 KStdGuiItem::close(), i18n("Do Not Close"),
	 "kate-filelist-loader-close-other" ) == KMessageBox::Yes )
      application()->documentManager()->closeAllDocuments();

    QString tmpFile;
    if( KIO::NetAccess::download( url, tmpFile, 0 ) )
    {
      QFile file(tmpFile);
      file.open(IO_ReadOnly);

      QTextStream t(&file);
      KURL tmp;
      while (!t.eof()) {
	if ( ! tmp.isEmpty() )
	  application()->documentManager()->openURL( tmp );

	tmp = KURL(t.readLine());
//         application()->activeMainWindow()->viewManager()->openURL(KURL(t.readLine()));
      }

      file.close();
      if ( ! tmp.isEmpty() )
         application()->activeMainWindow()->viewManager()->openURL(tmp);

      KIO::NetAccess::removeTempFile( tmpFile );
    } else application()->documentManager()->openURL(KURL());
  }
  else
  {
    KMessageBox::error(0, i18n("The selected filelist does not exist or is invalid."));
    // url not valid -> remove from list
    removeURLFromList(url);
  }
}

void PluginKateFileListLoader::slotSaveList()
{
  if (m_saveURL.isValid())
  {
    if (m_saveAs)
    {
    /*  if (m_oldInitURL!=application()->initPluginManager()->initScript())
      {
	switch (KMessageBox::questionYesNoCancel(0,i18n("<qt>Since the last time you saved the file list, Kate has been reinitialized by another plugin other than the <B>File List Loader</B>. Do you still want to save the list to %1?</qt>").arg(m_saveURL.prettyURL()),QString::null,KStdGuiItem::save(),KStdGuiItem::discard()))
        {
          case KMessageBox::Yes: save(); break;
          case KMessageBox::No: slotSaveListAs(); break;
          default: break;
        }
      }
      else*/ save();
    }
    else
    {
      /*if (m_saveURL!=application()->initPluginManager()->initScript())
      {
	switch (KMessageBox::questionYesNoCancel(0,i18n("<qt>Kate has been reinitialized by another plugin other than the <B>File List Loader</B>. Do you still want to save the list to %1?</qt>").arg(m_saveURL.prettyURL()),QString::null,KStdGuiItem::save(),KStdGuiItem::discard()))
        {
          case KMessageBox::Yes: save(); break;
          case KMessageBox::No: slotSaveListAs(); break;
          default: break;
        }
      }
      else */save();
    }
  } else slotSaveListAs();
}

void PluginKateFileListLoader::save()
{
  KTempFile file(locateLocal("tmp", "kate"), "katefll");
  for (uint i=0;i<application()->documentManager()->documents();i++)
  {
    *file.textStream()<<application()->documentManager()->document(i)->url().url()<<endl;
  }
  file.close();
  KIO::NetAccess::upload(file.name(), m_saveURL, 0);
  file.unlink();
  addURLToList( m_saveURL );
}

void PluginKateFileListLoader::slotSaveListAs()
{
  KURL url=KFileDialog::getSaveURL(QString::null,"*.katefl|Kate File List (*.katefl)");
  if (url.isValid())
  {
    m_oldInitURL="";//application()->initPluginManager()->initScript();
    m_saveURL=url;
    m_saveAs=true;
    save();
  }
}
