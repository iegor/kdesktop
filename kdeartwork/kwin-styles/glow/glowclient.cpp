/***************************************************************************
                          glowclient.cpp  -  description
                             -------------------
    begin                : Thu Sep 6 2001
    copyright            : (C) 2001 by Henning Burchardt
    email                : h_burchardt@gmx.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qbitmap.h>
#include <qimage.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kpixmapeffect.h>
#include <kpixmap.h>
#include <kstandarddirs.h>
#include <qlabel.h>
#include "resources.h"
#include "glowclient.h"
#include "glowbutton.h"

namespace Glow
{

static int titleHeight;
static int SIDE_MARGIN;
static int BOTTOM_MARGIN;
static const int TITLE_MARGIN = 2;
static const int TITLE_SPACING = 1;
static int RESIZE_HANDLE_HEIGHT;

//-----------------------------------------------------------------------------

GlowClientConfig::GlowClientConfig()
{
	themeName = "default";
}

void GlowClientConfig::load(KDecorationFactory *factory)
{
	KConfig conf("kwinglowrc");
	conf.setGroup("General");

	const QColor defaultCloseButtonColor(DEFAULT_CLOSE_BUTTON_COLOR);
	const QColor defaultMaximizeButtonColor(DEFAULT_MAXIMIZE_BUTTON_COLOR);
	const QColor defaultIconifyButtonColor(DEFAULT_ICONIFY_BUTTON_COLOR);
	const QColor defaultHelpButtonColor(DEFAULT_HELP_BUTTON_COLOR);
	const QColor defaultStickyButtonColor(DEFAULT_STICKY_BUTTON_COLOR);

	stickyButtonGlowColor = conf.readColorEntry(
		"stickyButtonGlowColor", &defaultStickyButtonColor);

	helpButtonGlowColor = conf.readColorEntry(
		"helpButtonGlowColor", &defaultHelpButtonColor);

	iconifyButtonGlowColor = conf.readColorEntry(
		"iconifyButtonGlowColor", &defaultIconifyButtonColor);

	maximizeButtonGlowColor = conf.readColorEntry(
		"maximizeButtonGlowColor", &defaultMaximizeButtonColor);

	closeButtonGlowColor = conf.readColorEntry(
		"closeButtonGlowColor", &defaultCloseButtonColor);

	showResizeHandle = conf.readBoolEntry("showResizeHandle", true);
	titlebarGradientType = conf.readNumEntry("titlebarGradientType",
		KPixmapEffect::DiagonalGradient);

	themeName = conf.readEntry ("themeName", "default");

	switch(KDecoration::options()->preferredBorderSize (factory)) {
	case KDecoration::BorderLarge:
		SIDE_MARGIN = 8;
		BOTTOM_MARGIN = 6;
		RESIZE_HANDLE_HEIGHT = 10;
		break;
	case KDecoration::BorderVeryLarge:
		SIDE_MARGIN = 12;
		BOTTOM_MARGIN = 12;
		RESIZE_HANDLE_HEIGHT = 18;
		break;
	case KDecoration::BorderHuge:
		SIDE_MARGIN = 18;
		BOTTOM_MARGIN = 18;
		RESIZE_HANDLE_HEIGHT = 27;
		break;
	case KDecoration::BorderVeryHuge:
		SIDE_MARGIN = 27;
		BOTTOM_MARGIN = 27;
		RESIZE_HANDLE_HEIGHT = 40;
		break;
	case KDecoration::BorderOversized:
		SIDE_MARGIN = 40;
		BOTTOM_MARGIN = 40;
		RESIZE_HANDLE_HEIGHT = 60;
		break;
	case KDecoration::BorderNormal:
	default:
		SIDE_MARGIN = 4;
		BOTTOM_MARGIN = 2;
		RESIZE_HANDLE_HEIGHT = 4;
	}
}

//-----------------------------------------------------------------------------

GlowClientGlobals::~GlowClientGlobals()
{
	deletePixmaps();
	m_instance = 0;
}

QString GlowClientGlobals::getPixmapName(PixmapType type, bool isActive)
{
	QString s = getPixmapTypeName(static_cast<PixmapType>(type));
	s += "|";
	s += isActive ? "Active" : "NotActive";
	return s;
}

GlowClientGlobals::GlowClientGlobals()
	: KDecorationFactory()
{
	_button_factory = new GlowButtonFactory();
	readConfig();
	readTheme ();
	if (!createPixmaps())
	{
		deletePixmaps();
		delete _theme;
		config()->themeName = "default";
		readTheme();
		createPixmaps();
	}
}

void GlowClientGlobals::readConfig()
{
	_config = new GlowClientConfig();
	_config->load(this);
}

QValueList< GlowClientGlobals::BorderSize >
GlowClientGlobals::borderSizes() const
{
    // the list must be sorted
    return QValueList< BorderSize >() << BorderNormal <<
	BorderLarge << BorderVeryLarge <<  BorderHuge <<
	BorderVeryHuge << BorderOversized;
}

void GlowClientGlobals::readTheme()
{
	QString theme_config_file = KGlobal::dirs()->findResource ("data",
			QString("kwin/glow-themes/") + config()->themeName + "/" +
			config()->themeName + ".theme");
	if (theme_config_file.isNull())
	{
		config()->themeName = "default";
		return;
	}

	KConfig conf (theme_config_file);
	_theme = new GlowTheme(default_glow_theme);

	_theme->buttonSize = conf.readSizeEntry ("buttonSize",
			&_theme->buttonSize);
	_theme->stickyOnPixmap = conf.readEntry ("stickyOnPixmap",
			_theme->stickyOnPixmap);
	_theme->stickyOffPixmap = conf.readEntry ("stickyOffPixmap",
			_theme->stickyOffPixmap);
	_theme->maximizeOnPixmap = conf.readEntry ("maximizeOnPixmap",
			_theme->maximizeOnPixmap);
	_theme->maximizeOffPixmap = conf.readEntry ("maximizeOffPixmap",
			_theme->maximizeOffPixmap);
	_theme->helpPixmap = conf.readEntry ("helpPixmap",
			_theme->helpPixmap);
	_theme->closePixmap = conf.readEntry ("closePixmap",
			_theme->closePixmap);
	_theme->iconifyPixmap = conf.readEntry ("iconifyPixmap",
			_theme->iconifyPixmap);
	_theme->stickyOnGlowPixmap = conf.readEntry ("stickyOnGlowPixmap",
			_theme->stickyOnGlowPixmap);
	_theme->stickyOffGlowPixmap = conf.readEntry ("stickyOffGlowPixmap",
			_theme->stickyOffGlowPixmap);
	_theme->maximizeOnGlowPixmap = conf.readEntry ("maximizeOnGlowPixmap",
			_theme->maximizeOnGlowPixmap);
	_theme->maximizeOffGlowPixmap = conf.readEntry ("maximizeOffGlowPixmap",
			_theme->maximizeOffGlowPixmap);
	_theme->helpGlowPixmap = conf.readEntry ("helpGlowPixmap",
			_theme->helpGlowPixmap);
	_theme->closeGlowPixmap = conf.readEntry ("closeGlowPixmap",
			_theme->closeGlowPixmap);
	_theme->iconifyGlowPixmap = conf.readEntry ("iconifyGlowPixmap",
			_theme->iconifyGlowPixmap);

	titleHeight = QFontMetrics(KDecoration::options()->font(true)).height();
	if (titleHeight < SIDE_MARGIN)
		titleHeight = SIDE_MARGIN;
	if (titleHeight < _theme->buttonSize.height())
		titleHeight = _theme->buttonSize.height();
}

bool GlowClientGlobals::reset( unsigned long /*changed*/ )
{
	deletePixmaps();
	delete _config;
	readConfig();
	delete _theme;
	readTheme ();
	if (! createPixmaps())
	{
		deletePixmaps();
		delete _theme;
		_config->themeName = "default";
		readTheme();
		createPixmaps();
	}
	return true; // FRAME
}

bool GlowClientGlobals::supports( Ability ability )
{
    switch( ability )
    {
        case AbilityAnnounceButtons:
        case AbilityButtonOnAllDesktops:
        case AbilityButtonSpacer:
        case AbilityButtonHelp:
        case AbilityButtonMinimize:
        case AbilityButtonMaximize:
        case AbilityButtonClose:
            return true;
        default:
            return false;
    };
}

bool GlowClientGlobals::createPixmaps()
{
	for( int type=0; type<=Close; type++ ) {
		if (! createPixmap(static_cast<PixmapType>(type), false))
			return false;
		if (! createPixmap(static_cast<PixmapType>(type), true))
			return false;
	}

	return true;
}

void GlowClientGlobals::deletePixmaps()
{
	PixmapCache::clear();
}

const QString GlowClientGlobals::getPixmapTypeName(PixmapType type)
{
	switch(type) {
		case (StickyOn):
			return "StickyOn";
		case(StickyOff ):
			return "StickyOff";
		case(Help):
			return "Help";
		case(Iconify):
			return "Iconify";
		case(MaximizeOn):
			return "MaximizeOn";
		case(MaximizeOff):
			return "MaximizeOff";
		case(Close):
			return "Close";
		default:
			return QString::null;
	}
}

bool GlowClientGlobals::createPixmap(PixmapType type, bool isActive)
{
	QString theme_dir = KGlobal::dirs()->findResource ("data",
			QString("kwin/glow-themes/") + _config->themeName + "/");

	QColor glow_color;
	QColor color = options()->color(ColorButtonBg, isActive);

	QImage bg_image (theme_dir+_theme->backgroundPixmap);
	QImage fg_image;
	QImage glow_image;

	switch(type) {
		case (StickyOn):
		{
			fg_image = QImage (theme_dir+_theme->stickyOnPixmap);
			glow_image = QImage (theme_dir+_theme->stickyOnGlowPixmap);
			glow_color = _config->stickyButtonGlowColor;
			break;
		}
		case (StickyOff):
		{
			fg_image = QImage (theme_dir+_theme->stickyOffPixmap);
			glow_image = QImage (theme_dir+_theme->stickyOffGlowPixmap);
			glow_color = _config->stickyButtonGlowColor;
			break;
		}
		case (Help):
		{
			fg_image = QImage (theme_dir+_theme->helpPixmap);
			glow_image = QImage (theme_dir+_theme->helpGlowPixmap);
			glow_color = _config->helpButtonGlowColor;
			break;
		}
		case (Iconify):
		{
			fg_image = QImage (theme_dir+_theme->iconifyPixmap);
			glow_image = QImage (theme_dir+_theme->iconifyGlowPixmap);
			glow_color = _config->iconifyButtonGlowColor;
			break;
		}
		case (MaximizeOn):
		{
			fg_image = QImage (theme_dir+_theme->maximizeOnPixmap);
			glow_image = QImage (theme_dir+_theme->maximizeOnGlowPixmap);
			glow_color = _config->maximizeButtonGlowColor;
			break;
		}
		case (MaximizeOff):
		{
			fg_image = QImage (theme_dir+_theme->maximizeOffPixmap);
			glow_image = QImage (theme_dir+_theme->maximizeOffGlowPixmap);
			glow_color = _config->maximizeButtonGlowColor;
			break;
		}
		case (Close):
		{
			fg_image = QImage (theme_dir+_theme->closePixmap);
			glow_image = QImage (theme_dir+_theme->closeGlowPixmap);
			glow_color = _config->closeButtonGlowColor;
			break;
		}
	}

	if (bg_image.size() != _theme->buttonSize
			|| fg_image.size() != _theme->buttonSize
			|| glow_image.size() != _theme->buttonSize)
		return false;

	QPixmap * glowPm = buttonFactory()->createGlowButtonPixmap(
		bg_image, fg_image, glow_image,
		color, glow_color);
	if (glowPm->isNull())
		return false;
	PixmapCache::insert(getPixmapName(type, isActive), glowPm);
	return true;
}

GlowClientGlobals *GlowClientGlobals::m_instance = 0;

GlowClientGlobals *GlowClientGlobals::instance()
{
	if( ! m_instance )
	{
		m_instance = new GlowClientGlobals();
	}
	return m_instance;
}

//-----------------------------------------------------------------------------

GlowClient::GlowClient( KDecorationBridge* b, KDecorationFactory* f )
	: KDecoration( b, f ),
		m_stickyButton(0), m_helpButton(0), m_minimizeButton(0),
		m_maximizeButton(0), m_closeButton(0),
		m_leftButtonLayout(0), m_rightButtonLayout(0), _main_layout(0)
{
}

void GlowClient::init()
{
	createMainWidget(WResizeNoErase | WStaticContents | WRepaintNoErase);
	widget()->installEventFilter(this);

	createButtons();
	resetLayout();
}

GlowClient::~GlowClient()
{
	PixmapCache::erase(QString::number(widget()->winId()));
}

void GlowClient::resizeEvent( QResizeEvent * )
{
	doShape();
	widget()->repaint(false);
}

void GlowClient::paintEvent( QPaintEvent * )
{
	GlowClientConfig *conf = GlowClientGlobals::instance()->config();
	QRect r_this = widget()->rect();
	QRect r_title = _title_spacer->geometry();
	QColorGroup titleCg = options()->colorGroup(ColorTitleBar, isActive());
	QColorGroup titleBlendCg=options()->colorGroup(ColorTitleBlend, isActive());
	QColorGroup cg = widget()->colorGroup();
	QColor titleColor = options()->color(ColorTitleBar, isActive());
	QColor titleBlendColor = options()->color(ColorTitleBlend, isActive());
	QColor bgColor = widget()->colorGroup().background();
	QPainter p;
	QPointArray pArray, pArray2, pArray3, pArray4;

	// pixmap for title bar
	QSize tBSize(width(), r_title.height());
	QSize gradientPixmapSize (tBSize-QSize(3,3));
	if (! gradientPixmapSize.isValid())
		gradientPixmapSize = QSize(0,0);
	KPixmap gradientPixmap(gradientPixmapSize);
	if (! gradientPixmapSize.isNull())
		KPixmapEffect::gradient(gradientPixmap, titleColor, titleBlendColor,
			(KPixmapEffect::GradientType) conf->titlebarGradientType);

	QPixmap  * title_buffer = new QPixmap(tBSize);
	p.begin(title_buffer);
	if (! gradientPixmap.isNull())
		p.drawPixmap(2, 2, gradientPixmap);

	if (tBSize.width()>=3 && tBSize.height()>=3)
	{
		// draw caption
		p.setFont(options()->font(isActive()));
		p.setPen(options()->color(ColorFont, isActive()));
		p.drawText(r_title.x(), 0,
			r_title.width(), r_title.height(),
			Qt::AlignLeft | Qt::AlignVCenter | Qt::SingleLine, caption());
	
		// draw split color beneath buttons top right
		pArray4 = QPointArray(4);
		pArray4.setPoint(0, tBSize.width()-1, tBSize.height()/2-1);
		pArray4.setPoint(1, r_title.x()+r_title.width()-1+tBSize.height()/2,
			tBSize.height()/2-1);
		pArray4.setPoint(2, r_title.x()+r_title.width()-1, tBSize.height());
		pArray4.setPoint(3, tBSize.width()-1, tBSize.height());
		p.setPen(Qt::NoPen);
		p.setBrush(bgColor);
		p.drawPolygon(pArray4);
				
		// draw borders
		pArray = QPointArray(3);
		pArray.setPoint(0, tBSize.width()-1, tBSize.height()/2-1);
		pArray.setPoint(1, r_title.x()+r_title.width()-1+tBSize.height()/2,
			tBSize.height()/2-1);
		pArray.setPoint(2, r_title.x()+r_title.width()-1, tBSize.height()-1);
		p.setPen(titleCg.mid());
		p.drawPolyline(pArray);
		p.drawLine (0, tBSize.height()-1, r_title.x()+r_title.width()-1, tBSize.height()-1);

		pArray2 = QPointArray(3);
		pArray2.setPoint(0, 1, tBSize.height()-2);
		pArray2.setPoint(1, 1, 1);
		pArray2.setPoint(2, tBSize.width()-2, 1);
		p.setPen(titleCg.light());
		p.drawPolyline(pArray2);
	}

	pArray3 = QPointArray(4);
	pArray3.setPoint(0, 0, tBSize.height()-1);
	pArray3.setPoint(1, 0, 0);
	pArray3.setPoint(2, tBSize.width()-1, 0);
	pArray3.setPoint(3, tBSize.width()-1, tBSize.height()-1);
	p.setPen(Qt::black);
	p.drawPolyline(pArray3);
	p.end();

	// insert title buffer in cache; before that, remove old buffer
	PixmapCache::erase(QString::number(widget()->winId()));
	PixmapCache::insert(QString::number(widget()->winId()), title_buffer);

	bitBlt(widget(), 0, 0, title_buffer);
	for (unsigned int i=0; i<m_buttonList.size(); ++i)
		m_buttonList[i]->repaint(false);

	p.begin(widget());
	p.setPen(Qt::black);
	//-----
	// draw borders
	p.drawLine(0,tBSize.height(),0,r_this.height()-1);
	p.drawLine(0,r_this.height()-1,r_this.width()-1,r_this.height()-1);
	p.drawLine(r_this.width()-1,r_this.height()-1,
		r_this.width()-1,tBSize.height()/2);
	//-----
	// fill content widget
	p.fillRect(1, tBSize.height(),
		r_this.width()-2, r_this.height()-tBSize.height()-1,
		options()->colorGroup(ColorFrame, isActive()).background());
	p.end();

	// paint resize handle if necessary
	if(conf->showResizeHandle && isResizable() && ! isShade()
			&& width()>=2 && height() >= RESIZE_HANDLE_HEIGHT)
	{
		p.begin(widget());
		p.setPen (Qt::black);
		p.drawLine (1, height() - RESIZE_HANDLE_HEIGHT,
			width()-2, height() - RESIZE_HANDLE_HEIGHT);
		p.end();
	}
}

void GlowClient::showEvent( QShowEvent * )
{
	doShape();
	widget()->repaint(false);
}

void GlowClient::mouseDoubleClickEvent( QMouseEvent *e )
{
	if(e->button() == LeftButton && _title_spacer->geometry().contains(e->pos()))
		titlebarDblClickOperation();
}

void GlowClient::wheelEvent( QWheelEvent *e )
{
    if (isSetShade() || QRect( 0, 0, width(), titleHeight ).contains(e->pos()))
        titlebarMouseWheelOperation( e->delta());
}

void GlowClient::activeChange()
{
	updateButtonPixmaps();
	widget()->repaint(false);
}

void GlowClient::iconChange()
{
	// we have no (t yet an) icon button, so do nothing
}

void GlowClient::shadeChange()
{
}

void GlowClient::captionChange()
{
    widget()->update(_title_spacer->geometry());
}

QSize GlowClient::minimumSize() const
{
    return widget()->minimumSize();
}

void GlowClient::resize( const QSize& s )
{
    widget()->resize( s );
}

void GlowClient::borders( int& left, int& right, int& top, int& bottom ) const
{
	left = right = SIDE_MARGIN;
	top = titleHeight + TITLE_MARGIN + 1;
	if (GlowClientGlobals::instance()->config()->showResizeHandle
		&& isResizable())
		bottom = RESIZE_HANDLE_HEIGHT;
	else
		bottom = BOTTOM_MARGIN;
}

void GlowClient::desktopChange()
{
	if (isOnAllDesktops()) {
		m_stickyButton->setPixmapName(
			GlowClientGlobals::instance()->getPixmapName(
				GlowClientGlobals::StickyOn, isActive()));
		m_stickyButton->setTipText(i18n("Not on all desktops"));
	} else {
		m_stickyButton->setPixmapName(
			GlowClientGlobals::instance()->getPixmapName(
				GlowClientGlobals::StickyOff, isActive()));
		m_stickyButton->setTipText(i18n("On all desktops"));
	}
}

void GlowClient::maximizeChange()
{
    if (maximizeMode() == MaximizeFull) {
		m_maximizeButton->setPixmapName(
			GlowClientGlobals::instance()->getPixmapName(
				GlowClientGlobals::MaximizeOn, isActive()));
		m_maximizeButton->setTipText(i18n("Restore"));
	} else {
		m_maximizeButton->setPixmapName(
			GlowClientGlobals::instance()->getPixmapName(
				GlowClientGlobals::MaximizeOff, isActive()));
		m_maximizeButton->setTipText(i18n("Maximize"));
	}
}

KDecoration::Position GlowClient::mousePosition(const QPoint &pos) const
{
	Position m = PositionCenter;

	int bottomSize
		= (GlowClientGlobals::instance()->config()->showResizeHandle)
		? RESIZE_HANDLE_HEIGHT : BOTTOM_MARGIN;

	const int range = 14 + 3*SIDE_MARGIN/2;

	if ( ( pos.x() > SIDE_MARGIN && pos.x() < width() - SIDE_MARGIN )
		&& ( pos.y() > 4 && pos.y() < height() - bottomSize ) )
		m = PositionCenter;
	else if ( pos.y() <= range && pos.x() <= range)
		m = PositionTopLeft;
	else if ( pos.y() >= height()-range && pos.x() >= width()-range)
		m = PositionBottomRight;
	else if ( pos.y() >= height()-range && pos.x() <= range)
		m = PositionBottomLeft;
	else if ( pos.y() <= range && pos.x() >= width()-range)
		m = PositionTopRight;
	else if ( pos.y() <= 4 )
		m = PositionTop;
	else if ( pos.y() >= height()-bottomSize )
		m = PositionBottom;
	else if ( pos.x() <= SIDE_MARGIN )
		m = PositionLeft;
	else if ( pos.x() >= width()-SIDE_MARGIN )
		m = PositionRight;
	else
		m = PositionCenter;

	return m;
}

void GlowClient::createButtons()
{
	GlowClientGlobals *globals = GlowClientGlobals::instance();
	GlowButtonFactory *factory = globals->buttonFactory();
	QSize size = globals->theme()->buttonSize;

	m_stickyButton = factory->createGlowButton(widget(),
					"StickyButton", isOnAllDesktops()?i18n("Not on all desktops"):i18n("On all desktops"), LeftButton|RightButton);
	m_stickyButton->setFixedSize(size);
	connect(m_stickyButton, SIGNAL(clicked()), this, SLOT(toggleOnAllDesktops()));
	m_buttonList.insert(m_buttonList.end(), m_stickyButton);

	m_helpButton = factory->createGlowButton(widget(),
					"HelpButton", i18n("Help"));
	m_helpButton->setFixedSize(size);
	connect(m_helpButton, SIGNAL(clicked()), this, SLOT(showContextHelp()));
	m_buttonList.insert(m_buttonList.end(), m_helpButton);

	m_minimizeButton = factory->createGlowButton(widget(),
					"IconifyButton", i18n("Minimize"));
	m_minimizeButton->setFixedSize(size);
	connect(m_minimizeButton, SIGNAL(clicked()), this, SLOT(minimize()));
	m_buttonList.insert(m_buttonList.end(), m_minimizeButton);

	m_maximizeButton=factory->createGlowButton(widget(),
					"MaximizeButton", i18n("Maximize"), LeftButton|MidButton|RightButton);
	m_maximizeButton->setFixedSize(size);
	connect(m_maximizeButton, SIGNAL(clicked()), this, SLOT(slotMaximize()));
	m_buttonList.insert(m_buttonList.end(), m_maximizeButton);

	m_closeButton = factory->createGlowButton(widget(),
					"CloseButton", i18n("Close"));
	m_closeButton->setFixedSize(size);
	connect(m_closeButton, SIGNAL(clicked()), this, SLOT(closeWindow()));
	m_buttonList.insert(m_buttonList.end(), m_closeButton);
}

void GlowClient::resetLayout()
{
	_main_layout = new QVBoxLayout(widget(), 0, 0);
	_main_layout->setResizeMode (QLayout::FreeResize);

	// update button positions and colors
	updateButtonPositions();
	updateButtonPixmaps();

	QBoxLayout * topLayout = new QBoxLayout(_main_layout,
			QBoxLayout::LeftToRight, 0, 0);
	topLayout->setMargin(0);
	topLayout->setSpacing(TITLE_SPACING);
	topLayout->addSpacing(SIDE_MARGIN);
	QVBoxLayout *outerLeftLayout = new QVBoxLayout(topLayout);
	outerLeftLayout->addSpacing(TITLE_MARGIN);
	outerLeftLayout->addItem(m_leftButtonLayout);
	outerLeftLayout->addSpacing(1);
	topLayout->addSpacing(SIDE_MARGIN);

	_title_spacer = new QSpacerItem(0, titleHeight + TITLE_MARGIN + 1,
		QSizePolicy::Expanding, QSizePolicy::Fixed);
	topLayout->addItem(_title_spacer);

	topLayout->addSpacing(SIDE_MARGIN);
	QVBoxLayout *outerRightLayout = new QVBoxLayout(topLayout);
	outerRightLayout->addSpacing(TITLE_MARGIN);
	outerRightLayout->addItem(m_rightButtonLayout);
	outerRightLayout->addSpacing(1);
	topLayout->addSpacing(SIDE_MARGIN);

	QBoxLayout *midLayout = new QBoxLayout(
			_main_layout, QBoxLayout::LeftToRight, 0, 0);
	midLayout->addSpacing(SIDE_MARGIN);
	if(isPreview())
		midLayout->addWidget(
				new QLabel( i18n( "<b><center>Glow preview</center></b>" ), widget()));
	else
		midLayout->addItem( new QSpacerItem( 0, 0 ));
	midLayout->addSpacing(SIDE_MARGIN);

	if(GlowClientGlobals::instance()->config()->showResizeHandle
		&& isResizable() ) {
		_bottom_spacer = new QSpacerItem(SIDE_MARGIN*2,
			RESIZE_HANDLE_HEIGHT, QSizePolicy::Expanding, QSizePolicy::Minimum);
	} else {
		_bottom_spacer = new QSpacerItem(SIDE_MARGIN*2,
			BOTTOM_MARGIN, QSizePolicy::Expanding, QSizePolicy::Minimum);
	}
	_main_layout->addItem (_bottom_spacer);
	_main_layout->setStretchFactor(topLayout, 0);
	_main_layout->setStretchFactor(midLayout, 1);
}

void GlowClient::updateButtonPositions()
{
	QString buttons = options()->titleButtonsLeft() + "|"
		+ options()->titleButtonsRight();
	bool leftButtons=true;

	// hide all buttons
	for( unsigned int i=0; i<m_buttonList.size(); i++ )
		m_buttonList[i]->hide();

	m_leftButtonList.clear();
	m_rightButtonList.clear();

	// reset left and right button layout
	if(m_leftButtonLayout)
		delete m_leftButtonLayout;
	m_leftButtonLayout = new QBoxLayout(0, QBoxLayout::LeftToRight, 0, 0, 0);
	m_leftButtonLayout->setMargin(0);
	m_leftButtonLayout->setSpacing(TITLE_SPACING);
	if(m_rightButtonLayout)
		delete m_rightButtonLayout;
	m_rightButtonLayout = new QBoxLayout(0, QBoxLayout::LeftToRight, 0, 0, 0);
	m_rightButtonLayout->setMargin(0);
	m_rightButtonLayout->setSpacing(TITLE_SPACING);

	for( unsigned int i=0; i<buttons.length(); i++ )
	{
		char c = buttons[i].latin1();
		GlowButton *button = 0;
		if( c=='S' ) // sticky
			button = m_stickyButton;
		else if( c=='H' && providesContextHelp() ) // help
			button = m_helpButton;
		else if( c=='I' && isMinimizable() ) // iconify
			button = m_minimizeButton;
		else if( c=='A' && isMaximizable() ) // maximize
			button = m_maximizeButton;
		else if( c=='X' && isCloseable() ) // close
			button= m_closeButton;
		else if( c=='_' ) // spacer item
		{
			if(leftButtons)
				m_leftButtonLayout->addSpacing(4);
			else
				m_rightButtonLayout->addSpacing(4);
		}
		else if( c=='|' )
			leftButtons = false;

		if (button) {
			button->show(); // show visible buttons
			if (leftButtons) {
				m_leftButtonList.insert(m_leftButtonList.end(), button);
				m_leftButtonLayout->addWidget(button);
			} else {
				m_rightButtonList.insert(m_rightButtonList.end(), button);
				m_rightButtonLayout->addWidget(button);
			}
		}
	}
}

void GlowClient::updateButtonPixmaps()
{
	GlowClientGlobals *globals = GlowClientGlobals::instance();

	if ( isOnAllDesktops() ) {
		m_stickyButton->setPixmapName(globals->getPixmapName(
			GlowClientGlobals::StickyOn, isActive()));
	} else {
		m_stickyButton->setPixmapName(globals->getPixmapName(
			GlowClientGlobals::StickyOff, isActive()));
	}
	m_helpButton->setPixmapName(globals->getPixmapName(
		GlowClientGlobals::Help, isActive()));

	m_minimizeButton->setPixmapName(globals->getPixmapName(
		GlowClientGlobals::Iconify, isActive()));

	if ( maximizeMode() == MaximizeFull ) {
		m_maximizeButton->setPixmapName(globals->getPixmapName(
			GlowClientGlobals::MaximizeOn, isActive()));
	} else {
		m_maximizeButton->setPixmapName(globals->getPixmapName(
			GlowClientGlobals::MaximizeOff, isActive()));
	}
	m_closeButton->setPixmapName(globals->getPixmapName(
		GlowClientGlobals::Close, isActive()));
}

void GlowClient::doShape()
{
	QRegion mask(widget()->rect());
	// edges

	mask -= QRegion(width()-1,0,1,1);
	mask -= QRegion(0,height()-1,1,1);
	mask -= QRegion(width()-1,height()-1,1,1);
	setMask(mask);
}

bool GlowClient::isLeft(GlowButton *button)
{
	for( unsigned int i=0; i<m_leftButtonList.size(); i++ )
		if( m_leftButtonList[i] == button )
			return true;
	return false;
}

bool GlowClient::isRight(GlowButton *button)
{
	for( unsigned int i=0; i<m_rightButtonList.size(); i++ )
		if( m_rightButtonList[i] == button )
			return true;
	return false;
}

void GlowClient::slotMaximize()
{
	maximize(m_maximizeButton->lastButton());
}

bool GlowClient::eventFilter( QObject* o, QEvent* e )
{
	if( o != widget())
		return false;
	switch( e->type())
	{
	case QEvent::Resize:
	    resizeEvent( static_cast< QResizeEvent* >( e ));
	    return true;
	case QEvent::Paint:
	    paintEvent( static_cast< QPaintEvent* >( e ));
	    return true;
	case QEvent::MouseButtonDblClick:
	    mouseDoubleClickEvent( static_cast< QMouseEvent* >( e ));
	    return true;
	case QEvent::MouseButtonPress:
	    processMousePressEvent( static_cast< QMouseEvent* >( e ));
	    return true;
	case QEvent::Show:
	    showEvent( static_cast< QShowEvent* >( e ));
	    return true;
	case QEvent::Wheel:
	    wheelEvent( static_cast< QWheelEvent* >( e ));
	    return true;
	default:
	    break;
	}
    return false;
}

KDecoration* GlowClientGlobals::createDecoration( KDecorationBridge* bridge )
{
	return new GlowClient( bridge, this );
}

} // end of namespace

extern "C"
KDE_EXPORT KDecorationFactory* create_factory()
{
	return Glow::GlowClientGlobals::instance();
}

#include "glowclient.moc"

