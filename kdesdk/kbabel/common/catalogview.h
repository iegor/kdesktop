/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>

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
#ifndef CATALOGVIEW_H
#define CATALOGVIEW_H

namespace KBabel
{

class EditCommand;

/**
 * This class represents a base class for a @ref Catalog view. It declares
 * an abstract method @ref update to be redefined by the actual
 * view. Using this method, the catalog the view is registered for
 * will inform the views about changes in the catalog.
 *
 * @short This class represents a base class for a catalog view.
 * @author Matthias Kiefer <matthias.kiefer@gmx.de>
 */
class CatalogView
{
public:
    /** The only method to be redefined in inherited classes.
     *  This method is called by @ref Catalog to update the view
     *  in case there are changes.
     *  @param cmd the command line performed on the catalog
     *  @param undo this is used for undo, reverse the operation
     */
    virtual void update(EditCommand* cmd, bool undo=false)=0;
};

}

#endif // CATALOGVIEW_H
