/*
 * deinterlacequality.cpp - dialog for selecting the quality of deinterlacing
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

#include <kdebug.h>
#include <klocale.h>
#include <kpushbutton.h> 
 
#include <qcheckbox.h>
#include <qslider.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qlabel.h>
#include <qlayout.h>
 
#include "deinterlacequality.h"
#include "deinterlacequality.moc"


DeinterlaceQuality::DeinterlaceQuality(QWidget* filterDialog, QWidget *parent, const char *name)
:KDialogBase(parent, name, true, i18n("Deinterlace Quality"), KDialogBase::Close)
{
  m_configStrings << "tvtime:method=Greedy2Frame,enabled=1,pulldown=vektor,framerate_mode=full,judder_correction=1,use_progressive_frame_flag=1,chroma_filter=1,cheap_mode=0";
  m_configStrings << "tvtime:method=Greedy2Frame,enabled=1,pulldown=vektor,framerate_mode=full,judder_correction=0,use_progressive_frame_flag=1,chroma_filter=0,cheap_mode=0";
  m_configStrings << "tvtime:method=Greedy,enabled=1,pulldown=none,framerate_mode=half_top,judder_correction=0,use_progressive_frame_flag=1,chroma_filter=0,cheap_mode=0";
  m_configStrings << "tvtime:method=Greedy,enabled=1,pulldown=none,framerate_mode=half_top,judder_correction=0,use_progressive_frame_flag=1,chroma_filter=0,cheap_mode=1";
  m_configStrings << "tvtime:method=LinearBlend,enabled=1,pulldown=none,framerate_mode=half_top,judder_correction=0,use_progressive_frame_flag=1,chroma_filter=0,cheap_mode=1";
  m_configStrings << "tvtime:method=LineDoubler,enabled=1,pulldown=none,framerate_mode=half_top,judder_correction=0,use_progressive_frame_flag=1,chroma_filter=0,cheap_mode=1";
  
  setInitialSize(QSize(680, 480));
  QWidget* mainWidget = makeMainWidget();
  QGridLayout* grid = new QGridLayout( mainWidget, 9, 2 );
  grid->setSpacing(5);
  grid->setMargin(5);

  m_qualitySlider = new QSlider( QSlider::Vertical, mainWidget );
  m_qualitySlider->setRange(0, 5);
  m_qualitySlider->setSteps(1, 1);
  m_qualitySlider->setTickmarks(QSlider::Right);
  grid->addMultiCellWidget(m_qualitySlider, 0, 5, 0, 0);

  QLabel* level0Descr = new QLabel(i18n("<b>Very low cpu usage, worst quality.</b><br>Half of vertical resolution is lost. For some systems (with PCI video cards) this might decrease the cpu usage when compared to plain video playback (no deinterlacing)."), mainWidget);
  grid->addWidget(level0Descr, 5, 1);
  
  QLabel* level1Descr = new QLabel(i18n("<b>Low cpu usage, poor quality.</b><br>Image is blurred vertically so interlacing effects are removed."), mainWidget);
  grid->addWidget(level1Descr, 4, 1);
  
  QLabel* level2Descr = new QLabel(i18n("<b>Medium cpu usage, medium quality.</b><br>Image is analysed and areas showing interlacing artifacts are fixed (interpolated)."), mainWidget);
  grid->addWidget(level2Descr, 3, 1);
  
  QLabel* level3Descr = new QLabel(i18n("<b>High cpu usage, good quality.</b><br>Conversion of dvd image format improves quality and fixes chroma upsampling bug."), mainWidget);
  grid->addWidget(level3Descr, 2, 1);
  
  QLabel* level4Descr = new QLabel(i18n("<b>Very high cpu usage, great quality.</b><br>Besides using smart deinterlacing algorithms it will also double the frame rate (30->60fps) to match the field rate of TVs. Detects and reverts 3-2 pulldown. *"), mainWidget);
  grid->addWidget(level4Descr, 1, 1);
  
  QLabel* level5Descr = new QLabel(i18n("<b>Very very high cpu usage, great quality with (experimental) improvements.</b><br>Enables judder correction (play films at their original 24 fps speed) and vertical color smoothing (fixes small color stripes seen in some dvds). *"), mainWidget);
  grid->addWidget(level5Descr, 0, 1);

  m_customBox = new QCheckBox(i18n("User defined"), mainWidget);
  grid->addMultiCellWidget(m_customBox, 6, 6, 0, 1);
  connect(m_customBox, SIGNAL(toggled(bool)), this, SLOT(slotCustomBoxToggled(bool)));

  m_customConfigButton = new KPushButton(i18n("Configure tvtime Deinterlace Plugin..."), mainWidget);
  m_customConfigButton->setSizePolicy(QSizePolicy (QSizePolicy::Minimum, QSizePolicy::Fixed));
  grid->addWidget(m_customConfigButton, 7, 1);
  connect(m_customConfigButton, SIGNAL(clicked()), filterDialog, SLOT(show()));

  QLabel* note = new QLabel(i18n("* <i>May require a patched 2.4 kernel (like RedHat one) or 2.6 kernel.</i>"), mainWidget);
  note->setAlignment(QLabel::WordBreak | QLabel::AlignVCenter);
  grid->addMultiCellWidget(note, 9, 9, 0, 1);
}

DeinterlaceQuality::~DeinterlaceQuality()
{
  kdDebug() << "DeinterlaceQuality: destructed" << endl;
}

void DeinterlaceQuality::slotLevelChanged( int level )
{
  // kdDebug() << "DeinterlaceQuality: Change to quality " << level << endl;
   emit signalSetDeinterlaceConfig(m_configStrings[level]);
}

void DeinterlaceQuality::slotCustomBoxToggled(bool on)
{
  if (on)
  {
    m_customConfigButton->setEnabled(true);
    m_qualitySlider->setEnabled(false);
  }
  else
  {
    m_customConfigButton->setEnabled(false);
    m_qualitySlider->setEnabled(true);
  }
}

void DeinterlaceQuality::setQuality(uint qu)
{
  if (qu < 10)
  {
    m_qualitySlider->setValue(qu);
    m_customBox->setChecked(false);
    slotCustomBoxToggled(false);
  }
  else
  {
    m_qualitySlider->setValue(qu-10);
    m_customBox->setChecked(true);
  }
  connect(m_qualitySlider, SIGNAL(valueChanged(int)), this, SLOT(slotLevelChanged(int)));
}     

uint DeinterlaceQuality::getQuality() const
{
  if (m_customBox->isChecked())
    return m_qualitySlider->value()+10;
   else
    return m_qualitySlider->value();
}
