/*
 * xine_part_iface.h
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

#ifndef XINE_PART_IFACE_H
#define XINE_PART_IFACE_H

#include <dcopobject.h>

class XinePartIface : virtual public DCOPObject
{
  K_DCOP
public:

k_dcop:
  /*
   * Returns the contrast value (0 - 65535).
   */
  virtual int getContrast() = 0;

  /*
   * Set the contrast (0 - 65535).
   */
  virtual void setContrast(int c) = 0;

  /*
   * Returns the brightness value (0 - 65535).
   */
  virtual int getBrightness() = 0;

  /*
   * Set the brightness (0 - 65535).
   */
  virtual void setBrightness(int b) = 0;

 /*
  * Move the menu cursor upwards.
  */
  virtual void dvdMenuUp() = 0;

 /*
  * Move the menu cursor downwards.
  */
  virtual void dvdMenuDown() = 0;

 /*
  * Move the menu cursor to the left.
  */
  virtual void dvdMenuLeft() = 0;

 /*
  * Move the menu cursor to the right.
  */
  virtual void dvdMenuRight() = 0;

 /*
  * Select the menu item pointed by the cursor.
  */
  virtual void dvdMenuSelect() = 0;

 /*
  *Toggle DVD menu on/off
  */
  virtual void dvdMenuToggle() = 0;

  /*
   * Set the aspect ratio automatically.
   */
  virtual void aspectRatioAuto() = 0;

  /*
   * Set the aspect ratio to 4:3.
   */
  virtual void aspectRatio4_3() = 0;

  /*
   * Set the aspect ration to 16:9.
   */
  virtual void aspectRatioAnamorphic() = 0;

  /*
   * Set the aspect ratio to 1:1.
   */
  virtual void aspectRatioSquare() = 0;

  /*
   * Set the aspect ratio to 2.11:1.
   */
  virtual void aspectRatioDVB() = 0;

  /*
   * Zoom in.
   */
  virtual void zoomIn() = 0;

  /*
   * Zoom out.
   */
  virtual void zoomOut() = 0;

  /*
   * Zoom off.
   */
  virtual void zoomOff() = 0;

  virtual void zoomInX() = 0;
  virtual void zoomOutX() = 0;
  virtual void zoomInY() = 0;
  virtual void zoomOutY() = 0;

  virtual QString screenShot() = 0;

  virtual void nextAudioChannel() = 0;
  virtual void nextSubtitleChannel() = 0;
  virtual void speedFaster() = 0;
  virtual void speedSlower() = 0;
};

#endif /* XINE_PART_IFACE_H */
