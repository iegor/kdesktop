/*
    This file is part of KBugBuster.

    Copyright (c) 2003 Cornelius Schumacher <schumacher@kde.org>

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

#include "bugserverconfig.h"

#include "kbbprefs.h"

#include <kdebug.h>
#include <kconfig.h>

BugServerConfig::BugServerConfig()
{
  mName = "KDE";
  mBaseUrl = "http://bugs.kde.org";
  mUser = "bugzilla@kde.org";
  mBugzillaVersion = "KDE";
}

BugServerConfig::BugServerConfig( const QString &name, const KURL &baseUrl )
  : mName( name ), mBaseUrl( baseUrl ), mBugzillaVersion( "KDE" )
{
}

BugServerConfig::~BugServerConfig()
{
}

void BugServerConfig::setName( const QString &name )
{
  mName = name;
}

QString BugServerConfig::name() const
{
  return mName;
}

void BugServerConfig::setBaseUrl( const KURL &baseUrl )
{
  mBaseUrl = baseUrl;
}

KURL BugServerConfig::baseUrl() const
{
  return mBaseUrl;
}

void BugServerConfig::setUser( const QString &user )
{
  mUser = user;
}

QString BugServerConfig::user() const
{
  return mUser;
}

void BugServerConfig::setPassword( const QString &password )
{
  mPassword = password;
}

QString BugServerConfig::password() const
{
  return mPassword;
}

void BugServerConfig::setBugzillaVersion( const QString &s )
{
  mBugzillaVersion = s;
}

QString BugServerConfig::bugzillaVersion() const
{
  return mBugzillaVersion;
}

QStringList BugServerConfig::bugzillaVersions()
{
  QStringList v;
  
  v << "2.10";
  v << "2.14.2";
  v << "2.16.2";
  v << "2.17.1";
  v << "KDE";
  v << "Bugworld";

  return v;
}

void BugServerConfig::readConfig( KConfig *cfg, const QString &name )
{
  mName = name;

  cfg->setGroup( "BugServer " + name );
  
  mBaseUrl = cfg->readEntry( "BaseUrl" );
  mUser = cfg->readEntry( "User" );
  mPassword = cfg->readEntry( "Password" );

  mBugzillaVersion = cfg->readEntry( "BugzillaVersion", "KDE" );

  mRecentPackages = cfg->readListEntry( "RecentPackages" );
  mCurrentPackage = cfg->readEntry( "CurrentPackage" );
  mCurrentComponent = cfg->readEntry( "CurrentComponent" );
  mCurrentBug = cfg->readEntry( "CurrentBug" );
}

void BugServerConfig::writeConfig( KConfig *cfg )
{
  cfg->setGroup( "BugServer " + mName );
  
  cfg->writeEntry( "BaseUrl", mBaseUrl.url() );
  cfg->writeEntry( "User", mUser );
  cfg->writeEntry( "Password", mPassword );

  cfg->writeEntry( "BugzillaVersion", mBugzillaVersion );

  cfg->writeEntry( "RecentPackages", mRecentPackages );
  cfg->writeEntry( "CurrentPackage", mCurrentPackage );
  cfg->writeEntry( "CurrentComponent", mCurrentComponent );
  cfg->writeEntry( "CurrentBug", mCurrentBug );
}


/*
 * vim:sw=4:ts=4:et
 */
