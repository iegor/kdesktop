/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2002 by Stanislav Visnovsky
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
#ifndef MULTIROUGHTRANSDLG_H
#define MULTIROUGHTRANSDLG_H

#include <roughtransdlg.h>

class CatManListItem;

class MultiRoughTransDlg : public RoughTransDlg
{
    Q_OBJECT

public:
    MultiRoughTransDlg(KBabelDictBox* dictBox, QPtrList<CatManListItem> list, QWidget *parent
            , const char *name=0);

protected slots:
    virtual void translate();
    // we show statistics at different point
    virtual void showStatistics() {}
    virtual void showAllStatistics();
    
    // we need to enable Start button at different conditions
    virtual void msgButtonClicked(int); 

private:

    QPtrList<CatManListItem> _fileList;
    KProgress *filesProgressbar;
};

#endif // ROUGHTRANSDLG_H
