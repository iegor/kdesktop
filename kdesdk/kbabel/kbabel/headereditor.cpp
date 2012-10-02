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
#include <kcursor.h>
#include <kglobalsettings.h>
#include <kglobal.h>
#include <kconfig.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <ktextedit.h>
#include <kurl.h>

#include "catalog.h"
#include "catalogitem.h"
#include "headereditor.h"
#include "headerwidget.h"

using namespace KBabel;

HeaderEditor::HeaderEditor(Catalog* cat,const char* name)
             : KDialogBase((QWidget*)0,name,false,QString::null, Ok|Cancel|Default|User1)
{
   restoreSettings();

   _catalog=cat;
   connect(_catalog,SIGNAL(signalFileOpened(bool)),this,SLOT(readHeader(bool)));
   connect(_catalog,SIGNAL(signalHeaderChanged()),this,SLOT(updateHeader()));

   setButtonText(User1,i18n("&Apply Settings"));
   setButtonWhatsThis (User1, i18n("<qt><p>This button "
	"updates the header using the current settings. "
	"The resulting header is the one that would be written into the PO file "
	"on saving.</p></qt>") );
   setButtonText(Default,i18n("&Reset"));
   setButtonWhatsThis (Default, i18n("<qt><p>This button "
	"will revert all changes made so far.</p></qt>") );

   _editor=new HeaderWidget(this,"internal headereditor");
   _editor->setMinimumSize(_editorSize);

   KCursor::setAutoHideCursor(_editor,true);

   readHeader(cat->isReadOnly());
   updateHeader();

   setMainWidget(_editor);
}

HeaderEditor::~HeaderEditor()
{
   saveSettings();
}

void HeaderEditor::saveSettings()
{
    KConfig* config = KGlobal::config();

    KConfigGroupSaver saver(config, "HeaderEditor" );

    config->writeEntry( "Size", _editor->size() );
}

void HeaderEditor::restoreSettings()
{
    KConfig* config = KGlobal::config();

    KConfigGroupSaver saver(config, "HeaderEditor" );

    QSize defaultSize(350,250);
    _editorSize = config->readSizeEntry("Size", &defaultSize );
}

bool HeaderEditor::isModified()
{
   return _editor->commentEdit->isModified() || _editor->headerEdit->isModified();
}

void HeaderEditor::readHeader(bool readOnly)
{
   setCaption(i18n("Header Editor for %1").arg(_catalog->currentURL().prettyURL()));

   _editor->headerEdit->setReadOnly(readOnly);
   _editor->commentEdit->setReadOnly(readOnly);
   enableButton(User1,!readOnly);
}

void HeaderEditor::updateHeader()
{
    _editor->headerEdit->setText(_catalog->header().msgstr().first());
    _editor->headerEdit->setModified(false);
    _editor->commentEdit->setText(_catalog->header().comment());
    _editor->commentEdit->setModified(false);
}


// update button
void HeaderEditor::slotUser1()
{
   CatalogItem header;

   bool error = !isValid();

   if(error)
   {
      QString msg=i18n("<qt><p>This is not a valid header.</p>\n"
        "<p>Please edit the header before updating!</p></qt>");

      KMessageBox::sorry(this,msg);

      return;
   }

   header.setComment( _editor->commentEdit->text() );
   header.setMsgstr( _editor->headerEdit->text() );

   header=_catalog->updatedHeader(header,true);

   _editor->headerEdit->setText(header.msgstr().first());
   _editor->commentEdit->setText(header.comment());
}


void HeaderEditor::slotDefault()
{
   updateHeader();
}

void HeaderEditor::slotCancel()
{
   updateHeader();

   QDialog::reject();
}

void HeaderEditor::slotOk()
{
   if(isModified())
   {
      if(!isValid())
      {
    	 QString msg=i18n("<qt><p>This is not a valid header.</p>\n"
    		"<p>Please edit the header before updating.</p></qt>");

         switch(KMessageBox::warningYesNo(this,msg,i18n("Warning"),KStdGuiItem::discard(),i18n("Edit")))
         {
            case KMessageBox::Yes:
            {
               slotCancel();
               return;
            }
            default:
               return;
         }
      }

      CatalogItem header;

      header.setComment( _editor->commentEdit->text() );
      header.setMsgstr( _editor->headerEdit->text() );

      _catalog->setHeader(header);
   }

   QDialog::accept();
}

bool HeaderEditor::isValid()
{
   // check the comments
   QStringList comments = QStringList::split('\n',_editor->commentEdit->text());

   for( QStringList::Iterator it = comments.begin(); it != comments.end(); ++it )
   {
	if( !(*it).startsWith("#") )
	{
	    return false;
	}
   }

   return true;
}

#include "headereditor.moc"
