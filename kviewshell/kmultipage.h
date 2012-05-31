// -*- C++ -*-
#ifndef _KMULTIPAGE_H
#define _KMULTIPAGE_H

#include "pageView.h"
#include "documentPageCache.h"
#include "documentRenderer.h"
#include "history.h"
#include "kmultipageInterface.h"
#include "marklist.h"

#include <kparts/part.h>
#include <qtimer.h>

class Anchor;
class DocumentWidget;
class KConfigDialog;
class KPrintDialogPage_PageOptions;
class KPrinter;
class PageView;
class QPainter;
class QSplitter;
class QToolBox;
class simplePageSize;
class TableOfContents;
class PageNumber;
class SearchWidget;


/** \brief This class provides plugin-specific GUI elements for kviewshell plugins

@author Wilfried Huss, Stefan Kebekus

 */

// TODO remove virtual inheritance for KDE 4. It's the reason for the strange DCOPObject construction
class KMultiPage : public KParts::ReadOnlyPart, virtual public kmultipageInterface
{
  Q_OBJECT

public:
  KMultiPage(QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name);
  virtual ~KMultiPage();

  /* returns the scrollview used for the display */
  virtual QWidget* mainWidget() {return _scrollView;}

  /* Methods which are associated with the DCOP functionality of the
     kmultipage. This method returns the file name (not the URL) of
     the currently loaded file. */
  QString name_of_current_file();

  /* Methods which are associated with the DCOP functionality of the
     kmultipage. This method can be implemented by the multipage,
     e.g. to jump to a certain location. */
  virtual ASYNC  jumpToReference(const QString& /*reference*/);

  /* Methods which are associated with the DCOP functionality of the
     kmultipage. This method checks if a given file is loaded. */
  bool   is_file_loaded(const QString& filename);

  /* Opens file and sets URL

     This method does the same as openFile, but sets the m_url of the
     kmultipage. This can be important, for the following reason:
     assume that a DVI is or DJVU-file is located on a web server at
     baseURL=http://www.x.x/x.dvi The file may refer to external
     graphic files using relative links.

     The file is downloaded by the kviewpart to a temporary file on
     the hard disk, say /tmp/t.dvi. The kviewpart would then call this
     method with filename=/tmp/t.dvi and baseURL=http://www.x.x/x.dvi,
     so the DVI-renderer knows to interpret the link to t.jpg as
     http://www.x.x/t.jpg, and will download the file from there.

     @warning This method is virtual only for technical reasons. Do
     not re-implement this method

     @returns true on success, false on failure
  */
  virtual bool openURL(const QString &filename, const KURL &base_url);

  /** Prints a document

  This method prints a document. The default implementation offers
  fairly good printer support, but printing with the default
  implementation is usually quite slow as tremendous amounts of data
  needs to be transferred to the printer. To limit the data sent to
  the printer, this default implementation prints only at low
  resolution and produces mediocre quality. This method can (and
  should) therefore be re-implemented if you have good code to convert
  your document to PostScript.

  Example: If your document consists of a single A4 page that contains
  a DJVU image of 30KB, then the default implementation would render
  the image in 600dpi, i.e. in about 7000x5000 pixel, and then send
  this huge graphics uncompressed to the printer. A smart
  implementation, on the other hand, would send the DJVU-file directly
  to the printer, together with a DJVU decoder, which is implemented
  in PostScript and uses only a few KB of memory, making for less than
  40KB of data sent to the printer.

  If you decide to re-implement this method, you need to decide if
  your implementation will support the options offered by the "page
  size & placement" tab of the print dialog. If these options are set,
  pages that are smaller/larger than the printer's paper size will be
  shrunk/enlarged and optionally centered on the paper.

  If your implementation does not support the options, the following
  code should be used:
  @code
  // Obtain a fully initialized KPrinter structure, and disable all
  // entries in the "Page Size & Placement" tab of the printer dialog.
  KPrinter *printer = getPrinter(false);
  // Abort with an error message if no KPrinter could be initialized
  if (printer == 0) {
    kdError(4300) << "KPrinter not available" << endl;
    return;
  }

  // Show the printer options dialog. Return immediately if the user
  // aborts.
  if (!printer->setup(parentWdg, i18n("Print %1").arg(m_file.section('/', -1)) )) {
    delete printer;
    return;
  }

  ...  <Produce a PostScript file, with filename, say, tmpPSFile. You
  can use all the features that KPrinter has to offer. Note that
  printer->pageList() gives a list of pages to be printed, where "1"
  denotes the first page, "2" the second, etc.> ...

  printer->printFiles( QStringList(tmpPSFile), true );
  delete printer;
  @endcode

  If your implementation does support the options, code must be
  provided to support the KPrinter options
  "kde-kviewshell-shrinkpage", "kde-kviewshell-expandpage",
  "kde-kviewshell-centerpage" and "kde-kviewshell-rotatepage". It is
  important to note that "kde-kviewshell-rotatepage" and
  "kde-kviewshell-centerpage" should by default treated as "true",
  while the other options should be "false" by default. The following
  code sees to that:
  @code
  // Obtain a fully initialized KPrinter structure, and enable all
  // entries in the "Page Size & Placement" tab of the printer dialog.
  KPrinter *printer = getPrinter(true);
  // Abort with an error message if no KPrinter could be initialized
  if (printer == 0) {
    kdError(4300) << "KPrinter not available" << endl;
    return;
  }

  // Show the printer options dialog. Return immediately if the user
  // aborts.
  if (!printer->setup(parentWdg, i18n("Print %1").arg(m_file.section('/', -1)) )) {
    delete printer;
    return;
  }

  if (printer->option( "kde-kviewshell-shrinkpage" ) == "true")
    <Shrink some pages to paper size, where necessary>
  if (printer->option( "kde-kviewshell-expandpage" ) == "true")
    <Expand some pages to paper size, where necessary>
  if (printer->option( "kde-kviewshell-centerpage" ) != "false") 
    <Center pages on paper>
  if (printer->option( "kde-kviewshell-rotatepage" ) != "false") 
    <rotate pages by 90 deg. counterclockwise, if paper orientation is different from pages orientation>

  ...  <Produce a PostScript file, with filename, say, tmpPSFile. You
  can use all the features that KPrinter has to offer. Note that
  printer->pageList() gives a list of pages to be printed, where "1"
  denotes the first page, "2" the second, etc.> ...

  printer->printFiles( QStringList(tmpPSFile), true ); 
  delete printer;
  @endcode 

  For more information, see the default implementation in the source
  file kmultipage.cpp. You might also look at the documentation to
  getPrinter().
  */
  virtual void print();


  /* Returns true if the document specifies page sizes, and false
     otherwise. NOTE: the information returned by this method is not
     always 100% reliable. Although unlikely, it is theoretically
     possible that this method returns 'true', but still some of the
     sizes returned by sizeOfPage() are invalid. */
  virtual bool hasSpecifiedPageSizes() const {return renderer && renderer->hasSpecifiedPageSizes();}

  /* This methos is similar to openFile(). It is used when the "Watch
     file" option is activated, and the file has changed on disk. It
     differs from openFile() in two aspects: first, the file is
     checked for validity with DVIRenderer.isValidFile(m_file) if the
     file is invalid, a timer event is used to call the method again
     after a brief pause. Secondly, when the GUI is updated, the
     implementation does not jump to the first page, but tries to keep
     the current page. */
  virtual void reload();

// Interface definition start ------------------------------------------------

  /** list of supported file formats, for saving
      
  This member must return the list of supported file formats for
  saving. These strings returned must be in the format explained in
  the documentation to KFileDialog::setFilter(), e.g. ""*.cpp *.cxx
  *.c++|C++ Source Files". The use of mimetype-filters is allowed, but
  discouraged. Among other penalties, if a multipage gives a
  mimetype-filter, e.g. "application/x-dvi", the open file dialog in
  kviewshell will not allow the user to see and select compressed
  dvi-files.
  */
  virtual QStringList fileFormats() const = 0;

  /// closes a file
  virtual bool closeURL();

  /* sets a zoom factor. The multipage implementation might refuse to
   use a given zoom factor, even if it falls within the bounds given
   by the constants MinZoom and MaxZoom which are defined in
   zoomlimits.h. In that case, the multipage implementation chooses a
   different zomm factor. The implementation returns the factor which
   has actually been used. A default implementation is provided. */
  virtual double setZoom(double z);
  
  /** reads in settings. Reimplementations must call this. */
  virtual void readSettings();
  
  /** writes settings. Reimplementations must call this. */
  virtual void writeSettings();
  
  /** Flag to indicate that this implementation has support for textserach and selection */
  virtual bool supportsTextSearch() const { return getRenderer() && getRenderer()->supportsTextSearch(); }

  /** Flag to indicate the document was modified since last saved

      KMultiPage implementations that offer functionality that
      modifies the document (e.g. remove or insert pages) must
      re-implement this method to return 

      @returns 'true' if the document was modified since it was last
      saved
  */
  virtual bool isModified() const {return false;}

  /* Returns the number of the first (i.e. top-left) page in the
     display. Such a number cannot be reasonably assigned
     (e.g. because the current document is empty, or because no
     document has been loaded yet), the method returns "invalid page",
     i.e. 0. */
  virtual PageNumber currentPageNumber();

  /* Returns the number of pages in the currently loaded document or
     0, if no document is loaded or if the document is empty */
  PageNumber numberOfPages() const {return renderer.isNull() ? (PageNumber)0 : renderer->totalPages();}

  /* List of pages selected in the sidebar

      @returns a list with the numbers of the currently selected
      pages. */
  virtual QValueList<int> selectedPages() {return markList()->selectedPages();}

  virtual History* history() { return &document_history; }

  /** Add pages to the KViewshell's central preferences dialog

      This method can be re-implemented to add documenttype specific
      configuration pages to the central preferences dialog. The
      documentation to KConfigDialog explains how to do that.

      The default implementation does nothing.

      @param configDialog a pointer to the KConfigDialog the dialog to
      add pages to
  */
  virtual void addConfigDialogs(KConfigDialog* configDialog) { Q_UNUSED(configDialog); }


  /* These methods calculate the Zoomfactor needed to fit the pages
     into the current viewport. Note that the return value need *not*
     be within the limits defined in "zoomLimits.h". If they are not,
     this indicates that fitting to width or height is currently not
     possible (e.g. because no document is loaded). The return value
     should then be ignored and any operation that relies on the
     return value should be aborted. */
  virtual double calculateFitToWidthZoomValue();
  virtual double calculateFitToHeightZoomValue();

  /* Returns the number of columns into which the widgets are aligned. */
  virtual Q_UINT8 getNrColumns() const { return _scrollView->getNrColumns(); }
  virtual Q_UINT8 getNrRows() const { return _scrollView->getNrRows(); }

  virtual bool overviewMode() const { return _scrollView->overviewMode(); }

  // =========== Interface definition ends

  /* ---------------------------------------------------------------------
     The following methods export functions of the DocumentPageCache which
     are currently needed by KViewPart. TODO: Clean this up again without
     directly linking DocumentPageCache to the KViewPart. */

  /* Returns the size of page 'page'. If the document does not
      specify a size (which happens, e.g., for some DVI-files), then
      the userPreferredSize is returned. */
  virtual SimplePageSize sizeOfPage(const PageNumber& page = 1) const { return pageCache->sizeOfPage(page); }
  
public slots:
  /* Sets the userPreferredSize, which is explained below */
  virtual void setUserPreferredSize(const SimplePageSize& t) { pageCache->setUserPreferredSize(t); }
  virtual void setUseDocumentSpecifiedSize(bool b) { pageCache->setUseDocumentSpecifiedSize(b); }
  // --------------------------------------------------------------------

protected:
  /** Update GUI after loading or closing of a file

      This method is called by openFile() when a new file was loaded,
      and by closeURL() when a file is closed so that the kmultipage
      implementation can update its own GUI, enable/disable actions,
      prepare info texts, etc. At the time the method is executed, the
      file has already been loaded into the renderer using
      renderer.setFile(), or closed using renderer.clear() and the
      standard KMultiPage GUI is set up. The filename can be accessed
      via m_file.

      The default implementation does nothing.

      @param success the return value of renderer.setFile() which
      indicates if the file could be successfully loaded by the
      renderer, or not. Note that setFile() is called even if the
      renderer returned 'false', so that the implemtation can then
      disable menu items, etc. */
  virtual void setFile(bool success);

  /* Creates new instances of DocumentWidget. If you need special
     functionality and reimplement the DocumentWidget class, then you
     should also reimplement this method to ensure that your new
     DocumentWidgets will be used. This function is also the right
     place to connect to signals emitted by DocumentWidget. */
  virtual DocumentWidget* createDocumentWidget();

  /* Used to enable/disable KActions of multiPage implementations.
     enableActions(true) should be called whenever a file is
     successfully loaded.  enableActions(false) is called when the
     file is closed. */
  virtual void enableActions(bool);

  /* Initializes all data structures that need to know the renderer.
     This function must be called in the constructor of multipage
     implementations. */
  void setRenderer(DocumentRenderer*);

  /** This function creates the page cache. If a multipage implementation needs
     additional functionaly from the cache overwrite this function to create a
     subclass of DocumentPageCache.
     @warning This function is called by the constructor, never call it explicitly.
  */
  virtual void initializePageCache();

  /* Returns a pointer to the renderer. */
  virtual QGuardedPtr<DocumentRenderer> getRenderer() const { return renderer; }

  PageView* scrollView() { return _scrollView; }

  MarkList* markList() { return _markList; }

  // The next two functions are used for the autozoom feature
  // TODO optimize (the calculation of the widest page needs to be done only once
  // per document, not everytime calculateFitToWidthZoomValue() is called)
  PageNumber widestPage() const;

  // TODO Generalize this for more than 2 columns
  double zoomForWidthColumns(unsigned int viewportWidth) const;

public slots:
  virtual void doSelectAll();

  virtual void clearSelection();

  virtual void copyText();

  /** Exports the document to a plain text file. */
  virtual void doExportText();

  /* Shows the "text search" dialog, if text search is supported by
     the renderer. Otherwise, the method returns immediately. */
  virtual void showFindTextDialog();


  /* This method may be called after the text search dialog
     'findDialog' has been set up, and the user has entered a search
     phrase. The method searches for the next occurence of the text,
     starting from the beginning of the current page, or after the
     currently selected text, if there is any. */
  virtual void findNextText();

  /* This method may be called after the text search dialog
     'findDialog' has been set up, and the user has entered a search
     phrase. The method searches for the next occurence of the text,
     starting from the end of the current page, or before the
     currently selected text, if there is any. */
  virtual void findPrevText();

  /** Opens a file requestor and starts a basic copy KIO-Job. A
      multipage implementation that wishes to offer saving in various
      formats must re-implement this slot. */
  virtual void slotSave();

  /* The standard implementation just calls slotSave. */
  virtual void slotSave_defaultFilename();

  /* Initialize/Update PageWidgets, thumbnail list and bookmark list

  This slot is called after a document was loaded, when the
  document was modified (e.g. when pages were deleted), or the
  rendering mode is changed (e.g. when a different accessibility
  viewmode is selected). The following is done

  - The page cache is cleared

  - all page widgets as well as the thumbnails are updated.
  */
  void renderModeChanged();

  /* Empties the page cache and --as the name suggests-- repaints all
     visible widgets. */
  void repaintAllVisibleWidgets();

  /* Tells the multipage if scrollbars should be used. */
  virtual void slotShowScrollbars(bool);

  /* Show or hide the sidebar widget. */
  virtual void slotShowSidebar(bool);

  /* Show or hide thumbnails. */
  virtual void slotShowThumbnails(bool);

  /* Used internally. */
  void slotIOJobFinished ( KIO::Job *job );

  /* Switches to fullscreen mode and back. */
  virtual void slotSetFullPage(bool fullpage);

  virtual void setViewMode(int);

  /* Is called if settings are changed in the configuration dialog.
     If this method is reimplemented in a child class, it needs to be
     called from there. */
  virtual void preferencesChanged();

  /* Page Navigation. */
  virtual bool gotoPage(const PageNumber& page);
  virtual void gotoPage(const Anchor& a);

  virtual void prevPage();
  virtual void nextPage();
  virtual void firstPage();
  virtual void lastPage();

  virtual void scrollUp();
  virtual void scrollDown();
  virtual void scrollLeft();
  virtual void scrollRight();

  virtual void scrollUpPage();
  virtual void scrollDownPage();
  virtual void scrollLeftPage();
  virtual void scrollRightPage();

  virtual void readUp();
  virtual void readDown();

  virtual void doGoBack();
  virtual void doGoForward();

  /* Scrolls the main scrollview by deltaInPixel (positive values
     scroll DOWN). If the user tries to scroll past the beginning or
     the end of a page, then the method either returns without doing
     anything (if the current page is the first or last page, resp, or
     if the method is called within 200ms after the beg. or end of the
     page was reached), or goes the the next/previous page. The delay
     makes it a little easier for the user to scroll with the mouse
     wheel or the keyboard without involuntarily moving to another
     page. */
  virtual void scroll(Q_INT32 deltaInPixel);

  virtual void slotEnableMoveTool(bool enable);

protected slots:
  virtual bool gotoPage(const PageNumber& page, int y, bool isLink = true);

  /* Make the selection visible */
  void gotoPage(const TextSelection&);

private slots:
   void         handleLocalLink(const QString &linkText);

signals:
  /** Emitted with argument "true" when the move tool is selected, and
      with argument "false" if selection tool is selected. */
  void enableMoveTool(bool enable);

  /* Emitted when a page has been selected in the MarkList. */
  void selected(const PageNumber& pageNumber);

  /* Emitted to indicate the number of pages in the file and the
     current page. The receiver will not change or update the display,
     nor call the gotoPage()-method. */
  void pageInfo(int nr, int currpg);

  void askingToCheckActions();

  /* emitted when a new preview is available */
  void previewChanged(bool previewAvailable);

  void viewModeChanged();

  /* Emitted when the zoom of the pageview changes. */
  void zoomChanged();

  void zoomOut();
  void zoomIn();

  /* Emitted it the status of the text selection changes. */
  void textSelected(bool);

  void searchEnabled(bool);

// Interface definition end --------------------------------------------------

public slots:
  virtual void generateDocumentWidgets(const PageNumber& startPage = PageNumber::invalidPage);

 protected slots:
  /* This is the slot where mouseWheel events are processed that come
     from the multipage/scrollview. This method calles scroll, a
     delta-value of 120 (i.e. one notch on a standard wheel mouse)
     scrolls by two 'lines'. */
  void wheelEvent(QWheelEvent *);

protected:
  /** Allocates and initializes a KPrinter structure

  This method is used in implementations of the print() method. See
  the documentation of print() for more information and for example
  code.  This method allocates and initializes a KPrinter
  structure. The list of pages marked in the sidebar is already set in
  the "kde-range" option in the KPrinter structure. In this option "1"
  means: first page of the document, "2" the second, etc.
  
  @param enablePageSizeFeatures Enables or diables the entries in the
  "Page Size & Placement" tab of the print dialog.

  @returns a pointer to a KPrinter, or 0 if no KPrinter could be
  allocated.

  @warning The KPrinter allocated is owned by the caller must be
  deleted before the KMultiPage is deleted. Otherwise, a segfault will
  occur.
  */
  KPrinter *getPrinter(bool enablePageSizeFeatures=true);


  /** Pointer to the parent widget 

  This pointer is automatically set by the constructor.
  */
  QGuardedPtr<QWidget> parentWdg;

  QPtrVector<DocumentWidget> widgetList;

  History document_history;

  /* Variable which is used internally by the method
     currentPageNumber() to provide 'lazy' page numbers. */
  PageNumber lastCurrentPage;

  /* The pageCache caches already drawn "documentPages" and invokes
     the renderer if the needed page is not available in the cache. */
  DocumentPageCache* pageCache;

private slots:
  void setCurrentPageNumber(const PageNumber&);
  void updateWidgetSize(const PageNumber&);

  /* Interrupts a search if one is currently performed, otherwise
     hide the search panel */
  void stopSearch();

private:
  /* For internal use by the reload()-method. See the comments in
     kmultipage.cpp, right before the timerEvent function. */
  int  timer_id;

  /* For internal use the reload()-method. This is a dreadful
     hack. The problem we adress with this timer event is the
     following: the kviewshell has a KDirWatch object which looks at
     the DVI file and calls reload() when the object has changed. That
     works very nicely in principle, but in practise, when TeX runs
     for several seconds over a complicated file, this does not work
     at all. First, most of the time, while TeX is still writing, the
     file is invalid. Thus, reload() is very often called when the DVI
     file is bad. We solve this problem by checking the file first. If
     the file is bad, we do not reload. Second, when the file finally
     becomes good, it very often happens that KDirWatch does not
     notify us anymore. Whether this is a bug or a side effect of a
     feature of KDirWatch, I dare not say. We remedy that problem by
     using a timer: when reload() was called on a bad file, we
     automatically come back (via the timerEvent() function) every
     second and check if the file became good. If so, we stop the
     timer. It may well happen that KDirWatch calls us several times
     while we are waiting for the file to become good, but that does
     not do any harm. */
  void timerEvent( QTimerEvent *e );

  /* This method opens a file and sets up the GUI when the file is
     loaded. It calls setFile() so that implementations of kmultipage
     can  update their own GUI. DO NOT REIMPLEMENT THIS METHOD. */
  bool openFile();

  /* Is set by setRenderer. */
  QGuardedPtr<DocumentRenderer> renderer;

  PageView *_scrollView;
  SearchWidget* searchWidget;
  QToolBox* sideBar;

  MarkList* _markList;
  TableOfContents* tableOfContents;

  QSplitter* splitterWidget;

  /* This timer is used to implement a brief delay when the user
     scrolls past the beginning or the end of the page before a the
     program moves to a new page. That way, it is a little easier for
     the user to scroll with the mouse wheel or the keyboard without
     involuntarily moving to another page. The timer is used in the
     scroll() method. */
  QTimer changePageDelayTimer;

  // This is set to true while a text search is performed.
  // If set to false the search is interrupted.
  bool searchInProgress;
};


#endif
