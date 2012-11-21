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

#include "MaximiseButton.h"

#include <qtooltip.h>

namespace RiscOS
{

/* XPM */
static const char * const maximise_xpm[] = {
"12 12 3 1",
" 	c None",
".	c #000000",
"+	c #FFFFFF",
"            ",
"  ........  ",
" .++++++++. ",
" .++++++++. ",
" .++++++++. ",
" .++++++++. ",
" .++++++++. ",
" .++++++++. ",
" .++++++++. ",
" .++++++++. ",
"  ........  ",
"            "};

/* XPM */
static const char * const unmaximise_xpm[] = {
"12 12 3 1",
" 	c None",
".	c #000000",
"+	c #FFFFFF",
"            ",
"            ",
"   ......   ",
"  .++++++.  ",
"  .++++++.  ",
"  .++++++.  ",
"  .++++++.  ",
"  .++++++.  ",
"  .++++++.  ",
"   ......   ",
"            ",
"            "};

MaximiseButton::MaximiseButton(QWidget * parent)
  : Button(parent, i18n("Maximize"),
           (ButtonState)(LeftButton|MidButton|RightButton)),
    on_(false)
{
   setPixmap(QPixmap((const char **)maximise_xpm));
}

void MaximiseButton::setOn(bool on)
{
   on_ = on;
   setPixmap(on_ ? QPixmap((const char **)unmaximise_xpm)
                 : QPixmap((const char **)maximise_xpm));
   repaint();
   QToolTip::remove(this);
   QToolTip::add(this, on_ ? i18n("Restore") : i18n("Maximize"));
}

void MaximiseButton::mouseReleaseEvent(QMouseEvent *e)
{
   Button::mouseReleaseEvent(e);

   if (!rect().contains(e->pos()))
      return;
   emit(maximizeClicked(lastButton_));
}

} // End namespace

// vim:ts=2:sw=2:tw=78
#include "MaximiseButton.moc"
