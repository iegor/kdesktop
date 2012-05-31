// -*- C++ -*-
// ***************************************************************************
//
// Copyright (C) 2005 by Wilfried Huss <Wilfried.Huss@gmx.at>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software 
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
// ***************************************************************************

#ifndef _EMPTYRENDERER_H_
#define _EMPTYRENDERER_H_

#include "documentRenderer.h"

class RenderedDocumentPage;


class EmptyRenderer : public DocumentRenderer
{
  Q_OBJECT

public:
  EmptyRenderer(QWidget* parent);

  virtual bool setFile(const QString &, const KURL &) { return false; }

  void drawPage(double res, RenderedDocumentPage* page) { Q_UNUSED(res); Q_UNUSED(page); }
};

#endif
