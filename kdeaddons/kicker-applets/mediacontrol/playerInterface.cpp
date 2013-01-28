/***************************************************************************
              this is the abstract class to access a player from
                             -------------------
    begin                : Mon Jan 15 21:09:00 CEST 2001
    copyright            : (C) 2001-2002 by Stefan Gehn
    email                : metz {AT} gehn {DOT} net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "playerInterface.h"
#include "playerInterface.moc"
#include <kapplication.h>
#include <kmessagebox.h>
#include <klocale.h>

PlayerInterface::PlayerInterface() { }   // Constructor
PlayerInterface::~PlayerInterface() { }  // Destructor

void PlayerInterface::startPlayer(const QString &desktopname)
{
	if (KApplication::startServiceByDesktopName(desktopname, QStringList(),
		0, 0, 0, "", false) > 0)
	{
		KMessageBox::error(0, i18n("Could not start media player."));
	}
}
