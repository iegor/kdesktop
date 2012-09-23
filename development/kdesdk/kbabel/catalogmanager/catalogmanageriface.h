/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2001     by Stanislav Visnovsky <visnovsky@kde.org>

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
#ifndef CATALOGMANAGERIFACE_H
#define CATALOGMANAGERIFACE_H

#include <dcopobject.h>

class CatalogManagerIface : virtual public DCOPObject
{
  K_DCOP
public:

k_dcop:

  /**
   * Sets the WId of the preferred window. The window will get
   * calls to open files or templates when requested in catalog manager.
   * @param id is the WId of the window to be used
   */
  virtual void setPreferredWindow( WId id ) { id = 0; }
  
  /**
   * Returns a next file containing the searched string. Invoked from
   * KBabel in case the FindNext() finishes a file and wants to continue in
   * the next one.
   * @returns a URL to the next file, QString:null if there is no more
   * files to be searched or empty string if the search string
   * is not found yet, but there is more files to be searched in.
   */
  virtual QCString findNextFile() = 0;
  
  /**
   * If you care about this file, you should update information shown.
   */
  virtual void updatedFile( QCString url) = 0;
};

#endif // CATALOGMANAGERIFACE_H
