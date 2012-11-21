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

#include <qtooltip.h>
#include "Button.h"
#include "Static.h"

namespace RiscOS
{

Button::Button(QWidget *parent, const QString& tip,
               const ButtonState realizeButtons)
  : QWidget(parent, "Button", 0),
    realizeButtons_(realizeButtons),
    lastButton_(NoButton),
    alignment_(Left),
    down_     (false),
    active_   (false)
{
   QToolTip::add(this, tip);
   setBackgroundColor(Qt::black);

   setFixedSize(Static::instance()->titleHeight() - 1,
                Static::instance()->titleHeight());
}

Button::~Button()
{
  // Empty.
}

void Button::setAlignment(Alignment a)
{
   alignment_ = a;
   repaint();
}

void Button::setActive(bool b)
{
   active_ = b;
   repaint();
}

Button::Alignment Button::alignment() const
{
   return alignment_;
}

void Button::mousePressEvent(QMouseEvent *e)
{
   down_ = true;
   lastButton_ = e->button();
   repaint();

   QMouseEvent me(e->type(), e->pos(), e->globalPos(),
                  (e->button()&realizeButtons_) ? LeftButton : NoButton,
                  e->state());
   QWidget::mousePressEvent(&me);
}

void Button::mouseReleaseEvent(QMouseEvent *e)
{
   down_ = false;
   lastButton_ = e->button();
   repaint();
   QMouseEvent me(e->type(), e->pos(), e->globalPos(),
                  (e->button()&realizeButtons_) ? LeftButton : NoButton,
                  e->state());
   QWidget::mouseReleaseEvent(&me);
}

void Button::setPixmap(const QPixmap &p)
{
   if (QPixmap::defaultDepth() <= 8)
      aPixmap_ = iPixmap_ = p;
   else
   {
      QRgb light;
      QRgb* data = NULL;
      QRgb w = qRgb(255, 255, 255);

      QImage aTx(p.convertToImage());
      QImage iTx(aTx.copy());

      const KDecorationOptions* options = KDecoration::options();
      light = options->color(KDecoration::ColorButtonBg, true).light(150).rgb();

      if (light == qRgb(0, 0, 0))
         light = qRgb(228, 228, 228);

      data = (QRgb *)aTx.bits();

      for (int x = 0; x < 144; x++)
         if (data[x] == w)
            data[x] = light;

      light = options->color(KDecoration::ColorButtonBg, false).light(150).rgb();

      if (light == qRgb(0, 0, 0))
         light = qRgb(228, 228, 228);

      data = (QRgb *)iTx.bits();

      for (int x = 0; x < 144; x++)
         if (data[x] == w)
            data[x] = light;

      aPixmap_.convertFromImage(aTx);
      iPixmap_.convertFromImage(iTx);

      if (0 != p.mask())
      {
         aPixmap_.setMask(*p.mask());
         iPixmap_.setMask(*p.mask());
      }
   }
   repaint();
}

void Button::paintEvent(QPaintEvent *)
{
   bitBlt(this, alignment_ == Left ? 1 : 0, 0,
          &Static::instance()->buttonBase(active_, down_));

   int i = width() / 2 - 6;

   bitBlt(this, alignment_ == Left ? i + 1 : i,
          i + 1, active_ ? &aPixmap_ : &iPixmap_);
}

} // End namespace

// vim:ts=2:sw=2:tw=78
#include "Button.moc"
