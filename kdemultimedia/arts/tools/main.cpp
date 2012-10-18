/*

    Copyright (C) 2000-2001 Stefan Westerfeld
                            <stefan@space.twc.de>
                       2003 Arnold Krille <arnold@arnoldarts.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    */

#include "main.h"
#include "environmentview.h"
#include "main.moc"
#include "levelmeters.h"
#include "midimanagerview.h"
#include "audiomanager.h"
#include "fftscopeview.h"
#include "mediatypesview.h"
#include "statusview.h"

#include <objectmanager.h>
#include <debug.h>
#include <artsversion.h>
#include <kartsfloatwatch.h>

#include <qlabel.h>
#include <qslider.h>
#include <qpushbutton.h>
#include <kaction.h>
#include <kled.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kartsserver.h>

#include <kstdaction.h>

#include "kartswidget.h"

using namespace std;
using namespace Arts;

void FreeVerbView::closeEvent(QCloseEvent *e) {
	e->accept();
	emit closed();
}

FreeVerbView::FreeVerbView(SimpleSoundServer server) : server(server) {
	freeverb = DynamicCast(server.createObject("Arts::Synth_FREEVERB"));
	arts_assert(!freeverb.isNull());
	freeverb.start();

	// put it into the effect chain
	effectID = server.outstack().insertTop(freeverb,"FreeVerb");

	GenericGuiFactory factory;
	Widget gui = factory.createGui(freeverb);

	QBoxLayout * l = new QHBoxLayout( this );
	if(!gui.isNull())
		l->add(new KArtsWidget(gui, this));
	else
		l->add(new QLabel(i18n("No GUI found for this effect."), this));
	l->freeze();
	show();
}

FreeVerbView::~FreeVerbView() {
	// remove effect
	server.outstack().remove(effectID);
}


VControl::VControl( KArtsServer* artsserver, QWidget *parent) : QFrame(parent)
	, freeVerbView(0)
	, server( artsserver )
{

	connect( server, SIGNAL( restartedServer() ), this, SLOT( initaRtsConnections() ) );
/*   	if(server.isNull())
   	{
		KMessageBox::error( 0, i18n("Connection to the soundserver failed - make sure that artsd is really running and that your kdelibs version is not older than kdemultimedia."));
		exit(1);
	}*/
	boxLayout = new QHBoxLayout( this );

	// 0   => 4.0
	// 200 => 2.0
	// 400 => 1.0
	// 600 => 0.5
	// 800 => 0.25
	// 1000 => 0.125
	// 1200 => 0.0 (forced)

	svcguiw = new KArtsWidget( this );
	boxLayout->addWidget( svcguiw );

	boxLayout->activate();
	show();

	initaRtsConnections();
}
VControl::~VControl() {
	if ( freeVerbView ) showFreeVerbView();
}

void VControl::useOldVolumeBar(int old) {
/*	delete stereoMeter;
	if (old) {
		stereoMeter= new StereoLedMeters(this);
	} else {
		stereoMeter= new PeakLevelMeters(this);
	}
	boxLayout->insertWidget(0, stereoMeter);
	stereoMeter->show();*/
	if ( old ) {
		svcgui.left().substyle( 3 );
		svcgui.right().substyle( 3 );
	} else {
		svcgui.left().substyle( 2 );
		svcgui.right().substyle( 2 );
	}
}

void VControl::showFreeVerbView() {
	if(!freeVerbView) {
		freeVerbView = new FreeVerbView(server->server());
		connect(freeVerbView,SIGNAL(closed()),this,SLOT(showFreeVerbView()));
	} else {
		delete freeVerbView;
		freeVerbView = 0;
	}
}

void VControl::initaRtsConnections() {
kdDebug() << k_funcinfo << endl;
	svcgui = Arts::StereoVolumeControlGui::null();
	svcgui = Arts::StereoVolumeControlGui( server->server().outVolume() );
	svcgui.title( i18n( "aRts Master Volume" ).utf8().data() );
	svcguiw->setContent( Arts::Widget() );
	svcguiw->setContent( svcgui );
	useOldVolumeBar( false );
kdDebug() << k_funcinfo << "done." << endl;
}

void MainWindow::toggleVolumeBar() {
	vc->useOldVolumeBar(showOldVolumeDisplay->isChecked());
}

MainWindow::MainWindow() : KMainWindow(0), kartsserver( new KArtsServer( this ) ) {
kdDebug() << k_funcinfo << endl;
	connect( kartsserver, SIGNAL( restartedServer() ), this, SLOT( serverRestarted() ) );

	vc = new VControl( kartsserver, this );
	setCentralWidget( vc );
	artsactions = new ArtsActions( kartsserver, actionCollection(), this );

	( void ) artsactions->actionScopeView();
	( void ) artsactions->actionAudioManager();
	( void ) artsactions->actionArtsStatusView();
	( void ) artsactions->actionMidiManagerView();
	( void ) artsactions->actionEnvironmentView();
	( void ) artsactions->actionMediaTypesView();
	( void ) new KAction( i18n("Toggle Free&Verb"), 0, vc, SLOT( showFreeVerbView() ), actionCollection(), "view_freeverb" );
	showOldVolumeDisplay=
		new KToggleAction( i18n( "Old aRts-Control-Style for VU-Meter" /*"&LED-Style Volume Display"*/ ), 0, this,
		SLOT( toggleVolumeBar() ), actionCollection(), "old_volume_display" );
	( void ) KStdAction::quit( this, SLOT( close() ), actionCollection(), "quit_artscontrol" );

	createGUI("artscontrol.rc");
	resize(20,300);
	show();
}

void MainWindow::serverRestarted() {
kdDebug() << k_funcinfo << endl;
	KMessageBox::sorry( this, "aRts had to restart!" );
}

int main(int argc, char **argv) {
	KAboutData aboutData( "artscontrol", I18N_NOOP("aRts control"),
		ARTS_VERSION, I18N_NOOP("Control tool for the aRts server"),
			KAboutData::License_GPL, I18N_NOOP("(c) 2000 Stefan Westerfeld\n(c) 2003 Arnold Krille") );
	aboutData.addAuthor( "Stefan Westerfeld", I18N_NOOP( "Author and aRts maintainer" ), "stefan@space.twc.de" );
	aboutData.addAuthor( "Arnold Krille", I18N_NOOP( "Some improvements" ), "arnold@arnoldarts.de" );

	KCmdLineArgs::init( argc, argv, &aboutData );

	KApplication app(argc, argv);

	// setup mcop communication
	QIOManager qiomanager;
	Dispatcher dispatcher(&qiomanager);

	ObjectManager::the()->provideCapability("kdegui");

	app.setMainWidget(new MainWindow);
	app.setName("artsbuilder");
	return app.exec();
}
