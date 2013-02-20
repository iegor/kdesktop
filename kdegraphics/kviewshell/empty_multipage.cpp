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

#include <config.h>

#include <kglobal.h>
#include <klocale.h>

#include "empty_multipage.h"

K_EXPORT_COMPONENT_FACTORY(emptymultipagepart, EmptyMultiPageFactory)

EmptyMultiPage::EmptyMultiPage(QWidget* parentWidget, const char* widgetName, QObject* parent, const char* name,
                               const QStringList& args)
  : KMultiPage(parentWidget, widgetName, parent, name), emptyRenderer(parentWidget)
{
  Q_UNUSED(args);
  setInstance(EmptyMultiPageFactory::instance());

  emptyRenderer.setName("Empty renderer");
  setRenderer(&emptyRenderer);
}

KAboutData* EmptyMultiPage::createAboutData()
{
  return new KAboutData("emptymultipage", I18N_NOOP("Empty Multipage"),
  "1.0", I18N_NOOP(""),
  KAboutData::License_GPL,
  I18N_NOOP("Copyright (c) 2005 Wilfried Huss"));
}

#include "empty_multipage.moc"
