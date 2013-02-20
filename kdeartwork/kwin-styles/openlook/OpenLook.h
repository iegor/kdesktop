/*
  'OpenLook' kwin client

  Porting to kde3.2 API 
    Copyright 2003 Luciano Montanaro <mikelima@cirulla.net>
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to
  deal in the Software without restriction, including without limitation the
  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
  sell copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
 
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
  AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef KWIN_WEB_H
#define KWIN_WEB_H

#include <qptrlist.h>
#include <qbutton.h>

#include <kdecoration.h>
#include <kdecorationfactory.h>

class QSpacerItem;

namespace OpenLook
{
  class OpenLookButton;

  class OpenLook : public KDecoration
  {
    Q_OBJECT
    public:

      OpenLook(KDecorationBridge *b, KDecorationFactory *f);
      ~OpenLook();
      void init();

    protected:
      bool eventFilter(QObject *o, QEvent *e);
      void resizeEvent(QResizeEvent *e);
      void paintEvent(QPaintEvent *e);
      void showEvent(QShowEvent *e);

      virtual void captionChange();
      void desktopChange();
      void activeChange();
      void shadeChange();
      void iconChange();
      void maximizeChange();
      void borders(int &left, int &right, int &top, int &bottom) const;
      QSize minimumSize() const;
      void resize( const QSize& );
      virtual void mouseDoubleClickEvent(QMouseEvent *);
      virtual void wheelEvent(QWheelEvent *e);

      virtual Position mousePosition(const QPoint &) const;
      virtual bool animateMinimize(bool);

    private:

      void doLayout();

      QRect titleRect() const;

      QRect topLeftRect() const;
      QRect topRightRect() const;
      QRect bottomLeftRect() const;
      QRect bottomRightRect() const;

      QRect buttonRect() const;

      void paintBorder(QPainter &) const;

      void paintTopLeftRect(QPainter &) const;
      void paintTopRightRect(QPainter &) const;
      void paintBottomLeftRect(QPainter &) const;
      void paintBottomRightRect(QPainter &) const;

      void paintButton(QPainter &) const;
      void paintArrow(QPainter &) const;

      bool isButtonPress(QMouseEvent *);
      bool isButtonRelease(QMouseEvent *);

      QSpacerItem   * titleSpacer_;
      QPoint          mousePressPoint_;
      bool            tool_;
      bool            buttonDown_;
  };
  
  class DecorationFactory: public QObject, public KDecorationFactory
  {
  public:
    DecorationFactory();
    virtual ~DecorationFactory();
    virtual KDecoration *createDecoration(KDecorationBridge *);
    virtual bool reset(unsigned long changed);
    virtual bool supports( Ability ability );
    QValueList< DecorationFactory::BorderSize > borderSizes() const;
  };
}

#endif
// vim:ts=2:sw=2:tw=78:set et:
