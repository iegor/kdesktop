/***************************************************************************
                 provides access to mediacontrol configuration file
                             -------------------
    begin                : forgot :/
    copyright            : (C) 2000-2002 by Stefan Gehn
    email                : metz {AT} gehn {DOT} net

    code-skeleton taken from knewsticker which is
    Copyright (c) Frerich Raabe <raabe@kde.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "configfrontend.h"
// #include <kdebug.h>

ConfigFrontend::ConfigFrontend() : QObject(0, 0)
{
	_config = new KConfig(QString::null, true, false);
	_ownConfig = true;
}

ConfigFrontend::~ConfigFrontend()
{
    if (_ownConfig)
    {
        delete _config;
    }
}

ConfigFrontend::ConfigFrontend(KConfig *config) : QObject(0, 0)
{
	_config = config;
	_config->setGroup("MediaControl");
	_ownConfig = false;
}

// ====================================================================================

uint ConfigFrontend::mouseWheelSpeed() const
{
	return _config->readNumEntry("Mouse wheel speed", 5);
}

void ConfigFrontend::setMouseWheelSpeed(const uint mouseWheelSpeed)
{
	_config->writeEntry("Mouse wheel speed", mouseWheelSpeed);
	_config->sync();
}

// ====================================================================================

QString ConfigFrontend::player() const
{
	return _config->readPathEntry("Player", "Noatun");
}

void ConfigFrontend::setPlayer(const QString &player)
{
	_config->writePathEntry("Player", player);
	_config->sync();
}

// ====================================================================================

QString ConfigFrontend::theme() const
{
	return _config->readEntry("Theme", "default");
}

void ConfigFrontend::setTheme(const QString &theme)
{
	_config->writeEntry("Theme", theme);
	_config->sync();
}

// ====================================================================================

bool ConfigFrontend::useCustomTheme() const
{
	return _config->readBoolEntry("UseCustomTheme", false);
}

void ConfigFrontend::setUseCustomTheme(const bool use)
{
	_config->writeEntry("UseCustomTheme", use);
	_config->sync();
}

// ====================================================================================

#include "configfrontend.moc"
