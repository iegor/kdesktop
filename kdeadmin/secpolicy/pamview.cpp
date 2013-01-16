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

#include <qlayout.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qlabel.h>
#include <qdir.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qregexp.h>

#include <kmessagebox.h>
#include <kglobal.h>
#include <klocale.h>

#include "ppitem.h"
#include "pamview.h"

#include <stdlib.h>

PamView::PamView(QWidget *parent, const char *name)
  : QWidget(parent, name)
{
  QHBoxLayout *topl = new QHBoxLayout(this);
  
  QVBoxLayout *leftl = new QVBoxLayout;
  topl->addLayout(leftl);
  
  QLabel *label = new QLabel(i18n("Available services:"), this);
  leftl->addWidget(label);
  
  servicesLB = new QListBox(this);
  leftl->addWidget(servicesLB);
  
  propertiesLV = new QListView(this);
  propertiesLV->addColumn(i18n("Category"));
  propertiesLV->addColumn(i18n("Level"));
  propertiesLV->addColumn(i18n("Module"));
  topl->addWidget(propertiesLV);

  init();
}

PamView::~PamView()
{
}

void PamView::init()
{
  initServices();
}

void PamView::initServices()
{

  QDir d("/etc/pam.d");
  d.setFilter(QDir::Files|QDir::Readable);

  if (!d.exists()) {
    KMessageBox::error(this, i18n("/etc/pam.d folder does not exist.\n"
				  "Either your system does not have PAM support "
				  "or there is some other configuration problem."));
    exit(1);
  }

  const QFileInfoList *list = d.entryInfoList();
  QFileInfoListIterator it(*list);
  QFileInfo *fi;
  while ((fi = it.current()) != 0) {
    servicesLB->insertItem(fi->fileName());
    QFile f(fi->filePath());
    if (f.open(IO_ReadOnly)) {
      QTextStream t(&f);
      QString s;
      while (!t.eof()) {
	s = t.readLine().stripWhiteSpace();
	if (s.isEmpty() || (s[0] == '#'))
	  continue;

	PamPropertiesItem *item = new PamPropertiesItem(fi->filePath(),
							QStringList::split(QRegExp("\\s"), s),
							propertiesLV);
	propertiesLV->insertItem(item);
      }
      f.close();
    }
    ++it;
  }
}

#include "pamview.moc"
