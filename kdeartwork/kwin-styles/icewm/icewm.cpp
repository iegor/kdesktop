/*
  $Id: icewm.cpp 489039 2005-12-16 22:05:28Z giessl $

  Gallium-IceWM themeable KWin client

  Copyright 2001
	Karol Szwed <gallium@kde.org>
	http://gallium.n3.net/

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

  -----------------------------------------------------------------------------
  This client loads most icewm 1.0.X pixmap themes, without taking into account
  specific font settings for clients, or coloured mouse cursors. Titlebar
  fonts can be changed via the kde control center. Bi-colour mouse cursors
  may be added in future if requested by users, as well as theme font support.
  Any styles using inbuilt icewm titlebar drawing without using pixmaps (e.g.
  Warp4, win95 etc.) are not fully supported, and may cause drawing errors,
  as these themes use in-built icewm drawing mechanisms.

  When a pixmap theme is not present (or a corrupt one is present) then very
  plain title decorations are painted instead, so that users don't see
  non-painted window areas where possible ;)

  At a later date, frame shaping may be added if really requested, and an
  update to support the latest icewm 1.1.X theme format may be made.

*/

#include <kconfig.h>
#include <kstandarddirs.h>
#include <kglobal.h>
#include <klocale.h>
#include <kdrawutil.h>
#include <qapplication.h>
#include <qlabel.h>
#include <qdrawutil.h>
#include <qdatetime.h>
#include <qbitmap.h>
#include <qcursor.h>
#include <qstring.h>
#include <qtooltip.h>
#include <qregexp.h>
#include "icewm.h"

namespace IceWM {

////////////////////////////////////////////////////////////////////////////////////////////
// Here's the global pixmap stuff - as memory efficient as it can be :)
////////////////////////////////////////////////////////////////////////////////////////////

// IceWM frame pixmaps
QPixmap* frameTL[] = {NULL, NULL};
QPixmap* frameT [] = {NULL, NULL};
QPixmap* frameTR[] = {NULL, NULL};
QPixmap* frameL [] = {NULL, NULL};
QPixmap* frameR [] = {NULL, NULL};
QPixmap* frameBL[] = {NULL, NULL};
QPixmap* frameB [] = {NULL, NULL};
QPixmap* frameBR[] = {NULL, NULL};

// Button pixmaps
QPixmap* closePix[]      = {NULL, NULL};
QPixmap* depthPix[]      = {NULL, NULL};
QPixmap* maximizePix[]   = {NULL, NULL};
QPixmap* minimizePix[]   = {NULL, NULL};
QPixmap* restorePix[]    = {NULL, NULL};
QPixmap* hidePix[]       = {NULL, NULL};
QPixmap* rollupPix[]     = {NULL, NULL};
QPixmap* rolldownPix[]   = {NULL, NULL};
QPixmap* menuButtonPix[] = {NULL, NULL};

// Titlebar pixmaps
QPixmap* titleJ[] = {NULL, NULL};
QPixmap* titleL[] = {NULL, NULL};
QPixmap* titleS[] = {NULL, NULL};
QPixmap* titleP[] = {NULL, NULL};
QPixmap* titleT[] = {NULL, NULL};
QPixmap* titleM[] = {NULL, NULL};
QPixmap* titleB[] = {NULL, NULL};
QPixmap* titleR[] = {NULL, NULL};
QPixmap* titleQ[] = {NULL, NULL};

ThemeHandler* clientHandler;

QString* titleButtonsLeft;
QString* titleButtonsRight;

QColor* colorActiveBorder;
QColor* colorInActiveBorder;
QColor* colorActiveButton;
QColor* colorInActiveButton;
QColor* colorActiveTitleBarText;
QColor* colorInActiveTitleBarText;
QColor* colorActiveTitleBar;
QColor* colorInActiveTitleBar;
QColor* colorActiveTitleTextShadow;
QColor* colorInActiveTitleTextShadow;

int  cornerSizeX;
int  cornerSizeY;
int  titleBarHeight;
int  borderSizeX;
int  borderSizeY;

bool validframe  			= false;
bool useActiveShadow 		= false;
bool useInActiveShadow 		= false;

// KControl Settings - Read from kwinicewmrc config file or icewm theme
bool themeTitleTextColors 	= true;		// Allow theme to set colors.
										// kcontrol will have no effect

bool titleBarOnTop 		  	= true;		// Titlebars can be below windows too :)
bool showMenuButtonIcon   	= false;	// Draw a mini icon over the menu pixmap.
bool customButtonPositions 	= false;	// Let the theme dictate the btn pos.
bool titleBarCentered 	  	= true;

enum styles {OTHER, WARP3, WARP4, MOTIF, WIN95, NICE} themeLook;

////////////////////////////////////////////////////////////////////////////////////////////
// General utility functions
////////////////////////////////////////////////////////////////////////////////////////////

// Returns true if both active and inactive pixmaps are valid, and not null
bool validPixmaps( QPixmap* p[] )
{
	return ( p[Active]   && ( !p[Active]->isNull()   ) &&
			 p[InActive] && ( !p[InActive]->isNull() ) );
}


////////////////////////////////////////////////////////////////////////////////////////////
// ThemeHandler class
//
// This class allows us to free dynamic memory upon being reset, or unloaded
// from kwin, so we don't leak big images everywhere, and handles the theme
// initialisation / destruction in general.
////////////////////////////////////////////////////////////////////////////////////////////

ThemeHandler::ThemeHandler()
{
	initialized = false;

	// Prevent having globals objects (use pointers to objects)
	titleButtonsLeft 	= new QString();
	titleButtonsRight 	= new QString();

	colorActiveBorder 			= new QColor();
	colorInActiveBorder 		= new QColor();
	colorActiveButton 	        = new QColor();
	colorInActiveButton 		= new QColor();
	colorActiveTitleBarText 	= new QColor();
	colorInActiveTitleBarText	= new QColor();
	colorActiveTitleBar 		= new QColor();
	colorInActiveTitleBar 		= new QColor();
	colorActiveTitleTextShadow	= new QColor();
	colorInActiveTitleTextShadow = new QColor();

	// Initialize
	readConfig();
	initTheme();
	validframe = isFrameValid();
	initialized = true;
}


ThemeHandler::~ThemeHandler()
{
	if (initialized)
		freePixmaps();

	delete colorInActiveTitleTextShadow;
	delete colorActiveTitleTextShadow;
	delete colorInActiveBorder;
	delete colorActiveTitleBarText;
	delete colorInActiveTitleBarText;
	delete colorActiveTitleBar;
	delete colorInActiveTitleBar;
	delete colorActiveBorder;
	delete colorActiveButton;
	delete colorInActiveButton;

	delete titleButtonsRight;
	delete titleButtonsLeft;
}


KDecoration* ThemeHandler::createDecoration( KDecorationBridge* bridge )
{
	return new IceWMClient( bridge, this );
}


// Converts KDE style button strings to icewm style button strings
void ThemeHandler::convertButtons( QString& s )
{
	s.replace( QRegExp("_"), "");	// Spacer	(ignored)
	s.replace( QRegExp("H"), "");	// Help		(ignored)
	s.replace( QRegExp("M"), "s");	// Sysmenu
	s.replace( QRegExp("S"), "d");	// Sticky/OnAllDesktops
	s.replace( QRegExp("I"), "i");	// Minimize
	s.replace( QRegExp("A"), "m");	// Maximize
	s.replace( QRegExp("X"), "x");	// Close
}


// Reverses all characters in a QString
QString ThemeHandler::reverseString( QString s )
{
	if (s.length() <= 1)
		return s;

	QString tmpStr;
	for(int i = s.length()-1; i >= 0; i--)
	{
		tmpStr += s[(unsigned int)i];
	}

	return tmpStr;
}


// This function reads the kwinicewmrc config file
void ThemeHandler::readConfig()
{
	KConfig conf("kwinicewmrc");
	conf.setGroup("General");
	themeName = conf.readEntry("CurrentTheme");
	themeTitleTextColors = conf.readBoolEntry("ThemeTitleTextColors", true);
	showMenuButtonIcon = conf.readBoolEntry("ShowMenuButtonIcon", false);
	titleBarOnTop = conf.readBoolEntry("TitleBarOnTop", true);

	customButtonPositions = KDecoration::options()->customButtonPositions();
	if (customButtonPositions)
	{
		*titleButtonsLeft  = KDecoration::options()->titleButtonsLeft();
		*titleButtonsRight = KDecoration::options()->titleButtonsRight();

		// Convert KDE to icewm style buttons
		convertButtons( *titleButtonsLeft );
		convertButtons( *titleButtonsRight );
	}

	// Provide a default theme alias
	if (themeName == "default")
		themeName = "";
}


// This creates the dynamic pixmaps upon loading the style
// into the pixmap buffers above, and configures the dimensioning stuff.
void ThemeHandler::initTheme()
{
	// Add a slash if required
	if ( !themeName.isEmpty() )
		themeName += "/";

	// We use kconfig to read icewm config files...
	// this is easy since icewm uses key=value pairs!
	KConfig config( locate("data", QString("kwin/icewm-themes/") +
					themeName + QString("default.theme")) );

	// Load specifics, or use IceWM defaults instead.
	borderSizeX = config.readNumEntry("BorderSizeX", 6);
	borderSizeY = config.readNumEntry("BorderSizeY", 6);
	cornerSizeX = config.readNumEntry("CornerSizeX", 24);
	cornerSizeY = config.readNumEntry("CornerSizeY", 24);
	titleBarCentered = (bool) config.readNumEntry("TitleBarCentered", 0);

	// Check if readConfig() hasn't overridden this value...
	if (!showMenuButtonIcon)
		showMenuButtonIcon = (bool) config.readNumEntry("ShowMenuButtonIcon", 0);
	titleBarHeight = config.readNumEntry("TitleBarHeight", 20);

	if (!customButtonPositions)
	{
		// Read in the button configuration, stripping any quotes
		// Ignore on all desktops 'd' on the left buttons
		// (some themes look bad with it on by default)
		*titleButtonsLeft = config.readEntry("TitleButtonsLeft", "s");
		*titleButtonsLeft = titleButtonsLeft->replace( QRegExp(QString("\"")), "");
		*titleButtonsRight = config.readEntry("TitleButtonsRight", "xmir");
		*titleButtonsRight = titleButtonsRight->replace( QRegExp(QString("\"")), "");

		// I have no idea why the right side buttons in icewm are reversed
		*titleButtonsRight = reverseString( *titleButtonsRight );
	}

	// Read the default border and text colours from the config file
	// And use IceWM defaults if not found
	QString s;

	s = config.readEntry("Look", "other");
	if (s=="motif") themeLook = MOTIF;
	else if (s=="warp3") themeLook = WARP3;
	else if (s=="warp4") themeLook = WARP4;
	else if (s=="win95") themeLook = WIN95;
	else if (s=="nice") themeLook = NICE;
	else themeLook = OTHER;

	s = config.readEntry("ColorActiveBorder", "#C0C0C0");
	*colorActiveBorder = decodeColor( s );
	s = config.readEntry("ColorNormalBorder", "#C0C0C0");
	*colorInActiveBorder = decodeColor( s );
	s = config.readEntry("ColorActiveButton", "#C0C0C0");
	*colorActiveButton = decodeColor( s );
	s = config.readEntry("ColorNormalButton", "#C0C0C0");
	*colorInActiveButton = decodeColor( s );

	// Use these as a last resort
	s = config.readEntry("ColorActiveTitleBar", "#0000A0");
	*colorActiveTitleBar = decodeColor( s );
	s = config.readEntry("ColorNormalTitleBar", "#808080");
	*colorInActiveTitleBar = decodeColor( s );

	// Read titlebar text colours
	s = config.readEntry("ColorActiveTitleBarText", "#FFFFFF");
	*colorActiveTitleBarText = decodeColor( s );
	s = config.readEntry("ColorNormalTitleBarText", "#000000");
	*colorInActiveTitleBarText = decodeColor( s );

	// Use title text shadows only with theme title text colors
	if ( themeTitleTextColors )
	{
		s = config.readEntry("ColorActiveTitleBarShadow");
		if (!s.isEmpty())
		{
			*colorActiveTitleTextShadow = decodeColor( s );
			useActiveShadow = true;
		} else
			useActiveShadow = false;

		s = config.readEntry("ColorNormalTitleBarShadow");
		if (!s.isEmpty())
		{
			*colorInActiveTitleTextShadow = decodeColor( s );
			useInActiveShadow = true;
		} else
			useInActiveShadow = false;
	} else
		{
			useActiveShadow = false;
			useInActiveShadow = false;
		}

	// Stretch pixmaps for speed, where required
	setPixmap( titleJ, "title", "J.xpm" );
	setPixmap( titleL, "title", "L.xpm" );
	setPixmap( titleS, "title", "S.xpm", true );

	setPixmap( titleP, "title", "P.xpm" );
	setPixmap( titleT, "title", "T.xpm", true );
	setPixmap( titleM, "title", "M.xpm" );
	setPixmap( titleB, "title", "B.xpm", true );
	setPixmap( titleR, "title", "R.xpm" );
	setPixmap( titleQ, "title", "Q.xpm" );

	setPixmapButton( closePix,     "close",     ".xpm" );
	setPixmapButton( depthPix,     "depth",     ".xpm" );
	setPixmapButton( maximizePix,  "maximize",  ".xpm" );
	setPixmapButton( minimizePix,  "minimize",  ".xpm" );
	setPixmapButton( restorePix,   "restore",   ".xpm" );
	setPixmapButton( hidePix,      "hide",      ".xpm" );
	setPixmapButton( rollupPix,    "rollup",    ".xpm" );
	setPixmapButton( rolldownPix,  "rolldown",  ".xpm" );
	setPixmapButton( menuButtonPix,"menuButton",".xpm" );

	// Top
	setPixmap( frameTL, "frame", "TL.xpm" );
	setPixmap( frameT,  "frame", "T.xpm", true );
	setPixmap( frameTR, "frame", "TR.xpm" );

	// Sides
	setPixmap( frameL, "frame", "L.xpm", true, Vertical );
	setPixmap( frameR, "frame", "R.xpm", true, Vertical );

	// Bottom
	setPixmap( frameBL, "frame", "BL.xpm" );
	setPixmap( frameB,  "frame", "B.xpm", true );
	setPixmap( frameBR, "frame", "BR.xpm" );

    // Make sure border sizes are at least reasonable...
	if (borderSizeX < 0)
		borderSizeX = 0;
	if (borderSizeY < 0)
		borderSizeY = 0;
	// ...and titleBarHeight as well
	if (titleBarHeight < 0)
		titleBarHeight = 0;

	// This is a work-around for some themes
	if (!titleT[Active])
		titleT[Active] = duplicateValidPixmap( Active );

	if (!titleB[Active])
		titleB[Active] = duplicateValidPixmap( Active );


	if (titleL[Active] && !titleL[InActive])
		titleL[InActive] = duplicateValidPixmap( InActive, titleL[Active]->width() );

	if (titleS[Active] && !titleS[InActive])
		titleS[InActive] = duplicateValidPixmap( InActive, titleS[Active]->width() );

	if (titleP[Active] && !titleP[InActive])
		titleP[InActive] = duplicateValidPixmap( InActive, titleP[Active]->width() );

	if (titleT[Active] && !titleT[InActive])
		titleT[InActive] = duplicateValidPixmap( InActive, titleT[Active]->width() );

	if (titleM[Active] && !titleM[InActive])
		titleM[InActive] = duplicateValidPixmap( InActive, titleM[Active]->width() );

	if (titleB[Active] && !titleB[InActive])
		titleB[InActive] = duplicateValidPixmap( InActive, titleB[Active]->width() );

	if (titleR[Active] && !titleR[InActive])
		titleR[InActive] = duplicateValidPixmap( InActive, titleR[Active]->width() );
}


QPixmap* ThemeHandler::duplicateValidPixmap( bool act, int size )
{
	QPixmap* p1 = NULL;
	// Use the stretch or title pixmaps instead
	if ( titleS[act] )
		p1 = new QPixmap( *titleS[act] );
	else if ( titleB[act] )
		p1 = new QPixmap( *titleB[act] );
	else if ( titleT[act] )
		p1 = new QPixmap( *titleT[act] );

	// Stretch if required
	if ( (size != -1) && p1 && (!p1->isNull()) )
		p1 = stretchPixmap( p1, true, size );

	return p1;
}


// Frees all memory used by pixmaps.
void ThemeHandler::freePixmaps()
{
	freePixmapGroup( frameTL );
	freePixmapGroup( frameT );
	freePixmapGroup( frameTR );
	freePixmapGroup( frameL );
	freePixmapGroup( frameR );
	freePixmapGroup( frameBL );
	freePixmapGroup( frameB );
	freePixmapGroup( frameBR );

	freePixmapGroup( closePix );
	freePixmapGroup( depthPix );
	freePixmapGroup( maximizePix );
	freePixmapGroup( minimizePix );
	freePixmapGroup( restorePix );
	freePixmapGroup( hidePix );
	freePixmapGroup( rollupPix );
	freePixmapGroup( rolldownPix );
	freePixmapGroup( menuButtonPix );

	freePixmapGroup( titleJ );
	freePixmapGroup( titleL );
	freePixmapGroup( titleS );
	freePixmapGroup( titleP );
	freePixmapGroup( titleT );
	freePixmapGroup( titleM );
	freePixmapGroup( titleB );
	freePixmapGroup( titleR );
	freePixmapGroup( titleQ );
}


// Frees a dynamic pixmap group from the heap.
void ThemeHandler::freePixmapGroup( QPixmap* p[] )
{
	if (p)
	{
		if (p[Active])   delete p[Active];
		if (p[InActive]) delete p[InActive];
		p[Active] = NULL;
		p[InActive] = NULL;
	} else
		qWarning("kwin-icewm: freePixmapGroup - invalid QPixmap** 'p'\n");
}


// Converts icewm colors #C0C0C0 or rgb:C0/C0/C0 to QColors
QColor ThemeHandler::decodeColor( QString& s )
{
	// Make rgb:C0/C0/C0, or #C0/C0/C0  -> C0C0C0
	s.replace( QRegExp("r"), "");
	s.replace( QRegExp("g"), "");
	s.replace( QRegExp("b"), "");
	s.replace( QRegExp("#"), "");
	s.replace( QRegExp("/"), "");
	s.replace( QRegExp(":"), "");
	s.replace( QRegExp("\\"), "");
	s.replace( QRegExp("\""), "");

	// Wierd error - return grey
	if (s.length() != 6)
		return QColor( 0xC0, 0xC0, 0xC0 );

	// Qt makes this conversion very easy
	return QColor( QString("#") + s );
}


// Stretches tiny pixmaps vertically or horizontally, taking into account
// repetition in patterns, so as not to make them mismatched
QPixmap* ThemeHandler::stretchPixmap( QPixmap* src, bool stretchHoriz, int stretchSize )
{
	if (!src) return NULL;
	if (src->isNull()) return NULL;

	int s_inc, size;

	// If its the right size already, just return
	if (stretchSize == -1)
	{
		if (stretchHoriz)
			s_inc = src->width();
		else
			s_inc = src->height();

		size = s_inc;
		if (size >= 100)
			return src;

		// Stretch an appropriate amount - taking care of pattern repetition
		while( size < 100 )
			size += s_inc;
	} else
		size = stretchSize;

	QPixmap* p = new QPixmap();
	if ( stretchHoriz )
		p->resize( size, src->height() );
	else
		p->resize( src->width(), size );

	QPainter pnt( p );
	if ( stretchHoriz )
		pnt.drawTiledPixmap( 0, 0, size, src->height(), *src);
	else
		pnt.drawTiledPixmap( 0, 0, src->width(), size, *src);
	pnt.end();

	delete src;
	return p;
}

static void draw3DRect(QPainter &pnt, QColor &col, int x, int y, int w, int h, bool up) {
	QColor light = col.light(135);
	QColor dark = col.dark(140);
	pnt.setPen(up ? light : dark);
	pnt.drawLine(x, y, x+w, y);
	pnt.drawLine(x, y, x, y+h);
	pnt.setPen(up ? dark : light);
	pnt.drawLine(x, y+h, x+w, y+h);
	pnt.drawLine(x+w, y, x+w, y+h);
	pnt.setPen(col);
	pnt.drawPoint(x+w, y);
	pnt.drawPoint(x, y+h);
}

void ThemeHandler::setPixmapButton( QPixmap* p[], QString s1, QString s2)
{
	if ( p[Active] )
		qWarning("kwin-icewm: setPixmap - should be null (1)\n");
	if ( p[InActive] )
		qWarning("kwin-icewm: setPixmap - should be null (2)\n");

	QString str = locate("appdata", QString("icewm-themes/")
				+ themeName + s1 + "A" + s2);
	if (str.isEmpty())
		str = locate("appdata", QString("icewm-themes/")
				+ themeName + s1 + s2);

        QPixmap *qp = new QPixmap(str);
	QColor cActive = themeLook == WIN95 ? *colorActiveTitleBar : *colorActiveButton;
	QColor cInActive = themeLook == WIN95 ? *colorInActiveTitleBar : *colorInActiveButton;

	if (!qp->isNull() && themeLook > 0) {
		int w = qp->width();
		if (themeLook > 0 && titleBarHeight > w) w = titleBarHeight;
		p[Active] = new QPixmap(w, 2*titleBarHeight );
		p[Active] -> fill(cActive);

		QPainter pnt( p[Active] );

		int offX = (w - qp->width())/2;
		int offY = (titleBarHeight - qp->height())/2;
		if (offY < 0) offY = 0;

		if (themeLook == WIN95) {
			draw3DRect(pnt, *colorActiveButton, offX-1, offY-1,
				   qp->width()+1, qp->height()+1, true);
			draw3DRect(pnt, *colorActiveButton, offX-1, offY-1 + titleBarHeight,
				   qp->width()+1, qp->height()+1, false);
		} else if (themeLook != WARP4) {
			draw3DRect(pnt, *colorActiveButton, 0, 0,
				   w-1, titleBarHeight-1, true);
			draw3DRect(pnt, *colorActiveButton, 0, titleBarHeight,
				   w-1, 2*titleBarHeight-1, false);
		}

		pnt.drawPixmap(offX, offY, *qp);
		if (qp->height() <= titleBarHeight) {
			pnt.drawPixmap(offX, titleBarHeight+offY, *qp);
		}
		pnt.end();
		delete qp;
	} else {
		p[Active] = qp;
	}

	str = locate("appdata", QString("icewm-themes/")
		     + themeName + s1 + "I" + s2);
	if (str.isEmpty())
		str = locate("appdata", QString("icewm-themes/")
			     + themeName + s1 + s2);

        qp = new QPixmap(str);
	if (!qp->isNull() && themeLook > 0) {
		int w = qp->width();
		if (titleBarHeight > w) w = titleBarHeight;
		p[InActive] = new QPixmap(w, 2*titleBarHeight );
		p[InActive] -> fill(cInActive);

		QPainter pnt( p[InActive] );

		int offX = (w - qp->width())/2;
		int offY = (titleBarHeight - qp->height())/2;
		if (offY < 0) offY = 0;

		if (themeLook == WIN95) {
			draw3DRect(pnt, *colorInActiveButton, offX-1, offY-1,
				   qp->width()+1, qp->height()+1, true);
			draw3DRect(pnt, *colorInActiveButton, offX-1, offY-1 + titleBarHeight,
				   qp->width()+1, qp->height()+1, false);
 		} else if (themeLook != WARP4) {
			draw3DRect(pnt, *colorInActiveButton, 0, 0,
				   w-1, titleBarHeight-1, true);
			draw3DRect(pnt, *colorActiveButton, 0, titleBarHeight,
				   w-1, 2*titleBarHeight-1, false);
		}
		pnt.drawPixmap(offX, offY, *qp);
		if (qp->height() <= titleBarHeight) {
			pnt.drawPixmap(offX, titleBarHeight+offY, *qp);
		}
		pnt.end();
		delete qp;
	} else {
		p[InActive] = qp;
	}
}




// Loads the specified Active/InActive files into the specific pixmaps, and
// can perform horizontal / vertical stretching if required for speed.
// Tries to implement some icewm specific pixmap handling for some dodgy themes
void ThemeHandler::setPixmap( QPixmap* p[], QString s1, QString s2,
							  bool stretch, bool stretchHoriz )
{
	if ( p[Active] )
		qWarning("kwin-icewm: setPixmap - should be null (1)\n");
	if ( p[InActive] )
		qWarning("kwin-icewm: setPixmap - should be null (2)\n");

	p[Active]   = new QPixmap( locate("data", QString("kwin/icewm-themes/")
								+ themeName + s1 + "A" + s2) );
	p[InActive] = new QPixmap( locate("data", QString("kwin/icewm-themes/")
								+ themeName + s1 + "I" + s2) );

	// Stretch the pixmap if requested.
	if ( stretch )
	{
		if (p[Active])
			p[Active] = stretchPixmap( p[Active], stretchHoriz );
		if (p[InActive])
			p[InActive] = stretchPixmap( p[InActive], stretchHoriz );
	}

	if ( p[Active] && p[InActive] )
	{
		// Make sure active and inactive pixmaps are the same width for proper painting
		if (p[Active]->width() > p[InActive]->width())
			p[InActive] = stretchPixmap( p[InActive], true, p[Active]->width() );
	}

}


// returns true if there were enough pixmaps loaded to
// draw the pixmap frame properly.
bool ThemeHandler::isFrameValid()
{
	return
	  ( validPixmaps( frameTL ) &&
		validPixmaps( frameT )  &&
		validPixmaps( frameTR ) &&
		validPixmaps( frameL )  &&
		validPixmaps( frameR )  &&
		validPixmaps( frameBL ) &&
		validPixmaps( frameB )  &&
		validPixmaps( frameBR ) );
}


// Resets the theme, and re-clients all kwin's wrapped windows.
bool ThemeHandler::reset( unsigned long)
{
	initialized = false;
	freePixmaps();
	readConfig();
	initTheme();
	validframe = isFrameValid();
	initialized = true;

	// recreate all clients
	return true;
}

bool ThemeHandler::supports( Ability ability )
{
    switch( ability )
    {
        case AbilityAnnounceButtons:
        case AbilityButtonMenu:
        case AbilityButtonOnAllDesktops:
        case AbilityButtonMinimize:
        case AbilityButtonMaximize:
        case AbilityButtonClose:
            return true;
        default:
            return false;
    };
}


////////////////////////////////////////////////////////////////////////////////////////////
// IceWM button class
////////////////////////////////////////////////////////////////////////////////////////////

IceWMButton::IceWMButton(IceWMClient *parent, const char *name, QPixmap* (*p)[2],
	  bool isToggle, const QString& tip, const int realizeBtns )
    : QButton(parent->widget(), name)
{
	m_realizeButtons = realizeBtns;
	setTipText(tip);
	setCursor(ArrowCursor);
	// Eliminate any possible background flicker
	setBackgroundMode( QWidget::NoBackground );
	client = parent;
	usePixmap( p );
	setFixedSize( sizeHint() );
	setToggleButton( isToggle );
}


void IceWMButton::setTipText(const QString &tip) {
	if(KDecoration::options()->showTooltips()) {
		QToolTip::remove(this );
		QToolTip::add(this, tip );
	}
}


QSize IceWMButton::sizeHint() const
{
	// Check for invalid data
	if ( validPixmaps( (QPixmap**) (*pix) ) )  // Cast to avoid dumb warning
	{
		QPixmap* p = (*pix)[ client->isActive() ? Active : InActive ];
		return( QSize(p->width(), titleBarHeight) );
	} else
		return( QSize(0, 0) );
}


void IceWMButton::usePixmap( QPixmap* (*p)[2] )
{
	if (validPixmaps( *p )) {
		pix = p;
		setFixedSize( (*pix)[Active]->width(), titleBarHeight );
		repaint( false );
	} else
		pix = NULL;
}


void IceWMButton::drawButton(QPainter *pnt)
{
	if ( pix && validPixmaps(*pix) )
	{
		QPixmap* p = (*pix)[ client->isActive() ? Active : InActive ];

		if( p && (!p->isNull()) )
		{
			int width = p->width();

			// Only draw the lower pixmap 1/2 for down, and upper 1/2 for up state
			if( isDown() || isOn() )
				pnt->drawPixmap(0, 0, *p, 0, titleBarHeight, width, titleBarHeight);
			else
				pnt->drawPixmap(0, 0, *p, 0, 0, width, titleBarHeight);
		}
	} else
		qWarning("kwin-icewm: Can't paint a null pixmap button");
}


void IceWMButton::turnOn( bool isOn )
{
	if ( isToggleButton() )
		setOn( isOn );
}


void IceWMButton::mousePressEvent( QMouseEvent* e )
{
	last_button = e->button();
	QMouseEvent me ( e->type(), e->pos(), e->globalPos(),
					 (e->button()&m_realizeButtons)?LeftButton:NoButton, e->state() );
	QButton::mousePressEvent( &me );
}


void IceWMButton::mouseReleaseEvent( QMouseEvent* e )
{
	last_button = e->button();
	QMouseEvent me ( e->type(), e->pos(), e->globalPos(),
					 (e->button()&m_realizeButtons)?LeftButton:NoButton, e->state() );
	QButton::mouseReleaseEvent( &me );
}



////////////////////////////////////////////////////////////////////////////////////////////
// IceWMClient class
////////////////////////////////////////////////////////////////////////////////////////////

IceWMClient::IceWMClient( KDecorationBridge* bridge, KDecorationFactory* factory )
    : KDecoration (bridge, factory),
      m_closing(false)
{
}


IceWMClient::~IceWMClient()
{
	// Free the menu pixmaps if previously allocated
	if ( menuButtonWithIconPix[Active] )
		delete menuButtonWithIconPix[Active];
	if ( menuButtonWithIconPix[InActive] )
		delete menuButtonWithIconPix[InActive];
}


void IceWMClient::init()
{
	createMainWidget( WNoAutoErase | WStaticContents );
	widget()->installEventFilter( this );

	// Set button pointers to null so we can track things
	for(int i= IceWMClient::BtnSysMenu; i < IceWMClient::BtnCount; i++)
		button[i] = NULL;

	// Make sure we can track the menu pixmaps too.
	menuButtonWithIconPix[Active] = NULL;
	menuButtonWithIconPix[InActive] = NULL;

	// No flicker thanks
	widget()->setBackgroundMode( NoBackground );

	// Pack the windowWrapper() window within a grid layout
	grid = new QGridLayout(widget(), 0, 0, 0);
	grid->setResizeMode(QLayout::FreeResize);
	grid->addRowSpacing(0, borderSizeY);	// Top grab bar

	// Do something IceWM can't do :)
	if (titleBarOnTop) {
        	if( isPreview())
			grid->addWidget( new QLabel( i18n( "<center><b>IceWM preview</b></center>" ), widget() ), 2, 1);
                else
			grid->addItem( new QSpacerItem( 0, 0 ), 2, 1);
		// no shade flicker
    	grid->addItem( new QSpacerItem( 0, 0, QSizePolicy::Fixed,									   QSizePolicy::Expanding ) );
    }
	else {
		// no shade flicker
    	grid->addItem( new QSpacerItem( 0, 0, QSizePolicy::Fixed,									   QSizePolicy::Expanding ) );
        	if( isPreview())
			grid->addWidget( new QLabel( i18n( "<center><b>IceWM preview</b></center>" ), widget() ), 1, 1);
                else
			grid->addItem( new QSpacerItem( 0, 0 ), 1, 1);
    }

	grid->setRowStretch(1, 10);
	grid->setRowStretch(2, 10);
	grid->setColStretch(1, 10);
	grid->addRowSpacing(3, borderSizeY);
	grid->addColSpacing(0, borderSizeX);
	grid->addColSpacing(2, borderSizeX);

	// Pack the titlebar with spacers and buttons
	hb = new QBoxLayout(0, QBoxLayout::LeftToRight, 0, 0, 0);
	hb->setResizeMode( QLayout::FreeResize );

	titleSpacerJ = addPixmapSpacer( titleJ );

	addClientButtons( *titleButtonsLeft );
	titleSpacerL = addPixmapSpacer( titleL );

	// Centre titlebar if required.
	QSizePolicy::SizeType spTitleBar;
	spTitleBar = titleBarCentered ? QSizePolicy::Expanding : QSizePolicy::Maximum;
	titleSpacerS = addPixmapSpacer( titleS, spTitleBar, 1 );
	titleSpacerP = addPixmapSpacer( titleP );

	titlebar = new QSpacerItem( titleTextWidth(caption()), titleBarHeight,
								QSizePolicy::Preferred, QSizePolicy::Fixed );
	hb->addItem(titlebar);

	titleSpacerM = addPixmapSpacer( titleM );
	titleSpacerB = addPixmapSpacer( titleB, QSizePolicy::Expanding, 1 );
	titleSpacerR = addPixmapSpacer( titleR );

	addClientButtons( *titleButtonsRight );

	titleSpacerQ = addPixmapSpacer( titleQ );

	if (titleBarOnTop)
		grid->addLayout ( hb, 1, 1 );
	else
		grid->addLayout ( hb, 2, 1 );
}


// Adds the buttons to the hbox layout as per the buttons specified
// in the button string 's'
void IceWMClient::addClientButtons( const QString& s )
{
	if (!s.isEmpty())
		for(unsigned int i = 0; i < s.length(); i++)
		{
			switch ( s[i].latin1() )
			{
				case 's':
					// Create the menu icons, and render with the current mini-icon
					// if explicitly requested by the theme.
					if ( (validPixmaps(menuButtonPix) || showMenuButtonIcon) && !button[BtnSysMenu])
					{
						if (showMenuButtonIcon) {
							renderMenuIcons();
							button[BtnSysMenu] = new IceWMButton(this, "menu",
								&menuButtonWithIconPix, false, i18n("Menu"), LeftButton|RightButton);
						}
						else
							button[BtnSysMenu] = new IceWMButton(this, "menu",
								&menuButtonPix, false, i18n("Menu"));

						connect( button[BtnSysMenu], SIGNAL(pressed()),
								 this, SLOT(menuButtonPressed()));
						connect( button[BtnSysMenu], SIGNAL(released()),
								 this, SLOT(menuButtonReleased()));
						hb->addWidget( button[BtnSysMenu] );
					}
					break;

				case 'x':
					if ( validPixmaps(closePix) && !button[BtnClose] && isCloseable())
					{
						button[BtnClose] = new IceWMButton(this, "close",
								&closePix, false, i18n("Close"));
						hb->addWidget( button[BtnClose] );
						connect( button[BtnClose], SIGNAL(clicked()),
								 this, SLOT(closeWindow()));
					}
					break;

				case 'm':
					if ( validPixmaps(maximizePix) && !button[BtnMaximize] && isMaximizable() )
					{
						button[BtnMaximize] = new IceWMButton(this, "maximize",
								&maximizePix, false, i18n("Maximize"), LeftButton|MidButton|RightButton);
						hb->addWidget( button[BtnMaximize] );
						connect( button[BtnMaximize], SIGNAL(clicked()),
								 this, SLOT(slotMaximize()));
					}
					break;

				case 'i':
					if ( validPixmaps(minimizePix) && !button[BtnMinimize] &&
						 isMinimizable() )
					{
						button[BtnMinimize] = new IceWMButton(this, "minimize",
								&minimizePix, false, i18n("Minimize"));
						hb->addWidget( button[BtnMinimize] );
						connect( button[BtnMinimize], SIGNAL(clicked()),
								 this, SLOT(minimize()));
					}
					break;

				/* Not yet implemented - how's hide useful anyway?
				case 'h':
					if ( button[BtnHide]  && !button[BtnHide] )
						hb->addWidget( button[BtnHide] );
					break; */

				case 'r':
					// NOTE: kwin doesn't have toggleShade() in clients.h !
				    if ( validPixmaps(rollupPix) && !button[BtnRollup] )
				    {
						button[BtnRollup] = new IceWMButton(this, "shade",
							isSetShade() ? &rolldownPix : &rollupPix,
										    false, i18n("Rollup"));
						hb->addWidget( button[BtnRollup] );
						connect( button[BtnRollup], SIGNAL(clicked()),
								 this, SLOT(toggleShade()));
					}
					break;

				case 'd':
					// Make depth == on all desktops
					if ( validPixmaps(depthPix) && !button[BtnDepth] )
					{
						button[BtnDepth] = new IceWMButton(this, "on_all_desktops",
								&depthPix, true, isOnAllDesktops()?i18n("Not on all desktops"):i18n("On all desktops"));
						button[BtnDepth]->turnOn( isOnAllDesktops() );
						hb->addWidget( button[BtnDepth] );
						connect( button[BtnDepth], SIGNAL(clicked()),
								 this, SLOT(toggleOnAllDesktops()));
					}
					break;
			}
		}
}


// Adds a pixmap to the titlebar layout via the use of a nice QSpacerItem
QSpacerItem* IceWMClient::addPixmapSpacer( QPixmap* p[], QSizePolicy::SizeType s, int hsize )
{
	QSpacerItem* sp;

	// Add a null spacer for zero image
	if ( p && p[Active] )
	{
		int w = (hsize == -1) ? p[Active]->width(): hsize;
		sp = new QSpacerItem( w, titleBarHeight, s, QSizePolicy::Fixed );
	}
	else
		sp = new QSpacerItem(0, 0, QSizePolicy::Maximum, QSizePolicy::Fixed );

	hb->addItem( sp );
	return sp;
}


void IceWMClient::renderMenuIcons()
{
	QPixmap miniIcon( icon().pixmap( QIconSet::Small, QIconSet::Normal) );

	if (!miniIcon.isNull())
	         for(int i = 0; i < 2; i++) {
                       if ( menuButtonWithIconPix[i] )
                              delete menuButtonWithIconPix[i];

                       // Try to be more friendly to dodgy themes - icewm assumes a square menu button
                       // but some pixmap themes don't provide a square menu button.
                       int w = titleBarHeight;
                       if (validPixmaps(menuButtonPix) && menuButtonPix[i]->width() > w)
                               w = menuButtonPix[i]->width();
                       menuButtonWithIconPix[i] = new QPixmap(w, 2*titleBarHeight );
                       if (themeLook != WIN95)
                               menuButtonWithIconPix[i] -> fill((i==0) ? *colorInActiveButton : *colorActiveButton);
                       else
                               menuButtonWithIconPix[i] -> fill((i==0) ? *colorInActiveTitleBar : *colorActiveTitleBar);
                       QPainter pnt( menuButtonWithIconPix[i] );

                       if (themeLook > 0 && themeLook != WIN95 && themeLook != WARP4) {
                               draw3DRect(pnt, *colorActiveButton, 0, 0,
                                          w-1, titleBarHeight-1, true);
                               draw3DRect(pnt, *colorActiveButton, 0, titleBarHeight,
                                          w-1, 2*titleBarHeight-1, false);
                       }
                       if (validPixmaps(menuButtonPix)) {
                               pnt.drawPixmap(0, 0, *menuButtonPix[i]);
                       }
                       int offset = (titleBarHeight - miniIcon.width())/2;
                       if (offset<0) offset = 0;
                       // Paint the mini icon over the menu pixmap in the centre
                       pnt.drawPixmap( offset, offset, miniIcon );
                       pnt.drawPixmap( offset, titleBarHeight+offset, miniIcon );
                       pnt.end();
                }

}


void IceWMClient::slotMaximize()
{
	maximize(button[BtnMaximize]->last_button);
}

void IceWMClient::toggleShade()
{
        setShade(!isSetShade());
}

int IceWMClient::titleTextWidth( const QString& s )
{
	// Obtains the actual width of the text, using the titlebar font
	QSize size;
	QFontMetrics fm( options()->font(true) );
	size = fm.size( 0, s );
	return size.width();
}


void IceWMClient::borders(int& left, int& right, int& top, int& bottom) const
{
	left = borderSizeX;
	right = borderSizeX;
	if( titleBarOnTop ) {
		top = titleBarHeight + borderSizeY;
		bottom = borderSizeY;
	} else {
		top = borderSizeY;
		bottom = titleBarHeight + borderSizeY;
	}
}


void IceWMClient::resize( const QSize& s )
{
	widget()->resize( s );
}


QSize IceWMClient::minimumSize() const
{
	return widget()->minimumSize();
}


// Repaint nicely upon resize to minimise flicker.
void IceWMClient::resizeEvent( QResizeEvent* e )
{
	calcHiddenButtons();

	if (widget()->isVisibleToTLW())
	{
		 widget()->update(widget()->rect());
		int dx = 0;
		int dy = 0;

		if ( e->oldSize().width() != widget()->width() )
			dx = 32 + QABS( e->oldSize().width() -  width() );

		if ( e->oldSize().height() != height() )
			dy = 8 + QABS( e->oldSize().height() -  height() );

		if ( dy )
			widget()->update( 0, height() - dy + 1, width(), dy );

		if ( dx )
		{
			widget()->update( width() - dx + 1, 0, dx, height() );
			widget()->update( QRect( QPoint(4,4), titlebar->geometry().bottomLeft() - QPoint(1,0) ) );
			widget()->update( QRect( titlebar->geometry().topRight(), QPoint( width() - 4, titlebar->geometry().bottom() ) ) );
			widget()->repaint(titlebar->geometry(), false);
		}
	}
}


// IceWM Paint magic goes here.
void IceWMClient::paintEvent( QPaintEvent* )
{
	QColor colorTitleShadow;
	QColor colorTitle;
	QColor c1;
	int rx, rw;

	QPainter p( widget() );
	int act = isActive() ? Active: InActive;

	// Determine titlebar shadow colors
	bool useShadow = isActive() ? useActiveShadow : useInActiveShadow;
	if ( useShadow )
		colorTitleShadow = isActive() ? *colorActiveTitleTextShadow : *colorInActiveTitleTextShadow;

	if ( themeTitleTextColors )
 		colorTitle = isActive()? *colorActiveTitleBarText : *colorInActiveTitleBarText;
	else
		colorTitle = options()->color(ColorFont, isActive());

	// Obtain widget bounds.
	QRect r;
	r = widget()->rect();
	int fillWidth  = r.width()  - 2*borderSizeX;
	int y = r.y();
	int x = r.x();
	int w = r.width();
	int h = r.height();

	// Do we have pixmaps for the frame?
	if (validframe)
	{
		// Top corner
		p.drawPixmap(0, 0, *frameTL[ act ], 0, 0, cornerSizeX, borderSizeY);
		p.drawPixmap(0, 0, *frameTL[ act ], 0, 0, borderSizeX, cornerSizeY);

		// Top right corner
		p.drawPixmap(w-cornerSizeX, 0, *frameTR[ act ],
					 frameTR[act]->width()-cornerSizeX, 0, cornerSizeX, borderSizeY);
		p.drawPixmap(w-borderSizeX, 0, *frameTR[ act ],
					 frameTR[act]->width()-borderSizeX, 0, borderSizeX, cornerSizeY);

		// Top bar
		p.drawTiledPixmap( cornerSizeX, 0, w-(2*cornerSizeX), borderSizeY, *frameT[ act ] );

		// Left bar
		p.drawTiledPixmap( 0, cornerSizeY, borderSizeX, h-(2*cornerSizeY), *frameL[ act ] );

		// Right bar
		p.drawTiledPixmap( w-borderSizeX, cornerSizeY, borderSizeX, h-(2*cornerSizeY),
						   *frameR[ act ],frameR[act]->width()-borderSizeX );

		// Bottom left corner
		p.drawPixmap(0, h-borderSizeY, *frameBL[ act ],
					 0, frameBL[act]->height()-borderSizeY, cornerSizeX, borderSizeY);
		p.drawPixmap(0, h-cornerSizeY, *frameBL[ act ],
					 0, frameBL[act]->height()-cornerSizeY, borderSizeX, cornerSizeY);

		// Bottom right corner
		p.drawPixmap(w-cornerSizeX, h-borderSizeY, *frameBR[ act ],
					 frameBR[act]->width()-cornerSizeX, frameBR[act]->height()-borderSizeY,
					 cornerSizeX, borderSizeY);

		p.drawPixmap(w-borderSizeX, h-cornerSizeY, *frameBR[ act ],
					 frameBR[act]->width()-borderSizeX, frameBR[act]->height()-cornerSizeY,
					 borderSizeX, cornerSizeY);

		// Bottom bar
		p.drawTiledPixmap(cornerSizeX, h-borderSizeY, w-(2*cornerSizeX), borderSizeY,
						  *frameB[ act ], 0, frameB[ act ]->height()-borderSizeY );

		// Ensure uncovered areas during shading are painted with something
		p.setPen( *colorInActiveBorder );
		if (titleBarOnTop)
	        p.drawLine( x+borderSizeX, y+h-borderSizeY-1,
						x+w-borderSizeX-1, y+h-borderSizeY-1);
		else
	        p.drawLine( x+borderSizeX, y+borderSizeY,
						x+w-borderSizeX-1, y+borderSizeY);

	} else
	{
		// Draw a stock IceWM frame instead of a pixmap frame
		c1 = isActive() ? *colorActiveBorder : *colorInActiveBorder;

		if (themeLook == WARP3 || themeLook == MOTIF) {
			draw3DRect(p, c1, x, y, w-1, h-1, true);
			p.setPen(c1);
			p.drawRect(x+1, y+1, w-2, h-2);
		} else {
			p.setPen( c1.light(135) );
			p.drawLine(0, 0, w-2, 0);
			p.drawLine(0, 0, 0, h-2);

			p.setPen(c1);
			p.drawLine(1, 1, w-3, 1);
			p.drawLine(1, 1, 1, h-3);

			p.setPen( c1.dark(140) );
			p.drawLine(1, h-2, w-2, h-2);
			p.drawLine(w-2, 1, w-2, h-2);

			p.setPen( Qt::black );
			p.drawLine(w-1, 0, w-1, h-1);
			p.drawLine(0, h-1, w-1, h-1);
		}


		// Fill frame border if required
		if (borderSizeX > 2)
		{
			// Fill Vertical sizes
			p.fillRect( x+2, y+2, borderSizeX-2, h-4, c1);
			p.fillRect( w-borderSizeX, y+2, borderSizeX-2, h-4, c1);
		}

		if (borderSizeY > 2)
		{
			// Fill horizontal frame parts
			p.fillRect( x+borderSizeX, y+2, fillWidth, borderSizeY-2, c1);
			p.fillRect( x+borderSizeX, h-borderSizeY, fillWidth, borderSizeY-2, c1);
		}

		if (themeLook == WARP3 || themeLook == MOTIF) {
			draw3DRect(p, c1, x+borderSizeX-1, y+borderSizeY-1,
				    w+1-2*borderSizeX, h+1-2*borderSizeY, false);
		}
		if (themeLook == MOTIF && !isShade()) {
			int xext = titleBarHeight + borderSizeX - 1;
			int yext = titleBarHeight + borderSizeY - 1;

			int xext2 = w-xext-2;
			int yext2 = h-yext-2;

			int bX = w - borderSizeX-1;
			int bY = h - borderSizeY-1;

			p.setPen( c1.dark(140) );
			p.drawLine(xext, 0, xext, borderSizeY);
			p.drawLine(xext2, 0, xext2, borderSizeY);
			p.drawLine(xext, bY, xext, h-1);
			p.drawLine(xext2, bY, xext2, h-1);

			p.drawLine(0, yext, borderSizeX, yext);
			p.drawLine(0, yext2, borderSizeX, yext2);
			p.drawLine(bX, yext, w-1, yext);
			p.drawLine(bX, yext2, w-1, yext2);

			p.setPen( c1.light(135) );

			++xext; ++yext; ++xext2; ++yext2;

			p.drawLine(xext, 0, xext, borderSizeY);
			p.drawLine(xext2, 0, xext2, borderSizeY);
			p.drawLine(xext, bY, xext, h-1);
			p.drawLine(xext2, bY, xext2, h-1);

			p.drawLine(0, yext, borderSizeX, yext);
			p.drawLine(0, yext2, borderSizeX, yext2);
			p.drawLine(bX, yext, w-1, yext);
			p.drawLine(bX, yext2, w-1, yext2);


		}

		// Ensure uncovered areas during shading are painted with something
		p.setPen( *colorInActiveBorder );
		if (titleBarOnTop)
	        p.drawLine( x+borderSizeX, y+h-borderSizeY-1,
						x+w-borderSizeX-1, y+h-borderSizeY-1);
		else
	        p.drawLine( x+borderSizeX, y+borderSizeY,
						x+w-borderSizeX-1, y+borderSizeY);
	}

	// Draw the title elements, if we need to draw a titlebar.
	if (titleBarHeight > 0)
	{
		QPixmap* titleBuffer = new QPixmap( width()-(2*borderSizeX), titleBarHeight );
		QPainter p2( titleBuffer, this );
		titleBuffer->fill( act ? *colorActiveTitleBar : *colorInActiveTitleBar );

		r = titleSpacerJ->geometry();
		if (!r.isEmpty() && titleJ[ act ])
			p2.drawPixmap( r.x()-borderSizeX, 0, *titleJ[ act ]);

		r = titleSpacerL->geometry();
		if (!r.isEmpty() && titleL[ act ])
			p2.drawPixmap( r.x()-borderSizeX, 0, *titleL[ act ]);

		r = titleSpacerS->geometry();
		if (!r.isEmpty() && titleS[ act ])
			p2.drawTiledPixmap( r.x()-borderSizeX, 0, r.width(), titleBarHeight, *titleS[ act ]);

		r = titleSpacerP->geometry();
		if (!r.isEmpty() && titleP[ act ])
			p2.drawPixmap( r.x()-borderSizeX, 0, *titleP[ act ]);

		r = titlebar->geometry();
		if (!r.isEmpty() && titleT[ act ] )
			p2.drawTiledPixmap( r.x()-borderSizeX, 0, r.width(), titleBarHeight, *titleT[ act ]);

		r = titleSpacerM->geometry();
		if (!r.isEmpty() && titleM[ act ])
			p2.drawPixmap( r.x()-borderSizeX, 0, *titleM[ act ], 0, 0, r.width(), r.height());

		r = titleSpacerB->geometry();
		if (!r.isEmpty() && titleB[ act ])
			p2.drawTiledPixmap( r.x()-borderSizeX, 0, r.width(), titleBarHeight, *titleB[ act ]);

		r = titleSpacerR->geometry();
		if (!r.isEmpty() && titleR[ act ])
			p2.drawPixmap( r.x()-borderSizeX, 0, *titleR[ act ], 0, 0, r.width(), r.height());

		r = titleSpacerQ->geometry();
		if (!r.isEmpty() && titleQ[ act ])
			p2.drawPixmap( r.x()-borderSizeX, 0, *titleQ[ act ], 0, 0, r.width(), r.height());

		p2.setFont( options()->font(true) );

		// Pre-compute as much as possible
		r = titlebar->geometry();
		rx = r.x() - borderSizeX;
		rw = width()-(2*borderSizeX)-r.x();

		// Paint a title text shadow if requested
		if ( useShadow )
		{
			p2.setPen( colorTitleShadow );
			p2.drawText(rx+1, 1, rw, titleBarHeight, AlignLeft|AlignVCenter, caption());
		}

		// Draw the title text
		p2.setPen( colorTitle );
		p2.drawText(rx, 0, rw, titleBarHeight, AlignLeft|AlignVCenter, caption());
		p2.end();

		bitBlt( widget(), borderSizeX, hb->geometry().y(), titleBuffer );

		delete titleBuffer;
	}
}


void IceWMClient::showEvent(QShowEvent *ev)
{
	calcHiddenButtons();

	titlebar->changeSize( titleTextWidth(caption()), titleBarHeight,
						  QSizePolicy::Preferred, QSizePolicy::Fixed );
	grid->activate();
	widget()->show();
	IceWMClient::showEvent(ev);
}


void IceWMClient::mouseDoubleClickEvent( QMouseEvent * e )
{
	if( e->button() != LeftButton )
		return;

	QRect r;
	if (titleBarOnTop)
		r.setRect( borderSizeX, borderSizeY, width()-(2*borderSizeX), titleBarHeight);
	else
		r.setRect( borderSizeX, height()-borderSizeY-titleBarHeight,
				   width()-(2*borderSizeX), titleBarHeight);

	if (r.contains( e->pos() ) )
		titlebarDblClickOperation();

}


void IceWMClient::wheelEvent(QWheelEvent *e)
{
	if (isSetShade() || QRect( 0, 0, width(), titleBarHeight ).contains( e->pos() ) )
		titlebarMouseWheelOperation( e->delta());
}


// Called via Client class when the miniIcon() changes
void IceWMClient::iconChange()
{
	if (validPixmaps(menuButtonPix) && showMenuButtonIcon)
	{
		if (button[BtnSysMenu])
		{
			renderMenuIcons();
			button[BtnSysMenu]->usePixmap( &menuButtonWithIconPix );
			if (button[BtnSysMenu]->isVisible())
				button[BtnSysMenu]->repaint(false);
		}
	}
}


void IceWMClient::desktopChange()
{
	if (button[BtnDepth])
	{
		button[BtnDepth]->turnOn( isOnAllDesktops() );
		button[BtnDepth]->repaint(false);
		button[BtnDepth]->setTipText(isOnAllDesktops() ? i18n("Not on all desktops") : i18n("On all desktops"));
	}
}


// Please don't modify the following unless you want layout problems
void IceWMClient::captionChange()
{
	QRect r( 0, borderSizeY, geometry().width(), titleBarHeight);

	titlebar->changeSize( titleTextWidth( caption() ), titleBarHeight,
						  QSizePolicy::Preferred, QSizePolicy::Fixed );
	titlebar->invalidate();
    grid->activate();
    widget()->repaint( r, false );
}


void IceWMClient::maximizeChange()
{
	// Change the button pixmap to restore if required
	if (button[BtnMaximize] && validPixmaps(restorePix))
	{
		button[BtnMaximize]->usePixmap( (maximizeMode()==MaximizeFull) ? &restorePix : &maximizePix );
		button[BtnMaximize]->setTipText( (maximizeMode()==MaximizeFull) ? i18n("Restore") : i18n("Maximize"));
	}
}


void IceWMClient::shadeChange()
{
	// Change the button pixmap to rolldown if required
	if (button[BtnRollup] && validPixmaps(rolldownPix))
	{
		button[BtnRollup]->usePixmap( isSetShade() ? &rolldownPix : &rollupPix );
		button[BtnRollup]->setTipText( isSetShade() ? i18n("Rolldown") : i18n("Rollup"));
	}

}


void IceWMClient::activeChange()
{
	widget()->repaint(false);

    // Reset the button pixmaps.
	for(int i= IceWMClient::BtnSysMenu; i < IceWMClient::BtnCount; i++)
		if(button[i])
			button[i]->repaint( false );
}


// This does the showing / hiding button magic
// for variable positioned buttons.
void IceWMClient::calcHiddenButtons()
{
	const int minwidth = 220; // Minimum width where all buttons are shown
	const int btn_width = 20; // Average width

	// Show/Hide buttons in this order - OnAllDesktops, Maximize, Menu, Rollup, Minimize, Close.
	IceWMButton* btnArray[] = { button[BtnDepth], button[BtnMaximize], button[BtnSysMenu],
								button[BtnRollup], button[BtnMinimize], button[BtnClose] };

	int current_width = width();
	int count = 0;
	int i;

	// Find out how many buttons we have to hide.
	while (current_width < minwidth)
	{
		current_width += btn_width;
		count++;
	}

	// Bound the number of buttons to hide
	if (count > 6) count = 6;

	// Hide the required buttons...
	for(i = 0; i < count; i++)
	{
		if (btnArray[i] && btnArray[i]->isVisible() )
			btnArray[i]->hide();
	}

	// Show the rest of the buttons...
	for(i = count; i < 6; i++)
	{
		if (btnArray[i] && (!btnArray[i]->isVisible()) )
			btnArray[i]->show();
	}
}


// Mouse position code modified from that in workspace.cpp
IceWMClient::Position IceWMClient::mousePosition( const QPoint& p ) const
{
    int rangeX = cornerSizeX;
    int rangeY = cornerSizeY;
    int borderX = borderSizeX;
    int borderY = borderSizeY;

    Position m = PositionCenter;

    if ((p.x()  > borderX && p.x() < width()  - borderX) &&
	    ( p.y() > borderY && p.y() < height() - borderY))
	return PositionCenter;

    if ( p.y() <= rangeY && p.x() <= rangeX)
		m = PositionTopLeft;
    else if ( p.y() >= height()-rangeY && p.x() >= width()-rangeX)
		m = PositionBottomRight;
    else if ( p.y() >= height()-rangeX && p.x() <= rangeX)
		m = PositionBottomLeft;
    else if ( p.y() <= rangeY && p.x() >= width()-rangeX)
		m = PositionTopRight;
    else if ( p.y() <= borderY )
		m = PositionTop;
    else if ( p.y() >= height()-borderY )
		m = PositionBottom;
    else if ( p.x() <= borderX )
		m = PositionLeft;
    else if ( p.x() >= width()-borderX )
    	m = PositionRight;
    else
		m = PositionCenter;
	return m;
}


void IceWMClient::menuButtonPressed()
{
	static QTime t;
	static IceWMClient* lastClient = NULL;
	bool dbl = ( lastClient == this && t.elapsed() <= QApplication::doubleClickInterval());
	lastClient = this;
	t.start();

	if (dbl)
	{
		m_closing = true;
		return;
    }

	QPoint menuPoint ( button[BtnSysMenu]->rect().bottomLeft() );

	// Move to right if menu on rhs, otherwise on left
	// and make this depend on windowWrapper(), not button.

        KDecorationFactory* f = factory();
	showWindowMenu( button[BtnSysMenu]->mapToGlobal(menuPoint) );
        if( !f->exists( this )) // 'this' was deleted
            return;
	button[BtnSysMenu]->setDown(false);
}

void IceWMClient::menuButtonReleased()
{
	if (m_closing)
		closeWindow();
}

bool IceWMClient::eventFilter( QObject* o, QEvent* e )
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

}

extern "C"
{
	KDE_EXPORT KDecorationFactory *create_factory()
	{
		IceWM::clientHandler = new IceWM::ThemeHandler;
		return IceWM::clientHandler;
	}
}


#include "icewm.moc"

// vim: ts=4
