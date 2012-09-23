/*
 * filterdialog.cpp - config dialog for postprocessing filters
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
#include <kpushbutton.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kcombobox.h>

#include <qvbox.h>
#include <qstringlist.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qscrollview.h>
 
#include "filterdialog.h"
#include "filterdialog.moc"


FilterDialog::FilterDialog(const QStringList& audioFilters, const QStringList& videoFilters, QWidget *parent, const char *name) :
  KDialogBase(KDialogBase::IconList, i18n("Effect Plugins"), KDialogBase::Ok, KDialogBase::Ok, parent, name, false)
{
  reparent(parent, pos(), false);
  setInitialSize(QSize(400,350), true);

/****** Audio Filters ******/  
  QWidget* audioPage = addPage(i18n("Audio"), i18n("Audio Filters"),
           KGlobal::iconLoader()->loadIcon("sound", KIcon::Panel, KIcon::SizeMedium));
  QGridLayout* audioGrid = new QGridLayout( audioPage, 3, 3 );
  audioGrid->setSpacing( 5 );

  QCheckBox* useAudioFiltersCB = new QCheckBox( audioPage );
  useAudioFiltersCB->setText( i18n("Enable audio filters") );
  useAudioFiltersCB->setChecked( true );
  connect( useAudioFiltersCB, SIGNAL(toggled(bool)), this, SLOT(slotUseAudioFilters(bool)));

  audioGrid->addMultiCellWidget( useAudioFiltersCB, 0, 0, 0, 2 );
  
  m_audioFilterCombo = new KComboBox( audioPage );
  m_audioFilterCombo->insertStringList( audioFilters );

  m_addAudioButton = new KPushButton( i18n("Add Filter"), audioPage );
  connect( m_addAudioButton, SIGNAL( clicked() ), this, SLOT( slotAddAudioClicked() ));
  m_removeAudioButton = new KPushButton( i18n("Remove All Filters"), audioPage );
  connect( m_removeAudioButton, SIGNAL( clicked() ), this, SIGNAL( signalRemoveAllAudioFilters() ));

  audioGrid->addWidget( m_audioFilterCombo, 1, 0 );
  audioGrid->addWidget( m_removeAudioButton, 1, 2 );
  audioGrid->addWidget( m_addAudioButton, 1, 1 );

  QScrollView* audioSv = new QScrollView( audioPage );
  audioSv->setResizePolicy(QScrollView::AutoOneFit);
  m_audioFilterPage = new QVBox(audioSv->viewport());
  m_audioFilterPage->setMargin( 5 );
  audioSv->addChild(m_audioFilterPage);

  audioGrid->addMultiCellWidget( audioSv, 2, 2, 0, 2  );
  
/****** Video Filters ******/  
  QWidget* videoPage = addPage(i18n("Video"), i18n("Video Filters"),
           KGlobal::iconLoader()->loadIcon("video", KIcon::Panel, KIcon::SizeMedium));
  QGridLayout* videoGrid = new QGridLayout( videoPage, 3, 3 );
  videoGrid->setSpacing( 5 );

  QCheckBox* useVideoFiltersCB = new QCheckBox( videoPage );
  useVideoFiltersCB->setText( i18n("Enable video filters") );
  useVideoFiltersCB->setChecked( true );
  connect( useVideoFiltersCB, SIGNAL(toggled(bool)), this, SLOT(slotUseVideoFilters(bool)));

  videoGrid->addMultiCellWidget( useVideoFiltersCB, 0, 0, 0, 2 );
  
  m_videoFilterCombo = new KComboBox( videoPage );
  m_videoFilterCombo->insertStringList( videoFilters );

  m_addVideoButton = new KPushButton( i18n("Add Filter"), videoPage );
  connect( m_addVideoButton, SIGNAL( clicked() ), this, SLOT( slotAddVideoClicked() ));
  m_removeVideoButton = new KPushButton( i18n("Remove All Filters"), videoPage );
  connect( m_removeVideoButton, SIGNAL( clicked() ), this, SIGNAL( signalRemoveAllVideoFilters() ));

  videoGrid->addWidget( m_videoFilterCombo, 1, 0 );
  videoGrid->addWidget( m_removeVideoButton, 1, 2 );
  videoGrid->addWidget( m_addVideoButton, 1, 1 );

  QScrollView* videoSv = new QScrollView( videoPage );
  videoSv->setResizePolicy(QScrollView::AutoOneFit);
  m_videoFilterPage = new QVBox(videoSv->viewport());
  m_videoFilterPage->setMargin( 5 );
  videoSv->addChild(m_videoFilterPage);

  videoGrid->addMultiCellWidget( videoSv, 2, 2, 0, 2  );
}


FilterDialog::~FilterDialog()
{
  kdDebug() << "FilterDialog: destructor" << endl;
}


void FilterDialog::slotUseAudioFilters( bool on )
{
  m_audioFilterCombo->setEnabled( on );
  m_removeAudioButton->setEnabled( on );
  m_addAudioButton->setEnabled( on );
  m_audioFilterPage->setEnabled( on );
  emit signalUseAudioFilters( on );
}


void FilterDialog::slotUseVideoFilters( bool on )
{
  m_videoFilterCombo->setEnabled( on );
  m_removeVideoButton->setEnabled( on );
  m_addVideoButton->setEnabled( on );
  m_videoFilterPage->setEnabled( on );
  emit signalUseVideoFilters( on );
}
