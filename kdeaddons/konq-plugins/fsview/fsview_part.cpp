/* This file is part of FSView.
   Copyright (C) 2002, 2003 Josef Weidendorfer <Josef.Weidendorfer@gmx.de>

   KCachegrind is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation, version 2.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

/*
 * The KPart embedding the FSView widget
 */

#include <qclipboard.h>
#include <qtimer.h>
#include <qwhatsthis.h>

#include <kinstance.h>
#include <kfiledialog.h>
#include <kfileitem.h>
#include <kparts/genericfactory.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kaction.h>
#include <kpopupmenu.h>
#include <kglobalsettings.h>
#include <kprotocolinfo.h>
#include <kio/job.h>
#include <kmessagebox.h>

// from kdebase/libkonq...
#include <konq_operations.h>
#include <konq_drag.h>

#include "fsview_part.h"




typedef KParts::GenericFactory<FSViewPart> FSViewPartFactory;
K_EXPORT_COMPONENT_FACTORY( libfsviewpart, FSViewPartFactory )


// FSJob, for progress

FSJob::FSJob(FSView* v)
  : KIO::Job(false)
{
  _view = v;
  QObject::connect(v, SIGNAL(progress(int,int,const QString&)),
                   this, SLOT(progressSlot(int,int,const QString&)));
}

void FSJob::kill(bool quietly)
{
  _view->stop();

  Job::kill(quietly);
}

void FSJob::progressSlot(int percent, int dirs, const QString& cDir)
{
  if (percent<100) {
    emitPercent(percent, 100);
    slotInfoMessage(this, i18n("Read 1 folder, in %1",
                               "Read %n folders, in %1",
                               dirs ).arg(cDir));
  }
  else
    slotInfoMessage(this, i18n("1 folder", "%n folders",  dirs));
}


// FSViewPart

KAboutData* FSViewPart::createAboutData()
{
  KAboutData* aboutData;
  aboutData = new KAboutData("fsview", I18N_NOOP("FSView"), "0.1.1",
                             I18N_NOOP("Filesystem Utilization Viewer"),
                             KAboutData::License_GPL,
                             I18N_NOOP("(c) 2003-2005, Josef Weidendorfer"));
  return aboutData;
}

FSViewPart::FSViewPart(QWidget *parentWidget, const char *widgetName,
                       QObject *parent, const char *name,
                       const QStringList& /* args */)
    : KParts::ReadOnlyPart(parent, name)
{
    // we need an instance
    setInstance( FSViewPartFactory::instance() );

    _view = new FSView(new Inode(), parentWidget, widgetName);
    QWhatsThis::add(_view, i18n("<p>This is the FSView plugin, a graphical "
				"browsing mode showing filesystem utilization "
				"by using a tree map visualization.</p>"
				"<p>Note that in this mode, automatic updating "
				"when filesystem changes are made "
				"is intentionally <b>not</b> done.</p>"
				"<p>For details on usage and options available, "
				"see the online help under "
				"menu 'Help/FSView Manual'.</p>"));

    _view->show();
    setWidget(_view);

    _ext = new FSViewBrowserExtension(this);
    _job = 0;

    _areaMenu = new KActionMenu (i18n("Stop at Area"),
				 actionCollection(), "treemap_areadir");
    _depthMenu = new KActionMenu (i18n("Stop at Depth"),
				 actionCollection(), "treemap_depthdir");
    _visMenu = new KActionMenu (i18n("Visualization"),
				 actionCollection(), "treemap_visdir");
    _colorMenu = new KActionMenu (i18n("Color Mode"),
				  actionCollection(), "treemap_colordir");

    KAction* action;
    action = new KAction( i18n( "&FSView Manual" ), "fsview",
			KShortcut(), this, SLOT(showHelp()),
                        actionCollection(), "help_fsview" );
    action->setToolTip(i18n("Show FSView manual"));
    action->setWhatsThis(i18n("Opens the help browser with the "
			      "FSView documentation"));

    QObject::connect (_visMenu->popupMenu(), SIGNAL (aboutToShow()),
		      SLOT (slotShowVisMenu()));
    QObject::connect (_areaMenu->popupMenu(), SIGNAL (aboutToShow()),
		      SLOT (slotShowAreaMenu()));
    QObject::connect (_depthMenu->popupMenu(), SIGNAL (aboutToShow()),
		      SLOT (slotShowDepthMenu()));
    QObject::connect (_colorMenu->popupMenu(), SIGNAL (aboutToShow()),
		      SLOT (slotShowColorMenu()));

    slotSettingsChanged(KApplication::SETTINGS_MOUSE);
    if (kapp)
      connect( kapp, SIGNAL( settingsChanged(int) ),
	       SLOT( slotSettingsChanged(int) ) );

    QObject::connect(_view,SIGNAL(returnPressed(TreeMapItem*)),
                     _ext,SLOT(selected(TreeMapItem*)));
    QObject::connect(_view,SIGNAL(selectionChanged()),
                     _ext,SLOT(updateActions()));
    QObject::connect(_view,
                     SIGNAL(contextMenuRequested(TreeMapItem*,const QPoint&)),
                     _ext,
                     SLOT(contextMenu(TreeMapItem*, const QPoint&)));

    QObject::connect(_view, SIGNAL(started()), this, SLOT(startedSlot()));
    QObject::connect(_view, SIGNAL(completed(int)),
		     this, SLOT(completedSlot(int)));

    QTimer::singleShot(1, this, SLOT(showInfo()));

    setXMLFile( "fsview_part.rc" );
}


FSViewPart::~FSViewPart()
{
  kdDebug(90100) << "FSViewPart Destructor" << endl;

  delete _job;
  _view->saveFSOptions();
}

void FSViewPart::slotSettingsChanged(int category)
{
  if (category != KApplication::SETTINGS_MOUSE) return;

  QObject::disconnect(_view,SIGNAL(clicked(TreeMapItem*)),
		      _ext,SLOT(selected(TreeMapItem*)));
  QObject::disconnect(_view,SIGNAL(doubleClicked(TreeMapItem*)),
		      _ext,SLOT(selected(TreeMapItem*)));
  
  if (KGlobalSettings::singleClick())
    QObject::connect(_view,SIGNAL(clicked(TreeMapItem*)),
		     _ext,SLOT(selected(TreeMapItem*)));
  else
    QObject::connect(_view,SIGNAL(doubleClicked(TreeMapItem*)),
		     _ext,SLOT(selected(TreeMapItem*)));
}

void FSViewPart::showInfo()
{
    QString info;
    info = i18n("FSView intentionally does not support automatic updates "
		"when changes are made to files or directories, "
		"currently visible in FSView, from the outside.\n"
		"For details, see the 'Help/FSView Manual'.");

    KMessageBox::information( _view, info, QString::null, "ShowFSViewInfo");
}

void FSViewPart::showHelp()
{
    KApplication::startServiceByDesktopName("khelpcenter",
					    QString("help:/konq-plugins/fsview/index.html"));
}

void FSViewPart::startedSlot()
{
  _job = new FSJob(_view);
  emit started(_job);
}

void FSViewPart::completedSlot(int dirs)
{
  if (_job) {
    _job->progressSlot(100, dirs, QString::null);
    delete _job;
    _job = 0;
  }

  KConfigGroup cconfig(_view->config(), QCString("MetricCache"));
  _view->saveMetric(&cconfig);

  emit completed();
}

void FSViewPart::slotShowVisMenu()
{
  _visMenu->popupMenu()->clear();
  _view->addVisualizationItems(_visMenu->popupMenu(), 1301);
}

void FSViewPart::slotShowAreaMenu()
{
  _areaMenu->popupMenu()->clear();
  _view->addAreaStopItems(_areaMenu->popupMenu(), 1001, 0);
}

void FSViewPart::slotShowDepthMenu()
{
  _depthMenu->popupMenu()->clear();
  _view->addDepthStopItems(_depthMenu->popupMenu(), 1501, 0);
}

void FSViewPart::slotShowColorMenu()
{
  _colorMenu->popupMenu()->clear();
  _view->addColorItems(_colorMenu->popupMenu(), 1401);
}

bool FSViewPart::openFile() // never called since openURL is reimplemented
{
  kdDebug(90100) << "FSViewPart::openFile " << m_file << endl;
  _view->setPath(m_file);

  return true;
}

bool FSViewPart::openURL(const KURL &url)
{
  kdDebug(90100) << "FSViewPart::openURL " << url.path() << endl;

  if (!url.isValid()) return false;
  if (!url.isLocalFile()) return false;

  m_url = url;
  emit setWindowCaption( m_url.prettyURL() );

  _view->setPath(url.path());

  return true;
}

bool FSViewPart::closeURL()
{
  kdDebug(90100) << "FSViewPart::closeURL " << endl;

  _view->stop();

  return true;
}

// FSViewBrowserExtension

FSViewBrowserExtension::FSViewBrowserExtension(FSViewPart* viewPart,
                                               const char *name)
  :KParts::BrowserExtension(viewPart, name)
{
  _view = viewPart->view();
}

FSViewBrowserExtension::~FSViewBrowserExtension()
{}

void FSViewBrowserExtension::updateActions()
{
  TreeMapItemList s = _view->selection();
  TreeMapItem* i;
  int canDel = 0, canCopy = 0;
  KURL::List urls;

  for(i=s.first();i;i=s.next()) {
    KURL u;
    u.setPath( ((Inode*)i)->path() );
    urls.append(u);
    canCopy++;
    if (  KProtocolInfo::supportsDeleting(  u ) ) canDel++;
  }
  emit enableAction( "copy", canCopy > 0 );
  emit enableAction( "cut", canDel > 0 );
  emit enableAction( "trash", canDel > 0);
  emit enableAction( "del", canDel > 0 );
  emit enableAction( "editMimeType", ( s.count() == 1 ) );

  emit selectionInfo(urls);

  kdDebug(90100) << "FSViewPart::updateActions, deletable " << canDel << endl;
}


void FSViewBrowserExtension::del()
{
  KonqOperations::del(_view, KonqOperations::DEL, _view->selectedUrls());
  // How to get notified of end of delete operation?
  // - search for the KonqOperations child of _view (name "KonqOperations")
  // - connect to destroyed signal
  KonqOperations* o = (KonqOperations*) _view->child("KonqOperations");
  if (o) connect(o, SIGNAL(destroyed()), SLOT(refresh()));
}

void FSViewBrowserExtension::trash()
{
  KonqOperations::del(_view, KonqOperations::TRASH, _view->selectedUrls());
  KonqOperations* o = (KonqOperations*) _view->child("KonqOperations");
  if (o) connect(o, SIGNAL(destroyed()), SLOT(refresh()));
}

void FSViewBrowserExtension::copySelection( bool move )
{
  KonqDrag *urlData = KonqDrag::newDrag( _view->selectedUrls(), move );
  QApplication::clipboard()->setData( urlData );
}

void FSViewBrowserExtension::editMimeType()
{
  Inode* i = (Inode*) _view->selection().first();
  if (i)
    KonqOperations::editMimeType( i->mimeType()->name() );
}


// refresh treemap at end of KIO jobs
void FSViewBrowserExtension::refresh()
{
  // only need to refresh common ancestor for all selected items
  TreeMapItemList s = _view->selection();
  TreeMapItem *i, *commonParent = s.first();
  if (!commonParent) return;
  while( (i=s.next()) )
    commonParent = commonParent->commonParent(i);

  /* if commonParent is a file, update parent directory */
  if ( !((Inode*)commonParent)->isDir() ) {
    commonParent = commonParent->parent();
    if (!commonParent) return;
  }

  kdDebug(90100) << "FSViewPart::refreshing "
	    << ((Inode*)commonParent)->path() << endl;

  _view->requestUpdate( (Inode*)commonParent );
}

void FSViewBrowserExtension::selected(TreeMapItem* i)
{
  if (!i) return;

  KURL url;
  url.setPath( ((Inode*)i)->path() );
  emit openURLRequest(url);
}

void FSViewBrowserExtension::contextMenu(TreeMapItem* /*item*/,const QPoint& p)
{
    TreeMapItemList s = _view->selection();
    TreeMapItem* i;
    KFileItemList items;
    items.setAutoDelete(true);

    for(i=s.first();i;i=s.next()) {
	KURL u;
	u.setPath( ((Inode*)i)->path() );
	QString mimetype = ((Inode*)i)->mimeType()->name();
	const QFileInfo& info = ((Inode*)i)->fileInfo();
	mode_t mode =
	    info.isFile() ? S_IFREG :
	    info.isDir() ? S_IFDIR :
	    info.isSymLink() ? S_IFLNK : (mode_t)-1;
	items.append(new KFileItem(u, mimetype, mode));
    }

    if (items.count()>0)
      emit popupMenu(_view->mapToGlobal(p), items);
}


#include "fsview_part.moc"
