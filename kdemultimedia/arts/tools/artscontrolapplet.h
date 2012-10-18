/***************************************************************************
                          artscontrolapplet.h  -  description
                             -------------------
    begin                : Don Jan 30 20:42:53 CET 2003
    copyright            : (C) 2003 by Arnold Krille
    email                : arnold@arnoldarts.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ARTSCONTROLAPPLET_H
#define ARTSCONTROLAPPLET_H

#ifdef HAVE_CONFIG_H
        #include <config.h>
#endif

#include <kpanelapplet.h>
#include <qstring.h>
#include <kconfig.h>

class ArtsControlAppletPrivate;

class ArtsControlApplet : public KPanelApplet
{
   Q_OBJECT
public:
	/**
	* Construct a @ref KPanelApplet just like any other widget.
	*
	* @param configFile The configFile handed over in the factory function.
	* @param Type The applet @ref type().
	* @param actions Standard RMB menu actions supported by the applet (see @ref action() ).
	* @param parent The pointer to the parent widget handed over in the factory function.
	* @param name A Qt object name for your applet.
	**/
	ArtsControlApplet(const QString& configFile, Type t = Normal, int = 0,
        QWidget * = 0, const char * = 0);
	/** destructor */
	~ArtsControlApplet();
	/**
	* Retrieve a suggested width for a given height.
	*
	* Every applet should reimplement this function.
	*
	* Depending on the panel orientation the height (horizontal panel) or the
	* width (vertical panel) of the applets is fixed.
	* The exact values of the fixed size component depend on the panel size.
	*
	* On a horizontal panel the applet height is fixed, the panel will
	* call @ref widthForHeight(int height) with @p height
	* equal to 'the fixed applet height'
	* when laying out the applets.
	*
	* The applet can now choose the other size component (width)
	* based on the given height.
	*
	* The width you return is granted.
	**/
	int widthForHeight( int ) const;
	/**
	* @return A suggested height for a given width.
	*
	* Every applet should reimplement this function.
	*
	* Depending on the panel orientation the height (horizontal panel) or the
	* width (vertical panel) of the applets is fixed.
	* The exact values of the fixed size component depend on the panel size.
	*
	* On a vertical panel the applet width is fixed, the panel will
	* call @ref heightForWidth(int width) with @p width
	* equal to 'the fixed applet width'
	* when laying out the applets.
	*
	* The applet can now choose the other size component (height)
	* based on the given width.
	*
	* The height you return is granted.
	**/
	int heightForWidth( int ) const;
	/**
	* Is called when the user selects "About" from the applets RMB menu.
	* Reimplement this function to launch a about dialog.
	*
	* Note that this is called only when your applet supports the About action.
	* See @ref Action and @ref KPanelApplet().
	**/
	void about();
	/**
	* Is called when the user selects "Help" from the applets RMB menu.
	* Reimplement this function to launch a manual or help page.
	*
	* Note that this is called only when your applet supports the Help action.
	* See @ref Action and @ref KPanelApplet().
	**/
	void help();
	/**
	* Is called when the user selects "Preferences" from the applets RMB menu.
	* Reimplement this function to launch a preferences dialog or kcontrol module.
	*
	* Note that this is called only when your applet supports the preferences action.
	* See @ref Action and @ref KPanelApplet().
	**/
	void preferences();

protected:

	void resizeEvent( QResizeEvent* );

	virtual void mousePressEvent( QMouseEvent* );

	void positionChange( Position );

public slots:
	void supdatelayout() { emit updateLayout(); }

protected slots:
	void resetLayout();

private:
	KConfig *ksConfig;
	ArtsControlAppletPrivate *p;
};

#endif
