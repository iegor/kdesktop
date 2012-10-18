/*
 * equalizer.cpp
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
#include <kdebug.h>

#include <qlayout.h>
#include <qtooltip.h>
#include <qlabel.h>

#include "equalizer.h"
#include "equalizer.moc"

Equalizer::Equalizer(QWidget *parent, const char *name)
            : KDialogBase(KDialogBase::Plain, i18n("Equalizer Settings"), KDialogBase::Default | KDialogBase::Close, KDialogBase::Close, parent, name, false)
{
  setInitialSize(QSize(450,250), true);
  reparent(parent, pos(), false);
  QWidget* page = plainPage();

  QGridLayout* mainGrid = new QGridLayout( page, 3, 1 );

  on = new QCheckBox( i18n("On"), page );
  mainGrid->addWidget( on, 0, 0 );
  connect(on, SIGNAL(toggled(bool)), this, SLOT(slotSetEnabled(bool)));
  
  volumeGain = new QCheckBox( i18n("Volume gain"), page );
  QToolTip::add(volumeGain, i18n("Volume Gain for Equalizer - If the sound becomes noisy disable this"));
  mainGrid->addWidget( volumeGain, 1, 0 );
  connect(volumeGain, SIGNAL(toggled(bool)), this, SIGNAL(signalSetVolumeGain(bool)));
  
  equalGroup = new QGroupBox( QString::null, page );
  mainGrid->addWidget( equalGroup, 2, 0 );

  QGridLayout* equalGrid = new QGridLayout(equalGroup, 2, 10);
  equalGrid->setSpacing(5);
  equalGrid->setMargin(10);

  QLabel* eq30Text = new QLabel("30Hz", equalGroup);
  eq30Slider = new QSlider(Qt::Vertical, equalGroup);
  eq30Slider->setRange(-100, -1);
  eq30Slider->setSteps(1, 10);
  eq30Slider->setTickInterval(50);
  eq30Slider->setTickmarks(QSlider::Right);
  connect(eq30Slider, SIGNAL(valueChanged(int)), this, SIGNAL(signalNewEq30(int)));
  equalGrid->addWidget(eq30Text, 1, 0);
  equalGrid->addWidget(eq30Slider, 0, 0);

  QLabel* eq60Text = new QLabel("60Hz", equalGroup);
  eq60Slider = new QSlider(Qt::Vertical, equalGroup);
  eq60Slider->setRange(-100, -1);
  eq60Slider->setSteps(1, 10);
  connect(eq60Slider, SIGNAL(valueChanged(int)), this, SIGNAL(signalNewEq60(int)));
  equalGrid->addWidget(eq60Text, 1, 1);
  equalGrid->addWidget(eq60Slider, 0, 1);

  QLabel* eq125Text = new QLabel("125Hz", equalGroup);
  eq125Slider = new QSlider(Qt::Vertical, equalGroup);
  eq125Slider->setRange(-100, -1);
  eq125Slider->setSteps(1, 10);
  connect(eq125Slider, SIGNAL(valueChanged(int)), this, SIGNAL(signalNewEq125(int)));
  equalGrid->addWidget(eq125Text, 1, 2);
  equalGrid->addWidget(eq125Slider, 0, 2);

  QLabel* eq250Text = new QLabel("250Hz", equalGroup);
  eq250Slider = new QSlider(Qt::Vertical, equalGroup);
  eq250Slider->setRange(-100, -1);
  eq250Slider->setSteps(1, 10);
  connect(eq250Slider, SIGNAL(valueChanged(int)), this, SIGNAL(signalNewEq250(int)));
  equalGrid->addWidget(eq250Text, 1, 3);
  equalGrid->addWidget(eq250Slider, 0, 3);

  QLabel* eq500Text = new QLabel("500Hz", equalGroup);
  eq500Slider = new QSlider(Qt::Vertical, equalGroup);
  eq500Slider->setRange(-100, -1);
  eq500Slider->setSteps(1, 10);
  connect(eq500Slider, SIGNAL(valueChanged(int)), this, SIGNAL(signalNewEq500(int)));
  equalGrid->addWidget(eq500Text, 1, 4);
  equalGrid->addWidget(eq500Slider, 0, 4);

  QLabel* eq1kText = new QLabel("1kHz", equalGroup);
  eq1kSlider = new QSlider(Qt::Vertical, equalGroup);
  eq1kSlider->setRange(-100, -1);
  eq1kSlider->setSteps(1, 10);
  connect(eq1kSlider, SIGNAL(valueChanged(int)), this, SIGNAL(signalNewEq1k(int)));
  equalGrid->addWidget(eq1kText, 1, 5);
  equalGrid->addWidget(eq1kSlider, 0, 5);

  QLabel* eq2kText = new QLabel("2kHz", equalGroup);
  eq2kSlider = new QSlider(Qt::Vertical, equalGroup);
  eq2kSlider->setRange(-100, -1);
  eq2kSlider->setSteps(1, 10);
  connect(eq2kSlider, SIGNAL(valueChanged(int)), this, SIGNAL(signalNewEq2k(int)));
  equalGrid->addWidget(eq2kText, 1, 6);
  equalGrid->addWidget(eq2kSlider, 0, 6);

  QLabel* eq4kText = new QLabel("4kHz", equalGroup);
  eq4kSlider = new QSlider(Qt::Vertical, equalGroup);
  eq4kSlider->setRange(-100, -1);
  eq4kSlider->setSteps(1, 10);
  connect(eq4kSlider, SIGNAL(valueChanged(int)), this, SIGNAL(signalNewEq4k(int)));
  equalGrid->addWidget(eq4kText, 1, 7);
  equalGrid->addWidget(eq4kSlider, 0, 7);

  QLabel* eq8kText = new QLabel("8kHz", equalGroup);
  eq8kSlider = new QSlider(Qt::Vertical, equalGroup);
  eq8kSlider->setRange(-100, -1);
  eq8kSlider->setSteps(1, 10);
  connect(eq8kSlider, SIGNAL(valueChanged(int)), this, SIGNAL(signalNewEq8k(int)));
  equalGrid->addWidget(eq8kText, 1, 8);
  equalGrid->addWidget(eq8kSlider, 0, 8);

  QLabel* eq16kText = new QLabel("16kHz", equalGroup);
  eq16kSlider = new QSlider(Qt::Vertical, equalGroup);
  eq16kSlider->setRange(-100, -1);
  eq16kSlider->setSteps(1, 10);
  eq16kSlider->setTickInterval(50);
  eq16kSlider->setTickmarks(QSlider::Left);
  connect(eq16kSlider, SIGNAL(valueChanged(int)), this, SIGNAL(signalNewEq16k(int)));
  equalGrid->addWidget(eq16kText, 1, 9);
  equalGrid->addWidget(eq16kSlider, 0, 9);

  connect(this, SIGNAL(defaultClicked()), this, SLOT(slotSetDefaultValues()));
}


Equalizer::~Equalizer()
{
}


void Equalizer::slotSetEnabled( bool enabled )
{
   equalGroup->setEnabled( enabled );
   volumeGain->setEnabled( enabled );
   enableButton( KDialogBase::Default, enabled );

   if (enabled)
   {
     emit signalSetVolumeGain( volumeGain->isChecked() );
     emit signalNewEq30( eq30Slider->value() );
     emit signalNewEq60( eq60Slider->value() );
     emit signalNewEq125( eq125Slider->value() );
     emit signalNewEq250( eq250Slider->value() );
     emit signalNewEq500( eq500Slider->value() );
     emit signalNewEq1k( eq1kSlider->value() );
     emit signalNewEq2k( eq2kSlider->value() );
     emit signalNewEq4k( eq4kSlider->value() );
     emit signalNewEq8k( eq8kSlider->value() );
     emit signalNewEq16k( eq16kSlider->value() );
   }
   else
   {
     emit signalSetVolumeGain( false );
     emit signalNewEq30( 0 );
     emit signalNewEq60( 0 );
     emit signalNewEq125( 0 );
     emit signalNewEq250( 0 );
     emit signalNewEq500( 0 );
     emit signalNewEq1k( 0 );
     emit signalNewEq2k( 0 );
     emit signalNewEq4k( 0 );
     emit signalNewEq8k( 0 );
     emit signalNewEq16k( 0 );
   } 
}   


void Equalizer::slotSetDefaultValues()
{
  eq30Slider->setValue(-50);
  eq60Slider->setValue(-50);
  eq125Slider->setValue(-50);
  eq250Slider->setValue(-50);
  eq500Slider->setValue(-50);
  eq1kSlider->setValue(-50);
  eq2kSlider->setValue(-50);
  eq4kSlider->setValue(-50);
  eq8kSlider->setValue(-50);
  eq16kSlider->setValue(-50);
}


/******* read from config-file ********/
void Equalizer::ReadValues(KConfig* config)
{
  config->setGroup("Equalizer");

  bool enabled = config->readBoolEntry( "Enabled", false );
  on->setChecked( enabled );

  bool gain = config->readBoolEntry( "Volume Gain", true );
  volumeGain->setChecked( gain );
  
  eq30Slider->setValue( config->readNumEntry("30Hz", -50) );
  eq60Slider->setValue( config->readNumEntry("60Hz", -50) );
  eq125Slider->setValue( config->readNumEntry("125Hz", -50) );
  eq250Slider->setValue( config->readNumEntry("250Hz", -50) );
  eq500Slider->setValue( config->readNumEntry("500Hz", -50) );
  eq1kSlider->setValue( config->readNumEntry("1kHz", -50) );
  eq2kSlider->setValue( config->readNumEntry("2kHz", -50) );
  eq4kSlider->setValue( config->readNumEntry("4kHz", -50) );
  eq8kSlider->setValue( config->readNumEntry("8kHz", -50) );
  eq16kSlider->setValue( config->readNumEntry("16kHz", -50) );

  if (!enabled)
    slotSetEnabled( false );
}


/************** save in config file *************/
void Equalizer::SaveValues(KConfig* config)
{
  config->setGroup("Equalizer");

  config->writeEntry( "Enabled", on->isChecked());
  config->writeEntry( "Volume Gain", volumeGain->isChecked());
  config->writeEntry("30Hz", eq30Slider->value());
  config->writeEntry("60Hz", eq60Slider->value());
  config->writeEntry("125Hz", eq125Slider->value());
  config->writeEntry("250Hz", eq250Slider->value());
  config->writeEntry("500Hz", eq500Slider->value());
  config->writeEntry("1kHz", eq1kSlider->value());
  config->writeEntry("2kHz", eq2kSlider->value());
  config->writeEntry("4kHz", eq4kSlider->value());
  config->writeEntry("8kHz", eq8kSlider->value());
  config->writeEntry("16kHz", eq16kSlider->value());
}  
