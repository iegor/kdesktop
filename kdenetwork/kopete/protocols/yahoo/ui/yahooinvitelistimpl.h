/*
    YahooInviteListImpl - conference invitation dialog
    
    Copyright (c) 2004 by Duncan Mac-Vicar P.    <duncan@kde.org>
    
    Kopete    (c) 2002-2004 by the Kopete developers  <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This program is free software; you can redistribute it and/or modify  *
    * it under the terms of the GNU General Public License as published by  *
    * the Free Software Foundation; either version 2 of the License, or     *
    * (at your option) any later version.                                   *
    *                                                                       *
    *************************************************************************
*/

#ifndef YAHOO_INVITE_LIST_IMPL
#define YAHOO_INVITE_LIST_IMPL

#include <qwidget.h>

#include "yahooinvitelistbase.h"

class YahooInviteListImpl : public YahooInviteListBase
{
	Q_OBJECT
public: 
	YahooInviteListImpl(QWidget *parent=0, const char *name=0);
	~YahooInviteListImpl();
	
	void fillFriendList( const QStringList &buddies );
	void addInvitees( const QStringList &buddies );
	void removeInvitees( const QStringList &buddies );
	void setRoom( const QString &room );
	void addParticipant( const QString &participant );
private:
	
signals:
	void readyToInvite( const QString &room, const QStringList &buddies, const QStringList &participants, const QString &msg );
protected slots:

public slots:
    virtual void btnInvite_clicked();
    virtual void btnCancel_clicked();
    virtual void btnAddCustom_clicked();
    virtual void btnRemove_clicked();
    virtual void btnAdd_clicked();
private:
	void updateListBoxes();

	QStringList m_buddyList;
	QStringList m_inviteeList;
	QStringList m_participants;
	QString m_room;
};

#endif

