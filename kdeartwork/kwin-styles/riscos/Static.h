/*
  RISC OS KWin client
  
  Copyright 2000
    Rik Hemsley <rik@kde.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#ifndef RISC_OS_STATIC_H
#define RISC_OS_STATIC_H

#include <qimage.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qstringlist.h>

#include "Palette.h"

namespace RiscOS
{

enum SymbolType { Lower, Close, Iconify, Max, Unmax };

class Static
{
   public:

      Static();

      ~Static();

      static Static* instance()
      {
         if (instance_ == 0)
         new Static;

         return instance_;
      }

      void reset();
      void updatePixmaps();

      const QPixmap& titleTextLeft(bool active) const;
      const QPixmap& titleTextRight(bool active) const;
      const QPixmap& resizeMidLeft(bool active) const;
      const QPixmap& resizeMidRight(bool active) const;
      const QPixmap& titleTextMid(bool active) const;
      const QPixmap& resizeMidMid(bool active) const;
      const QPixmap& buttonBase(bool active, bool down) const;
      const QPixmap& resize(bool active) const;

      int animationStyle() const;
      int titleHeight() const;
      int resizeHeight() const;

  private:

      void _init();

      void _drawBorder(QPixmap&, int, int);
      void _createTexture(QPixmap&, int, bool);

      void _initSizes();

      void _resizeAllPixmaps();
      void _blankAllPixmaps();

      void _initPalettes();
      void _initTextures();

      void _drawTitleTextAreaSides();
      void _drawResizeCentralAreaSides();
      void _drawTitleTextAreaBackground();
      void _drawResizeCentralAreaBackground();
      void _drawResizeHandles();
      void _drawButtonBackgrounds();

      static Static *instance_;

      Palette
         aTitlePal_,
         iTitlePal_,
         aResizePal_,
         iResizePal_,
         aButPal_,
         iButPal_;

      QPixmap
         aButtonUp_,         iButtonUp_,
         aButtonDown_,       iButtonDown_,
         aResize_,           iResize_,
         aResizeDown_,       iResizeDown_,
         aTitleTextLeft_,    iTitleTextLeft_,
         aTitleTextRight_,   iTitleTextRight_,
         aTitleTextMid_,     iTitleTextMid_,
         aResizeMidLeft_,    iResizeMidLeft_,
         aResizeMidRight_,   iResizeMidRight_,
         aResizeMid_,        iResizeMid_;

      QPixmap aTexture_, iTexture_, abTexture_, ibTexture_;

      QPainter painter_;
      bool down_;
      Palette palette_;
      double transx, transy;

      int animationStyle_;

      int titleHeight_;
      int buttonSize_;
      int resizeHeight_;

      bool hicolour_;
};

} // End namespace

#endif
 
// vim:ts=2:sw=2:tw=78
