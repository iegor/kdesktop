
/* This file is part of the KDE project
   Copyright (C) 2005 Wilfried Huss <Wilfried.Huss@gmx.at>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <config.h>

#include <kimageeffect.h>
#include <kdebug.h>
#include <qimage.h>

#include "kvsprefs.h"
#include "renderedDocumentPagePixmap.h"

RenderedDocumentPagePixmap::RenderedDocumentPagePixmap()
  : _accessiblePixmap(0), dirty(true)
{
}

RenderedDocumentPagePixmap::~RenderedDocumentPagePixmap()
{
  delete _accessiblePixmap;
}


QPainter* RenderedDocumentPagePixmap::getPainter()
{
  if (paintingActive()) {
    kdError(1223) << "RenderedDocumentPagePixmap::getPainter() called when painting was active" << endl;
    return 0;
  } else
    return new QPainter(this);
}


void RenderedDocumentPagePixmap::resize(int width, int height)
{
  QPixmap::resize(width, height);
  if(_accessiblePixmap)
    _accessiblePixmap->resize(width, height);

  dirty = true;
}

void RenderedDocumentPagePixmap::resize(const QSize& size)
{
  resize(size.width(), size.height());
}

QPixmap RenderedDocumentPagePixmap::accessiblePixmap()
{
  if (!_accessiblePixmap || dirty)
  {
    QImage backImage = this->convertToImage();
    switch (KVSPrefs::renderMode())
    {
      case KVSPrefs::EnumRenderMode::Inverted:
        // Invert image pixels using QImage internal function
        backImage.invertPixels(false);
        break;
      case KVSPrefs::EnumRenderMode::Recolor:
        // Recolor image using KImageEffect::flatten with dither:0
        KImageEffect::flatten(backImage, KVSPrefs::recolorForeground(), KVSPrefs::recolorBackground());
        break;
      case KVSPrefs::EnumRenderMode::BlackWhite:
        // Manual Gray and Contrast
        unsigned int* data = (unsigned int*)backImage.bits();
        int val;
        int pixels = backImage.width() * backImage.height();
        int con = KVSPrefs::bWContrast();
        int thr = 255 - KVSPrefs::bWThreshold();

        for( int i = 0; i < pixels; ++i )
        {
          val = qGray(data[i]);
          if (val > thr)
            val = 128 + (127 * (val - thr)) / (255 - thr);
          else if ( val < thr )
            val = (128 * val) / thr;
          if (con > 2)
          {
            val = con * (val - thr) / 2 + thr;
            if (val > 255)
              val = 255;
            else if (val < 0)
              val = 0;
          }
          data[i] = qRgba(val, val, val, 255);
        }
        break;
    }
    if (!_accessiblePixmap)
      _accessiblePixmap = new QPixmap(width(), height());

    _accessiblePixmap->convertFromImage(backImage);
    dirty = false;
  }

  return *_accessiblePixmap;
}

unsigned int RenderedDocumentPagePixmap::memory()
{
  return size().height() * size().width() * depth() / 8;
}

#include "renderedDocumentPagePixmap.moc"
