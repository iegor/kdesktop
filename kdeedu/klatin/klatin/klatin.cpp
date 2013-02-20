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

#include <qradiobutton.h>

#include <kapplication.h>
#include <kconfigdialog.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <kurlrequester.h>

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#include "klatin.h"
#include "settings.h"

KLatin::KLatin(QWidget* parent, const char *name)
	: KMainWindow(parent, name)
{
	m_section = 0;
	
	// Setup the actions for the menubar
	setupActions();

	// Used for resetting the GUI after leaving
	// a section, but used here to initialise
	// the GUI for the first time.
	resetGUI();
}

KLatin::~KLatin()
{
}

void KLatin::setupActions()
{
	// Setup various menu actions
	KStdAction::preferences(this, SLOT(loadSettings()), actionCollection());
	m_loadVocab = new KAction(i18n("Load &Vocabulary"), 0, this, SLOT(loadVocab()),  actionCollection(), "go_vocab");
	m_loadGrammar = new KAction(i18n("Load &Grammar"), 0, this, SLOT(loadGrammar()),  actionCollection(), "go_grammar");
	m_loadVerbs = new KAction(i18n("Load V&erbs"), 0, this, SLOT(loadVerbs()),  actionCollection(), "go_verbs");
	m_loadRevision = new KAction(i18n("Load &Revision"), 0, this, SLOT(loadRevision()),  actionCollection(), "go_revision");
	KStdAction::quit(kapp, SLOT(quit()), actionCollection());
}

void KLatin::startClicked()
{
	if (klatinchoose->VocabOption->isOn())
		loadVocab();
	if (klatinchoose->GrammarOption->isOn())
		loadGrammar();
	if (klatinchoose->VerbsOption->isOn())
		loadVerbs();
	if (klatinchoose->RevisionOption->isOn())
		loadRevision();
}

// Start of section loading code

void KLatin::loadVocab()
{
	klatinchoose->close();
	
	klatinvocabsection = new KLatinVocab(this);
	klatinvocabsection->show();

	slotWriteMsg(i18n("Ready"));

	// Set this widget as the central widget
	setCentralWidget(klatinvocabsection);

	updateSection(FALSE);
	
	// When the child emits the signal exitted(),
	// then reset the GUI to go back to the menu
	connect(klatinvocabsection, SIGNAL(exited()), this, SLOT(resetGUI()));

	// Set the section variable
	m_section = 1;
}

void KLatin::loadGrammar()
{
	klatinchoose->close();
	
	klatingrammarsection = new KLatinGrammar(this);
	klatingrammarsection->show();

	slotWriteMsg(i18n("Grammar"));

	// Disable all the menu entries under Section/
	updateSection(FALSE);
	
	// Set this widget as the central widget
	setCentralWidget(klatingrammarsection);

	// When the child emits the signal exited(),
	// then reset the GUI to go back to the menu
	connect(klatingrammarsection, SIGNAL(exited()), this, SLOT(resetGUI()));
	connect(klatingrammarsection, SIGNAL(statusMsg(const QString&)), this, SLOT(slotWriteMsg(const QString&)));
	// Set the section variable
	m_section = 2;
}

void KLatin::slotWriteMsg(const QString& message)
{
	statusBar()->message(message);
}

void KLatin::loadVerbs()
{
	klatinchoose->close();
	
	klatinverbssection = new KLatinVerbs(this);
	klatinverbssection->show();

	slotWriteMsg(i18n("Verbs"));

	// Disable all the menu entries under Section/
	updateSection(FALSE);

	// Set this widget as the central widget
	setCentralWidget(klatinverbssection);

	// When the child emits the signal exited(),
	// then reset the GUI to go back to the menu
	connect(klatinverbssection, SIGNAL(exited()), this, SLOT(resetGUI()));
	connect(klatinverbssection, SIGNAL(statusMsg(const QString&)), this, SLOT(slotWriteMsg(const QString&)));

	// Set the section variable
	m_section = 3;
}

void KLatin::loadRevision()
{
	kapp->invokeHelp("klatin-index", "klatin");
}

void KLatin::loadSettings()
{
	if (KConfigDialog::showDialog("settings")) 
		return; 

	KConfigDialog *dialog = new KConfigDialog(this, "settings", Settings::self());
	
	vocabPage = new VocabPage(0); 
	vocabPage->kcfg_DefaultFile->setMode(KFile::File | KFile::LocalOnly);
	vocabPage->kcfg_DefaultFile->setFilter("*.kvtml");
	vocabPage->kcfg_DefaultFile->setCaption(i18n("Load Vocabulary File"));
	dialog->addPage(vocabPage, i18n("Vocabulary"), "kdict"); 
	connect(dialog, SIGNAL(settingsChanged()), this, SLOT(settingsChanged())); 
	dialog->show();
}

void KLatin::settingsChanged()
{
	// Only the vocab section so far needs config refreshed for it
	if (m_section == 1) {
		klatinvocabsection->changeVocab(Settings::defaultFile());
	}
}

void KLatin::resetGUI()
{
	// Load the central widget to show the user the
	// various options that are available to choose
	// and setCentralWidget it.
	klatinchoose = new KLatinChoose(this);
	klatinchoose->QuitButton->setIconSet(KGlobal::iconLoader()->loadIconSet("exit", KIcon::Small));
	klatinchoose->show();
	setCentralWidget(klatinchoose);

	// Connect the "Start" button to slot StartPressed()
	connect(klatinchoose->StartButton, SIGNAL(clicked()), this, SLOT(startClicked()));
	// Make the quit button quit the application :)
	connect(klatinchoose->QuitButton, SIGNAL(clicked()), kapp, SLOT(quit()));
	
	// Enable all the menu entries under Section/
	updateSection(TRUE);

	// Use XML GUI to construct the menubar
	setupGUI();

	// Reset the section variable
	m_section = 0;

	slotWriteMsg(i18n("Ready"));
}

void KLatin::updateSection(bool m_bool)
{
	// Disable all the menu entries under Section/
	m_loadVocab->setEnabled(m_bool);
	m_loadGrammar->setEnabled(m_bool);
	m_loadVerbs->setEnabled(m_bool);
	m_loadRevision->setEnabled(m_bool);
}

#include "klatin.moc"
