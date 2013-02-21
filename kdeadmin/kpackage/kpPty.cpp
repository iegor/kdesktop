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

#include <qtimer.h>
#include <qregexp.h> 

#include <kprocctrl.h>
#include <kpty.h>
#include <kdebug.h>
#include <kpassdlg.h>

#include <kpPty.h>
#include <kpackage.h>
#include <kpTerm.h>
#include <options.h>
#include <utils.h>

#define SHPROMPT "# "
const int TIMEOUT = -3;
const int PASSWORD = -2;
const int PROMPT = -1;

extern Opts *opts;
//////////////////////////////////////////////////////////////////////////////

kpKProcIO::kpKProcIO ( QTextCodec *_codec)
  : KProcIO(_codec)
{
}

kpKProcIO::~kpKProcIO()
{
}

bool kpKProcIO::sstart (RunMode runmode)
{

  connect (this, SIGNAL (receivedStdout (KProcess *, char *, int)),
	   this, SLOT (received (KProcess *, char *, int)));


  connect (this, SIGNAL (wroteStdin(KProcess *)),
	   this, SLOT (sent (KProcess *)));

  return KProcess::start (runmode,( KProcess::Communication) ( KProcess::Stdin |  KProcess::Stdout));
}

//////////////////////////////////////////////////////////////////////////////

kpPty::kpPty() : QObject()
{
  pty = new kpKProcIO();
  pty->setUsePty(KProcess::All, false);

  connect(pty, SIGNAL(readReady(KProcIO *)), this,
		   SLOT(readLines()));
  connect(pty, SIGNAL(processExited(KProcess *)), this,
		   SLOT(done()));
  pty->pty()->setWinSize(0,80);
  tm = new QTimer(this);
  connect(tm, SIGNAL(timeout()), this, SLOT(slotTimeout()));

  eventLoop = FALSE;
  inSession = FALSE;
  pUnterm = FALSE;
  loginSession = FALSE;
  
  codec = QTextCodec::codecForLocale();
  QMap<QString, QCString> passwords;
}


kpPty::~kpPty()
{
}

void kpPty::startSu()
{
  kdDebug() << "startSu()\n";
  pty->setEnvironment("PS1", SHPROMPT);
#if defined(__FreeBSD__) || defined(__bsdi__)
  (*pty) << "su";
#else
  (*pty) << "su" << "-s" << "/bin/sh";
#endif

}

void kpPty::startSudo()
{
  kdDebug() << "startSudo()\n";
  pty->setEnvironment("PS1", SHPROMPT);
  (*pty) << "sudo" << "-p" << "Password: " << "/bin/sh";
}

void kpPty::startSsh()
{
  kdDebug() << "startSsh()\n";
  (*pty) << "/usr/bin/ssh" << "-t" << "-l" << "root";
  if (hostName.isEmpty()) {
    (*pty) << "-o" << "StrictHostKeyChecking=no" << "localhost";
  } else {
    (*pty) << hostName;
  }
  (*pty) << "env PATH=/bin:/usr/bin:/sbin:/usr/sbin:/usr/local/bin PS1='" SHPROMPT "' sh";
}

bool kpPty::needSession(bool needRoot) 
{
  return (!hostName.isEmpty() || needRoot);
}

bool kpPty::startSession(bool needRoot)
{
  bool interact = FALSE; // Have interacted with user, prevents loops
  bool passwordTried = FALSE; // Have tried the current save password, so need to put up dialog
  pUnterm = FALSE;
  kdDebug() << "kpPty::startSession\n";
  if (!inSession && needSession(needRoot)) {
    // Assume !needRoot actions are simple executables
    kdDebug() << "kpPty::startSession TRUE\n";
    loginSession = TRUE;
    int ret;
    QString s = "echo START=$?\n";

  FULL_RESTART:
    interact = FALSE;
    retList.clear();
    pty->resetAll();

    QString passMsg;
    kdDebug() << "privCmd=" << opts->privCmd << "\n";
    if (opts->privCmd == Opts::SSHcmd || !hostName.isEmpty()) {
      passMsg = i18n("The action you requested uses ssh. Please enter the password or pass phrase.\n");
      startSsh();
    } else if (opts->privCmd == Opts::SUcmd) {
      passMsg = i18n("The action you requested needs root privileges. Please enter root's password.\n");
      startSu();
    } else if (opts->privCmd == Opts::SUDOcmd) {
      passMsg = i18n("The action you requested needs root privileges. Please enter your SUDO password.\n");
      startSudo();
    }
    pty->sstart(KProcess::NotifyOnExit);

  RESTART:
    tm->start(6*1000, TRUE);
    eventLoop = TRUE;
    kdDebug() << "Loopst\n";
    kapp->enter_loop();
    kdDebug() << "Loopfn Result=" << Result  <<  "\n";
    tm->stop();
    if (Result == TIMEOUT) { // timeout
      interact = TRUE;
      //      kdDebug() << "Line=" << retList.last() << "\n";
      kpstart->addText(retList);
      kpstart->run("", i18n("Login Problem: Please login manually"));

      ret = kpstart->exec();
      kdDebug() << "Sret=" << ret << "\n";
      if (ret) {
	inSession = FALSE;
      } else {
	inSession = TRUE;
      }
    } else if (Result == PASSWORD) {  // We got a password prompt
      QCString pass;
      int res;
      interact = TRUE;
      // kdDebug() << "H=" << hostName << " PH=" << passwords[hostName] << " PT=" << passwordTried <<"\n";
      if (passwords[hostName] != 0 && !passwordTried) {
        pass = passwords[hostName];
        res = 1;
      } else {
        kdDebug() << "Passwd=" << retList.last() << "\n";
        QString msg = passMsg;
        //        kdDebug() << "privCmd=" << opts->privCmd << " host=" << hostName.isEmpty() << "\n";
        if (opts->privCmd == Opts::SSHcmd || !hostName.isEmpty()) {
          msg += retList.last();
        }
        int keep = 1;
        res = KPasswordDialog::getPassword(pass,msg,&keep);
        //     kdDebug() << "Pass=" << pass << " Keep=" << keep << " Res=" << res << "\n";
        if (keep) {
          passwords[hostName] = pass;
        } else {
          passwords.remove(hostName);
        }
      }
      pty->writeStdin(pass.append("\n"), false);
      passwordTried = TRUE;
      if (res) {
        retList.clear();
        goto RESTART;
      } else {
        inSession = FALSE;
      }
    } else if (Result == PROMPT) { // Got Prompt
      inSession = TRUE;
      kdDebug() << "kpPty::startSession TRUE\n";
    } else {  // process return code
      pty->writeStdin(QCString("\04"), false);  // SU doesn't listen to ^C
      if (interact) {
	goto FULL_RESTART;
      } else {
	QString errMsg = retList.join(" ");
	KpMsgE(errMsg, TRUE);
        inSession = FALSE;
      }
    }
  } else {
    kdDebug() << "kpPty::startSession Not needed\n";
  }
  
  loginSession = FALSE;
  if (!inSession)
    close();
  
  return inSession;
}

void kpPty::breakUpCmd(const QString &cmd)
{
  kdDebug() << " kpPty::run CMD=\""<< cmd <<"\" pty = " << pty << endl;

  bool quote = FALSE;
  QString s;
  QStringList cl = QStringList::split(" ", cmd);

  for ( QStringList::Iterator it = cl.begin(); it != cl.end(); ++it ) {
    int lastPt = (*it).length() - 1;
    if ((*it)[0] == '\'') { // Start of quoted string
      s = *it;
      if ((*it)[lastPt] == '\'') { // Also End of quoted string
	s.replace("'","");
	(*pty) << s;
	quote = FALSE;
      } else {
	s += " ";
	quote = TRUE;
      }
    } else if ((*it)[lastPt] == '\'') { // End of quoted string
      s += *it;
      s.replace("'","");
      (*pty) << s;
      quote = FALSE;
    }	else if (quote) {
      s += *it;
      s += " ";
    } else {
      (*pty) << (*it);
    }
  }
}

QStringList kpPty::run(const QString &cmd, bool inLoop, bool needRoot)
{
  Result = 0;

  pUnterm = FALSE;

  if (!inSession && !needSession(needRoot)) {
    // Assume !needRoot actions are simple executables
    pty->resetAll();
    breakUpCmd(cmd);
    pty->setEnvironment("TERM", "dumb");
    if (!pty->sstart(KProcess::NotifyOnExit)) {
      kdDebug() << " kpPty::run execute=0\n";
      return 0;
    }
  } else {
    if (startSession(needRoot)) {
      kdDebug() << "CMDroot='"<< cmd <<"'\n";
      QString s = cmd + ";echo RESULT=$?";
      pty->writeStdin(s);
      kdDebug() << " kpPty::run session\n";
    } else {
      kdDebug() << " kpPty::run other=0\n";
      return 0;
    }
  }

  retList.clear();

  if (inLoop) {
    eventLoop = TRUE;
    kapp->enter_loop();

    return retList;
  } else {
    return 0;
  }
}

void kpPty::close() {
  //  kdDebug() << "kpPty::close\n";

  pty->closeAll();
  while(pty->isRunning()) {
    KProcessController::theKProcessController->waitForProcessExit(1);
  }
  inSession = false;
}

void kpPty::finish(int ret)
{
   kdDebug() << "kpPty::finish " << ret << "\n";

  QStringList::Iterator l;
  Result = ret;

  if (ret == PROMPT) {  // Called program executed in session
    if (!retList.empty()) {
      l = retList.fromLast();
      if ((*l).right(2) == SHPROMPT) {
	retList.remove(l);                  // Remove prompt
      }
    }

    if (!retList.empty()) {
      int p;
      l = retList.fromLast();
      if ((p = (*l).find("RESULT=")) >= 0) {
	ret = (*l).mid(p+7).toInt(0,10);
	retList.remove(l);                  // Remove return code
      } else {
	ret = 666;
      }
    }

    if (!retList.empty()) {
      l = retList.begin();
      if ( l !=  retList.end()) {
	if ((*l).find("RESULT=") >= 0) {
	  retList.remove(l);                  // Remove command at start
	}
      }
    }
  } 
  emit result(retList,ret);
 

  if (eventLoop) {
    eventLoop = FALSE;
    kapp->exit_loop();
  }
}

void kpPty::readLines()
{
  bool unterm = FALSE;

  QString stext;
  while(pty->readln(stext, false, &unterm) >= 0)
  {
    stext = codec->toUnicode(stext.ascii(), stext.length());
    emit textIn(stext, !unterm);
//              kdDebug() << "[" << pUnterm << "-" << unterm << "-" << stext << ">\n";
    if (pUnterm) {
      QStringList::Iterator lst = retList.fromLast();
      if (lst != retList.end())
      {
        stext = *lst + stext;
        retList.remove(lst);
      }
    }
    int i;
    if (!unterm)
    {
      while (stext.endsWith("\r")) {
          stext.truncate(stext.length()-1);
      }
       
      i = stext.findRev('\r');
      if (i > -1) {
        stext = stext.mid(i+1);
      }
    }

    pUnterm = unterm;

    retList << stext;
 //   kdDebug() << "++" << stext << "\n";
    if (stext.right(2) == SHPROMPT) {  // Shell prompt
      emit textIn("\r \n", false);
      finish(PROMPT);
    } else if (loginSession)  {
      QRegExp rx( "^[^:]+:[\\s]*$");  // Password prompt
      if (rx.search(retList.last()) >= 0) {
        kdDebug() << loginSession << " " <<retList.last()<< " Match password p\n";
	finish(PASSWORD);
      }
    }
  }
  pty->ackRead();
}

void kpPty::keyOut(char ch)
{
  QCString s(2);
  s[0] = ch;
  s[1] = '\0';
  pty->writeStdin(s, false);
}

void kpPty::done()
{
  int ret = pty->exitStatus();
  QString stext;

  //kdDebug() << "Done (" << ret << ")" << endl;

  finish(ret);
}

void kpPty::slotTimeout()
{
  kdDebug() << "Timeout..............\n";
  finish(TIMEOUT);
}
#include "kpPty.moc"
