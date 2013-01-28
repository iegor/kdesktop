/* This file is part of the KDE project
 *
 * Copyright (C) 2005 Leo Savernik <l.savernik@aon.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "domtreewindow.h"
#include "domtreeview.h"
#include "domtreecommands.h"
#include "messagedialog.h"
#include "plugin_domtreeviewer.h"

#include <kaccel.h>
#include <kapplication.h>
#include <kcommand.h>
#include <kconfig.h>
#include <kdeversion.h>
#include <kglobal.h>
#include <khtml_part.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <kstandarddirs.h>
#include <ktextedit.h>
#include <kurl.h>
#include <kurldrag.h>
#include <kurlrequesterdlg.h>

#include <kparts/partmanager.h>
#include <kparts/browserextension.h>

#include <kedittoolbar.h>

#include <kstdaccel.h>
#include <kaction.h>
#include <kstdaction.h>

#include <qdatetime.h>
#include <qtimer.h>

using domtreeviewer::ManipulationCommand;

DOMTreeWindow::DOMTreeWindow(PluginDomtreeviewer *plugin)
    : KMainWindow( 0, "DOMTreeWindow" ),
      m_plugin(plugin), m_view(new DOMTreeView(this, "DOMTreeView", false))
{
    part_manager = 0;

    // set configuration object
    _config = new KConfig("domtreeviewerrc");

    // accept dnd
    setAcceptDrops(true);

    // tell the KMainWindow that this is indeed the main widget
    setCentralWidget(m_view);

    // message window dialog
    msgdlg = new MessageDialog(0, "MessageDialog");
    msgdlg->messagePane->setPaletteBackgroundColor(palette().active().base());
// msgdlg->show();

    // then, setup our actions
    setupActions();

    // Add typical actions and save size/toolbars/statusbar
    setupGUI(ToolBar | Keys | StatusBar | Save | Create,
             locate( "data", "domtreeviewer/domtreeviewerui.rc", instance()));

    // allow the view to change the statusbar and caption
#if 0
    connect(m_view, SIGNAL(signalChangeStatusbar(const QString&)),
            this,   SLOT(changeStatusbar(const QString&)));
    connect(m_view, SIGNAL(signalChangeCaption(const QString&)),
            this,   SLOT(changeCaption(const QString&)));
#endif
    connect(view(), SIGNAL(htmlPartChanged(KHTMLPart *)),
    		SLOT(slotHtmlPartChanged(KHTMLPart *)));

    ManipulationCommand::connect(SIGNAL(error(int, const QString &)),
    				this, SLOT(addMessage(int, const QString &)));

    infopanel_ctx = createInfoPanelAttrContextMenu();
    domtree_ctx = createDOMTreeViewContextMenu();

}

DOMTreeWindow::~DOMTreeWindow()
{
    kdDebug(90180) << k_funcinfo << this << endl;
    delete m_commandHistory;
    delete msgdlg;
    delete _config;
}

void DOMTreeWindow::executeAndAddCommand(ManipulationCommand *cmd)
{
    cmd->execute();
    if (cmd->isValid()) {
      m_commandHistory->addCommand(cmd, false);
      view()->hideMessageLine();
    }
}

void DOMTreeWindow::setupActions()
{
    KStdAction::close(this, SLOT(close()), actionCollection());

    KStdAction::cut(this, SLOT(slotCut()), actionCollection())->setEnabled(false);
    KStdAction::copy(this, SLOT(slotCopy()), actionCollection())->setEnabled(false);
    KStdAction::paste(this, SLOT(slotPaste()), actionCollection())->setEnabled(false);

    m_commandHistory = new KCommandHistory(actionCollection());

    KStdAction::find(this, SLOT(slotFind()), actionCollection());

    KStdAction::redisplay(m_view, SLOT(refresh()), actionCollection());

    // toggle manipulation dialog
    KAction *showMsgDlg = new KAction(i18n("Show Message Log"),
    			CTRL+Key_E, actionCollection(), "show_msg_dlg");
    connect(showMsgDlg, SIGNAL(activated()), SLOT(showMessageLog()));

//     KAction *custom = new KAction(i18n("Cus&tom Menuitem"), 0,
//                                   this, SLOT(optionsPreferences()),
//                                   actionCollection(), "custom_action");

    // actions for the dom tree list view toolbar
    KStdAction::up(view(), SLOT(moveToParent()), actionCollection(), "tree_up");
    KAction *tree_inc_level = new KAction(i18n("Expand"),
			"1rightarrow", CTRL+Key_Greater, view(),
			SLOT(increaseExpansionDepth()), actionCollection(),
			"tree_inc_level");
    tree_inc_level->setToolTip(i18n("Increase expansion level"));
    KAction *tree_dec_level = new KAction(i18n("Collapse"),
			"1leftarrow", CTRL+Key_Less, view(),
			SLOT(decreaseExpansionDepth()), actionCollection(),
			"tree_dec_level");
    tree_dec_level->setToolTip(i18n("Decrease expansion level"));

    // actions for the dom tree list view context menu

    del_tree = new KAction(i18n("&Delete"), "editdelete",
    			Key_Delete, view(), SLOT(deleteNodes()),
			actionCollection(), "tree_delete");
    del_tree->setToolTip(i18n("Delete nodes"));
    /*KAction *new_elem = */new KAction(i18n("New &Element ..."),
			"bookmark", KShortcut(), view(),
			SLOT(slotAddElementDlg()), actionCollection(),
			"tree_add_element");
    /*KAction *new_text = */new KAction(i18n("New &Text Node ..."),
			"text", KShortcut(), view(), SLOT(slotAddTextDlg()),
			actionCollection(), "tree_add_text");

    // actions for the info panel attribute list context menu
    del_attr = new KAction(i18n("&Delete"), "editdelete",
    			Key_Delete, view(), SLOT(deleteAttributes()),
			actionCollection(), "attr_delete");
    del_attr->setToolTip(i18n("Delete attributes"));

}

QPopupMenu *DOMTreeWindow::createInfoPanelAttrContextMenu()
{
  QWidget *w = factory()->container("infopanelattr_context", this);
  Q_ASSERT(w);
  return static_cast<QPopupMenu *>(w);
}

QPopupMenu *DOMTreeWindow::createDOMTreeViewContextMenu()
{
  QWidget *w = factory()->container("domtree_context", this);
  Q_ASSERT(w);
  return static_cast<QPopupMenu *>(w);
}

void DOMTreeWindow::saveProperties(KConfig *config)
{
    // the 'config' object points to the session managed
    // config file.  anything you write here will be available
    // later when this app is restored

#if 0
    if (!m_view->currentURL().isNull()) {
#if KDE_IS_VERSION(3,1,3)
        config->writePathEntry("lastURL", m_view->currentURL());
#else
        config->writeEntry("lastURL", m_view->currentURL());
#endif
    }
#endif
}

void DOMTreeWindow::readProperties(KConfig *config)
{
    // the 'config' object points to the session managed
    // config file.  this function is automatically called whenever
    // the app is being restored.  read in here whatever you wrote
    // in 'saveProperties'

#if 0
    QString url = config->readPathEntry("lastURL");

    if (!url.isEmpty())
        m_view->openURL(KURL::fromPathOrURL(url));
#endif
}

void DOMTreeWindow::dragEnterEvent(QDragEnterEvent *event)
{
    // accept uri drops only
    event->accept(KURLDrag::canDecode(event));
}

void DOMTreeWindow::dropEvent(QDropEvent *event)
{
    // this is a very simplistic implementation of a drop event.  we
    // will only accept a dropped URL.  the Qt dnd code can do *much*
    // much more, so please read the docs there
    KURL::List urls;

    // see if we can decode a URI.. if not, just ignore it
    if (KURLDrag::decode(event, urls) && !urls.isEmpty())
    {
        // okay, we have a URI.. process it
        const KURL &url = urls.first();
#if 0
        // load in the file
        load(url);
#endif
    }
}

void DOMTreeWindow::addMessage(int msg_id, const QString &msg)
{
  QDateTime t(QDateTime::currentDateTime());
  QString fullmsg = t.toString();
  fullmsg += ":";

  if (msg_id != 0)
    fullmsg += " (" + QString::number(msg_id) + ") ";
  fullmsg += msg;

  if (msgdlg) msgdlg->addMessage(fullmsg);
  view()->setMessage(msg);
  kdWarning() << fullmsg << endl;
}
void DOMTreeWindow::slotCut()
{
  // TODO implement
}

void DOMTreeWindow::slotCopy()
{
  // TODO implement
}

void DOMTreeWindow::slotPaste()
{
  // TODO implement
}

void DOMTreeWindow::slotFind()
{
  view()->slotFindClicked();
}

void DOMTreeWindow::showMessageLog()
{
  msgdlg->show();
  msgdlg->raise();
  msgdlg->setActiveWindow();
}

void DOMTreeWindow::optionsConfigureToolbars()
{
    // use the standard toolbar editor
    saveMainWindowSettings( config(), autoSaveGroup() );
    KEditToolbar dlg(actionCollection());
    connect(&dlg, SIGNAL(newToolbarConfig()), this, SLOT(newToolbarConfig()));
    dlg.exec();
}

void DOMTreeWindow::newToolbarConfig()
{
    // this slot is called when user clicks "Ok" or "Apply" in the toolbar editor.
    // recreate our GUI, and re-apply the settings (e.g. "text under icons", etc.)
    createGUI(locate( "data", "domtreeviewer/domtreeviewerui.rc", instance()));
    applyMainWindowSettings( config(), autoSaveGroup() );
}

void DOMTreeWindow::optionsPreferences()
{
#if 0
    // popup some sort of preference dialog, here
    DOMTreeWindowPreferences dlg;
    if (dlg.exec())
    {
        // redo your settings
    }
#endif
}

void DOMTreeWindow::changeStatusbar(const QString& text)
{
    // display the text on the statusbar
    statusBar()->message(text);
}

void DOMTreeWindow::changeCaption(const QString& text)
{
    // display the text on the caption
    setCaption(text);
}

void DOMTreeWindow::slotHtmlPartChanged(KHTMLPart *p)
{
  kdDebug(90180) << k_funcinfo << p << endl;

  if (p) {
    // set up manager connections
    if ( part_manager )
        disconnect(part_manager);

    part_manager = p->manager();

    connect(part_manager, SIGNAL(activePartChanged(KParts::Part *)),
    	SLOT(slotActivePartChanged(KParts::Part *)));
    connect(part_manager, SIGNAL(partRemoved(KParts::Part *)),
    	SLOT(slotPartRemoved(KParts::Part *)));

    // set up browser extension connections
    connect(p, SIGNAL(docCreated()), SLOT(slotClosePart()));
  }
}

void DOMTreeWindow::slotActivePartChanged(KParts::Part *p)
{
  kdDebug(90180) << k_funcinfo << p << endl;
  Q_ASSERT(p != view()->htmlPart());

  m_commandHistory->clear();
  view()->disconnectFromTornDownPart();
  view()->setHtmlPart(::qt_cast<KHTMLPart *>(p));
}

void DOMTreeWindow::slotPartRemoved(KParts::Part *p)
{
  kdDebug(90180) << k_funcinfo << p << endl;
  if (p != view()->htmlPart()) return;

  m_commandHistory->clear();
  view()->disconnectFromTornDownPart();
  view()->setHtmlPart(0);
}

void DOMTreeWindow::slotClosePart()
{
  kdDebug(90180) << k_funcinfo << endl;
  view()->disconnectFromTornDownPart();
  view()->connectToPart();
}

#include "domtreewindow.moc"
