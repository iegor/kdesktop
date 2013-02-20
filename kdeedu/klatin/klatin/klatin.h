/***************************************************************************
    begin                : Thu Jul 17
    copyright            : (C) 2001-2004 by George Wright
    email                : gwright@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef KLATIN_H
#define KLATIN_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kactionclasses.h>
#include <klineedit.h>
#include <kmainwindow.h>

#include "klatinchoose.h"
#include "klatingrammar.h"
#include "klatinverbs.h"
#include "klatinvocab.h"

#include "klatinsettings_vocabpage.h"

/** @short Main application class */
class KLatin : public KMainWindow
{
	Q_OBJECT
public:
	KLatin(QWidget* parent=0, const char *name=0);
	~KLatin();

	void setupActions();

public slots:
	/// Load vocabulary revision section
	void loadVocab();
	/// Load grammar revision section
	void loadGrammar();
	/// Load verb revision section
	void loadVerbs();
	/// Load revision notes in Konqueror
	void loadRevision();
	/// Parse settings from config file
	void loadSettings();

	/// Slot for the start button
	void startClicked();
	/// Reloads the section choose screen
	void resetGUI();
	
private slots:
	/// Slot for when settings are changed in the KConfig XT dialog
	void settingsChanged();
	
	void slotWriteMsg(const QString&);
	
private:

	KAction *m_loadVocab;
	KAction *m_loadGrammar;
	KAction *m_loadVerbs;
	KAction *m_loadRevision;

	KLatinChoose *klatinchoose;
	KLatinGrammar *klatingrammarsection;
	KLatinVocab *klatinvocabsection;
	KLatinVerbs *klatinverbssection;
	
	VocabPage *vocabPage;
	
	/// Current section loaded
	int m_section;
	
private:
	 /**
	  * Enable/Disable the items in the Section menu
	  *@param bool if bool is true then the menu items are enabled
	  */
	void updateSection(bool);
};

#endif // KLATIN_H
