//
// Class: documentRenderer
//
// Abstract Widget for displaying document types
// Needs to be implemented from the actual parts
// using kviewshell 
// Part of KViewshell - A generic interface for document viewers.
//
// (C) 2004-2005 Wilfried Huss, Stefan Kebekus. Distributed under the GPL.

#include <config.h>

#include <kdebug.h>

#include "documentRenderer.h"
#include "renderedDocumentPage.h"


DocumentRenderer::DocumentRenderer(QWidget* par)
  : mutex(true), parentWidget(par), accessibilityBackground(false),
    accessibilityBackgroundColor(QColor(255,255,255))
{
  numPages = 0;
  _isModified = false;
}


DocumentRenderer::~DocumentRenderer()
{
  // Wait for all access to this DocumentRenderer to finish
  QMutexLocker lock(&mutex);
  
  clear();
}


void DocumentRenderer::clear()
{
  // Wait for all access to this DocumentRenderer to finish
  QMutexLocker lock(&mutex);

  numPages = 0;
  pageSizes.clear();
  anchorList.clear();
  bookmarks.clear();
  _isModified = false;
}

void DocumentRenderer::setAccessibleBackground(bool accessibleMode, const QColor& background)
{
  // Wait for all access to this DocumentRenderer to finish
  QMutexLocker lock(&mutex);
  
  accessibilityBackground = accessibleMode;
  accessibilityBackgroundColor = background;
}

SimplePageSize DocumentRenderer::sizeOfPage(const PageNumber& page)
{
  // Wait for all access to this DocumentRenderer to finish
  QMutexLocker locker(&mutex);
  
  if (!page.isValid())
    return SimplePageSize();
  if (page > totalPages()) 
    return SimplePageSize();
  if (page > pageSizes.size())
    return SimplePageSize();
  
  return pageSizes[page-1];
}


void DocumentRenderer::drawThumbnail(double resolution, RenderedDocumentPage* page)
{
  // Wait for all access to this DocumentRenderer to finish
  QMutexLocker locker(&mutex);

  drawPage(resolution, page);
}

void DocumentRenderer::getText(RenderedDocumentPage* page)
{
  // We are only interrested in the textual data, so we can use a dummy value for the resolution.
  drawPage(100.0, page);
}

bool DocumentRenderer::isValidFile(const QString&) const
{
  return true;
}


Anchor DocumentRenderer::parseReference(const QString &reference)
{
  // Wait for all access to this documentRenderer to finish
  QMutexLocker locker(&mutex);

  if (isEmpty())
    return Anchor();

  // If the reference is a number, which we'll interpret as a
  // page number.
  bool ok;
  int page = reference.toInt(&ok);
  if (ok == true)
  {
    if (page < 1)
      page = 1;
    if (page > totalPages())
      page = totalPages();

    return Anchor(page, Length());
  }

  // If the reference is not a number, return an empty anchor.
  return Anchor();
}

Anchor DocumentRenderer::findAnchor(const QString &locallink)
{
  // Wait for all access to this DocumentRenderer to finish
  QMutexLocker locker(&mutex);

  QMap<QString,Anchor>::Iterator it = anchorList.find(locallink);
  if (it != anchorList.end()) 
    return *it;
  else 
    return Anchor();
}


#include "documentRenderer.moc"
