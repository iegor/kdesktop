/***************************************************************************
 *   Copyright (C) 2005 by Stefan Kebekus                                  *
 *   kebekus@kde.org                                                       *
 *                                                                         *
 *   Copyright (C) 2005 by Wilfried Huss                                   *
 *   Wilfried.Huss@gmx.at                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <kmessagebox.h>
#include <kdebug.h>
#include <klocale.h>
#include <qfileinfo.h>
#include <qimage.h>
#include <qpainter.h>
#include <kapp.h>

#include "GBitmap.h"
#include "BSByteStream.h"
#include "IFFByteStream.h"

#include "prefs.h"

#include "documentWidget.h"
#include "djvurenderer.h"
#include "djvumultipage.h"
#include "hyperlink.h"
#include "renderedDocumentPagePixmap.h"
#include "textBox.h"

//#define KF_DEBUG

inline GUTF8String GStringFromQString(const QString& x)
{
  GUTF8String retval=(const char*)x.utf8();
  return retval;
}


inline QString QStringFromGString(const GUTF8String& x)
{
  QString retval=QString::fromUtf8((const char*)x);
  return retval;
}


DjVuRenderer::DjVuRenderer(QWidget* par)
  : DocumentRenderer(par)
{
#ifdef KF_DEBUG
  kdError() << "DjVuRenderer( parent=" << par << " )" << endl;
#endif

  PPMstream = ByteStream::create();
}



DjVuRenderer::~DjVuRenderer()
{
#ifdef KF_DEBUG
  kdDebug() << "~DjVuRenderer" << endl;
#endif

  // Wait for all access to this documentRenderer to finish
  QMutexLocker locker( &mutex );
}


void DjVuRenderer::drawPage(double resolution, RenderedDocumentPage* page)
{
#ifdef KF_DEBUG
  kdDebug() << "DjVuRenderer::drawPage(documentPage*) called, page number " << page->getPageNumber() << endl;
#endif

  // Paranoid safety checks
  if (page == 0) {
    kdError() << "DjVuRenderer::drawPage(documentPage*) called with argument == 0" << endl;
    return;
  }
  if (page->getPageNumber() == 0) {
    kdError() << "DjVuRenderer::drawPage(documentPage*) called for a documentPage with page number 0" << endl;
    return;
  }

  // Wait for all access to this documentRenderer to finish
  QMutexLocker locker( &mutex );

  // more paranoid safety checks
  if (page->getPageNumber() > numPages) {
    kdError() << "DjVuRenderer::drawPage(documentPage*) called for a documentPage with page number " << page->getPageNumber()
	      << " but the current fax file has only " << numPages << " pages." << endl;
    return;
  }

  int pageNumber = page->getPageNumber() - 1;

  GP<DjVuImage> djvuPage = document->get_page(pageNumber, true);
  if (!djvuPage->wait_for_complete_decode())
  {
    kdDebug() << "decoding failed." << endl;
    return;
  }

  if (!pageSizes[pageNumber].isValid())
  {
    int djvuResolution = djvuPage->get_dpi();
    int djvuPageWidth = djvuPage->get_width();
    int djvuPageHeight = djvuPage->get_height();

    Length w, h;
    w.setLength_in_inch(djvuPageWidth / (double)djvuResolution);
    h.setLength_in_inch(djvuPageHeight / (double)djvuResolution);
    pageSizes[pageNumber].setPageSize(w, h);

    SimplePageSize ps = sizeOfPage(page->getPageNumber());

    // If we are not printing we need to resize the pixmap.
    RenderedDocumentPagePixmap* pagePixmap = dynamic_cast<RenderedDocumentPagePixmap*>(page);
    if (pagePixmap)
      pagePixmap->resize(ps.sizeInPixel(resolution));
  }
  
  //kdDebug() << "render page " << pageNumber + 1 << " at size (" << pageWidth << ", " << pageHeight << ")" << endl;
  
  int pageHeight = page->height();
  int pageWidth = page->width();
  
  GRect pageRect(0, 0, pageWidth, pageHeight);


  GP<GPixmap> djvuPixmap;
  if (Prefs::renderMode() == Prefs::EnumRenderMode::Color)
    djvuPixmap = djvuPage->get_pixmap(pageRect, pageRect);
  else if (Prefs::renderMode() == Prefs::EnumRenderMode::Foreground)
    djvuPixmap = djvuPage->get_fg_pixmap(pageRect, pageRect);
  else if (Prefs::renderMode() == Prefs::EnumRenderMode::Background)
    djvuPixmap = djvuPage->get_bg_pixmap(pageRect, pageRect);

  QPainter* foreGroundPaint = page->getPainter();
  if (foreGroundPaint != 0) 
  {
    if(djvuPixmap && Prefs::renderMode() != Prefs::EnumRenderMode::BlackAndWhite)
    {
      PPMstream->seek(0);
      djvuPixmap->save_ppm(*PPMstream);
      long pixmapsize = PPMstream->tell();
      PPMstream->seek(0);
      uchar* buf = new uchar[pixmapsize];
      long bytesRead = PPMstream->readall(buf, pixmapsize);

      bool ok = pixmap.loadFromData(buf, bytesRead, "PPM");
      if (!ok)
      {
        kdError() << "loading failed" << endl;
        //draw an empty page
        foreGroundPaint->fillRect(0, 0, pageWidth, pageHeight, Qt::white);
      }
      foreGroundPaint->drawPixmap(0, 0, pixmap);
      delete[] buf;

/*      for (int i = 0; i < pageHeight; i++)
      {
        GPixel* pixmapRow = (*djvuPixmap)[i];

        for (int j = 0; j < pageWidth; j++)
        {
          GPixel pixel = pixmapRow[j];
          foreGroundPaint->setPen(QColor(pixel.r, pixel.g, pixel.b));
          foreGroundPaint->drawPoint(j, pageHeight - i - 1);
        }
      }*/
    }
    else
    {
      GP<GBitmap> djvuBitmap = djvuPage->get_bitmap(pageRect, pageRect);
      if(djvuBitmap)
      {
        PPMstream->seek(0);
        if(djvuBitmap->get_grays() == 2)
          djvuBitmap->save_pbm(*PPMstream);
        else
          djvuBitmap->save_pgm(*PPMstream);

        long pixmapsize = PPMstream->tell();
        PPMstream->seek(0);
        uchar* buf = new uchar[pixmapsize];
        long bytesRead = PPMstream->readall(buf, pixmapsize);

        bool ok = pixmap.loadFromData(buf, bytesRead, "PPM");
        if (!ok)
        {
          kdError() << "loading failed" << endl;
          //draw an empty page
          foreGroundPaint->fillRect(0, 0, pageWidth, pageHeight, Qt::white);
        }
        foreGroundPaint->drawPixmap(0, 0, pixmap);
        delete[] buf;
/*
        for (int i = 0; i < pageHeight; i++)
        {
          unsigned char* bitmapRow = (*djvuBitmap)[i];
          for (int j = 0; j < pageWidth; j++)
          {
            unsigned char pixel = 255-bitmapRow[j];
            foreGroundPaint->setPen(QColor(pixel, pixel, pixel));
            foreGroundPaint->drawPoint(j, pageHeight - i - 1);
          }
        }*/
      }
      else
      {
        //draw an empty page
        foreGroundPaint->fillRect(0, 0, pageWidth, pageHeight, Qt::white);
      }
    }

    //kdDebug() << "rendering page " << pageNumber + 1 << " at size (" << pageWidth << ", " << pageHeight << ") finished." << endl;
    page->returnPainter(foreGroundPaint);
  }

  GP<DjVuTXT> pageText = getText(pageNumber);

  if (pageText)
  {
    QSize djvuPageSize(djvuPage->get_width(), djvuPage->get_real_height());
    fillInText(page, pageText, pageText->page_zone, djvuPageSize);
    //kdDebug() << "Text of page " << pageNumber << endl;
    //kdDebug() << (const char*)pageText->textUTF8 << endl;
  }

  getAnnotations(page, djvuPage);

  page->isEmpty = false;
}


bool DjVuRenderer::setFile(const QString &fname, const KURL &)
{
#ifdef KF_DEBUG
  kdDebug() << "DjVuRenderer::setFile(" << fname << ") called" << endl;
#endif

  // Wait for all access to this documentRenderer to finish
  QMutexLocker locker( &mutex );

  // If fname is the empty string, then this means: "close".
  if (fname.isEmpty()) {
    kdDebug() << "DjVuRenderer::setFile( ... ) called with empty filename. Closing the file." << endl;
    return true;
  }

  // Paranoid saftey checks: make sure the file actually exists, and
  // that it is a file, not a directory. Otherwise, show an error
  // message and exit..
  QFileInfo fi(fname);
  QString   filename = fi.absFilePath();
  if (!fi.exists() || fi.isDir()) {
    KMessageBox::error( parentWidget,
			i18n("<qt><strong>File error.</strong> The specified file '%1' does not exist.</qt>").arg(filename),
			i18n("File Error"));
    // the return value 'false' indicates that this operation was not successful.
    return false;
  }

  // Clear previously loaded document
  clear();

  // Now we assume that the file is fine and load the file.
  G_TRY {
    document = DjVuDocEditor::create_wait(GURL::Filename::UTF8(GStringFromQString(filename)));
  }
  G_CATCH(ex) {
    ;
  }
  G_ENDCATCH;

  // If the above assumption was false.
  if (!document)
  {
    KMessageBox::error( parentWidget,
      i18n("<qt><strong>File error.</strong> The specified file '%1' could not be loaded.</qt>").arg(filename),
      i18n("File Error"));

    clear();
    kdDebug(1223) << "Loading of document failed." << endl;
    return false;
  }

  bool r = initializeDocument();

  // the return value 'true' indicates that this operation was successful.
  return r;
}

void DjVuRenderer::getAnnotations(RenderedDocumentPage* page, GP<DjVuImage> djvuPage)
{
  GP<ByteStream> annotations = djvuPage->get_anno();
  if (!(annotations && annotations->size()))
    return;

  GP<DjVuANT> ant = DjVuANT::create();

  GP<IFFByteStream> iff = IFFByteStream::create(annotations);

  GUTF8String chkid;

  while (iff->get_chunk(chkid))
  {
    if (chkid == "ANTa")
    {
      ant->merge(*iff->get_bytestream());
    }
    else if (chkid == "ANTz")
    {
      GP<ByteStream> bsiff = BSByteStream::create(iff->get_bytestream());
      ant->merge(*bsiff);
    }
    iff->close_chunk();
  }

  if (!ant->is_empty())
  {
    // Scaling factors for the coordinate conversion.
    // TODO: Refractor this into a function shared with fillInText.
    int pageWidth = page->width();
    int pageHeight = page->height();

    double scaleX = pageWidth / (double)djvuPage->get_width();
    double scaleY = pageHeight / (double)djvuPage->get_height();

    GPList<GMapArea> map = ant->map_areas;

    for (GPosition pos = map; pos; ++pos)
    {
      // Currently we only support rectangular links
      if (!map[pos]->get_shape_type() == GMapArea::RECT)
        continue;

      GRect rect = map[pos]->get_bound_rect();

      QRect hyperlinkRect((int)(rect.xmin*scaleX+0.5), (int)((djvuPage->get_height()-rect.ymax)*scaleY+0.5),
                          (int)(rect.width()*scaleX +0.5), (int)(rect.height()*scaleY+0.5));

      QString url((const char*)map[pos]->url);
      QString target((const char*)map[pos]->target);
      QString comment((const char*)map[pos]->comment);

      // Create an anchor for this link.
      if (!anchorList.contains(url))
	{
	  // For now we only accept links to pages in the same document.
	  if(url[0] == '#' && target == "_self")
	    {
	      bool conversionOk;
	      PageNumber targetPage = url.remove('#').toInt(&conversionOk);
	      if (conversionOk)
		anchorList[url] = Anchor(targetPage, Length());
	    }
	}
      
      Hyperlink hyperlink(hyperlinkRect.bottom(), hyperlinkRect, url);
      page->hyperLinkList.push_back(hyperlink);
    }
  }
}


bool DjVuRenderer::initializeDocument()
{
  if (document == 0)
    return false;

  if (!document->wait_for_complete_init()) {
    kdDebug() << "Document Initialization failed." << endl;
    return false;
  }
  
  // Set the number of pages page sizes
  numPages = document->get_pages_num();

  pageSizes.resize(numPages);
  Length w,h;

  // Set the page sizes in the pageSizes array. Give feedback for
  // very long documents
  if (numPages > 100)
    emit setStatusBarText(i18n("Loading file. Computing page sizes..."));
  for(Q_UINT16 i=0; i<numPages; i++) {
    // Keep the GUI updated
    if (i%100 == 0)
      kapp->processEvents();
    
    GP<DjVuFile>  djvuFile = document->get_djvu_file(i);
    int resolution;
    int pageWidth;
    int pageHeight;
    bool ok = getPageInfo(djvuFile, pageWidth, pageHeight, resolution);
    if (!ok)
      kdError() << "Decoding info of page " << i << " failed." << endl;
    else {
      w.setLength_in_inch(pageWidth / (double)resolution);
      h.setLength_in_inch(pageHeight / (double)resolution);
      pageSizes[i].setPageSize(w, h);
    }
  }
  emit setStatusBarText(QString::null);
  
  // We will also generate a list of hyperlink-anchors in the document.
  // So declare the existing lists empty.
  anchorList.clear();
  return true;
}


GP<DjVuTXT> DjVuRenderer::getText(PageNumber pageNumber)
{
  GUTF8String chkid;

  const GP<DjVuFile> file = document->get_djvu_file(pageNumber);
  const GP<ByteStream> bs(file->get_text());
  if (bs)
  {
    long int i=0;
    const GP<IFFByteStream> iff(IFFByteStream::create(bs));
    while (iff->get_chunk(chkid))
    {
      i++;
      if (chkid == GUTF8String("TXTa"))
      {
        GP<DjVuTXT> txt = DjVuTXT::create();
        txt->decode(iff->get_bytestream());
        return txt;
      }
      else if (chkid == GUTF8String("TXTz"))
      {
        GP<DjVuTXT> txt = DjVuTXT::create();
        GP<ByteStream> bsiff=BSByteStream::create(iff->get_bytestream());
        txt->decode(bsiff);
        return txt;
      }
      iff->close_chunk();
    }
  }
  return 0;
}

void DjVuRenderer::fillInText(RenderedDocumentPage* page, const GP<DjVuTXT>& text, DjVuTXT::Zone& zone, QSize& djvuPageSize)
{
  if (zone.children.isempty())
  {
    int pageWidth = page->width();
    int pageHeight = page->height();

    double scaleX = pageWidth / (double)djvuPageSize.width();
    double scaleY = pageHeight / (double)djvuPageSize.height();

    QString zoneString = QStringFromGString(text->textUTF8.substr(zone.text_start, zone.text_length));

    //kdDebug() << "zone text: " << zoneString << endl;

    QRect textRect((int)(zone.rect.xmin*scaleX+0.5), (int)((djvuPageSize.height()-zone.rect.ymax)*scaleY+0.5),
                   (int)(zone.rect.width()*scaleX+0.5), (int)(zone.rect.height()*scaleY+0.5));
    //kdDebug() << "zone rect: " << textRect.x() << ", " << textRect.y() << ", " << textRect.width() << ", " << textRect.height() << endl;
    TextBox textBox(textRect, zoneString);
    page->textBoxList.push_back(textBox);
  }
  else
  {
    for (GPosition pos=zone.children; pos; ++pos)
    {
      fillInText(page, text, zone.children[pos], djvuPageSize);
    }
  }
}

bool DjVuRenderer::getPageInfo(GP<DjVuFile> file, int& width, int& height, int& dpi)
{
  if (!file || !file->is_all_data_present())
    return false;

  const GP<ByteStream> pbs(file->get_djvu_bytestream(false, false));
  const GP<IFFByteStream> iff(IFFByteStream::create(pbs));

  GUTF8String chkid;
  if (iff->get_chunk(chkid))
  {
    if (chkid == "FORM:DJVU")
    {
      while (iff->get_chunk(chkid) && chkid!="INFO")
        iff->close_chunk();
      if (chkid == "INFO")
      {
        GP<ByteStream> gbs = iff->get_bytestream();
        GP<DjVuInfo> info=DjVuInfo::create();
        info->decode(*gbs);
        int rot = ((360-GRect::findangle(info->orientation))/90)%4;

        width = (rot&1) ? info->height : info->width;
        height = (rot&1) ? info->width : info->height;
        dpi = info->dpi;
        return true;
      }
    }
    else if (chkid == "FORM:BM44" || chkid == "FORM:PM44")
    {
      while (iff->get_chunk(chkid) && chkid!="BM44" && chkid!="PM44")
        iff->close_chunk();
      if (chkid=="BM44" || chkid=="PM44")
      {
        GP<ByteStream> gbs = iff->get_bytestream();
        if (gbs->read8() == 0)
        {
          gbs->read8();
          gbs->read8();
          unsigned char xhi = gbs->read8();
          unsigned char xlo = gbs->read8();
          unsigned char yhi = gbs->read8();
          unsigned char ylo = gbs->read8();

          width = (xhi<<8)+xlo;
          height = (yhi<<8)+ylo;
          dpi = 100;
          return true;
        }
      }
    }
  }
  return false;
}

void DjVuRenderer::getText(RenderedDocumentPage* page)
{
  QMutexLocker locker( &mutex );
  
  int pageNumber = page->getPageNumber() - 1;
  GP<DjVuTXT> pageText = getText(pageNumber);

  if (pageText)
  {
    GP<DjVuFile> djvuFile = document->get_djvu_file(pageNumber);
    int resolution;
    int pageWidth;
    int pageHeight;
    bool ok = getPageInfo(djvuFile, pageWidth, pageHeight, resolution);

    if (ok)
    {
      QSize djvuPageSize(pageWidth, pageHeight);
      fillInText(page, pageText, pageText->page_zone, djvuPageSize);
    }
  }
}


bool DjVuRenderer::convertToPSFile( DjVuToPS &converter, QString filename, QValueList<int> &pageList )
{
  if (document == 0) {
    kdError(1223) << "DjVuRenderer::convertToPSFile(..) called when document was 0" << endl;
    return false;
  }

  QMutexLocker locker( &mutex );
  
  // Set up progress dialog
  KProgressDialog *pdialog = new KProgressDialog(parentWidget, "Printing-ProgressDialog", i18n("Printing..."), i18n("Preparing pages for printing..."), true);
  pdialog->setButtonText(i18n("Abort"));
  pdialog->showCancelButton(true);
  pdialog->progressBar()->setTotalSteps(pageList.size());
  pdialog->progressBar()->setFormat(QString::null);
  
  // Open output file
  GURL outname = GURL::Filename::UTF8(GStringFromQString(filename));
  GP<ByteStream> obs = ByteStream::create(outname, "w");
  
  QString pagename;
  QValueList<int>::ConstIterator it = pageList.begin();
  while (true) {
    pagename += QString::number(*it);
    ++it;
    if (it == pageList.end())
      break;
    pagename += ",";
  }
  GUTF8String pages = GStringFromQString(pagename);
  
  converter.set_info_cb(printerInfoCallBack, (void*)pdialog);
  bool iscancelled = false;
  G_TRY {
    converter.print(*obs, (DjVuDocument *)document, pages );	
  }
  G_CATCH(ex) {
    iscancelled = true;
  }
  G_ENDCATCH;
  
  delete pdialog;

  //  This is to keep the GUI updated.
  kapp->processEvents();
  
  obs->flush();
  return !iscancelled;
}


void DjVuRenderer::deletePages(Q_UINT16 from, Q_UINT16 to)
{
  // Paranoia security checks
  if (document == 0) {
    kdError(1223) << "DjVuRenderer::deletePages(...) called when no document was loaded" << endl;
    return;
  }
  if ((from > to) || (from == 0) || (from > totalPages()) || (to >  totalPages())) {
    kdError(1223) << "DjVuRenderer::deletePages(...) called with invalid arguments" << endl;
    return;
  }

  QMutexLocker locker( &mutex );

  KProgressDialog *pdialog = 0;
  if (to-from > 9) {
    pdialog = new KProgressDialog(parentWidget, "Printing-ProgressDialog", i18n("Deleting pages..."), i18n("Please wait while pages are removed..."), true);
    pdialog->showCancelButton(false);
    pdialog->progressBar()->setTotalSteps(to-from+1);
    pdialog->progressBar()->setFormat(QString::null);
    pdialog->show();
    kapp->processEvents();
  }
  
  // set the document pointer temporarily to 0, so that no-one tries
  // to render a page while we are deleting pages
  GP<DjVuDocEditor> document_new = document;
  document = 0;

  // Delete pages
  if (pdialog == 0) {
    GList<int> pageList;
    for(Q_UINT16 i=from; i<= to; i++)
      pageList.append(i-1);
    document_new->remove_pages(pageList); 
  } else {
    for(Q_UINT16 i=from; i<=to; i++) {
      document_new->remove_page(from-1); 
      pdialog->progressBar()->setProgress(i-from);
      pdialog->progressBar()->setFormat(i18n("deleting page %1").arg(i));
      kapp->processEvents();
    }
    delete pdialog;
  }
  _isModified = true;
  document = document_new;

  initializeDocument();
}


bool DjVuRenderer::save(const QString &filename)
{
  if (document == 0) {
    kdError() << "DjVuRenderer::save(..) called when document==0" << endl;
    return false;
  }
  
  QMutexLocker locker( &mutex );
  
  G_TRY {
    document->save_as(GURL::Filename::UTF8(GStringFromQString(filename)), true);
  }
  G_CATCH(ex) {
    return false;
  }
  G_ENDCATCH;

  document->save_as(GURL::Filename::UTF8(filename.ascii()), true);
  
  if (QFile::exists(filename) == false)
    return false;
  
  _isModified = false;
  return true;
}


void DjVuRenderer::printerInfoCallBack(int page_num, int page_count, int, DjVuToPS::Stage, void *pd)
{
  if (pd == 0)
    return;
  
  // Update the progress dialog.
  KProgressDialog *pdialog = (KProgressDialog *)pd;
  
  pdialog->progressBar()->setProgress(page_count);
  pdialog->progressBar()->setFormat(i18n("processing page %1").arg(page_num+1));
  pdialog->show();
  
  if (pdialog->wasCancelled())
    G_THROW("STOP");
  
  //  This is to keep the GUI updated.
  kapp->processEvents();
}


#include "djvurenderer.moc"
