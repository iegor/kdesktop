/*
 * dummy_part.cpp
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

#include <kparts/genericfactory.h>
#include <kaction.h>

#include "dummy_part.h"

typedef KParts::GenericFactory<DummyPart> DummyPartFactory;
K_EXPORT_COMPONENT_FACTORY (libdummypart, DummyPartFactory);


DummyPart::DummyPart(QWidget* parentWidget, const char* widgetName, QObject* parent, const char* name, const QStringList& /*args*/)
: KaffeinePart(parent, name ? name : "DummyPart")
{
  // we need an instance
  setInstance(DummyPartFactory::instance());

 // m_player = new Player(this);
 // m_player->setFocusPolicy(QWidget::ClickFocus);
 // setWidget(m_player);

  setXMLFile("dummy_part.rc");
  initActions();
}


DummyPart::~DummyPart()
{
}

bool DummyPart::isPlaying()
{
  return false;
}

uint DummyPart::volume() const
{
  return 0;
}

uint DummyPart::position() const
{
  return 0;
}

bool DummyPart::closeURL()
{
  return true;
}

KAboutData *DummyPart::createAboutData()
{
    KAboutData* aboutData = new KAboutData( "dummypart", I18N_NOOP("DummyPart"),
    "0.1", "Description...",
    KAboutData::License_GPL,
    "(c) 2004, Jürgen Kofler.", 0, "http://kaffeine.sourceforge.net");
    aboutData->addAuthor("Jürgen Kofler.",0, "kaffeine@gmx.net");

   return aboutData;
}

bool DummyPart::openURL(const MRL& mrl)
{
  // m_player->setLocation(mrl.url());
}

void DummyPart::slotPlay()
{
  // m_player->play();
}

void DummyPart::slotTogglePause()
{

}

void DummyPart::slotSetVolume(uint)
{

}

void DummyPart::slotSetPosition(uint)
{

}

void DummyPart::slotStop()
{
  // m_player->stop();
}

void DummyPart::slotMute()
{

}

void DummyPart::initActions()
{
  new KAction(i18n("Play"), "player_play", 0, this, SLOT(slotPlay()), actionCollection(), "player_play");
  new KAction(i18n("Pause"), "player_pause", Key_Space, this, SLOT(slotTogglePause()), actionCollection(), "player_pause");
  new KAction(i18n("Stop"), "player_stop", Key_Backspace, this, SLOT(slotStop()), actionCollection(), "player_stop");
}
