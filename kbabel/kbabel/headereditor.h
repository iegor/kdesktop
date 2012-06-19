/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2002-2003 by Stanislav Visnovsky
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
#ifndef HEADEREDITOR_H
#define HEADEREDITOR_H

#include <kdialogbase.h>

namespace KBabel
{
    class Catalog;
}

class HeaderWidget;

class HeaderEditor : public KDialogBase
{
   Q_OBJECT
public:
   /**
   * constructor for the HeaderEditor
   * @param cat The Catalog from which the header is edited
   */
   HeaderEditor(KBabel::Catalog* cat,const char* name=0);
   virtual ~HeaderEditor();

   bool isModified();

public slots:
   void readHeader(bool);

protected slots:

   /** update button */
   virtual void slotUser1();
   virtual void slotCancel();
   virtual void slotOk();
   virtual void slotDefault();

   /** updates the header when header was changed by the catalog */
   void updateHeader();

private:
   void saveSettings();
   void restoreSettings();
   bool isValid();
   
   KBabel::Catalog* _catalog;
   HeaderWidget* _editor;
   
   QSize _editorSize;
};

#endif // HEADEREDITOR_H
