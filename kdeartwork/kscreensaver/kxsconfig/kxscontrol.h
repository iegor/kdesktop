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

#ifndef __KXSCONTROL_H__
#define __KXSCONTROL_H__

#include <qwidget.h>
#include <qcheckbox.h>

#include "kxsitem.h"

class QLabel;
class QSlider;
class QSpinBox;
class QComboBox;
class QLineEdit;

//===========================================================================
class KXSRangeControl : public QWidget, public KXSRangeItem
{
  Q_OBJECT
public:
  KXSRangeControl(QWidget *parent, const QString &name, KConfig &config);
  KXSRangeControl(QWidget *parent, const QString &name, const QXmlAttributes &attr );

  virtual void read(KConfig &config);

signals:
  void changed();

protected slots:
  void slotValueChanged(int value);

protected:
  QSlider *mSlider;
  QSpinBox *mSpinBox;
};

//===========================================================================
class KXSDoubleRangeControl : public QWidget, public KXSDoubleRangeItem
{
  Q_OBJECT
public:
  KXSDoubleRangeControl(QWidget *parent, const QString &name, KConfig &config);
  KXSDoubleRangeControl(QWidget *parent, const QString &name, const QXmlAttributes &attr );

  virtual void read(KConfig &config);

signals:
  void changed();

protected slots:
  void slotValueChanged(int value);

protected:
  QSlider *mSlider;
  double  mStep;
};

//===========================================================================
class KXSCheckBoxControl : public QCheckBox, public KXSBoolItem
{
  Q_OBJECT
public:
  KXSCheckBoxControl(QWidget *parent, const QString &name, KConfig &config);
  KXSCheckBoxControl(QWidget *parent, const QString &name, const QXmlAttributes &attr );

  virtual void read(KConfig &config);

signals:
  void changed();

protected slots:
  void slotToggled(bool);
};

//===========================================================================
class KXSDropListControl : public QWidget, public KXSSelectItem
{
  Q_OBJECT
public:
  KXSDropListControl(QWidget *parent, const QString &name, KConfig &config);
  KXSDropListControl(QWidget *parent, const QString &name, const QXmlAttributes &attr );

  virtual void read(KConfig &config);

  virtual void addOption( const QXmlAttributes &attr );

signals:
  void changed();

protected slots:
  void slotActivated(int);

protected:
  QComboBox *mCombo;
};

//===========================================================================
class KXSLineEditControl : public QWidget, public KXSStringItem
{
  Q_OBJECT
public:
  KXSLineEditControl(QWidget *parent, const QString &name, KConfig &config);
  KXSLineEditControl(QWidget *parent, const QString &name, const QXmlAttributes &attr );

  virtual void read(KConfig &config);

signals:
  void changed();

protected slots:
  void textChanged(const QString &);

protected:
  QLineEdit *mEdit;
};

//===========================================================================
class KXSFileControl : public QWidget, public KXSStringItem
{
  Q_OBJECT
public:
  KXSFileControl(QWidget *parent, const QString &name, KConfig &config);
  KXSFileControl(QWidget *parent, const QString &name, const QXmlAttributes &attr );

  virtual void read(KConfig &config);

signals:
  void changed();

protected slots:
  void textChanged(const QString &);
  void selectFile();

protected:
  QLineEdit *mEdit;

};

#endif

