/*
 * equalizer.h
 *
 * Copyright (C) 2003-2004 Jürgen Kofler <kaffeine@gmx.net>
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

#ifndef EQUALIZER_H
#define EQUALIZER_H

#include <kdialogbase.h>
#include <kconfig.h>

#include <qwidget.h>
#include <qslider.h>
#include <qgroupbox.h>
#include <qcheckbox.h>

/**equalizer widget
  *@author Juergen  Kofler
  */

class Equalizer : public KDialogBase  {
   Q_OBJECT
public: 
  Equalizer(QWidget *parent=0, const char *name=0);
  ~Equalizer();

  void ReadValues(KConfig* config);
  void SaveValues(KConfig* config);

signals:
  void signalNewEq30(int);
  void signalNewEq60(int);
  void signalNewEq125(int);
  void signalNewEq250(int);
  void signalNewEq500(int);
  void signalNewEq1k(int);
  void signalNewEq2k(int);
  void signalNewEq4k(int);
  void signalNewEq8k(int);
  void signalNewEq16k(int);
  void signalSetVolumeGain(bool);

  
private slots:
  void slotSetDefaultValues();
  void slotSetEnabled( bool );

private:
  QCheckBox* on;
  QCheckBox* volumeGain;
  QGroupBox* equalGroup;
  QSlider* eq30Slider;
  QSlider* eq60Slider;
  QSlider* eq125Slider;
  QSlider* eq250Slider;
  QSlider* eq500Slider;
  QSlider* eq1kSlider;
  QSlider* eq2kSlider;
  QSlider* eq4kSlider;
  QSlider* eq8kSlider;
  QSlider* eq16kSlider;
};

#endif /* EQUALIZER_H */
