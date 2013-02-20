//
// Class: documentPageCache
//
// Cache that holds a number of pages in a document.
// Part of KDVI- A previewer for TeX DVI files.
//
// (C) 2004 Stefan Kebekus. Distributed under the GPL.

#include <config.h>

#include <kdebug.h>
#include <qapplication.h>

#include "documentPageCache.h"
#include "documentRenderer.h"
#include "kvsprefs.h"
#include "renderedDocumentPagePixmap.h"


//#define documentPageCache_DEBUG


DocumentPageCache::DocumentPageCache()
  : maxMemory(2*16777216), LRUCache(maxMemory, 200)
{
  LRUCache.setAutoDelete(true);

  resolutionInDPI = 0.0;
  Length w,h;
  w.setLength_in_mm(200);
  h.setLength_in_mm(300);
  userPreferredSize.setPageSize(w,h);
  useDocumentSpecifiedSize = true;
}


DocumentPageCache::~DocumentPageCache()
{
}


void DocumentPageCache::setRenderer(DocumentRenderer *_renderer)
{
  clear();
  renderer = _renderer;
}


SimplePageSize DocumentPageCache::sizeOfPage(const PageNumber& page) const
{
  // Paranoid safety checks
  if (!page.isValid()) {
    kdError(1223) << "DocumentPageCache::sizeOfPage( " <<  page << ") called with invalid page number." << endl;
    return SimplePageSize();
  }
  if (renderer.isNull()) {
    kdError(1223) << "DocumentPageCache::sizeOfPage( " <<  page << ") called when no renderer was set." << endl;
    return SimplePageSize();
  }

  SimplePageSize s = renderer->sizeOfPage(page);
  if (!useDocumentSpecifiedSize)
    s = userPreferredSize;

  if (!s.isValid())
  {
    // If the size is invalid use the size of the first Page in the document
    // as an estimate.
    s = renderer->sizeOfPage(1);
    if (!s.isValid())
      s = userPreferredSize;
  }

  return s;
}

void DocumentPageCache::setResolution(double res)
{
  resolutionInDPI = res;
}

QSize DocumentPageCache::sizeOfPageInPixel(const PageNumber& pg) const
{
  // Paranoid safety checks
  if (renderer.isNull()) {
    kdError(1223) << "DocumentPageCache::sizeOfPageInPixel( " << pg << " ) called but no renderer was set" << endl;
    return QSize();
  }
  if (!pg.isValid()) {
    kdError(1223) << "DocumentPageCache::sizeOfPageInPixel( " << pg << " ) called with invalid argument" << endl;
    return QSize();
  }

  SimplePageSize ps = sizeOfPage(pg);
  if (ps.isValid())
    return ps.sizeInPixel(resolutionInDPI);
  return userPreferredSize.sizeInPixel(resolutionInDPI); 
}


bool DocumentPageCache::isPageCached(const PageNumber& pageNumber, const QSize& size)
{
  // Paranoid checks
  if (renderer.isNull()) {
    kdError(1223) << "DocumentPageCache::isPageCached(..) called but no renderer was set" << endl;
    return false;
  }
  if (!pageNumber.isValid()) {
    kdError(1223) << "DocumentPageCache::isPageCached( " << pageNumber << " ) called, with invalid argument." << endl;
    return false;
  }
  if (renderer->totalPages() < pageNumber) {
    kdError(1223) << "DocumentPageCache::isPageCached( " << pageNumber
                  << " ) called but document contains only " << renderer->totalPages() << " pages." << endl;
    return false;
  }

  QString key = createKey(pageNumber, size);

  // Check if the page that we are looking for is in the cache.
  // We are not accessing the page, so we don't want it to be moved into the front.
  RenderedDocumentPagePixmap* page = LRUCache.find(key, false);

  if (page)
    return true;
  else
    return false;
}

QString DocumentPageCache::createKey(const PageNumber& pageNumber, const QSize& size)
{
  QString key;

  key = QString::number(pageNumber) + ":" +
        QString::number(size.width()) + ":" + QString::number(size.height());

  return key;
}

QString DocumentPageCache::createKey(const PageNumber& pageNumber)
{
  QSize pageSize = sizeOfPageInPixel(pageNumber);

  QString key;

  key = QString::number(pageNumber) + ":" +
        QString::number(pageSize.width()) + ":" + QString::number(pageSize.height());

  return key;
}

bool DocumentPageCache::isPageCached(const PageNumber& pageNumber)
{
  // Paranoid checks
  if (renderer.isNull()) {
    kdError(1223) << "DocumentPageCache::isPageCached(..) called but no renderer was set" << endl;
    return false;
  }
  if (!pageNumber.isValid()) {
    kdError(1223) << "DocumentPageCache::isPageCached( " << pageNumber << " ) called, with invalid argument." << endl;
    return false;
  }
  if (renderer->totalPages() < pageNumber) {
    kdError(1223) << "DocumentPageCache::isPageCached( " << pageNumber
                  << " ) called but document contains only " << renderer->totalPages() << " pages." << endl;
    return false;
  }

  return isPageCached(pageNumber, sizeOfPageInPixel(pageNumber));
}

RenderedDocumentPagePixmap* DocumentPageCache::getPage(const PageNumber& pageNr)
{
#ifdef DocumentPageCache_DEBUG
  kdDebug(1223) << "DocumentPageCache::getPage( pageNr=" << pageNr << " )" << endl;
#endif

  // Paranoid checks
  if (renderer.isNull()) {
    kdError(1223) << "DocumentPageCache::getPage(..) called but no renderer was set" << endl;
    return 0;
  }
  if (!pageNr.isValid()) {
    kdError(1223) << "DocumentPageCache::getPage( " << pageNr << " ) called, with invalid argument." << endl;
    return 0;
  }
  if (renderer->totalPages() < pageNr) {
    kdError(1223) << "DocumentPageCache::getPage( " << pageNr << " ) called but document contains only " << renderer->totalPages() << " pages." << endl;
    return 0;
  }

  // First check if the page that we are looking for is in the cache
  RenderedDocumentPagePixmap* page;
  page = LRUCache.find(createKey(pageNr));

  if (page)
    return page;

  // The page was not found in the cache, so we have to make a new
  // page and add this to the cache.
  page = createDocumentPagePixmap();

  // If that failed, issue an error message and quit.
  if (page == 0) {
    kdError(1223) << "DocumentPageCache::getPage(..) cannot allocate DocumentPage structure" << endl;
    return 0;
  }
  
  // Now 'page' contains a point to a page structure that we can
  // use. Add the page to the cache, and apply the renderer to the page.
  page->setPageNumber(pageNr);
  if (!renderer.isNull())
  {
    if (resolutionInDPI > 0.0)
    {
      page->resize(sizeOfPageInPixel(pageNr));
      QApplication::setOverrideCursor( waitCursor );
      renderer->drawPage(resolutionInDPI, page);
      QApplication::restoreOverrideCursor();

      // We always set the cache capacity to be at least n times the cost of the page we want to insert.
      // Where n is the number of pages that can be visible at the same time at very high zoomlevels.
      // n depends on the layout mode.
      // If these pages are not all in the cache, scrolling the view becomes very slow, because for each
      // paint event the pages need to be rerendered.
      // We set n for each viewmode differently so that the user is able to reduce memory consuption by
      // switching to a simpler viewmode like Single Page.
      int n = 4;
      switch (KVSPrefs::viewMode())
      {
        case KVSPrefs::EnumViewMode::SinglePage:
          n = 1;
          break;
        case KVSPrefs::EnumViewMode::Continuous:
          n = 2;
          break;
        default:
          n = 4;
      }
      LRUCache.setMaxCost(QMAX(page->memory() * n, maxMemory));

      if (!LRUCache.insert(createKey(pageNr), page, page->memory()))
      {
        kdError() << "DocumentPageCache::getPage(): inserting pagestructure into the cache failed.\n This should never happen. If you see this message, something is very wrong." << endl;
      }
    }
    else
      kdError(1223) << "DocumentPageCache::getPage() called, but no resolution or negative resolution was set" << endl;
  }

  return page;
}


RenderedDocumentPagePixmap* DocumentPageCache::createDocumentPagePixmap() const
{
 return new RenderedDocumentPagePixmap();
}


void DocumentPageCache::clear()
{
  LRUCache.clear();
}


void DocumentPageCache::setUserPreferredSize(const SimplePageSize& s)
{
  bool sizeChanged = !userPreferredSize.isNearlyEqual(s);
  userPreferredSize = s;

  if (sizeChanged)
    emit(paperSizeChanged());
}


void DocumentPageCache::setUseDocumentSpecifiedSize(bool b)
{
  bool valChanged = (useDocumentSpecifiedSize == b);

  useDocumentSpecifiedSize = b;
  if (valChanged)
    emit(paperSizeChanged());
}


QPixmap DocumentPageCache::createThumbnail(const PageNumber& pageNr, int width)
{
  // Paranoid checks
  if (renderer.isNull()) {
    kdError(1223) << "DocumentPageCache::createThumbnail(..) called but no renderer was set" << endl;
    thumbnailPage.resize(0,0);
    return thumbnailPage;
  }
  if (renderer->totalPages() < pageNr) {
    kdError(1223) << "DocumentPageCache::createThumbnail( " << pageNr << ", width ) called but document contains only " << renderer->totalPages() << " pages." << endl;
    thumbnailPage.resize(0,0);
    return thumbnailPage;
  }
  if (!pageNr.isValid()) {
    kdError(1223) << "DocumentPageCache::createThumbnail(..) called for page with invalid page specification" << endl;
    thumbnailPage.resize(0,0);
    return thumbnailPage;
  }
  if (!sizeOfPage().isValid()) {
    kdError(1223) << "DocumentPageCache::createThumbnail(..) called for page with invalid size" << endl;
    thumbnailPage.resize(0,0);
    return thumbnailPage;
  }

  thumbnailPage.setPageNumber(pageNr);
  thumbnailPage.resize(width, (int)(width/sizeOfPage(pageNr).aspectRatio() + 0.5 ) );
  renderer->drawThumbnail((double)(width)/sizeOfPage(pageNr).width().getLength_in_inch(), &thumbnailPage);

  if (KVSPrefs::changeColors() && KVSPrefs::renderMode() != KVSPrefs::EnumRenderMode::Paper)
  {
    return thumbnailPage.accessiblePixmap();
  }
  else
  {
    return thumbnailPage;
  }
}

void DocumentPageCache::deselectText() 
{
  userSelection.clear();
  emit textSelected(false);
}

void DocumentPageCache::selectText(const TextSelection& selection)
{
  userSelection = selection;
  emit textSelected(!userSelection.isEmpty());
}

#include "documentPageCache.moc"
