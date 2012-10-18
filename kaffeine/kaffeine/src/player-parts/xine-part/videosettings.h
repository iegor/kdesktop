/*
 * videosettings.h
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

#ifndef VIDEOSETTINGS_H
#define VIDEOSETTINGS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kdialogbase.h>

class QSlider;

class VideoSettings : public KDialogBase
{

   Q_OBJECT
   
public: 
  VideoSettings(int hue, int sat, int contrast, int bright, int avOffset,
                int spuOffset, QWidget *parent=0, const char *name=0);
 ~VideoSettings();

signals:
  void signalNewHue(int);
  void signalNewSaturation(int);
  void signalNewContrast(int);
  void signalNewBrightness(int);
  void signalNewAVOffset(int);
  void signalNewSpuOffset(int);

private slots:

  void slotSetDefaultValues();


private:
  QSlider* m_hueSlider;
  QSlider* m_satSlider;
  QSlider* m_contrastSlider;
  QSlider* m_brightSlider;
  QSlider* m_avOffsetSlider;
  QSlider* m_spuOffsetSlider;
                
};

#endif /* VIDEOSETTINGS_H */
