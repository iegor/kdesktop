/*
 * positionslider.h
 *
 * Copyright (C) 2004-2005 Giorgos Gousios
 * Copyright (C) 2004-2005 Jürgen Kofler <kaffeine@gmx.net>
 * Copyright (C) 2004-2005 Miguel Freitas
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

#ifndef POSITIONSLIDER_H
#define POSITIONSLIDER_H

#include <qslider.h>

class PositionSlider : public QSlider
{
  Q_OBJECT

 public:
  PositionSlider(Qt::Orientation, QWidget * parent = 0, const char* name = 0);
  virtual ~PositionSlider();

  void setPosition(int, bool);

signals:
  void signalStartSeeking();
  void signalStopSeeking();
  void sliderLastMove(int);

 protected:
   virtual void wheelEvent(QWheelEvent* e);
   bool eventFilter(QObject *obj, QEvent *ev);

 private slots:
   void slotSliderPressed();
   void slotSliderReleased();

 private:
   bool m_userChange;
};

#endif /* POSITIONSLIDER_H */
