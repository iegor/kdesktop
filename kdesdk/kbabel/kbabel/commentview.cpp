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

#include "commentview.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qwhatsthis.h>
#include <qdragobject.h>
#include <qtextview.h>

#include <kapplication.h>
#include <kconfig.h>
#include <kcursor.h>
#include <kdialog.h>
#include <klocale.h>
#include <kurl.h>
#include <kurldrag.h>

#include "resources.h"

#include "editcmd.h"
#include "hidingmsgedit.h"
#include "kbcatalog.h"

#define ID_DROP_OPEN          1
#define ID_DROP_OPEN_TEMPLATE 2

#define MAX_HISTORY 50

using namespace KBabel;

CommentView::CommentView(KBCatalog* catalog,QWidget *parent, Project::Ptr project)
    : KBCatalogView(catalog,parent, project)
{
    setAcceptDrops(true);

    QVBoxLayout* layout=new QVBoxLayout(this);

    commentEdit = new MsgMultiLineEdit(0, 0, this);
    commentEdit->setMinimumHeight(50);
    commentEdit->setSpacePoints(false);
    commentEdit->setHighlightSyntax(false);
    KCursor::setAutoHideCursor(commentEdit,true);

    QLabel* label=new QLabel(commentEdit,i18n("&Comment:"),this);

    QHBoxLayout* hb=new QHBoxLayout(layout);
    hb->addSpacing(KDialog::marginHint());
    hb->addWidget(label);

    layout->addWidget(commentEdit);
    layout->setStretchFactor(commentEdit,1);

    QWhatsThis::add(this,
       i18n("<qt><p><b>Comment Editor</b></p>\n\
This edit window shows you the comments of the currently displayed message.<p>\n\
<p>The comments normally contain information about where the message is found in the source\n\
code and status information about this message (fuzzy, c-format).\n\
Hints from other translators are also sometimes contained in comments.</p>\n\
<p>You can hide the comment editor by deactivating\n\
<b>Options->Show Comments</b>.</p></qt>"));

    commentEdit->setReadOnly(true);
    connect(commentEdit,SIGNAL(signalUndoCmd(KBabel::EditCommand*)),this,SLOT(forwardCommentEditCmd(KBabel::EditCommand*)));

    connect(commentEdit,SIGNAL(cursorPositionChanged(int,int))
            , this, SIGNAL(signalCursorPosChanged(int,int)));
	    
    connect(_catalog, SIGNAL(signalFileOpened(bool)), this, SLOT(setDisabled(bool)));
}

void CommentView::update(EditCommand* cmd, bool undo)
{
   if((int)_currentIndex==cmd->index())
   {
      if(cmd->part()==Comment)
      {
         commentEdit->processCommand(cmd,undo);
      }
   }
}

void CommentView::textCut()
{
   if(commentEdit->hasFocus())
   {
      commentEdit->cut();
   }
}

void CommentView::textCopy()
{
    if(commentEdit->hasSelectedText())
    {
	commentEdit->copy();
    }
}

void CommentView::textPaste()
{
   if(commentEdit->hasFocus())
   {
      commentEdit->paste();
   }
}

void CommentView::textSelectAll()
{
   if(commentEdit->hasFocus())
   {
      commentEdit->selectAll();
   }
}

void CommentView::textDeselectAll()
{
    commentEdit->selectAll(false);
}

void CommentView::updateView()
{
    commentEdit->setText ( _catalog->comment (_currentIndex) );
}

void CommentView::forwardCommentEditCmd(EditCommand* cmd)
{
   cmd->setPart(Comment);
   cmd->setIndex(_currentIndex);

   _catalog->applyEditCommand(cmd,this);
}

void CommentView::setReadOnly(bool on)
{
    commentEdit->setReadOnly( on );
}

void CommentView::setOverwriteMode(bool on)
{
    commentEdit->setOverwriteMode( on );
}

void CommentView::readFileSettings()
{
    setReadOnly( _catalog->isReadOnly() );
}

const QString CommentView::selectText(int from, int to)
{
    int line, col, endline, endcol;
    
    commentEdit->selectAll(false);
    commentEdit->setFocus();
    commentEdit->offset2Pos(from,line,col);
    commentEdit->offset2Pos(to,endline,endcol);

    commentEdit->setSelection(line,col,endline,endcol);
    commentEdit->setCursorPosition(endline,endcol);
    
    return commentEdit->selectedText();
}

bool CommentView::isActiveView ()
{
    return hasFocus () || commentEdit->hasFocus ();
}

int CommentView::currentIndex ()
{
    return commentEdit->currentIndex ();
}

const QString CommentView::selectedText ()
{
    return commentEdit->selectedText ();
}

void CommentView::installEventFilter( const QObject * filterObj )
{
    commentEdit->installEventFilter( filterObj );
}

bool CommentView::hasFocus()
{
    return commentEdit->hasFocus();
}

#include "commentview.moc"
