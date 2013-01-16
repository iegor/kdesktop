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
#include "procbuf.h"
#include <kprocess.h>
#include "kpackage.h"
#include <klocale.h>
#include <qlabel.h>
#include <kdebug.h>

Modal::Modal(QString msg, QWidget *parent, const char * name )
  : KDialog( parent, name, TRUE )
{
  QLabel *line1 = new QLabel(msg,this);
  line1->setAlignment(AlignCenter);
  line1->setAutoResize(true);

 }

void Modal::terminate()
{
  done(0);
}

procbuf::procbuf()
{
  m = NULL;
  tm = new QTimer(this);
  connect(tm, SIGNAL(timeout()), this, SLOT(slotTimeout()));
}

procbuf::~procbuf()
{
}

void procbuf::setup(QString cmd)
{
  buf.truncate(0);
  proc = new KProcess();
  connect(proc, SIGNAL( receivedStdout(KProcess *, char *, int)), 
			this, SLOT(slotReadInfo(KProcess *, char *, int)));
  connect(proc, SIGNAL( receivedStderr(KProcess *, char *, int)), 
			this, SLOT(slotReadInfo(KProcess *, char *, int)));
  connect(proc, SIGNAL( processExited(KProcess *)), 
			this, SLOT(slotExited(KProcess *)));
  proc->clearArguments();
  *proc << cmd;
  command = cmd;
}

void procbuf::slotReadInfo(KProcess *, char *buffer, int buflen)
{
   char last;

   last = buffer[buflen - 1];
   buffer[buflen - 1] = 0; 

   buf += buffer;
   buf += last;

   if (timed) {
     timed =  FALSE;
     tm->stop();
   }
}

void procbuf::slotExited(KProcess *)
{
  if (m) {
    m->terminate();
  }
  if (timed) {
    timed =  FALSE;
    tm->stop();
  }
}

void procbuf::slotTimeout()
{
  if (m) {
    m->terminate();
  }
  //  kdDebug() << "TTT\n";
}

int procbuf::start (QString  msg, bool errorDlg,
		    int timeout, QString timeMsg )
{
  if (timeout) {
    tm->start(timeout*1000, TRUE);   
    timed = true;
  }

  if (!proc->start(!msg.isNull() ? KProcess::NotifyOnExit : KProcess::Block,
		   KProcess::All)) {
    if (errorDlg) {
      KpMsgE(i18n("Kprocess Failure"),TRUE);
    }
    return 0;
  };

  if (!msg.isEmpty()) {
    m = new Modal(msg,kpkg, "wait");
    m->exec();
    delete m;
    m = 0;
  }

  kdDebug() << command 
	    << " dialog=" << errorDlg 
	    << " normal=" << proc->normalExit()
	    << " exit=" << proc->exitStatus() << endl;
  if (timed) {
    kdDebug() << "timeout..................\n";
    KpMsg("Error",i18n("Timeout: %1").arg(timeMsg), TRUE);
    delete proc; proc = 0;
    return 0;
  } else {
    if (!proc->normalExit() || proc->exitStatus()) {
      if (errorDlg) {
	KpMsg("Error",i18n("Kprocess error:%1").arg(buf), TRUE);
      }
      delete proc; proc = 0;
      return 0;
    }
  }
  delete proc;  proc = 0;
  return 1;
}











#include "procbuf.moc"
