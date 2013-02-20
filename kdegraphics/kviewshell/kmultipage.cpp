#include <config.h>

#include <kaction.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <kio/job.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprinter.h>
#include <kstdaction.h>
#include <qobject.h>
#include <qlayout.h>
#include <qpaintdevicemetrics.h>
#include <qprogressdialog.h>
#include <qsplitter.h>
#include <qurl.h>
#include <qtoolbox.h>
#include <qvbox.h>

#include "documentWidget.h"
#include "marklist.h"
#include "tableOfContents.h"
#include "kprintDialogPage_pageoptions.h"
#include "kvsprefs.h"
#include "kmultipage.h"
#include "pageNumber.h"
#include "renderedDocumentPagePrinter.h"
#include "searchWidget.h"
#include "textBox.h"
#include "zoomlimits.h"


//#define DEBUG_KMULTIPAGE

KMultiPage::KMultiPage(QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name)
  : DCOPObject("kmultipage"), KParts::ReadOnlyPart(parent, name)
{
  // For reasons which I don't understand, the initialization of the
  // DCOPObject above does not work properly, the name is ignored. It
  // works fine if we repeat the name here. -- Stefan Kebekus
  // This is because of the virtual inheritance. Get rid of it (but it's BC, and this is a lib...) -- DF
  setObjId("kmultipage");

  parentWdg = parentWidget;
  lastCurrentPage = 0;
  timer_id = -1;
  searchInProgress = false;
  
  QVBox* verticalBox = new QVBox(parentWidget);
  verticalBox->setFocusPolicy(QWidget::StrongFocus);
  setWidget(verticalBox);
  
  splitterWidget = new QSplitter(verticalBox, widgetName);
  splitterWidget->setOpaqueResize(false);
  splitterWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  
  // Create SideBar
  sideBar = new QToolBox(splitterWidget, "sidebar");
  
  // Create ContentsList
  tableOfContents = new TableOfContents(sideBar);
  sideBar->addItem(tableOfContents, QIconSet(SmallIcon("contents")), i18n("Contents"));

  connect(tableOfContents, SIGNAL(gotoPage(const Anchor&)), this, SLOT(gotoPage(const Anchor&)));
  
  // Create MarkList
  _markList = new MarkList(sideBar, "marklist");
  sideBar->addItem(_markList, QIconSet(SmallIcon("thumbnail")), i18n("Thumbnails"));

  // Restore state of the sidebar
  sideBar->setCurrentItem(sideBar->item(KVSPrefs::sideBarItem()));

  splitterWidget->setResizeMode(sideBar, QSplitter::KeepSize);

  connect(_markList, SIGNAL(selected(const PageNumber&)), this, SLOT(gotoPage(const PageNumber&)));

  _scrollView = new PageView(splitterWidget, widgetName);

  // Create Search Panel
  searchWidget = new SearchWidget(verticalBox);
  searchWidget->hide();
  connect(searchWidget, SIGNAL(findNextText()), this, SLOT(findNextText()));
  connect(searchWidget, SIGNAL(findPrevText()), this, SLOT(findPrevText()));

  sideBar->setMinimumWidth(80);
  sideBar->setMaximumWidth(300);

  connect(_scrollView, SIGNAL(currentPageChanged(const PageNumber&)), this, SLOT(setCurrentPageNumber(const PageNumber&)));
  connect(_scrollView, SIGNAL(viewSizeChanged(const QSize&)), scrollView(), SLOT(calculateCurrentPageNumber()));
  connect(_scrollView, SIGNAL(wheelEventReceived(QWheelEvent *)), this, SLOT(wheelEvent(QWheelEvent*)));

  connect(this, SIGNAL(enableMoveTool(bool)), _scrollView, SLOT(slotEnableMoveTool(bool)));

  splitterWidget->setCollapsible(sideBar, false);
  splitterWidget->setSizes(KVSPrefs::guiLayout());

  connect(searchWidget, SIGNAL(searchEnabled(bool)), this, SIGNAL(searchEnabled(bool)));
  connect(searchWidget, SIGNAL(stopSearch()), this, SLOT(stopSearch()));
}


KMultiPage::~KMultiPage()
{
  writeSettings();

  if (timer_id != -1)
    killTimer(timer_id);

  delete pageCache;
}

void KMultiPage::readSettings()
{
}

void KMultiPage::writeSettings()
{
  // Save TOC layout
  tableOfContents->writeSettings();

  KVSPrefs::setGuiLayout(splitterWidget->sizes());
  // Save state of the sidebar
  KVSPrefs::setSideBarItem(sideBar->indexOf(sideBar->currentItem()));
  KVSPrefs::writeConfig();
}

QString KMultiPage::name_of_current_file()
{
  return m_file;
}

bool KMultiPage::is_file_loaded(const QString& filename)
{
  return (filename == m_file);
}

void KMultiPage::slotSave_defaultFilename()
{
  slotSave();
}

void KMultiPage::slotSave()
{
  // Try to guess the proper ending...
  QString formats;
  QString ending;
  int rindex = m_file.findRev(".");
  if (rindex == -1) {
    ending = QString::null;
    formats = QString::null;
  } else {
    ending = m_file.mid(rindex); // e.g. ".dvi"
    formats = fileFormats().grep(ending).join("\n");
  }

  QString fileName = KFileDialog::getSaveFileName(QString::null, formats, 0, i18n("Save File As"));

  if (fileName.isEmpty())
    return;

  // Add the ending to the filename. I hope the user likes it that
  // way.
  if (!ending.isEmpty() && fileName.find(ending) == -1)
    fileName = fileName+ending;

  if (QFile(fileName).exists()) {
    int r = KMessageBox::warningContinueCancel (0, i18n("The file %1\nexists. Shall I overwrite that file?").arg(fileName),
				       i18n("Overwrite File"), i18n("Overwrite"));
    if (r == KMessageBox::Cancel)
      return;
  }

  KIO::Job *job = KIO::file_copy( KURL( m_file ), KURL( fileName ), 0600, true, false, true );
  connect( job, SIGNAL( result( KIO::Job * ) ), this, SLOT( slotIOJobFinished ( KIO::Job * ) ) );

  return;
}


void KMultiPage::setFile(bool)
{
  return;
}


bool KMultiPage::closeURL()
{
#ifdef DEBUG_KMULTIPAGE
  kdDebug(1233) << "KMultiPage::closeURL()" << endl;
#endif

  if (renderer.isNull())
    return false;
  
  // Clear navigation history.
  document_history.clear();
  
  // Close the file.
  renderer->setFile(QString::null, KURL());
  renderer->clear();
  
  // Delete Page Widgets.
  widgetList.setAutoDelete(true);
  widgetList.resize(0);
  widgetList.setAutoDelete(false);
  
  // Update ScrollView.
  scrollView()->layoutPages();
  enableActions(false);
  
  // Clear Thumbnail List.
  markList()->clear();
  
  // Clear Table of Contents
  tableOfContents->clear();
  
  // Clear Status Bar
  emit setStatusBarText(QString::null);

  return true;
}

void KMultiPage::slotIOJobFinished ( KIO::Job *job )
{
  if ( job->error() )
    job->showErrorDialog( 0L );
}

void KMultiPage::slotShowScrollbars(bool status)
{
  _scrollView->slotShowScrollbars(status);
}

void KMultiPage::slotShowSidebar(bool show)
{
  if (show)
    sideBar->show();
  else
    sideBar->hide();
}

void KMultiPage::slotShowThumbnails(bool show)
{
  markList()->slotShowThumbnails(show);
}

void KMultiPage::slotSetFullPage(bool fullpage)
{
  _scrollView->setFullScreenMode(fullpage);
  if (fullpage)
    slotShowSidebar(false);
}

void KMultiPage::preferencesChanged()
{
  // We need to read the config options otherwise the KVSPrefs-object would
  // not be syncronized between the kviewpart and the kmultipage.
  KVSPrefs::self()->readConfig();

  slotShowThumbnails(KVSPrefs::showThumbnails());

  // if we are in overviewmode and the number of columns or rows has changed
  if (scrollView()->overviewMode() &&
      (scrollView()->getNrColumns() != KVSPrefs::overviewModeColumns() ||
       scrollView()->getNrRows() != KVSPrefs::overviewModeRows()))
  {
    setViewMode(KVSPrefs::EnumViewMode::Overview);
  }

  if (KVSPrefs::changeColors() && KVSPrefs::renderMode() == KVSPrefs::EnumRenderMode::Paper)
    renderer->setAccessibleBackground(true, KVSPrefs::paperColor());
  else
    renderer->setAccessibleBackground(false);

  renderModeChanged();
}

void KMultiPage::setViewMode(int mode)
{
#ifdef DEBUG_KMULTIPAGE
  kdDebug(1233) << "KMultiPage::setViewMode(" << mode << ")" << endl;
#endif
  // Save the current page number because when we are changing the columns
  // and rows in the scrollview the currently shown Page probably out of view.
  PageNumber currentPage = currentPageNumber();

  // Save viewMode for future uses of KViewShell
  switch (mode) 
  {
    case KVSPrefs::EnumViewMode::SinglePage:
      KVSPrefs::setViewMode(KVSPrefs::EnumViewMode::SinglePage);

      // Don't do anything if the view mode is already set
      if ((scrollView()->getNrColumns() == 1) && (scrollView()->getNrRows() == 1) && (scrollView()->isContinuous() == false))
        return;
      
      scrollView()->setNrColumns(1);
      scrollView()->setNrRows(1);
      scrollView()->setContinuousViewMode(false);
      // We scroll the view to the top, so that top and not the bottom
      // of the visible page is shown.
      scrollView()->scrollTop();
      break;
    case KVSPrefs::EnumViewMode::ContinuousFacing:
      KVSPrefs::setViewMode(KVSPrefs::EnumViewMode::ContinuousFacing);

      // Don't do anything if the view mode is already set
      if ((scrollView()->getNrColumns() == 2) && (scrollView()->getNrRows() == 1) && (scrollView()->isContinuous() == true))
        return;

      scrollView()->setNrColumns(2);
      scrollView()->setNrRows(1);
      scrollView()->setContinuousViewMode(true);
      break;
    case KVSPrefs::EnumViewMode::Overview:
      KVSPrefs::setViewMode(KVSPrefs::EnumViewMode::Overview);

      // Don't do anything if the view mode is already set
      if ((scrollView()->getNrColumns() == KVSPrefs::overviewModeColumns()) && (scrollView()->getNrRows() == KVSPrefs::overviewModeRows()) && (scrollView()->isContinuous() == false))
        return;

      scrollView()->setNrColumns(KVSPrefs::overviewModeColumns());
      scrollView()->setNrRows(KVSPrefs::overviewModeRows());
      scrollView()->setContinuousViewMode(false);
      // We scroll the view to the top, so that top and not the bottom
      // of the visible tableau is shown.
      scrollView()->scrollTop();
      break;
    default:  //KVSPrefs::EnumViewMode::Continuous
      KVSPrefs::setViewMode(KVSPrefs::EnumViewMode::Continuous);

      // Don't do anything if the view mode is already set
      if ((scrollView()->getNrColumns() == 1) && (scrollView()->getNrRows() == 1) && (scrollView()->isContinuous() == true))
        return;
      
      scrollView()->setNrColumns(1);
      scrollView()->setNrRows(1);
      scrollView()->setContinuousViewMode(true);
  }
  generateDocumentWidgets(currentPage);
  KVSPrefs::writeConfig();
  emit viewModeChanged();
}

void KMultiPage::initializePageCache()
{
  pageCache = new DocumentPageCache();
}

DocumentWidget* KMultiPage::createDocumentWidget()
{
  DocumentWidget* documentWidget = new DocumentWidget(scrollView()->viewport(), scrollView(), pageCache, "singlePageWidget");
  connect(documentWidget, SIGNAL(clearSelection()), this, SLOT(clearSelection()));
  connect(this, SIGNAL(enableMoveTool(bool)), documentWidget, SLOT(slotEnableMoveTool(bool)));
  return documentWidget;
}


void KMultiPage::generateDocumentWidgets(const PageNumber& _startPage)
{
  PageNumber startPage = _startPage;
#ifdef DEBUG_KMULTIPAGE
  kdDebug(1233) << "KMultiPage::generateDocumentWidgets(" << startPage << ")" << endl;
#endif

  // Do nothing if no document is loaded.
  if (getRenderer().isNull() || getRenderer()->isEmpty())
    return;

  // This function is only called with an invalid pagenumber, when
  // the file has been loaded or reloaded.
  bool reload = !startPage.isValid();

  if (reload)
  {
    // Find the number of the current page, for later use.
    startPage = currentPageNumber();
  }

  // Make sure that startPage is in the permissible range.
  if (startPage < 1)
    startPage = 1;
  if (startPage > numberOfPages())
    startPage = numberOfPages();

  unsigned int tableauStartPage = startPage;

  // Find out how many widgets are needed, and resize the widgetList accordingly.
  widgetList.setAutoDelete(true);
  Q_UINT16 oldwidgetListSize = widgetList.size();
  if (numberOfPages() == 0)
    widgetList.resize(0);
  else
  {
    switch (KVSPrefs::viewMode())
    {
      case KVSPrefs::EnumViewMode::SinglePage:
        widgetList.resize(1);
        break;
      case KVSPrefs::EnumViewMode::Overview:
      {
        // Calculate the number of pages shown in overview mode.
        unsigned int visiblePages = KVSPrefs::overviewModeColumns() * KVSPrefs::overviewModeRows();
        // Calculate the number of the first page in the tableau.
        tableauStartPage = startPage - ((startPage - 1) % visiblePages);
        // We cannot have more widgets then pages in the document.
        visiblePages = QMIN(visiblePages, numberOfPages() - tableauStartPage + 1);
        if (widgetList.size() != visiblePages)
          widgetList.resize(visiblePages);
        break;
      }
      default:
        // In KVS_Continuous and KVS_ContinuousFacing all pages in the document are shown.
        widgetList.resize(numberOfPages());
    }
  }
  bool isWidgetListResized = (widgetList.size() != oldwidgetListSize);
  widgetList.setAutoDelete(false);

  // If the widgetList is empty, there is nothing left to do.
  if (widgetList.size() == 0) {
    scrollView()->addChild(&widgetList);
    return;
  }

  // Allocate DocumentWidget structures so that all entries of
  // widgetList point to a valid DocumentWidget.
  DocumentWidget *documentWidget;
  for(Q_UINT16 i=0; i<widgetList.size(); i++) {
    documentWidget = widgetList[i];
    if (documentWidget == 0) {
      documentWidget = createDocumentWidget();

      widgetList.insert(i, documentWidget);
      documentWidget->show();

      connect(documentWidget, SIGNAL(localLink(const QString &)), this, SLOT(handleLocalLink(const QString &)));
      connect(documentWidget, SIGNAL(setStatusBarText(const QString&)), this, SIGNAL(setStatusBarText(const QString&)) );
    }
  }

  // Set the page numbers for the newly allocated widgets. How this is
  // done depends on the viewMode.
  if (KVSPrefs::viewMode() == KVSPrefs::EnumViewMode::SinglePage) {
    // In KVS_SinglePage mode, any number between 1 and the maximum
    // number of pages is acceptable. If an acceptable value is found,
    // nothing is done, and otherwise '1' is set as a default.
    documentWidget = widgetList[0];
    if (documentWidget != 0) { // Paranoia safety check
      documentWidget->setPageNumber(startPage);
      documentWidget->update();
    } else
      kdError(4300) << "Zero-Pointer in widgetList in KMultiPage::generateDocumentWidgets()" << endl;
  } else {
    // In all other modes, the widgets will be numbered continuously,
    // starting from firstShownPage.
    for(Q_UINT16 i=0; i<widgetList.size(); i++) {
      documentWidget = widgetList[i];
      if (documentWidget != 0) // Paranoia safety check
      {
        if (KVSPrefs::viewMode() == KVSPrefs::EnumViewMode::Overview)
          documentWidget->setPageNumber(i+tableauStartPage);
        else
          documentWidget->setPageNumber(i+1);
      }
      else
        kdError(4300) << "Zero-Pointer in widgetList in KMultiPage::generateDocumentWidgets()" << endl;
    }
  }

  // Make the changes in the widgetList known to the scrollview. so
  // that the scrollview may update its contents.
  scrollView()->addChild(&widgetList);

  // If the number of widgets has changed, or the viewmode has been changed the widget 
  // that displays the current page may not be visible anymore. Bring it back into focus.
  if (isWidgetListResized || !reload)
    gotoPage(startPage);
}


bool KMultiPage::gotoPage(const PageNumber& page)
{
  return gotoPage(page, 0, true);
}


bool KMultiPage::gotoPage(const PageNumber& page, int y, bool isLink)
{
#ifdef DEBUG_KMULTIPAGE
  kdDebug(1233) << "KMultiPage::gotoPage()" << endl;
#endif

  if (widgetList.size() == 0) {
    kdError(4300) << "KMultiPage::gotoPage(" << page << ", y) called, but widgetList is empty" << endl;
    return false;
  }

  if (!page.isValid())
  {
    kdDebug(1223) << "KMultiPage::gotoPage(" << page << ") invalid pageNumber." << endl;
    return false;
  }

  if (isLink)
    document_history.add(page, y);

  DocumentWidget* pageWidget;

  // If we are in overview viewmode
  if (KVSPrefs::viewMode() == KVSPrefs::EnumViewMode::Overview)
  {
    unsigned int visiblePages = KVSPrefs::overviewModeColumns() * KVSPrefs::overviewModeRows();
    // Pagenumber of the first visibile Page in the current tableau
    unsigned int firstPage = ((DocumentWidget*)widgetList[0])->getPageNumber();
    // Pagenumber of the first page in the new tableau.
    unsigned int tableauStartPage = page + 1 - (page % visiblePages);
    // If these numbers arn't equal "page" is not in the current tableu.
    if (firstPage != tableauStartPage) // widgets need to be updated
    {
      if ((numberOfPages() - tableauStartPage + 1 < visiblePages) || (widgetList.size() < visiblePages))
      {
        // resize widgetList
        // the pages are also set correctly by "generateDocumentWidgets"
        generateDocumentWidgets(tableauStartPage);
      }
      else
      {
        // "page" is not shown in the scrollview, so we have to switch widgets.
        // Here we don't need to resize the widgetList.
        for (unsigned int i = 0; i < widgetList.size(); i++)
        {
          pageWidget = (DocumentWidget*)(widgetList[i]);
          if (pageWidget != 0)
            pageWidget->setPageNumber(tableauStartPage + i);
        }
        scrollView()->layoutPages();
      }
    }
    // move scrollview to "page".
    // Make the widget pageWidget visible in the scrollview. Somehow this
    // doesn't seem to trigger the signal contentsMoved in the
    // QScrollview, so that we better call setCurrentPage() ourselves.
    pageWidget = (DocumentWidget*)(widgetList[page % visiblePages]);

    scrollView()->moveViewportToWidget(pageWidget, y);

    // Set current page number.
    setCurrentPageNumber(page);

    return true;
  }
  else if (widgetList.size() == 1)
  {
    // If the widget list contains only a single element, then either
    // the document contains only one page, or we are in "single page"
    // view mode. In either case, we set the page number of the single
    // widget to 'page'
    pageWidget = (DocumentWidget*)(widgetList[0]);

    // Paranoia security check
    if (pageWidget == 0) {
      kdError(4300) << "KMultiPage::goto_Page() called with widgetList.size() == 1, but widgetList[0] == 0" << endl;
      return false;
    }

    if (pageCache->sizeOfPageInPixel(currentPageNumber()) == pageCache->sizeOfPageInPixel(page))
    {
      // We are rendering the page before we switch the widget to the new page.
      // To make a smooth transition. We only do this if the size of the current and new page are equal,
      // otherwise we would have to render the page twice, if autozoom is enabled.
      pageCache->getPage(page);
    }

    pageWidget->setPageNumber(page);
    scrollView()->layoutPages();
    scrollView()->moveViewportToWidget(pageWidget, y);
  } else {
    // There are multiple widgets, then we are either in the
    // "Continuous" or in the "Continouous-Facing" view mode. In that
    // case, we find the widget which is supposed to display page
    // 'page' and move the scrollview to make it visible

    // Paranoia security checks
    if (widgetList.size() < page) {
      kdError(4300) << "KMultiPage::goto_Page(page,y ) called with widgetList.size()=" << widgetList.size() << ", and page=" << page << endl;
      return false;
    }
    pageWidget = (DocumentWidget*)(widgetList[page-1]);
    if (pageWidget == 0) {
      kdError(4300) << "KMultiPage::goto_Page() called with widgetList.size() > 1, but widgetList[page] == 0" << endl;
      return false;
    }

    scrollView()->moveViewportToWidget(pageWidget, y);
  }

  if (isLink && y != 0)
    pageWidget->flash(y);

  // Set current page number.
  setCurrentPageNumber(page);
  return true;
}


void KMultiPage::handleLocalLink(const QString &linkText)
{
#ifdef DEBUG_SPECIAL
  kdDebug(4300) << "hit: local link to " << linkText << endl;
#endif

  if (renderer.isNull()) {
    kdError(4300) << "KMultiPage::handleLocalLink( " << linkText << " ) called, but renderer==0" << endl;
    return;
  }

  QString locallink;
  if (linkText[0] == '#' )
    locallink = linkText.mid(1); // Drop the '#' at the beginning
  else
    locallink = linkText;

  Anchor anch = renderer->findAnchor(locallink);

  if (anch.isValid())
    gotoPage(anch);
  else {
    if (linkText[0] != '#' ) {
      // We could in principle use KIO::Netaccess::run() here, but
      // it is perhaps not a very good idea to allow a DVI-file to
      // specify arbitrary commands, such as "rm -rvf /". Using
      // the kfmclient seems to be MUCH safer.
      QUrl DVI_Url(m_file);
      QUrl Link_Url(DVI_Url, linkText, true);

      QStringList args;
      args << "openURL";
      args << Link_Url.toString();
      kapp->kdeinitExec("kfmclient", args);
    }
  }
}

void KMultiPage::setCurrentPageNumber(const PageNumber& page)
{
#ifdef DEBUG_KMULTIPAGE
  kdDebug(1233) << "KMultiPage::setCurrentPageNumber()" << endl;
#endif

  if (page != currentPageNumber())
  {
    markList()->setCurrentPageNumber(page);
    emit pageInfo(numberOfPages(), currentPageNumber());
  }
}

PageNumber KMultiPage::currentPageNumber()
{
  return markList()->currentPageNumber();
}

void KMultiPage::doGoBack()
{
  HistoryItem *it = document_history.back();
  if (it != 0)
    gotoPage(it->page, it->ypos, false); // Do not add a history item.
  else
    kdDebug(4300) << "Faulty return -- bad history buffer" << endl;
  return;
}


void KMultiPage::doGoForward()
{
  HistoryItem *it = document_history.forward();
  if (it != 0)
    gotoPage(it->page, it->ypos, false); // Do not add a history item.
  else
    kdDebug(4300) << "Faulty return -- bad history buffer" << endl;
  return;
}


void KMultiPage::renderModeChanged()
{
  pageCache->clear();

  generateDocumentWidgets();
  scrollView()->layoutPages();

  for (Q_UINT16 i=0; i < widgetList.size(); i++)
  {
    DocumentWidget* documentWidget = widgetList[i];
    if (documentWidget == 0)
      continue;

    documentWidget->update();
  }

  markList()->repaintThumbnails();
}


void KMultiPage::repaintAllVisibleWidgets()
{
#ifdef DEBUG_KMULTIPAGE
  kdDebug(1233) << "KMultiPage::repaintAllVisibleWidgets()" << endl;
#endif

  bool everResized = false;

  // Go through the list of widgets and resize them, if necessary
  for(Q_UINT16 i=0; i<widgetList.size(); i++)
  {
    DocumentWidget* documentWidget = widgetList[i];
    if (documentWidget == 0)
      continue;

    // Resize, if necessary
    QSize pageSize = pageCache->sizeOfPageInPixel(documentWidget->getPageNumber());
    if (pageSize != documentWidget->pageSize())
    {
      documentWidget->setPageSize(pageSize);
      everResized = true;
    }
  }

  // If at least one widget was resized, all widgets should be
  // re-aligned. This will automatically update all necessary
  // widgets.
  if (everResized == true)
    scrollView()->layoutPages(true);
}


double KMultiPage::setZoom(double zoom)
{
#ifdef DEBUG_KMULTIPAGE
  kdDebug(1233) << "KMultiPage::setZoom(" << zoom << ")" << endl;
#endif

  if (zoom < ZoomLimits::MinZoom/1000.0)
    zoom = ZoomLimits::MinZoom/1000.0;
  if (zoom > ZoomLimits::MaxZoom/1000.0)
    zoom = ZoomLimits::MaxZoom/1000.0;

  pageCache->setResolution(QPaintDevice::x11AppDpiX()*zoom);
  emit zoomChanged();
  return zoom;
}


void KMultiPage::print()
{
  // Paranoid safety checks
  if (renderer.isNull())
    return;
  if (renderer->isEmpty())
    return;

  // Allocate the printer structure
  KPrinter *printer = getPrinter();
  if (printer == 0)
    return;

  // initialize the printer using the print dialog
  if ( printer->setup(parentWdg, i18n("Print %1").arg(m_file.section('/', -1))) ) {    
    // Now do the printing. 
    QValueList<int> pageList = printer->pageList();
    if (pageList.isEmpty()) 
      printer->abort();
    else {
      printer->setCreator("kviewshell");
      printer->setDocName(m_file);
      RenderedDocumentPagePrinter rdpp(printer);
      
      // Obtain papersize information that is required to perform
      // the resizing and centering, if this is wanted by the user.
      Length paperWidth, paperHeight;
      QPaintDeviceMetrics pdm(printer);
      paperWidth.setLength_in_mm(pdm.widthMM());
      paperHeight.setLength_in_mm(pdm.heightMM());
      
      QValueList<int>::ConstIterator it = pageList.begin();
      while (true) {
	SimplePageSize paper_s(paperWidth, paperHeight);

	// Printing usually takes a while. This is to keep the GUI
	// updated.
	qApp->processEvents();
	
	QPainter *paint = rdpp.getPainter();
	if (paint != 0) {
	  // Before drawing the page, we figure out the zoom-value,
	  // taking the "page sizes and placement" options from the
	  // printer dialog into account
	  double factual_zoom = 1.0;
	  
	  // Obtain pagesize information that is required to perform the
	  // resizing and centering, if this is wanted by the user.
	  SimplePageSize page_s = sizeOfPage(*it);
	  
	  paint->save();

	  // Rotate the page, if appropriate. By default, page
	  // rotation is enabled. This is also hardcoded into
	  // KPrintDialogPage_PageOptions.cpp
	  if ((page_s.isPortrait() != paper_s.isPortrait()) && (printer->option( "kde-kviewshell-rotatepage" ) != "false")) {
	    paint->rotate(-90);
	    paint->translate(-printer->resolution()*paperHeight.getLength_in_inch(), 0.0);
	    paper_s = paper_s.rotate90();
	  }

	  double suggested_zoom = page_s.zoomToFitInto(paper_s);
	  
	  // By default, "shrink page" and "expand page" are off. This
	  // is also hardcoded into KPrintDialogPage_PageOptions.cpp
	  if ((suggested_zoom < 1.0) && (printer->option( "kde-kviewshell-shrinkpage" ) == "true")) 
	    factual_zoom = suggested_zoom;
	  if ((suggested_zoom > 1.0) && (printer->option( "kde-kviewshell-expandpage" ) == "true")) 
	    factual_zoom = suggested_zoom;
	  
	  Length delX, delY;
	  // By default, "center page" is on. This is also hardcoded
	  // into KPrintDialogPage_PageOptions.cpp
	  if (printer->option( "kde-kviewshell-centerpage" ) != "false") {
	    delX = (paper_s.width() - page_s.width()*factual_zoom)/2.0;
	    delY = (paper_s.height() - page_s.height()*factual_zoom)/2.0;
	  }
	  
	  // Now draw the page.
	  rdpp.setPageNumber(*it);
	  
	  double resolution = factual_zoom*printer->resolution();
	  
	  paint->translate(resolution*delX.getLength_in_inch(), resolution*delY.getLength_in_inch());
	  renderer->drawPage(resolution, &rdpp);
	  paint->restore();
	}
	++it;
	if ((it == pageList.end()) || (printer->aborted() == true))
	  break;
	
	printer->newPage();
      }
      // At this point the rdpp is destructed. The last page is then
      // printed.
    }
  }
  delete printer;
}


void KMultiPage::setRenderer(DocumentRenderer* _renderer)
{
  renderer = _renderer;

  // Initialize documentPageCache.
  initializePageCache();
  pageCache->setRenderer(renderer);

  _markList->setPageCache(pageCache);

  // Clear widget list.
  widgetList.resize(0);

  // Relay signals.
  connect(renderer, SIGNAL(setStatusBarText(const QString&)), this, SIGNAL(setStatusBarText(const QString&)));
  connect(pageCache, SIGNAL(paperSizeChanged()), this, SLOT(renderModeChanged()));
  connect(pageCache, SIGNAL(textSelected(bool)), this, SIGNAL(textSelected(bool)));
  connect(renderer, SIGNAL(documentIsChanged()), this, SLOT(renderModeChanged()));
  connect(this, SIGNAL(zoomChanged()), this, SLOT(repaintAllVisibleWidgets()));
}


void KMultiPage::updateWidgetSize(const PageNumber& pageNumber)
{
  for(Q_UINT16 i=0; i<widgetList.size(); i++)
  {
    DocumentWidget* documentWidget = widgetList[i];
    if (documentWidget == 0)
      continue;

    if (documentWidget->getPageNumber() == pageNumber)
    {
      // Resize, if necessary
      QSize pageSize = pageCache->sizeOfPageInPixel(documentWidget->getPageNumber());
      if (pageSize != documentWidget->pageSize())
      {
        documentWidget->setPageSize(pageSize);
        scrollView()->layoutPages();
      }
      // We have just one widget per page.
      break;
    }
  }

  // Update marklist
  markList()->updateWidgetSize(pageNumber);
}


PageNumber KMultiPage::widestPage() const
{
  Length maxWidth;
  PageNumber pageNumber = 1;

  for (int i = 1; i <= numberOfPages(); i++)
  {
    Length width = pageCache->sizeOfPage(i).width();

    if (width > maxWidth)
    {
      maxWidth = width;
      pageNumber = i;
    }
  }

  return pageNumber;
}

double KMultiPage::zoomForWidthColumns(unsigned int viewportWidth) const
{
  Length maxLeftColumnWidth;
  Length maxRightColumnWidth;
  Length maxWidth;

  PageNumber widestPageLeft;
  PageNumber widestPageRight;

  for (int i = 1; i <= numberOfPages(); i++)
  {
    Length width = pageCache->sizeOfPage(i).width();

    if ( i % 2 == 0) // page is in left column
    {
      if (width > maxLeftColumnWidth)
      {
        maxLeftColumnWidth = width;
        widestPageLeft = i;
      }
    }

    if ( i % 2 == 1) // page is in right column
    {
      if (width > maxRightColumnWidth)
        maxRightColumnWidth = width;
        widestPageRight = i;
    }
  }

  double ratio =  maxLeftColumnWidth / (maxLeftColumnWidth + maxRightColumnWidth);

  // This number is the amount of space the left column should occupy in the viewport.
  unsigned int leftTargetWidth = (unsigned int)(ratio * viewportWidth);

  return pageCache->sizeOfPage(widestPageLeft).zoomForWidth(leftTargetWidth);
}

double KMultiPage::calculateFitToHeightZoomValue()
{
  PageNumber pageNumber = 1;

  // See below, in the documentation of the method "calculatefitToWidthZoomLevel"
  // for an explanation of the complicated calculation we are doing here.
  int columns = scrollView()->getNrColumns();
  int rows = scrollView()->getNrRows();
  int continuousViewmode = scrollView()->isContinuous();
  bool fullScreenMode = scrollView()->fullScreenMode();

  if (columns == 1 && rows == 1 && !continuousViewmode) // single page mode
  {
    pageNumber = currentPageNumber();
    if (!pageNumber.isValid())
      pageNumber = 1;
  }

  int pageDistance = scrollView()->distanceBetweenPages();
  if (columns == 1 && rows == 1 && !continuousViewmode && fullScreenMode)
  {
    // In Single Page Fullscreen Mode we want to fit the page to the
    // window without a margin around it.
    pageDistance = 0;
  }

  int targetViewportHeight = scrollView()->viewportSize(0,0).height();
  int targetPageHeight = (targetViewportHeight - rows*pageDistance)/rows;
  int targetPageWidth  = (int)(targetPageHeight * pageCache->sizeOfPage(pageNumber).aspectRatio() );
  int targetViewportWidth = targetPageWidth * columns + (columns+1)*pageDistance;
  targetViewportHeight = scrollView()->viewportSize(targetViewportWidth, targetViewportHeight).height();
  targetPageHeight = (targetViewportHeight - rows*pageDistance)/rows;

  return pageCache->sizeOfPage(pageNumber).zoomForHeight(targetPageHeight);
}


double KMultiPage::calculateFitToWidthZoomValue()
{
  PageNumber pageNumber = 1;

  int columns = scrollView()->getNrColumns();
  int rows = scrollView()->getNrRows();
  int continuousViewmode = scrollView()->isContinuous();
  bool fullScreenMode = scrollView()->fullScreenMode();

  if (columns == 1 && rows == 1 && !continuousViewmode) // single page mode
  {
    // To calculate the zoom level in single page mode we need the size
    // of the current page. When a new document is opened this function
    // is called while the currentPageNumber is invalid. We use the size
    // of the first page of the document in this case.
    pageNumber = currentPageNumber();
    if (!pageNumber.isValid())
      pageNumber = 1;
  }

  if (columns == 1 && rows == 1 && continuousViewmode) // continuous viewmode
  {
    pageNumber = widestPage();
    if (!pageNumber.isValid())
      pageNumber = 1;
  }

  // rows should be 1 for Single Page Viewmode,
  // the number of Pages in Continuous Viewmode
  // and number of Pages/2 in Continuous-Facing Viewmode
  if (continuousViewmode)
    rows = (int)(ceil(numberOfPages() / (double)columns));

  int pageDistance = scrollView()->distanceBetweenPages();
  if (columns == 1 && rows == 1 && !continuousViewmode && fullScreenMode)
  {
    // In Single Page Fullscreen Mode we want to fit the page to the
    // window without a margin around it.
    pageDistance = 0;
  }
  // There is a slight complication here... if we just take the width
  // of the viewport and scale the contents by a factor x so that it
  // fits the viewport exactly, then, depending on chosen papersize
  // (landscape, etc.), the contents may be higher than the viewport
  // and the QScrollview may or may not insert a scrollbar at the
  // right. If the scrollbar appears, then the usable width of the
  // viewport becomes smaller, and scaling by x does not really fit
  // the (now smaller page) anymore.

  // Calculate the width and height of the view, disregarding the
  // possible complications with scrollbars, e.g. assuming the maximal
  // space is available.

  // width of the widget excluding possible scrollbars
  int targetViewportWidth  = scrollView()->viewportSize(0,0).width();

  // maximal width of a single page
  int targetPageWidth = (targetViewportWidth - (columns+1) * pageDistance) / columns;

  // maximal height of a single page
  int targetPageHeight = (int)(targetPageWidth/pageCache->sizeOfPage(pageNumber).aspectRatio());
  // FIXME: this is only correct if all pages in the document have the same height
  int targetViewportHeight = rows * targetPageHeight + (rows+1) * pageDistance;

  // Think again, this time use only the area which is really
  // acessible (which, in case that targetWidth targetHeight don't fit
  // the viewport, is really smaller because of the scrollbars).
  targetViewportWidth = scrollView()->viewportSize(targetViewportWidth, targetViewportHeight).width();

  if (columns == 2 && continuousViewmode) // continuous facing
  {
    // TODO Generalize this for more than 2 columns
    return zoomForWidthColumns(targetViewportWidth - (columns+1) * pageDistance);
  }

  // maximal width of a single page (now the scrollbars are taken into account)
  targetPageWidth = (targetViewportWidth - (columns+1) * pageDistance) / columns;

  return pageCache->sizeOfPage(pageNumber).zoomForWidth(targetPageWidth);
}


void KMultiPage::prevPage()
{
  Q_UINT8 cols = scrollView()->getNrColumns();
  Q_UINT8 rows = scrollView()->getNrRows();

  PageNumber np = 1;
  if (cols*rows < currentPageNumber())
  {
    np = currentPageNumber() - cols*rows;
  }

  gotoPage(np);
}


void KMultiPage::nextPage()
{
  Q_UINT8 cols = scrollView()->getNrColumns();
  Q_UINT8 rows = scrollView()->getNrRows();

  PageNumber np = QMIN(currentPageNumber() + cols*rows, (Q_UINT16)numberOfPages());

  gotoPage(np);
}


void KMultiPage::firstPage()
{
  gotoPage(1);
}


void KMultiPage::lastPage()
{
  gotoPage(numberOfPages());
}


void KMultiPage::scroll(Q_INT32 deltaInPixel)
{
  QScrollBar* scrollBar = scrollView()->verticalScrollBar();
  if (scrollBar == 0) {
    kdError(4300) << "KMultiPage::scroll called without scrollBar" << endl;
    return;
  }

  if (deltaInPixel < 0) {
    if (scrollBar->value() == scrollBar->minValue()) {
      if ( (currentPageNumber() == 1) || (changePageDelayTimer.isActive()) )
        return;

      if (scrollView()->isContinuous())
        return;

      changePageDelayTimer.stop();
      prevPage();

      scrollView()->setContentsPos(scrollView()->contentsX(), scrollBar->maxValue());
      return;
    }
  }

  if (deltaInPixel > 0) {
    if (scrollBar->value() == scrollBar->maxValue()) {
      if ( (currentPageNumber() == numberOfPages()) || (changePageDelayTimer.isActive()) )
        return;

      if (scrollView()->isContinuous())
        return;

      changePageDelayTimer.stop();
      nextPage();

      scrollView()->setContentsPos(scrollView()->contentsX(), 0);
      return;
    }
  }

  scrollBar->setValue(scrollBar->value() + deltaInPixel);

  if ( (scrollBar->value() == scrollBar->maxValue()) || (scrollBar->value() == scrollBar->minValue()) )
    changePageDelayTimer.start(200,true);
  else
    changePageDelayTimer.stop();
}


void KMultiPage::scrollUp()
{
  QScrollBar* scrollBar = scrollView()->verticalScrollBar();
  if (scrollBar == 0)
    return;

  scroll(-scrollBar->lineStep());
}


void KMultiPage::scrollDown()
{
  QScrollBar* scrollBar = scrollView()->verticalScrollBar();
  if (scrollBar == 0)
    return;

  scroll(scrollBar->lineStep());
}

void KMultiPage::scrollLeft()
{
  QScrollBar* scrollBar = scrollView()->horizontalScrollBar();
  if (scrollBar)
    scrollBar->subtractLine();
}


void KMultiPage::scrollRight()
{
  QScrollBar* scrollBar = scrollView()->horizontalScrollBar();
  if (scrollBar)
    scrollBar->addLine();
}


void KMultiPage::scrollUpPage()
{
  QScrollBar* scrollBar = scrollView()->verticalScrollBar();
  if (scrollBar)
    scrollBar->subtractPage();
}


void KMultiPage::scrollDownPage()
{
  QScrollBar* scrollBar = scrollView()->verticalScrollBar();
  if (scrollBar)
    scrollBar->addPage();
}


void KMultiPage::scrollLeftPage()
{
  QScrollBar* scrollBar = scrollView()->horizontalScrollBar();
  if (scrollBar)
    scrollBar->subtractPage();
}


void KMultiPage::scrollRightPage()
{
  QScrollBar* scrollBar = scrollView()->horizontalScrollBar();
  if (scrollBar)
    scrollBar->addPage();
}


void KMultiPage::readDown()
{
  PageView* sv = scrollView();

  if (sv->atBottom())
  {
    if (sv->isContinuous())
      return;

    if (currentPageNumber() == numberOfPages())
      return;

    nextPage();
    sv->setContentsPos(sv->contentsX(), 0);
  }
  else
    sv->readDown();
}


void KMultiPage::readUp()
{
  PageView* sv = scrollView();

  if (sv->atTop())
  {
    if (sv->isContinuous())
      return;

    if (currentPageNumber() == 1)
      return;

    prevPage();
    sv->setContentsPos(sv->contentsX(),  sv->contentsHeight());
  }
  else
    sv->readUp();
}


void KMultiPage::jumpToReference(const QString& reference)
{
  if (renderer.isNull())
    return;
  
  gotoPage(renderer->parseReference(reference));
}


void KMultiPage::gotoPage(const Anchor &a)
{
  if (!a.page.isValid() || (renderer.isNull()))
    return;

  gotoPage(a.page, (int)(a.distance_from_top.getLength_in_inch()*pageCache->getResolution() + 0.5), true);
}


void KMultiPage::gotoPage(const TextSelection& selection)
{
  if (selection.isEmpty())
  {
    kdError(4300) << "KMultiPage::gotoPage(...) called with empty TextSelection." << endl;
    return;
  }

  RenderedDocumentPage* pageData = pageCache->getPage(selection.getPageNumber());

  if (pageData == 0) {
#ifdef DEBUG_DOCUMENTWIDGET
    kdDebug(4300) << "DocumentWidget::paintEvent: no documentPage generated" << endl;
#endif
    return;
  }

  switch (widgetList.size())
  {
    case 0:
      kdError(4300) << "KMultiPage::select() while widgetList is empty" << endl;
      break;
    case 1:
      ((DocumentWidget*)widgetList[0])->select(selection);
      break;
    default:
      if (widgetList.size() < currentPageNumber())
        kdError(4300) << "KMultiPage::select() while widgetList.size()=" << widgetList.size() << "and currentPageNumber()=" << currentPageNumber() << endl;
      else
        ((DocumentWidget*)widgetList[selection.getPageNumber() - 1])->select(selection);
  }

  unsigned int y = pageData->textBoxList[selection.getSelectedTextStart()].box.top();
  gotoPage(selection.getPageNumber(), y, false);
}


void KMultiPage::doSelectAll()
{
  switch( widgetList.size() ) {
  case 0:
    kdError(4300) << "KMultiPage::doSelectAll() while widgetList is empty" << endl;
    break;
  case 1:
    ((DocumentWidget *)widgetList[0])->selectAll();
    break;
  default:
    if (widgetList.size() < currentPageNumber())
      kdError(4300) << "KMultiPage::doSelectAll() while widgetList.size()=" << widgetList.size() << "and currentPageNumber()=" << currentPageNumber() << endl;
    else
      ((DocumentWidget *)widgetList[currentPageNumber()-1])->selectAll();
  }
}



void  KMultiPage::showFindTextDialog()
{
  if ((renderer.isNull()) || (renderer->supportsTextSearch() == false))
    return;

  searchWidget->show();
  searchWidget->setFocus();
}

void KMultiPage::stopSearch()
{
  if (searchInProgress)
  {
    // stop the search
    searchInProgress = false;
  }
  else
    searchWidget->hide();
}

void KMultiPage::findNextText()
{
#ifdef KDVI_MULTIPAGE_DEBUG
  kdDebug(4300) << "KMultiPage::findNextText() called" << endl;
#endif

  searchInProgress = true;

  // Used to remember if the documentPage we use is from the cache.
  // If not we need to delete it manually to avoid a memory leak.
  bool cachedPage = false;

  QString searchText = searchWidget->getText();

  if (searchText.isEmpty())
  {
    kdError(4300) << "KMultiPage::findNextText() called when search text was empty" << endl;
    return;
  }

  bool case_sensitive = searchWidget->caseSensitive();

  // Find the page and text position on the page where the search will
  // start. If nothing is selected, we start at the beginning of the
  // current page. Otherwise, start after the selected text.  TODO:
  // Optimize this to get a better 'user feeling'
  Q_UINT16 startingPage;
  Q_UINT16 startingTextItem;

  TextSelection userSelection = pageCache->selectedText();
  if (userSelection.isEmpty())
  {
    startingPage     = currentPageNumber();
    startingTextItem = 0;
  }
  else
  {
    startingPage     = userSelection.getPageNumber();
    startingTextItem = userSelection.getSelectedTextEnd()+1;
  }

  TextSelection foundSelection;

  RenderedDocumentPagePixmap* searchPage = 0;

  for(unsigned int i = 0; i < numberOfPages(); i++)
  {
    unsigned int pageNumber = (i + startingPage - 1) % numberOfPages() + 1;

    if (!searchInProgress)
    {
      // Interrupt the search
      setStatusBarText(i18n("Search interrupted"));
      if (!cachedPage)
        delete searchPage;
      return;
    }

    if (i != 0)
    {
      setStatusBarText(i18n("Search page %1 of %2").arg(pageNumber).arg(numberOfPages()));
      kapp->processEvents();
    }

    // Check if we already have a rendered version of the page in the cache. As we are only interested in the
    // text we don't care about the page size.
    if (pageCache->isPageCached(pageNumber))
    {
      // If the last search page used was created locally, we need to destroy it
      if (!cachedPage)
        delete searchPage;

      searchPage = pageCache->getPage(pageNumber);
      cachedPage = true;
    }
    else
    {
      // If the page is not in the cache we draw a small version of it, since this is faster.

      // We only create a new searchPage if we need to, otherwise reuse the existing one.
      if (!searchPage || cachedPage)
        searchPage = new RenderedDocumentPagePixmap();

      cachedPage = false;

      searchPage->resize(1,1);
      searchPage->setPageNumber(pageNumber);
      renderer->getText(searchPage);
    }

    // If there is no text in the current page, try the next one.
    if (searchPage->textBoxList.size() == 0)
      continue;

    foundSelection = searchPage->find(searchText, startingTextItem, case_sensitive);

    if (foundSelection.isEmpty())
    {
      // In the next page, start search again at the beginning.
      startingTextItem = 0;
      clearSelection();

      if (pageNumber == numberOfPages())
      {
        int answ = KMessageBox::questionYesNo(scrollView(),
                   i18n("<qt>The search string <strong>%1</strong> could not be found by the "
                        "end of the document. Should the search be restarted from the beginning "
                        "of the document?</qt>").arg(searchText),
                   i18n("Text Not Found"), KStdGuiItem::cont(), KStdGuiItem::cancel());

        if (answ != KMessageBox::Yes)
        {
          setStatusBarText(QString::null);
          searchInProgress = false;
          if (!cachedPage)
            delete searchPage;
          return;
        }
      }
    }
    else
    {
      pageCache->selectText(foundSelection);
      gotoPage(pageCache->selectedText());
      setStatusBarText(QString::null);
      searchInProgress = false;
      if (!cachedPage)
        delete searchPage;
      return;
    }
  }

  KMessageBox::sorry(scrollView(), i18n("<qt>The search string <strong>%1</strong> could not be found.</qt>").arg(searchText));
  setStatusBarText(QString::null);
  searchInProgress = false;
  if (!cachedPage)
    delete searchPage;
}


void KMultiPage::findPrevText()
{
#ifdef KDVI_MULTIPAGE_DEBUG
  kdDebug(4300) << "KMultiPage::findPrevText() called" << endl;
#endif

  searchInProgress = true;

  // Used to remember if the documentPage we use is from the cache.
  // If not we need to delete it manually to avoid a memory leak.
  bool cachedPage = false;

  QString searchText = searchWidget->getText();

  if (searchText.isEmpty())
  {
    kdError(4300) << "KMultiPage::findPrevText() called when search text was empty" << endl;
    return;
  }

  bool case_sensitive = searchWidget->caseSensitive();

  // Find the page and text position on the page where the search will
  // start. If nothing is selected, we start at the beginning of the
  // current page. Otherwise, start after the selected text.  TODO:
  // Optimize this to get a better 'user feeling'
  unsigned int startingPage;
  int startingTextItem;

  TextSelection userSelection = pageCache->selectedText();
  if (userSelection.isEmpty())
  {
    startingPage     = currentPageNumber();
    startingTextItem = -1;
  }
  else
  {
    startingPage     = userSelection.getPageNumber();
    startingTextItem = userSelection.getSelectedTextStart()-1;
  }

  TextSelection foundSelection;

  RenderedDocumentPagePixmap* searchPage = 0;

  for(unsigned int i = 0; i < numberOfPages(); i++)
  {
    int pageNumber = startingPage - i;
    if (pageNumber <= 0)
      pageNumber += numberOfPages();

    if (!searchInProgress)
    {
      // Interrupt the search
      setStatusBarText(i18n("Search interrupted"));
      if (!cachedPage)
        delete searchPage;
      return;
    }

    if (i != 0)
    {
      setStatusBarText(i18n("Search page %1 of %2").arg(pageNumber).arg(numberOfPages()));
      kapp->processEvents();
    }

    // Check if we already have a rendered version of the page in the cache. As we are only interested in the
    // text we don't care about the page size.
    if (pageCache->isPageCached(pageNumber))
    {
      // If the last search page used was created locally, we need to destroy it
      if (!cachedPage)
        delete searchPage;

      searchPage = pageCache->getPage(pageNumber);
      cachedPage = true;
    }
    else
    {
      // If the page is not in the cache we draw a small version of it, since this is faster.

      // We only create a new searchPage if we need to, otherwise reuse the existing one.
      if (!searchPage || cachedPage)
        searchPage = new RenderedDocumentPagePixmap();

      cachedPage = false;

      searchPage->resize(1,1);
      searchPage->setPageNumber(pageNumber);
      renderer->getText(searchPage);
    }

    // If there is no text in the current page, try the next one.
    if (searchPage->textBoxList.size() == 0)
      continue;

    foundSelection = searchPage->findRev(searchText, startingTextItem, case_sensitive);

    if (foundSelection.isEmpty())
    {
      // In the next page, start search again at the beginning.
      startingTextItem = -1;
      clearSelection();

      if (pageNumber == 1)
      {
        int answ = KMessageBox::questionYesNo(scrollView(),
                  i18n("<qt>The search string <strong>%1</strong> could not be found by the "
                        "beginning of the document. Should the search be restarted from the end "
                        "of the document?</qt>").arg(searchText),
                  i18n("Text Not Found"), KStdGuiItem::cont(), KStdGuiItem::cancel());

        if (answ != KMessageBox::Yes)
        {
          setStatusBarText(QString::null);
          searchInProgress = false;
          if (!cachedPage)
            delete searchPage;
          return;
        }
      }
    }
    else
    {
      pageCache->selectText(foundSelection);
      gotoPage(pageCache->selectedText());
      setStatusBarText(QString::null);
      searchInProgress = false;
      if (!cachedPage)
        delete searchPage;
      return;
    }
  }

  KMessageBox::sorry(scrollView(), i18n("<qt>The search string <strong>%1</strong> could not be found.</qt>").arg(searchText));
  setStatusBarText(QString::null);
  searchInProgress = false;
  if (!cachedPage)
    delete searchPage;
}


void KMultiPage::clearSelection()
{
  PageNumber page = pageCache->selectedText().getPageNumber();

  if (!page.isValid())
    return;

  // Clear selection
  pageCache->deselectText();

  // Now we need to update the widget which contained the selection
  switch(widgetList.size())
  {
    case 0:
      kdError(4300) << "KMultiPage::clearSelection() while widgetList is empty" << endl;
      break;
    case 1:
      widgetList[0]->update();
      break;
    default:
      for (unsigned int i = 0; i < widgetList.size(); i++)
      {
        DocumentWidget* pageWidget = (DocumentWidget*)widgetList[i];
        if (pageWidget->getPageNumber() == page)
        {
          pageWidget->update();
          break;
        }
      }
  }
}

void KMultiPage::copyText()
{
  pageCache->selectedText().copyText();
}

void KMultiPage::timerEvent( QTimerEvent * )
{
#ifdef KMULTIPAGE_DEBUG
  kdDebug(4300) << "Timer Event " << endl;
#endif
  reload();
}


void KMultiPage::reload()
{
#ifdef KMULTIPAGE_DEBUG
  kdDebug(4300) << "Reload file " << m_file << endl;
#endif
  
  if (renderer.isNull()) {
    kdError() << "KMultiPage::reload() called, but no renderer was set" << endl;
    return;
  }
  
  if (renderer->isValidFile(m_file)) {
    pageCache->clear();
    pageCache->deselectText();
    document_history.clear();
    emit setStatusBarText(i18n("Reloading file %1").arg(m_file));
    Q_INT32 pg = currentPageNumber();

    killTimer(timer_id);
    timer_id = -1;
    bool r = renderer->setFile(m_file, m_url);
    
    generateDocumentWidgets();

    // Set Table of Contents
    tableOfContents->setContents(renderer->getBookmarks());

    // Adjust number of widgets in the thumbnail sidebar
    markList()->clear();
    markList()->setNumberOfPages(numberOfPages(), KVSPrefs::showThumbnails());

    setCurrentPageNumber(pg);
    setFile(r);
    emit setStatusBarText(QString::null);
  } else {
    if (timer_id == -1)
      timer_id = startTimer(1000);
  }
}


bool KMultiPage::openFile()
{
  if (renderer.isNull()) {
    kdError(4300) << "KMultiPage::openFile() called when no renderer was set" << endl;
    return false;
  }

  pageCache->deselectText();
  document_history.clear();
  pageCache->clear();
  emit setStatusBarText(i18n("Loading file %1").arg(m_file));

  bool r = renderer->setFile(m_file, m_url);

  if (r) {
    setCurrentPageNumber(1);
    generateDocumentWidgets();

    // Set number of widgets in the thumbnail sidebar
    markList()->clear();
    markList()->setNumberOfPages(numberOfPages(), KVSPrefs::showThumbnails());
    
    QString reference = url().ref();
    if (!reference.isEmpty())
      gotoPage(renderer->parseReference(reference));
    
    // Set Table of Contents
    tableOfContents->setContents(renderer->getBookmarks());
  } else
    m_file = QString::null;

  
  setFile(r);
  
  // Clear Statusbar
  emit setStatusBarText(QString::null);
  return r;
}


bool KMultiPage::openURL(const QString &filename, const KURL &base_url)
{
  m_file = filename;
  m_url = base_url;

  bool success = openFile();
  if (success)
    setCurrentPageNumber(1);

  return success;
}


void KMultiPage::enableActions(bool fileLoaded)
{
  Q_UNUSED(fileLoaded);
}

void KMultiPage::wheelEvent(QWheelEvent *e)
{
  QScrollBar *sb = scrollView()->verticalScrollBar();
  if (sb == 0)
    return;

  // Zoom in/out
  if (e->state() & ControlButton)
  {
    if (e->delta() < 0)
      emit zoomOut();
    else
      emit zoomIn();
    return;
  }

  Q_INT32 pxl = -(e->delta()*sb->lineStep())/60;
  if (pxl == 0)
  {
    if (e->delta() > 0)
      pxl = -1;
    else
      pxl = 1;
  }

  // Faster scrolling
  if (e->state() & ShiftButton)
    pxl *= 10;

  scroll(pxl);
}


KPrinter *KMultiPage::getPrinter(bool enablePageSizeFeatures)
{
    // Allocate a new KPrinter structure, if necessary
  KPrinter *printer = new KPrinter(true);
  if (printer == 0) {
    kdError(1223) << "KMultiPage::getPrinter(..): Cannot allocate new KPrinter structure" << endl;
    return 0;
  }
  
  // Allocate a new KPrintDialogPage structure and add it to the
  // printer, if the kmultipage implementation requests that
  if (enablePageSizeFeatures == true) {
    KPrintDialogPage_PageOptions *pageOptions = new KPrintDialogPage_PageOptions();
    if (pageOptions == 0) {
      kdError(1223) << "KMultiPage::getPrinter(..): Cannot allocate new KPrintDialogPage_PageOptions structure" << endl;
      delete printer;
      return 0;
    }
    printer->addDialogPage( pageOptions );
  }
  
  // Feed the printer with useful defaults and information.
  printer->setPageSelection( KPrinter::ApplicationSide );
  printer->setCurrentPage( currentPageNumber() );
  printer->setMinMax( 1, numberOfPages() );
  printer->setFullPage( true );
  
  // If pages are marked, give a list of marked pages to the
  // printer. We try to be smart and optimize the list by using ranges
  // ("5-11") wherever possible. The user will be tankful for
  // that. Complicated? Yeah, but that's life.
  QValueList<int> selectedPageNo = selectedPages();
  if (selectedPageNo.isEmpty() == true)
    printer->setOption( "kde-range", "" );
  else {
    int commaflag = 0;
    QString range;
    QValueList<int>::ConstIterator it = selectedPageNo.begin();
    do{
      int val = *it;
      if (commaflag == 1)
	range +=  QString(", ");
      else
	commaflag = 1;
      int endval = val;
      if (it != selectedPageNo.end()) {
	QValueList<int>::ConstIterator jt = it;
	jt++;
	do{
	  int val2 = *jt;
	  if (val2 == endval+1)
	    endval++;
	  else
	    break;
	  jt++;
	} while( jt != selectedPageNo.end() );
	it = jt;
      } else
	it++;
      if (endval == val)
	range +=  QString("%1").arg(val);
      else
	range +=  QString("%1-%2").arg(val).arg(endval);
    } while (it != selectedPageNo.end() );
    printer->setOption( "kde-range", range );
  }
  
  return printer;  
}

void KMultiPage::doExportText()
{
  // Generate a suggestion for a reasonable file name
  QString suggestedName = url().filename();
  suggestedName = suggestedName.left(suggestedName.find(".")) + ".txt";

  QString fileName = KFileDialog::getSaveFileName(suggestedName, i18n("*.txt|Plain Text (Latin 1) (*.txt)"), scrollView(), i18n("Export File As"));

  if (fileName.isEmpty())
    return;

  QFileInfo finfo(fileName);
  if (finfo.exists())
  {
    int r = KMessageBox::warningContinueCancel (scrollView(),
                i18n("The file %1\nexists. Do you want to overwrite that file?").arg(fileName),
                i18n("Overwrite File"), i18n("Overwrite"));

    if (r == KMessageBox::Cancel)
      return;
  }

  QFile textFile(fileName);
  textFile.open(IO_WriteOnly);
  QTextStream stream(&textFile);

  QProgressDialog progress(i18n("Exporting to text..."), i18n("Abort"), renderer->totalPages(),
                           scrollView(), "export_text_progress", true);
  progress.setMinimumDuration(300);

  RenderedDocumentPagePixmap dummyPage;
  dummyPage.resize(1, 1);

  for(unsigned int page = 1; page <= renderer->totalPages(); page++)
  {
    progress.setProgress(page);
    qApp->processEvents();

    if (progress.wasCancelled())
      break;

    dummyPage.setPageNumber(page);
    // We gracefully ignore any errors (bad file, etc.)
    renderer->getText(&dummyPage);

    for(unsigned int i = 0; i < dummyPage.textBoxList.size(); i++)
    {
      // We try to detect newlines
      if (i > 0)
      {
        // Like all our textalgorithmns this currently assumes left to right text.
        // TODO: make this more generic. But we first would need to guess the corrent
        // orientation.
        if (dummyPage.textBoxList[i].box.top() > dummyPage.textBoxList[i-1].box.bottom() &&
            dummyPage.textBoxList[i].box.x() < dummyPage.textBoxList[i-1].box.x())
        {
          stream << "\n";
        }
      }
      stream << dummyPage.textBoxList[i].text;
    }

    // Send newline after each page.
    stream << "\n";
  }

  // Switch off the progress dialog, etc.
  progress.setProgress(renderer->totalPages());
  return;
}

void KMultiPage::slotEnableMoveTool(bool enable)
{
  emit enableMoveTool(enable);
}

#include "kmultipage.moc"
