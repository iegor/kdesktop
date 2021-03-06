/*
    KNode, the KDE newsreader
    Copyright (c) 1999-2005 the KNode authors.
    See file AUTHORS for details

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software Foundation,
    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, US
*/

#include <qstring.h>

#include <kwin.h>
#include <kscoringeditor.h>

#include "knscoring.h"
#include "knaccountmanager.h"
#include "kngroupmanager.h"
#include "utilities.h"
#include "knglobals.h"

//----------------------------------------------------------------------------
NotifyCollection* KNScorableArticle::notifyC = 0;

KNScorableArticle::KNScorableArticle(KNRemoteArticle* a)
  : ScorableArticle(), _a(a)
{
}


KNScorableArticle::~KNScorableArticle()
{
}


void KNScorableArticle::addScore(short s)
{
  _a->setScore(_a->score()+s);
  _a->setChanged(true);
}

void KNScorableArticle::changeColor(const QColor& c)
{
  _a->setColor(c);
}

void KNScorableArticle::displayMessage(const QString& s)
{
  if (!_a->isNew()) return;
  if (!notifyC) notifyC = new NotifyCollection();
  notifyC->addNote(*this,s);
}

QString KNScorableArticle::from() const
{
  return _a->from()->asUnicodeString();
}


QString KNScorableArticle::subject() const
{
  return _a->subject()->asUnicodeString();
}


QString KNScorableArticle::getHeaderByType(const QString& s) const
{
  KMime::Headers::Base *h = _a->getHeaderByType(s.latin1());
  if (!h) return "";
  QString t = _a->getHeaderByType(s.latin1())->asUnicodeString();
  Q_ASSERT( !t.isEmpty() );
  return t;
}


void KNScorableArticle::markAsRead()
{
  _a->setRead();
}

//----------------------------------------------------------------------------

KNScorableGroup::KNScorableGroup()
{
}


KNScorableGroup::~KNScorableGroup()
{
}

//----------------------------------------------------------------------------

KNScoringManager::KNScoringManager() : KScoringManager("knode")
{
}


KNScoringManager::~KNScoringManager()
{
}


QStringList KNScoringManager::getGroups() const
{
  KNAccountManager *am = knGlobals.accountManager();
  QStringList res;
  QValueList<KNNntpAccount*>::Iterator it;
  for ( it = am->begin(); it != am->end(); ++it ) {
    QStringList groups;
    knGlobals.groupManager()->getSubscribed( (*it), groups);
    res += groups;
  }
  res.sort();
  return res;
}


QStringList KNScoringManager::getDefaultHeaders() const
{
  QStringList l = KScoringManager::getDefaultHeaders();
  l << "Lines";
  l << "References";
  return l;
}


void KNScoringManager::configure()
{
  KScoringEditor *dlg = KScoringEditor::createEditor(this, knGlobals.topWidget);

  if (dlg) {
    dlg->show();
    KWin::activateWindow(dlg->winId());
  }
}

#include "knscoring.moc"
