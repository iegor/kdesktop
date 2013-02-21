// (c) 2000 Peter Putzer

#include <qscrollbar.h>
#include <qlayout.h>
#include <qstyle.h>

#include <kdebug.h>
#include <kdialog.h>
#include <kapplication.h>

#include "kscroller.h"

class KScroller::KScrollerPrivate
{
public:
  KScrollerPrivate ()
    : setCornerWidget (0L)
  {
  }

  ~KScrollerPrivate () {}

  QWidget* setCornerWidget;
};

KScroller::KScroller (QWidget* parent, const char* name)
  : QFrame (parent, name),
    d (new KScrollerPrivate()),
    mVertical (new QScrollBar (QScrollBar::Vertical, this)),
    mHorizontal (new QScrollBar (QScrollBar::Horizontal, this)),
    mCornerWidget (new QWidget (this)),
    mContent (0L),
    mVerticalOld (0),
    mHorizontalOld (0)
{
  mCornerWidget->hide();
  mVertical->hide();
  mHorizontal->hide();

  connect (mVertical, SIGNAL (valueChanged (int)),
           this, SLOT (scrollVertical (int)));
  connect (mHorizontal, SIGNAL (valueChanged (int)),
           this, SLOT (scrollHorizontal (int)));
}

KScroller::~KScroller ()
{
  delete d;
}

void KScroller::setContent (QWidget* content)
{
  delete mContent;

  mContent = content;
  updateScrollBars();
}

void KScroller::setCornerWidget (QWidget* corner)
{
  delete mCornerWidget;

  mCornerWidget = d->setCornerWidget = corner;

  updateScrollBars();
}

QWidget* KScroller::cornerWidget ()
{
  return d->setCornerWidget;
}

void KScroller::resizeEvent (QResizeEvent* e)
{
  QFrame::resizeEvent (e);
 
  updateScrollBars();
}

QSize KScroller::minimumSizeHint() const
{
  QSize size = sizeHint();
  if (size.width() > 300)
     size.setWidth(300);
  return size;
}

QSize KScroller::sizeHint() const
{
  QSize size = mContent->minimumSize();
  int extra = kapp->style().pixelMetric( QStyle::PM_ScrollBarExtent, 0 );
  size += QSize( extra, extra );
  return size;
}

void KScroller::updateScrollBars ()
{
  int w = width();
  int h = height();
  QSize cs = mContent->minimumSize();

  setupVertical ( cs.width(), cs.height(), w, h);
  setupHorizontal ( cs.width(), cs.height(), w, h);
  mContent->resize (cs);

  setupCornerWidget (w, h);
}

void KScroller::setupHorizontal (int cw, int, int w, int h)
{
  mHorizontal->setValue (0);
  mHorizontalOld = 0;

  if (cw > w)
    {
      int extra = kapp->style().pixelMetric( QStyle::PM_ScrollBarExtent, 0 );

      if (!mVertical->isHidden())
        w -= extra;

      mHorizontal->setRange (0, cw - w);
      mHorizontal->setPageStep (w);
      mHorizontal->setLineStep (25);
      mHorizontal->setGeometry (0, h - extra, w, extra);

      mHorizontal->raise();
      mHorizontal->show();
    }
  else
    {
      mHorizontal->hide();
    }
}

void KScroller::setupVertical (int, int ch, int w, int h)
{
  mVertical->setValue (0);
  mVerticalOld = 0;

  if (ch > h)
    {
      int extra = kapp->style().pixelMetric( QStyle::PM_ScrollBarExtent, 0 );
      mVertical->setRange (0, ch - h);
      mVertical->setPageStep (h);
      mVertical->setLineStep (25);

      mVertical->setGeometry (w - extra, 0, extra, h);

      mVertical->raise();
      mVertical->show();
    }
  else
    {
      mVertical->hide();
    }
}

void KScroller::scrollVertical (int value)
{  
  int amount = value - mVerticalOld;
  mVerticalOld = value;

  mContent->move (mContent->x(), mContent->y() - amount);
}

void KScroller::scrollHorizontal (int value)
{
  int amount = value - mHorizontalOld;
  mHorizontalOld = value;
  
  mContent->move (mContent->x() - amount, mContent->y());
}

void KScroller::setupCornerWidget (int w, int h)
{
  if (!mVertical->isHidden() && !mHorizontal->isHidden())
    {
      int extra = kapp->style().pixelMetric( QStyle::PM_ScrollBarExtent, 0 );

      mCornerWidget->raise();
      mCornerWidget->setGeometry (w - extra, h - extra, extra, extra);

      mCornerWidget->show();
    }
  else
    {
      mCornerWidget->hide();
    }
}

#include "kscroller.moc"
