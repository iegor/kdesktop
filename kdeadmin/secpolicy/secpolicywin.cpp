/*
 * Copyright (c) 1999 by Preston Brown <pbrown@kde.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <kglobal.h>
#include <klocale.h>
#include <kapplication.h>
#include <kmenubar.h>
#include <kdialog.h>
#include <dcopclient.h>
#include <qpopupmenu.h>
#include <kstdaccel.h>

#include "pamview.h"
#include "secpolicywin.h"

SecPolicyWin::SecPolicyWin(const char *name)
  : KMainWindow(0,name)
{
  view = new PamView(this);
  setCentralWidget(view);

  KMenuBar *mBar = menuBar();
  QPopupMenu *fileMenu = new QPopupMenu(this);
	fileMenu->insertItem(i18n("&Quit"), kapp, SLOT(closeAllWindows()), KStdAccel::shortcut(KStdAccel::Quit));
  mBar->insertItem(i18n("&File"), fileMenu);
  
  connect(kapp, SIGNAL(lastWindowClosed()), kapp, SLOT(quit()));

  adjustSize();
  // work around KTMainWindow sizing bug.
  resize(sizeHint().width(), sizeHint().height() + mBar->height() +
	 KDialog::marginHint());
  show();
}

SecPolicyWin::~SecPolicyWin()
{
}

void SecPolicyWin::closeEvent(QCloseEvent *e)
{
  e->accept();
}
