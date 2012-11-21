//-----------------------------------------------------------------------------
//
// KDE xscreensaver configuration dialog
//
// Copyright (c)  Martin R. Jones <mjones@kde.org> 1999
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation;
// version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#include "kxsconfig.h"
#include <klocale.h>
#include <qxml.h>

//===========================================================================
KXSConfigItem::KXSConfigItem(const QString &name, KConfig &config)
  : mName(name)
{
  config.setGroup(name);
  mLabel = i18n(config.readEntry("Label").utf8());
}

KXSConfigItem::KXSConfigItem(const QString &name, const QXmlAttributes &attr )
  : mName(name)
{
  mLabel = attr.value("_label");
}

//===========================================================================
KXSRangeItem::KXSRangeItem(const QString &name, KConfig &config)
  : KXSConfigItem(name, config), mInvert(false)
{
  mMinimum = config.readNumEntry("Minimum");
  mMaximum = config.readNumEntry("Maximum");
  mValue   = config.readNumEntry("Value");
  mSwitch  = config.readEntry("Switch");
}

KXSRangeItem::KXSRangeItem(const QString &name, const QXmlAttributes &attr )
  : KXSConfigItem(name, attr), mInvert(false)
{
  mMinimum = attr.value("low").toInt();
  mMaximum = attr.value("high").toInt();
  mValue   = attr.value("default").toInt();
  mSwitch  = attr.value("arg");
  int pos = mSwitch.find( '%' );
  if (pos >= 0)
    mSwitch.insert(pos+1, '1');
  if ( attr.value("convert") == "invert" )
    mInvert = true;
  if (mInvert)
    mValue = mMaximum-(mValue-mMinimum);
}

QString KXSRangeItem::command()
{
  return mSwitch.arg(mInvert?mMaximum-(mValue-mMinimum):mValue);
}

void KXSRangeItem::read(KConfig &config)
{
  config.setGroup(mName);
  if (config.hasKey("Value"))
      mValue = config.readNumEntry("Value");
}

void KXSRangeItem::save(KConfig &config)
{
  config.setGroup(mName);
  config.writeEntry("Value", mValue);
}

//===========================================================================
KXSDoubleRangeItem::KXSDoubleRangeItem(const QString &name, KConfig &config)
  : KXSConfigItem(name, config), mInvert(false)
{
  mMinimum = config.readDoubleNumEntry("Minimum");
  mMaximum = config.readDoubleNumEntry("Maximum");
  mValue   = config.readDoubleNumEntry("Value");
  mSwitch  = config.readEntry("Switch");
}

KXSDoubleRangeItem::KXSDoubleRangeItem(const QString &name, const QXmlAttributes &attr)
  : KXSConfigItem(name, attr), mInvert(false)
{
  mMinimum = attr.value("low").toDouble();
  mMaximum = attr.value("high").toDouble();
  mValue   = attr.value("default").toDouble();
  mSwitch  = attr.value("arg");
  int pos = mSwitch.find( '%' );
  if (pos >= 0)
    mSwitch.insert(pos+1, '1');
  if ( attr.value("convert") == "invert" )
    mInvert = true;
  if (mInvert)
    mValue = mMaximum-(mValue-mMinimum);
}

QString KXSDoubleRangeItem::command()
{
  return mSwitch.arg(mInvert?mMaximum-(mValue-mMinimum):mValue);
}

void KXSDoubleRangeItem::read(KConfig &config)
{
  config.setGroup(mName);
  if (config.hasKey("Value"))
      mValue = config.readDoubleNumEntry("Value");
}

void KXSDoubleRangeItem::save(KConfig &config)
{
  config.setGroup(mName);
  config.writeEntry("Value", mValue);
}


//===========================================================================
KXSBoolItem::KXSBoolItem(const QString &name, KConfig &config)
  : KXSConfigItem(name, config)
{
  mValue = config.readBoolEntry("Value");
  mSwitchOn  = config.readEntry("SwitchOn");
  mSwitchOff = config.readEntry("SwitchOff");
}

KXSBoolItem::KXSBoolItem(const QString &name, const QXmlAttributes &attr )
  : KXSConfigItem(name, attr)
{
  mSwitchOn  = attr.value("arg-set");
  mSwitchOff = attr.value("arg-unset");
  mValue = mSwitchOn.isEmpty() ? true : false;
}

QString KXSBoolItem::command()
{
  return mValue ? mSwitchOn : mSwitchOff;
}

void KXSBoolItem::read(KConfig &config)
{
  config.setGroup(mName);
  if (config.hasKey("Value"))
      mValue = config.readBoolEntry("Value");
}

void KXSBoolItem::save(KConfig &config)
{
  config.setGroup(mName);
  config.writeEntry("Value", mValue);
}

//===========================================================================
KXSSelectItem::KXSSelectItem(const QString &name, KConfig &config)
  : KXSConfigItem(name, config)
{
  mOptions = config.readListEntry("Options");
  mSwitches = config.readListEntry("Switches");
  mValue = config.readNumEntry("Value");
}

KXSSelectItem::KXSSelectItem(const QString &name, const QXmlAttributes &attr )
  : KXSConfigItem(name, attr), mValue(0)
{
}

void KXSSelectItem::addOption( const QXmlAttributes &attr )
{
    QString opt = attr.value("_label");
    QString arg = attr.value("arg-set");
    if ( arg.isEmpty() )
	mValue = mSwitches.count();
    mOptions += opt;
    mSwitches += arg;
}

QString KXSSelectItem::command()
{
  QStringList::Iterator it = mSwitches.at(mValue);
  return (*it);
}

void KXSSelectItem::read(KConfig &config)
{
  config.setGroup(mName);
  if (config.hasKey("Value"))
      mValue = config.readNumEntry("Value");
}

void KXSSelectItem::save(KConfig &config)
{
  config.setGroup(mName);
  config.writeEntry("Value", mValue);
}


//===========================================================================
KXSStringItem::KXSStringItem(const QString &name, KConfig &config)
  : KXSConfigItem(name, config)
{
  mValue = config.readEntry("Value");
  mSwitch = config.readEntry("Switch");
  int pos = mSwitch.find( '%' );
  if (pos >= 0) {
    mSwitch.insert(pos+1, "\"");
    mSwitch.insert(pos, "\"");
  }
}

KXSStringItem::KXSStringItem(const QString &name, const QXmlAttributes &attr )
  : KXSConfigItem(name, attr)
{
  mSwitch = attr.value("arg");
  int pos = mSwitch.find( '%' );
  if (pos >= 0) {
    mSwitch.insert(pos+1, "1\"");
    mSwitch.insert(pos, "\"");
  }
}

QString KXSStringItem::command()
{
  if (!mValue.isEmpty())
      return mSwitch.arg(mValue);
  return "";
}

void KXSStringItem::read(KConfig &config)
{
  config.setGroup(mName);
  if (config.hasKey("Value"))
      mValue = config.readEntry("Value");
}

void KXSStringItem::save(KConfig &config)
{
  config.setGroup(mName);
  config.writeEntry("Value", mValue);
}

