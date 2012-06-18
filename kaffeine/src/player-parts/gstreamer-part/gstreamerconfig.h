/*
 * gstreamerconfig.h - config dialog for gstreamer parameters
 *
 * Copyright (C) 2005 Jürgen Kofler <kaffeine@gmx.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef GSTREAMERCONFIG_H
#define GSTREAMERCONFIG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kdialogbase.h>

class QString;
class QStringList;
class KComboBox;
class KLineEdit;

class GStreamerConfig : public KDialogBase
{
	Q_OBJECT
public:
	GStreamerConfig(const QStringList& audioDrivers, const QStringList& videoDrivers);
	~GStreamerConfig();

	QString getAudioDriver() const;
	QString getVideoDriver() const;
	QString getDrive() const;

	void setDrive(const QString& drive);
	void setAudioDriver(const QString& name);
	void setVideoDriver(const QString& name);

private:
	KComboBox* m_audioDriverBox;
	KComboBox* m_videoDriverBox;
	KLineEdit* m_driveEdit;
};

#endif /* GSTREAMERCONFIG_H */
