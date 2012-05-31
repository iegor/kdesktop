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

#ifndef EMPTY_MULTIPAGE_H
#define EMPTY_MULTIPAGE_H

#include "kmultipage.h"
#include "emptyRenderer.h"

#include <kparts/genericfactory.h>


class EmptyMultiPage : public KMultiPage
{
  Q_OBJECT

public:
  EmptyMultiPage(QWidget* parentWidget, const char* widgetName, QObject* parent, const char* name,
                 const QStringList& args = QStringList());

  virtual QStringList fileFormats() const { return QString::null; }

  /// opens a file
  virtual bool openFile() { return false; }

  static KAboutData* createAboutData();

private:
  EmptyRenderer emptyRenderer;
};

typedef KParts::GenericFactory<EmptyMultiPage> EmptyMultiPageFactory;

#endif
