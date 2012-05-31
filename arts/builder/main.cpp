/*

	Copyright (C) 1998 - 2000 Stefan Westerfeld
                              stefan@space.twc.de

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

#include "structure.h"
#include "structureport.h"
#include "menumaker.h"
#include "session.h"
#include "dirmanager.h"
#include "moduleinfo.h"
#include "qiomanager.h"
#include "artsversion.h"
#include "propertypanel.h"
#include "module.h"
#include "autorouter.h"
#include "portposdlg.h"
#include "interfacedlg.h"
#include "execdlg.h"
#include "retrievedlg.h"

#include "config.h"

#include <kdebug.h>
#include <arts/debug.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kinputdialog.h>
#include <kstdaccel.h>
#include <kfiledialog.h>
#include <ksavefile.h>
#include <kmessagebox.h>
#include <kaboutdata.h>
#include <kstandarddirs.h>
#include <kartsserver.h>
#include <qfile.h>
#include <qpopupmenu.h>

#include <soundserver.h>

#include <list>
#include <iostream>

#include <unistd.h>
#include <string.h> // strerror always here?
//#include <errno.h>


using namespace std;

/*************************************************************/

class ArtsBuilderApp :public KApplication
{
protected:
	ArtsBuilderWindow *mainWindow;

public:
	ArtsBuilderApp();
	ArtsBuilderApp(QString filename);
	void start();
	void end();
};

/*************************************************************/

ArtsBuilderWindow::ArtsBuilderWindow(const char *name)
	: KDockMainWindow(0, name),
	  mainDock(0),
	  modulewidget(0),
	  propertyDock(0),
	  propertyPanel(0),
	  menumaker(0),
	  structure(0),
	  execDlg(0)
{
#if 0  /* PORT */
	ModuleBroker = Synthesizer->moduleBroker();
	assert(ModuleBroker);

//---- publish my widgets on the server ----

	GUIServer = new GUIServer_impl(ModuleBroker, Synthesizer);
    arts_debug("GUIServer:\n%s", ArtsOrb->object_to_string(GUIServer));
	GUIServer->incRef();

//---- trigger autoloading of all structures that are present in my dirs ----

	list<string> datadirs = PortableKDE::globalDirs("data");
	list<string>::iterator it;

	for(it = datadirs.begin(); it != datadirs.end(); ++it)
	{
		string common = *it;
		common += "artsbuilder";
		if(chdir(common.c_str()) == 0)
			ModuleBroker->addPublishingPath(common.c_str());
	}
	/*
	string common = (const char *)PortableKDE::globalDir("data");
	common += "/artsbuilder";
	arts_debug("%s", common.c_str());
	if(chdir(common.c_str()) == 0)
		ModuleBroker->addPublishingPath(common.c_str());
	*/

	// just make sure that the mapsDir exists
	(void)DirManager::mapDir();

	if(chdir(DirManager::structureDir()) == 0)	// retry
	{
		ModuleBroker->addPublishingPath(DirManager::structureDir());
		Synthesizer->addArtsDirectory(DirManager::baseDir());

	}
#endif

	arts_debug("PORT: structure");
	structure = new Structure();
	arts_debug("PORT: structure ok");
	//ModuleList = structure->getModuleList();

	mainDock = createDockWidget("mainDockWidget", 0, 0, "main_dock_widget");

	arts_debug("PORT: modulewidget");
	modulewidget = new ModuleWidget(structure, mainDock, "mwidget");
	mainDock->setWidget(modulewidget);
	connect(modulewidget, SIGNAL(modified(bool)), SLOT(setModified(bool)));
	arts_debug("PORT: modulewidget ok");

	// allow others to dock to the 4 sides
	mainDock->setDockSite(KDockWidget::DockCorner);
	// forbit docking abilities of module widget itself
	mainDock->setEnableDocking(KDockWidget::DockNone);

	setView(mainDock);
	setMainDockWidget(mainDock);

	propertyDock = createDockWidget("propertyDock", 0, 0, i18n("Port Properties"));

	propertyPanel = new PropertyPanel(propertyDock, "ppanel");

	propertyDock->setWidget(propertyPanel);
	propertyDock->manualDock(mainDock,                // dock target
							 KDockWidget::DockBottom, // dock site
							 80);                    // relation target/this (in percent)

	// selection
	connect(modulewidget,  SIGNAL(portSelected(ModulePort *)),
			propertyPanel, SLOT  (setSelectedPort(ModulePort *)));
	connect(propertyPanel, SIGNAL(portSelected(ModulePort *)),
			modulewidget,  SLOT  (selectPort(ModulePort *)));
	connect(modulewidget,  SIGNAL(componentSelected(StructureComponent *)),
			propertyPanel, SLOT  (setSelectedComponent(StructureComponent *)));

	// connection
	connect(propertyPanel, SIGNAL(startConnection(ModulePort *)),
			modulewidget,  SLOT  (startConnection(ModulePort *)));

	// port properties changed
	connect(propertyPanel, SIGNAL(portPropertiesChanged(ModulePort *)),
			modulewidget,  SLOT  (portPropertiesChanged(ModulePort *)));

	arts_debug("PORT: setcanvas");
	structure->setCanvas(modulewidget);
	arts_debug("PORT: setcanvas ok");

	mbroker_updateCount = 0;

	arts_debug("PORT: menumaker");
	menumaker = new MenuMaker(new KActionMenu(i18n("Modules"), actionCollection(), "modulesmenu"));
	//menumaker->addCategory("&Gui", "Gui_");
	menumaker->addCategory(i18n("&Synthesis"), "Arts::Synth_");
	menumaker->addCategory(i18n("&Synthesis/&Arithmetic + Mixing"), "Arts::Synth_ADD$");
	menumaker->addCategory(i18n("&Synthesis/&Arithmetic + Mixing"), "Arts::Synth_AUTOPANNER$");
	menumaker->addCategory(i18n("&Synthesis/&Arithmetic + Mixing"), "Arts::Synth_MUL$");
	menumaker->addCategory(i18n("&Synthesis/&Arithmetic + Mixing"), "Arts::Synth_DIV$");
	menumaker->addCategory(i18n("&Synthesis/&Arithmetic + Mixing"), "Arts::Synth_MULTI_ADD$");
	menumaker->addCategory(i18n("&Synthesis/&Arithmetic + Mixing"), "Arts::Synth_XFADE$");
	menumaker->addCategory(i18n("&Synthesis/&Busses"), "Arts::Synth_BUS_");
	menumaker->addCategory(i18n("&Synthesis/&Delays"), "Arts::Synth_DELAY$");
	menumaker->addCategory(i18n("&Synthesis/&Delays"), "Arts::Synth_CDELAY$");
	menumaker->addCategory(i18n("&Synthesis/&Envelopes"), "Arts::Synth_PSCALE$");
	menumaker->addCategory(i18n("&Synthesis/&Envelopes"), "Arts::Synth_ENVELOPE_");
	menumaker->addCategory(i18n("&Synthesis/Effe&cts"), "Arts::Synth_FREEVERB$");
	menumaker->addCategory(i18n("&Synthesis/Effe&cts"), "Arts::Synth_FX_");
	menumaker->addCategory(i18n("&Synthesis/Effe&cts"), "Arts::Synth_PITCH_SHIFT$");
	menumaker->addCategory(i18n("&Synthesis/Effe&cts"), "Arts::Synth_TREMOLO$");
	menumaker->addCategory(i18n("&Synthesis/&Filters"), "Arts::Synth_ATAN_SATURATE$");
	menumaker->addCategory(i18n("&Synthesis/&Filters"), "Arts::Synth_BRICKWALL_LIMITER$");
	menumaker->addCategory(i18n("&Synthesis/&Filters"), "Arts::Synth_MOOG_VCF");
	menumaker->addCategory(i18n("&Synthesis/&Filters"), "Arts::Synth_SHELVE_CUTOFF$");
	menumaker->addCategory(i18n("&Synthesis/&Filters"), "Arts::Synth_RC$");
	menumaker->addCategory(i18n("&Synthesis/&Filters"), "Arts::Synth_STD_EQUALIZER$");
	menumaker->addCategory(i18n("&Synthesis/&Midi + Sequencing"), "Arts::Synth_MIDI");
	menumaker->addCategory(i18n("&Synthesis/&Midi + Sequencing"), "Arts::Interface_MIDI");
	menumaker->addCategory(i18n("&Synthesis/&Midi + Sequencing"), "Arts::Synth_SEQUENCE$");
	menumaker->addCategory(i18n("&Synthesis/&Midi + Sequencing"), "Arts::Synth_SEQUENCE_FREQ$");
	menumaker->addCategory(i18n("&Synthesis/&Midi + Sequencing"), "Arts::Synth_STRUCT_KILL$");
	menumaker->addCategory(i18n("&Synthesis/Sam&ples "), "Arts::Synth_PLAY_");
	menumaker->addCategory(i18n("&Synthesis/&Sound IO"), "Arts::Synth_AMAN_");
	menumaker->addCategory(i18n("&Synthesis/&Sound IO"), "Arts::Synth_CAPTURE_WAV$");
	menumaker->addCategory(i18n("&Synthesis/&Sound IO"), "Arts::Synth_PLAY$");
	menumaker->addCategory(i18n("&Synthesis/&Sound IO"), "Arts::Synth_RECORD$");
	menumaker->addCategory(i18n("&Synthesis/&Sound IO"), "Arts::Synth_FULL_DUPLEX_");
	menumaker->addCategory(i18n("&Synthesis/&Sound IO"), "Arts::Synth_FILEPLAY");
	menumaker->addCategory(i18n("&Synthesis/&Tests"), "Arts::Synth_NIL$");
	menumaker->addCategory(i18n("&Synthesis/&Tests"), "Arts::Synth_DEBUG$");
	menumaker->addCategory(i18n("&Synthesis/&Tests"), "Arts::Synth_DATA$");
	menumaker->addCategory(i18n("&Synthesis/&Tests"), "Arts::Synth_MIDI_DEBUG$");
	menumaker->addCategory(i18n("&Synthesis/&Oscillation && Modulation"), "Arts::Synth_FREQUENCY$");
	menumaker->addCategory(i18n("&Synthesis/&Oscillation && Modulation"), "Arts::Synth_FM_SOURCE$");
	menumaker->addCategory(i18n("&Synthesis/&Oscillation && Modulation"), "Arts::Synth_OSC$");
	menumaker->addCategory(i18n("&Synthesis/&WaveForms"), "Arts::Synth_WAVE_");
	menumaker->addCategory(i18n("&Synthesis/&WaveForms"), "Arts::Synth_NOISE$");
	menumaker->addCategory(i18n("&Synthesis/&Internal"), "Arts::Synth_PARAM_");

	menumaker->addCategory(i18n("&Examples"), "example_");
	menumaker->addCategory(i18n("&Instruments"), "instrument_");
	menumaker->addCategory(i18n("&Mixer-Elements"), "mixer_element_");
	menumaker->addCategory(i18n("&Templates"), "template_");
	menumaker->addCategory(i18n("&Other"), "*");
	arts_debug("PORT: menumaker ok");

/*
	m_modules->insertItem(i18n("&Gui"), m_modules_gui);
	m_modules->insertItem(i18n("&Synthesis"), m_modules_synth);
	m_modules->insertItem(i18n("&Instruments"), m_modules_instruments);
	m_modules->insertItem(i18n("&Other"), m_modules_other);
 */

#if 000
	connect(menubar, SIGNAL(highlighted(int)), this, SLOT(activateMenu(int)));
	connect(m_view, SIGNAL(activated(int)), modulewidget, SLOT(setZoom(int)));
	connect(m_ports, SIGNAL(activated(int)), this, SLOT(addPort(int)));
	connect(m_file_new, SIGNAL(activated(int)), this, SLOT(fileNew(int)));

	//connect(m_modules, SIGNAL(activated(int)), this, SLOT(addModule(int)));
	/*
	connect(m_modules_synth, SIGNAL(activated(int)), this, SLOT(addModule(int)));
	connect(m_modules_gui, SIGNAL(activated(int)), this, SLOT(addModule(int)));
	connect(m_modules_instruments, SIGNAL(activated(int)), this, SLOT(addModule(int)));
	connect(m_modules_other, SIGNAL(activated(int)), this, SLOT(addModule(int)));
	*/
	connect(kapp, SIGNAL(lastWindowClosed()), this , SLOT(quit()));

	// update the modules menu once for the start
#endif

	arts_debug("PORT: activatemenu");
	connect(menumaker, SIGNAL(activated(const char *)), this, SLOT(addModule(const char *)));
	fillModuleMenu();
	arts_debug("PORT: activatemenu ok");
	setupActions();

	createGUI();

	// connect to aboutToShow to correctly show state of dockwidget there:
	QPopupMenu *viewmenu = (QPopupMenu*)factory()->container("view", this);
	if (viewmenu)
		connect(viewmenu, SIGNAL(aboutToShow()), this, SLOT(viewMenuAboutToShow()));
	else
		arts_debug("view menu not found!");

	m_filename = QString::null;
	setModified(false);

	installEventFilter(propertyPanel);
}

void ArtsBuilderWindow::setupActions()
{
	// File menu
	KStdAction::openNew(this, SLOT(fileNew()), actionCollection());

	(void)new KAction(i18n("Open Session..."), 0, this, SLOT(openSession()),
					  actionCollection(), "file_open_session");
	KStdAction::open(this, SLOT(open()), actionCollection());
	(void)new KAction(i18n("Open E&xample..."), Qt::CTRL + Qt::Key_X, this, SLOT(openExample()),
					  actionCollection(), "file_open_example");
	KStdAction::save(this, SLOT(save()), actionCollection());
	KStdAction::saveAs(this, SLOT(saveAs()), actionCollection());
	(void)new KAction(i18n("&Retrieve From Server..."), Qt::CTRL + Qt::Key_R, this, SLOT(retrieve()),
					  actionCollection(), "file_retrieve_from_server");
	(void)new KAction(i18n("&Execute Structure"), "artsbuilderexecute", Qt::CTRL + Qt::Key_E, this, SLOT(execute()),
					  actionCollection(), "file_execute_structure");
	(void)new KAction(i18n("&Rename Structure..."), Qt::CTRL + Qt::Key_R, this, SLOT(rename()),
					  actionCollection(), "file_rename_structure");
	(void)new KAction(i18n("&Publish Structure"), Qt::CTRL + Qt::Key_P, this, SLOT(publish()),
					  actionCollection(), "file_publish_structure");
	KStdAction::quit(this, SLOT(close()), actionCollection());

	// Edit menu
	(void)new KAction(i18n("&Delete"), Qt::Key_Delete, modulewidget, SLOT(delModule()),
					  actionCollection(), "edit_delete");
	KStdAction::selectAll(modulewidget, SLOT(selectAll()), actionCollection());

	// View menu
	viewPropertiesAction= new KToggleAction(i18n("&Property Panel"), 0,
											propertyDock, SLOT(changeHideShowState()),
											actionCollection(), "view_properties");
	(void)new KAction(i18n("200%"), 0, this, SLOT(viewAt200()),
					  actionCollection(), "view_200");
	(void)new KAction(i18n("150%"), 0, this, SLOT(viewAt150()),
					  actionCollection(), "view_150");
	(void)new KAction(i18n("100%"), 0, this, SLOT(viewAt100()),
					  actionCollection(), "view_100");
	(void)new KAction(i18n("50%"), 0, this, SLOT(viewAt50()),
					  actionCollection(), "view_50");

	// Ports menu
	(void)new KAction(i18n("Create IN Audio Signal"), 0, this, SLOT(createInAudioSignal()),
					  actionCollection(), "ports_create_in_audio_signal");
	(void)new KAction(i18n("Create OUT Audio Signal"), 0, this, SLOT(createOutAudioSignal()),
					  actionCollection(), "ports_create_out_audio_signal");
	(void)new KAction(i18n("Create IN String Property"), 0, this, SLOT(createInStringProperty()),
					  actionCollection(), "ports_create_in_string_property");
	(void)new KAction(i18n("Create IN Audio Property"), 0, this, SLOT(createInAudioProperty()),
					  actionCollection(), "ports_create_in_audio_property");
	(void)new KAction(i18n("Implement Interface..."), 0, this, SLOT(addInterface()),
					  actionCollection(), "ports_implement_interface");
	(void)new KAction(i18n("Change Positions/Names..."), 0, this, SLOT(changePortPositions()),
					  actionCollection(), "ports_change_positions");
}

void ArtsBuilderWindow::fillModuleMenu()
{
	long updateCount = 3;  /* PORT: automatic update of menues missing */

	if(updateCount != mbroker_updateCount)
	{
		mbroker_updateCount = updateCount;
		//---- query all available objects ----
		Arts::TraderQuery query;
		query.supports("Buildable", "true");
		vector<Arts::TraderOffer> *offers = query.query();

		menumaker->clear();
		//m_file_new->clear();

		vector<Arts::TraderOffer>::iterator i;
		long n = 1;		/* TODO:PORT: is this necessary? I think not */
		for(i = offers->begin(); i != offers->end(); ++i)
		{
			Arts::TraderOffer& offer = *i;
			string name = offer.interfaceName();
			menumaker->addItem(name.c_str(),n++);

			/* PORT: templates missing
			if(strncmp(name, "template_", strlen("template_")) == 0)
			{
				char *xname = strdup(&name[strlen("template_")]);
				int x;
				for(x = 0;xname[x] != 0; x++)
					if(xname[x] == '_') xname[x] = ' ';

				m_file_new->insertItem(xname, i);
			}
			*/
		}
		delete offers;
	}
#if 0
	if(0) /*item == modules_menu_item)  PORT!!! */
	{
		long updateCount = ModuleBroker->updateCount();

		// if the contents of the ModukeBroker changed, update our "modules"-Menu
		if(updateCount != mbroker_updateCount)
		{
			mbroker_updateCount = updateCount;
			//---- query all available objects ----
			ArtsCorba::StringSeq_var Modules = ModuleBroker->publishedModules();
			assert(Modules);

			menumaker->clear();
			m_file_new->clear();

			unsigned long i;
			for(i = 0; i < Modules->length();i++)
			{
				const char *name = (*Modules)[i];
				menumaker->addItem(name, i);

				if(strncmp(name, "template_", strlen("template_")) == 0)
				{
					char *xname = strdup(&name[strlen("template_")]);
					int x;
					for(x = 0;xname[x] != 0; x++)
						if(xname[x] == '_') xname[x] = ' ';

					m_file_new->insertItem(xname, i);
				}
			}
		}
	}
#endif
}

void ArtsBuilderWindow::quit()
{
	if(execDlg) return;
	arts_debug(">> ArtsBuilderWindow::quit() called");
	kapp->quit();
	arts_debug("<< leaving ArtsBuilderWindow::quit()");
}

ArtsBuilderWindow::~ArtsBuilderWindow()
{
	delete structure;
}

void ArtsBuilderWindow::viewMenuAboutToShow()
{
	viewPropertiesAction->setChecked(propertyDock->isVisible());
}

void ArtsBuilderWindow::publish()
{
	checkName();
	structure->publish();
	KMessageBox::information(this,
		i18n("The structure has been published as: '%1' on the server.").arg( structure->name().c_str() ));
}

QString ArtsBuilderWindow::getOpenFilename(const char *pattern, const char *initialDir)
{
	arts_debug(">>>>> getOpenFilename");
	QString filename = KFileDialog::getOpenFileName(initialDir, pattern, this);
	arts_debug(">>>>> opendlg closed");
	if(!filename.isEmpty())
	{
		arts_debug("open... %s", filename.local8Bit().data());

		// check that the file is ok:

		FILE *infile = fopen(QFile::encodeName(filename), "r");

		if(infile)
		{
			fclose(infile);
			return(filename);
		}
	}
	return QString("");
}

void ArtsBuilderWindow::fileNew()
{
	if(!promptToSave())
		return;

	propertyPanel->setSelectedComponent(0);
	structure->clear();
	modulewidget->reInit();
	m_filename = QString::null;
	setModified(false);
}

void ArtsBuilderWindow::open()
{
	if(!promptToSave())
		return;

	open(getOpenFilename("*.arts", DirManager::structureDir()));
}

void ArtsBuilderWindow::open(QString filename)
{
	if(!promptToSave())
		return;

	if(!filename.isEmpty())
	{
		structure->load(QFile::encodeName(filename));
		modulewidget->reInit();
		if(!structure->valid())
		{
			KMessageBox::sorry(this,
			i18n("The structure could not be loaded correctly. Maybe some of\n"
				 "the modules used in the file are not available in this\n"
				 "version of aRts."),
				i18n("Arts Warning"));
		}
		m_filename = filename;
		setModified(false);
		setCaption(m_filename);
	}
}

void ArtsBuilderWindow::openSession()
{
	if(!promptToSave())
		return;

	QString filename = getOpenFilename("*.arts-session", DirManager::sessionDir());

	if(!filename.isEmpty())
	{
		Session *session = new Session();
		session->loadSession(QFile::encodeName(filename));

		assert(!execDlg);
		execDlg = new ExecDlg(0, session);
		assert(execDlg);

		// this will create the widgets that will eventually get into the
		// execdlg
		session->startExecute();

		execDlg->start();
		execDlg->show();

		connect(execDlg, SIGNAL(ready()), this, SLOT(endexecute()));

		hide();
		// m_filename = filename; FIXME: DOESN'T THIS BELONG HERE?
		setModified(false);
	}
}

void ArtsBuilderWindow::openExample()
{
	if(!promptToSave())
		return;

	QString dir = locate("data", "artsbuilder/examples/");
	if(!dir)
		KMessageBox::sorry(
			this,
			i18n("Unable to find the examples folder.\nUsing the current folder instead."),
			i18n("aRts Warning"));

	open(getOpenFilename("*.arts", QFile::encodeName(dir)));
}

void ArtsBuilderWindow::saveAs()
{
	checkName();
	string defaultname = string(structure->name()) + string(".arts");

	chdir(DirManager::structureDir());
	KFileDialog *dlg = new KFileDialog(0, "*.arts", this, 0, true /*,false TODO: acceptURLs */);

	dlg->setSelection(defaultname.c_str());
	dlg->setCaption(i18n("Save As"));

	QString filename;
	if(dlg->exec() == QDialog::Accepted)
		filename = dlg->selectedFile();

	delete dlg;
	// QString filename = KFileDialog::getSaveFileName(0, "*.arts", this);
	// filename.detach();

	if(!filename.isEmpty())
		save(filename);
}

bool ArtsBuilderWindow::save(QString filename)
{
	arts_debug("trying to save structure as '%s'", filename.local8Bit().data());

	KSaveFile file(filename);

	if(file.status()) {
		KMessageBox::sorry(this,
			i18n("The file '%1' could not be opened for writing: %2")
			.arg(filename).arg(strerror(file.status())),
			i18n("aRts Warning"));
		return false;
	}

	structure->saveInto(file.fstream());

	if(!file.close()) {
		KMessageBox::sorry(this,
			i18n("Saving to file '%1' could not be finished correctly: %2")
			.arg(filename).arg(strerror(file.status())),
			i18n("aRts Warning"));
		return false;
	}

	// tell the server to rescan for structures
	Arts::SoundServerV2 server = KArtsServer().server();
	if(!server.isNull()) server.checkNewObjects();

	m_filename = filename;
	setModified(false);
	return true;
}

void ArtsBuilderWindow::save()
{
	if(m_filename.isEmpty())
		saveAs();
	else
		save(m_filename);
}

void ArtsBuilderWindow::checkName()
{
	if(strncmp(structure->name().c_str(), "template_", strlen("template_")) == 0)
		rename();
}

void ArtsBuilderWindow::rename()
{
    bool ok;

    QString name = KInputDialog::getText( i18n( "Rename Structure" ),
      i18n( "Enter structure name:" ), structure->name().c_str(), &ok, this );
	if (ok)
	{
		arts_debug("rename OK...");
		structure->rename(name.local8Bit());
	}

	setModified(true);
}

void ArtsBuilderWindow::retrieve()
{
	if(!promptToSave())
		return;

	RetrieveDlg rd(0);

	if(rd.exec())
	{
		QString result = rd.result();
		if(!result.isEmpty())
		{
			structure->retrieve(result.local8Bit());
			modulewidget->reInit();
		}
	}
	// maybe set m_filename to null or sth. here?
	setModified(true);
}

void ArtsBuilderWindow::execute()
{
	assert(structure);
	assert(!execDlg);
	execDlg = new ExecDlg(0, structure);
	assert(execDlg);

	// this will create the widgets that will eventually get into the
	// execdlg
	if(structure->startExecute())
	{
		execDlg->start();
		execDlg->show();

		connect(execDlg, SIGNAL(ready()), this, SLOT(endexecute()));

		hide();
	}
	else
	{
		delete execDlg;
		execDlg = 0;

		KMessageBox::sorry(this,
			i18n("Could not execute your structure. Make sure that the\n"
				 "sound server (artsd) is running.\n"), i18n("aRts Warning"));
	}
}

void ArtsBuilderWindow::endexecute()
{
	show();
	assert(execDlg);
	delete execDlg;
	// will be done by the execDlg itself now
	//structure->stopExecute();

	execDlg = 0;
}

void ArtsBuilderWindow::oldFileNewWhatTheHellDoesItDo(int what)
{
	if(!promptToSave())
		return;

	const char *name = menumaker->findID(what);
	assert(name);
	structure->retrieve(name);
	modulewidget->reInit();
	setModified(false);
}

void ArtsBuilderWindow::createInAudioSignal()
{
	// data that goes into the structure
	modulewidget->addPort(Arts::PortType(Arts::output, "float", Arts::conn_stream, false));
	setModified(true);
}

void ArtsBuilderWindow::createOutAudioSignal()
{
	// data that goes out of the structure
	modulewidget->addPort(Arts::PortType(Arts::input, "float", Arts::conn_stream, false));
	setModified(true);
}

void ArtsBuilderWindow::createInStringProperty()
{
	// data that goes into the structure
	modulewidget->addPort(Arts::PortType(Arts::output, "string", Arts::conn_property, false));
	setModified(true);
}

void ArtsBuilderWindow::createInAudioProperty()
{
	// data that goes into the structure
	modulewidget->addPort(Arts::PortType(Arts::output, "float", Arts::conn_property, false));
	setModified(true);
}

void ArtsBuilderWindow::changePortPositions()
{
	PortPosDlg *ppd = new PortPosDlg(this, structure);
	ppd->exec();
	setModified(true);
	// XXX: delete ppd?
}

void ArtsBuilderWindow::addInterface()
{
	InterfaceDlg *ifd = new InterfaceDlg(this);
	ifd->exec();

	Arts::ModuleInfo minfo = makeModuleInfo(ifd->interfaceName());
	if(!minfo.name.empty())
		modulewidget->addInterface(minfo);

	delete ifd;
}

void ArtsBuilderWindow::viewAt50()
{
	modulewidget->setZoom(50);
}

void ArtsBuilderWindow::viewAt100()
{
	modulewidget->setZoom(100);
}

void ArtsBuilderWindow::viewAt150()
{
	modulewidget->setZoom(150);
}

void ArtsBuilderWindow::viewAt200()
{
	modulewidget->setZoom(200);
}

void ArtsBuilderWindow::addModule(const char *name)
{
	arts_return_if_fail (name != 0);

	arts_debug("addModule(%s)", name);
	Arts::ModuleInfo minfo = makeModuleInfo(name);

	if(!minfo.name.empty())
		modulewidget->addModule(minfo);
#if 0
	const char *name = menumaker->findID(module);
	assert(name);

	arts_debug("selected (%s) (module=%d)", name, module);

	ArtsCorba::ModuleBroker_var ModuleBroker = Synthesizer->moduleBroker();
	ArtsCorba::ModuleInfo_var minfo = ModuleBroker->lookupModule(name);


	if(minfo)
	{
		modulewidget->addModule(minfo);

/*
		Module *m = structure->createModule(minfo);
		modulewidget->addModule(m);
*/
	}
#endif
	setModified(true);
}

bool ArtsBuilderWindow::isModified()
{
	return modified;
}

void ArtsBuilderWindow::setModified(bool m)
{
	modified = m;
	setCaption(m_filename, modified);
	actionCollection()->action(KStdAction::stdName(KStdAction::Save))->setEnabled(modified);
}

bool ArtsBuilderWindow::queryClose()
{
	return promptToSave();
}

bool ArtsBuilderWindow::promptToSave()
{
	bool result;
	int query;

	if(!isModified())
		return true;

	query = KMessageBox::warningYesNoCancel(this,
		i18n("The current structure has been modified.\nWould you like to save it?"), QString::null, KStdGuiItem::save(), KStdGuiItem::discard());

	result = false;
	switch(query)
	{
	  case KMessageBox::Yes:
		  save();
		  result = !modified;
		  break;
	  case KMessageBox::No:
		  result = true;
		  setModified(false);
		  break;
	  case KMessageBox::Cancel:
		  break;
	}
	return result;
}

/*************************************************************/

ArtsBuilderApp::ArtsBuilderApp()
{
	start();
}

ArtsBuilderApp::ArtsBuilderApp(QString filename)
{
	start();
	if(QFile::exists(filename))
	{
		mainWindow->open(filename);
	} else {
		KMessageBox::sorry(0,
		   i18n("The specified file '%1' does not exist.").arg(filename),
		   i18n("aRts Warning"));
	}
}

void ArtsBuilderApp::start()
{
	arts_debug("PORT: mainWindow");
	mainWindow = new ArtsBuilderWindow("main");

	arts_debug("PORT: mainWindow ok");
	mainWindow->resize(680, 500);
	arts_debug("PORT: mainWindow show");
	mainWindow->show();
	arts_debug("PORT: mainWindow show ok");

#if 0 /* PORT */
	ArtsCorba::ModuleBroker_var ModuleBroker = theSynthesizer->moduleBroker();
	assert(ModuleBroker);
#endif

	setTopWidget(mainWindow);
}

void ArtsBuilderApp::end()
{
	delete mainWindow;
}

/*************************************************************/

static KCmdLineOptions options[] =
{
	{ "+[file]", I18N_NOOP("Optional .arts file to be loaded"), 0 },
	KCmdLineLastOption
};

#ifdef COMMON_BINARY
int artsbuilder_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	KAboutData aboutData("artsbuilder",
						 I18N_NOOP("artsbuilder"),
						 ARTS_VERSION,
						 I18N_NOOP("aRts synthesizer designer"),
						 KAboutData::License_GPL,
						 "(C) 1998-2001, Stefan Westerfeld",
						 I18N_NOOP("The analog real-time synthesizer graphical design tool."),
						 "http://www.arts-project.org/",
						 "submit@bugs.kde.org");

	aboutData.addAuthor("Stefan Westerfeld", I18N_NOOP("Author"), "stefan@twc.de");
	aboutData.addCredit("Waldo Bastian", 0, "bastian@kde.org");
	aboutData.addCredit("Jens Hahn", 0, "Jens.Hahn@t-online.de");
	aboutData.addCredit("Martin Lorenz", 0, "lorenz@ch.tum.de");
	aboutData.addCredit("Hans Meine", 0, "hans_meine@gmx.net");
	aboutData.addCredit("Jeff Tranter", 0, "tranter@pobox.com");

	KCmdLineArgs::init(argc, argv, &aboutData);
	KCmdLineArgs::addCmdLineOptions(options);
	Arts::QIOManager iomanager;
	Arts::Dispatcher dispatcher(&iomanager);

	Arts::ObjectManager::the()->provideCapability("kdegui");

	// check for one optional filename argument
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	if(args->count() > 1) {
		args->usage("");
	}
	if(args->count() > 0)
	{
		ArtsBuilderApp Application(QFile::decodeName(args->arg(0)));
		args->clear();
		return Application.exec();
	} else {
		ArtsBuilderApp Application;
		args->clear();
		return Application.exec();
	}
}
#include "main.moc"
