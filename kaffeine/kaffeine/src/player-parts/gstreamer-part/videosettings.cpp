/*
 * videosettings.cpp
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

#include <klocale.h>

#include <qslider.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qnamespace.h>

#include "videosettings.h"
#include "videosettings.moc"


VideoSettings::VideoSettings(int hue, int sat, int contr, int bright, QWidget *parent, const char *name)
         : KDialogBase(KDialogBase::Plain, i18n("Video Settings"), KDialogBase::Default | KDialogBase::Close, KDialogBase::Close, parent, name, false)
{
  setInitialSize(QSize(450,200), true);
  QWidget* page = plainPage();

  QGridLayout* grid = new QGridLayout(page, 4, 2);
  grid->setSpacing(5);

  QLabel* hueText = new QLabel(i18n("Hue"), page);
  hueText->setAlignment(AlignRight);
  m_hueSlider = new QSlider(Qt::Horizontal, page);
  m_hueSlider->setRange(-1000, 1000);
  m_hueSlider->setSteps(10, 100);
  m_hueSlider->setValue(hue);
  connect(m_hueSlider, SIGNAL(valueChanged(int)), this, SIGNAL(signalNewHue(int)));
  grid->addWidget(hueText, 0, 0);
  grid->addWidget(m_hueSlider, 0, 1);

  QLabel* satText = new QLabel(i18n("Saturation"), page);
  satText->setAlignment(AlignRight);
  m_satSlider = new QSlider(Qt::Horizontal, page);
  m_satSlider->setRange(-1000, 1000);
  m_satSlider->setSteps(10, 100);
  m_satSlider->setValue(sat);
  connect(m_satSlider, SIGNAL(valueChanged(int)), this, SIGNAL(signalNewSaturation(int)));
  grid->addWidget(satText, 1, 0);
  grid->addWidget(m_satSlider, 1, 1);

  QLabel* contrastText = new QLabel(i18n("Contrast"), page);
  contrastText->setAlignment(AlignRight);
  m_contrastSlider = new QSlider(Qt::Horizontal, page);
  m_contrastSlider->setRange(-1000, 1000);
  m_contrastSlider->setSteps(10, 100);
  m_contrastSlider->setValue(contr);
  connect(m_contrastSlider, SIGNAL(valueChanged(int)), this, SIGNAL(signalNewContrast(int)));
  grid->addWidget(contrastText, 2, 0);
  grid->addWidget(m_contrastSlider, 2, 1);

  QLabel* brightText = new QLabel(i18n("Brightness"), page);
  brightText->setAlignment(AlignRight);
  m_brightSlider = new QSlider(Qt::Horizontal, page);
  m_brightSlider->setRange(-1000, 1000);
  m_brightSlider->setSteps(10, 100);
  m_brightSlider->setValue(bright);
  connect(m_brightSlider, SIGNAL(valueChanged(int)), this, SIGNAL(signalNewBrightness(int)));
  grid->addWidget(brightText, 3, 0);
  grid->addWidget(m_brightSlider, 3, 1);

  connect(this, SIGNAL(defaultClicked()), this, SLOT(slotSetDefaultValues()));
}

VideoSettings::~VideoSettings()
{

}

void VideoSettings::slotSetDefaultValues()
{
  m_hueSlider->setValue(1000);
  m_satSlider->setValue(0);
  m_contrastSlider->setValue(0);
  m_brightSlider->setValue(0);
}
