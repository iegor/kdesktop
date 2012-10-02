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
#ifndef KBBPREFS_H
#define KBBPREFS_H

#include <qmap.h>

#include <kconfigskeleton.h>

#include "mailsender.h"

class QStringList;

class KBBPrefs : public KConfigSkeleton
{
  public:
    virtual ~KBBPrefs();

    static KBBPrefs *instance();

  protected:
    void usrSetDefaults();
    void usrReadConfig();
    void usrWriteConfig();

    void setMessageButtonsDefault();

  private:
    KBBPrefs();

    static KBBPrefs *mInstance;

  public:
    int             mRecentPackagesCount;

    QValueList<int> mSplitter1;
    QValueList<int> mSplitter2;

    int             mMailClient;
    bool            mShowClosedBugs;
    bool            mShowWishes;
    bool            mSendBCC;
    QString         mOverrideRecipient;

    bool            mShowVoted;
    int             mMinVotes;

    int             mWrapColumn;

    QMap<QString,QString> mMessageButtons;

    int mMsgDlgWidth;
    int mMsgDlgHeight;
    QValueList<int> mMsgDlgSplitter;

    bool mDebugMode;
    
    QString mCurrentServer;
};

#endif
