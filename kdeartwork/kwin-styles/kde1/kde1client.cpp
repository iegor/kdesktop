/*****************************************************************
kwin - the KDE window manager
								
Copyright (C) 1999, 2000 Matthias Ettrich <ettrich@kde.org>
******************************************************************/
#include "kde1client.h"
#include <qcursor.h>
#include <qlayout.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <qdrawutil.h>
#include <qbitmap.h>
#include <kdrawutil.h>
#include <klocale.h>
#include <kpixmap.h>
#include <qdatetime.h>
#include <qimage.h>
#include <qtooltip.h>

#include "kde1client_bitmaps.h"

// Default button layout
const char default_left[]  = "MS";
const char default_right[] = "H_IAX";

namespace KDE1
{

QPixmap* close_pix = 0;
QPixmap* maximize_pix = 0;
QPixmap* minimize_pix = 0;
QPixmap* normalize_pix = 0;
QPixmap* pinup_pix = 0;
QPixmap* pindown_pix = 0;
QPixmap* menu_pix = 0;
QPixmap* question_mark_pix = 0;

QPixmap* dis_close_pix = 0;
QPixmap* dis_maximize_pix = 0;
QPixmap* dis_minimize_pix = 0;
QPixmap* dis_normalize_pix = 0;
QPixmap* dis_pinup_pix = 0;
QPixmap* dis_pindown_pix = 0;
QPixmap* dis_menu_pix = 0;
QPixmap* dis_question_mark_pix = 0;

QPixmap* titleBuffer = 0;

bool pixmaps_created = FALSE;

inline const KDecorationOptions* options() { return KDecoration::options(); }

void create_pixmaps()
{
    if ( pixmaps_created )
        return;
    pixmaps_created = true;
    QColorGroup aGrp = options()->colorGroup(KDecorationOptions::ColorButtonBg, true);
    QColorGroup iGrp = options()->colorGroup(KDecorationOptions::ColorButtonBg, false);

    QPainter aPainter, iPainter;
    close_pix = new QPixmap(16, 16);
    dis_close_pix = new QPixmap(16, 16);
    aPainter.begin(close_pix); iPainter.begin(dis_close_pix);
    kColorBitmaps(&aPainter, aGrp, 0, 0, 16, 16, true, close_white_bits,
                  NULL, NULL, close_dgray_bits, NULL, NULL);
    kColorBitmaps(&iPainter, iGrp, 0, 0, 16, 16, true, close_white_bits,
                  NULL, NULL, close_dgray_bits, NULL, NULL);
    aPainter.end(); iPainter.end();
    close_pix->setMask(QBitmap(16, 16, close_mask_bits, true));
    dis_close_pix->setMask(*close_pix->mask());

    minimize_pix = new QPixmap(16, 16);
    dis_minimize_pix = new QPixmap(16, 16);
    aPainter.begin(minimize_pix); iPainter.begin(dis_minimize_pix);
    kColorBitmaps(&aPainter, aGrp, 0, 0, 16, 16, true, iconify_white_bits,
                  NULL, NULL, iconify_dgray_bits, NULL, NULL);
    kColorBitmaps(&iPainter, iGrp, 0, 0, 16, 16, true, iconify_white_bits,
                  NULL, NULL, iconify_dgray_bits, NULL, NULL);
    aPainter.end(); iPainter.end();
    minimize_pix->setMask(QBitmap(16, 16, iconify_mask_bits, true));
    dis_minimize_pix->setMask(*minimize_pix->mask());

    maximize_pix = new QPixmap(16, 16);
    dis_maximize_pix = new QPixmap(16, 16);
    aPainter.begin(maximize_pix); iPainter.begin(dis_maximize_pix);
    kColorBitmaps(&aPainter, aGrp, 0, 0, 16, 16, true, maximize_white_bits,
                  NULL, NULL, maximize_dgray_bits, NULL, NULL);
    kColorBitmaps(&iPainter, iGrp, 0, 0, 16, 16, true, maximize_white_bits,
                  NULL, NULL, maximize_dgray_bits, NULL, NULL);
    aPainter.end(); iPainter.end();
    maximize_pix->setMask(QBitmap(16, 16, maximize_mask_bits, true));
    dis_maximize_pix->setMask(*maximize_pix->mask());

    normalize_pix = new QPixmap(16, 16);
    dis_normalize_pix = new QPixmap(16, 16);
    aPainter.begin(normalize_pix); iPainter.begin(dis_normalize_pix);
    kColorBitmaps(&aPainter, aGrp, 0, 0, 16, 16, true, maximizedown_white_bits,
                  NULL, NULL, maximizedown_dgray_bits, NULL, NULL);
    kColorBitmaps(&iPainter, iGrp, 0, 0, 16, 16, true, maximizedown_white_bits,
                  NULL, NULL, maximizedown_dgray_bits, NULL, NULL);
    aPainter.end(); iPainter.end();
    normalize_pix->setMask(QBitmap(16, 16, maximizedown_mask_bits, true));
    dis_normalize_pix->setMask(*normalize_pix->mask());

    menu_pix = new QPixmap(16, 16);
    dis_menu_pix = new QPixmap(16, 16);
    aPainter.begin(menu_pix); iPainter.begin(dis_menu_pix);
    kColorBitmaps(&aPainter, aGrp, 0, 0, 16, 16, true, menu_white_bits,
                  NULL, NULL, menu_dgray_bits, NULL, NULL);
    kColorBitmaps(&iPainter, iGrp, 0, 0, 16, 16, true, menu_white_bits,
                  NULL, NULL, menu_dgray_bits, NULL, NULL);
    aPainter.end(); iPainter.end();
    menu_pix->setMask(QBitmap(16, 16, menu_mask_bits, true));
    dis_menu_pix->setMask(*menu_pix->mask());

    pinup_pix = new QPixmap(16, 16);
    dis_pinup_pix = new QPixmap(16, 16);
    aPainter.begin(pinup_pix); iPainter.begin(dis_pinup_pix);
    kColorBitmaps(&aPainter, aGrp, 0, 0, 16, 16, true, pinup_white_bits,
                  pinup_gray_bits, NULL, pinup_dgray_bits, NULL, NULL);
    kColorBitmaps(&iPainter, iGrp, 0, 0, 16, 16, true, pinup_white_bits,
                  pinup_gray_bits, NULL, pinup_dgray_bits, NULL, NULL);
    aPainter.end(); iPainter.end();
    pinup_pix->setMask(QBitmap(16, 16, pinup_mask_bits, true));
    dis_pinup_pix->setMask(*pinup_pix->mask());

    pindown_pix = new QPixmap(16, 16);
    dis_pindown_pix = new QPixmap(16, 16);
    aPainter.begin(pindown_pix); iPainter.begin(dis_pindown_pix);
    kColorBitmaps(&aPainter, aGrp, 0, 0, 16, 16, true, pindown_white_bits,
                  pindown_gray_bits, NULL, pindown_dgray_bits, NULL, NULL);
    kColorBitmaps(&iPainter, iGrp, 0, 0, 16, 16, true, pindown_white_bits,
                  pindown_gray_bits, NULL, pindown_dgray_bits, NULL, NULL);
    aPainter.end(); iPainter.end();
    pindown_pix->setMask(QBitmap(16, 16, pindown_mask_bits, true));
    dis_pindown_pix->setMask(*pindown_pix->mask());

    question_mark_pix = new QPixmap(16, 16);
    dis_question_mark_pix = new QPixmap(16, 16);
    aPainter.begin(question_mark_pix); iPainter.begin(dis_question_mark_pix);
    kColorBitmaps(&aPainter, aGrp, 0, 0, 16, 16, true, help_light_bits,
                  NULL, NULL, help_dark_bits, NULL, NULL);
    kColorBitmaps(&iPainter, iGrp, 0, 0, 16, 16, true, help_light_bits,
                  NULL, NULL, help_dark_bits, NULL, NULL);
    aPainter.end(); iPainter.end();
    question_mark_pix->setMask(QBitmap(16, 16, help_mask_bits, true));
    dis_question_mark_pix->setMask(*question_mark_pix->mask());

    titleBuffer = new KPixmap;
}

void delete_pixmaps()
{
    delete close_pix;
    delete maximize_pix;
    delete minimize_pix;
    delete normalize_pix;
    delete pinup_pix;
    delete pindown_pix;
    delete menu_pix;
    delete question_mark_pix;
    delete dis_close_pix;
    delete dis_maximize_pix;
    delete dis_minimize_pix;
    delete dis_normalize_pix;
    delete dis_pinup_pix;
    delete dis_pindown_pix;
    delete dis_menu_pix;
    delete dis_question_mark_pix;
    delete titleBuffer;
    pixmaps_created = false;
}

void drawGradient
(
  QPainter & p,
  const QRect & t,
  const QColor & c1,
  const QColor & c2
)
{
  // Don't draw a million vertical lines if we don't need to.

  if (c1 == c2  || QPixmap::defaultDepth() <= 8)
  {
    p.fillRect(t, c1);
    return;
  }

  // rikkus: Adapted from KPixmapEffect::gradient().

  int rca = c1.red();
  int gca = c1.green();
  int bca = c1.blue();

  int rDiff = c2.red()   - rca;
  int gDiff = c2.green() - gca;
  int bDiff = c2.blue()  - bca;

  int rl = rca << 16;
  int gl = gca << 16;
  int bl = bca << 16;

  int rcdelta = ((1 << 16) / t.width()) * rDiff;
  int gcdelta = ((1 << 16) / t.width()) * gDiff;
  int bcdelta = ((1 << 16) / t.width()) * bDiff;

  for (int x = 0; x < t.width(); x++)
  {
    rl += rcdelta;
    gl += gcdelta;
    bl += bcdelta;

    p.setPen(QColor(rl >> 16, gl >> 16, bl >> 16));

    p.drawLine(t.x() + x, 0, t.x() + x, t.y() + t.height() - 1);
  }
}


void StdClient::reset( unsigned long )
{
    if (button[ButtonMenu] && (icon().pixmap( QIconSet::Small, QIconSet::Normal ).isNull()))
        button[ButtonMenu]->setIconSet(isActive() ? *menu_pix : *dis_menu_pix);
    if (button[ButtonSticky])
        button[ButtonSticky]->setIconSet(isOnAllDesktops() ? isActive() ? *pindown_pix : *dis_pindown_pix :
                                         isActive() ? *pinup_pix : *dis_pinup_pix );
    if (button[ButtonMinimize])
        button[ButtonMinimize]->setIconSet(isActive() ? *minimize_pix : *dis_minimize_pix);
    if (button[ButtonMaximize])
        button[ButtonMaximize]->setIconSet(isActive() ? *maximize_pix : *dis_maximize_pix);
    if (button[ButtonClose])
        button[ButtonClose]->setIconSet(isActive() ? *close_pix : *dis_close_pix);
    if (button[ButtonHelp])
        button[ButtonHelp]->setIconSet(isActive() ? *question_mark_pix : *dis_question_mark_pix);

    widget()->setFont(options()->font(true));
}


StdClient::StdClient( KDecorationBridge* b, KDecorationFactory* f )
    : KDecoration( b, f )
{
}

void StdClient::init()
{
    createMainWidget();
    widget()->installEventFilter( this );

    widget()->setFont(options()->font(isActive() ));

    QGridLayout* g = new QGridLayout( widget(), 0, 0, 3, 2 );
    g->setRowStretch( 1, 10 );
    if( isPreview())
        g->addWidget( new QLabel( i18n( "<center><b>KDE 1 preview</b></center>" ), widget()), 1, 1 );
    else
        g->addItem( new QSpacerItem( 0, 0 ), 1, 1 ); //no widget in the middle
    g->addItem( new QSpacerItem( 0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding ) );

    g->addColSpacing(0, 1);
    g->addColSpacing(2, 1);
    g->addRowSpacing(2, 1);

    int fh = widget()->fontMetrics().lineSpacing();
    titlebar = new QSpacerItem(10, fh, QSizePolicy::Expanding,
			       QSizePolicy::Minimum );

    QBoxLayout* hb = new QBoxLayout(0, QBoxLayout::LeftToRight, 0, 0, 0);
    g->addLayout( hb, 0, 1 );

    // setup titlebar buttons
    for (int n=0; n<ButtonTypeCount; n++) button[n] = 0;
    addButtons(hb, options()->customButtonPositions() ? options()->titleButtonsLeft() : QString(default_left));
    hb->addItem(titlebar);
    addButtons(hb, options()->customButtonPositions() ? options()->titleButtonsRight() : QString(default_right));

    for (int n=0; n<ButtonTypeCount; n++) {
	if ( !button[n] )
	    continue;
	button[n]->setBackgroundMode( PaletteBackground );
	button[n]->setMouseTracking( FALSE );
	button[n]->setAutoRaise( TRUE );
	button[n]->setFocusPolicy( QWidget::NoFocus );
	button[n]->setFixedSize( 20, 20 );
    }

#if 0 // not sure what this is supposed to mean (and why)
    if ( isTransient() ) {
	// lighter decoration for transient windows
	button[1]->hide();
//	button[2]->hide();
	button[3]->hide();
	button[4]->hide();
    }
#endif
}

void StdClient::addButtons(QBoxLayout *hb, const QString& s)
{
    if (s.length() > 0) {
        for (unsigned n=0; n < s.length(); n++) {
            switch (s[n]) {
              case 'M': // Menu button
                  if (!button[ButtonMenu]) {
                      button[ButtonMenu] = new QToolButton( widget(), 0 );
                      QToolTip::add( button[ButtonMenu], i18n("Menu") );
                      button[ButtonMenu]->setCursor(ArrowCursor);
                      if( icon().pixmap( QIconSet::Small, QIconSet::Normal ).isNull())
                          button[ButtonMenu]->setIconSet(isActive() ? *menu_pix : *dis_menu_pix);
                      else
                          button[ButtonMenu]->setIconSet( icon().pixmap( QIconSet::Small, QIconSet::Normal ));
                      connect( button[0], SIGNAL( pressed() ), this, SLOT( menuButtonPressed() ) );
                      hb->addWidget(button[ButtonMenu]);
                  }
                  break;

              case 'S': // Sticky button
                  if (!button[ButtonSticky]) {
                     button[ButtonSticky] = new QToolButton( widget(), 0 );
                     button[ButtonSticky]->setIconSet( isOnAllDesktops()?*pindown_pix:*pinup_pix );
                     QToolTip::add( button[ButtonSticky], isOnAllDesktops()?i18n("Not On All Desktops"):i18n("On All Desktops")  );
                     button[ButtonSticky]->setCursor(ArrowCursor);
                     button[ButtonSticky]->setIconSet(isOnAllDesktops() ? isActive() ? *pindown_pix : *dis_pindown_pix :
                                                      isActive() ? *pinup_pix : *dis_pinup_pix );
                     connect( button[ButtonSticky], SIGNAL( clicked() ), this, ( SLOT( toggleOnAllDesktops() ) ) );
                     hb->addWidget(button[ButtonSticky]);
                  }
                  break;

              case 'H': // Help button
                  if ((!button[ButtonHelp]) && providesContextHelp()) {
                      button[ButtonHelp] = new QToolButton( widget(), 0 );
                      QToolTip::add( button[ButtonHelp], i18n("Help") );
                      button[ButtonHelp]->setIconSet( isActive() ? *question_mark_pix : *dis_question_mark_pix);
                      connect( button[ButtonHelp], SIGNAL( clicked() ), this, ( SLOT( showContextHelp() ) ) );
                      button[ButtonHelp]->setCursor(ArrowCursor);
                      hb->addWidget(button[ButtonHelp]);
                  }
                  break;

              case 'I': // Minimize button
                  if ((!button[ButtonMinimize]) && isMinimizable())  {
                      button[ButtonMinimize] = new QToolButton( widget(), 0 );
                      QToolTip::add( button[ButtonMinimize], i18n("Minimize") );
                      button[ButtonMinimize]->setCursor(ArrowCursor);
                      button[ButtonMinimize]->setIconSet(isActive() ? *minimize_pix : *dis_minimize_pix);
                      connect( button[ButtonMinimize], SIGNAL( clicked() ), this, ( SLOT( minimize() ) ) );
                      hb->addWidget(button[ButtonMinimize]);
                  }
                  break;

              case 'A': // Maximize button
                  if ((!button[ButtonMaximize]) && isMaximizable()) {
                      const bool max = maximizeMode()!=MaximizeRestore;
                      button[ButtonMaximize] = new ThreeButtonButton( widget(), 0 );
                      QToolTip::add( button[ButtonMaximize], max?i18n("Restore"):i18n("Maximize") );
                      button[ButtonMaximize]->setCursor(ArrowCursor);
                      button[ButtonMaximize]->setIconSet( max?(isActive() ? *normalize_pix : *dis_normalize_pix):(isActive() ? *maximize_pix : *dis_maximize_pix) );
                      connect( button[ButtonMaximize], SIGNAL( clicked(ButtonState) ),
                               this, ( SLOT( maxButtonClicked(ButtonState) ) ) );
                      hb->addWidget(button[ButtonMaximize]);
                  }
                  break;

              case 'X': // Close button
                  if ((!button[ButtonClose]) && isCloseable()) {
                      button[ButtonClose] = new QToolButton( widget(), 0 );
                      QToolTip::add( button[ButtonClose], i18n("Close") );
                      button[ButtonClose]->setCursor(ArrowCursor);
                      button[ButtonClose]->setIconSet(isActive() ? *close_pix : *dis_close_pix);
                      connect( button[ButtonClose], SIGNAL( clicked() ), this, ( SLOT( closeWindow() ) ) );
                      hb->addWidget(button[ButtonClose]);
                  }
                  break;

              case '_': // Spacer item
	          hb->addItem( new QSpacerItem( 5, 0, QSizePolicy::Fixed, QSizePolicy::Expanding ) );
            }
        }
    }
}

void StdClient::activeChange()
{
    bool on = isActive();
    if (button[ButtonMenu] && (icon().pixmap( QIconSet::Small, QIconSet::Normal ).isNull()))
        button[ButtonMenu]->setIconSet(on ? *menu_pix : *dis_menu_pix);
    if (button[ButtonSticky])
        button[ButtonSticky]->setIconSet(isOnAllDesktops() ? on ? *pindown_pix : *dis_pindown_pix :
                          on ? *pinup_pix : *dis_pinup_pix );
    if (button[ButtonMinimize])
        button[ButtonMinimize]->setIconSet(on ? *minimize_pix : *dis_minimize_pix);
    if (button[ButtonMaximize])
        button[ButtonMaximize]->setIconSet(on ? *maximize_pix : *dis_maximize_pix);
    if (button[ButtonClose])
        button[ButtonClose]->setIconSet(on ? *close_pix : *dis_close_pix);
    if (button[ButtonHelp])
        button[ButtonHelp]->setIconSet(on ? *question_mark_pix : *dis_question_mark_pix);
    widget()->repaint( titlebar->geometry(), false );
}


StdClient::~StdClient()
{
    for (int n=0; n<ButtonTypeCount; n++) {
        if (button[n]) delete button[n];
    }
}


void StdClient::resizeEvent( QResizeEvent* )
{
    QRegion rr = widget()->rect();
    QRect t = titlebar->geometry();
//     t.setTop( 0 );
//     QRegion r = rr.subtract( QRect( t.x()+1, 0, t.width()-2, 1 ) );
//     setMask( r );

    if ( widget()->isVisibleToTLW() && !widget()->testWFlags( WStaticContents )) {
	// manual clearing without the titlebar (we selected WResizeNoErase )
	QRect cr( 2, 2, width()-4, height()- 4 );
	widget()->erase( QRegion( cr ).subtract( t ) );
    }
}

/*!\reimp
 */
void StdClient::captionChange()
{
    widget()->repaint( titlebar->geometry(), FALSE );
}


/*!\reimp
 */
void StdClient::maximizeChange()
{
    bool m = maximizeMode() == MaximizeFull;
    if (button[ButtonMaximize]) {
        button[ButtonMaximize]->setIconSet( m?*normalize_pix:*maximize_pix  );
        QToolTip::remove( button[ButtonMaximize] );
        QToolTip::add( button[ButtonMaximize], m ? i18n("Restore") : i18n("Maximize") );
    }
}


/*!\reimp
 */
void StdClient::desktopChange()
{
    bool s = isOnAllDesktops();
    if (button[ButtonSticky]) {
        button[ButtonSticky]->setIconSet( s?*pindown_pix:*pinup_pix );
        QToolTip::remove( button[ButtonSticky] );
        QToolTip::add( button[ButtonSticky], s ? i18n("Not On All Desktops") : i18n("On All Desktops") );
    }
}

void StdClient::paintEvent( QPaintEvent* )
{
    QPainter p( widget() );
    QRect t = titlebar->geometry();
    QRegion r = widget()->rect();
    r = r.subtract( t );
    p.setClipRegion( r );
    qDrawWinPanel( &p, widget()->rect(), widget()->colorGroup() );
//     t.setTop( 1 );
//     p.setClipRegion( t );
//     t.setTop( 0 );

    QRect titleRect( 0, 0, t.width(), t.height() );
    titleBuffer->resize( titleRect.width(), titleRect.height() );
    QPainter p2( titleBuffer );

    drawGradient(p2, titleRect, options()->color(KDecorationOptions::ColorTitleBar, isActive()),
        options()->color(KDecorationOptions::ColorTitleBlend, isActive()));
//     p.setPen( options()->color(KDecorationOptions::ColorTitleBar, isActive()).light() );
//     p.drawLine(t.left(), t.top()+1,  t.right(), t.top()+1);
    if ( isActive() )
	qDrawShadePanel( &p2, 0, 0, titleRect.width(), titleRect.height(),
			 widget()->colorGroup(), true, 1 );
    titleRect.setLeft( 4 );
    titleRect.setWidth( titleRect.width() - 2 );
    p2.setPen(options()->color(KDecorationOptions::ColorFont, isActive()));
    p2.setFont(options()->font(isActive()));
    p2.drawText( titleRect, AlignLeft|AlignVCenter|SingleLine, caption() );
    p2.end();
    p.end();
    bitBlt( widget(), t.topLeft(), titleBuffer );
}


void StdClient::mouseDoubleClickEvent( QMouseEvent * e )
{
    if ( e->button() == LeftButton && titlebar->geometry().contains( e->pos() ) )
	titlebarDblClickOperation();
}


void StdClient::wheelEvent( QWheelEvent * e )
{
    if (isSetShade() || QRect( 0, 0, width(), titlebar->geometry().height() ).contains( e->pos() ) )
        titlebarMouseWheelOperation( e->delta());
}


void StdClient::iconChange()
{
    if (button[ButtonMenu]) {
        if ( icon().pixmap( QIconSet::Small, QIconSet::Normal ).isNull())
            button[ButtonMenu]->setIconSet(isActive() ? *menu_pix : *dis_menu_pix);
        else
            button[ButtonMenu]->setIconSet( icon().pixmap( QIconSet::Small, QIconSet::Normal ) );
        button[ButtonMenu]->repaint( FALSE );
    }
}


/*!
  Indicates that the menu button has been clicked. One press shows
  the window operation menu, a double click closes the window.
 */
void StdClient::menuButtonPressed()
{
    QRect menuRect = button[ButtonMenu]->rect();
    menuRect.addCoords(-1, 0, +1, +2);
    QPoint menuTop = button[ButtonMenu]->mapToGlobal(menuRect.topLeft());
    QPoint menuBottom = button[ButtonMenu]->mapToGlobal(menuRect.bottomRight());
    KDecorationFactory* f = factory(); // needs to be saved before
    showWindowMenu( QRect(menuTop, menuBottom) );
    if( !f->exists( this )) // destroyed, return immediately
        return;
    button[ButtonMenu]->setDown(false);
}


void StdClient::maxButtonClicked( ButtonState button )
{
    maximize( button );
}

bool StdClient::eventFilter( QObject* o, QEvent* e )
{
    if ( o != widget() )
	return false;

    switch ( e->type() ) {
    case QEvent::Resize:
	resizeEvent( static_cast< QResizeEvent* >( e ) );
	return true;

    case QEvent::Paint:
	paintEvent( static_cast< QPaintEvent* >( e ) );
	return true;

    case QEvent::MouseButtonDblClick:
	mouseDoubleClickEvent( static_cast< QMouseEvent* >( e ) );
	return true;

    case QEvent::MouseButtonPress:
	processMousePressEvent( static_cast< QMouseEvent* >( e ) );
	return true;

    case QEvent::Wheel:
	wheelEvent( static_cast< QWheelEvent* >( e ));
	return true;

    default:
	return false;
    }
}

QSize StdClient::minimumSize() const
{
    return widget()->minimumSize().expandedTo( QSize( 100, 50 ));
}

void StdClient::borders( int& left, int& right, int& top, int& bottom ) const
{
    left = right = bottom = 6;
    top = 6 + widget()->fontMetrics().lineSpacing()+2;
}

void StdClient::resize( const QSize& s )
{
    widget()->resize( s );
}

StdToolClient::StdToolClient( KDecorationBridge* b, KDecorationFactory* f )
    : KDecoration( b, f )
{
}

void StdToolClient::init()
{
    createMainWidget();
    widget()->installEventFilter( this );

    widget()->setFont(options()->font(isActive(), true ));

    QGridLayout* g = new QGridLayout( widget(), 0, 0, 2 );
    g->setRowStretch( 1, 10 );
    if( isPreview())
        g->addWidget( new QLabel( i18n( "<center><b>KDE 1 decoration</b></center>" ), widget()), 1, 1 );
    else
        g->addItem( new QSpacerItem( 0, 0 ), 1, 1 ); //no widget in the middle
    g->addItem( new QSpacerItem( 0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding ) );

    g->addColSpacing(0, 1);
    g->addColSpacing(2, 1);
    g->addRowSpacing(2, 1);

    closeBtn = new QToolButton( widget(), 0 );
    QToolTip::add( closeBtn, i18n("Close") );
    connect( closeBtn, SIGNAL( clicked() ), this, ( SLOT( closeWindow() ) ) );
    closeBtn->setFixedSize( 13, 13);
    reset( -1U );

    QHBoxLayout* hb = new QHBoxLayout;
    g->addLayout( hb, 0, 1 );

    int fh = widget()->fontMetrics().lineSpacing()+2;

    titlebar = new QSpacerItem(10, fh, QSizePolicy::Expanding,
			       QSizePolicy::Minimum );
    hb->addItem( titlebar );
    hb->addWidget( closeBtn );
}

StdToolClient::~StdToolClient()
{
}

void StdToolClient::resizeEvent( QResizeEvent* )
{
//     QRegion r = rect();
//     QRect t = titlebar->geometry();
//     t.setTop( 0 );
//     r = r.subtract( QRect(0, 0, width(), 1) );
//     r = r.subtract (QRect( 0, 0, 1, t.height() ) );
//     r = r.subtract (QRect( width()-1, 0, 1, t.height() ) );
//     setMask( r );
}

void StdToolClient::paintEvent( QPaintEvent* )
{
    QPainter p( widget() );
    QRect t = titlebar->geometry();
    QRect r = widget()->rect();
    qDrawWinPanel( &p, r, widget()->colorGroup() );
    r.setTop( t.bottom()+1 );
    qDrawWinPanel( &p, r, widget()->colorGroup() );
    p.fillRect( QRect( QPoint(t.topLeft() ), QPoint( width() - t.left(), t.bottom() ) ),
		options()->color(KDecorationOptions::ColorTitleBar, isActive()));
    p.setPen( options()->color(KDecorationOptions::ColorTitleBar, isActive()).light() );
    t.setLeft( t.left() + 4 );
    t.setRight( t.right() - 2 );
    p.setPen(options()->color(KDecorationOptions::ColorFont, isActive()));
    p.setFont(options()->font(isActive(), true));
    p.drawText( t, AlignLeft|AlignVCenter|SingleLine, caption() );
}


void StdToolClient::mouseDoubleClickEvent( QMouseEvent * e )
{
    if ( e->button() == LeftButton && titlebar->geometry().contains( e->pos() ) )
        titlebarDblClickOperation();
}

void StdToolClient::wheelEvent( QWheelEvent * e )
{
    if (isSetShade() || QRect( 0, 0, width(), titlebar->geometry().height() ).contains( e->pos() ) )
        titlebarMouseWheelOperation( e->delta());
}

void StdToolClient::captionChange()
{
    widget()->repaint( titlebar->geometry(), FALSE );
}

void StdToolClient::reset( unsigned long )
{
    QImage img = close_pix->convertToImage();
    img = img.smoothScale( 12, 12 );
    QPixmap pm;
    pm.convertFromImage( img );
    closeBtn->setPixmap( pm );
    widget()->setFont(options()->font(isActive(), true ));
}

bool StdToolClient::eventFilter( QObject* o, QEvent* e )
{
    if ( o != widget() )
	return false;

    switch ( e->type() ) {
    case QEvent::Resize:
	resizeEvent( static_cast< QResizeEvent* >( e ) );
	return true;

    case QEvent::Paint:
	paintEvent( static_cast< QPaintEvent* >( e ) );
	return true;

    case QEvent::MouseButtonDblClick:
	mouseDoubleClickEvent( static_cast< QMouseEvent* >( e ) );
	return true;

    case QEvent::MouseButtonPress:
	processMousePressEvent( static_cast< QMouseEvent* >( e ) );
	return true;

    case QEvent::Wheel:
	wheelEvent( static_cast< QWheelEvent* >( e ));
	return true;

    default:
	return false;
    }
}

QSize StdToolClient::minimumSize() const
{
    return widget()->minimumSize().expandedTo( QSize( 100, 50 ));
}

void StdToolClient::borders( int& left, int& right, int& top, int& bottom ) const
{
    left = right = bottom = 6;
    top = 6 + widget()->fontMetrics().lineSpacing();
}

void StdToolClient::resize( const QSize& s )
{
    widget()->resize( s );
}

StdFactory::StdFactory()
{
    create_pixmaps();
}

StdFactory::~StdFactory()
{
    delete_pixmaps();
}

const int SUPPORTED_WINDOW_TYPES_MASK = NET::NormalMask | NET::DesktopMask | NET::DockMask
    | NET::ToolbarMask | NET::MenuMask | NET::DialogMask | NET::OverrideMask | NET::TopMenuMask
    | NET::UtilityMask | NET::SplashMask;

KDecoration* StdFactory::createDecoration( KDecorationBridge* b )
{
    NET::WindowType type = windowType( SUPPORTED_WINDOW_TYPES_MASK, b );
    if( type == NET::Utility || type == NET::Menu || type == NET::Toolbar )
	return new StdToolClient( b, this );
    return new StdClient( b, this );
}

bool StdFactory::reset( unsigned long mask )
{
    bool needHardReset = false;

    // doesn't obey the Border size setting
    if( mask & ( SettingFont | SettingButtons ))
        needHardReset = true;

    if( mask & ( SettingFont | SettingColors )) {
        KDE1::delete_pixmaps();
        KDE1::create_pixmaps();
    }

    if( !needHardReset )
        resetDecorations( mask );
    return needHardReset;
}

bool StdFactory::supports( Ability ability )
{
    switch( ability )
    {
        case AbilityAnnounceButtons:
        case AbilityButtonMenu:
        case AbilityButtonOnAllDesktops:
        case AbilityButtonHelp:
        case AbilityButtonMinimize:
        case AbilityButtonMaximize:
        case AbilityButtonClose:
        case AbilityButtonSpacer:
            return true;
        default:
            return false;
    };
}


}

extern "C"
{
	KDE_EXPORT KDecorationFactory *create_factory()
	{
                return new KDE1::StdFactory();
	}
}

#include "kde1client.moc"
