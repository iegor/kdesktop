/***************************************************************************
                          mediacontrol configuration dialog
                             -------------------
    begin                : forgot :/
    copyright            : (C) 2000-2005 by Stefan Gehn
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "mediacontrolconfig.h"
#include "mediacontrolconfigwidget.h"

#include <qdir.h>
#include <qcheckbox.h>
#include <qlistbox.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qgroupbox.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klistbox.h>
#include <klocale.h>
#include <knuminput.h>
#include <kstandarddirs.h>

MediaControlConfig::MediaControlConfig( ConfigFrontend *cfg, QWidget *parent, const char* name)
: KDialogBase( parent, name, false, i18n("MediaControl"), Ok | Apply | Cancel, Ok, false )
{
	_configFrontend = cfg;
	if (!_configFrontend) // emergency!!!
		return;

	_child = new MediaControlConfigWidget(this);
	setMainWidget ( _child );

#ifdef HAVE_XMMS
	_child->playerListBox->insertItem("XMMS");
#endif
	_child->playerListBox->insertItem("Noatun");
	_child->playerListBox->insertItem("Amarok");
	_child->playerListBox->insertItem("JuK");
	_child->playerListBox->insertItem("mpd");
	_child->playerListBox->insertItem("KsCD");

	_child->themeListBox->clear();
	// fill with available skins
	KGlobal::dirs()->addResourceType("themes", KStandardDirs::kde_default("data") + "mediacontrol");
	QStringList list = KGlobal::dirs()->resourceDirs("themes");
	for (QStringList::ConstIterator it = list.constBegin(); it != list.constEnd(); ++it)
		readSkinDir(*it);

	connect(_child->mWheelScrollAmount, SIGNAL(valueChanged(int)), SLOT(slotConfigChanged()));
	connect(_child->playerListBox, SIGNAL(selectionChanged()), SLOT(slotConfigChanged()));
	connect(_child->themeListBox, SIGNAL(selectionChanged()), SLOT(slotConfigChanged()));
	connect(_child->themeListBox, SIGNAL(selectionChanged(QListBoxItem *)), SLOT(slotChangePreview(QListBoxItem *)));
	connect(_child->mUseThemes, SIGNAL(toggled(bool)), SLOT(slotConfigChanged()) );
	connect(_child->mUseThemes, SIGNAL(toggled(bool)), SLOT(slotUseThemesToggled(bool)) );

	load();
	show();

	enableButtonApply ( false ); // apply id disabled until something changed
}

void MediaControlConfig::readSkinDir( const QString &dir )
{
	QDir directory( dir );
	if (!directory.exists())
		return;

	const QFileInfoList *list = directory.entryInfoList();
	QFileInfoListIterator it(*list);

	while ( it.current() )
	{
		// append directory-name to our theme-listbox
		if ( QFile(it.current()->absFilePath()+"/play.png").exists() )
			_child->themeListBox->insertItem ( it.current()->baseName(), -1 );
		++it;
	}
}

// ============================================================================

void MediaControlConfig::load()
{
	// find the playerstring from config in the playerlist and select it if found
	QListBoxItem *item = 0;

	item = _child->playerListBox->findItem(  _configFrontend->player() );
	if ( item )
		_child->playerListBox->setCurrentItem ( item );
	else
		_child->playerListBox->setCurrentItem( 0 );

	// reset item to a proper state
	item=0;

	_child->mWheelScrollAmount->setValue( _configFrontend->mouseWheelSpeed() );

	// Select the used Theme
	item = _child->themeListBox->findItem(  _configFrontend->theme() );
	if ( item )
		_child->themeListBox->setCurrentItem( item );
	else
		_child->themeListBox->setCurrentItem( 0 );


	bool ison = _configFrontend->useCustomTheme();
	_child->mUseThemes->setChecked( ison );
	slotUseThemesToggled( ison );
}

void MediaControlConfig::save()
{
//	kdDebug(90200) << "MediaControlConfig::save()" << endl;
	for ( int it=0 ; it <= _child->playerListBox->numRows(); ++it )
	{
		if ( _child->playerListBox->isSelected(it) )
		{
			_configFrontend->setPlayer ( _child->playerListBox->text(it) );
		}
	}

	_configFrontend->setMouseWheelSpeed ( _child->mWheelScrollAmount->value() );

	for ( int it=0 ; it <= _child->themeListBox->numRows(); ++it )
	{
		if ( _child->themeListBox->isSelected(it) )
		{
			_configFrontend->setTheme ( _child->themeListBox->text(it) );
		}
	}

	_configFrontend->setUseCustomTheme( _child->mUseThemes->isChecked() );

	emit configChanged();
}

void MediaControlConfig::slotApply()
{
	save();
	enableButtonApply(false);
}

void MediaControlConfig::slotOk()
{
	save();
	emit closing();
}

void MediaControlConfig::slotCancel()
{
	emit closing();
}

void MediaControlConfig::slotConfigChanged()
{
	enableButtonApply ( true );
}

void MediaControlConfig::slotChangePreview(QListBoxItem *item)
{
	QString skindir = item->text();
	_child->previewPrev->setIconSet(SmallIconSet(locate("themes",skindir+"/prev.png")));
	_child->previewPlay->setIconSet(SmallIconSet(locate("themes",skindir+"/play.png")));
	_child->previewPause->setIconSet(SmallIconSet(locate("themes",skindir+"/pause.png")));
	_child->previewStop->setIconSet(SmallIconSet(locate("themes",skindir+"/stop.png")));
	_child->previewNext->setIconSet(SmallIconSet(locate("themes",skindir+"/next.png")));
}

void MediaControlConfig::slotUseThemesToggled(bool on)
{
	_child->themeListBox->setEnabled(on);
	_child->previewGroupBox->setEnabled(on);
}

#include "mediacontrolconfig.moc"
