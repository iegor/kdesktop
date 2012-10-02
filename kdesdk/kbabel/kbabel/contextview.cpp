/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2002-2004  by Stanislav Visnovsky
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

#include "contextview.h"

#include <qlayout.h>
#include <qtextview.h>
#include <qwhatsthis.h>

#include <kcursor.h>
#include <klocale.h>

#include "resources.h"
#include "kbcatalog.h"

using namespace KBabel;

ContextView::ContextView(KBCatalog* catalog,QWidget *parent, Project::Ptr project)
    : KBCatalogView(catalog,parent,project)
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setResizeMode( QLayout::Minimum );

    _textview = new QTextView (this, "context textview");
    KCursor::setAutoHideCursor(_textview->viewport(),true);
    _textview->setReadOnly(true);
    
    layout->addWidget (_textview);

    QWhatsThis::add(this,
        i18n("<qt><p><b>PO Context</b></p>"
             "<p>This window shows the context of the current message "
             "in the PO file. Normally it shows four messages in front "
             "of the current message and four after it.</p>"
             "<p>You can hide the tools window by deactivating "
             "<b>Options->Show Tools</b>.</p></qt></qt>"));
	     
    connect(_catalog, SIGNAL(signalFileOpened(bool)), this, SLOT(setDisabled(bool)));
}

void ContextView::updateView()
{
    uint total = _catalog->numberOfEntries();
    if(total==0)
	return;

    QString text;
    uint startIndex;
    uint context = 4;
    if(_currentIndex < context)
    {
        startIndex = 0;
    }
    else if(_currentIndex > total-2*context-2)
    {
	startIndex = total-2*context-1;
    }
    else
    {
        startIndex = _currentIndex-context;
    }

    for(uint i=startIndex; i < QMIN(startIndex+(2*context+1), total); i++)
    {
        if(i == _currentIndex)
        {
            text += "<p><hr/><b>"+i18n("current entry") +"</b><hr/></p>";
            continue;
        }

        QString entry;
        QString temp;
        temp = _catalog->comment(i);
        if(!temp.isEmpty())
        {
            temp = QStyleSheet::convertFromPlainText(temp);
            temp.replace(QRegExp("^<p>"),"");
            temp.replace(QRegExp("</p>$"),"");
            entry += "<i>"+temp+"</i><br/>";
        }

        // FIXME: should care about plural forms
        temp = QStyleSheet::convertFromPlainText(_catalog->msgid(i).first());
         temp.replace(QRegExp("^<p>"),"");
        temp.replace(QRegExp("</p>$"),"");
        entry += temp + "<br/>---<br/>";

        QStringList tempList = _catalog->msgstr(i);

        if(tempList.isEmpty())
        {
            entry +="<i><b><font color=\"red\">"
        	+i18n("untranslated")
                +"</font></b></i><br/>";
        }
        else
        {
            if( tempList.count() == 1 )
	    {
		temp = tempList.first();
	    }
	    else
	    {
		uint counter = 1;
		temp = "";
		for( QStringList::Iterator i=tempList.begin() ; i != tempList.end() ; ++i)
		{
		    temp += i18n("Plural %1: %2\n").arg(counter++).arg(*i);
		}
	    }
            temp = QStyleSheet::convertFromPlainText(temp);
            temp.replace(QRegExp("^<p>"),"");
            temp.replace(QRegExp("</p>$"),"");

            entry += temp+"<br/>";
        }

        text += "<p>"+entry+"</p>";
    }
    _textview->setText("<qt>"+text+"</qt>");

    int height = _textview->contentsHeight();
    _textview->setContentsPos(0,height/2);
}

#include "contextview.moc"
