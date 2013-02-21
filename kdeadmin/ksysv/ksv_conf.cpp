/***************************************************************************
    begin                : Sun Oct 3 1999
    copyright            : (C) 1997-99 by Peter Putzer
    email                : putzer@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2.                              *
 *                                                                         *
 ***************************************************************************/
// KSysV Config Object

#include <kapplication.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kmainwindow.h>
#include <kconfig.h>

#include "ksv_core.h"
#include "ksv_conf.h"

KSVConfig::KSVConfig ()
  : mConfig (KGlobal::config())
{
  readSettings();
}

void KSVConfig::setPanningFactor (int val)
{
  mPanningFactor = val > 100 ? 100 : val;
  mPanningFactor = mPanningFactor < 0 ? 0 : mPanningFactor;
}

void KSVConfig::readSettings()
{
  mConfig->setGroup("Path Settings"); 
  mScriptPath = mConfig->readEntry("ScriptPath", "/etc/rc.d/init.d");
  mRunlevelPath = mConfig->readEntry("RunlevelPath", "/etc/rc.d");

  mConfig->setGroup("Other Settings");
  mShowLog = mConfig->readBoolEntry("ShowLog", true);
  mConfigured = mConfig->readBoolEntry("Configured", false);
  mPanningFactor = mConfig->readNumEntry("PanningFactor", 80);
  mShowDescription = mConfig->readBoolEntry("ShowDescription", true);

  mConfig->setGroup("Colors");
  mNewNormalColor = mConfig->readColorEntry ("New Text", &Qt::blue);
  mNewSelectedColor = mConfig->readColorEntry ("New Selected Text", &Qt::blue);
  mChangedNormalColor = mConfig->readColorEntry ("Changed Text", &Qt::red);
  mChangedSelectedColor = mConfig->readColorEntry ("Changed Selected Text", &Qt::red); 

  mConfig->setGroup("Fonts");
  QFont tmp = KGlobalSettings::generalFont();
  mServiceFont = mConfig->readFontEntry ("Service Font", &tmp);
  tmp = KGlobalSettings::fixedFont();
  mNumberFont = mConfig->readFontEntry ("Number Font", &tmp);

  readRunlevels();
}

void KSVConfig::readLegacySettings ()
{
  mConfig->setGroup("Path Settings");
  
  mScriptPath = mConfig->readEntry("ScriptPath", "/etc/rc.d/init.d");
  mRunlevelPath = mConfig->readEntry("RunlevelPath", "/etc/rc.d");

  mConfig->setGroup("Other Settings");

  mShowLog = mConfig->readBoolEntry("ShowLog", true);
  mConfigured = mConfig->readBoolEntry("Configured", false);
  mPanningFactor = mConfig->readNumEntry("PanningFactor", 80);
  mShowDescription = mConfig->readBoolEntry("ShowDescription", true);
  mConfig->setGroup("Colors");
  mNewNormalColor = mConfig->readColorEntry("New", &Qt::blue);
  mChangedNormalColor = mConfig->readColorEntry("Changed", &Qt::red);
}

void KSVConfig::writeSettings() {
  mConfig->setGroup("Path Settings");	
  mConfig->writeEntry("ScriptPath", mScriptPath);
  mConfig->writeEntry("RunlevelPath", mRunlevelPath);

  mConfig->setGroup("Other Settings");
  mConfig->writeEntry("ShowLog", mShowLog);
  mConfig->writeEntry("Configured", mConfigured);
  mConfig->writeEntry("PanningFactor", mPanningFactor);
  mConfig->writeEntry("ShowDescription", mShowDescription);

  mConfig->setGroup("Colors");
  mConfig->writeEntry("New Text", mNewNormalColor);
  mConfig->writeEntry("New Selected Text", mNewSelectedColor);
  mConfig->writeEntry("Changed Text", mChangedNormalColor);
  mConfig->writeEntry("Changed Selected Text", mChangedSelectedColor);

  mConfig->setGroup("Fonts");
  mConfig->writeEntry("Service Font", mServiceFont);
  mConfig->writeEntry("Number Font", mNumberFont);

  // save screen geometry
  KMainWindow* mw = static_cast<KMainWindow*>(kapp->mainWidget());

  if (mw)
    {
      mConfig->setGroup("Geometry");
      mConfig->writeEntry("X-Position", mw->x());
      mConfig->writeEntry("Y-Position", mw->y());
    }
  
  writeRunlevels();
  
  // flush everything
  mConfig->sync();
}

bool KSVConfig::showRunlevel (int index) const 
{
  if (mShowRunlevel.contains (index))
	return mShowRunlevel[index];
  else
	return false;
}

void KSVConfig::readRunlevels ()
{
  mConfig->setGroup ("Runlevels");
  
  QValueList<int> list = mConfig->readIntListEntry ("Show Runlevels");

  for (QValueList<int>::Iterator it = list.begin(); it != list.end(); ++it)
	mShowRunlevel[*it] = true;

  if (mShowRunlevel.isEmpty())
	for (int i = 0; i < ksv::runlevelNumber; ++i)
	  mShowRunlevel[i] = true;
}

void KSVConfig::writeRunlevels ()
{
  QValueList<int> list;

  for (QMap<int,bool>::Iterator it = mShowRunlevel.begin(); it != mShowRunlevel.end(); ++it)
	if (it.data())
	  list.append (it.key());
  
  mConfig->setGroup ("Runlevels");
  mConfig->writeEntry ("Show Runlevels", list);
}

void KSVConfig::setShowRunlevel (int index, bool state)
{
  mShowRunlevel[index] = state;
}

QPoint KSVConfig::position() const
{
  mConfig->setGroup("Geometry");
  
  return QPoint(mConfig->readNumEntry("X-Position", 0),
				mConfig->readNumEntry("Y-Position", 0));
}

void KSVConfig::setScriptPath (const QString& path)
{
  mScriptPath = path;
}

void KSVConfig::setRunlevelPath (const QString& path)
{
  mRunlevelPath = path;
}

KSVConfig* KSVConfig::self()
{
  static KSVConfig conf;
  
  return &conf;
}

void KSVConfig::setNewNormalColor (const QColor& col)
{
  mNewNormalColor = col;
}

void KSVConfig::setNewSelectedColor (const QColor& col)
{
  mNewSelectedColor = col;
}

void KSVConfig::setChangedNormalColor (const QColor& col)
{
  mChangedNormalColor = col;
}

void KSVConfig::setChangedSelectedColor (const QColor& col)
{
  mChangedSelectedColor = col;
}

void KSVConfig::setNumberFont (const QFont& font)
{
  mNumberFont = font;
}

void KSVConfig::setServiceFont (const QFont& font)
{
  mServiceFont = font;
}

void KSVConfig::setShowMessage (ksv::Messages msg, bool on)
{
  mConfig->setGroup("Notification Messages");
  mConfig->writeEntry (ksv::notifications[msg], on);
}

bool KSVConfig::showMessage (ksv::Messages msg) const
{
  mConfig->setGroup("Notification Messages");
  return mConfig->readBoolEntry (ksv::notifications[msg], true);
}

