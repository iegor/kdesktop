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

#include "StickyButton.h"

#include <qtooltip.h>

namespace RiscOS
{

/* XPM */
static const char * const unsticky_xpm[] = {
"12 12 3 1",
" 	c None",
".	c #000000",
"+	c #FFFFFF",
"    ...     ",
"   .+++.    ",
"   .+++.    ",
"   .+++.    ",
"   .+++.    ",
"  .+++++.   ",
" .+++++++.  ",
".+++++++++. ",
".....+..... ",
"    .+.     ",
"    .+.     ",
"     .      "};

/* XPM */
static const char * const sticky_xpm[] = {
"12 12 3 1",
" 	c None",
".	c #000000",
"+	c #FFFFFF",
"        .   ",
"       .+.  ",
"      .+++. ",
"     .+++++.",
" ....+++++. ",
"  .++++++.  ",
"   .++++.   ",
"   .+++.    ",
"  .+..+.    ",
" .+.  ..    ",
".+.    .    ",
"..          "};


StickyButton::StickyButton(QWidget *parent)
  : Button(parent, i18n("On all desktops")), on_(false)
{
   setPixmap(QPixmap((const char **)sticky_xpm));
}

void StickyButton::setOn(bool on)
{
   on_ = on;
   setPixmap(on_ ? QPixmap((const char **)unsticky_xpm) :
                   QPixmap((const char **)sticky_xpm));
   repaint();
   QToolTip::remove(this);
   QToolTip::add(this, on_ ? i18n("Not on all desktops")
                           : i18n("On all desktops"));
}

void StickyButton::mouseReleaseEvent(QMouseEvent *e)
{
   Button::mouseReleaseEvent(e);

   if (!rect().contains(e->pos()))
      return;

   emit(toggleSticky());   
}

} // End namespace

// vim:ts=2:sw=2:tw=78
#include "StickyButton.moc"
