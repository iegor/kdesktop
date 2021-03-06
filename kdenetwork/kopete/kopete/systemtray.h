/*
    systemtray.h  -  Kopete Tray Dock Icon

    Copyright (c) 2002      by Nick Betcher           <nbetcher@kde.org>
    Copyright (c) 2002-2003 by Martijn Klingens       <klingens@kde.org>
    Copyright (c) 2003      by Olivier Goffart        <ogoffart @ kde.org>

    Kopete    (c) 2002-2005 by the Kopete developers  <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This program is free software; you can redistribute it and/or modify  *
    * it under the terms of the GNU General Public License as published by  *
    * the Free Software Foundation; either version 2 of the License, or     *
    * (at your option) any later version.                                   *
    *                                                                       *
    *************************************************************************
*/

#ifndef SYSTEMTRAY_H
#define SYSTEMTRAY_H

#include <qpixmap.h>
#include <qmovie.h>

#include <ksystemtray.h>

#include "kopetemessageevent.h"

class QTimer;
class QPoint;
class KPopupMenu;
class KActionMenu;
class KopeteBalloon;

/**
 * @author Nick Betcher <nbetcher@kde.org>
 *
 * NOTE: This class is for use ONLY in libkopete! It is not public API, and
 *       is NOT supposed to remain binary compatible in the future!
 */
class KopeteSystemTray : public KSystemTray
{
	Q_OBJECT

public:
	/**
	 * Retrieve the system tray instance
	 */
	static KopeteSystemTray* systemTray( QWidget* parent = 0, const char* name = 0 );

	~KopeteSystemTray();

	// One method, multiple interfaces :-)
	void startBlink( const QString &icon );
	void startBlink( const QPixmap &icon );
	void startBlink( const QMovie &movie );
	void startBlink();

	void stopBlink();
	bool isBlinking() const { return mIsBlinking; };
	KPopupMenu *contextMenu() const { return KSystemTray::contextMenu(); };

protected:
	virtual void mousePressEvent( QMouseEvent *e );
	virtual void mouseDoubleClickEvent( QMouseEvent *me );
	virtual void contextMenuAboutToShow( KPopupMenu * );

signals:
	void aboutToShowMenu(KPopupMenu *am);

private slots:
	void slotBlink();
	void slotNewEvent(Kopete::MessageEvent*);
	void slotEventDone(Kopete::MessageEvent *);
	void slotConfigChanged();
	void slotReevaluateAccountStates();
	void slotRemoveBalloon();
	void addBalloon();

private:
	KopeteSystemTray( QWidget* parent, const char* name );
	QString squashMessage( const Kopete::Message& msgText );
	void removeBalloonEvent(Kopete::MessageEvent *);

	QTimer *mBlinkTimer;
	QPixmap mKopeteIcon;
	QPixmap mBlinkIcon;
	QMovie mMovie;

	bool mIsBlinkIcon;
	bool mIsBlinking;

	static KopeteSystemTray* s_systemTray;

	QPtrList<Kopete::MessageEvent> mEventList;
	QPtrList<Kopete::MessageEvent> mBalloonEventList;
	KopeteBalloon *m_balloon;
};

#endif

// vim: set noet ts=4 sts=4 sw=4:

