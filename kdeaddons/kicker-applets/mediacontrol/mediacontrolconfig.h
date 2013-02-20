/***************************************************************************
                        mediacontrol configuration dialog
                             -------------------
    begin                : forgot :/
    copyright            : (C) 2000-2005 by Stefan Gehn
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

#ifndef MEDIACONTROLCONFIG_H
#define MEDIACONTROLCONFIG_H

#include "configfrontend.h"
#include <kdialogbase.h>

class MediaControlConfigWidget;
class ConfigFrontend;
class KConfig;

class MediaControlConfig: public KDialogBase
{
Q_OBJECT
public:
	MediaControlConfig(ConfigFrontend *cfg, QWidget *parent = 0, const char* name = "MediaControlConfig");

	void readSkinDir(const QString &dir);
	void load();
	void save();

signals:
	void closing();
	void configChanged();

protected slots:
	void slotConfigChanged();
	void slotChangePreview(QListBoxItem *item);
	void slotUseThemesToggled(bool);
	virtual void slotApply();
	virtual void slotOk();
	virtual void slotCancel();

private:
	ConfigFrontend *_configFrontend;
	MediaControlConfigWidget *_child;
};
#endif // MEDIACONTROLCONFIG_H
