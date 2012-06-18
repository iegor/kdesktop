/*
 * screenshotpreview.h
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

#ifndef SCREENSHOTPREVIEW_H
#define SCREENSHOTPREVIEW_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kpreviewwidgetbase.h>

class QImage;
class QWidget;

class ScreenshotPreview : public KPreviewWidgetBase
{
   Q_OBJECT
public: 
  ScreenshotPreview(const QImage& img, QWidget *parent=0, const char *name=0);
  ~ScreenshotPreview();

public slots:
 virtual void showPreview(const KURL&);
 virtual void clearPreview();
  
protected:
 virtual void paintEvent(QPaintEvent*);

private:
  QImage m_previewImg; 
  
};

#endif /* SCREENSHOTPREVIEW_H */
