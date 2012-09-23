/* This file is part of the KDE libraries
   Copyright (C) 2000 Max Judin <novaprint@mtu-net.ru>
   Modified 2002 Andreas Zehender <zehender@kde.org>

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
#include "pmdockwidget.h"
#include "pmdockwidget_private.h"

#include <qapplication.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qobjectlist.h>
#include <qstrlist.h>
#include <qcursor.h>
#include <qwidgetlist.h>
#include <qtabwidget.h>
#include <qstyle.h>

#ifndef NO_KDE2
#include <kconfig.h>
#include <kglobal.h>
#include <ktoolbar.h>
#include <kpopupmenu.h>
#include <kwin.h>
#include <kdebug.h>
#ifdef Q_WS_X11
#include <X11/X.h>
#include <X11/Xlib.h>
#endif
#else
#include <qapplication.h>
#include <qtoolbar.h>
#include <qpopupmenu.h>
#endif

#include "pmdebug.h"

#include <kparts/event.h>
#include <kparts/part.h>
#include <kaccel.h>
#include <kparts/plugin.h>
#include <kstatusbar.h>
#include <kinstance.h>
#include <khelpmenu.h>
#include <kstandarddirs.h>
#include <qapplication.h>
#include <kdebug.h>
#include <kxmlguifactory.h>

#include <assert.h>

#define DOCK_CONFIG_VERSION "0.0.5"

static const char* const close_xpm[]={
"7 7 2 1",
"# c black",
". c None",
".......",
"##..##.",
".####..",
"..##...",
".####..",
"##..##.",
"......."};

static const char* const todesktop_xpm[]={
"5 5 2 1",
"# c black",
". c None",
"####.",
"##...",
"#.#..",
"#..#.",
"....#"};

static const char* const dockback_xpm[]={
"5 5 2 1",
"# c black",
". c None",
"#....",
".#..#",
"..#.#",
"...##",
".####"};

static const char* const not_close_xpm[]={
"5 5 2 1",
"# c black",
". c None",
"#####",
"#...#",
"#...#",
"#...#",
"#####"};

class PMDockMainWindowPrivate
{
public:
  PMDockMainWindowPrivate()
  {
    m_activePart = 0;
    m_bShellGUIActivated = false;
    m_helpMenu = 0;
  }
  ~PMDockMainWindowPrivate()
  {
  }

  QGuardedPtr<Part> m_activePart;
  bool m_bShellGUIActivated;
  KHelpMenu *m_helpMenu;
};


/**
 * A special kind of KMainWindow that is able to have dockwidget child widgets.
 *
 * The main widget should be a dockwidget where other dockwidgets can be docked to
 * the left, right, top, bottom or to the middle.
 * Furthermore, the PMDockMainWindow has got the KDocManager and some data about the dock states.
 *
 * @author Max Judin.
*/
PMDockMainWindow::PMDockMainWindow( QWidget* parent, const char *name, WFlags f)
:KMainWindow( parent, name, f )
{
  QString new_name = QString(name) + QString("_DockManager");
  dockManager = new PMDockManager( this, new_name.latin1() );
  mainDockWidget = 0L;

  d = new PMDockMainWindowPrivate( );
  PartBase::setPartObject( this );
}

PMDockMainWindow::~PMDockMainWindow()
{
   delete dockManager;
   delete d;
}

// kparts/dockmainwindow stuff
void PMDockMainWindow::createGUI( Part * part )
{
  kdDebug(1000) << QString("DockMainWindow::createGUI for %1").arg(part?part->name():"0L") << endl;

  KXMLGUIFactory *factory = guiFactory();

  setUpdatesEnabled( false );

  QPtrList<Plugin> plugins;

  if ( d->m_activePart )
  {
    kdDebug(1000) << QString("deactivating GUI for %1").arg(d->m_activePart->name()) << endl;

    GUIActivateEvent ev( false );
    QApplication::sendEvent( d->m_activePart, &ev );

    factory->removeClient( d->m_activePart );

    disconnect( d->m_activePart, SIGNAL( setWindowCaption( const QString & ) ),
             this, SLOT( setCaption( const QString & ) ) );
    disconnect( d->m_activePart, SIGNAL( setStatusBarText( const QString & ) ),
             this, SLOT( slotSetStatusBarText( const QString & ) ) );
  }

  if( !d->m_bShellGUIActivated )
  {
    loadPlugins( this, this, KGlobal::instance() );
    createShellGUI();
    d->m_bShellGUIActivated = true;
  }

  if ( part )
  {
    // do this before sending the activate event
    connect( part, SIGNAL( setWindowCaption( const QString & ) ),
             this, SLOT( setCaption( const QString & ) ) );
    connect( part, SIGNAL( setStatusBarText( const QString & ) ),
             this, SLOT( slotSetStatusBarText( const QString & ) ) );

    factory->addClient( part );

    GUIActivateEvent ev( true );
    QApplication::sendEvent( part, &ev );

  }

  setUpdatesEnabled( true );

  d->m_activePart = part;
}

void PMDockMainWindow::slotSetStatusBarText( const QString & text )
{
  statusBar()->message( text );
}

void PMDockMainWindow::createShellGUI( bool create )
{
    bool bAccelAutoUpdate = accel()->setAutoUpdate( false );
    assert( d->m_bShellGUIActivated != create );
    d->m_bShellGUIActivated = create;

    if ( create )
    {
       if ( isHelpMenuEnabled() && !d->m_helpMenu )
            d->m_helpMenu = new KHelpMenu( this, instance()->aboutData(), true, actionCollection() );

        QString f = xmlFile();
        setXMLFile( locate( "config", "ui/ui_standards.rc", instance() ) );
        if ( !f.isEmpty() )
            setXMLFile( f, true );
        else
        {
            QString auto_file( instance()->instanceName() + "ui.rc" );
            setXMLFile( auto_file, true );
        }

        GUIActivateEvent ev( true );
        QApplication::sendEvent( this, &ev );

        guiFactory()->addClient( this );

    }
    else
    {
        GUIActivateEvent ev( false );
        QApplication::sendEvent( this, &ev );

        guiFactory()->removeClient( this );
    }
    accel()->setAutoUpdate( bAccelAutoUpdate );
}
// end kparts/dockmainwindow

void PMDockMainWindow::setMainDockWidget( PMDockWidget* mdw )
{
  if ( mainDockWidget == mdw ) return;
  mainDockWidget = mdw;
}

void PMDockMainWindow::setView( QWidget *view )
{
  if ( view->isA("PMDockWidget") ){
    if ( view->parent() != this ) ((PMDockWidget*)view)->applyToWidget( this );
  }

#ifndef NO_KDE2
  KMainWindow::setCentralWidget(view);
#else
  QMainWindow::setCentralWidget(view);
#endif
}

PMDockWidget* PMDockMainWindow::createDockWidget( const QString& name, const QPixmap &pixmap, QWidget* parent, const QString& strCaption, const QString& strTabPageLabel)
{
  return new PMDockWidget( dockManager, name.latin1(), pixmap, parent, strCaption, strTabPageLabel );
}

void PMDockMainWindow::makeDockVisible( PMDockWidget* dock )
{
  if ( dock != 0L)
    dock->makeDockVisible();
}

void PMDockMainWindow::makeDockInvisible( PMDockWidget* dock )
{
  if ( dock != 0L)
    dock->undock();
}

void PMDockMainWindow::makeWidgetDockVisible( QWidget* widget )
{
  makeDockVisible( dockManager->findWidgetParentDock(widget) );
}

void PMDockMainWindow::writeDockConfig(QDomElement &base)
{
  dockManager->writeConfig(base);
}

void PMDockMainWindow::readDockConfig(QDomElement &base)
{
  dockManager->readConfig(base);
}

#ifndef NO_KDE2
void PMDockMainWindow::writeDockConfig( KConfig* c, QString group )
{
  dockManager->writeConfig( c, group );
}

void PMDockMainWindow::readDockConfig( KConfig* c, QString group )
{
  dockManager->readConfig( c, group );
}
#endif

void PMDockMainWindow::slotDockWidgetUndocked()
{
  QObject* pSender = (QObject*) sender();
  if (!pSender->inherits("PMDockWidget")) return;
  PMDockWidget* pDW = (PMDockWidget*) pSender;
  emit dockWidgetHasUndocked( pDW);
}

/*************************************************************************/
PMDockWidgetAbstractHeaderDrag::PMDockWidgetAbstractHeaderDrag( PMDockWidgetAbstractHeader* parent, PMDockWidget* dock, const char* name )
:QFrame( parent, name )
{
  dw = dock;
  installEventFilter( dock->dockManager() );
}
/*************************************************************************/
PMDockWidgetHeaderDrag::PMDockWidgetHeaderDrag( PMDockWidgetAbstractHeader* parent, PMDockWidget* dock, const char* name )
:PMDockWidgetAbstractHeaderDrag( parent, dock, name )
{
}

void PMDockWidgetHeaderDrag::paintEvent( QPaintEvent* )
{
  QPainter paint;

  paint.begin( this );

  style().drawPrimitive (QStyle::PE_DockWindowHandle, &paint, QRect(0,0,width(), height()), colorGroup());

  paint.end();
}
/*************************************************************************/
PMDockWidgetAbstractHeader::PMDockWidgetAbstractHeader( PMDockWidget* parent, const char* name )
:QFrame( parent, name )
{
}
/*************************************************************************/
PMDockWidgetHeader::PMDockWidgetHeader( PMDockWidget* parent, const char* name )
:PMDockWidgetAbstractHeader( parent, name )
{
  layout = new QHBoxLayout( this );
  layout->setResizeMode( QLayout::Minimum );

  drag = new PMDockWidgetHeaderDrag( this, parent );

  closeButton = new PMDockButton_Private( this, "DockCloseButton" );
  closeButton->setPixmap( const_cast< const char** >(close_xpm) );
  int buttonWidth = 9, buttonHeight = 9;
  closeButton->setFixedSize(buttonWidth,buttonHeight);
  connect( closeButton, SIGNAL(clicked()), parent, SIGNAL(headerCloseButtonClicked()));
  // MODIFICATION (zehender)
  // The shell will delete the widget
  // undock is unnecessary
  // connect( closeButton, SIGNAL(clicked()), parent, SLOT(undock()));

  stayButton = new PMDockButton_Private( this, "DockStayButton" );
  stayButton->setToggleButton( true );
  stayButton->setPixmap( const_cast< const char** >(not_close_xpm) );
  stayButton->setFixedSize(buttonWidth,buttonHeight);
  connect( stayButton, SIGNAL(clicked()), this, SLOT(slotStayClicked()));
  stayButton->hide( );

  dockbackButton = new PMDockButton_Private( this, "DockbackButton" );
  dockbackButton->setPixmap( const_cast< const char** >(dockback_xpm));
  dockbackButton->setFixedSize(buttonWidth,buttonHeight);
  connect( dockbackButton, SIGNAL(clicked()), parent, SIGNAL(headerDockbackButtonClicked()));
  connect( dockbackButton, SIGNAL(clicked()), parent, SLOT(dockBack()));

  // MODIFICATION (zehender)
  // Add a button to undock the widget and dock it as top level
  // widget to the desktop
  toDesktopButton = new PMDockButton_Private( this, "ToDesktopButton" );
  toDesktopButton->setPixmap( const_cast< const char** >(todesktop_xpm));
  toDesktopButton->setFixedSize(buttonWidth,buttonHeight);
  connect( toDesktopButton, SIGNAL(clicked()), parent, SLOT(toDesktop()));

  layout->addWidget( drag );
  layout->addWidget( dockbackButton );
  layout->addWidget( toDesktopButton );
  layout->addWidget( stayButton );
  layout->addWidget( closeButton );
  layout->activate();
  drag->setFixedHeight( layout->minimumSize().height() );
}

void PMDockWidgetHeader::setTopLevel( bool isTopLevel )
{
  if ( isTopLevel ){
    PMDockWidget* par = (PMDockWidget*)parent();
    if( par) {
      if( par->isDockBackPossible())
        dockbackButton->show();
      else
        dockbackButton->hide();
    }
    stayButton->hide();
    closeButton->hide();
    toDesktopButton->hide();
    drag->setEnabled( true );
  } else {
    dockbackButton->hide();
    stayButton->hide();
    closeButton->show();
    toDesktopButton->show();
  }
  layout->activate();
  updateGeometry();
}

void PMDockWidgetHeader::setDragPanel( PMDockWidgetHeaderDrag* nd )
{
  if ( !nd ) return;

  delete layout;
  layout = new QHBoxLayout( this );
  layout->setResizeMode( QLayout::Minimum );

  delete drag;
  drag = nd;

  layout->addWidget( drag );
  layout->addWidget( dockbackButton );
  layout->addWidget( toDesktopButton );
  layout->addWidget( stayButton );
  layout->addWidget( closeButton );
  layout->activate();
  drag->setFixedHeight( layout->minimumSize().height() );
}

void PMDockWidgetHeader::slotStayClicked()
{
  setDragEnabled(!stayButton->isOn());
}

bool PMDockWidgetHeader::dragEnabled() const
{
  return drag->isEnabled();
}

void PMDockWidgetHeader::setDragEnabled(bool b)
{
  stayButton->setOn(!b);
  closeButton->setEnabled(b);
  drag->setEnabled(b);
}

#ifndef NO_KDE2
void PMDockWidgetHeader::saveConfig( KConfig* c )
{
  c->writeEntry( QString("%1%2").arg(parent()->name()).arg(":stayButton"), stayButton->isOn() );
}

void PMDockWidgetHeader::loadConfig( KConfig* c )
{
  setDragEnabled( !c->readBoolEntry( QString("%1%2").arg(parent()->name()).arg(":stayButton"), false ) );
}
#endif

/*************************************************************************/
PMDockWidget::PMDockWidget( PMDockManager* dockManager, const char* name, const QPixmap &pixmap, QWidget* parent, const QString& strCaption, const QString& strTabPageLabel, WFlags f)
: QWidget( parent, name, f )
  ,formerBrotherDockWidget(0L)
  ,currentDockPos(DockNone)
  ,formerDockPos(DockNone)
  ,pix(new QPixmap(pixmap))
  ,prevSideDockPosBeforeDrag(DockNone)
{
  d = new PMDockWidgetPrivate();  // create private data

  d->_parent = parent;

  layout = new QVBoxLayout( this );
  layout->setResizeMode( QLayout::Minimum );

  manager = dockManager;
  manager->childDock->append( this );
  installEventFilter( manager );

  header = 0L;
  setHeader( new PMDockWidgetHeader( this, "AutoCreatedDockHeader" ) );
  if( strCaption == 0L)
    setCaption( name );
  else
    setCaption( strCaption);

  if( strTabPageLabel == " ")
    setTabPageLabel( caption());
  else
    setTabPageLabel( strTabPageLabel);

  eDocking = DockFullDocking;
  sDocking = DockFullSite;

  isGroup = false;
  isTabGroup = false;

  setIcon( pixmap);
  widget = 0L;

  QObject::connect(this, SIGNAL(hasUndocked()), manager->main, SLOT(slotDockWidgetUndocked()) );
  applyToWidget( parent, QPoint(0,0) );
}

void PMDockWidget::slotSetCaption( const QString& str )
{
   setTabPageLabel( str );
   setCaption( str );
}

PMDockWidget::~PMDockWidget()
{
  if ( !manager->undockProcess ){
    d->blockHasUndockedSignal = true;
    undock();
    d->blockHasUndockedSignal = false;
  }
  emit iMBeingClosed();
  manager->childDock->remove( this );
  delete pix;
  delete d; // destroy private data
}

void PMDockWidget::setHeader( PMDockWidgetAbstractHeader* h )
{
  if ( !h ) return;

  if ( header ){
    delete header;
    delete layout;
    header = h;
    layout = new QVBoxLayout( this );
    layout->setResizeMode( QLayout::Minimum );
    layout->addWidget( header );
     setWidget( widget );
  } else {
    header = h;
    layout->addWidget( header );
  }
}

void PMDockWidget::setEnableDocking( int pos )
{
  eDocking = pos;
  updateHeader();
}

void PMDockWidget::updateHeader()
{
  if ( parent() ){
    if ( (parent() == manager->main) || isGroup || (eDocking == PMDockWidget::DockNone) ){
      header->hide();
    } else {
      header->setTopLevel( false );
      header->show();
    }
  } else {
    header->setTopLevel( true );
    header->show();
  }
}

void PMDockWidget::applyToWidget( QWidget* s, const QPoint& p )
{
  if ( parent() != s )
  {
    hide();
    reparent(s, 0, QPoint(0,0), false);
  }

  if ( s && s->inherits("PMDockMainWindow") ){
    ((PMDockMainWindow*)s)->setView( this );
  }

  if ( s == manager->main ){
      setGeometry( QRect(QPoint(0,0), manager->main->geometry().size()) );
  }

  if ( !s )
  {
    move(p);

#ifndef NO_KDE2
#ifdef Q_WS_X11
    if (d->transient && d->_parent)
      XSetTransientForHint( qt_xdisplay(), winId(), d->_parent->winId() );

    KWin::setType( winId(), d->windowType );
#endif
#endif

  }
  updateHeader();

  setIcon(*pix);
}

void PMDockWidget::show()
{
  if ( parent() || manager->main->isVisible() )
    if ( !parent() ){
     emit manager->setDockDefaultPos( this );
     emit setDockDefaultPos();
     if ( parent() ){
        makeDockVisible();
      } else {
        QWidget::show();
      }
    } else {
     QWidget::show();
    }
}

#ifndef NO_KDE2

void PMDockWidget::setDockWindowType (NET::WindowType windowType)
{
  d->windowType = windowType;
  applyToWidget( parentWidget(), QPoint(0,0) );
}

#endif

void PMDockWidget::setDockWindowTransient (QWidget *parent, bool transientEnabled)
{
  d->_parent = parent;
  d->transient = transientEnabled;
  applyToWidget( parentWidget(), QPoint(0,0) );
}

bool PMDockWidget::event( QEvent* pevent )
{
  switch ( pevent->type() )
  {
    #undef FocusIn
    case QEvent::FocusIn:
      if (widget && !d->pendingFocusInEvent) {
         d->pendingFocusInEvent = true;
         widget->setFocus();
      }
      d->pendingFocusInEvent = false;
      break;
    case QEvent::ChildRemoved:
      if ( widget == ((QChildEvent*)pevent)->child() ) widget = 0L;
      break;
    case QEvent::Show:
      if ( widget ) widget->show();
      emit manager->change();
      break;
    case QEvent::Hide:
      if ( widget ) widget->hide();
      emit manager->change();
      break;
    case QEvent::CaptionChange:
      if ( parentWidget() ){
        if ( parent()->inherits("PMDockSplitter") ){
          ((PMDockSplitter*)(parent()))->updateName();
        }
        if ( parentDockTabGroup() ){
          setDockTabName( parentDockTabGroup() );
          parentDockTabGroup()->setTabLabel( this, tabPageLabel() );
        }
      }
      break;
    case QEvent::Close:
      // MODIFICATION (zehender)
      // Top level widget is closed
      // emit same signal as if the widget is docked and closed with
      // the header button
      emit headerCloseButtonClicked( );
      // emit iMBeingClosed();
      break;
    default:
      break;
  }
  #undef KeyPress
  bool processed = QWidget::event( pevent );
  if( pevent->type( ) == QEvent::AccelOverride && !processed && !parent( ) ){
	  // MODIFICATION (zehender)
	  // floating widget, post event to main window
	  processed = qApp->sendEvent( manager->dockMainWidget( ), pevent );
  }
  return processed;
}

PMDockWidget* PMDockWidget::manualDock( PMDockWidget* target, DockPosition dockPos, int spliPos, QPoint pos, bool check, int tabIndex )
{
  if (this == target)
    return 0L;  // docking to itself not possible

  bool succes = true; // tested flag

  // do not dock into a floating widget
  if( target && !target->parent( ) )
     target = 0;

  // check allowed this dock submit this operations
  if ( !(eDocking & (int)dockPos) ){
    succes = false;
  }

  // check allowed target submit this operations
  if ( target && !(target->dockSite( ) & (int)dockPos) ){
    succes = false;
  }

  if ( parent() && !parent()->inherits("PMDockSplitter") && !parentDockTabGroup() ){
    succes = false;
  }

  // if docking to a tab group, and position is not center
  // dock to the parent of the tab group
  if( target && ( dockPos != PMDockWidget::DockCenter )
      && ( dockPos != PMDockWidget::DockNone ) )
  {
     QWidget* pdt = target->parentDockTabGroup( );
     if( pdt )
        return manualDock( ( PMDockWidget* ) ( pdt->parent( ) ),
                           dockPos, spliPos, pos, check, tabIndex );
  }

  if ( !succes ){
    // try to make another manualDock
    PMDockWidget* dock_result = 0L;
    if ( target && !check ){
      PMDockWidget::DockPosition another__dockPos = PMDockWidget::DockNone;
      switch ( dockPos ){
        case PMDockWidget::DockLeft  : another__dockPos = PMDockWidget::DockRight ; break;
        case PMDockWidget::DockRight : another__dockPos = PMDockWidget::DockLeft  ; break;
        case PMDockWidget::DockTop   : another__dockPos = PMDockWidget::DockBottom; break;
        case PMDockWidget::DockBottom: another__dockPos = PMDockWidget::DockTop   ; break;
        default: break;
      }
      dock_result = target->manualDock( this, another__dockPos, spliPos, pos, true, tabIndex );
    }
    return dock_result;
  }
  // end check block

  d->blockHasUndockedSignal = true;
  undock();
  d->blockHasUndockedSignal = false;

  if ( !target ){
    move( pos );
    show();
    emit manager->change();
    return this;
  }

  PMDockTabGroup* parentTab = target->parentDockTabGroup();
  if ( parentTab ){
    // add to existing TabGroup
    applyToWidget( parentTab );
    parentTab->insertTab( this, icon() ? *icon() : QPixmap(),
                          tabPageLabel(), tabIndex );
    setDockTabName( parentTab );
    if( !toolTipStr.isEmpty())
      parentTab->setTabToolTip( this, toolTipStr);

    currentDockPos = PMDockWidget::DockCenter;
    emit manager->change();
    return (PMDockWidget*)parentTab->parent();
  }

  // MODIFICATION (Zehender):
  // If DockPosition is DockLeft or DockRight, add the widget
  // left or right of the target, so that a new vertical splitter
  // (a splitter with horizontal widget layout :-) is created
  // that spawns the full height of the main view

  if( ( dockPos == PMDockWidget::DockLeft ) ||
      ( dockPos == PMDockWidget::DockRight ) )
  {
     PMDockWidget* newTarget = target;
     bool found = false;
     QWidget* wtarget = target;
     while( wtarget && !found )
     {
        if( wtarget->inherits( "PMDockWidget" ) )
        {
           PMDockWidget* dw = ( PMDockWidget* ) wtarget;
           newTarget = dw;
           QWidget* pw = wtarget->parentWidget( );
           if( pw )
           {
              if( pw->inherits( "PMDockSplitter" ) )
              {
                 PMDockSplitter* ds = ( PMDockSplitter* ) pw;
                 if( ds->splitterOrientation( ) == Vertical )
                    found = true;
              }
           }
        }
        wtarget = wtarget->parentWidget( );
     }

     if( newTarget != target )
        return manualDock( newTarget, dockPos, spliPos, pos, check, tabIndex );
  }

  // END MODIFICATION

  // create a new dockwidget that will contain the target and this
  QWidget* parentDock = target->parentWidget();
  PMDockWidget* newDock = new PMDockWidget( manager, "tempName", QPixmap(""), parentDock );
  newDock->currentDockPos = target->currentDockPos;

  if ( dockPos == PMDockWidget::DockCenter ){
    newDock->isTabGroup = true;
  } else {
    newDock->isGroup = true;
  }
  newDock->eDocking = (target->eDocking & eDocking) & (~(int)PMDockWidget::DockCenter);

  newDock->applyToWidget( parentDock );

  if ( !parentDock ){
    // dock to a toplevel dockwidget means newDock is toplevel now
    newDock->move( target->frameGeometry().topLeft() );
    newDock->resize( target->geometry().size() );
    if ( target->isVisibleToTLW() ) newDock->show();
  }

  // redirect the dockback button to the new dockwidget
  if( target->formerBrotherDockWidget != 0L) {
    newDock->formerBrotherDockWidget = target->formerBrotherDockWidget;
    if( formerBrotherDockWidget != 0L)
      QObject::connect( newDock->formerBrotherDockWidget, SIGNAL(iMBeingClosed()),
                        newDock, SLOT(loseFormerBrotherDockWidget()) );
      target->loseFormerBrotherDockWidget();
    }
  newDock->formerDockPos = target->formerDockPos;

  if ( dockPos == PMDockWidget::DockCenter )
  {
    PMDockTabGroup* tab = new PMDockTabGroup( newDock, "_dock_tab");
    QObject::connect(tab, SIGNAL(currentChanged(QWidget*)), d, SLOT(slotFocusEmbeddedWidget(QWidget*)));
    newDock->setWidget( tab );

    target->applyToWidget( tab );
    applyToWidget( tab );


    tab->insertTab( target, target->icon() ? *(target->icon()) : QPixmap(),
                    target->tabPageLabel() );
    if( !target->toolTipString().isEmpty())
      tab->setTabToolTip( target, target->toolTipString());

    tab->insertTab( this, icon() ? *icon() : QPixmap(),
                    tabPageLabel(), tabIndex );
    if( !toolTipString().isEmpty())
      tab->setTabToolTip( this, toolTipString());

    setDockTabName( tab );
    tab->show();

    currentDockPos = DockCenter;
    target->formerDockPos = target->currentDockPos;
    target->currentDockPos = DockCenter;
  }
  else {
    // if to dock not to the center of the target dockwidget,
    // dock to newDock
    PMDockSplitter* panner = 0L;
    if ( dockPos == PMDockWidget::DockTop  || dockPos == PMDockWidget::DockBottom ) panner = new PMDockSplitter( newDock, "_dock_split_", Horizontal, spliPos, manager->splitterHighResolution() );
    if ( dockPos == PMDockWidget::DockLeft || dockPos == PMDockWidget::DockRight  ) panner = new PMDockSplitter( newDock, "_dock_split_", Vertical , spliPos, manager->splitterHighResolution() );
    newDock->setWidget( panner );

    panner->setOpaqueResize(manager->splitterOpaqueResize());
    panner->setKeepSize(manager->splitterKeepSize());
    panner->setFocusPolicy( NoFocus );
    target->applyToWidget( panner );
    applyToWidget( panner );
    target->formerDockPos = target->currentDockPos;
    if ( dockPos == PMDockWidget::DockRight) {
      panner->activate( target, this );
      currentDockPos = PMDockWidget::DockRight;
      target->currentDockPos = PMDockWidget::DockLeft;
    }
    else if( dockPos == PMDockWidget::DockBottom) {
      panner->activate( target, this );
      currentDockPos = PMDockWidget::DockBottom;
      target->currentDockPos = PMDockWidget::DockTop;
    }
    else if( dockPos == PMDockWidget::DockTop) {
      panner->activate( this, target );
      currentDockPos = PMDockWidget::DockTop;
      target->currentDockPos = PMDockWidget::DockBottom;
    }
    else if( dockPos == PMDockWidget::DockLeft) {
      panner->activate( this, target );
      currentDockPos = PMDockWidget::DockLeft;
      target->currentDockPos = PMDockWidget::DockRight;
    }
    target->show();
    show();
    panner->show();
  }

  if ( parentDock ){
    if ( parentDock->inherits("PMDockSplitter") ){
      PMDockSplitter* sp = (PMDockSplitter*)parentDock;
      sp->deactivate();
      if ( sp->getFirst() == target )
        sp->activate( newDock, 0L );
      else
        sp->activate( 0L, newDock );
    }
  }

  newDock->show();
  emit target->docking( this, dockPos );
  emit manager->replaceDock( target, newDock );
  emit manager->change();

  return newDock;
}

PMDockTabGroup* PMDockWidget::parentDockTabGroup() const
{
  if ( !parent() ) return 0L;
  QWidget* candidate = parentWidget()->parentWidget();
  if ( candidate && candidate->inherits("PMDockTabGroup") ) return (PMDockTabGroup*)candidate;
  return 0L;
}

void PMDockWidget::toDesktop()
{
   QPoint p = mapToGlobal( QPoint( -30, -30 ) );
   if( p.x( ) < 0 )
      p.setX( 0 );
   if( p.y( ) < 0 )
      p.setY( 0 );
   manualDock( 0, DockDesktop, 50, p );
}

void PMDockWidget::undock()
{
  QWidget* parentW = parentWidget();
  if ( !parentW ){
    hide();
    if (!d->blockHasUndockedSignal)
      emit hasUndocked();
    return;
  }

  formerDockPos = currentDockPos;
  currentDockPos = PMDockWidget::DockDesktop;

  manager->blockSignals(true);
  manager->undockProcess = true;

  bool isV = parentW->isVisibleToTLW();

  PMDockTabGroup* parentTab = parentDockTabGroup();
  if ( parentTab ){
    d->index = parentTab->indexOf( this); // memorize the page position in the tab widget
    parentTab->removePage( this );
    formerBrotherDockWidget = (PMDockWidget*)parentTab->page(0);
    QObject::connect( formerBrotherDockWidget, SIGNAL(iMBeingClosed()),
                      this, SLOT(loseFormerBrotherDockWidget()) );
    applyToWidget( 0L );
    if ( parentTab->count() == 1 ){

      // last subdock widget in the tab control
      PMDockWidget* lastTab = (PMDockWidget*)parentTab->page(0);
      parentTab->removePage( lastTab );
      lastTab->applyToWidget( 0L );
      lastTab->move( parentTab->mapToGlobal(parentTab->frameGeometry().topLeft()) );

      // PMDockTabGroup always have a parent that is a PMDockWidget
      PMDockWidget* parentOfTab = (PMDockWidget*)parentTab->parent();
      delete parentTab; // PMDockTabGroup

      QWidget* parentOfDockWidget = parentOfTab->parentWidget();
      if ( parentOfDockWidget == 0L ){
          if ( isV ) lastTab->show();
      } else {
        if ( parentOfDockWidget->inherits("PMDockSplitter") ){
          PMDockSplitter* split = (PMDockSplitter*)parentOfDockWidget;
          lastTab->applyToWidget( split );
          split->deactivate();
          if ( split->getFirst() == parentOfTab ){
            split->activate( lastTab );
            if ( ((PMDockWidget*)split->parent())->splitterOrientation == Vertical )
              emit ((PMDockWidget*)split->getAnother(parentOfTab))->docking( parentOfTab, PMDockWidget::DockLeft );
            else
              emit ((PMDockWidget*)split->getAnother(parentOfTab))->docking( parentOfTab, PMDockWidget::DockTop );
          } else {
            split->activate( 0L, lastTab );
            if ( ((PMDockWidget*)split->parent())->splitterOrientation == Vertical )
              emit ((PMDockWidget*)split->getAnother(parentOfTab))->docking( parentOfTab, PMDockWidget::DockRight );
            else
              emit ((PMDockWidget*)split->getAnother(parentOfTab))->docking( parentOfTab, PMDockWidget::DockBottom );
          }
          split->show();
        } else {
          lastTab->applyToWidget( parentOfDockWidget );
        }
        lastTab->show();
      }
      manager->blockSignals(false);
      emit manager->replaceDock( parentOfTab, lastTab );
      lastTab->currentDockPos = parentOfTab->currentDockPos;
      emit parentOfTab->iMBeingClosed();
      manager->blockSignals(true);
      delete parentOfTab;

    } else {
      setDockTabName( parentTab );
    }
  } else {
/*********************************************************************************************/
    if ( parentW->inherits("PMDockSplitter") ){
      PMDockSplitter* parentSplitterOfDockWidget = (PMDockSplitter*)parentW;
      d->splitPosInPercent = parentSplitterOfDockWidget->separatorPos();

      PMDockWidget* secondWidget = (PMDockWidget*)parentSplitterOfDockWidget->getAnother( this );
      PMDockWidget* group        = (PMDockWidget*)parentSplitterOfDockWidget->parentWidget();
      formerBrotherDockWidget = secondWidget;
      applyToWidget( 0L );
      group->hide();

      if( formerBrotherDockWidget != 0L)
        QObject::connect( formerBrotherDockWidget, SIGNAL(iMBeingClosed()),
                          this, SLOT(loseFormerBrotherDockWidget()) );

      if ( !group->parentWidget() ){
        secondWidget->applyToWidget( 0L, group->frameGeometry().topLeft() );
        secondWidget->resize( group->width(), group->height() );
      } else {
        QWidget* obj = group->parentWidget();
        secondWidget->applyToWidget( obj );
        if ( obj->inherits("PMDockSplitter") ){
          PMDockSplitter* parentOfGroup = (PMDockSplitter*)obj;
          parentOfGroup->deactivate();

          if ( parentOfGroup->getFirst() == group )
            parentOfGroup->activate( secondWidget );
          else
            parentOfGroup->activate( 0L, secondWidget );
        }
      }
      secondWidget->currentDockPos = group->currentDockPos;
      secondWidget->formerDockPos  = group->formerDockPos;
      delete parentSplitterOfDockWidget;
      manager->blockSignals(false);
      emit manager->replaceDock( group, secondWidget );
      emit group->iMBeingClosed();
      manager->blockSignals(true);
      delete group;

      if ( isV ) secondWidget->show();
    } else {
      applyToWidget( 0L );
    }
/*********************************************************************************************/
  }
  manager->blockSignals(false);
  if (!d->blockHasUndockedSignal)
    emit manager->change();
  manager->undockProcess = false;

  if (!d->blockHasUndockedSignal)
    emit hasUndocked();
}

void PMDockWidget::setWidget( QWidget* mw )
{
  if ( !mw ) return;

  if ( mw->parent() != this ){
    mw->reparent(this, 0, QPoint(0,0), false);
  }

  widget = mw;
  delete layout;

  layout = new QVBoxLayout( this );
  layout->setResizeMode( QLayout::Minimum );

  layout->addWidget( header );
  layout->addWidget( widget,1 );
}

void PMDockWidget::setDockTabName( PMDockTabGroup* tab )
{
  QString listOfName;
  QString listOfCaption;
  for ( int i = 0; i < tab->count(); ++i ) {
    QWidget *w = tab->page( i );
    listOfCaption.append( w->caption() ).append(",");
    listOfName.append( w->name() ).append(",");
  }
  listOfCaption.remove( listOfCaption.length()-1, 1 );
  listOfName.remove( listOfName.length()-1, 1 );

  tab->parentWidget()->setName( listOfName.utf8() );
  tab->parentWidget()->setCaption( listOfCaption );

  tab->parentWidget()->repaint( false ); // PMDockWidget->repaint
  if ( tab->parentWidget()->parent() )
    if ( tab->parentWidget()->parent()->inherits("PMDockSplitter") )
      ((PMDockSplitter*)(tab->parentWidget()->parent()))->updateName();
}

bool PMDockWidget::mayBeHide() const
{
  bool f = (parent() != manager->main);
  return ( !isGroup && !isTabGroup && f && isVisible() && ( eDocking != (int)PMDockWidget::DockNone ) );
}

bool PMDockWidget::mayBeShow() const
{
  bool f = (parent() != manager->main);
  return ( !isGroup && !isTabGroup && f && !isVisible() );
}

void PMDockWidget::changeHideShowState()
{
  if ( mayBeHide() ){
    undock();
    return;
  }

  if ( mayBeShow() ){
    if ( manager->main->inherits("PMDockMainWindow") ){
      ((PMDockMainWindow*)manager->main)->makeDockVisible(this);
    } else {
      makeDockVisible();
    }
  }
}

void PMDockWidget::makeDockVisible()
{
  if ( parentDockTabGroup() ){
    parentDockTabGroup()->showPage( this );
  }
  if ( isVisible() ) return;

  QWidget* p = parentWidget();
  while ( p ){
    if ( !p->isVisible() )
      p->show();
    p = p->parentWidget();
  }
  if( parent() == 0L) // is undocked
    dockBack();
  show();
}

void PMDockWidget::loseFormerBrotherDockWidget()
{
  if( formerBrotherDockWidget != 0L)
    QObject::disconnect( formerBrotherDockWidget, SIGNAL(iMBeingClosed()),
                         this, SLOT(loseFormerBrotherDockWidget()) );
  formerBrotherDockWidget = 0L;
  repaint();
}

void PMDockWidget::dockBack()
{
  if( formerBrotherDockWidget) {
    // search all children if it tries to dock back to a child
    bool found = false;
    QObjectList* cl = queryList("PMDockWidget");
    QObjectListIt it( *cl );
    QObject * obj;
    while ( !found && (obj=it.current()) != 0 ) {
      ++it;
      QWidget* widg = (QWidget*)obj;
      if( widg == formerBrotherDockWidget)
        found = true;
    }
    delete cl;

    if( !found) {
      // can dock back to the former brother dockwidget
      manualDock( formerBrotherDockWidget, formerDockPos, d->splitPosInPercent, QPoint(0,0), false, d->index);
      formerBrotherDockWidget = 0L;
      makeDockVisible();
      return;
    }
  }

  // else dockback to the dockmainwindow (default behaviour)
  manualDock( ((PMDockMainWindow*)manager->main)->getMainDockWidget(), formerDockPos, d->splitPosInPercent, QPoint(0,0), false, d->index);
  formerBrotherDockWidget = 0L;
  if (parent())
    makeDockVisible();
}

bool PMDockWidget::isDockBackPossible() const
{
  if( (formerBrotherDockWidget == 0L) || !(formerBrotherDockWidget->dockSite() & formerDockPos))
    return false;
  else
    return true;
}

/**************************************************************************************/

class PMDockManager::PMDockManagerPrivate
{
public:
  /**
   * This rectangle is used to highlight the current dockposition. It stores global screen coordinates.
   */
  QRect dragRect;

  /**
   * This rectangle is used to erase the previously highlighted dockposition. It stores global screen coordinates.
   */
  QRect oldDragRect;

  /**
   * This flag stores the information if dragging is ready to start. Used between mousePress and mouseMove event.
   */
  bool readyToDrag;

  /**
   * This variable stores the offset of the mouse cursor to the upper left edge of the current drag widget.
   */
  QPoint dragOffset;

  /**
   * These flags store information about the splitter behaviour
   */
  bool splitterOpaqueResize;
  bool splitterKeepSize;
  bool splitterHighResolution;
};

PMDockManager::PMDockManager( QWidget* mainWindow , const char* name )
:QObject( mainWindow, name )
  ,main(mainWindow)
  ,currentDragWidget(0L)
  ,currentMoveWidget(0L)
  ,childDockWidgetList(0L)
  ,autoCreateDock(0L)
  ,storeW(0)
  ,storeH(0)
  ,draging(false)
  ,undockProcess(false)
  ,dropCancel(true)
{
  d = new PMDockManagerPrivate;
  d->splitterOpaqueResize = false;
  d->splitterKeepSize = false;
  d->splitterHighResolution = false;

  main->installEventFilter( this );

  undockProcess = false;

  menuData = new QPtrList<MenuDockData>;
  menuData->setAutoDelete( true );
  menuData->setAutoDelete( true );

#ifndef NO_KDE2
  menu = new KPopupMenu();
#else
  menu = new QPopupMenu();
#endif

  connect( menu, SIGNAL(aboutToShow()), SLOT(slotMenuPopup()) );
  connect( menu, SIGNAL(activated(int)), SLOT(slotMenuActivated(int)) );

  childDock = new QObjectList();
  childDock->setAutoDelete( false );
}

PMDockManager::~PMDockManager()
{
  delete menuData;
  delete menu;

  QObjectListIt it( *childDock );
  PMDockWidget * obj;

  while ( (obj=(PMDockWidget*)it.current()) ) {
    delete obj;
  }
  delete childDock;
  delete d;
}

void PMDockManager::activate()
{
  QObjectListIt it( *childDock );
  PMDockWidget * obj;

  while ( (obj=(PMDockWidget*)it.current()) ) {
    ++it;
    if ( obj->widget ) obj->widget->show();
    if ( !obj->parentDockTabGroup() ){
        obj->show();
    }
  }
  if ( !main->inherits("QDialog") ) main->show();
}

bool PMDockManager::eventFilter( QObject *obj, QEvent *event )
{
/* This doesn't seem to fullfill any sense, other than breaking
   QMainWindow's layout all over the place
   The first child of the mainwindow is not necessarily a meaningful
   content widget but in Qt3's QMainWindow it can easily be a QToolBar.
   In short: QMainWindow knows how to layout its children, no need to
   mess that up.

   >>>>>I need this in the PMDockArea at the moment (JoWenn)

  if ( obj == main && event->type() == QEvent::Resize && dynamic_cast<PMDockArea*>(main) && main->children() ){
#ifndef NO_KDE2
    kdDebug()<<"PMDockManager::eventFilter(): main is a PMDockArea and there are children"<<endl;
#endif
    QWidget* fc = (QWidget*)main->children()->getFirst();
    if ( fc )
      fc->setGeometry( QRect(QPoint(0,0), main->geometry().size()) );
  }
*/

  if ( obj->inherits("PMDockWidgetAbstractHeaderDrag") ){
    PMDockWidget* pDockWdgAtCursor = 0L;
    PMDockWidget* curdw = ((PMDockWidgetAbstractHeaderDrag*)obj)->dockWidget();
    switch ( event->type() ){
      case QEvent::MouseButtonDblClick:
        if (curdw->currentDockPos == PMDockWidget::DockDesktop)  curdw->dockBack();
        else curdw->toDesktop( );
        break;
      case QEvent::MouseButtonPress:
        if ( ((QMouseEvent*)event)->button() == LeftButton ){
          if ( curdw->eDocking != (int)PMDockWidget::DockNone ){
            dropCancel = true;
            curdw->setFocus();
            qApp->processOneEvent();

            currentDragWidget = curdw;
            currentMoveWidget = 0L;
            childDockWidgetList = new QWidgetList();
            childDockWidgetList->append( curdw );
            findChildDockWidget( curdw, *childDockWidgetList );

            d->oldDragRect = QRect();
            d->dragRect = QRect(curdw->geometry());
            QPoint p = curdw->mapToGlobal(QPoint(0,0));
            d->dragRect.moveTopLeft(p);
            drawDragRectangle();
            d->readyToDrag = true;

            d->dragOffset = QCursor::pos()-currentDragWidget->mapToGlobal(QPoint(0,0));
          }
        }
        break;
      case QEvent::MouseButtonRelease:
        if ( ((QMouseEvent*)event)->button() == LeftButton ){
          if ( draging ){
            if ( !dropCancel )
              drop();
            else
              cancelDrop();
          }
          if (d->readyToDrag) {
              d->readyToDrag = false;
              d->oldDragRect = QRect();
              d->dragRect = QRect(curdw->geometry());
              QPoint p = curdw->mapToGlobal(QPoint(0,0));
              d->dragRect.moveTopLeft(p);
              drawDragRectangle();
              currentDragWidget = 0L;
              delete childDockWidgetList;
              childDockWidgetList = 0L;
          }
          draging = false;
          dropCancel = true;
        }
        break;
      case QEvent::MouseMove:
        if ( draging ) {
          pDockWdgAtCursor = findDockWidgetAt( QCursor::pos() );
          PMDockWidget* oldMoveWidget = currentMoveWidget;
          if ( currentMoveWidget  && pDockWdgAtCursor == currentMoveWidget ) { //move
            dragMove( currentMoveWidget, currentMoveWidget->mapFromGlobal( QCursor::pos() ) );
            break;
          } else {
            if (dropCancel && curdw) {
              d->dragRect = QRect(curdw->geometry());
              QPoint p = curdw->mapToGlobal(QPoint(0,0));
              d->dragRect.moveTopLeft(p);
            }else
              d->dragRect = QRect();

            drawDragRectangle();
          }

          if ( !pDockWdgAtCursor && (curdw->eDocking & (int)PMDockWidget::DockDesktop) == 0 ){
              // just moving at the desktop
              currentMoveWidget = pDockWdgAtCursor;
              curPos = PMDockWidget::DockDesktop;
          } else {
            if ( oldMoveWidget && pDockWdgAtCursor != currentMoveWidget ) { //leave
              currentMoveWidget = pDockWdgAtCursor;
              curPos = PMDockWidget::DockDesktop;
            }
          }

          if ( oldMoveWidget != pDockWdgAtCursor && pDockWdgAtCursor ) { //enter pDockWdgAtCursor
            currentMoveWidget = pDockWdgAtCursor;
            curPos = PMDockWidget::DockDesktop;
          }
        } else {
          if (d->readyToDrag) {
            d->readyToDrag = false;
          }
          if ( (((QMouseEvent*)event)->state() == LeftButton) &&
               (curdw->eDocking != (int)PMDockWidget::DockNone) ) {
            startDrag( curdw);
          }
        }
        break;
      default:
        break;
    }
  }
  return QObject::eventFilter( obj, event );
}

PMDockWidget* PMDockManager::findDockWidgetAt( const QPoint& pos )
{
  dropCancel = true;

  if (!currentDragWidget)
    return 0L; // pointer access safety

  if (currentDragWidget->eDocking == (int)PMDockWidget::DockNone ) return 0L;

  QWidget* p = QApplication::widgetAt( pos );
  if ( !p ) {
    dropCancel = false;
    return 0L;
  }
#if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
  p = p->topLevelWidget();
#endif
  QWidget* w = 0L;
  findChildDockWidget( w, p, p->mapFromGlobal(pos) );
  if ( !w ){
    if ( !p->inherits("PMDockWidget") ) {
      return 0L;
    }
    w = p;
  }
  if ( qt_find_obj_child( w, "PMDockSplitter", "_dock_split_" ) ) return 0L;
  if ( qt_find_obj_child( w, "PMDockTabGroup", "_dock_tab" ) ) return 0L;
  if (!childDockWidgetList) return 0L;
  if ( childDockWidgetList->find(w) != -1 ) return 0L;
  if ( currentDragWidget->isGroup && ((PMDockWidget*)w)->parentDockTabGroup() ) return 0L;

  PMDockWidget* www = (PMDockWidget*)w;
  if ( www->dockSite( ) == (int)PMDockWidget::DockNone ) return 0L;

  PMDockWidget::DockPosition curPos = PMDockWidget::DockDesktop;
  QPoint cpos  = www->mapFromGlobal( pos );

  int ww = www->widget->width() / 3;
  int hh = www->widget->height() / 3;

  if ( cpos.y() <= hh ){
    curPos = PMDockWidget::DockTop;
  } else
    if ( cpos.y() >= 2*hh ){
      curPos = PMDockWidget::DockBottom;
    } else
      if ( cpos.x() <= ww ){
        curPos = PMDockWidget::DockLeft;
      } else
        if ( cpos.x() >= 2*ww ){
          curPos = PMDockWidget::DockRight;
        } else
            curPos = PMDockWidget::DockCenter;

  if ( !(www->dockSite( ) & (int)curPos) ) return 0L;
  if ( !(currentDragWidget->eDocking & (int)curPos) ) return 0L;
  if ( www->manager != this ) return 0L;

  dropCancel = false;
  return www;
}

void PMDockManager::findChildDockWidget( QWidget*& ww, const QWidget* p, const QPoint& pos )
{
  if ( p->children() ) {
    QWidget *w;
    QObjectListIt it( *p->children() );
    it.toLast();
    while ( it.current() ) {
      if ( it.current()->isWidgetType() ) {
        w = (QWidget*)it.current();
        if ( w->isVisible() && w->geometry().contains(pos) ) {
          if ( w->inherits("PMDockWidget") ) ww = w;
          findChildDockWidget( ww, w, w->mapFromParent(pos) );
          return;
        }
      }
      --it;
    }
  }
  return;
}

void PMDockManager::findChildDockWidget( const QWidget* p, QWidgetList& list )
{
  if ( p->children() ) {
    QWidget *w;
    QObjectListIt it( *p->children() );
    it.toLast();
    while ( it.current() ) {
      if ( it.current()->isWidgetType() ) {
        w = (QWidget*)it.current();
        if ( w->isVisible() ) {
          if ( w->inherits("PMDockWidget") ) list.append( w );
          findChildDockWidget( w, list );
        }
      }
      --it;
    }
  }
  return;
}

void PMDockManager::findFloatingWidgets( QPtrList<PMDockWidget>& l )
{
   QObjectListIt it( *childDock );
   for( ; it.current( ); ++it )
      if( it.current( )->inherits( "PMDockWidget" ) &&
          !it.current( )->parent( ) )
         l.append( ( PMDockWidget* ) it.current( ) );
}

void PMDockManager::startDrag( PMDockWidget* w )
{
  if(( w->currentDockPos == PMDockWidget::DockLeft) || ( w->currentDockPos == PMDockWidget::DockRight)
   || ( w->currentDockPos == PMDockWidget::DockTop) || ( w->currentDockPos == PMDockWidget::DockBottom)) {
    w->prevSideDockPosBeforeDrag = w->currentDockPos;

    if ( w->parentWidget()->inherits("PMDockSplitter") ){
      PMDockSplitter* parentSplitterOfDockWidget = (PMDockSplitter*)(w->parentWidget());
      w->d->splitPosInPercent = parentSplitterOfDockWidget->separatorPos();
    }
  }

  curPos = PMDockWidget::DockDesktop;
  draging = true;

  QApplication::setOverrideCursor(QCursor(sizeAllCursor));
}

void PMDockManager::dragMove( PMDockWidget* dw, QPoint pos )
{
  QPoint p = dw->mapToGlobal( dw->widget->pos() );
  PMDockWidget::DockPosition oldPos = curPos;

  QSize r = dw->widget->size();
  if ( dw->parentDockTabGroup() ){
    curPos = PMDockWidget::DockCenter;
    if ( oldPos != curPos ) {
      d->dragRect.setRect( p.x()+2, p.y()+2, r.width()-4, r.height()-4 );
    }
    return;
  }

  int w = r.width() / 3;
  int h = r.height() / 3;

  PMDockMainWindow* mw = ( PMDockMainWindow* ) parent( );
  QWidget* cw = mw->centralWidget( );
  QPoint cwp = cw->mapToGlobal( QPoint( 0, 0 ) );
  int cwh = cw->height( );

  if ( pos.y() <= h )
  {
     curPos = PMDockWidget::DockTop;
     w = r.width();
  }
  else if ( pos.y() >= 2*h )
  {
     curPos = PMDockWidget::DockBottom;
     p.setY( p.y() + 2*h );
     w = r.width();
  }
  else if ( pos.x() <= w )
  {
     curPos = PMDockWidget::DockLeft;
     h = r.height();
     p.setY( cwp.y( ) );
     h = cwh;
  }
  else if ( pos.x() >= 2*w )
  {
     curPos = PMDockWidget::DockRight;
     p.setX( p.x() + 2*w );
     p.setY( cwp.y( ) );
     h = cwh;
  }
  else
  {
     curPos = PMDockWidget::DockCenter;
     p.setX( p.x() + w );
     p.setY( p.y() + h );
  }

  if ( oldPos != curPos ) {
    d->dragRect.setRect( p.x(), p.y(), w, h );
    drawDragRectangle();
  }
}


void PMDockManager::cancelDrop()
{
  QApplication::restoreOverrideCursor();

  delete childDockWidgetList;
  childDockWidgetList = 0L;

  d->dragRect = QRect();  // cancel drawing
  drawDragRectangle();    // only the old rect will be deleted
}


void PMDockManager::drop()
{
  d->dragRect = QRect();  // cancel drawing
  drawDragRectangle();    // only the old rect will be deleted

  QApplication::restoreOverrideCursor();

  delete childDockWidgetList;
  childDockWidgetList = 0L;

  if ( dropCancel ) return;
  if ( !currentMoveWidget && ((currentDragWidget->eDocking & (int)PMDockWidget::DockDesktop) == 0) ) {
    d->dragRect = QRect();  // cancel drawing
    drawDragRectangle();    // only the old rect will be deleted
    return;
  }
  if ( !currentMoveWidget && !currentDragWidget->parent() ) {
    currentDragWidget->move( QCursor::pos() - d->dragOffset );
  }
  else {
    int splitPos = currentDragWidget->d->splitPosInPercent;
    // do we have to calculate 100%-splitPosInPercent?
    if( (curPos != currentDragWidget->prevSideDockPosBeforeDrag) && (curPos != PMDockWidget::DockCenter) && (curPos != PMDockWidget::DockDesktop)) {
      switch( currentDragWidget->prevSideDockPosBeforeDrag) {
      case PMDockWidget::DockLeft:   if(curPos != PMDockWidget::DockTop)    splitPos = 100-splitPos; break;
      case PMDockWidget::DockRight:  if(curPos != PMDockWidget::DockBottom) splitPos = 100-splitPos; break;
      case PMDockWidget::DockTop:    if(curPos != PMDockWidget::DockLeft)   splitPos = 100-splitPos; break;
      case PMDockWidget::DockBottom: if(curPos != PMDockWidget::DockRight)  splitPos = 100-splitPos; break;
      default: break;
      }
    }
    currentDragWidget->manualDock( currentMoveWidget, curPos , splitPos, QCursor::pos() - d->dragOffset );
    currentDragWidget->makeDockVisible();
  }
}


static QDomElement createStringEntry(QDomDocument &doc, const QString &tagName, const QString &str)
{
    QDomElement el = doc.createElement(tagName);

    el.appendChild(doc.createTextNode(str));
    return el;
}


static QDomElement createBoolEntry(QDomDocument &doc, const QString &tagName, bool b)
{
    return createStringEntry(doc, tagName, QString::fromLatin1(b? "true" : "false"));
}


static QDomElement createNumberEntry(QDomDocument &doc, const QString &tagName, int n)
{
    return createStringEntry(doc, tagName, QString::number(n));
}


static QDomElement createRectEntry(QDomDocument &doc, const QString &tagName, const QRect &rect)
{
    QDomElement el = doc.createElement(tagName);

    QDomElement xel = doc.createElement("x");
    xel.appendChild(doc.createTextNode(QString::number(rect.x())));
    el.appendChild(xel);
    QDomElement yel = doc.createElement("y");
    yel.appendChild(doc.createTextNode(QString::number(rect.y())));
    el.appendChild(yel);
    QDomElement wel = doc.createElement("width");
    wel.appendChild(doc.createTextNode(QString::number(rect.width())));
    el.appendChild(wel);
    QDomElement hel = doc.createElement("height");
    hel.appendChild(doc.createTextNode(QString::number(rect.height())));
    el.appendChild(hel);

    return el;
}


static QDomElement createListEntry(QDomDocument &doc, const QString &tagName,
                                   const QString &subTagName, const QStrList &list)
{
    QDomElement el = doc.createElement(tagName);

    QStrListIterator it(list);
    for (; it.current(); ++it) {
        QDomElement subel = doc.createElement(subTagName);
        subel.appendChild(doc.createTextNode(QString::fromLatin1(it.current())));
        el.appendChild(subel);
    }

    return el;
}


static QString stringEntry(QDomElement &base, const QString &tagName)
{
    return base.namedItem(tagName).firstChild().toText().data();
}


static bool boolEntry(QDomElement &base, const QString &tagName)
{
    return base.namedItem(tagName).firstChild().toText().data() == "true";
}


static int numberEntry(QDomElement &base, const QString &tagName)
{
    return stringEntry(base, tagName).toInt();
}


static QRect rectEntry(QDomElement &base, const QString &tagName)
{
    QDomElement el = base.namedItem(tagName).toElement();

    int x = numberEntry(el, "x");
    int y = numberEntry(el, "y");
    int width = numberEntry(el, "width");
    int height = numberEntry(el,  "height");

    return QRect(x, y, width, height);
}


static QStrList listEntry(QDomElement &base, const QString &tagName, const QString &subTagName)
{
    QStrList list;

    QDomElement subel = base.namedItem(tagName).firstChild().toElement();
    while (!subel.isNull()) {
        if (subel.tagName() == subTagName)
            list.append(subel.firstChild().toText().data().latin1());
        subel = subel.nextSibling().toElement();
    }

    return list;
}


void PMDockManager::writeConfig(QDomElement &base)
{
    // First of all, clear the tree under base
    while (!base.firstChild().isNull())
        base.removeChild(base.firstChild());
    QDomDocument doc = base.ownerDocument();

    QStrList nameList;
    QString mainWidgetStr;

    // collect widget names
    QStrList nList;
    QObjectListIt it(*childDock);
    PMDockWidget *obj1;
    while ( (obj1=(PMDockWidget*)it.current()) ) {
        if ( obj1->parent() == main )
            mainWidgetStr = QString::fromLatin1(obj1->name());
        nList.append(obj1->name());
        ++it;
    }

    nList.first();
    while ( nList.current() ) {
        PMDockWidget *obj = getDockWidgetFromName( nList.current() );
        if (obj->isGroup && (nameList.find( obj->firstName.latin1() ) == -1
                             || nameList.find(obj->lastName.latin1()) == -1)) {
            // Skip until children are saved (why?)
            nList.next();
            if ( !nList.current() ) nList.first();
            continue;
        }

        QDomElement groupEl;

        if (obj->isGroup) {
            //// Save a group
            groupEl = doc.createElement("splitGroup");

            groupEl.appendChild(createStringEntry(doc, "firstName", obj->firstName));
            groupEl.appendChild(createStringEntry(doc, "secondName", obj->lastName));
            groupEl.appendChild(createNumberEntry(doc, "orientation", (int)obj->splitterOrientation));
            groupEl.appendChild(createNumberEntry(doc, "separatorPos", ((PMDockSplitter*)obj->widget)->separatorPos()));
        } else if (obj->isTabGroup) {
            //// Save a tab group
            groupEl = doc.createElement("tabGroup");

            QStrList list;
            for ( int i = 0; i < ((PMDockTabGroup*)obj->widget)->count(); ++i )
                list.append( ((PMDockTabGroup*)obj->widget)->page( i )->name() );
            groupEl.appendChild(createListEntry(doc, "tabs", "tab", list));
            groupEl.appendChild(createNumberEntry(doc, "currentTab", ((PMDockTabGroup*)obj->widget)->currentPageIndex()));
        } else {
            //// Save an ordinary dock widget
            groupEl = doc.createElement("dock");
        }

        groupEl.appendChild(createStringEntry(doc, "name", QString::fromLatin1(obj->name())));
        groupEl.appendChild(createBoolEntry(doc, "hasParent", obj->parent()));
        if ( !obj->parent() ) {
            groupEl.appendChild(createRectEntry(doc, "geometry", QRect(main->frameGeometry().topLeft(), main->size())));
            groupEl.appendChild(createBoolEntry(doc, "visible", obj->isVisible()));
        }
        if (obj->header && obj->header->inherits("PMDockWidgetHeader")) {
            PMDockWidgetHeader *h = static_cast<PMDockWidgetHeader*>(obj->header);
            groupEl.appendChild(createBoolEntry(doc, "dragEnabled", h->dragEnabled()));
        }

        base.appendChild(groupEl);
        nameList.append(obj->name());
        nList.remove();
        nList.first();
    }

    if (main->inherits("PMDockMainWindow")) {
        PMDockMainWindow *dmain = (PMDockMainWindow*)main;
        QString centralWidgetStr = QString(dmain->centralWidget()? dmain->centralWidget()->name() : "");
        base.appendChild(createStringEntry(doc, "centralWidget", centralWidgetStr));
        QString mainDockWidgetStr = QString(dmain->getMainDockWidget()? dmain->getMainDockWidget()->name() : "");
        base.appendChild(createStringEntry(doc, "mainDockWidget", mainDockWidgetStr));
    } else {
        base.appendChild(createStringEntry(doc, "mainWidget", mainWidgetStr));
    }

    base.appendChild(createRectEntry(doc, "geometry", QRect(main->frameGeometry().topLeft(), main->size())));
}


void PMDockManager::readConfig(QDomElement &base)
{
    if (base.namedItem("group").isNull()
        && base.namedItem("tabgroup").isNull()
        && base.namedItem("dock").isNull()) {
        activate();
        return;
    }

    autoCreateDock = new QObjectList();
    autoCreateDock->setAutoDelete( true );

    bool isMainVisible = main->isVisible();
    main->hide();

    QObjectListIt it(*childDock);
    PMDockWidget *obj1;
    while ( (obj1=(PMDockWidget*)it.current()) ) {
        if ( !obj1->isGroup && !obj1->isTabGroup ) {
            if ( obj1->parent() )
                obj1->undock();
            else
                obj1->hide();
        }
        ++it;
    }

    QDomElement childEl = base.firstChild().toElement();
    while (!childEl.isNull() ) {
        PMDockWidget *obj = 0;

        if (childEl.tagName() == "splitGroup") {
            // Read a group
            QString name = stringEntry(childEl, "name");
            QString firstName = stringEntry(childEl, "firstName");
            QString secondName = stringEntry(childEl, "secondName");
            int orientation = numberEntry(childEl, "orientation");
            int separatorPos = numberEntry(childEl, "separatorPos");

            PMDockWidget *first = getDockWidgetFromName(firstName);
            PMDockWidget *second = getDockWidgetFromName(secondName);
            if (first && second) {
                obj = first->manualDock(second,
                                        (orientation == (int)Vertical)? PMDockWidget::DockLeft : PMDockWidget::DockTop,
                                        separatorPos);
                if (obj)
                    obj->setName(name.latin1());
            }
        } else if (childEl.tagName() == "tabGroup") {
            // Read a tab group
            QString name = stringEntry(childEl, "name");
            QStrList list = listEntry(childEl, "tabs", "tab");

            PMDockWidget *d1 = getDockWidgetFromName( list.first() );
            list.next();
            PMDockWidget *d2 = getDockWidgetFromName( list.current() );

            PMDockWidget *obj = d2->manualDock( d1, PMDockWidget::DockCenter );
            if (obj) {
                PMDockTabGroup *tab = (PMDockTabGroup*)obj->widget;
                list.next();
                while (list.current() && obj) {
                    PMDockWidget *tabDock = getDockWidgetFromName(list.current());
                    obj = tabDock->manualDock(d1, PMDockWidget::DockCenter);
                    list.next();
                }
                if (obj) {
                    obj->setName(name.latin1());
                    tab->showPage(tab->page(numberEntry(childEl, "currentTab")));
                }
            }
        } else if (childEl.tagName() == "dock") {
            // Read an ordinary dock widget
            obj = getDockWidgetFromName(stringEntry(childEl, "name"));
        }

        if (!boolEntry(childEl, "hasParent")) {
            QRect r = rectEntry(childEl, "geometry");
            obj = getDockWidgetFromName(stringEntry(childEl, "name"));
            obj->applyToWidget(0);
            obj->setGeometry(r);
            if (boolEntry(childEl, "visible"))
                obj->QWidget::show();
        }

        if (obj && obj->header && obj->header->inherits("PMDockWidgetHeader")) {
            PMDockWidgetHeader *h = static_cast<PMDockWidgetHeader*>(obj->header);
            h->setDragEnabled(boolEntry(childEl, "dragEnabled"));
        }

        childEl = childEl.nextSibling().toElement();
    }

    if (main->inherits("PMDockMainWindow")) {
        PMDockMainWindow *dmain = (PMDockMainWindow*)main;

        QString mv = stringEntry(base, "centralWidget");
        if (!mv.isEmpty() && getDockWidgetFromName(mv) ) {
            PMDockWidget *mvd  = getDockWidgetFromName(mv);
            mvd->applyToWidget(dmain);
            mvd->show();
            dmain->setCentralWidget(mvd);
        }
        QString md = stringEntry(base, "mainDockWidget");
        if (!md.isEmpty() && getDockWidgetFromName(md)) {
            PMDockWidget *mvd  = getDockWidgetFromName(md);
            dmain->setMainDockWidget(mvd);
        }
    } else {
        QString mv = stringEntry(base, "mainWidget");
        if (!mv.isEmpty() && getDockWidgetFromName(mv)) {
            PMDockWidget *mvd  = getDockWidgetFromName(mv);
            mvd->applyToWidget(main);
            mvd->show();
        }
    }

    QRect mr = rectEntry(base, "geometry");
    main->setGeometry(mr);
    if (isMainVisible)
        main->show();

    delete autoCreateDock;
    autoCreateDock = 0;
}


#ifndef NO_KDE2
void PMDockManager::writeConfig( KConfig* c, QString group )
{
  //debug("BEGIN Write Config");
  if ( !c ) c = KGlobal::config();
  if ( group.isEmpty() ) group = "dock_setting_default";

  c->setGroup( group );
  c->writeEntry( "Version", DOCK_CONFIG_VERSION );

  QStrList nameList;
  QStrList findList;
  QObjectListIt it( *childDock );
  PMDockWidget * obj;

  // collect PMDockWidget's name
  QStrList nList;
  while ( (obj=(PMDockWidget*)it.current()) ) {
    ++it;
    //debug("  +Add subdock %s", obj->name());
    nList.append( obj->name() );
    if ( obj->parent() == main )
      c->writeEntry( "Main:view", obj->name() );
  }

  nList.first();
  while ( nList.current() ){
    //debug("  -Try to save %s", nList.current());
    obj = getDockWidgetFromName( nList.current() );
    QString cname = obj->name();
    if ( obj->header ){
      obj->header->saveConfig( c );
    }
/*************************************************************************************************/
    if ( obj->isGroup ){
      if ( findList.find( obj->firstName.latin1() ) != -1 && findList.find( obj->lastName.latin1() ) != -1 ){

        c->writeEntry( cname+":type", "GROUP");
        if ( !obj->parent() ){
          c->writeEntry( cname+":parent", "___null___");
          c->writeEntry( cname+":geometry", QRect(obj->frameGeometry().topLeft(), obj->size()) );
          c->writeEntry( cname+":visible", obj->isVisible());
        } else {
          c->writeEntry( cname+":parent", "yes");
        }
        c->writeEntry( cname+":first_name", obj->firstName );
        c->writeEntry( cname+":last_name", obj->lastName );
        c->writeEntry( cname+":orientation", (int)obj->splitterOrientation );
        c->writeEntry( cname+":sepPos", ((PMDockSplitter*)obj->widget)->separatorPos() );

        nameList.append( obj->name() );
        findList.append( obj->name() );
        //debug("  Save %s", nList.current());
        nList.remove();
        nList.first();
      } else {
/*************************************************************************************************/
        //debug("  Skip %s", nList.current());
        //if ( findList.find( obj->firstName ) == -1 )
        //  debug("  ? Not found %s", obj->firstName);
        //if ( findList.find( obj->lastName ) == -1 )
        //  debug("  ? Not found %s", obj->lastName);
        nList.next();
        if ( !nList.current() ) nList.first();
      }
    } else {
/*************************************************************************************************/
      if ( obj->isTabGroup){
        c->writeEntry( cname+":type", "TAB_GROUP");
        if ( !obj->parent() ){
          c->writeEntry( cname+":parent", "___null___");
          c->writeEntry( cname+":geometry", QRect(obj->frameGeometry().topLeft(), obj->size()) );
          c->writeEntry( cname+":visible", obj->isVisible());
        } else {
          c->writeEntry( cname+":parent", "yes");
        }
        QStrList list;
        for ( int i = 0; i < ((PMDockTabGroup*)obj->widget)->count(); ++i )
          list.append( ((PMDockTabGroup*)obj->widget)->page( i )->name() );
        c->writeEntry( cname+":tabNames", list );
        c->writeEntry( cname+":curTab", ((PMDockTabGroup*)obj->widget)->currentPageIndex() );

        nameList.append( obj->name() );
        findList.append( obj->name() ); // not really need !!!
        //debug("  Save %s", nList.current());
        nList.remove();
        nList.first();
      } else {
/*************************************************************************************************/
        if ( !obj->parent() ){
          c->writeEntry( cname+":type", "NULL_DOCK");
          c->writeEntry( cname+":geometry", QRect(obj->frameGeometry().topLeft(), obj->size()) );
          c->writeEntry( cname+":visible", obj->isVisible());
        } else {
          c->writeEntry( cname+":type", "DOCK");
        }
        nameList.append( cname.latin1() );
        //debug("  Save %s", nList.current());
        findList.append( obj->name() );
        nList.remove();
        nList.first();
      }
    }
  }
  c->writeEntry( "NameList", nameList );

  c->writeEntry( "Main:Geometry", QRect(main->frameGeometry().topLeft(), main->size()) );
  c->writeEntry( "Main:visible", main->isVisible()); // curently nou use

  if ( main->inherits("PMDockMainWindow") ){
    PMDockMainWindow* dmain = (PMDockMainWindow*)main;
    // for PMDockMainWindow->setView() in readConfig()
    c->writeEntry( "Main:view", dmain->centralWidget() ? dmain->centralWidget()->name():"" );
    c->writeEntry( "Main:dock", dmain->getMainDockWidget()     ? dmain->getMainDockWidget()->name()    :"" );
  }

  c->sync();
  //debug("END Write Config");
}
#include <qmessagebox.h>
void PMDockManager::readConfig( KConfig* c, QString group )
{
  if ( !c ) c = KGlobal::config();
  if ( group.isEmpty() ) group = "dock_setting_default";

  c->setGroup( group );
  QStrList nameList;
  c->readListEntry( "NameList", nameList );
  QString ver = c->readEntry( "Version", "0.0.1" );
  nameList.first();
  if ( !nameList.current() || ver != DOCK_CONFIG_VERSION ){
    activate();
    return;
  }

  autoCreateDock = new QObjectList();
  autoCreateDock->setAutoDelete( true );

  bool isMainVisible = main->isVisible();
 // if (isMainVisible)  // CCC
  //QMessageBox::information(0,"","hallo");
//COMMENTED4TESTING  main->hide();

  QObjectListIt it( *childDock );
  PMDockWidget * obj;

  while ( (obj=(PMDockWidget*)it.current()) ){
    ++it;
    if ( !obj->isGroup && !obj->isTabGroup )
    {
      if ( obj->parent() ) obj->undock(); else obj->hide();
    }
  }

  nameList.first();
  while ( nameList.current() ){
    QString oname = nameList.current();
    c->setGroup( group );
    QString type = c->readEntry( oname + ":type" );
    obj = 0L;

    if ( type == "GROUP" ){
      PMDockWidget* first = getDockWidgetFromName( c->readEntry( oname + ":first_name" ) );
      PMDockWidget* last  = getDockWidgetFromName( c->readEntry( oname + ":last_name"  ) );
      int sepPos = c->readNumEntry( oname + ":sepPos" );

      Orientation p = (Orientation)c->readNumEntry( oname + ":orientation" );
      if ( first  && last ){
        obj = first->manualDock( last, ( p == Vertical ) ? PMDockWidget::DockLeft : PMDockWidget::DockTop, sepPos );
        if (obj){
          obj->setName( oname.latin1() );
        }
      }
    }

    if ( type == "TAB_GROUP" ){
      QStrList list;
      PMDockWidget* tabDockGroup = 0L;
      c->readListEntry( oname+":tabNames", list );
      PMDockWidget* d1 = getDockWidgetFromName( list.first() );
      list.next();
      PMDockWidget* d2 = getDockWidgetFromName( list.current() );
      tabDockGroup = d2->manualDock( d1, PMDockWidget::DockCenter );
      if ( tabDockGroup ){
        PMDockTabGroup* tab = (PMDockTabGroup*)tabDockGroup->widget;
        list.next();
        while ( list.current() && tabDockGroup ){
          PMDockWidget* tabDock = getDockWidgetFromName( list.current() );
          tabDockGroup = tabDock->manualDock( d1, PMDockWidget::DockCenter );
          list.next();
        }
        if ( tabDockGroup ){
          tabDockGroup->setName( oname.latin1() );
          c->setGroup( group );
          tab->showPage( tab->page( c->readNumEntry( oname+":curTab" ) ) );
        }
      }
      obj = tabDockGroup;
    }

    if ( type == "NULL_DOCK" || c->readEntry( oname + ":parent") == "___null___" ){
      QRect r = c->readRectEntry( oname + ":geometry" );
      obj = getDockWidgetFromName( oname );
      obj->applyToWidget( 0L );
      obj->setGeometry(r);

      c->setGroup( group );
      if ( c->readBoolEntry( oname + ":visible" ) ){
        obj->QWidget::show();
      }
    }

    if ( type == "DOCK"  ){
      obj = getDockWidgetFromName( oname );
    }

    if ( obj && obj->header){
      obj->header->loadConfig( c );
    }
    nameList.next();
  }

  if ( main->inherits("PMDockMainWindow") ){
    PMDockMainWindow* dmain = (PMDockMainWindow*)main;

    c->setGroup( group );
    QString mv = c->readEntry( "Main:view" );
    if ( !mv.isEmpty() && getDockWidgetFromName( mv ) ){
      PMDockWidget* mvd  = getDockWidgetFromName( mv );
      mvd->applyToWidget( dmain );
      mvd->show();
      dmain->setView( mvd );
    }
    c->setGroup( group );
    QString md = c->readEntry( "Main:dock" );
    if ( !md.isEmpty() && getDockWidgetFromName( md ) ){
      PMDockWidget* mvd  = getDockWidgetFromName( md );
      dmain->setMainDockWidget( mvd );
    }
  } else {
    c->setGroup( group );
    QString mv = c->readEntry( "Main:view" );
    if ( !mv.isEmpty() && getDockWidgetFromName( mv ) ){
      PMDockWidget* mvd  = getDockWidgetFromName( mv );
      mvd->applyToWidget( main );
      mvd->show();
    }

  }
  // delete all autocreate dock
  delete autoCreateDock;
  autoCreateDock = 0L;

  c->setGroup( group );
  QRect mr = c->readRectEntry("Main:Geometry");
  main->setGeometry(mr);
  if ( isMainVisible ) main->show();
}
#endif

PMDockWidget* PMDockManager::getDockWidgetFromName( const QString& dockName )
{
  QObjectListIt it( *childDock );
  PMDockWidget * obj;
  while ( (obj=(PMDockWidget*)it.current()) ) {
    ++it;
    if ( QString(obj->name()) == dockName ) return obj;
  }

  PMDockWidget* autoCreate = 0L;
  if ( autoCreateDock ){
    autoCreate = new PMDockWidget( this, dockName.latin1(), QPixmap("") );
    autoCreateDock->append( autoCreate );
  }
  return autoCreate;
}
void PMDockManager::setSplitterOpaqueResize(bool b)
{
  d->splitterOpaqueResize = b;
}

bool PMDockManager::splitterOpaqueResize() const
{
  return d->splitterOpaqueResize;
}

void PMDockManager::setSplitterKeepSize(bool b)
{
  d->splitterKeepSize = b;
}

bool PMDockManager::splitterKeepSize() const
{
  return d->splitterKeepSize;
}

void PMDockManager::setSplitterHighResolution(bool b)
{
  d->splitterHighResolution = b;
}

bool PMDockManager::splitterHighResolution() const
{
  return d->splitterHighResolution;
}

void PMDockManager::slotMenuPopup()
{
  menu->clear();
  menuData->clear();

  QObjectListIt it( *childDock );
  PMDockWidget * obj;
  int numerator = 0;
  while ( (obj=(PMDockWidget*)it.current()) ) {
    ++it;
    if ( obj->mayBeHide() )
    {
      menu->insertItem( obj->icon() ? *(obj->icon()) : QPixmap(), QString("Hide ") + obj->caption(), numerator++ );
      menuData->append( new MenuDockData( obj, true ) );
    }

    if ( obj->mayBeShow() )
    {
      menu->insertItem( obj->icon() ? *(obj->icon()) : QPixmap(), QString("Show ") + obj->caption(), numerator++ );
      menuData->append( new MenuDockData( obj, false ) );
    }
  }
}

void PMDockManager::slotMenuActivated( int id )
{
  MenuDockData* data = menuData->at( id );
  data->dock->changeHideShowState();
}

PMDockWidget* PMDockManager::findWidgetParentDock( QWidget* w ) const
{
  QObjectListIt it( *childDock );
  PMDockWidget * dock;
  PMDockWidget * found = 0L;

  while ( (dock=(PMDockWidget*)it.current()) ) {
    ++it;
    if ( dock->widget == w ){ found  = dock; break; }
  }
  return found;
}

void PMDockManager::drawDragRectangle()
{
  if (d->oldDragRect == d->dragRect)
    return;

  int i;
  QRect oldAndNewDragRect[2];
  oldAndNewDragRect[0] = d->oldDragRect;
  oldAndNewDragRect[1] = d->dragRect;

  // 2 calls, one for the old and one for the new drag rectangle
  for (i = 0; i <= 1; i++) {
    if (oldAndNewDragRect[i].isEmpty())
      continue;

    PMDockWidget* pDockWdgAtRect = (PMDockWidget*) QApplication::widgetAt( oldAndNewDragRect[i].topLeft(), true );
    if (!pDockWdgAtRect)
      continue;

    bool isOverMainWdg = false;
    bool unclipped;
    PMDockMainWindow* pMain = 0L;
    PMDockWidget* pTLDockWdg = 0L;
    QWidget* topWdg;
    if (pDockWdgAtRect->topLevelWidget() == main) {
      isOverMainWdg = true;
      topWdg = pMain = (PMDockMainWindow*) main;
      unclipped = pMain->testWFlags( WPaintUnclipped );
      pMain->setWFlags( WPaintUnclipped );
    }
    else {
      topWdg = pTLDockWdg = (PMDockWidget*) pDockWdgAtRect->topLevelWidget();
      unclipped = pTLDockWdg->testWFlags( WPaintUnclipped );
      pTLDockWdg->setWFlags( WPaintUnclipped );
    }

    // draw the rectangle unclipped over the main dock window
    QPainter p;
    p.begin( topWdg );
      if ( !unclipped ) {
        if (isOverMainWdg)
          pMain->clearWFlags(WPaintUnclipped);
        else
          pTLDockWdg->clearWFlags(WPaintUnclipped);
      }
      // draw the rectangle
      p.setRasterOp(Qt::NotXorROP);
      QRect r = oldAndNewDragRect[i];
      r.moveTopLeft( r.topLeft() - topWdg->mapToGlobal(QPoint(0,0)) );
      p.drawRect(r.x(), r.y(), r.width(), r.height());
    p.end();
  }

  // memorize the current rectangle for later removing
  d->oldDragRect = d->dragRect;
}


#ifdef _JOWENN_EXPERIMENTAL_

PMDockArea::PMDockArea( QWidget* parent, const char *name)
:QWidget( parent, name)
{
  QString new_name = QString(name) + QString("_DockManager");
  dockManager = new PMDockManager( this, new_name.latin1() );
  mainDockWidget = 0L;
}

PMDockArea::~PMDockArea()
{
	delete dockManager;
}

PMDockWidget* PMDockArea::createDockWidget( const QString& name, const QPixmap &pixmap, QWidget* parent, const QString& strCaption, const QString& strTabPageLabel)
{
  return new PMDockWidget( dockManager, name.latin1(), pixmap, parent, strCaption, strTabPageLabel );
}

void PMDockArea::makeDockVisible( PMDockWidget* dock )
{
  if ( dock != 0L)
    dock->makeDockVisible();
}

void PMDockArea::makeDockInvisible( PMDockWidget* dock )
{
  if ( dock != 0L)
    dock->undock();
}

void PMDockArea::makeWidgetDockVisible( QWidget* widget )
{
  makeDockVisible( dockManager->findWidgetParentDock(widget) );
}

void PMDockArea::writeDockConfig(QDomElement &base)
{
  dockManager->writeConfig(base);
}

void PMDockArea::readDockConfig(QDomElement &base)
{
  dockManager->readConfig(base);
}

void PMDockArea::slotDockWidgetUndocked()
{
  QObject* pSender = (QObject*) sender();
  if (!pSender->inherits("PMDockWidget")) return;
  PMDockWidget* pDW = (PMDockWidget*) pSender;
  emit dockWidgetHasUndocked( pDW);
}

void PMDockArea::resizeEvent(QResizeEvent *rsize)
{
  QWidget::resizeEvent(rsize);
  if (children()){
#ifndef NO_KDE2
    kdDebug()<<"PMDockArea::resize"<<endl;
#endif
    QObjectList *list=queryList("QWidget",0,false);

    QObjectListIt it( *list ); // iterate over the buttons
    QObject *obj;

    while ( (obj = it.current()) != 0 ) {
        // for each found object...
        ((QWidget*)obj)->setGeometry(QRect(QPoint(0,0),size()));
	break;
    }
    delete list;
#if 0
    PMDockSplitter *split;
//    for (unsigned int i=0;i<children()->count();i++)
    {
//    	QPtrList<QObject> list(children());
//       QObject *obj=((QPtrList<QObject*>)children())->at(i);
	QObject *obj=children()->getFirst();
       if (split=dynamic_cast<PMDockSplitter*>(obj))
       {
          split->setGeometry( QRect(QPoint(0,0), size() ));
//	  break;
       }
    }
#endif
   }
}

#ifndef NO_KDE2
void PMDockArea::writeDockConfig( KConfig* c, QString group )
{
  dockManager->writeConfig( c, group );
}

void PMDockArea::readDockConfig( KConfig* c, QString group )
{
  dockManager->readConfig( c, group );
}

void PMDockArea::setMainDockWidget( PMDockWidget* mdw )
{
  if ( mainDockWidget == mdw ) return;
  mainDockWidget = mdw;
  mdw->applyToWidget(this);
}
#endif


#endif

void PMDockWidgetAbstractHeader::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

void PMDockWidgetAbstractHeaderDrag::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

void PMDockWidgetHeaderDrag::virtual_hook( int id, void* data )
{ PMDockWidgetAbstractHeaderDrag::virtual_hook( id, data ); }

void PMDockWidgetHeader::virtual_hook( int id, void* data )
{ PMDockWidgetAbstractHeader::virtual_hook( id, data ); }

void PMDockTabGroup::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

void PMDockWidget::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

void PMDockManager::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

void PMDockMainWindow::virtual_hook( int id, void* data )
{ KMainWindow::virtual_hook( id, data ); }

void PMDockArea::virtual_hook( int, void* )
{ /*KMainWindow::virtual_hook( id, data );*/ }


#ifndef NO_INCLUDE_MOCFILES // for Qt-only projects, because tmake doesn't take this name
#include "pmdockwidget.moc"
#endif
