#ifndef __SYSTEMCLIENT_H
#define __SYSTEMCLIENT_H

#include <qvariant.h>
#include <qbitmap.h>
#include <qbutton.h>

#include <kpixmap.h>
#include <kdecoration.h>
#include <kdecorationfactory.h>


class QLabel;
class QSpacerItem;
class QBoxLayout;

namespace System {

class SystemButton;

enum ButtonType {
    ButtonClose=0,
    ButtonSticky,
    ButtonMinimize,
    ButtonMaximize,
    ButtonHelp,
    ButtonTypeCount
};

class SystemClient : public KDecoration
{
   Q_OBJECT
   public:
      SystemClient(KDecorationBridge* bridge, KDecorationFactory* factory);
      ~SystemClient();
      virtual Position mousePosition(const QPoint& p) const;
      virtual void resize(const QSize&);
      virtual bool eventFilter(QObject* o, QEvent* e);
      virtual void init();
   protected:
      virtual void maximizeChange();
      virtual void captionChange();
      virtual void shadeChange() {};
      virtual void iconChange();
      virtual void desktopChange();
      virtual void activeChange();
      virtual QSize minimumSize() const;
      virtual void borders(int&, int&, int&, int&) const;
      virtual void reset( unsigned long changed );
      void drawRoundFrame(QPainter &p, int x, int y, int w, int h);
      void resizeEvent( QResizeEvent* );
      void paintEvent( QPaintEvent* );
      void showEvent( QShowEvent* );
      void mouseDoubleClickEvent( QMouseEvent * );
      void wheelEvent(QWheelEvent *e);
      void doShape();
      void recalcTitleBuffer();
   private:
      void addButtons(QBoxLayout* hb, const QString& buttons);
   private slots:
      void maxButtonClicked();

   private:
      SystemButton* button[ButtonTypeCount];
      QSpacerItem* titlebar;
      QPixmap titleBuffer;
      QString oldTitle;
};

class SystemButton : public QButton
{
   public:
      SystemButton(SystemClient *parent=0, const char *name=0,
                   const unsigned char *bitmap=NULL, const QString& tip=NULL);
      void setBitmap(const unsigned char *bitmap);
      void reset();
      QSize sizeHint() const;
      void setTipText(const QString &tip);
      ButtonState last_button;
   protected:
      virtual void drawButton(QPainter *p);
      void drawButtonLabel(QPainter *){}
      QBitmap deco;

      void mousePressEvent( QMouseEvent* e );
      void mouseReleaseEvent( QMouseEvent* e );

   private:
      SystemClient* client;
};


class SystemDecoFactory : public QObject, public KDecorationFactory
{
   Q_OBJECT
   public:
      SystemDecoFactory();
      virtual ~SystemDecoFactory();
      virtual KDecoration *createDecoration(KDecorationBridge *);
      virtual bool reset(unsigned long);
      virtual bool supports( Ability ability );
      virtual QValueList< BorderSize > borderSizes() const;
   private:
      void readConfig();
};



}

#endif
