/*
 * deinterlacequality.h - dialog for selecting the quality of deinterlacing
 *
 * Copyright (C) 2004-2005 Jürgen Kofler <kaffeine@gmx.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef DEINTERLACEQUALITY_H
#define DEINTERLACEQUALITY_H

#include <kdialogbase.h>

/**
  *@author Juergen  Kofler
  */

class QCheckBox;
class QSlider;
class QString;
class QStringList;
class KPushButton;    

class DeinterlacerConfigDialog : public KDialogBase
{
  Q_OBJECT
public:
  DeinterlacerConfigDialog(QWidget *parent=0, const char *name=0)
  : KDialogBase( parent, name, true, i18n("Configure tvtime Deinterlace Plugin"), KDialogBase::Close )
  {
    setInitialSize(QSize(450,400), true);

    mainWidget = makeVBoxMainWidget();
  }

  ~DeinterlacerConfigDialog() {}
  QWidget* getMainWidget() const { return (QWidget*)mainWidget; }

private:
  QVBox* mainWidget;
};

class DeinterlaceQuality : public KDialogBase  {
   Q_OBJECT
public: 
  DeinterlaceQuality(QWidget* filterDialog, QWidget *parent=0, const char *name=0);
  ~DeinterlaceQuality();  

  void setQuality(uint);
  uint getQuality() const;
  
signals:
  void signalSetDeinterlaceConfig(const QString&);

private slots:
  void slotLevelChanged(int);
  void slotCustomBoxToggled(bool);

private:
  QStringList m_configStrings;
  QSlider* m_qualitySlider;
  QCheckBox* m_customBox;
  KPushButton* m_customConfigButton;
};

#endif /* DEINTERLACEQUALITY_H */
