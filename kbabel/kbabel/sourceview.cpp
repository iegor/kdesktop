/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2004-2005  by Stanislav Visnovsky
			    <visnovsky@kde.org>

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

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt. If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.

**************************************************************************** */

#include "sourceview.h"
#include "context.h"

#include <qlayout.h>
#include <qwhatsthis.h>

#include <kcursor.h>
#include <klocale.h>

#include "resources.h"
#include "kbcatalog.h"

using namespace KBabel;

SourceView::SourceView(KBCatalog* catalog,QWidget *parent, Project::Ptr project)
    : KBCatalogView(catalog,parent,project)
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setResizeMode( QLayout::Minimum );

    _contextView = new SourceContext (this, project);
    layout->addWidget (_contextView);
    
    connect(_catalog, SIGNAL(signalFileOpened(bool)), this, SLOT(setDisabled(bool)));
}

void SourceView::updateView()
{
    if (isVisible ())
    {
        // Note: we use Catalog::comment instead of Catalog::context as SourceContext::setContext has to repeat the major part of the code of Catalog::context, so SourceContext::setContext can do the whole job alone.
	_contextView->setContext( _catalog->packageDir(), _catalog->packageName(), _catalog->comment(_currentIndex), _catalog->currentURL() );
    }
}

void SourceView::setProject(KBabel::Project::Ptr project)
{
    KBCatalogView::setProject(project);
    _contextView->setProject(project);
}

#include "sourceview.moc"
