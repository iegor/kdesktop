/***************************************************************************
                          ksig.cpp  -  description
                             -------------------
    begin                : Tue Jul  9 23:14:22 EDT 2002
    copyright            : (C) 2002 by Scott Wheeler
    email                : wheeler@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <kapplication.h>
#include <kstdaction.h>
#include <kaction.h>
#include <kconfig.h>
#include <kglobalsettings.h>
#include <kmessagebox.h>
#include <kstatusbar.h>
#include <ktextedit.h>
#include <klistviewsearchline.h>
#include <klocale.h>

#include <qsplitter.h>

#include "ksig.h"
#include "siglistviewitem.h"
#include "standardtext.h"

////////////////////////////////////////////////////////////////////////////////
// public methods
////////////////////////////////////////////////////////////////////////////////

KSig::KSig(QWidget *parent, const char *name) : KMainWindow(parent, name),
						changed(false)
{
    setupActions();
    setupLayout();
    setupSearchLine();

    if (!initialGeometrySet())
        resize(640,480);
    setupGUI(ToolBar | Keys | StatusBar | Create);
    setAutoSaveSettings();

    readConfig();
    loadData();
}

KSig::~KSig()
{

}

////////////////////////////////////////////////////////////////////////////////
// private methods
////////////////////////////////////////////////////////////////////////////////

void KSig::setupActions()
{
    KStdAction::openNew(this, SLOT(add()), actionCollection());
    new KAction(i18n("Remove"), "editdelete", 0, this, SLOT(remove()), actionCollection(), "remove");
    KStdAction::save(this, SLOT(save()), actionCollection());
    KStdAction::quit(this, SLOT(quit()), actionCollection());

    KStdAction::cut(kapp, SLOT(cut()), actionCollection());
    KStdAction::copy(kapp, SLOT(copy()), actionCollection());
    KStdAction::paste(kapp, SLOT(paste()), actionCollection());
    new KAction(i18n("C&lear"), "editclear", 0, kapp, SLOT(clear()), actionCollection(), "clear");

    new KAction(i18n("Edit Standard Header"), 0, 0, this, SLOT(editHeader()), actionCollection(), "editHeader");
    new KAction(i18n("Edit Standard Footer"), 0, 0, this, SLOT(editFooter()), actionCollection(), "editFooter");
}

void KSig::setupLayout()
{
    splitter = new QSplitter(Qt::Vertical, this);

    setCentralWidget(splitter);

    sigList = SigListView::instance(splitter, "sigList");

    sigEdit = new KTextEdit(splitter, "sigEdit");
    sigEdit->setTextFormat(Qt::PlainText);
    sigEdit->setFont(KGlobalSettings::fixedFont());
    sigEdit->setEnabled(false);
    sigEdit->setCheckSpellingEnabled(true);

    statusBar()->insertItem(i18n(" Line: %1 ").arg(0), LineNumber, 0, true);
    statusBar()->insertItem(i18n(" Col: %1 ").arg(0), ColumnNumber, 0, true);
    statusBar()->show();
    
    updateListLock = false;
    connect(sigEdit, SIGNAL(textChanged()), this, SLOT(updateList()));
    connect(sigList, SIGNAL(selectionChanged()), this, SLOT(updateEdit()));
    connect(sigEdit, SIGNAL(cursorPositionChanged(int,int)), this, SLOT(updateStatus(int,int)));
}

void KSig::setupSearchLine()
{
    KListViewSearchLineWidget *searchWidget = new KListViewSearchLineWidget(sigList, this);
    new KWidgetAction(searchWidget, i18n("Search"), KShortcut(), 0, 0, actionCollection(), "search");
}

void KSig::loadData()
{
    if(sigList->childCount() > 0) {
        sigList->setCurrentItem(sigList->firstChild());
        sigList->setSelected(sigList->firstChild(), true);

        sigEdit->setEnabled(true);
    }
    else {
        updateListLock = true;
        sigEdit->setText(i18n("To get started, first create a new signature by selecting "
                              "\"New\" above. You will then be able to edit and save "
                              "your collection of signatures."));
        updateListLock = false;
    }
    setDataChanged(false);
}

void KSig::readConfig()
{
    setAutoSaveSettings();
    KConfig *config = KGlobal::config();
    {
	KConfigGroupSaver saver(config, "Settings");
	if(splitter) {
	    QValueList<int> sizes;

	    sizes.append(config->readNumEntry("ListSize", -1));
	    sizes.append(config->readNumEntry("EditSize", -1));

	    if(sizes[0] > 0 && sizes[1] > 0)
		splitter->setSizes(sizes);
	}

	header = config->readEntry("Header");
	footer = config->readEntry("Footer");
    }
}

void KSig::writeConfig()
{
    KConfig *config = KGlobal::config();
    {
	KConfigGroupSaver saver(config, "Settings");
	if(splitter) {
	    QValueList<int> sizes = splitter->sizes();

	    config->writeEntry("ListSize", sizes[0]);
	    config->writeEntry("EditSize", sizes[1]);
	}

	config->writeEntry("Header", header);
	config->writeEntry("Footer", footer);
    }
    config->sync();
}

bool KSig::queryClose()
{
    if(changed) {
	int saveChanges = KMessageBox::questionYesNoCancel(this, i18n("Do you want to save your changes before exiting?"),QString::null,KStdGuiItem::save(),KStdGuiItem::discard());
	if(saveChanges == KMessageBox::Cancel)
	    return(false);
	else if(saveChanges == KMessageBox::Yes)
	    save();
    }

    writeConfig();
    return(true);
}

////////////////////////////////////////////////////////////////////////////////
// private slots
////////////////////////////////////////////////////////////////////////////////

void KSig::add()
{
    setDataChanged();
    SigListViewItem *sigItem = sigList->createItem();
    sigList->setCurrentItem(sigItem);

    sigEdit->setEnabled(true);
    sigEdit->setFocus();
    sigEdit->clear();
}

void KSig::remove()
{
    setDataChanged();
    sigEdit->clear();
    delete(sigList->currentItem());
}

void KSig::save()
{
    sigList->save();
    setDataChanged(false);
}

void KSig::setDataChanged(bool value)
{
    changed = value;

    if(changed)
	setCaption(kapp->makeStdCaption(QString::null, true, true));
    else
	setCaption(kapp->makeStdCaption(QString::null, true, false));
}

void KSig::editHeader()
{
    StandardText *t = new StandardText(0, 0, true);
    t->setLabel(i18n("Standard signature header:"));
    t->setText(header);
    
    connect(t, SIGNAL(textUpdated(const QString &)), this, SLOT(setHeader(const QString &)));
    
    t->exec();
}

void KSig::editFooter()
{
    StandardText *t = new StandardText(0, 0, true);
    t->setLabel(i18n("Standard signature footer:"));
    t->setText(footer);
    
    connect(t, SIGNAL(textUpdated(const QString &)), this, SLOT(setFooter(const QString &)));
    
    t->exec();
}

void KSig::updateList()
{
    if(!updateListLock) {
	setDataChanged();
	SigListViewItem *item = sigList->currentItem();
	if(item)
	    item->setText(sigEdit->text());
    }
}

void KSig::updateEdit()
{
    updateListLock = true;

    SigListViewItem *item = sigList->currentItem();
    if(item)
	sigEdit->setText(item->text());

    updateListLock = false;
}

void KSig::updateStatus(int line, int column)
{
    statusBar()->changeItem(i18n(" Line: %1 ").arg(line + 1), LineNumber);
    statusBar()->changeItem(i18n(" Col: %1 ").arg(column + 1), ColumnNumber);
}

void KSig::quit()
{
    close();
}

#include "ksig.moc"
