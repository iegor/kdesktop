/*
 * screenshotpreview.cpp
 *
 * Copyright (C) 2003-2005 Jürgen Kofler <kaffeine@gmx.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <kglobalsettings.h>
 
#include <qpainter.h>
#include <qwidget.h>
#include <qimage.h>

#include "screenshotpreview.h"
#include "screenshotpreview.moc"


ScreenshotPreview::ScreenshotPreview(const QImage& img, QWidget *parent, const char *name )
  : KPreviewWidgetBase(parent,name)
{
  setMinimumWidth(200);
  m_previewImg = img.copy(); /* deep copy */
}

ScreenshotPreview::~ScreenshotPreview()
{
}

void ScreenshotPreview::showPreview(const KURL&) /* reimplemented to do nothing */
{
}

void ScreenshotPreview::clearPreview() /* reimplemented to do nothing */
{
}

/* show preview picture */

void ScreenshotPreview::paintEvent(QPaintEvent*)
{
  int imgHeight, posy;

  imgHeight = (int)((double) m_previewImg.height() / m_previewImg.width() * (width()-5));
  posy = (height() - imgHeight) / 2;

  QString info = QString::number(m_previewImg.width()) + "x" + QString::number(m_previewImg.height());

  QFont font = KGlobalSettings::generalFont();
  font.setPointSize(10);
  QFontMetrics met(font);
    
  QPainter painter(this);
  painter.drawImage(QRect(5, posy, width(), imgHeight), m_previewImg);

  painter.setFont(font);
  painter.drawText((width()-met.width(info))/2, posy + imgHeight + 20, info);
}
