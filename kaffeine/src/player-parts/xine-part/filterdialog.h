/*
 * filterdialog.h - config dialog for postprocessing filters
 *
 * Copyright (C) 2003-2005 Jürgen Kofler <kaffeine@gmx.net>
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

#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <kdialogbase.h>

class KComboBox;
class QVBox;
class QString;
class QStringList;

/**
  *@author Juergen  Kofler
  */

class FilterDialog : public KDialogBase
{
   Q_OBJECT
public: 
  FilterDialog(const QStringList& audiofilters, const QStringList& videofilters, QWidget *parent=0, const char *name=0);
  ~FilterDialog();

signals:
  void signalCreateAudioFilter(const QString& name, QWidget* parent);
  void signalRemoveAllAudioFilters();
  void signalUseAudioFilters(bool);
  void signalCreateVideoFilter(const QString& name, QWidget* parent);
  void signalRemoveAllVideoFilters();
  void signalUseVideoFilters(bool);
  
private slots:
  void slotAddAudioClicked() { emit signalCreateAudioFilter(m_audioFilterCombo->currentText(), (QWidget*)m_audioFilterPage); }
  void slotUseAudioFilters(bool on);
  void slotAddVideoClicked() { emit signalCreateVideoFilter(m_videoFilterCombo->currentText(), (QWidget*)m_videoFilterPage); }
  void slotUseVideoFilters(bool on);

private:
  KComboBox* m_audioFilterCombo;      
  QVBox* m_audioFilterPage;
  KPushButton* m_addAudioButton;
  KPushButton* m_removeAudioButton;  
  
  KComboBox* m_videoFilterCombo;      
  QVBox* m_videoFilterPage;
  KPushButton* m_addVideoButton;
  KPushButton* m_removeVideoButton;
};

#endif /* FILTERDIALOG_H */
