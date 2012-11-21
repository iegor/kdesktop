/*
 *	$Id: cdeclient.cpp 489039 2005-12-16 22:05:28Z giessl $
 *
 *	CDE KWin client - emulates the look and feel
 *	of dtwm, the CDE window manager.
 *
 *	Copyright (c) 2000-2001, 2002
 *		Chris Lee       <lee@azsites.com>
 *		Lennart Kudling <kudling@kde.org>
 *      	Fredrik Höglund <fredrik@kde.org>
 *
 *	Copyright (c) 2003,2004
 *		Luciano Montanaro <mikelima@cirulla.net>
 *
 *	Originally based on the KStep client.
 *
 *	Distributed under the terms of the BSD license.
 */

#include "cdeclient.h"
#include <qdatetime.h>
#include <qlayout.h>
#include <qbutton.h>
#include <qcursor.h>
#include <qlabel.h>
#include <qtooltip.h>
#include <qdrawutil.h>
#include <qpainter.h>
#include <qapplication.h>
#include <klocale.h>
#include <kconfig.h>

extern "C" KDE_EXPORT KDecorationFactory* create_factory()
{
    return new CDE::CdeClientFactory();
}

namespace CDE {

static int s_frameWidth = 5;
static int s_buttonSize = 19;
static bool titlebarButtonMode = true;
static bool coloredFrame = true;
static Qt::AlignmentFlags textAlignment = Qt::AlignHCenter;

// Precomputed border sizes for accessibility
// The sizes are applied for tiny -> normal -> large -> very large -> huge ->
// very huge -> oversized
static const int borderSizes[] = { 4, 6, 9, 12, 18, 26, 42 };

// Parameters needed to draw the widgets (offsets from the border)
static int s_o1 = 4;
static int s_o2 = 7;
static int s_w1 = 11;
static int s_w2 = 5;

// These are the line segments for the X on the close button

static const int NUM_CLOSEL_COORDS = 2 * 14;
static const QCOORD closeLLinesTemplate[NUM_CLOSEL_COORDS] =
    { 14,3, 12,3,  12,3, 9,6,  5,3, 3,3,  3,3, 3,5,
      3,5, 6,8, 6,9, 3,12,  3,12, 3,14 };

static const int NUM_CLOSED_COORDS = 2 * 18;
static const QCOORD closeDLinesTemplate[NUM_CLOSED_COORDS] =
    { 5,3, 8,6,  14,4, 14,5,  14,5, 11,8,  11,9, 14,12,  14,12, 14,14,
      14,14, 12,14,  12,14, 9,11,  8,11, 5,14,  5,14, 4,14 };

static QCOORD closeLLines[NUM_CLOSEL_COORDS];

static QCOORD closeDLines[NUM_CLOSED_COORDS];

// These are the line segments for the ? on the help button
static const int NUM_HELPL_COORDS = 2 * 16;
static const QCOORD helpLLinesTemplate[NUM_HELPL_COORDS] =
    { 4,6, 4,5,  4,5, 6,3,  6,3, 9,3,  10,3, 11,4,
      9,7, 7,9,  7,9, 7,10,  7,14, 7,13,  8,12, 9,12  };

static const int NUM_HELPD_COORDS = 2 * 14;
static const QCOORD helpDLinesTemplate[NUM_HELPD_COORDS] =
    {  5,7, 8,6,  12,5, 12,8,  12,8, 10,10,  10,10, 10,11,
       10,11, 8,11,  10,14, 10,13,  9,15, 8,15 };

static QCOORD helpLLines[NUM_HELPL_COORDS];

static QCOORD helpDLines[NUM_HELPD_COORDS];


// This question mark is taller than the one above and
// is positioned one pixel higher on the button
/*
static const QCOORD helpLLines[] =
    { 4,5, 4,4,  4,4, 6,2,  6,2, 9,2,  10,2, 11,3,
      9,6, 7,8,  7,9, 7,10,  7,13, 8,12,  8,12, 9,12  };

static const QCOORD helpDLines[] =
    {  5,6, 8,5,  12,4, 12,7,  12,7, 10,9,  10,10, 10,11,
       10,11, 8,11,  10,13, 9,14,  9,14, 8,14 };
*/
// Same as the one above but with a larger dot under
// the question mark
/*
static const QCOORD helpLLines[] =
    { 4,5, 4,4,  4,4, 6,2,  6,2, 9,2,  10,2, 11,3,
      9,6, 7,8,  7,9, 7,10,  7,14, 7,13,  8,12, 9,12  };

static const QCOORD helpDLines[] =
    {  5,6, 8,5,  12,4, 12,7,  12,7, 10,9,  10,10, 10,11,
       10,11, 8,11,  10,13, 10,14,  9,15, 8,15 };
*/

static inline const KDecorationOptions* options()
{
    return KDecoration::options();
}

static void fixColorGroup(QColorGroup & colorGroup)
{
    QColor light = colorGroup.light();

    int hue, saturation, value;

    light.hsv(&hue, &saturation, &value);

    if (value < 128)
    {
      light.setHsv(hue, saturation, 128);
      colorGroup.setColor(QColorGroup::Light, light);
    }

    QColor dark = colorGroup.dark();

    dark.hsv(&hue, &saturation, &value);

    if (value < 84)
    {
      dark.setHsv(hue, saturation, 84);
      colorGroup.setColor(QColorGroup::Dark, dark);
    }
}

// scaling helper function used to scale the close 'X' glyph

static int scaleCoord(int c)
{
    if (c < 6) return c;
    if (c <= 11) return c + (s_buttonSize - 19) / 2;
    return c + s_buttonSize - 19;
}

static void readConfig(CdeClientFactory *f)
{
    KConfig conf( "kwincderc" );

    conf.setGroup("General");
    coloredFrame = conf.readBoolEntry( "UseTitleBarBorderColors", true );
    titlebarButtonMode = conf.readBoolEntry( "TitlebarButtonMode", true );

    QString value = conf.readEntry( "TextAlignment", "AlignHCenter" );
    if ( value == "AlignLeft" )
	textAlignment = Qt::AlignLeft;
    else if ( value == "AlignHCenter" )
	textAlignment = Qt::AlignHCenter;
    else if ( value == "AlignRight" )
	textAlignment = Qt::AlignRight;


    // find preferred border size
    int i = options()->preferredBorderSize(f);
    if (i >= 0 && i <= 6) s_frameWidth = borderSizes[i];

    // Do not allow malicious users or corrupt config files to
    // go past the domain of the valid border sizes.

    // Size limit increased for accessability. LM
    if (s_frameWidth < 0)  s_frameWidth = 0;
    if (s_frameWidth > 30) s_frameWidth = 30;

    // Force button size to be in a reasonable range.
    // If the frame width is large, the button size must be large too.
    s_buttonSize = QFontMetrics(options()->font( true )).height() + 2;
    if (s_buttonSize < 19) s_buttonSize = 19;
    if (s_buttonSize < s_frameWidth) s_buttonSize = s_frameWidth;
    s_buttonSize |= 1; // Be sure the button size is odd.

    // Calculate widths and offsets for the button icons
    s_o1 = s_buttonSize * 4 / 19;
    s_o2 = s_buttonSize * 7 / 19;
    s_w1 = s_buttonSize - 2 * s_o1;
    s_w2 = s_buttonSize - 2 * s_o2;

    // Copy and scale the close icon
    int offset = (s_buttonSize - 19) / 2;
    for (int i = 0; i < NUM_CLOSEL_COORDS; i++) {
	closeLLines[i] = scaleCoord(closeLLinesTemplate[i]);
    }
    for (int i = 0; i < NUM_CLOSED_COORDS; i++) {
	closeDLines[i] = scaleCoord(closeDLinesTemplate[i]);
    }
    // Copy and center the help icon
    for (int i = 0; i < NUM_HELPL_COORDS; i++) {
	helpLLines[i] = helpLLinesTemplate[i] + offset;
    }
    for (int i = 0; i < NUM_HELPD_COORDS; i++) {
	helpDLines[i] = helpDLinesTemplate[i] + offset;
    }
}

// ---------------------------------------

CdeClientFactory::CdeClientFactory()
{
    CDE::readConfig(this);
}

CdeClientFactory::~CdeClientFactory()
{
}

KDecoration *CdeClientFactory::createDecoration(KDecorationBridge *b)
{
    return new CdeClient(b, this);
}

bool CdeClientFactory::reset(unsigned long /*changed*/)
{
    // TODO Do not recreate decorations if it is not needed. Look at
    // ModernSystem for how to do that
    // For now just return true.
    CDE::readConfig(this);
    return true;
}

bool CdeClientFactory::supports( Ability ability )
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
            return true;
        default:
            return false;
    };
}

QValueList< CdeClientFactory::BorderSize >
CdeClientFactory::borderSizes() const
{
    // the list must be sorted
    return QValueList< BorderSize >() << BorderTiny << BorderNormal <<
	BorderLarge << BorderVeryLarge <<  BorderHuge <<
	BorderVeryHuge << BorderOversized;
}

// ---------------------------------------

CdeClient::CdeClient(KDecorationBridge *b, KDecorationFactory *f)
    : KDecoration(b, f)
{
}

void CdeClient::init()
{
    createMainWidget(WStaticContents | WResizeNoErase | WRepaintNoErase);
    widget()->installEventFilter(this);

    widget()->setBackgroundMode(NoBackground);

    mainLayout = new QVBoxLayout(widget());
    QBoxLayout* windowLayout = new QBoxLayout(0, QBoxLayout::LeftToRight, 0, 0, 0);
    titleLayout = new QBoxLayout(0, QBoxLayout::LeftToRight, 0, 0, 0);

    // TODO Check if this stuff can be simplified.
    // Border sizes are from a fixed set now.
    if ( s_frameWidth > 1 )
    {
       // the style normally draws a black frame around the window, so we
       // need 1 line of space for that in addition to the normal window frame
    	mainLayout->setMargin( s_frameWidth+1 );
    }
    else
    {
	// but if the frame is set to just 1 pixel we just draw the black frame
	// instead of the normal window frame, so no extra space is needed. if
	// its 0 we don't draw anything.
    	mainLayout->setMargin( s_frameWidth );
    }

    mainLayout->addLayout( titleLayout );
    mainLayout->addLayout( windowLayout, 1 );

    if (isPreview())
        windowLayout->addWidget(new QLabel(i18n(
			"<center><b>CDE preview</b></center>"), widget()), 1);
    else
    	windowLayout->addItem( new QSpacerItem( 0, 0 ));

    for ( int i=0; i < BtnCount; i++ )
        button[i] = NULL;

    addClientButtons( options()->titleButtonsLeft() );

    titlebar = new QSpacerItem( 10, 16, QSizePolicy::Expanding, QSizePolicy::Minimum );
    titleLayout->addItem( titlebar );

    addClientButtons( options()->titleButtonsRight() );

    titlebarPressed = false;
    closing = false;
}

void CdeClient::addClientButtons( const QString& s )
{
    if ( s.length() > 0 )
        for ( unsigned int i = 0; i < s.length(); i++ )
        {
            switch( s[i].latin1() )
            {
                // Menu button
                case 'M':
                    if ( ! button[BtnMenu] )
                    {
                        button[BtnMenu] = new CdeButton( this, "menu", BtnMenu, i18n("Menu"), LeftButton|RightButton );
                        connect( button[BtnMenu], SIGNAL(pressed()), SLOT(menuButtonPressed()) );
                        connect( button[BtnMenu], SIGNAL(released()), SLOT(menuButtonReleased()) );
                        titleLayout->addWidget( button[BtnMenu] );
                    }
                    break;

                //Help button
                case 'H':
                    if ( providesContextHelp() && (! button[BtnHelp] ) )
                    {
                        button[BtnHelp] = new CdeButton( this, "help", BtnHelp, i18n("Help") );
                        connect(button[BtnHelp],
				SIGNAL(clicked()), SLOT(showContextHelp()));
                        titleLayout->addWidget( button[BtnHelp] );
                    }
                    break;

		//Minimize button
                case 'I':
                    if ( (! button[BtnIconify] ) && isMinimizable() )
                    {
                        button[BtnIconify] = new CdeButton( this, "iconify", BtnIconify, i18n("Minimize") );
                        connect(button[BtnIconify],
				SIGNAL(clicked()), SLOT(minimize()));
                        titleLayout->addWidget( button[BtnIconify] );
                    }
                    break;

                // Maximize button
                case 'A':
                    if ( (! button[BtnMax] ) && isMaximizable() )
                    {
                        button[BtnMax] = new CdeButton(this, "maximize", BtnMax, i18n("Maximize"), LeftButton|MidButton|RightButton);
                        connect(button[BtnMax], SIGNAL(clicked()),
				SLOT(maximizeButtonClicked()));
                        titleLayout->addWidget( button[BtnMax] );
                    }
                    break;

                // Close button
                case 'X':
                    if ( !button[BtnClose] && isCloseable())
                    {
                        button[BtnClose] = new CdeButton(this, "close", BtnClose, i18n("Close"));
                        connect( button[BtnClose], SIGNAL( clicked()), SLOT(closeWindow()) );
                        titleLayout->addWidget( button[BtnClose] );
                    }
		// Add onAlldesktops button and spacers
            }
        }

}

void CdeClient::captionChange()
{
    widget()->repaint(titlebar->geometry(), false);
}

void CdeClient::activeChange()
{
    for ( int i=0; i < BtnCount; i++ )
	if ( button[i] ) button[i]->reset();

    widget()->repaint(false);
}

void CdeClient::maximizeChange()
{
    if ( button[BtnMax] ) {
	bool m = maximizeMode() == MaximizeFull;
	QToolTip::remove(button[BtnMax]);
	QToolTip::add(button[BtnMax], m ? i18n("Restore") : i18n("Maximize"));
	button[BtnMax]->repaint();
    }
}

void CdeClient::iconChange()
{
}

void CdeClient::shadeChange()
{
}

void CdeClient::showEvent(QShowEvent *)
{
    widget()->repaint();
}

void CdeClient::desktopChange()
{
    // Nothing to do yet
}

QSize CdeClient::minimumSize() const
{
    return QSize(2 * (s_buttonSize + s_frameWidth),
	         2 * s_frameWidth + s_buttonSize);
}

void CdeClient::resize(const QSize& s)
{
    widget()->resize(s);
}

void CdeClient::maximizeButtonClicked()
{
    if (button[BtnMax]) {
	maximize(button[BtnMax]->lastButton());
    }
}

void CdeClient::menuButtonPressed()
{
    static QTime* t = NULL;
    static CdeClient* lastClient = NULL;
    if( t == NULL )
	t = new QTime;
    bool dbl = ( lastClient == this && t->elapsed() <= QApplication::doubleClickInterval());
    lastClient = this;
    t->start();
    if( !dbl )
    {
	QRect menuRect = button[BtnMenu]->rect();
        QPoint menuTop = button[BtnMenu]->mapToGlobal(menuRect.topLeft());
        QPoint menuBottom = 
	    button[BtnMenu]->mapToGlobal(menuRect.bottomRight());
        KDecorationFactory* f = factory();
        showWindowMenu(QRect(menuTop, menuBottom));
        if( !f->exists( this )) // 'this' was deleted
            return;
	button[BtnMenu]->setDown(false);
    }
    else
       closing = true;
}

void CdeClient::menuButtonReleased()
{
    if( closing )
        closeWindow();
}

void CdeClient::resizeEvent( QResizeEvent* e)
{
    if (widget()->isVisibleToTLW()) {
        widget()->update();
        int dx = 0;
        int dy = 0;

	    if ( e->oldSize().width() != width() )
	       dx = 32 + QABS( e->oldSize().width() -  width() );

	    if ( e->oldSize().height() != height() )
	       dy = 8 + QABS( e->oldSize().height() -  height() );

	    if ( dy )
	       widget()->update( 0, height() - dy + 1, width(), dy );

        if ( dx )
        {
	    widget()->update( width() - dx + 1, 0, dx, height() );
	    widget()->update( QRect( QPoint(4,4),
	    titlebar->geometry().bottomLeft() - QPoint(1,0) ) );
	    widget()->update(QRect(titlebar->geometry().topRight(),
			QPoint(width() - 4, titlebar->geometry().bottom())));

	    // Titlebar needs no paint event
	    QApplication::postEvent( this, new QPaintEvent( titlebar->geometry(), false ) );
	}
    }
}

void CdeClient::paintEvent( QPaintEvent* )
{
    QPainter p(widget());

    QColorGroup colorGroup;

    if ( coloredFrame )
	colorGroup = options()->colorGroup( KDecoration::ColorTitleBar, isActive() );
    else
	colorGroup = options()->colorGroup( KDecoration::ColorFrame, isActive() );

    fixColorGroup( colorGroup );

    QRect trect = titlebar->geometry();
    QRect mrect = widget()->rect();

    if ( s_frameWidth > 0 )
    {
	// draw black frame:
	p.setPen( Qt::black );
	p.drawRect( mrect );
    }

    p.setPen( Qt::NoPen );
    p.setBrush( colorGroup.background() );


    if ( s_frameWidth > 1 )
    {
	bool shaded = isShade();
	int longSide = s_frameWidth + s_buttonSize;

	// draw frame-background:
	p.drawRect( 1, 1,
    		mrect.width() - 2, s_frameWidth );
	p.drawRect( 1, mrect.height() - s_frameWidth - 1,
		mrect.width() - 2, s_frameWidth );
	p.drawRect( 1, s_frameWidth + 1,
		s_frameWidth, mrect.height() - 2*s_frameWidth - 2 );
	p.drawRect( mrect.width() - s_frameWidth - 1, s_frameWidth + 1,
		s_frameWidth, mrect.height() - 2*s_frameWidth - 2 );

	if ( ! shaded )
	{
	    // draw left  and right frames:
	    qDrawShadePanel( &p, 1, longSide + 1,
		s_frameWidth, mrect.height() - 2 * (longSide + 1),
		colorGroup );

	    qDrawShadePanel( &p, mrect.width() - s_frameWidth - 1, longSide + 1,
		s_frameWidth, mrect.height() - 2 * (longSide + 1),
		colorGroup );
	}

	// draw top and bottom frames:
	qDrawShadePanel( &p, longSide + 1, 1,
	    mrect.width() - 2 * (longSide + 1), s_frameWidth,
	    colorGroup );

	qDrawShadePanel( &p, longSide + 1, mrect.height() - s_frameWidth - 1,
	    mrect.width() - 2 * (longSide + 1), s_frameWidth,
	    colorGroup );

	// draw light corner parts:
	p.setPen( colorGroup.light() );

	// tl corner:
	p.drawLine( 1, 1, longSide - 1, 1 );
	p.drawLine( 1, 1, 1, longSide - 1 );

	// tr corner:
	p.drawLine( mrect.width() - 3, 1, mrect.width() - longSide - 1, 1 );
	p.drawLine( mrect.width() - longSide - 1, 1,
	    mrect.width() - longSide - 1, s_frameWidth - 1 );
	p.drawLine( mrect.width() - s_frameWidth - 1, s_frameWidth,
	    mrect.width() - s_frameWidth - 1, longSide - 1 );

	// br corner:
	if ( !shaded )
	{
	    p.drawLine( mrect.width() - 3, mrect.height() - longSide - 1,
		mrect.width() - s_frameWidth - 1, mrect.height() - longSide - 1 );
	}
	p.drawLine( mrect.width() - s_frameWidth - 1, mrect.height() - longSide,
	    mrect.width() - s_frameWidth - 1, mrect.height() - s_frameWidth - 1 );
	p.drawLine( mrect.width() - s_frameWidth - 2, mrect.height() - s_frameWidth - 1,
	    mrect.width() - longSide - 1, mrect.height() - s_frameWidth - 1 );
	p.drawLine( mrect.width() - longSide - 1, mrect.height() - s_frameWidth,
	    mrect.width() - longSide - 1, mrect.height() - 2 );

	// bl corner:
	if ( !shaded )
	{
	    p.drawLine( s_frameWidth-1, mrect.height() - longSide - 1,
		2, mrect.height() - longSide - 1 );
	}
	p.drawLine( 1, mrect.height() - longSide - 1,
	    1, mrect.height() - 3 );
	p.drawLine( longSide - 1, mrect.height() - s_frameWidth - 1,
	    s_frameWidth + 1, mrect.height() - s_frameWidth - 1 );

	// draw dark corner parts:
	p.setPen( colorGroup.dark() );

	// tl corner:
	if ( !shaded )
	    p.drawLine( 1, longSide, s_frameWidth, longSide );
	p.drawLine( s_frameWidth, longSide - 1, s_frameWidth, s_frameWidth );
	p.drawLine( s_frameWidth + 1, s_frameWidth, longSide, s_frameWidth );
	p.drawLine( s_frameWidth + s_buttonSize, s_frameWidth, longSide, 1 );

	// tr corner:
	p.drawLine( mrect.width() - longSide - 1, s_frameWidth,
	    mrect.width() - s_frameWidth - 2, s_frameWidth );
	if ( !shaded )
	{
	    p.drawLine( mrect.width() - s_frameWidth - 1, longSide,
		mrect.width() - 2, longSide );
	}
	p.drawLine( mrect.width() - 2, longSide, mrect.width() - 2, 1 );

	// br corner:
	p.drawLine( mrect.width() - longSide - 1, mrect.height() - 2,
	    mrect.width() - 3, mrect.height() - 2 );
	p.drawLine( mrect.width() - 2, mrect.height() - 2,
	    mrect.width() - 2, mrect.height() - longSide - 2 );

	// bl corner:
	p.drawLine( 1, mrect.height() - 2,
		longSide, mrect.height() - 2 );
	p.drawLine( s_frameWidth + s_buttonSize, mrect.height() - 3,
		longSide, mrect.height() - s_frameWidth - 1 );
	p.drawLine( s_frameWidth, mrect.height() - s_frameWidth - 1,
		s_frameWidth, mrect.height() - longSide - 1 );
    }


    p.setPen( Qt::NoPen );

    if ( !coloredFrame )
    {
	colorGroup = options()->colorGroup( KDecoration::ColorTitleBar, isActive() );
	fixColorGroup( colorGroup );
	p.setBrush( colorGroup.background() );
    }

    // draw titlebar:
    p.drawRect( trect );
    qDrawShadePanel( &p, trect, colorGroup, titlebarPressed );

    // draw caption:
    if ( titlebarPressed ) // move the caption right and down if the titlebar is pressed
	trect.moveBy( 1,1 ); // Note: the real Mwm doesn't actually do this

    p.setFont( options()->font( true ) );
    p.setPen( options()->color( KDecoration::ColorFont, isActive() ) );
    if ( p.fontMetrics().width( caption() ) > trect.width() - 6 )
    {
	// left align the text if its too wide to fit in the titlebar
	p.drawText( trect.x() + 3, trect.y(),
	    trect.width() - 6, trect.height(),
	    AlignLeft | AlignVCenter, caption() );
    }
    else
    {
	// otherwise we'll draw it according to the user settings
	p.drawText( trect.x() + 3, trect.y(),
	    trect.width() - 6, trect.height(),
	    textAlignment | AlignVCenter, caption() );
    }

    // Draw a line behind the wrapped window to prevent having
    // unpainted areas when we're shaded.
    p.setPen( colorGroup.dark() );
    p.drawLine(s_frameWidth + 1, mrect.height() - s_frameWidth - 2,
               mrect.width() - s_frameWidth - 2, mrect.height() - s_frameWidth - 2);

}

KDecoration::Position CdeClient::mousePosition( const QPoint& p ) const
{
    const int range = s_frameWidth + s_buttonSize;
    const int border = s_frameWidth + 1;

    Position m = PositionCenter;

    if ((p.x() > border && p.x() < width() - border)
         && (p.y() > border && p.y() < height() - border))
        return PositionCenter;

    if (p.y() < range && p.x() <= range)
        m = PositionTopLeft;
    else if (p.y() >= height() - range && p.x() >= width() - range)
        m = PositionBottomRight;
    else if (p.y() >= height()-range && p.x() <= range)
        m = PositionBottomLeft;
    else if (p.y() < range && p.x() >= width() - range)
        m = PositionTopRight;
    else if (p.y() < border)
        m = PositionTop;
    else if (p.y() >= height() - border)
        m = PositionBottom;
    else if (p.x() <= border)
        m = PositionLeft;
    else if (p.x() >= width() - border)
        m = PositionRight;
    else
        m = PositionCenter;
    return m;
}

void CdeClient::mouseDoubleClickEvent( QMouseEvent * e )
{
    if ( e->button() == LeftButton && titlebar->geometry().contains( e->pos() ) )
	titlebarDblClickOperation();
}

void CdeClient::wheelEvent( QWheelEvent * e )
{
    if (isSetShade() || titleLayout->geometry().contains( e->pos() ) )
        titlebarMouseWheelOperation( e->delta());
}

void CdeClient::mousePressEvent( QMouseEvent * e )
{
    if ( e->button() == LeftButton && titlebar->geometry().contains( e->pos() ) )
    {
	if ( titlebarButtonMode )
	{
	    titlebarPressed = true;
	    widget()->repaint(titlebar->geometry(), false);
	}
    }
}

void CdeClient::borders(int &left, int &right, int &top, int &bottom) const
{
    left = right = bottom = s_frameWidth + 1;
    top = s_buttonSize + s_frameWidth + 1;
}

void CdeClient::mouseReleaseEvent( QMouseEvent * e )
{
    if ( e->button() == LeftButton && titlebarPressed )
    {
	titlebarPressed = false;
	widget()->repaint(titlebar->geometry(), false);
    }
}

bool CdeClient::eventFilter(QObject *o, QEvent *e)
{
    if (o != widget())
	return false;
    switch (e->type()) {
    case QEvent::Resize:
	resizeEvent(static_cast< QResizeEvent* >(e));
	return true;
    case QEvent::Paint:
	paintEvent(static_cast< QPaintEvent* >(e));
	return true;
    case QEvent::MouseButtonDblClick:
	mouseDoubleClickEvent(static_cast< QMouseEvent* >(e));
	return true;
    case QEvent::MouseButtonPress:
	processMousePressEvent(static_cast< QMouseEvent* >(e));
	return true;
    case QEvent::Show:
	showEvent(static_cast< QShowEvent* >(e));
	return true;
    case QEvent::Wheel:
	wheelEvent( static_cast< QWheelEvent* >( e ));
	return true;
    default:
	break;
    }
    return false;
}

// ---------------------------------------

CdeButton::CdeButton(CdeClient* parent,
	const char* name, int btnType, const QString& tip, int realize_btns)
    : QButton(parent->widget(), name), m_btnType(btnType), last_button(NoButton)
{
    setBackgroundMode( QWidget::NoBackground );
    setFixedSize( s_buttonSize, s_buttonSize );
    resize( s_buttonSize, s_buttonSize );
    m_parent = parent;

    setCursor(ArrowCursor);
    QToolTip::add(this, tip);

    m_realize_buttons = realize_btns;
}

void CdeButton::reset()
{
    repaint( false );
}

void CdeButton::drawButton( QPainter* p )
{
    p->setBrush( options()->color( KDecoration::ColorTitleBar, m_parent->isActive() ) );
    p->drawRect( 0, 0, s_buttonSize, s_buttonSize );

    QColorGroup colorGroup =
      options()->colorGroup( KDecoration::ColorTitleBar, m_parent->isActive() );

    fixColorGroup(colorGroup);

    qDrawShadePanel( p, 0, 0, s_buttonSize, s_buttonSize,
		    colorGroup, isDown() );

    switch ( m_btnType )
    {
        case (BtnMenu):
            qDrawShadePanel( p, s_o1, s_o2, s_w1, s_w2, colorGroup );
	break;
	case (BtnHelp):
	    p->setPen( colorGroup.light() );
	    p->drawLineSegments( QPointArray(16, helpLLines) );
	    p->setPen( colorGroup.dark() );
	    p->drawLineSegments( QPointArray(14, helpDLines) );
	break;
	case (BtnIconify):
	    qDrawShadePanel( p, s_o2, s_o2, s_w2, s_w2, colorGroup );
	break;
	case (BtnMax):
	    qDrawShadePanel( p, s_o1, s_o1, s_w1, s_w1, colorGroup,
		    m_parent->maximizeMode() == KDecoration::MaximizeFull );
	break;
	case (BtnClose):
	    p->setPen( colorGroup.dark() );
	    p->drawLineSegments( QPointArray(18, closeDLines) );
	    p->setPen( colorGroup.light() );
	    p->drawLineSegments( QPointArray(15, closeLLines) );
	break;
    }
}

void CdeButton::mousePressEvent(QMouseEvent *e)
{
    last_button = e->button();
    QMouseEvent me(e->type(), e->pos(),
	    e->globalPos(), (e->button()&m_realize_buttons)?LeftButton:NoButton, e->state());
    QButton::mousePressEvent(&me);
}

void CdeButton::mouseReleaseEvent(QMouseEvent * e)
{
    last_button = e->button();
    QMouseEvent me(e->type(), e->pos(),
	    e->globalPos(), (e->button()&m_realize_buttons)?LeftButton:NoButton, e->state());
    QButton::mouseReleaseEvent(&me);
}

} // CDE namespace

#include "cdeclient.moc"

// vim: sw=4
