/*
 * videosettings.cpp
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

#include <klocale.h>

#include <qslider.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qgroupbox.h>

#include "videosettings.h"
#include "videosettings.moc"


VideoSettings::VideoSettings(int hue, int sat, int contrast, int bright,
        int avOffset, int spuOffset, QWidget *parent, const char *name)
         : KDialogBase(KDialogBase::Plain, i18n("Video Settings"), KDialogBase::Default | KDialogBase::Close, KDialogBase::Close, parent, name, false)
{
  reparent(parent, pos(), false);
  setInitialSize(QSize(450,250), true);
  QWidget* page = plainPage();

  QVBoxLayout* b = new QVBoxLayout(page);

  QGroupBox *videoGroup = new QGroupBox(QString::null, page);
  b->addWidget(videoGroup);

  QGridLayout* videoGrid = new QGridLayout(videoGroup, 6, 2);
  videoGrid->setSpacing(5);
  videoGrid->setMargin(10);

  QLabel* hueText = new QLabel(i18n("Hue"), videoGroup);
  hueText->setAlignment(AlignRight);
  m_hueSlider = new QSlider(Qt::Horizontal, videoGroup);
  m_hueSlider->setRange(0, 65535);
  m_hueSlider->setSteps(10, 1000);
  m_hueSlider->setValue(hue);
  connect(m_hueSlider, SIGNAL(valueChanged(int)), this, SIGNAL(signalNewHue(int)));
  videoGrid->addWidget(hueText, 0, 0);
  videoGrid->addWidget(m_hueSlider, 0, 1);

  QLabel* satText = new QLabel(i18n("Saturation"), videoGroup);
  satText->setAlignment(AlignRight);
  m_satSlider = new QSlider(Qt::Horizontal, videoGroup);
  m_satSlider->setRange(0, 65535);
  m_satSlider->setSteps(10, 1000);
  m_satSlider->setValue(sat);
  connect(m_satSlider, SIGNAL(valueChanged(int)), this, SIGNAL(signalNewSaturation(int)));
  videoGrid->addWidget(satText, 1, 0);
  videoGrid->addWidget(m_satSlider, 1, 1);

  QLabel* contrastText = new QLabel(i18n("Contrast"), videoGroup);
  contrastText->setAlignment(AlignRight);
  m_contrastSlider = new QSlider(Qt::Horizontal, videoGroup);
  m_contrastSlider->setRange(0, 65535);
  m_contrastSlider->setSteps(10, 1000);
  m_contrastSlider->setValue(contrast);
  connect(m_contrastSlider, SIGNAL(valueChanged(int)), this, SIGNAL(signalNewContrast(int)));
  videoGrid->addWidget(contrastText, 2, 0);
  videoGrid->addWidget(m_contrastSlider, 2, 1);

  QLabel* brightText = new QLabel(i18n("Brightness"), videoGroup);
  brightText->setAlignment(AlignRight);
  m_brightSlider = new QSlider(Qt::Horizontal, videoGroup);
  m_brightSlider->setRange(0, 65535);
  m_brightSlider->setSteps(10, 1000);
  m_brightSlider->setValue(bright);
  connect(m_brightSlider, SIGNAL(valueChanged(int)), this, SIGNAL(signalNewBrightness(int)));
  videoGrid->addWidget(brightText, 3, 0);
  videoGrid->addWidget(m_brightSlider, 3, 1);

  QLabel* avOffsetText = new QLabel(i18n("Audio/Video Offset"), videoGroup);
  avOffsetText->setAlignment(AlignRight);
  m_avOffsetSlider = new QSlider(Qt::Horizontal, videoGroup);
  m_avOffsetSlider->setRange(-90000, 90000); // +/- 1 sec
  m_avOffsetSlider->setSteps(100, 10000);
  m_avOffsetSlider->setValue(avOffset);
  connect(m_avOffsetSlider, SIGNAL(valueChanged(int)), this, SIGNAL(signalNewAVOffset(int)));
  videoGrid->addWidget(avOffsetText, 4, 0);
  videoGrid->addWidget(m_avOffsetSlider, 4, 1);

  QLabel* spuOffsetText = new QLabel(i18n("Subtitle Offset"), videoGroup);
  spuOffsetText->setAlignment(AlignRight);
  m_spuOffsetSlider = new QSlider(Qt::Horizontal, videoGroup);
  m_spuOffsetSlider->setRange(-90000, 90000);  // +/- 1 sec
  m_spuOffsetSlider->setSteps(100, 10000);
  m_spuOffsetSlider->setValue(spuOffset);
  connect(m_spuOffsetSlider, SIGNAL(valueChanged(int)), this, SIGNAL(signalNewSpuOffset(int)));
  videoGrid->addWidget(spuOffsetText, 5, 0);
  videoGrid->addWidget(m_spuOffsetSlider, 5, 1);

  connect(this, SIGNAL(defaultClicked()), this, SLOT(slotSetDefaultValues()));
}



VideoSettings::~VideoSettings()
{

}


void VideoSettings::slotSetDefaultValues()
{
  m_hueSlider->setValue(32768);
  m_satSlider->setValue(32768);
  m_contrastSlider->setValue(32768);
  m_brightSlider->setValue(32768);
  m_avOffsetSlider->setValue(0);
  m_spuOffsetSlider->setValue(0);
}
