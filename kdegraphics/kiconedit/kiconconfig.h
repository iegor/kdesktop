/*
    KDE Icon Editor - a small icon drawing program for the KDE.
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/  

#ifndef __KICONCONFIG_H__
#define __KICONCONFIG_H__

#include <qpixmap.h>
#include <kdialogbase.h>

#include "knew.h"
#include "utils.h"
#include "properties.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

class KAccel;
class KKeyChooser;
class KColorButton;
class KURLRequester;
class QComboBox;

class KTemplateEditDlg : public KDialogBase
{
  Q_OBJECT
public:
  KTemplateEditDlg(QWidget *);
  ~KTemplateEditDlg() {};

  QString name();
  QString path();
  void setName(const QString &);
  void setPath(const QString &);
  
public slots:
  void slotTextChanged();

protected:
  QLineEdit *ln_name;
  KURLRequester *ln_path;
};

class KTemplateConfig : public QWidget
{
  Q_OBJECT
public:
  KTemplateConfig(QWidget*);
  ~KTemplateConfig();

  void saveSettings();

protected slots:
  void add();
  void edit();
  void remove();
  void checkSelection(int);

protected:
  KIconListBox *templates;
  QPushButton *btadd, *btedit, *btremove;
};

class KBackgroundConfig : public QWidget
{
  Q_OBJECT
public:
  KBackgroundConfig(QWidget *parent);
  ~KBackgroundConfig();

public slots:
  void saveSettings();
  void selectColor(const QColor & newColor);
  void selectPixmap();

signals:

protected slots:
  void slotBackgroundMode(int);

protected:
  KColorButton *btcolor;
  QPushButton *btpix;
  QPixmap pix;
  QString pixpath;
  QWidget::BackgroundMode bgmode;
  QLabel *lb_ex;
};

class KMiscConfig : public QWidget
{
  Q_OBJECT
public:
  KMiscConfig(QWidget *parent);
  ~KMiscConfig();

public slots:
  void saveSettings();
  void pasteMode(bool);
  void showRulers(bool);

protected slots:
  void slotTransparencyDisplayType(int);

signals:

protected:
  bool pastemode, showrulers;
  QRadioButton *rbp;
  KColorButton *m_solidColorButton;
  KColorButton *m_checkerboardColor1Button;
  KColorButton *m_checkerboardColor2Button;
  QComboBox *m_checkerboardSizeCombo;
};

class KIconConfig : public KDialogBase
{
  Q_OBJECT

public:
  
  KIconConfig(QWidget *parent);
  ~KIconConfig();

protected slots:
  void slotApply();
  void slotOk();
  void finis();

protected:
  KTemplateConfig *temps;
  KBackgroundConfig *backs;
  KMiscConfig *misc;
};

#endif //__KICONCONFIG_H__
