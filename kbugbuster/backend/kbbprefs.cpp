/*
    This file is part of KBugBuster.

    Copyright (c) 2001,2003 Cornelius Schumacher <schumacher@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/

#include <qstring.h>
#include <qstringlist.h>

#include <klocale.h>
#include <kdebug.h>
#include <kconfig.h>

#include "bugsystem.h"
#include "bugserver.h"
#include "bugserverconfig.h"

#include "kbbprefs.h"

KBBPrefs *KBBPrefs::mInstance = 0;

KBBPrefs::KBBPrefs() : KConfigSkeleton()
{
  setCurrentGroup("History");

  addItemInt("RecentPackagesCount",mRecentPackagesCount,7);
  addItemIntList("Splitter1",mSplitter1);
  addItemIntList("Splitter2",mSplitter2);


  setCurrentGroup("Personal Settings");

  addItemInt("MailClient",mMailClient,MailSender::KMail,"Mail Client");
  addItemBool("ShowClosedBugs",mShowClosedBugs,false);
  addItemBool("ShowWishes",mShowWishes,true);
  addItemBool("ShowVotes", mShowVoted, false);
  addItemInt("MinimumVotes", mMinVotes, 0);
  addItemBool("SendBCC",mSendBCC,false);
  addItemString("OverrideRecipient",mOverrideRecipient,QString::null);
  addItemInt("WrapColumn",mWrapColumn,90);


  setCurrentGroup("MsgInputDlg");

  addItemInt("MsgDialogWidth",mMsgDlgWidth);
  addItemInt("MsgDialogHeight",mMsgDlgHeight);
  addItemIntList("MsgDialogSplitter",mMsgDlgSplitter);


  setCurrentGroup( "Debug" );

  addItemBool( "DebugMode", mDebugMode, false );


  setCurrentGroup( "Servers" );

  addItemString("CurrentServer",mCurrentServer);
}


KBBPrefs::~KBBPrefs()
{
  delete mInstance;
  mInstance = 0;
}


KBBPrefs *KBBPrefs::instance()
{
  if (!mInstance) {
    mInstance = new KBBPrefs();
    mInstance->readConfig();
  }

  return mInstance;
}

void KBBPrefs::usrSetDefaults()
{
  setMessageButtonsDefault();
}

void KBBPrefs::usrReadConfig()
{
  mMessageButtons.clear();

  config()->setGroup("MessageButtons");
  QStringList buttonList = config()->readListEntry("ButtonList");
  if (buttonList.isEmpty()) {
    setMessageButtonsDefault();
  } else {
    QStringList::ConstIterator it;
    for(it = buttonList.begin(); it != buttonList.end(); ++it) {
      QString text = config()->readEntry(*it);
      mMessageButtons.insert(*it,text);
    }
  }

  BugSystem::self()->readConfig( config() );
}

void KBBPrefs::usrWriteConfig()
{
  config()->setGroup("MessageButtons");
  QStringList buttonList;
  QMap<QString,QString>::ConstIterator it;
  for(it = mMessageButtons.begin();it != mMessageButtons.end();++it) {
    buttonList.append(it.key());
    config()->writeEntry(it.key(),it.data());
  }
  config()->writeEntry("ButtonList",buttonList);

  BugSystem::self()->writeConfig( config() );
}

void KBBPrefs::setMessageButtonsDefault()
{
  mMessageButtons.clear();
  mMessageButtons.insert(i18n("Bug Fixed in CVS"),"Thank you for your bug report.\n"
  "The bug that you reported has been identified and has been fixed in the\n"
  "latest development (CVS) version of KDE. The bug report will be closed.\n");
  mMessageButtons.insert(i18n("Duplicate Report"),"Thank you for your bug report.\n"
  "This bug/feature request has already been reported and this report will\n"
  "be marked as a duplicate.\n");
  mMessageButtons.insert(i18n("Packaging Bug"),"Thank you for your bug report.\n"
  "The bug that you reported appears to be a packaging bug, due to a\n"
  "problem in the way in which your distribution/vendor has packaged\n"
  "KDE for distribution.\n"
  "The bug report will be closed since it is not a KDE problem.\n"
  "Please send the bug report to your distribution/vendor instead.\n");
  mMessageButtons.insert(i18n("Feature Implemented in CVS"),"Thank you for your bug report.\n"
  "The feature that you requested has been implemented in the latest\n"
  "development (CVS) version of KDE. The feature request will be closed.\n");
  mMessageButtons.insert(i18n("More Information Required"),"Thank you for your bug report.\n"
  "You have not provided enough information for us to be able to reproduce\n"
  "the bug. Please provide a detailed account of the steps required to\n"
  "trigger and reproduce the bug. Without this information, we will not be\n"
  "able to reproduce, identify and fix the bug.\n");
  mMessageButtons.insert(i18n("No Longer Applicable"),"Thank you for your bug report.\n"
  "The bug that your reported no longer applies to the latest development\n"
  "(CVS) version of KDE. This is most probably because it has been fixed,\n"
  "the application has been substantially modified or the application no\n"
  "longer exists. The bug report will be closed.\n");
  mMessageButtons.insert(i18n("Won't Fix Bug"),"Thank you for your bug report/feature request.\n"
  "Unfortunately, this bug will never be fixed or the feature never\n"
  "implemented. The bug report/feature request will be closed.\n");
  mMessageButtons.insert(i18n("Cannot Reproduce Bug"),"Thank you for your bug report.\n"
  "This bug can not be reproduced using the current development (CVS)\n"
  "version of KDE. This suggests that the bug has already been fixed.\n"
  "The bug report will be closed.\n");
}

