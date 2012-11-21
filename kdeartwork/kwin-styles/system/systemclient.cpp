#include "systemclient.h"

#include <qlayout.h>
#include <qdrawutil.h>
#include <qbitmap.h>
#include <qtooltip.h>
#include <qlabel.h>
#include <qcursor.h>

#include <kpixmapeffect.h>
#include <kdrawutil.h>
#include <klocale.h>
#include <kapplication.h>
#include <kdebug.h>

// Default button layout
const char default_left[]  = "X";
const char default_right[] = "HSIA";

namespace System {

static unsigned char iconify_bits[] = {
    0x00, 0x00, 0xff, 0xff, 0x7e, 0x3c, 0x18, 0x00};

static unsigned char maximize_bits[] = {
    0x00, 0x18, 0x3c, 0x7e, 0xff, 0xff, 0x00, 0x00};

static unsigned char r_minmax_bits[] = {
    0x0c, 0x18, 0x33, 0x67, 0xcf, 0x9f, 0x3f, 0x3f};

static unsigned char l_minmax_bits[] = {
    0x30, 0x18, 0xcc, 0xe6, 0xf3, 0xf9, 0xfc, 0xfc};    
    
static unsigned char unsticky_bits[] = {
    0x00, 0x18, 0x18, 0x7e, 0x7e, 0x18, 0x18, 0x00};

static unsigned char sticky_bits[] = {
    0x00, 0x00, 0x00, 0x7e, 0x7e, 0x00, 0x00, 0x00};

static unsigned char question_bits[] = {
    0x3c, 0x66, 0x60, 0x30, 0x18, 0x00, 0x18, 0x18};

static KPixmap *aUpperGradient=0;
static KPixmap *iUpperGradient=0;

static KPixmap *btnPix=0;
static KPixmap *btnPixDown=0;
static KPixmap *iBtnPix=0;
static KPixmap *iBtnPixDown=0;
static QColor *btnForeground;

static bool pixmaps_created = false;

static void drawButtonFrame(KPixmap *pix, const QColorGroup &g)
{
    QPainter p;
    p.begin(pix);
    p.setPen(g.mid());
    p.drawLine(0, 0, 13, 0);
    p.drawLine(0, 0, 0, 13);
    p.setPen(g.light());
    p.drawLine(13, 0, 13, 13);
    p.drawLine(0, 13, 13, 13);
    p.setPen(g.dark());
    p.drawRect(1, 1, 12, 12);
    p.end();
}

static void create_pixmaps()
{
    if(pixmaps_created)
        return;
    pixmaps_created = true;

    if(QPixmap::defaultDepth() > 8){
        // titlebar
        aUpperGradient = new KPixmap;
        aUpperGradient->resize(32, 18);
        iUpperGradient = new KPixmap;
        iUpperGradient->resize(32, 18);
        QColor bgColor = kapp->palette().active().background();
        KPixmapEffect::gradient(*aUpperGradient,
                                KDecoration::options()->color(KDecorationOptions::ColorFrame, true).light(130),
                                bgColor,
                                KPixmapEffect::VerticalGradient);
        KPixmapEffect::gradient(*iUpperGradient,
                                KDecoration::options()->color(KDecorationOptions::ColorFrame, false).light(130),
                                bgColor,
                                KPixmapEffect::VerticalGradient);

        // buttons
        KPixmap aPix;
        aPix.resize(12, 12);
        KPixmap iPix;
        iPix.resize(12, 12);
        KPixmap aInternal;
        aInternal.resize(8, 8);
        KPixmap iInternal;
        iInternal.resize(8, 8);

        QColor hColor(KDecoration::options()->color(KDecorationOptions::ColorButtonBg, false));
        KPixmapEffect::gradient(iInternal,
                                hColor.dark(120),
                                hColor.light(120),
                                KPixmapEffect::DiagonalGradient);
        KPixmapEffect::gradient(iPix,
                                hColor.light(150),
                                hColor.dark(150),
                                KPixmapEffect::DiagonalGradient);

        hColor =KDecoration::options()->color(KDecorationOptions::ColorButtonBg, true);
        KPixmapEffect::gradient(aInternal,
                                hColor.dark(120),
                                hColor.light(120),
                                KPixmapEffect::DiagonalGradient);
        KPixmapEffect::gradient(aPix,
                                hColor.light(150),
                                hColor.dark(150),
                                KPixmapEffect::DiagonalGradient);
        bitBlt(&aPix, 1, 1, &aInternal, 0, 0, 8, 8, Qt::CopyROP, true);
        bitBlt(&iPix, 1, 1, &iInternal, 0, 0, 8, 8, Qt::CopyROP, true);

        // normal buttons
        btnPix = new KPixmap;
        btnPix->resize(14, 14);
        bitBlt(btnPix, 2, 2, &aPix, 0, 0, 10, 10, Qt::CopyROP, true);
        drawButtonFrame(btnPix, KDecoration::options()->colorGroup(KDecorationOptions::ColorFrame, true));

        iBtnPix = new KPixmap;
        iBtnPix->resize(14, 14);
        bitBlt(iBtnPix, 2, 2, &iPix, 0, 0, 10, 10, Qt::CopyROP, true);
        drawButtonFrame(iBtnPix, KDecoration::options()->colorGroup(KDecorationOptions::ColorFrame, false));


        // pressed buttons
        hColor = KDecoration::options()->color(KDecorationOptions::ColorButtonBg, false);
        KPixmapEffect::gradient(iInternal,
                                hColor.light(130),
                                hColor.dark(130),
                                KPixmapEffect::DiagonalGradient);
        KPixmapEffect::gradient(iPix,
                                hColor.light(150),
                                hColor.dark(150),
                                KPixmapEffect::DiagonalGradient);

        hColor =KDecoration::options()->color(KDecorationOptions::ColorButtonBg, true);
        KPixmapEffect::gradient(aInternal,
                                hColor.light(130),
                                hColor.dark(130),
                                KPixmapEffect::DiagonalGradient);
        KPixmapEffect::gradient(aPix,
                                hColor.light(150),
                                hColor.dark(150),
                                KPixmapEffect::DiagonalGradient);
        bitBlt(&aPix, 1, 1, &aInternal, 0, 0, 8, 8, Qt::CopyROP, true);
        bitBlt(&iPix, 1, 1, &iInternal, 0, 0, 8, 8, Qt::CopyROP, true);

        btnPixDown = new KPixmap;
        btnPixDown->resize(14, 14);
        bitBlt(btnPixDown, 2, 2, &aPix, 0, 0, 10, 10, Qt::CopyROP, true);
        drawButtonFrame(btnPixDown, KDecoration::options()->colorGroup(KDecorationOptions::ColorFrame,
                                                        true));

        iBtnPixDown = new KPixmap;
        iBtnPixDown->resize(14, 14);
        bitBlt(iBtnPixDown, 2, 2, &iPix, 0, 0, 10, 10, Qt::CopyROP, true);
        drawButtonFrame(iBtnPixDown, KDecoration::options()->colorGroup(KDecorationOptions::ColorFrame,
                                                         false));
    }
    if(qGray(KDecoration::options()->color(KDecorationOptions::ColorButtonBg, true).rgb()) > 128)
        btnForeground = new QColor(Qt::black);
    else
        btnForeground = new QColor(Qt::white);
}

static void delete_pixmaps()
{
    if(aUpperGradient){
        delete aUpperGradient;
        delete iUpperGradient;
        delete btnPix;
        delete btnPixDown;
        delete iBtnPix;
        delete iBtnPixDown;
        aUpperGradient = 0;
    }
    delete btnForeground;
    pixmaps_created = false;
}

SystemButton::SystemButton(SystemClient *parent, const char *name,
                           const unsigned char *bitmap, const QString& tip)
: QButton(parent->widget(), name)
{
   setTipText(tip);
   setBackgroundMode( NoBackground );
   setCursor(ArrowCursor);
   resize(14, 14);
   if(bitmap)
      setBitmap(bitmap);
   client = parent;
}

void SystemButton::setTipText(const QString &tip)
{
   if (KDecoration::options()->showTooltips())
   {
      QToolTip::remove(this );
      QToolTip::add(this, tip );
   }
}


QSize SystemButton::sizeHint() const
{
    return(QSize(14, 14));
}

void SystemButton::reset()
{
   repaint(false);
}

void SystemButton::setBitmap(const unsigned char *bitmap)
{
    deco = QBitmap(8, 8, bitmap, true);
    deco.setMask(deco);
    repaint();
}

void SystemButton::drawButton(QPainter *p)
{
    if(btnPixDown){
        if(client->isActive())
            p->drawPixmap(0, 0, isDown() ? *btnPixDown : *btnPix);
        else
            p->drawPixmap(0, 0, isDown() ? *iBtnPixDown : *iBtnPix);
    }
    else{
        QColorGroup g = KDecoration::options()->colorGroup(KDecorationOptions::ColorFrame,
                                            client->isActive());
        int x2 = width()-1;
        int y2 = height()-1;
        // outer frame
        p->setPen(g.mid());
        p->drawLine(0, 0, x2, 0);
        p->drawLine(0, 0, 0, y2);
        p->setPen(g.light());
        p->drawLine(x2, 0, x2, y2);
        p->drawLine(0, x2, y2, x2);
        p->setPen(g.dark());
        p->drawRect(1, 1, width()-2, height()-2);
        // inner frame
        g = KDecoration::options()->colorGroup(KDecorationOptions::ColorButtonBg, client->isActive());
        p->fillRect(3, 3, width()-6, height()-6, g.background());
        p->setPen(isDown() ? g.mid() : g.light());
        p->drawLine(2, 2, x2-2, 2);
        p->drawLine(2, 2, 2, y2-2);
        p->setPen(isDown() ? g.light() : g.mid());
        p->drawLine(x2-2, 2, x2-2, y2-2);
        p->drawLine(2, x2-2, y2-2, x2-2);

    }

    if(!deco.isNull()){
        p->setPen(*btnForeground);
        p->drawPixmap(isDown() ? 4 : 3, isDown() ? 4 : 3, deco);
    }
}

void SystemButton::mousePressEvent( QMouseEvent* e )
{
   last_button = e->button();
   QMouseEvent me ( e->type(), e->pos(), e->globalPos(), LeftButton, e->state() );
   QButton::mousePressEvent( &me );
}

void SystemButton::mouseReleaseEvent( QMouseEvent* e )
{
   last_button = e->button();
   QMouseEvent me ( e->type(), e->pos(), e->globalPos(), LeftButton, e->state() );
   QButton::mouseReleaseEvent( &me );
}



SystemClient::SystemClient(KDecorationBridge* bridge, KDecorationFactory* factory)
    : KDecoration(bridge, factory)
{}

SystemClient::~SystemClient()
{
    for (int n=0; n<ButtonTypeCount; n++) {
        if (button[n]) delete button[n];
    }
}

void SystemClient::init()
{
   createMainWidget(0);
   widget()->installEventFilter( this );

   QGridLayout* g = new QGridLayout(widget(), 0, 0, 2);

   if (isPreview())
   {
      g->addWidget(new QLabel(i18n("<center><b>System++ preview</b></center>"), widget()), 1, 1);
   }
   else
   {
      g->addItem(new QSpacerItem( 0, 0 ), 1, 1); // no widget in the middle
   }
//   g->addItem( new QSpacerItem( 0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding ) );
   g->setRowStretch(1, 10);

   g->addColSpacing(0, 2);
   g->addColSpacing(2, 2);
   g->addRowSpacing(2, 6);

   QBoxLayout* hb = new QBoxLayout(0, QBoxLayout::LeftToRight, 0, 0, 0);
   hb->setResizeMode(QLayout::FreeResize);
   g->addLayout( hb, 0, 1 );
   hb->addSpacing(3);   
   
   titlebar = new QSpacerItem(10, 14, QSizePolicy::Expanding,
                              QSizePolicy::Minimum);    

    // setup titlebar buttons
    for (int n=0; n<ButtonTypeCount; n++) button[n] = 0;
    addButtons(hb, KDecoration::options()->customButtonPositions() ? 
                   KDecoration::options()->titleButtonsLeft() : QString(default_left));
    hb->addSpacing(2);    
    hb->addItem(titlebar);
    hb->addSpacing(3);
    addButtons(hb, KDecoration::options()->customButtonPositions() ? 
                   KDecoration::options()->titleButtonsRight() : QString(default_right));
    hb->addSpacing(2);

   widget()->setBackgroundMode(QWidget::NoBackground);
   recalcTitleBuffer();
}

void SystemClient::addButtons(QBoxLayout *hb, const QString& s)
{
    unsigned char *minmax_bits;
    int l_max = KDecoration::options()->titleButtonsLeft().find('A');
    if (s.length() > 0) {
        for (unsigned n=0; n < s.length(); n++) {
            switch (s[n]) {
              case 'X': // Close button
                  if ((!button[ButtonClose]) && isCloseable()) {
                      button[ButtonClose] = new SystemButton(this, "close", NULL, i18n("Close"));
                      connect( button[ButtonClose], SIGNAL( clicked() ), this, ( SLOT( closeWindow() ) ) );
                      hb->addWidget(button[ButtonClose]);
                      hb->addSpacing(1);
                  }
                  break;
                  
              case 'S': // Sticky button
                  if (!button[ButtonSticky]) {
                     button[ButtonSticky] = new SystemButton(this, "sticky", NULL, i18n("On all desktops"));
                     if(isOnAllDesktops())
                         button[ButtonSticky]->setBitmap(unsticky_bits);
                     else
                         button[ButtonSticky]->setBitmap(sticky_bits);
                     connect( button[ButtonSticky], SIGNAL( clicked() ), this, ( SLOT( toggleOnAllDesktops() ) ) );
                     hb->addWidget(button[ButtonSticky]);
                     hb->addSpacing(1);
                  }
                  break;

              case 'I': // Minimize button
                  if ((!button[ButtonMinimize]) && isMinimizable())  {
                      button[ButtonMinimize] = new SystemButton(this, "iconify", iconify_bits, i18n("Minimize"));
                      connect( button[ButtonMinimize], SIGNAL( clicked() ), this, ( SLOT( minimize() ) ) );
                      hb->addWidget(button[ButtonMinimize]);
                      hb->addSpacing(1);
                  }
                  break;

              case 'A': // Maximize button
                  if ((!button[ButtonMaximize]) && isMaximizable()) {
                      if (maximizeMode()==MaximizeFull) {
                          if (KDecoration::options()->customButtonPositions() && (l_max>-1))
                              minmax_bits = l_minmax_bits;
                          else
                              minmax_bits = r_minmax_bits;
                          button[ButtonMaximize] = new SystemButton(this, "maximize", minmax_bits, i18n("Restore"));
                      }
                      else
                          button[ButtonMaximize] = new SystemButton(this, "maximize", maximize_bits, i18n("Maximize"));
                      connect( button[ButtonMaximize], SIGNAL( clicked() ), this, ( SLOT( maxButtonClicked() ) ) );
                      hb->addWidget(button[ButtonMaximize]);
                      hb->addSpacing(1);
                  }
                  break;

              case 'H': // Help button
                  if ((!button[ButtonHelp]) && providesContextHelp()) {
                      button[ButtonHelp] = new SystemButton(this, "help", question_bits, i18n("Help"));
                      connect( button[ButtonHelp], SIGNAL( clicked() ), this, ( SLOT( showContextHelp() ) ) );
                      hb->addWidget(button[ButtonHelp]);
                      hb->addSpacing(1);
                  }
                  break;
            
            }
        }
    }
}

bool SystemClient::eventFilter( QObject* o, QEvent* e )
{
    if( o != widget())
       return false;
    switch( e->type())
    {
    case QEvent::Resize:
       resizeEvent(static_cast< QResizeEvent* >( e ) );
       return true;
    case QEvent::Paint:
       paintEvent(static_cast< QPaintEvent* >( e ) );
       return true;
    case QEvent::MouseButtonDblClick:
       mouseDoubleClickEvent(static_cast< QMouseEvent* >( e ) );
       return true;
    case QEvent::MouseButtonPress:
       processMousePressEvent(static_cast< QMouseEvent* >( e ) );
       return true;
    case QEvent::Wheel:
       wheelEvent( static_cast< QWheelEvent* >( e ));
       return true;
    default:
        break;
    }
    return false;
}

void SystemClient::reset(unsigned long)
{
   titleBuffer.resize(0, 0);
   recalcTitleBuffer();
   widget()->repaint();
   if (button[ButtonClose])
       button[ButtonClose]->reset();
   if (button[ButtonSticky])
       button[ButtonSticky]->reset();
   if (button[ButtonMinimize])
       button[ButtonMinimize]->reset();
   if (button[ButtonMaximize])
       button[ButtonMaximize]->reset();
   if (button[ButtonHelp])
       button[ButtonHelp]->reset();
}

void SystemClient::maxButtonClicked()
{
   maximize( button[ButtonMaximize]->last_button );
}

void SystemClient::resizeEvent( QResizeEvent* )
{
    //Client::resizeEvent( e );
    recalcTitleBuffer();
    doShape();
    /*
    if ( isVisibleToTLW() && !testWFlags( WStaticContents )) {
        QPainter p( this );
	QRect t = titlebar->geometry();
	t.setTop( 0 );
	QRegion r = rect();
	r = r.subtract( t );
	p.setClipRegion( r );
	p.eraseRect( rect() );
        }*/
}

void SystemClient::resize( const QSize& s )
{
   widget()->resize( s );
}


QSize SystemClient::minimumSize() const
{
   return widget()->minimumSize();
}


void SystemClient::recalcTitleBuffer()
{
    if(oldTitle == caption() && width() == titleBuffer.width())
        return;
    QFontMetrics fm(options()->font(true));
    titleBuffer.resize(width(), 18);
    QPainter p;
    p.begin(&titleBuffer);
    if(aUpperGradient)
        p.drawTiledPixmap(0, 0, width(), 18, *aUpperGradient);
    else
        p.fillRect(0, 0, width(), 18,
                   options()->colorGroup(KDecorationOptions::ColorFrame, true).
                   brush(QColorGroup::Button));

    QRect t = titlebar->geometry();
    t.setTop( 2 );
    t.setLeft( t.left() + 4 );
    t.setRight( t.right() - 2 );

    QRegion r(t.x(), 0, t.width(), 18);
    r -= QRect(t.x()+((t.width()-fm.width(caption()))/2)-4,
               0, fm.width(caption())+8, 18);
    p.setClipRegion(r);
    int i, ly;
    for(i=0, ly=4; i < 4; ++i, ly+=3){
        p.setPen(options()->color(KDecorationOptions::ColorTitleBar, true).light(150));
        p.drawLine(0, ly, width()-1, ly);
        p.setPen(options()->color(KDecorationOptions::ColorTitleBar, true).dark(120));
        p.drawLine(0, ly+1, width()-1, ly+1);
    }
    p.setClipRect(t);
    p.setPen(options()->color(KDecorationOptions::ColorFont, true));
    p.setFont(options()->font(true));

    p.drawText(t.x()+((t.width()-fm.width(caption()))/2)-4,
               0, fm.width(caption())+8, 18, AlignCenter, caption());
    p.setClipping(false);
    p.end();
    oldTitle = caption();
}

void SystemClient::captionChange()
{
   recalcTitleBuffer();
   widget()->repaint(titlebar->geometry(), false);
}

void SystemClient::drawRoundFrame(QPainter &p, int x, int y, int w, int h)
{
   kDrawRoundButton(&p, x, y, w, h,
                    options()->colorGroup(KDecorationOptions::ColorFrame, isActive()), false);

}

void SystemClient::paintEvent( QPaintEvent* )
{
    QPainter p(widget());
    QRect t = titlebar->geometry();

    QBrush fillBrush(widget()->colorGroup().brush(QColorGroup::Background).pixmap() ?
                     widget()->colorGroup().brush(QColorGroup::Background) :
                     options()->colorGroup(KDecorationOptions::ColorFrame, isActive()).
                     brush(QColorGroup::Button));

    p.fillRect(1, 18, width()-2, height()-19, fillBrush);

    t.setTop( 2 );
    t.setLeft( t.left() + 4 );
    t.setRight( t.right() - 2 );

    if(isActive())
        p.drawPixmap(0, 0, titleBuffer);
    else{
        if(iUpperGradient)
            p.drawTiledPixmap(0, 0, width(), 18, *iUpperGradient);
        else
            p.fillRect(0, 0, width(), 18, fillBrush);
        p.setPen(options()->color(KDecorationOptions::ColorFont, isActive()));
        p.setFont(options()->font(isActive()));
        p.drawText(t, AlignCenter, caption() );
    }

    p.setPen(options()->colorGroup(KDecorationOptions::ColorFrame, isActive()).light());
    p.drawLine(width()-20, height()-7, width()-10,  height()-7);
    p.drawLine(width()-20, height()-5, width()-10,  height()-5);
    p.setPen(options()->colorGroup(KDecorationOptions::ColorFrame, isActive()).dark());
    p.drawLine(width()-20, height()-6, width()-10,  height()-6);
    p.drawLine(width()-20, height()-4, width()-10,  height()-4);

    drawRoundFrame(p, 0, 0, width(), height());
}

#define QCOORDARRLEN(x) sizeof(x)/(sizeof(QCOORD)*2)

void SystemClient::doShape()
{
    // using a bunch of QRect lines seems much more efficent than bitmaps or
    // point arrays

    QRegion mask;
    kRoundMaskRegion(mask, 0, 0, width(), height());
    setMask(mask);
}

void SystemClient::showEvent(QShowEvent *)
{
//    Client::showEvent(ev);
    doShape();
    widget()->show();
//    widget()->repaint();
}

/*void SystemClient::windowWrapperShowEvent( QShowEvent* )
{
    doShape();
}*/

void SystemClient::mouseDoubleClickEvent( QMouseEvent * e )
{
   if ( e->button() == LeftButton && titlebar->geometry().contains( e->pos() ) )
      titlebarDblClickOperation();
}

void SystemClient::wheelEvent( QWheelEvent *e )
{
    if (isSetShade() || QRect( 0, 0, width(), titlebar->geometry().height() ).contains( e->pos() ) )
        titlebarMouseWheelOperation( e->delta());
}

void SystemClient::maximizeChange()
{
    unsigned char *minmax_bits;
    int l_max = KDecoration::options()->titleButtonsLeft().find('A');
    if (KDecoration::options()->customButtonPositions() && (l_max>-1))
        minmax_bits = l_minmax_bits;
    else
        minmax_bits = r_minmax_bits;
    if (button[ButtonMaximize]) {
        button[ButtonMaximize]->setBitmap((maximizeMode()==MaximizeFull) ? minmax_bits : maximize_bits);
        button[ButtonMaximize]->setTipText((maximizeMode()==MaximizeFull) ? i18n("Restore") : i18n("Maximize"));
    }
}

void SystemClient::activeChange()
{
    widget()->repaint(false);
    if (button[ButtonClose])
        button[ButtonClose]->reset();
    if (button[ButtonSticky])
        button[ButtonSticky]->reset();
    if (button[ButtonMinimize])
        button[ButtonMinimize]->reset();
    if (button[ButtonMaximize])
        button[ButtonMaximize]->reset();
    if (button[ButtonHelp])
        button[ButtonHelp]->reset();
}

void SystemClient::iconChange()
{
//   if (button[BtnMenu] && button[BtnMenu]->isVisible())
//      button[BtnMenu]->repaint(false);
}

void SystemClient::desktopChange()
{
   if (button[ButtonSticky]) {
       button[ButtonSticky]->setBitmap(isOnAllDesktops() ? unsticky_bits : sticky_bits);
       button[ButtonSticky]->setTipText(isOnAllDesktops() ? i18n("Not on all desktops") : i18n("On all desktops"));
   }
}

/*void SystemClient::stickyChange(bool on)
{
}*/

KDecoration::Position SystemClient::mousePosition(const QPoint &p) const
{
   return KDecoration::mousePosition(p);
}

void SystemClient::borders(int& left, int& right, int& top, int& bottom) const
{
    left = 4;
    right = 4;
    top =  18;
    bottom = 8;

/*    if ((maximizeMode()==MaximizeFull) && !options()->moveResizeMaximizedWindows()) {
        left = right = bottom = 0;
        top = 1 + titleHeight + (borderSize-1);
    }*/
}

SystemDecoFactory::SystemDecoFactory()
{
   create_pixmaps();
}

SystemDecoFactory::~SystemDecoFactory()
{
   delete_pixmaps();
}

KDecoration *SystemDecoFactory::createDecoration( KDecorationBridge *b )
{
   return new SystemClient(b, this);
}

bool SystemDecoFactory::reset( unsigned long changed )
{
   System::delete_pixmaps();
   System::create_pixmaps();
   // Ensure changes in tooltip state get applied
   resetDecorations(changed);
   return true;
}

bool SystemDecoFactory::supports( Ability ability )
{
    switch( ability )
    {
        case AbilityAnnounceButtons:
        case AbilityButtonOnAllDesktops:
        case AbilityButtonHelp:
        case AbilityButtonMinimize:
        case AbilityButtonMaximize:
        case AbilityButtonClose:
            return true;
        default:
            return false;
    };
}

QValueList<KDecorationFactory::BorderSize> SystemDecoFactory::borderSizes() const
{ // the list must be sorted
   return QValueList< BorderSize >() << BorderNormal;
}

}

extern "C" KDE_EXPORT KDecorationFactory *create_factory()
{
   return new System::SystemDecoFactory();
}

#include "systemclient.moc"
