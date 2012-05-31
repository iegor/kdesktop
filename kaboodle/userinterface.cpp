// Copyright (C) 2002 Neil Stevens <neil@qualityassistant.com>
// Copyright (C) 1999 Charles Samuels <charles@kde.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// THE AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
// AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name(s) of the author(s) shall not be
// used in advertising or otherwise to promote the sale, use or other dealings
// in this Software without prior written authorization from the author(s).

#include <arts/kplayobjectfactory.h>
#include <kconfig.h>
#include <kdialog.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenubar.h>
#include <kpropertiesdialog.h>
#include <kstatusbar.h>
#include <kstdaction.h>
#include <kurldrag.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qlcdnumber.h>
#include <qvbox.h>
#include <kkeydialog.h>
#include <kvideowidget.h>

#include "conf.h"
#include "kaboodleapp.h"
#include "kaboodle_factory.h"
#include "player.h"
#include "view.h"
#include "userinterface.h"

Kaboodle::UserInterface::UserInterface(QWidget *parent, const KURL &initialFile)
	: KParts::MainWindow(parent)
{
	setAcceptDrops(true);
	setStandardToolBarMenuEnabled(true);

	KStdAction::open(this, SLOT(fileOpen()), actionCollection());
	KStdAction::quit(kapp, SLOT(quit()), actionCollection());
	KStdAction::preferences(this, SLOT(playerPreferences()), actionCollection());
        KStdAction::keyBindings( this, SLOT( slotConfigureKeys() ), actionCollection() );

	menubarAction = KStdAction::showMenubar(this, SLOT(showMenubar()), actionCollection());
	propertiesAction = new KAction(i18n("Properties"), 0, this, SLOT(properties()), actionCollection(), "properties");
	propertiesAction->setEnabled(false);

	part = new Player(this, "KaboodlePlayer", this, "KaboodleView");
	part->view()->setButtons(KMediaPlayer::View::Seeker);

	setCentralWidget(part->view());
	createGUI(part);
	delete toolBar("mainToolBar");

	statusBar()->show();

	connect(part, SIGNAL(setWindowCaption(const QString &)), this, SLOT(updateTitle(const QString &)));
	connect(part->view(), SIGNAL(adaptSize(int, int)), this, SLOT(adaptSize(int, int)));

	setIcon(SmallIcon("kaboodle"));

	resize(320, minimumHeight());
	applyMainWindowSettings(KGlobal::config());
	menubarAction->setChecked(!menuBar()->isHidden());

	applySettings();

	if(!initialFile.isEmpty())
	{
		part->openURL(initialFile);
		propertiesAction->setEnabled(true);
	}

	show();
}

void Kaboodle::UserInterface::slotConfigureKeys()
{
	KKeyDialog dialog(this, 0);
	dialog.insert(actionCollection(), KaboodleFactory::instance()->aboutData()->programName() );
	dialog.insert(part->actionCollection(), i18n("Player") );
	View *view = static_cast<View*>( part->view() );
	dialog.insert(view->videoWidget()->actionCollection(), i18n("Video"));
	(void) dialog.configure();
}

Kaboodle::UserInterface::~UserInterface(void)
{
	saveMainWindowSettings(KGlobal::config());
}

void Kaboodle::UserInterface::fileOpen(void)
{
	KURL file(KFileDialog::getOpenURL(QString::null, KDE::PlayObjectFactory::mimeTypes().join(" "), this, i18n("Select File to Play")));
	if(file.isValid())
	{
		part->openURL(file);
		propertiesAction->setEnabled(true);
	}
}

void Kaboodle::UserInterface::dragEnterEvent(QDragEnterEvent *event)
{
    // accept uri drops only
    event->accept(KURLDrag::canDecode(event));
}

void Kaboodle::UserInterface::dropEvent(QDropEvent *event)
{
	KURL::List list;
	if (KURLDrag::decode(event, list))
	{
		if (!list.isEmpty())
			part->openURL(list.first());
	}
}

void Kaboodle::UserInterface::playerPreferences(void)
{
	Conf dlg(this);
	dlg.exec();
	applySettings();
}

void Kaboodle::UserInterface::applySettings(void)
{
	View *view = static_cast<View *>(part->view());
	KConfig &config = *KGlobal::config();
	config.setGroup("core");
	view->setAutoPlay(config.readBoolEntry("autoPlay", true));
	view->setQuitAfterPlaying(config.readBoolEntry("quitAfterPlaying", true));
}

void Kaboodle::UserInterface::showMenubar(void)
{
	if(menubarAction->isChecked())
		menuBar()->show();
	else
		menuBar()->hide();
}

void Kaboodle::UserInterface::updateTitle(const QString &text)
{
	setCaption(text);
	statusBar()->message(text);
}

void Kaboodle::UserInterface::properties(void)
{
	if(!part->currentURL().isEmpty())
		(void)new KPropertiesDialog(part->currentURL());
}

void Kaboodle::UserInterface::adaptSize(int newViewWidth, int newViewHeight)
{
	if(!newViewWidth) return;
	View *view = static_cast<View *>(part->view());
	int extraWidth = width() - view->width();
	int extraHeight = height() - view->height();
	resize(newViewWidth + extraWidth, newViewHeight + extraHeight);
}

#include "userinterface.moc"

