/*
 * pref.h
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

#ifndef PREF_H
#define PREF_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qtoolbutton.h>

#include <kdialogbase.h>

class QCheckBox;
class QSpinBox;
class QComboBox;
class QLineEdit;

class KaffeinePreferences : public KDialogBase
{
    Q_OBJECT

public:
  KaffeinePreferences();
  virtual ~KaffeinePreferences() {}

  void setConfig(bool pauseVideo, bool tray, uint duration, bool useEncoding, const QString& encoding);
  void setDvbClient( bool enabled, const QString &address, int port, int info, const QString &tspath );

signals:
  void signalClearRecent();
  void signalEmbedSystemTray(bool);
  void signalUseAlternateEncoding(bool);
  void signalAlternateEncoding(const QString&);
  void signalSetOSDTimeout(uint);
  void signalPauseVideo(bool);
  void signalDvbClient(bool,const QString&,int,int,const QString&);

private slots:
  void slotOkPressed();
  void slotApplyPressed();
  void slotUseAlternateEncodingToggled(bool);
  void slotEmbedInTrayToggled(bool);
  void setShiftDir();

private:
  QCheckBox* m_systemTray;
  QSpinBox* m_osdTimeout;
  QCheckBox* m_useAlternateEncoding;
  QComboBox* m_alternateEncoding;
  QCheckBox* m_pauseVideo;
  QCheckBox* m_dcEnabled;
  QLineEdit* m_dcAddress;
  QSpinBox* m_dcPort;
  QSpinBox* m_dcInfo;
  QLineEdit *m_shiftDirLe;
  QToolButton *m_shiftDirBtn;
};

#endif /* PREF_H */
