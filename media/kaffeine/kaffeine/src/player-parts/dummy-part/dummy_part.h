/*
 * dummy_part.h
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

#ifndef DUMMYPART_H
#define DUMMYPART_H

#include <kparts/factory.h>

#include "kaffeinepart.h"

/**
 * DummyPart - use this as template for own player parts
 * @author Jürgen Kofler <kaffeine@gmx.net>
 *
 */


class DummyPart : public KaffeinePart
{
    Q_OBJECT
public:
  DummyPart(QWidget*, const char*, QObject*, const char*, const QStringList&);
  virtual ~DummyPart();

  /*
   *Reimplement from KaffeinePart
   */
  bool isPlaying();
  uint volume() const; /* percent */
  uint position() const; /* percent */

  bool closeURL();
  static KAboutData* createAboutData();

public slots:
  /*
   * Reimplement from KaffeinePart
   */
  bool openURL(const MRL& mrl);
  void slotPlay();
  void slotTogglePause();
  void slotSetVolume(uint); /* percent */
  void slotSetPosition(uint); /* percent */
  void slotStop();
  void slotMute();

private:
  void initActions();

private:
  // Player* m_play;

};

#endif /* DUMMYPART_H */
