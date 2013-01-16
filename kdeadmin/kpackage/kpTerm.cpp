/*
** Copyright (C) 1999,2000 Toivo Pedaste <toivo@ucs.uwa.edu.au>
**
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/


#include "../config.h"

#include <qvbox.h>

#include <kglobalsettings.h>
#include <kdebug.h>

#include <kpTerm.h>


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

kpTerm::kpTerm(kpPty *pt, QWidget * parent,  const char * name ) :
  QTextEdit(parent,name)
{
  pty = pt;
  setFont(KGlobalSettings::fixedFont());
  //  setMinimumWidth(fontMetrics().maxWidth()*80 +
  //      minimumSizeHint().width());
  setWordWrap(NoWrap);
  setReadOnly(TRUE);
}

void kpTerm::doConnect()
{
  connect(pty, SIGNAL(textIn(const QString &, bool)), this,
	  SLOT(textIn(const QString &, bool)));
  connect(pty,SIGNAL(result(QStringList &, int)),
	  this,SLOT(slotResult(QStringList &, int)));
  connect(this, SIGNAL(keyOut(char)), pty,
	  SLOT(keyOut(char)));
}

void kpTerm::doUnconnect()
{
  disconnect(pty, SIGNAL(textIn(const QString &, bool)), this,
		   SLOT(textIn(const QString &, bool)));
  disconnect(pty,SIGNAL(result(QStringList &, int)),
	  this,SLOT(slotResult(QStringList &, int)));
  disconnect(this, SIGNAL(keyOut(char)), pty,
		   SLOT(keyOut(char)));
}

bool kpTerm::run(const QString &cmd, QStringList &r) 
{
  setReadOnly(FALSE);
  setFocus();
  if (pty->startSession(TRUE)) {
    doConnect();

    r =  pty->run(cmd,FALSE);
    return TRUE;
  } else {
    return FALSE;
  }
}
 
void kpTerm::cancel() {
  emit keyOut('\03');
}

void kpTerm::done()
{
   clear();
   doUnconnect();
   setReadOnly(TRUE);
   clearFocus();
}

void kpTerm::keyPressEvent ( QKeyEvent * e ) 
{
  //  kdDebug() << "K=" << e->ascii() << "," << e->ascii() << "\n";
  if (e->ascii()) {
    emit keyOut(e->ascii());
  } else {
      QTextEdit::keyPressEvent (e);
  }
  setCursorPosition(9999,9999);
}

void kpTerm::textIn(const QString &stext, bool bNewLine)
{
  QRegExp chrs("[\\010\\012\\015]");
  QString del = "\010";
  // kdDebug() << "Tin=[" << stext << "]\n";
  if (stext.find(chrs) < 0) {
    insert( stext );
  } else {
    int p;
    int op = 0;

    while ((p = stext.find(chrs,op)) >= 0) {
      if (p != op) {
	insert( stext.mid(op, p-op));
      }
      if (stext[p] == '\b') {
	doKeyboardAction(ActionBackspace);
      } else if (stext[p] == '\r') {
	 moveCursor(MoveLineStart, false);
      }  else if (stext[p] == '\n') {
	moveCursor(MoveEnd, false);
	doKeyboardAction(ActionReturn);
      }
      op = p + 1;
    }
    if ((signed int)stext.length() > op)
      insert( stext.right(stext.length()-op));
  }
  if (bNewLine) {
   	moveCursor(MoveEnd, false);
	doKeyboardAction(ActionReturn);
  }
  moveCursor(MoveEnd, false);
}

void kpTerm::insert ( const QString & str, bool) {
  int x,y;
  getCursorPosition(&y,&x);

  if (str.length() > 0) {
    //    kdDebug() << "ins:"  << y << "," << x << str <<":" << str.length() << "\n";
    if (x == 0 && str != "\n") {
      doKeyboardAction(ActionKill);
      getCursorPosition(&y,&x);
      //      kdDebug() << "k="  << y << "," << x <<"\n";
    }
    QTextEdit::insert(str,(bool)FALSE);
  }
}

void kpTerm::slotResult(QStringList &rlist, int ret)
{
  emit result(rlist, ret);
  doUnconnect();
}



//////////////////////////////////////////////////////////////////////////////
//
// Dialog window for password prompt
//
//////////////////////////////////////////////////////////////////////////////
kpRun::kpRun( QWidget *parent)
  :  KDialogBase(parent, "kpRun", true, QString::null,
		Cancel, Cancel, true )
{
  QVBox *page = makeVBoxMainWidget();
  title = new QLabel("", page);
  QFont f( KGlobalSettings::generalFont());
  f.setBold(true);
  f.setPointSize(f.pointSize()+4);
  title->setFont(f);

  term = new kpTerm(kpty,page);
  resize(600, 300);
  connect(term,SIGNAL(result(QStringList &, int)),
	  this,SLOT(slotResult(QStringList &, int)));
 
  hide();
}

bool kpRun::run(QString cmd, QString msg)
{
  QStringList r;

  title->setText(msg);
  if (!cmd.isEmpty()) {
    return term->run(cmd, r);
  } else {
    term->doConnect();
    term->setReadOnly(FALSE);
    term->setFocus();
    return true;
  }
}

void kpRun::addText(const QStringList &ret)
{
  int last = ret.count()-1;
  int i = 0;
  for ( QStringList::ConstIterator it = ret.begin(); it != ret.end(); ++it, ++i ) {
    //    kdDebug() << "ks=" << *it << "\n";
    term->textIn(*it, (i != last));
  }
}

void kpRun::slotResult(QStringList &, int ret)
{
  if (ret == 0 || ret == 666) {
    term->clear();
    if (ret == 0) 
      accept();
    else
      reject();
  }
}

void kpRun::slotCancel()
{
  term->clear();
  term->cancel();
  accept();
}

#include "kpTerm.moc"
