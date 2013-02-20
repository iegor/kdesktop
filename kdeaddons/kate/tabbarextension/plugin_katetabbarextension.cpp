/***************************************************************************
                           plugin_katetabbarextension.cpp
                           -------------------
    begin                : 2004-04-20
    copyright            : (C) 2004 by Dominik Haumann
    email                : dhdev@gmx.de
 ***************************************************************************/

/***************************************************************************
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 ***************************************************************************/


// BEGIN INCLUDES
#include "plugin_katetabbarextension.h"

#include <kaction.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kglobalsettings.h>

#include <kdebug.h>
#include <ktoolbar.h>
#include <kconfig.h>
#include <kiconloader.h>
#include <kfiledialog.h>

#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
// END

class PluginView : public KXMLGUIClient
{
  friend class KatePluginTabBarExtension;

  public:
    Kate::MainWindow *win;
    KateTabBarExtension* tabbar;
};

extern "C"
{
  void* init_libkatetabbarextensionplugin()
  {
    KGlobal::locale()->insertCatalogue("katetabbarextension");
    return new KatePluginFactory;
  }
}

KatePluginFactory::KatePluginFactory()
{
  s_instance = new KInstance( "kate" );
}

KatePluginFactory::~KatePluginFactory()
{
  delete s_instance;
}

QObject* KatePluginFactory::createObject(
    QObject* parent, const char* name, const char*, const QStringList & )
{
  return new KatePluginTabBarExtension( parent, name );
}

KInstance* KatePluginFactory::s_instance = 0L;

// BEGIN KatePluginTabBarExtension
KatePluginTabBarExtension::KatePluginTabBarExtension(
    QObject* parent, const char* name )
  : Kate::Plugin ( (Kate::Application*)parent, name ),
    pConfig(new KConfig("katetabbarextensionpluginrc"))
{
  pConfig->setGroup("global");
}

KatePluginTabBarExtension::~KatePluginTabBarExtension()
{
  while (m_views.count() > 0)
  {
      removeView(m_views.at(0)->win);
  }

  delete pConfig;
}

void KatePluginTabBarExtension::addView(Kate::MainWindow *win)
{
  PluginView *view = new PluginView ();

  bool bHoriz = pConfig->readBoolEntry("horizontal orientation", true);
  bool sort   = pConfig->readBoolEntry("sort", true);

  view->tabbar = new KateTabBarExtension( application()->documentManager(),
      win, bHoriz, sort, 0, "tabs_hbox");

  new KWidgetAction(view->tabbar, "tab_bar_widget",
      KShortcut::null(), 0, 0, view->actionCollection(), "tabbar_widget");

  view->setInstance (new KInstance("kate"));
  view->setXMLFile("plugins/katetabbarextension/ui.rc");
  win->guiFactory()->addClient (view);
  view->win = win;

  m_views.append (view);

  KToolBar* toolbar = dynamic_cast<KToolBar*>
      (win->guiFactory()->container("tabbarExtensionToolBar", view));
  if (toolbar) {
    connect(toolbar, SIGNAL( orientationChanged(Orientation) ),
        view->tabbar, SLOT( slotMoved(Orientation) ));
  }
}

void KatePluginTabBarExtension::removeView(Kate::MainWindow *win)
{
  for (uint z=0; z < m_views.count(); z++) {
    if (m_views.at(z)->win == win)
    {
      PluginView *view = m_views.at(z);

      // the last tabbar, save options
      if (m_views.count() == 1)
      {
        pConfig->writeEntry("horizontal orientation",
            view->tabbar->orientation()==Qt::Horizontal?true:false);
        pConfig->writeEntry("sort", view->tabbar->sortByName());
        pConfig->sync();
      }

      m_views.remove (view);
      win->guiFactory()->removeClient (view);
      delete view->tabbar;
      delete view;
    }
  }
}

Kate::PluginConfigPage* KatePluginTabBarExtension::configPage(
    uint, QWidget *w, const char* /*name*/)
{
  KateTabBarExtensionConfigPage* p = new KateTabBarExtensionConfigPage(this, w);
  initConfigPage( p );
  connect( p, SIGNAL(configPageApplyRequest(KateTabBarExtensionConfigPage*)),
      SLOT(applyConfig(KateTabBarExtensionConfigPage*)) );
  return (Kate::PluginConfigPage*)p;
}

void KatePluginTabBarExtension::initConfigPage( KateTabBarExtensionConfigPage* p )
{
  p->pSortAlpha->setChecked(m_views.at(0)->tabbar->sortByName());
}

void KatePluginTabBarExtension::applyConfig( KateTabBarExtensionConfigPage* p )
{
  m_views.at(0)->tabbar->setSortByName(p->pSortAlpha->isChecked());
  // sync m_config in destructor
}
// END KatePluginTabBarExtension

// BEGIN KateTabBarButton
KateTabBarButton::KateTabBarButton(Kate::ViewManager* pViewManager,
    Kate::Document *pDoc, QWidget * parent, const char * name)
  : QPushButton(parent, name),
    modified(false),
    myDocID(pDoc->documentNumber()),
    doc(pDoc),
    viewManager(pViewManager)
{
  setFlat(true);
  setToggleButton(true);
  setFocusPolicy(QWidget::NoFocus);

  setText(pDoc->docName());

  connect(this, SIGNAL(toggled(bool)), SLOT(setOn(bool)));
}

KateTabBarButton::~KateTabBarButton() {}

uint KateTabBarButton::documentNumber()
{
  return myDocID;
}

void KateTabBarButton::setDirty(bool d)
{
  if (d) {
    setIconSet(SmallIconSet("cancel"));
  } else {
    if (modified) setIconSet(SmallIconSet("modified"));
    else setIconSet(QIconSet());
  }
}

void KateTabBarButton::setText( const QString& newText)
{
  QToolTip::remove(this);

  if (newText.length() > 20) {
    // squeeze to length 17+3=20
    QPushButton::setText(newText.left(9) + "..." + newText.right(8));
    QToolTip::add(this, newText);
  } else {
    QPushButton::setText(newText);
  }
}

QString KateTabBarButton::fullName() const
{
  if (doc) {
    return doc->docName();
  } else {
    return QString("");
  }
}

void KateTabBarButton::triggerModified()
{
  modified = !modified;
  if (modified) {
    QColor c(255, 0, 0);
    setPaletteForegroundColor( c );
    setIconSet(SmallIconSet("modified"));
  } else {
    QColor c(KGlobalSettings::textColor());
    setPaletteForegroundColor( c );
    setIconSet(QIconSet());
  }
}

void KateTabBarButton::setOn(bool on)
{
  disconnect( SIGNAL(toggled(bool)));
//  kdDebug() << "setOn: " << (int)on << endl;
  if ((!on) && viewManager->activeView()->document()->documentNumber()
      == documentNumber()) {
//    kdDebug() << "setOn aborted " << endl;
    QPushButton::setOn(true);
  } else {
    QPushButton::setOn(on);

    if (on) emit myToggled(this);
  }
  connect(this, SIGNAL(toggled(bool)), SLOT(setOn(bool)));
}
// END KateTabBarButton

// BEGIN KateTabBarExtension
KateTabBarExtension::KateTabBarExtension( Kate::DocumentManager *pDocManager,
    Kate::MainWindow *win, bool bHorizOrientation, bool bSort,
    QWidget* parent, const char* name, WFlags f )
  : QWidget(parent, name, f),
    pCurrentTab(0), m_win(win), m_docManager(pDocManager), m_sort(false)
{
  if (bHorizOrientation) {
    top = new QBoxLayout(this, QBoxLayout::LeftToRight);
    m_orientation = Qt::Horizontal;
  } else {
    top = new QBoxLayout(this, QBoxLayout::TopToBottom);
    m_orientation = Qt::Vertical;
  }

  // add all already existing documents to the tabbar
  for (uint i = 0; i < pDocManager->documents(); i++)
  {
    slotDocumentCreated (pDocManager->document(i));
  }

  connect(m_win->viewManager(), SIGNAL(viewChanged()), SLOT(slotViewChanged()));
  connect(pDocManager,
      SIGNAL(documentCreated(Kate::Document *)),
      SLOT(slotDocumentCreated(Kate::Document *)));
  connect(pDocManager,
      SIGNAL(documentDeleted(uint)),
      SLOT(slotDocumentDeleted(uint)));

  setSortByName(bSort);
}

KateTabBarExtension::~KateTabBarExtension() {}

void KateTabBarExtension::slotMoved(Orientation o)
{
  // the tabbar moved (top, right, bottom, left or fluently)
  switch (o) {
    case Qt::Vertical:
      top->setDirection(QBoxLayout::TopToBottom);
      break;

    case Qt::Horizontal:
      top->setDirection(QBoxLayout::LeftToRight);
      break;
  }

  m_orientation = o;
}

Qt::Orientation KateTabBarExtension::orientation() const
{
  return m_orientation;
}

bool KateTabBarExtension::sortByName() const
{
  return m_sort;
}

void KateTabBarExtension::setSortByName(bool sbn)
{
  if (m_sort != sbn) {
    m_sort = sbn;
    if (m_sort)
      updateSort();
  }
}

void KateTabBarExtension::updateSort()
{
//  kdDebug() << "updateSort called" << endl;

  if (sortByName()) {
    // remove all tabs from the tabbar
    KateTabBarButton* tab;
    for (tab = m_tabs.first(); tab; tab = m_tabs.next() ) {
      top->remove(tab);
    }

    // now sort
    m_tabs.sort();

    // and finally add tabs again. FIXME: Is there a better way? :(
    for (tab = m_tabs.first(); tab; tab = m_tabs.next() ) {
      top->addWidget(tab);
    }
  }
}

void KateTabBarExtension::slotDocumentCreated (Kate::Document *doc)
{
//  kdDebug() << "slotDocumentCreated" << endl;
  if (!doc) return;

  KateTabBarButton* tab = new KateTabBarButton(m_win->viewManager(), doc, this);
  connect(tab, SIGNAL(myToggled(KateTabBarButton*)),
      SLOT(slotActivateView(KateTabBarButton*)));
  connect(doc, SIGNAL(nameChanged(Kate::Document *)),
      SLOT(slotNameChanged(Kate::Document *)));
  connect(doc, SIGNAL(modStateChanged(Kate::Document *)),
      SLOT(slotModChanged(Kate::Document *)));
  connect(doc,
      SIGNAL(modifiedOnDisc(Kate::Document *, bool, unsigned char)),
      SLOT(slotModifiedOnDisc(Kate::Document *, bool, unsigned char)));
  if(doc->isModified()) tab->triggerModified();
  tab->show();
  top->addWidget(tab);
  m_tabs.append(tab);

  updateSort();
}

void KateTabBarExtension::slotDocumentDeleted (uint documentNumber)
{
//  kdDebug() << "slotDocumentDeleted " << endl;
  KateTabBarButton* tab;
  for (tab = m_tabs.first(); tab; tab = m_tabs.next() ) {
    if (tab->documentNumber() == documentNumber) {
      tab->disconnect();
      top->remove(tab);
      m_tabs.removeRef(tab);
      delete tab;
      tab = 0;
      break;
    }
  }
}

void KateTabBarExtension::slotActivateView(KateTabBarButton* newTab)
{
//  kdDebug() << "slotActiavateView" << endl;
  pCurrentTab = newTab;
  if (pCurrentTab) {
    KateTabBarButton* tab;
    for (tab = m_tabs.first(); tab; tab = m_tabs.next() ) {
      if (tab->isOn() && tab != pCurrentTab)
        tab->setOn(false);
    }
    uint id = pCurrentTab->documentNumber();
    m_win->viewManager()->activateView( id );
  }
}

void KateTabBarExtension::slotModChanged (Kate::Document *doc)
{
//  kdDebug() << "slotModChanged" << endl;

  if (!doc) return;

  KateTabBarButton* tab;
  for (tab = m_tabs.first(); tab; tab = m_tabs.next() ) {
    if (tab->documentNumber() == doc->documentNumber()) {
      // found
      tab->triggerModified();
      break;
    }
  }
}

void KateTabBarExtension::slotModifiedOnDisc (
    Kate::Document *doc, bool b, unsigned char /*reason*/)
{
//  kdDebug() << "slotModifiedOnDisc: " << (int)b << endl;

  // find corresponding tab
  KateTabBarButton* tab;
  for (tab = m_tabs.first(); tab; tab = m_tabs.next() ) {
    if (tab->documentNumber() == doc->documentNumber()) {
      tab->setDirty(b);
    }
  }
}

void KateTabBarExtension::slotNameChanged (Kate::Document *doc)
{
  if (!doc) return;
//  kdDebug() << "slotNameChanged " << doc->docName() << endl;

  KateTabBarButton* tab;
  for (tab = m_tabs.first(); tab; tab = m_tabs.next() ) {
    if (tab->documentNumber() == doc->documentNumber()) {
      tab->setText(doc->docName());
      break;
    }
  }

  updateSort();
}

void KateTabBarExtension::slotViewChanged ()
{
//  kdDebug() << "slotVieChanged()" << endl;
  Kate::View *view = m_win->viewManager()->activeView();
  if (!view) return;

  KateTabBarButton* tab;
  for (tab = m_tabs.first(); tab; tab = m_tabs.next() ) {
    if (tab->documentNumber()
        == ((Kate::Document *)view->getDoc())->documentNumber()) {
      pCurrentTab = tab;
      for (tab = m_tabs.first(); tab; tab = m_tabs.next() ) {
        if (tab->isOn()) tab->setOn(false);
      }
      if (!pCurrentTab->isOn()) pCurrentTab->setOn(true);

      break;
    }
  }
}
// END KateTabBarExtension

// BEGIN KateTabBarExtensionConfigPage
KateTabBarExtensionConfigPage::KateTabBarExtensionConfigPage(
    QObject* /*parent*/ /*= 0L*/, QWidget *parentWidget /*= 0L*/)
  : Kate::PluginConfigPage( parentWidget )
{
  QVBoxLayout* top = new QVBoxLayout(this, 0,
      KDialogBase::spacingHint());

  QGroupBox* gb = new QGroupBox( i18n("Sorting Behavior"),
      this, "tab_bar_extension_config_page_layout" );
  gb->setColumnLayout(1, Qt::Vertical);
  gb->setInsideSpacing(KDialogBase::spacingHint());
  pSortAlpha = new QCheckBox(i18n("Sort files alphabetically"), gb);

  top->add(gb);
  top->addStretch(1);
//  throw signal changed
  connect(pSortAlpha, SIGNAL(toggled(bool)), this, SIGNAL(changed()));
}

KateTabBarExtensionConfigPage::~KateTabBarExtensionConfigPage() {}

void KateTabBarExtensionConfigPage::apply()
{
    emit configPageApplyRequest( this );
}
// END KateTabBarExtensionConfigPage


// BEGIN MyPtrList implementaion
int MyPtrList::compareItems ( QPtrCollection::Item item1_,
    QPtrCollection::Item item2_ )
{
  KateTabBarButton* item1 = reinterpret_cast<KateTabBarButton*>(item1_);
  KateTabBarButton* item2 = reinterpret_cast<KateTabBarButton*>(item2_);

  if (item1->fullName().lower() < item2->fullName().lower()) {
//    kdDebug() << item1->fullName().lower() << " < "
//              << item2->fullName().lower() << endl;
    return -1;
  } else {
    if (item1->fullName().lower() > item2->fullName().lower()) {
//      kdDebug() << item1->fullName().lower() << " > "
//                << item2->fullName().lower() << endl;
      return 1;
    } else {
//      kdDebug() << item1->fullName().lower() << " == "
//                << item2->fullName().lower() << endl;
      return 0;
    }
  }
}
// END MyPtrList implementaion

#include "plugin_katetabbarextension.moc"
