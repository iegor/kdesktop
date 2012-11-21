/***************************************************************************
                          glowclient.h  -  description
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

#ifndef GLOW_CLIENT_H
#define GLOW_CLIENT_H

#include <vector>
#include <map>
#include <kdecoration.h>
#include <kdecorationfactory.h>

class QPixmap;
class QBitmap;
class QTimer;
class QBoxLayout;
class QGridLayout;
class QVBoxLayout;
class QSpacerItem;

namespace Glow
{

class GlowButton;
class GlowButtonFactory;

//-----------------------------------------------------------------------------
// GlowTheme
//-----------------------------------------------------------------------------

struct GlowTheme
{
	QSize buttonSize;
	
	QString backgroundPixmap;
	QString backgroundAlphaPixmap;

	QString stickyOnPixmap;
	QString stickyOffPixmap;
	QString maximizeOnPixmap;
	QString maximizeOffPixmap;
	QString helpPixmap;
	QString closePixmap;
	QString iconifyPixmap;

	QString stickyOnGlowPixmap;
	QString stickyOffGlowPixmap;
	QString maximizeOnGlowPixmap;
	QString maximizeOffGlowPixmap;
	QString helpGlowPixmap;
	QString closeGlowPixmap;
	QString iconifyGlowPixmap;
};

static GlowTheme default_glow_theme = {
	QSize (17, 17),
	"background.png", "background_alpha.png",
	"stickyon.png", "stickyoff.png",
	"maximizeon.png", "maximizeoff.png",
	"help.png", "close.png", "iconify.png",
	"stickyon_glow.png", "stickyoff_glow.png",
	"maximizeon_glow.png", "maximizeoff_glow.png",
	"help_glow.png", "close_glow.png", "iconify_glow.png" };

//-----------------------------------------------------------------------------
// GlowClientConfig
//-----------------------------------------------------------------------------

class GlowClientConfig
{
public:
	GlowClientConfig();

	void load (KDecorationFactory *factory);

	QColor stickyButtonGlowColor;
	QColor helpButtonGlowColor;
	QColor iconifyButtonGlowColor;
	QColor maximizeButtonGlowColor;
	QColor closeButtonGlowColor;
	bool showResizeHandle;
	int titlebarGradientType;
	QString themeName;
};

//-----------------------------------------------------------------------------
// GlowClientGlobals
//-----------------------------------------------------------------------------

class GlowClientGlobals : public KDecorationFactory
{
public:
	enum PixmapType { StickyOn, StickyOff, Help, Iconify, MaximizeOn,
		MaximizeOff, Close };

	static GlowClientGlobals *instance();
	
	~GlowClientGlobals();

	virtual KDecoration* createDecoration( KDecorationBridge* b );
	virtual bool reset( unsigned long changed );
    virtual bool supports( Ability ability );
	QValueList< GlowClientGlobals::BorderSize >  borderSizes() const;
	
	QString getPixmapName(PixmapType type, bool isActive);

	GlowTheme * theme() const { return _theme; }
	GlowClientConfig * config() const { return _config; }
	GlowButtonFactory * buttonFactory() { return _button_factory; }

private:
	static GlowClientGlobals *m_instance;
	
	GlowTheme * _theme;
	GlowClientConfig * _config;
	GlowButtonFactory * _button_factory;

	GlowClientGlobals();
	void readConfig();
	void readTheme ();
	bool createPixmaps();
	void deletePixmaps();
	bool createPixmap(PixmapType type,bool isActive);
	const QString getPixmapTypeName(PixmapType type);
};

//-----------------------------------------------------------------------------
// GlowClient
//-----------------------------------------------------------------------------

class GlowClient : public KDecoration
{
	Q_OBJECT
public:
	GlowClient( KDecorationBridge* b, KDecorationFactory* f );
	~GlowClient();
	
	virtual void init();
	virtual void borders( int&, int&, int&, int& ) const;
	virtual void resize( const QSize& );
	virtual QSize minimumSize() const;

protected:
	virtual void resizeEvent( QResizeEvent * );
	virtual void paintEvent( QPaintEvent * );
	virtual void showEvent( QShowEvent * );
	virtual void mouseDoubleClickEvent( QMouseEvent * );
	virtual void wheelEvent( QWheelEvent * );
	virtual void maximizeChange();
	virtual void activeChange();
	virtual void iconChange();
	virtual void desktopChange();
	virtual void shadeChange();
	virtual void captionChange();
	virtual Position mousePosition(const QPoint &) const;
	virtual bool eventFilter( QObject* o, QEvent* e );

private:
	std::vector<GlowButton*> m_buttonList;
	std::vector<GlowButton*> m_leftButtonList;
	std::vector<GlowButton*> m_rightButtonList;
	GlowButton *m_stickyButton;
	GlowButton *m_helpButton;
	GlowButton *m_minimizeButton;
	GlowButton *m_maximizeButton;
	GlowButton *m_closeButton;
	QBoxLayout *m_leftButtonLayout;
	QBoxLayout *m_rightButtonLayout;
	QSpacerItem * _bottom_spacer;
	QSpacerItem * _title_spacer;
	QVBoxLayout * _main_layout;

	void createButtons();
	void updateButtonPositions();
	/**
	 * Before this method is called we have to update the button
	 * positions with updateButtonPositions() because the pixmaps
	 * depend on the position
	 */
	void updateButtonPixmaps();
	void resetLayout();
	void doShape();
	bool isLeft(GlowButton *button);
	bool isRight(GlowButton *button);

protected slots:
	void slotMaximize();
};

} // namespace

#endif
