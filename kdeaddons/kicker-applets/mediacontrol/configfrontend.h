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

#ifndef CONFIGFRONTEND_H
#define CONFIGFRONTEND_H

#include <kconfig.h>
#include <klocale.h>

class ConfigFrontend : public QObject
{
	Q_OBJECT
	public:
		ConfigFrontend();
		ConfigFrontend(KConfig *);
		virtual ~ConfigFrontend();

		uint mouseWheelSpeed() const;
		QString player() const;
		QString theme() const;
		bool useCustomTheme() const;

	public slots:
		void setMouseWheelSpeed(const uint);
		void setPlayer(const QString &player);
		void setTheme(const QString &theme);
		void setUseCustomTheme(const bool use);
		void reparseConfiguration() { _config->reparseConfiguration(); }

	private:
		KConfig *_config;
		bool _ownConfig:1;
};

#endif // CONFIGFRONTEND_H
