#include <qfileinfo.h>
#include <qhbox.h>
#include <qlayout.h>
#include <qscrollview.h>
#include <qscrollbar.h>
#include <qtimer.h>
#include <qpainter.h>
#include <qprinter.h>
#include <qprintdialog.h>

#include <kaboutdialog.h>
#include <kaccel.h>
#include <kaction.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kconfigdialog.h>
#include <kdebug.h>
#include <kdirwatch.h>
#include <kfiledialog.h>
#include <kfilterbase.h>
#include <kfilterdev.h>
#include <kglobal.h>
#include <kinstance.h>
#include <kio/job.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kmimetype.h>
#include <kparts/componentfactory.h>
#include <kparts/genericfactory.h>
#include <kparts/partmanager.h>
#include <kprogress.h>
#include <kstandarddirs.h>
#include <kstdaction.h>
#include <ktempfile.h>
#include <ktrader.h>
#include <kinputdialog.h>

#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "kviewpart.h"
#include "kmultipage.h"
#include "pageSize.h"
#include "pageSizeDialog.h"
#include "zoomlimits.h"

#include "optionDialogGUIWidget_base.h"
#include "optionDialogAccessibilityWidget.h"

#include "kvsprefs.h"

#define MULTIPAGE_VERSION 2

typedef KParts::GenericFactory<KViewPart> KViewPartFactory;
K_EXPORT_COMPONENT_FACTORY(kviewerpart, KViewPartFactory)

KViewPart::KViewPart(QWidget *parentWidget, const char *widgetName, QObject *parent,
                     const char *name, const QStringList& args)
  : KViewPart_Iface(parent, name), showSidebar(0), saveAction(0), partManager(0),
    multiPageLibrary(QString::null), aboutDialog(0)
{
  KGlobal::locale()->insertCatalogue("kviewshell");

  tmpUnzipped = 0L;
  pageChangeIsConnected = false;
  setInstance(KViewPartFactory::instance());

  watch = KDirWatch::self();
  connect(watch, SIGNAL(dirty(const QString&)), this, SLOT(fileChanged(const QString&)));
  watch->startScan();

  mainWidget = new QHBox(parentWidget, widgetName);
  mainWidget->setFocusPolicy(QWidget::StrongFocus);
  setWidget(mainWidget);

  // Setup part manager
  partManager = new KParts::PartManager(parentWidget, "PartManager for kviewpart");
  setManager(partManager);
  // Don't switch to another part when pressing a mouse button
  partManager->setActivationButtonMask(0);
  // Without this the GUI-items of the KMultiPages are not merged
  partManager->setAllowNestedParts(true);

  connect(partManager, SIGNAL(activePartChanged(KParts::Part*)), this, SIGNAL(pluginChanged(KParts::Part*)));
  partManager->addPart(this);

  // create the displaying part

  // Search for service
  KTrader::OfferList offers;

  if (!args.isEmpty())
  {
    // If a default MimeType is specified try to load a MultiPage supporting it.
    QString defaultMimeType = args.first();
    offers = KTrader::self()->query(
        QString::fromLatin1("KViewShell/MultiPage" ),
        QString("([X-KDE-MultiPageVersion] == %1) and "
                "([X-KDE-MimeTypes] == '%2')").arg(MULTIPAGE_VERSION).arg(defaultMimeType));
  }

  // If no default MimeType is given or no MultiPage has been found, try to load the Empty MultiPage.
  if (offers.isEmpty())
  {
    offers = KTrader::self()->query(
        QString::fromLatin1("KViewShell/MultiPage" ),
        QString("([X-KDE-MultiPageVersion] == %1) and "
                "([X-KDE-EmptyMultiPage] == 1)").arg(MULTIPAGE_VERSION));
  }

  // If still no MultiPage has been found, report an error and abort.
  if (offers.isEmpty())
  {
    KMessageBox::error(parentWidget, i18n("<qt>No MultiPage found.</qt>"));
    //    return;
  }

  KService::Ptr service = offers.first();
  kdDebug(1223) << service->library() << endl;

  // Try to load the multiPage
  int error;
  multiPage = static_cast<KMultiPage*>(KParts::ComponentFactory::createInstanceFromService<KParts::ReadOnlyPart>(service, mainWidget,
														 service->name().utf8(), QStringList(), &error ));

  // If the loading of the MultiPage failed report and error and abort.
  if (!multiPage) {
    QString reason;
    switch(error) {
    case KParts::ComponentFactory::ErrNoServiceFound:
      reason = i18n("<qt>No service implementing the given mimetype and fullfilling the given constraint expression can be found.</qt>");
      break;
    case KParts::ComponentFactory::ErrServiceProvidesNoLibrary:
      reason = i18n("<qt>The specified service provides no shared library.</qt>");
      break;
    case KParts::ComponentFactory::ErrNoLibrary:
      reason = i18n("<qt><p>The specified library <b>%1</b> could not be loaded. The error message returned was:</p>"
		    "<p><b>%2</b></p></qt>").arg(service->library()).arg(KLibLoader::self()->lastErrorMessage());
      break;
    case KParts::ComponentFactory::ErrNoFactory:
      reason = i18n("<qt>The library does not export a factory for creating components.</qt>");
      break;
    case KParts::ComponentFactory::ErrNoComponent:
      reason = i18n("<qt>The factory does not support creating components of the specified type.</qt>");
      break;
    }

    QString text = i18n("<qt><p><b>Problem:</b> The document <b>%1</b> cannot be shown.</p>"
			"<p><b>Reason:</b> The software component <b>%2</b> which is required to "
			"display your files could not be initialized. This could point to "
			"serious misconfiguration of your KDE system, or to damaged program files.</p>"
			"<p><b>What you can do:</b> You could try to re-install the software packages in "
			"question. If that does not help, you could file an error report, either to the "
			"provider of your software (e.g. the vendor of your Linux distribution), or "
			"directly to the authors of the software. The entry <b>Report Bug...</b> in the "
			"<b>Help</b> menu helps you to contact the KDE programmers.</p></qt>").arg(m_file).arg(service->library());
    QString caption = i18n("Error Initializing Software Component");
    KMessageBox::detailedError(mainWidget, text, reason, caption);
    emit setStatusBarText(QString::null);
    return;
  }
  // Make the KViewPart the parent of the MultiPage.
  // So the Partmanager treats it as a nested KPart.
  insertChild(multiPage);

  // Remember the name of the library.
  multiPageLibrary = service->library();
  // Add the multipage to the GUI.
  partManager->addPart(multiPage);

  exportTextAction = new KAction(i18n("Text..."), 0, this, SLOT(mp_exportText()), actionCollection(), "export_text");

  // edit menu
  findTextAction = KStdAction::find(this, SLOT(mp_showFindTextDialog()), actionCollection(), "find");
  findNextAction = KStdAction::findNext(this, SLOT(mp_findNextText()), actionCollection(), "findnext");
  findNextAction->setEnabled(false);
  findPrevAction = KStdAction::findPrev(this, SLOT(mp_findPrevText()), actionCollection(), "findprev");
  findPrevAction->setEnabled(false);

  selectAllAction = KStdAction::selectAll(this, SLOT(mp_doSelectAll()), actionCollection(), "edit_select_all");

  copyTextAction = KStdAction::copy(this, SLOT(mp_copyText()), actionCollection(), "copy_text");
  copyTextAction->setEnabled(false);

  deselectAction = KStdAction::deselect(this, SLOT(mp_clearSelection()), actionCollection(), "edit_deselect_all");
  deselectAction->setEnabled(false);

  saveAction = KStdAction::save(this, SLOT(mp_slotSave_defaultFilename()), actionCollection());

  // settings menu
  showSidebar = new KToggleAction (i18n("Show &Sidebar"), "show_side_panel", 0, this,
                                   SLOT(slotShowSidebar()), actionCollection(), "show_sidebar");
  showSidebar->setCheckedState(i18n("Hide &Sidebar"));
  watchAct = new KToggleAction(i18n("&Watch File"), 0, 0, 0, actionCollection(), "watch_file");
  scrollbarHandling = new KToggleAction (i18n("Show Scrollbars"), 0, 0, 0, actionCollection(), "scrollbarHandling");
  scrollbarHandling->setCheckedState(i18n("Hide Scrollbars"));

  // View modes
  QStringList viewModes;
  viewModes.append(i18n("Single Page"));
  viewModes.append(i18n("Continuous"));
  viewModes.append(i18n("Continuous - Facing"));
  viewModes.append(i18n("Overview"));
  viewModeAction = new KSelectAction (i18n("View Mode"), 0, 0, 0, actionCollection(), "viewmode");
  viewModeAction->setItems(viewModes);

  // Orientation menu
  QStringList orientations;
  orientations.append(i18n("Portrait"));
  orientations.append(i18n("Landscape"));
  orientation = new KSelectAction (i18n("Preferred &Orientation"), 0, 0, 0, actionCollection(), "view_orientation");
  orientation->setItems(orientations);
  connect(orientation, SIGNAL(activated (int)), &userRequestedPaperSize, SLOT(setOrientation(int)));

  // Zoom Menu
  zoom_action = new KSelectAction (i18n("&Zoom"), 0, 0, 0, actionCollection(), "view_zoom");
  zoom_action->setEditable(true);
  zoom_action->setItems(_zoomVal.zoomNames());

  connect (&_zoomVal, SIGNAL(zoomNamesChanged(const QStringList &)), zoom_action, SLOT(setItems(const QStringList &)));
  connect (&_zoomVal, SIGNAL(valNoChanged(int)), zoom_action, SLOT(setCurrentItem(int)));
  connect (&_zoomVal, SIGNAL(zoomNameChanged(const QString &)), this, SIGNAL(zoomChanged(const QString &)) );
  connect (zoom_action, SIGNAL(activated(const QString &)), this, SLOT(setZoomValue(const QString &)));
  _zoomVal.setZoomValue(1.0); // should not be necessary @@@@
  emit(zoomChanged("100%"));

  // Paper Size Menu
  media = new KSelectAction (i18n("Preferred Paper &Size"), 0, 0, 0, actionCollection(), "view_media");
  QStringList items = userRequestedPaperSize.pageSizeNames();
  items.prepend(i18n("Custom Size..."));
  media->setItems(items);
  connect (media, SIGNAL(activated(int)), this, SLOT(slotMedia(int)));

  useDocumentSpecifiedSize = new KToggleAction(i18n("&Use Document Specified Paper Size"), 0, this, SLOT(slotShowSidebar()),
					       actionCollection(), "view_use_document_specified_size");

  // Zoom Actions
  zoomInAct = KStdAction::zoomIn (this, SLOT(zoomIn()), actionCollection());
  zoomOutAct = KStdAction::zoomOut(this, SLOT(zoomOut()), actionCollection());

  fitPageAct = new KToggleAction(i18n("&Fit to Page"), "view_fit_window", Key_P,
                                 actionCollection(), "view_fit_to_page");
  fitWidthAct = new KToggleAction(i18n("Fit to Page &Width"), "view_fit_width", Key_W,
                                  actionCollection(), "view_fit_to_width");
  fitHeightAct = new KToggleAction(i18n("Fit to Page &Height"), "view_fit_height", Key_H,
                                   actionCollection(), "view_fit_to_height");

  fitPageAct -> setExclusiveGroup("view_fit");
  fitWidthAct -> setExclusiveGroup("view_fit");
  fitHeightAct -> setExclusiveGroup("view_fit");

  connect(fitPageAct, SIGNAL(toggled(bool)), this, SLOT(enableFitToPage(bool)));
  connect(fitWidthAct, SIGNAL(toggled(bool)), this, SLOT(enableFitToWidth(bool)));
  connect(fitHeightAct, SIGNAL(toggled(bool)), this, SLOT(enableFitToHeight(bool)));

  // go menu
  backAct = KStdAction::prior(this, SLOT(mp_prevPage()), actionCollection());
  forwardAct = KStdAction::next(this, SLOT(mp_nextPage()), actionCollection());
  startAct = KStdAction::firstPage(this, SLOT(mp_firstPage()), actionCollection());
  endAct = KStdAction::lastPage(this, SLOT(mp_lastPage()), actionCollection());
  gotoAct = KStdAction::gotoPage(this, SLOT(goToPage()), actionCollection());
  gotoAct->setShortcut("CTRL+G");

  readUpAct = new KAction(i18n("Read Up Document"), "up", SHIFT+Key_Space, this, SLOT(mp_readUp()), actionCollection(), "go_read_up");
  readDownAct = new KAction(i18n("Read Down Document"), "down", Key_Space, this, SLOT(mp_readDown()), actionCollection(), "go_read_down");

  printAction = KStdAction::print(this, SLOT(slotPrint()), actionCollection());

  saveAsAction = KStdAction::saveAs(this, SLOT(mp_slotSave()), actionCollection());

  // mode action
  moveModeAction = new KRadioAction(i18n("&Move Tool"), "movetool", Key_F4, actionCollection(), "move_tool");
  selectionModeAction = new KRadioAction(i18n("&Selection Tool"), "selectiontool", Key_F5, actionCollection(), "selection_tool");

  moveModeAction->setExclusiveGroup("tools");
  selectionModeAction->setExclusiveGroup("tools");

  moveModeAction->setChecked(true);

  connect(moveModeAction, SIGNAL(toggled(bool)), this, SLOT(slotEnableMoveTool(bool)));
  //connect(selectionModeAction, SIGNAL(toggled(bool)), this, SLOT(slotEnableSelectionTool(bool)));

  // history action
  backAction = new KAction(i18n("&Back"), "1leftarrow", 0,
                   this, SLOT(mp_doGoBack()), actionCollection(), "go_back");
  forwardAction = new KAction(i18n("&Forward"), "1rightarrow", 0,
                      this, SLOT(mp_doGoForward()), actionCollection(), "go_forward");

  backAction->setEnabled(false);
  forwardAction->setEnabled(false);


  settingsAction = KStdAction::preferences(this, SLOT(doSettings()), actionCollection());

  // We only show this menuitem if no default mimetype is set. This usually means kviewshell
  // has been started by itself. Otherwise if KDVI or KFaxView has been started show the
  // additional about information.
  if (!args.isEmpty())
  {
    aboutAction = new KAction(i18n("About KViewShell"), "kviewshell", 0, this,
                            SLOT(aboutKViewShell()), actionCollection(), "help_about_kviewshell");
  }

  // keyboard accelerators
  accel = new KAccel(mainWidget);
  accel->insert(I18N_NOOP("Scroll Up"), Key_Up, this, SLOT(mp_scrollUp()));
  accel->insert(I18N_NOOP("Scroll Down"), Key_Down, this, SLOT(mp_scrollDown()));
  accel->insert(I18N_NOOP("Scroll Left"), Key_Left, this, SLOT(mp_scrollLeft()));
  accel->insert(I18N_NOOP("Scroll Right"), Key_Right, this, SLOT(mp_scrollRight()));

  accel->insert(I18N_NOOP("Scroll Up Page"), SHIFT+Key_Up, this, SLOT(mp_scrollUpPage()));
  accel->insert(I18N_NOOP("Scroll Down Page"), SHIFT+Key_Down, this, SLOT(mp_scrollDownPage()));
  accel->insert(I18N_NOOP("Scroll Left Page"), SHIFT+Key_Left, this, SLOT(mp_scrollLeftPage()));
  accel->insert(I18N_NOOP("Scroll Right Page"), SHIFT+Key_Right, this, SLOT(mp_scrollRightPage()));

  accel->readSettings();
  readSettings();

  m_extension = new KViewPartExtension(this);

  setXMLFile("kviewerpart.rc");

  initializeMultiPage();

  // The page size dialog is constructed on first usage -- saves some
  // memory when not used.
  _pageSizeDialog = 0;

  checkActions();
  viewModeAction->setCurrentItem(KVSPrefs::viewMode());

  // We disconnect because we dont want some FocusEvents to trigger a GUI update, which might mess
  // with our menus.
  disconnect(partManager, SIGNAL(activePartChanged(KParts::Part*)), this, SIGNAL(pluginChanged(KParts::Part*)));
}

KViewPart::~KViewPart()
{
  writeSettings();

  // Without the next two lines, konqueror crashes when it is quit
  // while displaying a DVI file. I don't really understand
  // why... --Stefan.
  if (manager() != 0)
    manager()->removePart(this);

  // Delete the partManager;
  setManager(0);
  delete partManager;

  delete multiPage;
  delete tmpUnzipped;
}


void KViewPart::initializeMultiPage()
{
  // Paper Size handling
  multiPage->setUseDocumentSpecifiedSize(useDocumentSpecifiedSize->isChecked());
  multiPage->setUserPreferredSize(userRequestedPaperSize);
  connect(&userRequestedPaperSize, SIGNAL(sizeChanged(const SimplePageSize&)), multiPage, SLOT(setUserPreferredSize(const SimplePageSize&)));
  connect(useDocumentSpecifiedSize, SIGNAL(toggled(bool)), multiPage, SLOT(setUseDocumentSpecifiedSize(bool)));


  connect(scrollbarHandling, SIGNAL(toggled(bool)),  multiPage, SLOT(slotShowScrollbars(bool)));

  // connect to the multi page view
  connect( this, SIGNAL(scrollbarStatusChanged(bool)), multiPage, SLOT(slotShowScrollbars(bool)));
  connect( multiPage, SIGNAL(pageInfo(int, int)), this, SLOT(pageInfo(int, int)) );
  connect( multiPage, SIGNAL(askingToCheckActions()), this, SLOT(checkActions()) );
  connect( multiPage, SIGNAL( started( KIO::Job * ) ), this, SIGNAL( started( KIO::Job * ) ) );
  connect( multiPage, SIGNAL( completed() ), this, SIGNAL( completed() ) );
  connect( multiPage, SIGNAL( canceled( const QString & ) ), this, SIGNAL( canceled( const QString & ) ) );
  connect( multiPage, SIGNAL( setStatusBarText( const QString& ) ), this, SLOT( setStatusBarTextFromMultiPage( const QString& ) ) );

  connect( multiPage, SIGNAL(zoomIn()), this, SLOT(zoomIn()) );
  connect( multiPage, SIGNAL(zoomOut()), this, SLOT(zoomOut()) );

  // change the viewmode
  connect(viewModeAction, SIGNAL(activated (int)), multiPage, SLOT(setViewMode(int)));

  // Update zoomlevel on viewmode changes
  connect(multiPage, SIGNAL(viewModeChanged()), this, SLOT(updateZoomLevel()));

  // navigation history
  connect(multiPage->history(), SIGNAL(backItem(bool)), backAction, SLOT(setEnabled(bool)));
  connect(multiPage->history(), SIGNAL(forwardItem(bool)), forwardAction, SLOT(setEnabled(bool)));

  // text selection
  connect(multiPage, SIGNAL(textSelected(bool)), copyTextAction, SLOT(setEnabled(bool)));
  connect(multiPage, SIGNAL(textSelected(bool)), deselectAction, SLOT(setEnabled(bool)));

  // text search
  connect(multiPage, SIGNAL(searchEnabled(bool)), findNextAction, SLOT(setEnabled(bool)));
  connect(multiPage, SIGNAL(searchEnabled(bool)), findPrevAction, SLOT(setEnabled(bool)));

  // allow parts to have a GUI, too :-)
  // (will be merged automatically)
  insertChildClient( multiPage );
}

void KViewPart::slotStartFitTimer()
{
  fitTimer.start(100, true);
}


QString KViewPart::pageSizeDescription()
{
  PageNumber nr = multiPage->currentPageNumber();
  if (!nr.isValid())
    return QString::null;
  SimplePageSize ss = multiPage->sizeOfPage(nr);
  if (!ss.isValid())
    return QString::null;
  pageSize s(ss);

  QString size = " ";
  if (s.formatNumber() == -1) {
    if (KGlobal::locale()-> measureSystem() == KLocale::Metric)
      size += QString("%1x%2 mm").arg(s.width().getLength_in_mm(), 0, 'f', 0).arg(s.height().getLength_in_mm(), 0, 'f', 0);
    else
      size += QString("%1x%2 in").arg(s.width().getLength_in_inch(), 0, 'g', 2).arg(s.height().getLength_in_inch(), 0, 'g', 2);
  } else {
    size += s.formatName() + "/";
    if (s.getOrientation() == 0)
      size += i18n("portrait");
    else
      size += i18n("landscape");
  }
  return size+" ";
}


void KViewPart::restoreDocument(const KURL &url, int page)
{
  if (openURL(url))
    multiPage->gotoPage(page);
}


void KViewPart::saveDocumentRestoreInfo(KConfig* config)
{
  config->writePathEntry("URL", url().url());
  if (multiPage->numberOfPages() > 0)
    config->writeEntry("Page", multiPage->currentPageNumber());
}


void KViewPart::slotFileOpen()
{
  if ((!multiPage.isNull()) && (multiPage->isModified() == true)) {
    int ans = KMessageBox::warningContinueCancel( 0,
					 i18n("Your document has been modified. Do you really want to open another document?"),
					 i18n("Warning - Document Was Modified"),KStdGuiItem::open());
    if (ans == KMessageBox::Cancel)
      return;
  }

  KURL url = KFileDialog::getOpenURL(QString::null, supportedMimeTypes().join(" "));

  if (!url.isEmpty())
    openURL(url);
}

QStringList KViewPart::supportedMimeTypes()
{
  QStringList supportedMimeTypes;

  // Search for service
  KTrader::OfferList offers = KTrader::self()->query(
      QString::fromLatin1("KViewShell/MultiPage"),
      QString("([X-KDE-MultiPageVersion] == %1)").arg(MULTIPAGE_VERSION)
  );

  if (!offers.isEmpty())
  {
    KTrader::OfferList::ConstIterator iterator = offers.begin();
    KTrader::OfferList::ConstIterator end = offers.end();

    for (; iterator != end; ++iterator)
    {
      KService::Ptr service = *iterator;
      QString mimeType = service->property("X-KDE-MimeTypes").toString();
      supportedMimeTypes << mimeType;

    }
  }

  // The kviewshell is also able to read compressed files and to
  // uncompress them on the fly.

  // Check if this version of KDE supports bzip2
  bool bzip2Available = (KFilterBase::findFilterByMimeType( "application/x-bzip2" ) != 0L);

  supportedMimeTypes << "application/x-gzip";

  if (bzip2Available)
  {
    supportedMimeTypes << "application/x-bzip2";
  }

  return supportedMimeTypes;
}

QStringList KViewPart::fileFormats() const
{
  // Compile a list of the supported filename patterns

  // First we build a list of the mimetypes which are supported by the
  // currently installed KMultiPage-Plugins.
  QStringList supportedMimeTypes;
  QStringList supportedPattern;

  // Search for service
  KTrader::OfferList offers = KTrader::self()->query(
      QString::fromLatin1("KViewShell/MultiPage"),
      QString("([X-KDE-MultiPageVersion] == %1)").arg(MULTIPAGE_VERSION)
  );

  if (!offers.isEmpty())
  {
    KTrader::OfferList::ConstIterator iterator = offers.begin();
    KTrader::OfferList::ConstIterator end = offers.end();

    for (; iterator != end; ++iterator)
    {
      KService::Ptr service = *iterator;
      QString mimeType = service->property("X-KDE-MimeTypes").toString();
      supportedMimeTypes << mimeType;

      QStringList pattern = KMimeType::mimeType(mimeType)->patterns();
      while(!pattern.isEmpty())
      {
        supportedPattern.append(pattern.front().stripWhiteSpace());
        pattern.pop_front();
      }
    }
  }

  // The kviewshell is also able to read compressed files and to
  // uncompress them on the fly. Thus, we modify the list of supported
  // file formats which we obtain from the multipages to include
  // compressed files like "*.dvi.gz". We add "*.dvi.bz2" if support
  // for bzip2 is compiled into KDE.

  // Check if this version of KDE supports bzip2
  bool bzip2Available = (KFilterBase::findFilterByMimeType( "application/x-bzip2" ) != 0L);

  QStringList compressedPattern;

  for(QStringList::Iterator it = supportedPattern.begin(); it != supportedPattern.end(); ++it )
  {
    if ((*it).find(".gz", -3) == -1) // Paranoia safety check
      compressedPattern.append(*it + ".gz");

    if ((bzip2Available) && ((*it).find(".bz2", -4) == -1)) // Paranoia safety check
      compressedPattern.append(*it + ".bz2");
  }

  while (!compressedPattern.isEmpty())
  {
    supportedPattern.append(compressedPattern.front());
    compressedPattern.pop_front();
  }

  kdDebug(1223) << "Supported Pattern: " << supportedPattern << endl;

  return supportedPattern;
}


void KViewPart::slotSetFullPage(bool fullpage)
{
  if (multiPage)
    multiPage->slotSetFullPage(fullpage);
  else
    kdError(1223) << "KViewPart::slotSetFullPage() called without existing multipage" << endl;

  // Restore normal view
  if (fullpage == false)
  {
    slotShowSidebar();
    multiPage->slotShowScrollbars(scrollbarHandling->isChecked());
  }
}


void KViewPart::slotShowSidebar()
{
  bool show = showSidebar->isChecked();
  multiPage->slotShowSidebar(show);
}


bool KViewPart::openFile()
{
  KURL tmpFileURL;

  // We try to be error-tolerant about filenames. If the user calls us
  // with something like "test", and we are using the DVI-part, we'll
  // also look for "testdvi" and "test.dvi".
  QFileInfo fi(m_file);
  m_file = fi.absFilePath();

  if (!fi.exists())
  {
    QStringList supportedPatterns = fileFormats();
    QStringList endings;

    for (QStringList::Iterator it = supportedPatterns.begin(); it != supportedPatterns.end(); ++it)
    {
      // Only consider patterns starting with "*."
      if ((*it).find("*.") == 0)
      {
        // Remove first Letter from string
        endings.append((*it).mid(2).stripWhiteSpace());
      }
    }
    kdDebug(1223) << "Supported Endings: " << endings << endl;

    // Now try to append the endings with and without "." to the given filename,
    // and see if that gives a existing file.
    for (QStringList::Iterator it = endings.begin(); it != endings.end(); ++it)
    {
      fi.setFile(m_file+(*it));
      if (fi.exists())
      {
        m_file = m_file+(*it);
        break;
      }
      fi.setFile(m_file+"."+(*it));
      if (fi.exists())
      {
        m_file = m_file+"."+(*it);
        break;
      }
    }

    // If we still have not found a file. Show an error message and return.
    if (!fi.exists())
    {
      KMessageBox::error(mainWidget, i18n("<qt>File <nobr><strong>%1</strong></nobr> does not exist.</qt>").arg(m_file));
      emit setStatusBarText(QString::null);
      return false;
    }
    m_url.setPath(QFileInfo(m_file).absFilePath());
  }

  // Set the window caption now, before we do any uncompression and generation of temporary files.
  tmpFileURL.setPath(m_file);
  emit setStatusBarText(i18n("Loading '%1'...").arg(tmpFileURL.prettyURL()));
  emit setWindowCaption( tmpFileURL.prettyURL() ); // set Window caption WITHOUT the reference part!

  // Check if the file is compressed
  KMimeType::Ptr mimetype = KMimeType::findByPath( m_file );

  if (( mimetype->name() == "application/x-gzip" ) || ( mimetype->name() == "application/x-bzip2" ) ||
      ( mimetype->parentMimeType() == "application/x-gzip" ) ||
      ( mimetype->parentMimeType() == "application/x-bzip2" ))
  {
    // The file is compressed. Make a temporary file, and store an uncompressed version there...
    if (tmpUnzipped != 0L)  // Delete old temporary file
      delete tmpUnzipped;

    tmpUnzipped = new KTempFile;
    if (tmpUnzipped == 0L)
    {
      KMessageBox::error(mainWidget, i18n("<qt><strong>File Error!</strong> Could not create "
                         "temporary file.</qt>"));
      emit setWindowCaption(QString::null);
      emit setStatusBarText(QString::null);
      return false;
    }
    tmpUnzipped->setAutoDelete(true);
    if(tmpUnzipped->status() != 0)
    {
      KMessageBox::error(mainWidget, i18n("<qt><strong>File Error!</strong> Could not create temporary file "
                         "<nobr><strong>%1</strong></nobr>.</qt>").arg(strerror(tmpUnzipped->status())));
      emit setWindowCaption(QString::null);
      emit setStatusBarText(QString::null);
      return false;
    }

    QIODevice* filterDev;
    if (( mimetype->parentMimeType() == "application/x-gzip" ) ||
        ( mimetype->parentMimeType() == "application/x-bzip2" ))
      filterDev = KFilterDev::deviceForFile(m_file, mimetype->parentMimeType());
    else
      filterDev = KFilterDev::deviceForFile(m_file);
    if (filterDev == 0L)
    {
      emit setWindowCaption(QString::null);
      emit setStatusBarText(QString::null);
      return false;
    }
    if(!filterDev->open(IO_ReadOnly))
    {
      KMessageBox::detailedError(mainWidget, i18n("<qt><strong>File Error!</strong> Could not open the file "
          "<nobr><strong>%1</strong></nobr> for uncompression. "
          "The file will not be loaded.</qt>").arg(m_file),
          i18n("<qt>This error typically occurs if you do not have enough permissions to read the file. "
          "You can check ownership and permissions if you right-click on the file in the Konqueror "
          "file manager and then choose the 'Properties' menu.</qt>"));
      emit setWindowCaption(QString::null);
      delete filterDev;
      emit setStatusBarText(QString::null);
      return false;
    }

    KProgressDialog* prog = new KProgressDialog(0L, "uncompress-progress",
        i18n("Uncompressing..."),
        i18n("<qt>Uncompressing the file <nobr><strong>%1</strong></nobr>. Please wait.</qt>").arg(m_file));

    prog->progressBar()->setTotalSteps((int) fi.size()/1024);
    prog->progressBar()->setProgress(0);
    prog->setMinimumDuration(250);

    QByteArray buf(1024);
    int read = 0, wrtn = 0;

    bool progress_dialog_was_cancelled = false;
    while ((read = filterDev->readBlock(buf.data(), buf.size())) > 0)
    {
      kapp->processEvents();
      progress_dialog_was_cancelled = prog->wasCancelled();
      if (progress_dialog_was_cancelled)
        break;
      prog->progressBar()->advance(1);

      wrtn = tmpUnzipped->file()->writeBlock(buf.data(), read);
      if(read != wrtn)
        break;
    }
    delete filterDev;
    delete prog;
    tmpUnzipped->sync();

    if (progress_dialog_was_cancelled) {
      emit setStatusBarText(QString::null);
      return false;
    }

    if ((read != 0) || (tmpUnzipped->file()->size() == 0))
    {
      KMessageBox::detailedError(mainWidget, i18n("<qt><strong>File Error!</strong> Could not uncompress "
          "the file <nobr><strong>%1</strong></nobr>. The file will not be loaded.</qt>").arg( m_file ),
          i18n("<qt>This error typically occurs if the file is corrupt. "
          "If you want to be sure, try to decompress the file manually using command-line tools.</qt>"));
      emit setWindowCaption(QString::null);
      emit setStatusBarText(QString::null);
      return false;
    }
    tmpUnzipped->close();

    m_file = tmpUnzipped->name();
  }

  // Now call the openURL-method of the multipage and give it an URL
  // pointing to the downloaded file.
  tmpFileURL.setPath(m_file);
  // Pass the reference part of the URL through to the multipage
  tmpFileURL.setRef(m_url.ref());

  mimetype = KMimeType::findByURL(tmpFileURL);

  // Search for service
  KTrader::OfferList offers = KTrader::self()->query(
      QString::fromLatin1("KViewShell/MultiPage" ),
  QString("([X-KDE-MultiPageVersion] == %1) and "
          "([X-KDE-MimeTypes] == '%2')").arg(MULTIPAGE_VERSION).arg(mimetype->name()));

  if (offers.isEmpty()) {
    KMessageBox::detailedError(mainWidget, i18n("<qt>The document <b>%1</b> cannot be shown because "
						"its file type is not supported.</qt>").arg(m_file),
			       i18n("<qt>The file has mime type <b>%1</b> which is not supported by "
				    "any of the installed KViewShell plugins.</qt>").arg(mimetype->name()));
    emit setWindowCaption(QString::null);
    emit setStatusBarText(QString::null);
    return false;
  }

  KService::Ptr service = offers.first();

  // The the new multiPage is different then the currently loaded one.
  if (service->library() != multiPageLibrary)
  {
    // We write the settings before we load the new multipage, so
    // that the new multipage gets the same settings than the
    // currently loaded one.
    writeSettings();

    // Delete old config dialog
    KConfigDialog* configDialog = KConfigDialog::exists("kviewshell_config");
    delete configDialog;

    KMultiPage* oldMultiPage = multiPage;

    // Try to load the multiPage
    int error;
    multiPage = static_cast<KMultiPage*>(KParts::ComponentFactory::createInstanceFromService<KParts::ReadOnlyPart>(service, mainWidget,
														   service->name().utf8(), QStringList(), &error ));

    if (multiPage.isNull()) {
      QString reason;
      switch(error) {
      case KParts::ComponentFactory::ErrNoServiceFound:
	reason = i18n("<qt>No service implementing the given mimetype and fullfilling the given constraint expression can be found.</qt>");
	break;
      case KParts::ComponentFactory::ErrServiceProvidesNoLibrary:
	reason = i18n("<qt>The specified service provides no shared library.</qt>");
	break;
      case KParts::ComponentFactory::ErrNoLibrary:
	reason = i18n("<qt><p>The specified library <b>%1</b> could not be loaded. The error message returned was:</p> <p><b>%2</b></p></qt>").arg(service->library()).arg(KLibLoader::self()->lastErrorMessage());
	break;
      case KParts::ComponentFactory::ErrNoFactory:
	reason = i18n("<qt>The library does not export a factory for creating components.</qt>");
	break;
      case KParts::ComponentFactory::ErrNoComponent:
	reason = i18n("<qt>The factory does not support creating components of the specified type.</qt>");
	break;
      }

      QString text = i18n("<qt><p><b>Problem:</b> The document <b>%1</b> cannot be shown.</p>"
			  "<p><b>Reason:</b> The software "
			  "component <b>%2</b> which is required to display files of type <b>%3</b> could "
			  "not be initialized. This could point to serious misconfiguration of your KDE "
			  "system, or to damaged program files.</p>"
			  "<p><b>What you can do:</b> You could try to re-install the software packages in "
			  "question. If that does not help, you could file an error report, either to the "
			  "provider of your software (e.g. the vendor of your Linux distribution), or "
			  "directly to the authors of the software. The entry <b>Report Bug...</b> in the "
			  "<b>Help</b> menu helps you to contact the KDE programmers.</p></qt>").arg(m_file).arg(service->library()).arg(mimetype->name());
      QString caption = i18n("Error Initializing Software Component");
      if (reason.isEmpty())
	KMessageBox::error(mainWidget, text, caption);
      else
	KMessageBox::detailedError(mainWidget, text, reason, caption);
      emit setStatusBarText(QString::null);
      return false;
    }

    // Remember the name of the part. So only need to switch if really necessary.
    multiPageLibrary = service->library();

    connect(partManager, SIGNAL(activePartChanged(KParts::Part*)), this, SIGNAL(pluginChanged(KParts::Part*)));

    // Switch to the new multiPage
    partManager->replacePart(oldMultiPage, multiPage);

    delete oldMultiPage;

    // The next line makes the plugin switch much more smooth. Without it the new document
    // is at first show at a very small zoomlevel before the zoom switches to the right value.
    // This makes the plugin switching actually slower.
    // TODO: Get rid of this without causing nasty artifacts.
    kapp->processEvents();
    initializeMultiPage();
    partManager->setActivePart(this);

    // We disconnect because we dont want some FocusEvents to trigger a GUI update, which might mess
    // with our menus.
    disconnect(partManager, SIGNAL(activePartChanged(KParts::Part*)), this, SIGNAL(pluginChanged(KParts::Part*)));

    readSettings();
  }

  // Set the multipage to the current viewmode.
  multiPage->setViewMode(viewModeAction->currentItem());

  // Load the URL
  bool r = multiPage->openURL(m_file, m_url);
  updateZoomLevel(); // @@@@@@@@@@@@@

  // We disable the selection tool for plugins that dont support text.
  // Currently this is only the fax plugin.
  if (multiPage->supportsTextSearch())
  {
    selectionModeAction->setEnabled(true);
  }
  else
  {
    selectionModeAction->setEnabled(false);
    moveModeAction->setChecked(true);
  }
  // Switch the new multipage to the right tool
  slotEnableMoveTool(moveModeAction->isChecked());

  if (r) {
    // Add the file to the watchlist
    watch->addFile( m_file );

    // Notify the ViewShell about the newly opened file.
    emit fileOpened();
  } else {
    m_url = QString::null;
    emit setWindowCaption(QString::null);
  }

  checkActions();
  emit zoomChanged(QString("%1%").arg((int)(_zoomVal.value()*100.0+0.5)));
  emit setStatusBarText(QString::null);
  return r;
}


void KViewPart::reload()
{
  multiPage->reload();
}


void KViewPart::fileChanged(const QString &file)
{
  if (file == m_file && watchAct->isChecked())
    multiPage->reload();
}


bool KViewPart::closeURL_ask()
{
  if (multiPage.isNull())
    return false;

  if (multiPage->isModified() == true) {
    int ans = KMessageBox::warningContinueCancel( 0,
					 i18n("Your document has been modified. Do you really want to close it?"),
					 i18n("Document Was Modified"), KStdGuiItem::close());
    if (ans == KMessageBox::Cancel)
      return false;
  }

  return closeURL();
}


bool KViewPart::closeURL()
{
  if (multiPage.isNull())
    return false;

  if( watch && !m_file.isEmpty() )
    watch->removeFile( m_file );

  KParts::ReadOnlyPart::closeURL();
  multiPage->closeURL();
  m_url = QString::null;
  checkActions();
  emit setWindowCaption("");

  return true;
}


void KViewPart::slotMedia(int id)
{
  // If the user has chosen one of the 'known' paper sizes, set the
  // user requested paper size to that value. Via signals and slots,
  // this will update the menus, and also the GUI, if necessary.
  if (id > 1) {
    userRequestedPaperSize.setPageSize(media->currentText());
    return;
  }

  // If the user has chosen "Custom paper size..", show the paper size
  // dialog. Construct it, if necessary. The paper size dialog will
  // know the address of userRequestedPaperSize and change this
  // member, if the user clicks ok/accept. The signal/slot mechanism
  // will then make sure that the necessary updates in the GUI are
  // done.
  if (_pageSizeDialog == 0) {
    _pageSizeDialog = new pageSizeDialog(mainWidget, &userRequestedPaperSize);
    if (_pageSizeDialog == 0) {
      kdError(1223) << "Could not construct the page size dialog!" << endl;
      return;
    }
  }

  // Reset the "preferred paper size" menu. We don't want to have the
  // "custom paper size" check if the user aborts the dialog.
  checkActions();

  // Set or update the paper size dialog to show the currently
  // selected value.
  _pageSizeDialog->setPageSize(userRequestedPaperSize.serialize());
  _pageSizeDialog->show();
}


void KViewPart::pageInfo(int numpages, int currentpage)
{
  updateZoomLevel();

  // ATTN: The string here must be the same as in setPage() below
  QString pageString = i18n("Page %1 of %2").arg(currentpage).arg(numpages);
  if (pageChangeIsConnected) {
    emit pageChanged(pageString);
    emit sizeChanged(pageSizeDescription());
  } else
    emit setStatusBarText(pageString);

  checkActions();
}


void KViewPart::goToPage()
{
  bool ok = false;
  int p = KInputDialog::getInteger(i18n("Go to Page"), i18n("Page:"),
                                   multiPage->currentPageNumber(), 1, multiPage->numberOfPages(),
                                   1 /*step*/, &ok, mainWidget, "gotoDialog");
  if (ok)
    multiPage->gotoPage(p);
}


void KViewPart::disableZoomFit()
{
  if (fitPageAct -> isChecked())
  {
    fitPageAct -> setChecked(false);
    enableFitToPage(false);
  }
  else if(fitWidthAct -> isChecked())
  {
    fitWidthAct -> setChecked(false);
    enableFitToWidth(false);
  }
  else if (fitHeightAct -> isChecked())
  {
    fitHeightAct -> setChecked(false);
    enableFitToHeight(false);
  }
}

void KViewPart::zoomIn()
{
  disableZoomFit();

  float oldVal = _zoomVal.value();
  float newVal = _zoomVal.zoomIn();

  if (oldVal != newVal)
    _zoomVal.setZoomValue(multiPage->setZoom(_zoomVal.zoomIn()));
}


void KViewPart::zoomOut()
{
  disableZoomFit();

  float oldVal = _zoomVal.value();
  float newVal = _zoomVal.zoomOut();

  if (oldVal != newVal)
    _zoomVal.setZoomValue(multiPage->setZoom(_zoomVal.zoomOut()));
}

void KViewPart::updateZoomLevel()
{
  if (fitPageAct->isChecked())
  {
    fitToPage();
  }
  else if (fitWidthAct->isChecked())
  {
    fitToWidth();
  }
  else if (fitHeightAct->isChecked())
  {
    fitToHeight();
  }
  else
  {
    // Manuell Zoom
  }
}

void KViewPart::enableFitToPage(bool enable)
{
  if (enable)
  {
    fitToPage();
    connect(multiPage->mainWidget(), SIGNAL(viewSizeChanged(const QSize&)),
            this, SLOT(slotStartFitTimer()));
    connect(&fitTimer, SIGNAL(timeout()), SLOT(fitToPage()));
  }
  else
  {
    disconnect(multiPage->mainWidget(), SIGNAL(viewSizeChanged(const QSize&)),
               this, SLOT(slotStartFitTimer()));
    disconnect(&fitTimer, SIGNAL(timeout()), this, SLOT(fitToPage()));
  }
}

void KViewPart::enableFitToWidth(bool enable)
{
  if (enable)
  {
    fitToWidth();
    connect(multiPage->mainWidget(), SIGNAL(viewSizeChanged(const QSize&)),
            this, SLOT(slotStartFitTimer()));
    connect(&fitTimer, SIGNAL(timeout()), SLOT(fitToWidth()));
  }
  else
  {
    disconnect(multiPage->mainWidget(), SIGNAL(viewSizeChanged(const QSize&)),
               this, SLOT(slotStartFitTimer()));
    disconnect(&fitTimer, SIGNAL(timeout()), this, SLOT(fitToWidth()));
  }
}

void KViewPart::enableFitToHeight(bool enable)
{
  if (enable)
  {
    fitToHeight();
    connect(multiPage->mainWidget(), SIGNAL(viewSizeChanged(const QSize&)),
            this, SLOT(slotStartFitTimer()));
    connect(&fitTimer, SIGNAL(timeout()), SLOT(fitToHeight()));
  }
  else
  {
    disconnect(multiPage->mainWidget(), SIGNAL(viewSizeChanged(const QSize&)),
               this, SLOT(slotStartFitTimer()));
    disconnect(&fitTimer, SIGNAL(timeout()), this, SLOT(fitToHeight()));
  }
}


void KViewPart::fitToPage()
{
  double z = QMIN(multiPage->calculateFitToHeightZoomValue(),
                  multiPage->calculateFitToWidthZoomValue());

  // Check if the methods returned usable values. Values that are not
  // within the limits indicate that fitting to width or height is
  // currently not possible (e.g. because no document is
  // loaded). In that case, we abort.
  if ((z < ZoomLimits::MinZoom/1000.0) || (z > ZoomLimits::MaxZoom/1000.0))
    return;

  multiPage->setZoom(z);
  _zoomVal.setZoomFitPage(z);
}


void KViewPart::fitToHeight()
{
  double z = multiPage->calculateFitToHeightZoomValue();

  // Check if the method returned a usable value. Values that are not
  // within the limits indicate that fitting to height is currently
  // not possible (e.g. because no document is loaded). In that case,
  // we abort.
  if ((z < ZoomLimits::MinZoom/1000.0) || (z > ZoomLimits::MaxZoom/1000.0))
    return;

  multiPage->setZoom(z);
  _zoomVal.setZoomFitHeight(z);
}


void KViewPart::fitToWidth()
{
  double z = multiPage->calculateFitToWidthZoomValue();

  // Check if the method returned a usable value. Values that are not
  // within the limits indicate that fitting to width is currently not
  // possible (e.g. because no document is loaded). In that case, we
  // abort.
  if ((z < ZoomLimits::MinZoom/1000.0) || (z > ZoomLimits::MaxZoom/1000.0))
    return;

  multiPage->setZoom(z);
  _zoomVal.setZoomFitWidth(z);
}


void KViewPart::setZoomValue(const QString &sval)
{
  if (sval == i18n("Fit to Page Width"))
  {
    fitWidthAct -> setChecked(true);
    fitToWidth();
  }
  else if (sval == i18n("Fit to Page Height"))
  {
    fitHeightAct -> setChecked(true);
    fitToHeight();
  }
  else if (sval == i18n("Fit to Page"))
  {
    fitPageAct -> setChecked(true);
    fitToPage();
  }
  else
  {
    disableZoomFit();

    float fval = _zoomVal.value();
    _zoomVal.setZoomValue(sval);
    if (fval != _zoomVal.value())
      _zoomVal.setZoomValue(multiPage->setZoom(_zoomVal.value()));
  }

  mainWidget->setFocus();
}


void KViewPart::checkActions()
{
  if (multiPage.isNull())
    return;

  int currentPage = multiPage->currentPageNumber();
  int numberOfPages = multiPage->numberOfPages();

  bool doc = !url().isEmpty();

  useDocumentSpecifiedSize->setEnabled(multiPage->hasSpecifiedPageSizes() );

  if (multiPage->overviewMode())
  {
    int visiblePages = multiPage->getNrRows() *
                       multiPage->getNrColumns();

    // firstVisiblePage is the smallest currently shown pagenumber.
    int firstVisiblePage = currentPage - (currentPage % visiblePages);

    backAct->setEnabled(doc && currentPage >= visiblePages);
    forwardAct->setEnabled(doc && firstVisiblePage <= numberOfPages - visiblePages);

    startAct->setEnabled(doc && firstVisiblePage > 1);
    endAct->setEnabled(doc && firstVisiblePage + visiblePages < numberOfPages);
  }
  else
  {
    backAct->setEnabled(doc && currentPage > 1);
    forwardAct->setEnabled(doc && currentPage < numberOfPages);

    startAct->setEnabled(doc && currentPage > 1);
    endAct->setEnabled(doc && currentPage < numberOfPages);
  }

  gotoAct->setEnabled(doc && numberOfPages > 1);
  readDownAct->setEnabled(doc);
  readUpAct->setEnabled(doc);

  zoomInAct->setEnabled(doc);
  zoomOutAct->setEnabled(doc);

  fitPageAct->setEnabled(doc);
  fitHeightAct->setEnabled(doc);
  fitWidthAct->setEnabled(doc);

  media->setEnabled(doc);
  orientation->setEnabled(doc);

  printAction->setEnabled(doc);

  saveAction->setEnabled(multiPage->isModified());

  saveAsAction->setEnabled(doc);

  if (userRequestedPaperSize.formatNumber() != -1) {
    orientation->setCurrentItem(userRequestedPaperSize.getOrientation());
    orientation->setEnabled(true);
    media->setCurrentItem(userRequestedPaperSize.formatNumber()+1);
  } else {
    orientation->setEnabled(false);
    media->setCurrentItem(userRequestedPaperSize.formatNumber()-1);
  }

  bool textSearch = false;
  if (doc && multiPage->supportsTextSearch())
    textSearch = true;

  exportTextAction->setEnabled(textSearch);
  findTextAction->setEnabled(textSearch);
  selectAllAction->setEnabled(textSearch);
}


void KViewPart::slotPrint()
{
  // TODO: REMOVE THIS METHOD
  // @@@@@@@@@@@@@@@
  multiPage->print();
}


void KViewPart::readSettings()
{
  showSidebar->setChecked(KVSPrefs::pageMarks());
  slotShowSidebar();

  watchAct->setChecked(KVSPrefs::watchFile());

  // Read zoom value. Even if 'fitToPage' has been set above, there is
  // no widget available right now, so setting a good default value
  // from the configuration file is perhaps not a bad idea.
  float _zoom = KVSPrefs::zoom();
  if ( (_zoom < ZoomLimits::MinZoom/1000.0) || (_zoom > ZoomLimits::MaxZoom/1000.0)) {
    kdWarning(1223) << "Illeagal zoom value of " << _zoom*100.0 << "% found in the preferences file. Setting zoom to 100%." << endl;
    _zoom = 1.0;
  }
  _zoomVal.setZoomValue(multiPage->setZoom(_zoom));

  // The value 'fitToPage' has several meanings: 1 is 'fit to page
  // width', 2 is 'fit to page height', 3 is 'fit to page'. Other
  // values indicate 'no fit to page'. Note: at the time this code is
  // executed, the methods fitToWidth(), etc., do not work well at all
  // (perhaps some data is not initialized yet)? For that reason, we
  // do not call these methods, and load the last zoom-value from the
  // configuration file below. The hope is that this value is not
  // terribly wrong. If the user doesn't like it, it suffices to
  // resize the window just a bit...
  switch(KVSPrefs::fitToPage()) {
  case KVSPrefs::EnumFitToPage::FitToPage:
    fitPageAct->setChecked(true);
    _zoomVal.setZoomFitPage(_zoom);
    enableFitToPage(true);
    break;
  case KVSPrefs::EnumFitToPage::FitToPageWidth:
    fitWidthAct->setChecked(true);
    _zoomVal.setZoomFitWidth(_zoom);
    enableFitToWidth(true);
    break;
  case KVSPrefs::EnumFitToPage::FitToPageHeight:
    fitHeightAct->setChecked(true);
    _zoomVal.setZoomFitHeight(_zoom);
    enableFitToHeight(true);
    break;
  }

  // Read Paper Size. and orientation. The GUI is updated
  // automatically by the signals/slots mechanism whenever
  // userRequestedPaperSize is changed.
  userRequestedPaperSize.setPageSize(KVSPrefs::paperFormat());

  // Check if scrollbars should be shown.
  bool sbstatus = KVSPrefs::scrollbars();
  scrollbarHandling->setChecked(sbstatus);
  emit scrollbarStatusChanged(sbstatus);

  // Check if document specified paper sizes should be shown. We do
  // not need to take any action here, because this method is called
  // only in the constructor of the KViewPart when no document is loaded.
  useDocumentSpecifiedSize->setChecked(KVSPrefs::useDocumentSpecifiedSize());

  multiPage->readSettings();
}


void KViewPart::writeSettings()
{
  KVSPrefs::setPageMarks(showSidebar->isChecked());
  KVSPrefs::setWatchFile(watchAct->isChecked());
  KVSPrefs::setZoom(_zoomVal.value());
  KVSPrefs::setPaperFormat(userRequestedPaperSize.serialize());
  KVSPrefs::setScrollbars(scrollbarHandling->isChecked());
  KVSPrefs::setUseDocumentSpecifiedSize(useDocumentSpecifiedSize->isChecked());

  if (!multiPage.isNull())
    multiPage->writeSettings();

  if (fitPageAct->isChecked())
    KVSPrefs::setFitToPage(KVSPrefs::EnumFitToPage::FitToPage);
  else if(fitWidthAct->isChecked())
    KVSPrefs::setFitToPage(KVSPrefs::EnumFitToPage::FitToPageWidth);
  else if (fitHeightAct->isChecked())
    KVSPrefs::setFitToPage(KVSPrefs::EnumFitToPage::FitToPageHeight);
  else
    KVSPrefs::setFitToPage(KVSPrefs::EnumFitToPage::DontFit);

  KVSPrefs::writeConfig();
}


void KViewPart::connectNotify ( const char *sig )
{
  if (QString(sig).contains("pageChanged"))
    pageChangeIsConnected = true;
}


void KViewPart::setStatusBarTextFromMultiPage( const QString &msg )
{
  if (msg.isEmpty())
  {
    if (pageChangeIsConnected)
      emit setStatusBarText(QString::null);
    else
    {
      int currentPage = multiPage->currentPageNumber();
      int numberOfPages = multiPage->numberOfPages();
      emit setStatusBarText(i18n("Page %1 of %2").arg(currentPage).arg(numberOfPages));
    }
  }
  else
    emit setStatusBarText(msg);
}

KAboutData* KViewPart::createAboutData()
{
  return new KAboutData("kviewerpart", I18N_NOOP("Document Viewer Part"),
                        "0.6", I18N_NOOP(""),
                        KAboutData::License_GPL,
                        I18N_NOOP("Copyright (c) 2005 Wilfried Huss"));
}

void KViewPart::aboutKViewShell()
{
  if (aboutDialog == 0)
  {
    // Create Dialog
    aboutDialog = new KAboutDialog(mainWidget, "about_kviewshell");
    aboutDialog->setTitle(I18N_NOOP("KViewShell"));
    aboutDialog->setVersion("0.6");
    aboutDialog->setAuthor("Matthias Hoelzer-Kluepfel", QString::null, QString::null,
                           I18N_NOOP("Original Author"));

    aboutDialog->addContributor("Matthias Hoelzer-Kluepfel", "mhk@caldera.de", QString::null,
                                I18N_NOOP("Framework"));
    aboutDialog->addContributor("David Sweet", "dsweet@kde.org", "http://www.chaos.umd.edu/~dsweet",
                                I18N_NOOP("Former KGhostView Maintainer"));
    aboutDialog->addContributor("Mark Donohoe", QString::null, QString::null,
                                I18N_NOOP("KGhostView Author"));
    aboutDialog->addContributor("Markku Hihnala", QString::null, QString::null,
                                I18N_NOOP("Navigation widgets"));
    aboutDialog->addContributor("David Faure", QString::null, QString::null,
                                I18N_NOOP("Basis for shell"));
    aboutDialog->addContributor("Daniel Duley", QString::null, QString::null,
                                I18N_NOOP("Port to KParts"));
    aboutDialog->addContributor("Espen Sand", QString::null, QString::null,
                                I18N_NOOP("Dialog boxes"));
    aboutDialog->addContributor("Stefan Kebekus", "kebekus@kde.org", QString::null,
                                I18N_NOOP("DCOP-Interface, major improvements"));
    aboutDialog->addContributor("Wilfried Huss", "Wilfried.Huss@gmx.at", QString::null,
                                I18N_NOOP("Interface enhancements"));
  }
  aboutDialog->show();
}

void KViewPart::doSettings()
{
  if (KConfigDialog::showDialog("kviewshell_config"))
    return;

  KConfigDialog* configDialog = new KConfigDialog(mainWidget, "kviewshell_config", KVSPrefs::self());

  optionDialogGUIWidget_base* guiWidget = new optionDialogGUIWidget_base(mainWidget);
  configDialog->addPage(guiWidget, i18n("User Interface"), "view_choose");

  optionDialogAccessibilityWidget* accWidget = new optionDialogAccessibilityWidget(mainWidget);
  configDialog->addPage(accWidget, i18n("Accessibility"), "access");

  multiPage->addConfigDialogs(configDialog);

  connect(configDialog, SIGNAL(settingsChanged()), this, SLOT(preferencesChanged()));
  configDialog->show();
}

void KViewPart::preferencesChanged()
{
  multiPage->preferencesChanged();
}

void KViewPart::partActivateEvent( KParts::PartActivateEvent *ev )
{
  QApplication::sendEvent( multiPage, ev );
}


void KViewPart::guiActivateEvent( KParts::GUIActivateEvent *ev )
{
  QApplication::sendEvent( multiPage, ev );
}


void KViewPart::slotEnableMoveTool(bool enable)
{
  // Safety Check
  if (multiPage.isNull())
    return;

  multiPage->slotEnableMoveTool(enable);
}


KViewPartExtension::KViewPartExtension(KViewPart *parent)
  : KParts::BrowserExtension( parent, "KViewPartExtension")
{
}


// KMultiPage Interface
void KViewPart::mp_prevPage()
{
  multiPage->prevPage();
}

void KViewPart::mp_nextPage()
{
  multiPage->nextPage();
}

void KViewPart::mp_firstPage()
{
  multiPage->firstPage();
}

void KViewPart::mp_lastPage()
{
  multiPage->lastPage();
}


void KViewPart::mp_readUp()
{
  multiPage->readUp();
}

void KViewPart::mp_readDown()
{
  multiPage->readDown();
}


void KViewPart::mp_scrollUp()
{
  multiPage->scrollUp();
}

void KViewPart::mp_scrollDown()
{
  multiPage->scrollDown();
}

void KViewPart::mp_scrollLeft()
{
  multiPage->scrollLeft();
}

void KViewPart::mp_scrollRight()
{
  multiPage->scrollRight();
}

void KViewPart::mp_scrollUpPage()
{
  multiPage->scrollUpPage();
}

void KViewPart::mp_scrollDownPage()
{
  multiPage->scrollDownPage();
}

void KViewPart::mp_scrollLeftPage()
{
  multiPage->scrollLeftPage();
}

void KViewPart::mp_scrollRightPage()
{
  multiPage->scrollRightPage();
}


void KViewPart::mp_slotSave()
{
  multiPage->slotSave();
}

void KViewPart::mp_slotSave_defaultFilename()
{
  multiPage->slotSave_defaultFilename();
}


void KViewPart::mp_doGoBack()
{
  multiPage->doGoBack();
}

void KViewPart::mp_doGoForward()
{
  multiPage->doGoForward();
}


void KViewPart::mp_showFindTextDialog()
{
  multiPage->showFindTextDialog();
}

void KViewPart::mp_findNextText()
{
  multiPage->findNextText();
}

void KViewPart::mp_findPrevText()
{
  multiPage->findPrevText();
}

void KViewPart::mp_doSelectAll()
{
  multiPage->doSelectAll();
}

void KViewPart::mp_clearSelection()
{
  multiPage->clearSelection();
}

void KViewPart::mp_copyText()
{
  multiPage->copyText();
}

void KViewPart::mp_exportText()
{
  multiPage->doExportText();
}

#include "kviewpart.moc"

