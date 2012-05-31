   /*

    $Id: kfax.cpp 604390 2006-11-12 18:29:12Z mkoller $

    Copyright (C) 1997 Bernd Johannes Wuebben
                       wuebben@math.cornell.edu

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

    */

#ifdef KDE_USE_FINAL
/* NewImage() in viewfax.cpp needs to fiddle with the Display structure */
#define XLIB_ILLEGAL_ACCESS
#endif

#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#include <qfile.h>
#include <qstrlist.h>
#include <qtimer.h>
#include <qpopupmenu.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qbitmap.h>

#include <klocale.h>
#include <kaboutdata.h>
#include <kstandarddirs.h>
#include <kiconloader.h>
#include <kfiledialog.h>
#include <kfilemetainfo.h>
#include <kstdaccel.h>
#include <kconfig.h>
#include <kmenubar.h>
#include <kmessagebox.h>
#include <kcmdlineargs.h>
#include <kio/netaccess.h>
#include <knotifyclient.h>
#include <ktempfile.h>
#include <kstdaction.h>
#include <kdebug.h>
#include <kurldrag.h>
#include <kstatusbar.h>
#include <kaction.h>
#include <kprocess.h>
#include <kprinter.h>
#include <kio/job.h>
#include <kdebug.h>

#include "faxexpand.h"
#include "kfax.h"
#include "version.h"
#include "viewfax.h"
#include "options.h"
#include "kfax_printsettings.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <X11/cursorfont.h>

// StatusBar field IDs
#define ID_LINE_COLUMN 1
#define ID_INS_OVR 2
#define ID_GENERAL 3
#define ID_FNAME 4
#define ID_TYPE 5
#define ID_PAGE_NO 6


TopLevel *toplevel;

extern  int GetImage(struct pagenode *pn);

void 	TurnFollowing(int How, struct pagenode *pn);
void 	handle_X_event(XEvent event);
void 	ShowLoop(void);
void 	SetupDisplay();
void 	mysighandler(int sig);
void    setFaxDefaults();

void 	parse(char* buf, char** args);

extern void g31expand(struct pagenode *pn, drawfunc df);
extern void g32expand(struct pagenode *pn, drawfunc df);
extern void g4expand(struct pagenode *pn, drawfunc df);

#define PATIENCE 100000

static char KFAX_FILETYPES[] = "image/fax-g3 image/tiff";

int 	ExpectConfNotify = 1;

GC 	PaintGC;
Cursor 	WorkCursor;
Cursor 	ReadyCursor;
Cursor 	MoveCursor;
Cursor 	LRCursor;
Cursor 	UDCursor;

extern Time Lasttime;
extern bool have_cmd_opt;
extern  struct pagenode *viewpage;

extern 	XImage *FlipImage(XImage *xi);
extern 	XImage *MirrorImage(XImage *xi);
extern 	XImage *RotImage(XImage *Image);
extern 	XImage *ZoomImage(XImage *Big);
extern 	void FreeImage(XImage *Image);

extern 	XImage *Image, *Images[MAXZOOM];

int 	xpos, ox;   /* x, old x, offset x, new xpos*/
int 	ypos, oy;  /* y, old y, offset y, new y */
int 	offx, offy;
int 	nx, ny;

int oz, Resize, Refresh;		/* old zoom, window size changed,
					   needs updating */
int PaneWidth, PaneHeight;		/* current size of our window */
int AbsX, AbsY;				/* absolute position of centre of window */

Display* qtdisplay;

int 	startingup;
Window  qtwin;    // the qt window
Window 	Win;
int 	qwindow_height;
int 	qwindow_width;
bool 	have_no_fax = TRUE;
bool	display_is_setup = FALSE;
struct 	optionsinfo fop;   // contains the fax options

extern 	struct pagenode *firstpage, *lastpage, *thispage;
extern  struct pagenode* auxpage;

extern 	struct pagenode defaultpage;

bool buttondown;

bool MyApp::x11EventFilter( XEvent * ev)
{
  if (KApplication::x11EventFilter(ev))
    return TRUE;

  if (ev->type  ==  ButtonRelease){
    /* this is so that the cursor goes back to normal on leaving the fax window
       and that the fax won't be moved when I reenter after I release the mouse*/

    if (buttondown == true){
      buttondown = false;
      XDefineCursor(qtdisplay, Win, ReadyCursor);
      XFlush(qtdisplay);
    }
  }
  if ( ev->xany.window == qtwin ||
       ev->xany.window == Win){

    if(startingup || have_no_fax)
      return FALSE;

    toplevel->handle_X_event(*ev);
    ev->xany.window = qtwin;

  }

  return FALSE;

}

TopLevel::TopLevel (QWidget *, const char *name)
    : KMainWindow (0, name)
{
  setMinimumSize (100, 100);

  buttondown = false;

  setupMenuBar();
  setupStatusBar();
  updateActions();

  resize(550,400);
  setupGUI();

  readSettings();

  faxqtwin = new QFrame(this);

  qtwin = faxqtwin->winId();
  faxqtwin->setFrameStyle(QFrame::Panel | QFrame::Sunken);

  // Create a Vertical scroll bar

  vsb = new QScrollBar( QScrollBar::Vertical,faxqtwin,"scrollBar" );
  vsb->hide();
  connect( vsb, SIGNAL(valueChanged(int)), SLOT(scrollVert(int)) );

  // Create a Horizontal scroll bar

  hsb = new QScrollBar( QScrollBar::Horizontal,faxqtwin,"scrollBar" );
  connect( hsb, SIGNAL(valueChanged(int)), SLOT(scrollHorz(int)) );
  hsb->hide();

  setCentralWidget(faxqtwin);

  setAcceptDrops(true);

  show();
}


TopLevel::~TopLevel()
{
}

void TopLevel::setupMenuBar()
{
  // File menu
  KStdAction::open( this, SLOT( faxOpen() ), actionCollection() );
  actRecent =  KStdAction::openRecent( this, SLOT( faxOpen( const KURL & ) ),
          actionCollection() );
  actSave = KStdAction::save( this, SLOT( faxSave() ), actionCollection() );
  actSaveAs = KStdAction::saveAs( this, SLOT( faxSaveAs() ),
          actionCollection() );
  actPrint = KStdAction::print( this, SLOT( print() ), actionCollection() );
  KStdAction::quit( this, SLOT( close() ), actionCollection() );
  actAdd = new KAction( i18n( "A&dd..." ), "filenew", KShortcut(), this,
      SLOT( faxAdd() ), actionCollection(), "file_add_fax" );

  actRecent->setMaxItems( 5 );

  // View Menu
  actSize = KStdAction::actualSize( this, SLOT( actualSize() ),
      actionCollection() );
  actZoomIn = KStdAction::zoomIn( this, SLOT( zoomin() ), actionCollection() );
  actZoomOut = KStdAction::zoomOut( this, SLOT( zoomout() ),
      actionCollection() );

  actRotate = new KAction( i18n( "&Rotate Page" ), "rotate", KShortcut(), this,
      SLOT( rotatePage() ), actionCollection(), "view_rotate" );
  actMirror = new KAction( i18n( "Mirror Page" ), KShortcut(), this,
      SLOT( mirrorPage() ), actionCollection(), "view_mirror" );
  actFlip = new KAction( i18n( "&Flip Page" ), KShortcut(), this,
      SLOT( flipPage() ), actionCollection(), "view_flip" );

  // Go menu
  actNext = KStdAction::next( this, SLOT( nextPage() ), actionCollection() );
  actPrev = KStdAction::prior( this, SLOT( prevPage() ), actionCollection() );
  actFirst = KStdAction::firstPage( this, SLOT( firstPage() ),
      actionCollection() );
  actLast = KStdAction::lastPage( this, SLOT( lastPage() ),
      actionCollection() );

  // Settings menu
  KStdAction::preferences( this, SLOT( faxoptions() ), actionCollection() );
}

void TopLevel::setupStatusBar()
{
  statusbar = statusBar();

  statusbar->insertFixedItem(i18n("w: 00000 h: 00000"), ID_INS_OVR);
  statusbar->insertFixedItem(i18n("Res: XXXXX"), ID_GENERAL);
  statusbar->insertFixedItem(i18n("Type: XXXXXXX"), ID_TYPE);
  statusbar->insertFixedItem(i18n("Page: XX of XX"), ID_PAGE_NO);
  statusbar->insertItem(QString::null, ID_FNAME, 1);
  statusbar->setItemAlignment( ID_FNAME, AlignLeft );

  statusbar->changeItem(QString::null, ID_INS_OVR);
  statusbar->changeItem(QString::null, ID_GENERAL);
  statusbar->changeItem(QString::null, ID_TYPE);
  statusbar->changeItem(QString::null, ID_PAGE_NO);
  statusbar->changeItem(QString::null, ID_FNAME);
}

void TopLevel::readSettings()
{
  config = kapp->config();

  applyMainWindowSettings( config, "MainWindowSettings" );

  actRecent->loadEntries( config );

  config->setGroup("General Options");

  config->setGroup("Fax Options");

  fop.width = config->readNumEntry("width", 1728);
  fop.resauto = config->readNumEntry("resauto", 1);
  fop.geomauto = config->readNumEntry("geomauto", 1);
  fop.height = config->readNumEntry("height", 2339);
  fop.fine = config->readNumEntry("resolution", 1);
  fop.flip = config->readNumEntry("flip", 0);
  fop.invert = config->readNumEntry("invert", 0);
  fop.lsbfirst = config->readNumEntry("lsb", 0);
  fop.raw = config->readNumEntry("raw", 3);

  setFaxDefaults();
}

void TopLevel::updateActions()
{
  actAdd->setEnabled( thispage );
  actSave->setEnabled( thispage );
  actSaveAs->setEnabled( thispage );
  actPrint->setEnabled( thispage );

  actRotate->setEnabled( thispage );
  actFlip->setEnabled( thispage );
  actMirror->setEnabled( thispage );

  updateGoActions();
  updateZoomActions();
}

void TopLevel::updateGoActions()
{
  actNext->setEnabled( thispage && thispage->next );
  actPrev->setEnabled( thispage && thispage->prev );
  actFirst->setEnabled( thispage && thispage->prev );
  actLast->setEnabled( thispage && thispage->next );
}

void TopLevel::updateZoomActions()
{
  actSize->setEnabled( Image && oz > 0 );
  actZoomIn->setEnabled( Image && oz > 0 );
  actZoomOut->setEnabled( Image && oz < MAXZOOM-1 && Image->width >= 256 );
}

bool TopLevel::queryClose()
{
  saveMainWindowSettings( config, "MainWindowSettings" );
  actRecent->saveEntries( config );

  return true;
}

void TopLevel::writeSettings()
{
  config = kapp->config();

  config->setGroup("General Options");

  config->setGroup("Fax Options");

  config->writeEntry("resauto",fop.resauto);
  config->writeEntry("geomauto",fop.geomauto);
  config->writeEntry("width",fop.width);
  config->writeEntry("height",fop.height);
  config->writeEntry("resolution",fop.fine);
  config->writeEntry("flip",fop.flip);
  config->writeEntry("invert",fop.invert);
  config->writeEntry("lsb",fop.lsbfirst);
  config->writeEntry("raw",fop.raw);

  config->sync();
}

void TopLevel::faxOpen()
{
  KURL url = KFileDialog::getOpenURL(QString::null, KFAX_FILETYPES);

  faxOpen( url );

  actRecent->addURL( fileURL );
}

void TopLevel::faxOpen( const KURL & url )
{
  if (!url.isValid())
    return;

  faxClose();
  faxAdd( url );

  fileURL = url;

  updateActions();
}

void TopLevel::faxAdd()
{
  KURL url = KFileDialog::getOpenURL(QString::null, KFAX_FILETYPES);

  faxAdd( url );

  actRecent->addURL( fileURL );
}

void TopLevel::faxAdd( const KURL & url )
{
  if (!url.isValid())
    return;

  openNetFile(url);

  updateGoActions();
}

void TopLevel::faxSave()
{
  saveNetFile(fileURL);
}

void TopLevel::faxSaveAs()
{
  fileURL = KFileDialog::getSaveURL(QString::null, KFAX_FILETYPES);

  if (fileURL.isEmpty())
    return;

  faxSave();

  actRecent->addURL( fileURL );
}


static void freeImages()
{
  int i;
  for (i = 1; i < MAXZOOM; i++) {
    if (Images[i])
	FreeImage(Images[i]);
    Images[i] = NULL;
  }
}

static XImage *generateZoomImages(int maxzoom)
{
  int i;
  for (i = 1; i < MAXZOOM; i++){
    if (!Images[i-1])
	continue;
    Image = Images[i] = ZoomImage(Images[i-1]);
    if(Image == NULL){// out of memory
      Image = Images[i -1];
      break;
    }
  }

  i = maxzoom;
  while (!Images[i])
	i--;
  return Images[i];
}


void TopLevel::zoom( int factor )
{
  if(!thispage || !Image || !faxqtwin || !display_is_setup)
    return;

  Resize = Refresh = 1;

  Image = generateZoomImages(factor);

  PaneWidth = Image->width;
  PaneHeight = Image->height;

  resizeView();
  putImage();

  uiUpdate();

  updateZoomActions();
}

void TopLevel::zoomin()
{
  if ( oz > 0 )
  {
    oz--;
    zoom( oz );
  }
}

void TopLevel::zoomout()
{
  if (oz < MAXZOOM && Image->width >= 256)
  {
    ++oz;
    zoom( oz );
  }
}

void TopLevel::actualSize()
{
  oz = 0;
  zoom( oz );
}

void loadfile(QString filename)
{
  // Typical FAX resolutions are:
  // Standard: 203 dpi x 98 dpi
  // Fine: 203 dpi x 196 lpi
  // Super Fine: 203 dpi y 392 lpi,  or
  //             406 dpi x 392 lpi
  QSize dpi(203,196);

  KFileMetaInfo metaInfo(filename);
  if (metaInfo.isValid() && metaInfo.item("Resolution").isValid())
  {
    QSize s = metaInfo.item("Resolution").value().toSize();
    if (s.width() >= 100 && s.height() >= 100)
	dpi = s;
  }

  (void) notetiff(QFile::encodeName(filename));

  struct pagenode *pn;
  for(pn = firstpage; pn; pn = pn->next) 
   if (!pn->dpiX) {
	pn->dpiX = dpi.width();
	pn->dpiY = dpi.height();
   }
}

void TopLevel::openadd(QString filename)
{
  auxpage = lastpage;

  loadfile(filename);

  if( firstpage != lastpage )
  {
    if(auxpage->next)
      auxpage = auxpage->next;
  }

  // auxpage should now point to the first pagenode which was created for
  // the newly added fax file.
  have_no_fax = false;
  thispage = auxpage;
  newPage();
  resizeView();
  putImage();
}

void TopLevel::resizeEvent(QResizeEvent *e)
{
  KMainWindow::resizeEvent(e);

  resizeView();
}

void TopLevel::wheelEvent( QWheelEvent *e )
{
  e->accept();

  if ( e->state() & ShiftButton )
  {
    if ( e->delta() < 0 )
      zoomin();
    else
      zoomout();
  }
  else
  {
    int offset = QApplication::wheelScrollLines()*vsb->lineStep();
    if ( e->state() & ControlButton )
      offset = vsb->pageStep();
    offset = -e->delta()*offset/120;
    vsb->setValue( vsb->value() + offset );
  }
}

void TopLevel::resizeView()
{
  if(!faxqtwin || !display_is_setup)
    return;

//printf("In resizeView() entered\n");

  qwindow_width = faxqtwin->width();
  qwindow_height = faxqtwin->height();

  if( hsb->isVisible())
    qwindow_height -= 16;

  if( vsb->isVisible())
    qwindow_width  -= 16;

  if(Image){
    PaneWidth = Image->width;
    PaneHeight = Image->height;
  }

  //  printf("faxw %d qtw %d\n", PaneWidth , faxqtwin->width());

  if( (PaneHeight  > qwindow_height ) &&
      (PaneWidth  > qwindow_width)){

    vsb->setGeometry(faxqtwin->width() - 16,0,16,faxqtwin->height()-16);
    hsb->setGeometry(0,faxqtwin->height() - 16 ,faxqtwin->width()-16,16);

    qwindow_width = faxqtwin->width() -16;
    qwindow_height = faxqtwin->height()-16;


    vsb->raise();
    vsb->show();
    hsb->show();
  }
  else{

    if( PaneHeight  > qwindow_height){
      vsb->setGeometry(faxqtwin->width() - 16,0,16,faxqtwin->height());


      qwindow_width = faxqtwin->width() -16 ;
      qwindow_height = faxqtwin->height();


      vsb->show();
      hsb->hide();
      hsb->raise();
    }
     else
       vsb->hide();

    if( PaneWidth  > qwindow_width ){
      hsb->setGeometry(0,faxqtwin->height() - 16 ,faxqtwin->width(),16);
      hsb->show();
      hsb->raise();
      vsb->hide();
      qwindow_width = faxqtwin->width() ;
      qwindow_height = faxqtwin->height() -16;

    }
    else
      hsb->hide();

  }

  if(Image){
    hsb->setRange(0,QMAX(0,Image->width - qwindow_width));
    hsb->setSteps(5,qwindow_width/2);
    //    printf("hsb range: %d\n",QMAX(0,Image->width - qwindow_width));
    vsb->setRange(0,QMAX(0,Image->height - qwindow_height));
    vsb->setSteps(5,qwindow_height/2);
    //    printf("vsb range: %d\n",QMAX(0,Image->height - qwindow_height));
    //    printf("vsb QMIN %d vdb QMAX %d\n",vsb->QMINValue(),vsb->QMAXValue());
  }


  Resize = 1;
  uiUpdate();

}

bool TopLevel::loadAllPages( int &numpages, int &currentpage )
{
  struct pagenode *pn;

  numpages = 0;
  currentpage = 1;

  for(pn = firstpage; pn; pn = pn->next) {
    ++numpages;
    if (pn == thispage)
	currentpage = numpages;
    if (!Pimage(pn)) {
	int k = -1;
	while((k != 0) && (k != 3) && (k != 1))
		k = GetImage(pn); // fetch image if it is not available yet.
    }
  }
  return (numpages != 0);
}

void TopLevel::print(){
  if(!thispage || !firstpage) {
    return KMessageBox::sorry(this, i18n("There is no document active."));
  }

  int pages, currentpage;
  loadAllPages(pages, currentpage);

  KPrinter printer(true, QPrinter::HighResolution);
  printer.setFullPage( true );
  printer.setUsePrinterResolution( true );
  printer.setCreator( i18n("KFax") + " " KFAXVERSION );
  printer.setDocName( QString("%1 - %2").arg(firstpage->name).arg(i18n("KFax")));
  printer.setDocFileName( firstpage->name );
  printer.setPageSelection( KPrinter::ApplicationSide );
  printer.setMinMax( 1, pages ); 
  printer.setCurrentPage( currentpage );
  printer.addDialogPage(new KFAXPrintSettings());
  if ( !printer.setup( this ) )
	return;

  QPainter painter;
  painter.begin( &printer );
  printIt(printer, painter);
  painter.end();
}


void TopLevel::printIt( KPrinter &printer, QPainter &painter )
{
  QPaintDeviceMetrics dm(painter.device());

  QApplication::setOverrideCursor( waitCursor );
  kapp->processEvents();

  const bool fullpage = printer.option(APP_KFAX_SCALE_FULLPAGE) == "true";
  const bool center_h = printer.option(APP_KFAX_CENTER_HORZ) == "true";
  const bool center_v = printer.option(APP_KFAX_CENTER_VERT) == "true";

  int currentpage = 0;
  bool first_page_printed = false;
  struct pagenode *pn;
  for(pn = firstpage; pn; pn = pn->next) {

    ++currentpage;
    // should this page be printed ?
    if (printer.pageList().findIndex(currentpage) < 0)
	continue;

    XImage *Image = Pimage(pn);
    if (!Image)
	continue;

    // byte-swapping the image
    QByteArray bytes( Image->height*Image->bytes_per_line );
    for (int y=Image->height-1; y>=0; --y) {
      Q_UINT32 offset  = y*Image->bytes_per_line;
      Q_UINT32 *source = (Q_UINT32 *) (Image->data + offset);
      Q_UINT32 *dest   = (Q_UINT32 *) (bytes.data() + offset);
      for (int x=(Image->bytes_per_line/4)-1; x>=0; --x) {
 	Q_UINT32 dv = 0, sv = *source;
	for (int bit=32; bit>0; --bit) {
		dv <<= 1;
		dv |= sv&1;
		sv >>= 1;
	}
        *dest = dv;
	++dest;
	++source;
      }
    }

    QImage image( (uchar *)bytes.data(), Image->bytes_per_line*8, Image->height, 1, NULL, 2, QImage::LittleEndian);

    if (first_page_printed)
	printer.newPage();
    first_page_printed = true;

    const QSize printersize( dm.width(), dm.height() );
    kdDebug() << "Printersize = " << printersize << endl;
    // print Image in original size if possible, else scale it.

    const QSize size(  // logical size of the image
			Image->width  * dm.logicalDpiX() / pn->dpiX,
			Image->height * dm.logicalDpiY() / pn->dpiY
		);

    kdDebug()	<< "Org image size = " << Image->width << "x" << Image->height 
		<< " logical picture res = " << pn->dpiX << "x" << pn->dpiY << endl;
    kdDebug()	<< "New image size = " << size 
		<< " logical printer res = " << dm.logicalDpiX() << "x" << dm.logicalDpiY() << endl;

    uint top, left, bottom, right;
    if (fullpage)
       top = left = bottom = right = 0;
    else
       printer.margins( &top, &left, &bottom, &right );
    kdDebug() << "Margins = " << top << " " << left << " " << bottom << " " << right << endl;

    const QSize maxSize( printersize.width()-left-right, printersize.height()-top-bottom );
    QSize scaledImageSize = size;
    if (size.width() > maxSize.width() || size.height() > maxSize.height() ) {
	// Image does not fit - scale it and print centered
	scaledImageSize.scale( maxSize, QSize::ScaleMin );
	kdDebug() << "Image does not fit - scaling to " << maxSize << endl;
    } else {
	// Image does fit - print it in original size, but centered
	scaledImageSize.scale( size, QSize::ScaleMin );
	kdDebug() << "Image does fit - scaling to " << size << endl;
    }
    kdDebug() << "Final image size " << scaledImageSize << endl;
    int x,y;
    if (center_h)
	x = (maxSize.width()-scaledImageSize.width())/2 + left;
    else
	x = left;
    if (center_v)
	y = (maxSize.height()-scaledImageSize.height())/2 + top;
    else
	y = top;
    painter.drawImage( QRect(x,y,scaledImageSize.width(), scaledImageSize.height()), image );

  }

  QApplication::restoreOverrideCursor();
}

void TopLevel::saveNetFile( const KURL& dest)
{
    if ( !dest.isValid() )
    {
	KMessageBox::sorry(this, i18n("Malformed URL"));
	return;
    }

    statusbar->message( i18n( "Saving..." ) );
   
    KURL source = KURL::fromPathOrURL(thispage->pathname);
    bool ok = KIO::NetAccess::file_copy( source, dest, -1, true, false, this);

    statusbar->clear();

    if (!ok)
	  KMessageBox::error(this, i18n("Failure in 'copy file()'\n"
				"Could not save file!"));
}

void TopLevel::openNetFile( const KURL &u)
{
  if ( !u.isValid() )
  {
	KMessageBox::error(this, i18n("Malformed URL"));
	return;
  }

  if ( u.isLocalFile() )
  {
    QString string = i18n("Loading '%1'").arg(u.path());
    statusbar->message(string);
    openadd( u.path());
    statusbar->clear();
  }
  else
  {
    statusbar->message(i18n("Downloading..."));
    QString tmpFile = QString::null;
    if ( KIO::NetAccess::download( u, tmpFile, this ) )
    {
      openadd( tmpFile );
      setCaption( u.prettyURL() );
    }
    statusbar->clear();
    KIO::NetAccess::removeTempFile( tmpFile );
  }
}

void TopLevel::dragEnterEvent( QDragEnterEvent * event)
{
  event->accept(KURLDrag::canDecode(event));
}

void TopLevel::dropEvent( QDropEvent * event)
{

  KURL::List list;

  if (KURLDrag::decode(event, list) && !list.isEmpty())
  {
    // Load the first file in this window
    const KURL &url = list.first();
    openNetFile( url );
  }
}

void SetupDisplay(){

  if(display_is_setup){
    return;
  }

  display_is_setup = TRUE;

  xpos = ypos = ox = oy = 0;
  ExpectConfNotify = 1;

  /*  XSizeHints size_hints;*/

  Win = XCreateSimpleWindow(qtdisplay,qtwin,1,1,
			    1,1,
			    0,
			    BlackPixel(qtdisplay,XDefaultScreen(qtdisplay)),
			    WhitePixel(qtdisplay,XDefaultScreen(qtdisplay)));

  PaintGC = XCreateGC(qtdisplay, Win, 0L, (XGCValues *) NULL);
  XSetForeground(qtdisplay, PaintGC, BlackPixel(qtdisplay, XDefaultScreen(qtdisplay) ));
  XSetBackground(qtdisplay, PaintGC, WhitePixel(qtdisplay, XDefaultScreen(qtdisplay) ));
  XSetFunction(qtdisplay, PaintGC, GXcopy);
  WorkCursor = XCreateFontCursor(qtdisplay, XC_watch);
  //ReadyCursor = XCreateFontCursor(qtdisplay, XC_plus);
  ReadyCursor = XCreateFontCursor(qtdisplay, XC_hand2);
  MoveCursor = XCreateFontCursor(qtdisplay, XC_fleur);
  LRCursor = XCreateFontCursor(qtdisplay, XC_sb_h_double_arrow);
  UDCursor = XCreateFontCursor(qtdisplay, XC_sb_v_double_arrow);

  XSelectInput(qtdisplay, Win, Button2MotionMask | ButtonPressMask |
	       ButtonReleaseMask | ExposureMask | KeyPressMask |
	       SubstructureNotifyMask | LeaveWindowMask | OwnerGrabButtonMask |
	       StructureNotifyMask);

  XMapRaised(qtdisplay, Win);

  XDefineCursor(qtdisplay, Win, ReadyCursor);
  XFlush(qtdisplay);

  memset(Images, 0, sizeof(Images));

  // Start at half the Size
  oz = 1;
}

void TopLevel::handle_X_event(XEvent Event)
{
  if(!thispage || !Image || !faxqtwin || !display_is_setup)
    return;

  bool putimage = false; // Do we actually have to write the image to the scree?

  do {
    switch(Event.type) {
    case MappingNotify:
      XRefreshKeyboardMapping((XMappingEvent *)(&Event));
		break;

    case LeaveNotify:
      /*      buttondown = false;
	XDefineCursor(qtdisplay, Win, ReadyCursor);
	XFlush(qtdisplay);*/
      break;
    case Expose:
      {

	if(Event.xexpose.count != 0)
	  break;

	if(!Image)
	  break;

	putimage = TRUE;
      }
    break;

    case KeyPress:
      if (ExpectConfNotify &&
	  (Event.xkey.time < (Lasttime + PATIENCE)))
	break;
      Lasttime = Event.xkey.time;
      ExpectConfNotify = 0;
      switch(XKeycodeToKeysym(qtdisplay, Event.xkey.keycode, 0)) {
      case XK_m:
	mirrorPage();
	if (Event.xkey.state & ShiftMask)
	  TurnFollowing(TURN_M, thispage->next);
  	break;
      case XK_o:
	zoomout();
	break;

      case XK_i:
	zoomin();
	break;

      case XK_Up:
	ypos-= qwindow_height / 3;
	putimage = TRUE;
	break;
      case XK_Down:
	ypos+= qwindow_height / 3;
	putimage = TRUE;
	break;
      case XK_Left:
	xpos-= qwindow_width / 4;
	putimage = TRUE;
	break;
      case XK_Right:
	xpos+= qwindow_width / 4;
	putimage = TRUE;
	break;
      case XK_Home:
      case XK_R7:
	if (Event.xkey.state & ShiftMask) {
	  thispage = firstpage;
	  newPage();
	  resizeView();
	  putImage();
	  break;
	}
	xpos= 0;
	ypos= 0;
	putImage();
	break;
      case XK_End:
      case XK_R13:
	if (Event.xkey.state & ShiftMask) {
	  thispage = lastpage;
	  newPage();
	  resizeView();
	  putImage();
	  break;
	}
	xpos= Image->width;
	ypos= Image->height;
	putImage();
	break;
      case XK_l:
      case XK_r:
	rotatePage();
	if (Event.xkey.state & ShiftMask)
	  TurnFollowing(TURN_L, thispage->next);
	break;
      case XK_p:
      case XK_minus:
      case XK_Prior:
      case XK_R9:
      case XK_BackSpace:
	prevPage();
	break;
      case XK_n:
      case XK_plus:
      case XK_space:
      case XK_Next:
      case XK_R15:
	nextPage();
	break;
      case XK_u:
	flipPage();
	if (Event.xkey.state & ShiftMask)
	  TurnFollowing(TURN_U, thispage->next);
	break;

      case XK_q:
	if (viewpage) {
	  thispage = viewpage;
	  viewpage = NULL;
	  newPage();
	  resizeView();
	  putImage();
	}

      }

      break;

    case ButtonPress:

      if (ExpectConfNotify && (Event.xbutton.time < (Lasttime + PATIENCE)))
	break;

      Lasttime = Event.xbutton.time;
      ExpectConfNotify = 0;


      switch (Event.xbutton.button) {

      case Button1:
	buttondown = true;

	switch (((Image->width > qwindow_width)<<1) |
		(Image->height > qwindow_height)) {
	case 0:
	  break;
	case 1:
	  XDefineCursor(qtdisplay, Win, UDCursor);
	  break;
	case 2:
	  XDefineCursor(qtdisplay, Win, LRCursor);
	  break;
	case 3:
	  XDefineCursor(qtdisplay, Win, MoveCursor);
	}

	XFlush(qtdisplay);
		offx = Event.xbutton.x;
		offy = Event.xbutton.y;
	break;

      }

      break;

    case MotionNotify:
      if(!buttondown)
	break;
      do {

	nx = Event.xmotion.x;
	ny = Event.xmotion.y;


      } while (XCheckTypedEvent(qtdisplay, MotionNotify, &Event));


      xpos+= offx - nx;
      ypos+= offy - ny;

      offx = nx;
      offy = ny;

      putimage = TRUE;

      break;

    case ButtonRelease:

      if (Event.xbutton.button == Button1) {

	buttondown = false;
	XDefineCursor(qtdisplay, Win, ReadyCursor);
	XFlush(qtdisplay);
      }

    }

  } while (XCheckWindowEvent(qtdisplay, Win, KeyPressMask|ButtonPressMask, &Event));

  if(putimage == TRUE) {
    Refresh = Resize = 1;
    putImage();
  }
}

void TopLevel::rotatePage()
{
  if(!thispage || !Image || !faxqtwin || !display_is_setup)
    return;

  XDefineCursor(qtdisplay, Win, WorkCursor);
  XFlush(qtdisplay);

  XImage *newrotimage = RotImage(Images[0]);

  XDefineCursor(qtdisplay, Win, ReadyCursor);

  if(newrotimage == NULL){ // out of memory
    return;
  }

  thispage->extra = Images[0] = newrotimage;
  thispage->orient ^= TURN_L;

  freeImages();
  Image = generateZoomImages(oz);

  { int t = xpos ; xpos= ypos; ypos= t; }

  Refresh = Resize = 1;

  putImage();
}

void TopLevel::flipPage()
{
  if(!thispage || !Image || !faxqtwin || !display_is_setup)
    return;

  XDefineCursor(qtdisplay, Win, WorkCursor);
  XFlush(qtdisplay);

  XImage *newflipimage = FlipImage(Images[0]);

  XDefineCursor(qtdisplay, Win, ReadyCursor);

  if(newflipimage == NULL){ // out of memory
    return;
  }

  thispage->extra = Images[0] = newflipimage;
  thispage->orient ^= TURN_U;

  freeImages();
  Image = generateZoomImages(oz);

  Refresh = Resize = 1;
  putImage();
}

void TopLevel::mirrorPage()
{
  if(!thispage || !Image || !faxqtwin || !display_is_setup)
    return;

  XDefineCursor(qtdisplay, Win, WorkCursor);
  XFlush(qtdisplay);

  XImage *newmirror = MirrorImage(Images[0]);

  XDefineCursor(qtdisplay, Win, ReadyCursor);

  if(newmirror == NULL){ // out of memory
    return;
  }
  thispage->extra = Images[0] = newmirror;
  thispage->orient ^= TURN_M;

  freeImages();
  Image = generateZoomImages(oz);

  Refresh = Resize = 1;
  putImage();
}

void TopLevel::scrollHorz(int){

  if(!Image)
    return;

  //  printf("hsb value: %d\n",hsb->value());
  xpos=  hsb->value() + qwindow_width/2;

  Refresh = 1;
  putImage();
}

void TopLevel::scrollVert(int ){

  if(!Image)
    return;

  //  printf("vsb value: %d\n",vsb->value());
  ypos=  vsb->value() + qwindow_height/2;

  Refresh = 1;
  putImage();
}

void TopLevel::lastPage()
{
  if(!thispage)
    return;

  if ( thispage->next )
  {
    while(thispage->next != NULL)
      thispage = thispage->next;

    newPage();
    resizeView();
    putImage();
  }

  updateGoActions();
}

void TopLevel::firstPage()
{
  if(!thispage)
    return;

  if ( thispage->prev )
  {
    while(thispage->prev != NULL)
      thispage = thispage->prev;

    newPage();
    resizeView();
    putImage();
  }

  updateGoActions();
}

void TopLevel::nextPage()
{
  if(!thispage)
    return;

  if (thispage->next)
  {
    thispage = thispage->next;

    newPage();
    resizeView();
    putImage();
  }

  updateGoActions();
}

void TopLevel::prevPage()
{
  if(!thispage)
    return;

  if (thispage->prev)
  {
    thispage = thispage->prev;

    newPage();
    resizeView();
    putImage();
  }

  updateGoActions();
}

void TopLevel::newPage(){

  if(!display_is_setup)
    SetupDisplay();

  XDefineCursor(qtdisplay, Win, WorkCursor);
  XFlush(qtdisplay);

  freeImages();

  int k = -1;

  if(!thispage) {
    XDefineCursor(qtdisplay, Win, ReadyCursor);
    return;
  }

  if (Pimage(thispage) == NULL){

    while((k != 0) && (k != 3) && (k !=1))
      k = GetImage(thispage);

  }

  if (k == 3 ){

    XDefineCursor(qtdisplay, Win, ReadyCursor);
    FreeFax();
    /*    KMessageBox::sorry(i18n("Bad fax file k=3"));*/
    return;
  }

  if (k == 0 ){

    XDefineCursor(qtdisplay, Win, ReadyCursor);
    FreeFax();
    /*    KMessageBox::sorry(i18n("Bad fax file k=0"));*/
    return;
 }

  Image =  Images[0] = Pimage(thispage);

  setCaption(QFile::decodeName(thispage->name));

  Image = generateZoomImages(oz);
  
  PaneWidth = Image->width;
  PaneHeight = Image->height;
  Refresh = 1;

  XDefineCursor(qtdisplay, Win, ReadyCursor);
  uiUpdate();

}


void TopLevel::faxClose()
{
  FreeFax();

  setCaption(i18n("KFax"));
  // TODO replace this with unmapping the window.
  if(display_is_setup)
    XResizeWindow(qtdisplay,Win,1,1); // we want a clear gray background.

  resizeView();
  vsb->hide();
  hsb->hide();

  fileURL = QString::null;

  updateActions();
}

void TopLevel::FreeFax()
{
  if(display_is_setup)
    XClearWindow(qtdisplay, Win);

  freeImages();

  pagenode *pn;
  for (pn = firstpage; pn; pn = pn->next){
    if(Pimage(pn)){
       FreeImage(Pimage(pn));
       pn->extra = NULL;
    }
  }

  Image = NULL;

  for (pn = firstpage; pn; pn = pn->next){
    if(pn->pathname){
       free(pn->pathname);
    }
  }


  if(firstpage){
    for(pn = firstpage->next; pn; pn = pn->next){
      if(pn->prev){
	free(pn->prev);
      }
    }
  }

  if(lastpage)
    free(lastpage);

  firstpage = lastpage = viewpage = thispage = auxpage = NULL;

  uiUpdate();
}

void TopLevel::uiUpdate(){

  if(thispage){

    struct pagenode *pn ;
    int pages = 0;
    int currentpage = 0;

    for(pn = firstpage; pn ; pn = pn->next){
      pages ++;
      if (thispage == pn)
	currentpage = pages;
    }

    QString pagestr = i18n("Page: %1 of %2").arg(currentpage).arg(pages);

    statusbar->changeItem(pagestr, ID_PAGE_NO);

    if(Image){
      QString wh = i18n("W: %1 H: %2").arg(Image->width).arg(Image->height);
      statusbar->changeItem(wh, ID_INS_OVR);
    }

    QString resolution = i18n("Res: %1").arg(thispage->vres?i18n("Fine"):i18n("Normal"));
    // TODO: resolution += QString("%1x%2").arg(thispage->dpiX).arg(thispage->dpiY);
    statusbar->changeItem(resolution, ID_GENERAL);

    statusbar->changeItem(thispage->name, ID_FNAME);

    QString typestring;

    if(thispage->type == FAX_TIFF){
      typestring = i18n("Type: Tiff ");
    }
    else if ( thispage->type == FAX_RAW){
      typestring = i18n("Type: Raw ");
    }

    if ( thispage->expander == g31expand )
      typestring += "G3";

    if ( thispage->expander == g32expand )
      typestring += "G3 2D";

    if ( thispage->expander == g4expand )
      typestring += "G4";

    statusbar->changeItem(typestring,ID_TYPE);
    updateActions();
  }
}

void kfaxerror(const QString& title, const QString& error){
  KMessageBox::error(toplevel, error, title);
}

void TopLevel::putImage()
{

  // TODO do I really need to set Refresh or Resize to 1 , is there
  // really still someonce calling this with out haveing set Refresh or Resize to 1?

  if ( !Image || !display_is_setup || !thispage )
    return;

  if ( qwindow_width > Image->width){
	  xpos= Image->width/2;
  }
  else{
    if(xpos< qwindow_width/2){
      xpos = qwindow_width/2;
    }
    else{
      if(xpos> Image->width - qwindow_width/2){
	xpos= Image->width - qwindow_width/2;
      }

    }
  }

  if ( qwindow_height > Image->height){
    ypos= Image->height/2;
  }
  else{
    if(ypos< qwindow_height/2){
      ypos = qwindow_height/2;
    }
    else{
      if(ypos> Image->height - qwindow_height/2){
	ypos= Image->height - qwindow_height/2;
      }

    }
  }

  if (xpos!= ox || ypos!= oy || Refresh || Resize){

    /* In the following we use qwindow_height -1 etc since the main view
       has a sunken frame and I need to offset by 1 pixel to the right and
       one pixel down so that I don't paint my fax into the border of the frame*/

    XResizeWindow(qtdisplay,Win,QMIN(qwindow_width -1,Image->width ),
		  QMIN(qwindow_height -1,Image->height ));

    XPutImage(qtdisplay, Win, PaintGC, Image,
	      QMAX(xpos - qwindow_width/2,0), QMAX(ypos - qwindow_height/2,0),
	      0, 0, QMIN(qwindow_width -1,Image->width)  ,
	      QMIN(qwindow_height -1,Image->height) );

    vsb->setValue(QMAX(ypos - qwindow_height/2,0));
    hsb->setValue(QMAX(xpos - qwindow_width/2,0));

    XFlush(qtdisplay);
  }

  ox = xpos;
  oy = ypos;

  Resize = Refresh = 0;

}

void TopLevel::faxoptions(){

 OptionsDialog * opd = new OptionsDialog(this, "options");
 opd->setWidgets(&fop);

 if(opd->exec()){

   struct optionsinfo *newops;
   newops = opd->getInfo();

   fop.resauto 	=  newops->resauto;
   fop.geomauto	=  newops->geomauto;
   fop.width 	=  newops->width;
   fop.height 	=  newops->height;
   fop.fine 	=  newops->fine;
   fop.landscape=  newops->landscape;
   fop.flip 	=  newops->flip;
   fop.invert 	=  newops->invert;
   fop.lsbfirst =  newops->lsbfirst;
   fop.raw 	=  newops->raw;

   setFaxDefaults();

   writeSettings();
 }

 delete opd;
}

void setFaxDefaults(){

  // fop is called in readSettings, so this can't be
  // called after a TopLevel::readSettings()

  if(have_cmd_opt ) // we have commad line options all kfaxrc defaults are
    return;         // overridden

  if(fop.resauto == 1)
    defaultpage.vres = -1;
  else
    defaultpage.vres = fop.fine;

  if(fop.geomauto == 1){
    defaultpage.width = defaultpage.height = 0;
  }
  else{
    defaultpage.width  = fop.width;
    defaultpage.height = fop.height;
  }

  if(fop.landscape)
    defaultpage.orient |= TURN_L;

  if(fop.flip)
     defaultpage.orient |= TURN_U;

  defaultpage.inverse  = fop.invert;
  defaultpage.lsbfirst = fop.lsbfirst;

  switch (fop.raw) {
	case 2:	defaultpage.expander = g32expand;
		break;
	case 4: defaultpage.expander = g4expand;
		break;
	default:defaultpage.expander = g31expand;
  }

}

static const char description[] =
        I18N_NOOP("KDE G3/G4 Fax Viewer");

static KCmdLineOptions options[] =
{
   {"f", 0, 0 },
   {"fine",         I18N_NOOP( "Fine resolution" ), 0 },
   {"n", 0, 0 },
   {"normal",       I18N_NOOP( "Normal resolution" ), 0 },
   {"height",       I18N_NOOP( "Height (number of fax lines)" ), 0 },
   {"w", 0, 0 },
   {"width",        I18N_NOOP( "Width (dots per fax line)" ), 0 },
   {"l", 0, 0 },
   {"landscape",    I18N_NOOP( "Turn image 90 degrees (landscape mode)" ), 0 },
   {"u", 0, 0 },
   {"upsidedown",   I18N_NOOP( "Turn image upside down" ), 0 },
   {"i", 0, 0 },
   {"invert",       I18N_NOOP( "Invert black and white" ), 0 },
   {"m", 0, 0 },
   {"mem <bytes>",  I18N_NOOP( "Limit memory use to 'bytes'" ), "8M" },
   {"r", 0, 0 },
   {"reverse",      I18N_NOOP( "Fax data is packed lsb first" ), 0 },
   {"2" ,           I18N_NOOP( "Raw files are g3-2d" ), 0 },
   {"4",            I18N_NOOP( "Raw files are g4" ), 0 },
   {"+file(s)",     I18N_NOOP( "Fax file(s) to show" ), 0 },
   KCmdLineLastOption
};

int main (int argc, char **argv)
{
  KAboutData aboutData( "kfax", I18N_NOOP("KFax"),
      KFAXVERSION, description, KAboutData::License_GPL,
      "(c) 1997-98 Bernd Johannes Wuebben");
  aboutData.addAuthor( "Bernd Johannes Wuebben", 0, "wuebben@kde.org" );
  aboutData.addCredit( "Nadeem Hasan", I18N_NOOP( "UI Rewrite, lots of code "
      "cleanups and fixes" ), "nhasan@kde.org" );
  aboutData.addCredit( "Helge Deller", I18N_NOOP( "Printing Rewrite, lots of code "
      "cleanups and fixes"), "deller@kde.org" );

  KCmdLineArgs::init(argc, argv, &aboutData);
  KCmdLineArgs::addCmdLineOptions( options );

  MyApp a;

  qtdisplay = qt_xdisplay();

  viewfaxmain();

  toplevel = new TopLevel();
  toplevel->show();

  startingup = 1;
  a.processEvents();
  a.flushX();

  startingup = 0;

  faxinit();
  if(!have_no_fax){

    thispage = firstpage;

    toplevel->newPage();
    toplevel->resizeView();
    //TODO : I don't think I need this putImage();
    toplevel->putImage();
  }

  toplevel->uiUpdate();

  return a.exec ();
}


#include "kfax.moc"
