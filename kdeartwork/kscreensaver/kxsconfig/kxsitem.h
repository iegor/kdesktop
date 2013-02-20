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

#ifndef __KXSITEM_H__
#define __KXSITEM_H__

#include <kconfig.h>

class QXmlAttributes;

class KXSConfigItem
{
public:
  KXSConfigItem(const QString &name, KConfig &config);
  KXSConfigItem(const QString &name, const QXmlAttributes &attr);
  virtual ~KXSConfigItem() {}

  virtual QString command() = 0;
  virtual void save(KConfig &config) = 0;
  virtual void read(KConfig &config) = 0;

protected:
  QString mName;
  QString mLabel;
};

class KXSRangeItem : public KXSConfigItem
{
public:
  KXSRangeItem(const QString &name, KConfig &config);
  KXSRangeItem(const QString &name, const QXmlAttributes &attr);

  virtual QString command();
  virtual void save(KConfig &config);
  virtual void read(KConfig &config);

protected:
  QString mSwitch;
  int mMinimum;
  int mMaximum;
  int mValue;
  bool mInvert;
};

class KXSDoubleRangeItem : public KXSConfigItem
{
public:
  KXSDoubleRangeItem(const QString &name, KConfig &config);
  KXSDoubleRangeItem(const QString &name, const QXmlAttributes &attr);

  virtual QString command();
  virtual void read(KConfig &config);
  virtual void save(KConfig &config);

protected:
  QString mSwitch;
  double mMinimum;
  double mMaximum;
  double mValue;
  bool mInvert;
};

class KXSBoolItem : public KXSConfigItem
{
public:
  KXSBoolItem(const QString &name, KConfig &config);
  KXSBoolItem(const QString &name, const QXmlAttributes &attr);

  virtual QString command();
  virtual void read(KConfig &config);
  virtual void save(KConfig &config);

protected:
  QString mSwitchOn;
  QString mSwitchOff;
  bool    mValue;
};

class KXSSelectItem : public KXSConfigItem
{
public:
  KXSSelectItem(const QString &name, KConfig &config);
  KXSSelectItem(const QString &name, const QXmlAttributes &attr);

  virtual void addOption(const QXmlAttributes &attr);

  virtual QString command();

  virtual void read(KConfig &config);
  virtual void save(KConfig &config);

protected:
  QStringList mOptions;
  QStringList mSwitches;
  int         mValue;
};

class KXSStringItem : public KXSConfigItem
{
public:
  KXSStringItem(const QString &name, KConfig &config);
  KXSStringItem(const QString &name, const QXmlAttributes &attr);

  virtual QString command();
  virtual void save(KConfig &config);
  virtual void read(KConfig &config);

protected:
  QString mSwitch;
  QString mValue;
};

#endif

