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

#include "kbcatalogview.h"
#include "resources.h"

using namespace KBabel;

KBCatalogView::KBCatalogView(KBCatalog* catalog, QWidget* parent, Project::Ptr project)
    : QWidget (parent), KBabel::CatalogView () , _project (project)
{
    if (catalog == 0)
      kdFatal(KBABEL) << "catalog==0" << endl;

    _catalog=catalog;
    _catalog->registerView(this);

    _config = KSharedConfig::openConfig ("kbabelrc");

    _currentIndex=1;	// here we use 1 to accept update at opening a file
    
    connect ( _project, SIGNAL (signalSettingsChanged()), this, SLOT (readProjectSettings()) );
    connect ( _catalog, SIGNAL (signalFileOpened(bool)), this, SLOT (readFileSettings() ) );
    
    readProjectSettings();
    readFileSettings();
    readConfigurationSettings();
}

KBCatalogView::~KBCatalogView()
{
   _catalog->removeView(this);

   // check if this view was the last view and delete the catalog if necessary
   if(!_catalog->hasView())
   {
      delete _catalog;
   }
}

void KBCatalogView::update(EditCommand*, bool )
{
}

void KBCatalogView::textCut()
{
}

void KBCatalogView::textCopy()
{
}

void KBCatalogView::textPaste()
{
}

void KBCatalogView::textSelectAll()
{
}

void KBCatalogView::gotoEntry(const DocPosition& pos)
{
    if ( _currentIndex == pos.item )
	return;
	
    _currentIndex=pos.item;
    
    updateView ();
}

void KBCatalogView::readProjectSettings()
{
}

void KBCatalogView::readFileSettings()
{
    setReadOnly (_catalog->isReadOnly());
}

void KBCatalogView::readConfigurationSettings()
{
}

void KBCatalogView::readSessionSettings(KConfig *)
{
}

void KBCatalogView::writeConfigurationSettings(KConfig *)
{
}

void KBCatalogView::writeSessionSettings(KConfig *)
{
}

void KBCatalogView::setReadOnly (bool on)
{
    setEnabled (on);
}

void KBCatalogView::setProject(KBabel::Project::Ptr project)
{
    _project = project;
    readProjectSettings();
}

#include "kbcatalogview.moc"
