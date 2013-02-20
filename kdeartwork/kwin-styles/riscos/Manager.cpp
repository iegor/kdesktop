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

#include <unistd.h> // for usleep
#include <config.h> // for usleep on non-linux platforms
#include <math.h> // for sin and cos

#include <qapplication.h>
#include <qimage.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>

#include <netwm.h>

#include "Manager.h"
#include "Static.h"
#include "AboveButton.h"
#include "CloseButton.h"
#include "HelpButton.h"
#include "IconifyButton.h"
#include "LowerButton.h"
#include "MaximiseButton.h"
#include "StickyButton.h"

extern "C"
{
   KDE_EXPORT KDecorationFactory* create_factory()
   {
      return new RiscOS::Factory();
   }
}


namespace RiscOS
{

Manager::Manager(KDecorationBridge *bridge,
                 KDecorationFactory *factory)
  : KDecoration(bridge, factory),
    topLayout_    (NULL),
    titleLayout_  (NULL),
    titleSpacer_  (NULL)
{
}

Manager::~Manager()
{
}

void Manager::init()
{
   createMainWidget(WNoAutoErase);

   widget()->installEventFilter(this);
   widget()->setBackgroundMode(NoBackground);

   leftButtonList_.setAutoDelete(true);
   rightButtonList_.setAutoDelete(true);

   resetLayout();
}

bool Manager::eventFilter(QObject *o, QEvent *e)
{
   if (o != widget()) return false;
   switch (e->type())
   {
      case QEvent::Resize:
         resizeEvent(static_cast<QResizeEvent*>(e));
         return true;
      case QEvent::Paint:
         paintEvent(static_cast<QPaintEvent*>(e));
         return true;
      case QEvent::MouseButtonDblClick:
         mouseDoubleClickEvent(static_cast<QMouseEvent*>(e));
         return true;
      case QEvent::MouseButtonPress:
         processMousePressEvent(static_cast<QMouseEvent*>(e));
         return true;
      case QEvent::Wheel:
         wheelEvent( static_cast< QWheelEvent* >( e ));
         return true;
      case QEvent::MouseButtonRelease:
         return false;
      case QEvent::Show:
         return false;
      case QEvent::MouseMove:
         return false;
      case QEvent::Enter:
         return false;
      case QEvent::Leave:
         return false;
      case QEvent::Move:
         return false;
      default:
         return false;
   }
}

void Manager::reset(unsigned long /*changed*/)
{
   resetLayout();
}

void Manager::borders(int &left, int &right, int &top, int &bottom) const
{
   left = right = 1;
   top = Static::instance()->titleHeight();
   bottom = isResizable() ? Static::instance()->resizeHeight() : 1;
}

void Manager::resize(const QSize &s)
{
   widget()->resize(s);
}

QSize Manager::minimumSize() const
{
   return widget()->minimumSize();
}

void Manager::activeChange()
{
   updateTitleBuffer();
   widget()->repaint();
   emit(activeChanged(isActive()));
}

void Manager::captionChange()
{
   updateTitleBuffer();
   widget()->repaint();
}

void Manager::iconChange()
{
}

void Manager::maximizeChange()
{
   emit(maximizeChanged(maximizeMode() == MaximizeFull));
}

void Manager::desktopChange()
{
}

void Manager::shadeChange()
{
}

void Manager::paintEvent(QPaintEvent *e)
{
   QPainter p(widget());

   QRect r(e->rect());

   bool intersectsLeft = r.intersects(QRect(0, 0, 1, height()));

   bool intersectsRight =
      r.intersects(QRect(width() - 1, 0, width(), height()));

   if (intersectsLeft || intersectsRight)
   {
      p.setPen(Qt::black);

      if (intersectsLeft)
         p.drawLine(0, r.top(), 0, r.bottom());

      if (intersectsRight)
         p.drawLine(width() - 1, r.top(), width() - 1, r.bottom());
   }

   Static * s = Static::instance();

   bool active = isActive();

   // Title bar.

   QRect tr = titleSpacer_->geometry();
   bitBlt(widget(), tr.topLeft(), &titleBuf_);

   // Resize bar.

   if (isResizable())
   {
      int rbt = height() - Static::instance()->resizeHeight(); // Resize bar top

      bitBlt(widget(), 0, rbt, &(s->resize(active)));
      bitBlt(widget(), 30, rbt, &(s->resizeMidLeft(active)));

      p.drawTiledPixmap(32, rbt, width() - 34,
                        Static::instance()->resizeHeight(),
                        s->resizeMidMid(active));

      bitBlt(widget(), width() - 32, rbt, &(s->resizeMidRight(active)));
      bitBlt(widget(), width() - 30, rbt, &(s->resize(active)));
   }
   else
      p.drawLine(1, height() - 1, width() - 2, height() - 1);
}

void Manager::resizeEvent(QResizeEvent*)
{
   updateButtonVisibility();
   updateTitleBuffer();
   widget()->repaint();
}

void Manager::updateButtonVisibility()
{
#if 0
   enum SizeProblem = { None, Small, Medium, Big };
   SizeProblem sizeProblem = None;

   if (width() < 80) sizeProblem = Big;
   else if (width() < 100) sizeProblem = Medium;
   else if (width() < 180) sizeProblem = Small;

   switch (sizeProblem) {

      case Small:
         above_    ->hide();
         lower_    ->hide();
         sticky_   ->hide();
         help_     ->hide();
         iconify_  ->show();
         maximise_ ->hide();
         close_    ->show();
         break;

      case Medium:
         above_    ->hide();
         lower_    ->hide();
         sticky_   ->hide();
         help_     ->hide();
         iconify_  ->hide();
         maximise_ ->hide();
         close_    ->show();
         break;

      case Big:
         above_    ->hide();
         lower_    ->hide();
         sticky_   ->hide();
         help_     ->hide();
         iconify_  ->hide();
         maximise_ ->hide();
         close_    ->hide();
         break;

      case None:
      default:
         above_    ->show();
         lower_    ->show();
         sticky_   ->show();
         if (providesContextHelp())
           help_->show();
         iconify_  ->show();
         maximise_ ->show();
         close_    ->show();
         break;
   }

   layout()->activate();
#endif
}

void Manager::updateTitleBuffer()
{
   bool active = isActive();

   Static * s = Static::instance();

   QRect tr = titleSpacer_->geometry();

   if (tr.width() == 0 || tr.height() == 0)
      titleBuf_.resize(8, 8);
   else
      titleBuf_.resize(tr.size());

   QPainter p(&titleBuf_);

   p.drawPixmap(0, 0, s->titleTextLeft(active));

   p.drawTiledPixmap(3, 0, tr.width() - 6, Static::instance()->titleHeight(),
                     s->titleTextMid(active));

   p.setPen(options()->color(KDecorationOptions::ColorFont, active));

   p.setFont(options()->font(active));

   p.drawText(4, 2, tr.width() - 8, Static::instance()->titleHeight() - 4,
              AlignCenter, caption());

   p.drawPixmap(tr.width() - 3, 0, s->titleTextRight(active));
}

KDecoration::Position Manager::mousePosition(const QPoint& p) const
{
   Position m = PositionCenter;

   // Look out for off-by-one errors here.

   if (isResizable())
   {
      if (p.y() > (height() - (Static::instance()->resizeHeight() + 1)))
      {
        // Keep order !

         if (p.x() >= (width() - 30))
            m = PositionBottomRight;
         else if (p.x() <= 30)
            m = PositionBottomLeft;
         else
            m = PositionBottom;
      }
      else
      {
         m = PositionCenter;
         // Client::mousePosition(p);
      }
   }
   else
   {
      m = PositionCenter;
      // Client::mousePosition(p);
   }

   return m;
}

void Manager::mouseDoubleClickEvent(QMouseEvent *e)
{
   if (e->button() == LeftButton && titleSpacer_->geometry().contains(e->pos()))
      titlebarDblClickOperation();
}

void Manager::wheelEvent(QWheelEvent *e)
{
    if (isSetShade() || titleLayout_->geometry().contains(e->pos()) )
        titlebarMouseWheelOperation( e->delta());
}

void Manager::paletteChange(const QPalette &)
{
   resetLayout();
}

void Manager::stickyChange(bool b)
{
   emit(stickyChanged(b));
}

void Manager::slotToggleSticky()
{
   toggleOnAllDesktops();
   emit(stickyChanged(isOnAllDesktops()));
}

void Manager::slotAbove()
{
   setKeepAbove(!keepAbove());
}

void Manager::slotLower()
{
   setKeepBelow(!keepBelow());
}

void Manager::slotMaximizeClicked(ButtonState state)
{
#if KDE_IS_VERSION(3, 3, 0)
   maximize(state);
#else
   switch (state)
   {
      case RightButton:
         maximize(maximizeMode() ^ MaximizeHorizontal);
         break;

      case MidButton:
         maximize(maximizeMode() ^ MaximizeVertical);
         break;

      case LeftButton:
      default:
         maximize(maximizeMode() == MaximizeFull ? MaximizeRestore
                                                 : MaximizeFull);
         break;
   }
#endif
   emit(maximizeChanged(maximizeMode() == MaximizeFull));
}

bool Manager::animateMinimize(bool iconify)
{
   int style = Static::instance()->animationStyle();

   switch (style)
   {
      case 1:
      {
         // Double twisting double back, with pike ;)

         if (!iconify) // No animation for restore.
            return true;

         // Go away quick.
         helperShowHide(false);
         qApp->syncX();

         QRect r = iconGeometry();

         if (!r.isValid())
            return true;

         // Algorithm taken from Window Maker (http://www.windowmaker.org)

         int sx = geometry().x();
         int sy = geometry().y();
         int sw = width();
         int sh = height();
         int dx = r.x();
         int dy = r.y();
         int dw = r.width();
         int dh = r.height();

         double steps = 12;

         double xstep = double((dx-sx)/steps);
         double ystep = double((dy-sy)/steps);
         double wstep = double((dw-sw)/steps);
         double hstep = double((dh-sh)/steps);

         double cx = sx;
         double cy = sy;
         double cw = sw;
         double ch = sh;

         double finalAngle = 3.14159265358979323846;

         double delta  = finalAngle / steps;

         QPainter p(workspaceWidget());
         p.setRasterOp(Qt::NotROP);

         for (double angle = 0; ; angle += delta)
         {
            if (angle > finalAngle)
               angle = finalAngle;

            double dx = (cw / 10) - ((cw / 5) * sin(angle));
            double dch = (ch / 2) * cos(angle);
            double midy = cy + (ch / 2);

            QPoint p1(int(cx + dx), int(midy - dch));
            QPoint p2(int(cx + cw - dx), p1.y());
            QPoint p3(int(cx + dw + dx), int(midy + dch));
            QPoint p4(int(cx - dx), p3.y());

            grabXServer();

            p.drawLine(p1, p2);
            p.drawLine(p2, p3);
            p.drawLine(p3, p4);
            p.drawLine(p4, p1);

            p.flush();

            usleep(500);

            p.drawLine(p1, p2);
            p.drawLine(p2, p3);
            p.drawLine(p3, p4);
            p.drawLine(p4, p1);

            ungrabXServer();

            cx += xstep;
            cy += ystep;
            cw += wstep;
            ch += hstep;

            if (angle >= finalAngle)
               break;
         }
      }
      break;

      case 2:
      {
         // KVirc style ? Maybe. For qwertz.

         if (!iconify) // No animation for restore.
            return true;

         // Go away quick.
         helperShowHide(false);
         qApp->syncX();

         int stepCount = 12;

         QRect r(geometry());

         int dx = r.width() / (stepCount * 2);
         int dy = r.height() / (stepCount * 2);

         QPainter p(workspaceWidget());
         p.setRasterOp(Qt::NotROP);

         for (int step = 0; step < stepCount; step++)
         {
            r.moveBy(dx, dy);
            r.setWidth(r.width() - 2 * dx);
            r.setHeight(r.height() - 2 * dy);

            grabXServer();

            p.drawRect(r);
            p.flush();
            usleep(200);
            p.drawRect(r);

            ungrabXServer();
         }
      }
      break;


      default:
      {
         QRect icongeom = iconGeometry();

         if (!icongeom.isValid())
            return true;

         QRect wingeom = geometry();

         QPainter p(workspaceWidget());

         p.setRasterOp(Qt::NotROP);
#if 0
         if (iconify)
            p.setClipRegion(QRegion(workspaceWidget()->rect()) - wingeom);
#endif
         grabXServer();

         p.drawLine(wingeom.bottomRight(), icongeom.bottomRight());
         p.drawLine(wingeom.bottomLeft(), icongeom.bottomLeft());
         p.drawLine(wingeom.topLeft(), icongeom.topLeft());
         p.drawLine(wingeom.topRight(), icongeom.topRight());

         p.flush();

         qApp->syncX();

         usleep(30000);

         p.drawLine(wingeom.bottomRight(), icongeom.bottomRight());
         p.drawLine(wingeom.bottomLeft(), icongeom.bottomLeft());
         p.drawLine(wingeom.topLeft(), icongeom.topLeft());
         p.drawLine(wingeom.topRight(), icongeom.topRight());

         ungrabXServer();
      }
      break;
   }
   return true;
}

void Manager::createTitle()
{
   leftButtonList_.clear();
   rightButtonList_.clear();

   QString buttons;

   if (options()->customButtonPositions())
      buttons = options()->titleButtonsLeft() + "|" +
                options()->titleButtonsRight();
   else
      buttons = "XSH|IA";

   QPtrList<Button> *buttonList = &leftButtonList_;

   for (unsigned int i = 0; i < buttons.length(); ++i)
   {
      Button * tb = NULL;

      switch (buttons[i].latin1())
      {
         case 'S': // Sticky
            tb = new StickyButton(widget());
            connect(this, SIGNAL(stickyChanged(bool)),
                    tb, SLOT(setOn(bool)));
            connect(tb, SIGNAL(toggleSticky()), this, SLOT(slotToggleSticky()));
            emit(stickyChanged(isOnAllDesktops()));
            break;

         case 'H': // Help
            if (providesContextHelp())
            {
               tb = new HelpButton(widget());
               connect(tb, SIGNAL(help()), this, SLOT(showContextHelp()));
            }
            break;

         case 'I': // Minimize
            if (isMinimizable())
            {
               tb = new IconifyButton(widget());
               connect(tb, SIGNAL(iconify()), this, SLOT(minimize()));
            }
            break;

         case 'A': // Maximize
            if (isMaximizable())
            {
               tb = new MaximiseButton(widget());
               connect(tb, SIGNAL(maximizeClicked(ButtonState)),
                       this, SLOT(slotMaximizeClicked(ButtonState)));
               connect(this, SIGNAL(maximizeChanged(bool)),
                       tb, SLOT(setOn(bool)));
               emit(maximizeChanged(maximizeMode() == MaximizeFull));
            }
            break;

         case 'F': // Above
            tb = new AboveButton(widget());
            connect(tb, SIGNAL(above()), this, SLOT(slotAbove()));
            break;

         case 'B': // Lower
            tb = new LowerButton(widget());
            connect(tb, SIGNAL(lower()), this, SLOT(slotLower()));
            break;

         case 'X': // Close
            if (isCloseable())
            {
               tb = new CloseButton(widget());
               connect(tb, SIGNAL(closeWindow()), this, SLOT(closeWindow()));
            }
            break;

         case '|':
            buttonList = &rightButtonList_;
            break;
      }

      if (tb != NULL)
      {
         connect(this, SIGNAL(activeChanged(bool)), tb, SLOT(setActive(bool)));
         buttonList->append(tb);
      }
   }

   for (QPtrListIterator<Button> it(leftButtonList_); it.current(); ++it)
   {
      it.current()->setAlignment(Button::Left);
      titleLayout_->addWidget(it.current());
   }

   titleSpacer_ = new QSpacerItem(0, Static::instance()->titleHeight(),
                                  QSizePolicy::Expanding, QSizePolicy::Fixed);

   titleLayout_->addItem(titleSpacer_);

   for (QPtrListIterator<Button> it(rightButtonList_); it.current(); ++it)
   {
      it.current()->setAlignment(Button::Right);
      titleLayout_->addWidget(it.current());
   }
}

void Manager::resetLayout()
{
   delete topLayout_;
   topLayout_ = new QVBoxLayout(widget(), 0, 0);
   topLayout_->setResizeMode(QLayout::FreeResize);

   titleLayout_ = new QBoxLayout(topLayout_, QBoxLayout::LeftToRight, 0, 0);
   titleLayout_->setResizeMode(QLayout::FreeResize);

   createTitle();

   QBoxLayout *midLayout = new QBoxLayout(topLayout_, QBoxLayout::LeftToRight,
                                          0, 0);
   midLayout->setResizeMode(QLayout::FreeResize);
   midLayout->addSpacing(1);
   if (isPreview())
      midLayout->addWidget(
         new QLabel(i18n("<center><b>RiscOS preview</b></center>"), widget()));
   midLayout->addSpacing(1);

   if (isResizable())
      topLayout_->addSpacing(Static::instance()->resizeHeight());
   else
      topLayout_->addSpacing(1);
}

// --------------------

Factory::Factory()
{
   (void) RiscOS::Static::instance();
}

Factory::~Factory()
{
   delete RiscOS::Static::instance();
}

bool Factory::reset(unsigned long /*changed*/)
{
   RiscOS::Static::instance()->reset();
   return true;
}

bool Factory::supports( Ability ability )
{
    switch( ability )
    {
        case AbilityAnnounceButtons:
        case AbilityButtonOnAllDesktops:
        case AbilityButtonHelp:
        case AbilityButtonMinimize:
        case AbilityButtonMaximize:
        case AbilityButtonClose:
        case AbilityButtonAboveOthers:
        case AbilityButtonBelowOthers:
            return true;
        default:
            return false;
    };
}

KDecoration* Factory::createDecoration(KDecorationBridge *bridge)
{
   return new Manager(bridge, this);
}

} // End namespace

// vim:ts=2:sw=2:tw=78
#include "Manager.moc"
