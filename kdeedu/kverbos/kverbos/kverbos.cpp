/***************************************************************************
                          kverbos.cpp  -  description
                             -------------------
    begin                : Fre Dez 14 19:28:09 CET 2001
    copyright            : (C) 2001 by Arnold Kraschinski
    email                : arnold.k67@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// include files for QT
#include <qprinter.h>
#include <qcheckbox.h>
#include <qradiobutton.h>

// include files for KDE
#include <kfiledialog.h>
#include <kinputdialog.h>
#include "kstartuplogo.h"
#include <kstandarddirs.h>
#include <kstatusbar.h>

// application specific includes
#include "kverbos.h"
#include "kverbosview.h"

// my includes
#include "kerfassen.h"
#include "kverbedit.h"
#include "kverbosoptions.h"
#include "kresult.h"
#include "kfeedercontrol.h"
#include "prefs.h"

// ID Nummern für die Statuszeile
#define ID_STATUS_MSG 1
#define ID_TRAINED_MSG 2
#define ID_CORRECT_MSG 3
#define ID_VERBS_MSG 4
#define ID_USERNAME_MSG 5

int KVerbosApp::kverbosCounter = 0;

KVerbosApp::KVerbosApp(QWidget* , const char* name):KMainWindow (0, name)
{
  // als erstes das Logo starten
  KStartupLogo* start_logo = new KStartupLogo(0);
  start_logo->show();
  start_logo->raise();

  // die Instanzen von kverbos zählen
  kverbosCounter++;

  config=kapp->config();

  ///////////////////////////////////////////////////////////////////
  // call inits to invoke all other construction parts
  initStatusBar();
  initActions();
  initDocument();
  initView();

  readOptions();

  ///////////////////////////////////////////////////////////////////
  // disable actions at startup
  // Mir ist nicht klar, warum die Autoren des Frameworks diese
  // Menuoptionen deaktivieren.
  // Ich werde hier jedenfalls gleich einmal einige auskommentieren.
  // fileSave->setEnabled(false);
  // fileSaveAs->setEnabled(false);
  filePrint->setEnabled(false);
  editCut->setEnabled(false);
  editCopy->setEnabled(false);
  editPaste->setEnabled(false);
  userResults->setEnabled(false);

  // El programa ha leído las opciones del programa. Entonces puedo iniciar el objeto
  // por la control de KFeeder.
  if (useKFeeder)
  {
    kfeederControl = new KFeederControl(kverbosCounter, showKFeederSplash, automaticKFeeder);
    useKFeeder = kfeederControl->isActive();
    if (!useKFeeder)
    {
      // the KFeeder didn't start so it isn't installed. Delete the control object
      delete kfeederControl;
      kfeederControl = 0;
    };
  }
  else
    kfeederControl = 0;

  // das Logo wieder löschen
  delete start_logo;
}

KVerbosApp::~KVerbosApp()
{
	// den Instanzenzähler zurücksetzen
	kverbosCounter--;
	if (kfeederControl)
	{
		disconnect(view, SIGNAL(informKFeeder(int)), this, SLOT(slotFeedIt(int)));
		delete kfeederControl;
		kfeederControl = 0;
	};
}

void KVerbosApp::initActions()
{
  fileNewWindow = new KAction(i18n("New &Window"), "window_new.png", 0, this, SLOT(slotFileNewWindow()), actionCollection(),"file_new_window");
  fileNew = KStdAction::openNew(this, SLOT(slotFileNew()), actionCollection());
  fileOpen = KStdAction::open(this, SLOT(slotFileOpen()), actionCollection());
  fileOpenRecent = KStdAction::openRecent(this, SLOT(slotFileOpenRecent(const KURL&)), actionCollection());
  fileOpenStandard = new KAction(i18n("Open &Standard Verb File"), "fileopen.png", 0, this, SLOT(slotFileOpenStandard()), actionCollection(),"file_open_standard");
  fileSave = KStdAction::save(this, SLOT(slotFileSave()), actionCollection());
  fileSaveAs = KStdAction::saveAs(this, SLOT(slotFileSaveAs()), actionCollection());
  fileClose = KStdAction::close(this, SLOT(slotFileClose()), actionCollection());
  filePrint = KStdAction::print(this, SLOT(slotFilePrint()), actionCollection());
  fileQuit = KStdAction::quit(this, SLOT(slotFileQuit()), actionCollection());
  editCut = KStdAction::cut(this, SLOT(slotEditCut()), actionCollection());
  editCopy = KStdAction::copy(this, SLOT(slotEditCopy()), actionCollection());
  editPaste = KStdAction::paste(this, SLOT(slotEditPaste()), actionCollection());
  editErfassen = new KAction(i18n("E&nter New Verb..."), 0, 0, this, SLOT(slotEditErfassen()), actionCollection(),"edit_erfassen");
  editBearbeiten = new KAction(i18n("&Edit Verb List..."), 0, 0, this, SLOT(slotEditBearbeiten()), actionCollection(), "edit_bearbeiten");

  settingOptions = new KAction(i18n("&Configure KVerbos..."), 0, 0, this, SLOT(slotSettingsOptions()), actionCollection(), "settings_options");
  userUsername = new KAction(i18n("&Username..."), 0, 0, this, SLOT(slotSettingsUsername()), actionCollection(), "user_username");
  userResults = new KAction(i18n("&Results"), 0, 0, this, SLOT(slotUserResults()), actionCollection(), "user_results");
 
  fileNewWindow->setToolTip(i18n("Opens a new application window"));
  fileOpenStandard->setToolTip(i18n("Opens the standard KVerbos verb file"));
  fileNew->setToolTip(i18n("Creates a new document"));
  fileOpen->setToolTip(i18n("Opens an existing document"));
  fileOpenRecent->setToolTip(i18n("Opens a recently used file"));
  fileSave->setToolTip(i18n("Saves the actual document"));
  fileSaveAs->setToolTip(i18n("Saves the actual document as..."));
  fileClose->setToolTip(i18n("Closes the actual document"));
  filePrint ->setToolTip(i18n("Prints out the actual document"));
  fileQuit->setToolTip(i18n("Quits the application"));
  editCut->setToolTip(i18n("Cuts the selected section and puts it to the clipboard"));
  editCopy->setToolTip(i18n("Copies the selected section to the clipboard"));
  editPaste->setToolTip(i18n("Pastes the clipboard contents to actual position"));
  editErfassen->setToolTip(i18n("Add new verbs."));
  editBearbeiten->setToolTip(i18n("Edit the list of verbs."));
  settingOptions->setToolTip(i18n("Change some options of the program"));
  userUsername->setToolTip(i18n("Enter your name as the username"));
  userResults->setToolTip(i18n("These are your latest results."));

  // icons
  editBearbeiten->setIcon("edit.png");
  editErfassen->setIcon("editclear.png");
  settingOptions->setIcon("configure.png");
  userUsername->setIcon("kverbosuser.png");
  userResults->setIcon("kverbosuser.png");
  
  setupGUI();
}


void KVerbosApp::initStatusBar()
{
  ///////////////////////////////////////////////////////////////////
  // STATUSBAR
  // TODO: add your own items you need for displaying current application status.
  statusBar()->insertItem(i18n("Ready."), ID_STATUS_MSG);
  statusBar()->insertItem(i18n("Trained: 0"), ID_TRAINED_MSG);
  statusBar()->insertItem(i18n("Correct: 0"), ID_CORRECT_MSG);
  statusBar()->insertItem(i18n("Number of verbs: 0"), ID_VERBS_MSG);
  statusBar()->insertItem(i18n("User: nobody"), ID_USERNAME_MSG);
}

void KVerbosApp::initDocument()
{
  doc = new KVerbosDoc(this);
  doc->newDocument();
  connect(doc, SIGNAL(anzahlVerbenGeaendert(int)), this, SLOT(slotStatusVerbMsg(int)));
  connect(this, SIGNAL(testAnzahl(int)), this, SLOT(slotStatusVerbMsg(int)));
}

void KVerbosApp::initView()
{
  ////////////////////////////////////////////////////////////////////
  // create the main widget here that is managed by KTMainWindow's view-region and
  // connect the widget to your document to display document contents.

  view = new KVerbosView(this);
  doc->addView(view);
  setCentralWidget(view);	
  setCaption(doc->URL().fileName(),false);
  //
  connect(view, SIGNAL(numberTrainedChanged(int)), this, SLOT(slotStatusTrainedMsg(int)));
  connect(view, SIGNAL(numberCorrectChanged(int)), this, SLOT(slotStatusCorrectMsg(int)));
  connect(view, SIGNAL(informKFeeder(int)), this, SLOT(slotFeedIt(int)));
}

void KVerbosApp::openDocumentFile(const KURL& url)
{
  slotStatusMsg(i18n("Opening file..."));

  doc->openDocument( url);
  fileOpenRecent->addURL( url );
  slotStatusMsg(i18n("Ready."));
}


KVerbosDoc* KVerbosApp::getDocument() const
{
  return doc;
}

void KVerbosApp::saveOptions()
{
  fileOpenRecent->saveEntries(config,"Recent Files");

  // Meine eigenen Optionen
  // Die zum Lernen ausgewählten Zeiten
  config->setGroup("tiempos");
  QString key;
  for (int i = presente; i<MAX_TIEMPOS; i++)
  {
    key.setNum(i);
    QString options = timeOptions[i][0] ? "true " : "false ";
    options = timeOptions[i][1] ? options + "true " : options + "false ";
    options = timeOptions[i][2] ? options + "true" : options + "false";
    config->writeEntry(key, options);
  };
  // den Korrekturmodus abspeichern
  config->setGroup("correction mode");
  config->writeEntry("strict", correctStrict);
  // Abspeichern, ob der KFeeder benutzt werden soll
  config->setGroup("use of KFeeder");
  config->writeEntry("kfeeder", useKFeeder);
  config->writeEntry("kfeederSplash", showKFeederSplash);
  config->writeEntry("kfeederAutomatic", automaticKFeeder);
}


void KVerbosApp::readOptions()
{
  // initialize the recent file list
  fileOpenRecent->loadEntries(config,"Recent Files");

  // Meine eigenen Optionen
  // Die zum Lernen ausgewählten Zeiten
  config->setGroup("tiempos");
  QString key;
  for (int i = presente; i<MAX_TIEMPOS; i++)
  {
    key.setNum(i);
    QString options = config->readEntry ( key, "true false false");
    int pos1 = 0;
    int pos2 = options.find (' ', pos1, false);
    timeOptions[i][0] = (options.left(pos2) == "true") ? true : false;
    pos1 =  pos2 + 1;
    pos2 = options.find (' ', pos1, false);
    timeOptions[i][1] = (options.mid(pos1,pos2-pos1) == "true") ? true : false;
    timeOptions[i][2] = (options.right(options.length()-pos2-1) == "true") ? true : false;
  };
  // Solange einige Zeiten nicht fertig sind werden die hier auf false gesetzt.
  /*TODO*/
  for (int i = subPresente; i<MAX_TIEMPOS; i++)
  {
    timeOptions[i][0] = false;
    timeOptions[i][1] = false;
    timeOptions[i][2] = false;
  };
  // Korrekturmodus
  config->setGroup("correction mode");
  correctStrict = config->readBoolEntry("strict", false);
  // Nutzung des KFeeders
  config->setGroup("use of KFeeder");
  useKFeeder = config->readBoolEntry("kfeeder", true);
  showKFeederSplash = config->readBoolEntry("kfeederSplash", false);
  automaticKFeeder = config->readBoolEntry("kfeederAutomatic", true);
}

void KVerbosApp::saveProperties(KConfig *_cfg)
{
  if(doc->URL().fileName()!=i18n("Untitled") && !doc->isModified())
  {
    // saving to tempfile not necessary

  }
  else
  {
    KURL url=doc->URL();	
    _cfg->writePathEntry("filename", url.url());
    _cfg->writeEntry("modified", doc->isModified());
    QString tempname = kapp->tempSaveName(url.url());
    QString tempurl= KURL::encode_string(tempname);
    KURL _url(tempurl);
    doc->saveDocument(_url);
  }
}


void KVerbosApp::readProperties(KConfig* _cfg)
{
  QString filename = _cfg->readPathEntry("filename");
  KURL url(filename);
  bool modified = _cfg->readBoolEntry("modified", false);
  if(modified)
  {
    bool canRecover;
    QString tempname = kapp->checkRecoverFile(filename, canRecover);
    KURL _url(tempname);
    if(canRecover)
    {
      doc->openDocument(_url);
      doc->setModified();
      setCaption(_url.fileName(),true);
      QFile::remove(tempname);
    }
  }
  else
  {
    if(!filename.isEmpty())
    {
      doc->openDocument(url);
      setCaption(url.fileName(),false);
    }
  }
}

bool KVerbosApp::queryClose()
{
  return doc->saveModified();
}

bool KVerbosApp::queryExit()
{
  saveOptions();
  return true;
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void KVerbosApp::slotFileNewWindow()
{
  slotStatusMsg(i18n("Opening a new application window..."));

  KVerbosApp *new_window= new KVerbosApp();
  new_window->show();

  slotStatusMsg(i18n("Ready."));
}

void KVerbosApp::slotFileNew()
{
  slotStatusMsg(i18n("Creating new document..."));

  if(!doc->saveModified())
  {
    // here saving wasn't successful

  }
  else
  {
    doc->newDocument();		
    setCaption(doc->URL().fileName(), false);
  }

  slotStatusMsg(i18n("Ready."));
}

void KVerbosApp::slotFileOpen()
{
  slotStatusMsg(i18n("Opening file..."));

  if(!doc->saveModified())
  {
     // here saving wasn't successful

  }
  else
  {
    KURL url=KFileDialog::getOpenURL(QString::null,
        i18n("*|All Files"), this, i18n("Open File"));
    if(!url.isEmpty())
    {
      doc->openDocument(url);
      // Die zu bearbeitende Datei in dem Dokument setzen
      doc->setURL(url);
      setCaption(url.fileName(), false);
      fileOpenRecent->addURL( url );
    }
  }
  slotStatusMsg(i18n("Ready."));
}

void KVerbosApp::slotFileOpenRecent(const KURL& url)
{
  slotStatusMsg(i18n("Opening file..."));

  if(!doc->saveModified())
  {
     // here saving wasn't successful
  }
  else
  {
    doc->openDocument(url);
    doc->setURL(url);
    setCaption(url.fileName(), false);
  }

  slotStatusMsg(i18n("Ready."));
}

void KVerbosApp::slotFileOpenStandard()
{
	slotStatusMsg(i18n("Opening file..."));
	QString userLanguage = Prefs::userLanguage();
	if(!doc->saveModified())
	{
		// here saving wasn't successful

	}
	else
	{
		KURL url(locate("appdata", "data/"+userLanguage+"/verbos.verbos"));
		if(!url.isEmpty())
		{
			doc->openDocument(url);
			// Die zu bearbeitende Datei in dem Dokument setzen
			doc->setURL(url);
			setCaption(url.fileName(), false);
			fileOpenRecent->addURL( url );
		}
	}
	slotStatusMsg(i18n("Ready."));
}

void KVerbosApp::slotFileSave()
{
  slotStatusMsg(i18n("Saving file..."));
  if (doc->URL().fileName() == i18n("Untitled"))
  {
    // Hier liegt ein Fall vor, der zu "Save as ... " gehört.
    slotFileSaveAs();
  }
  else
  {
    doc->saveDocument(doc->URL());
  };
  slotStatusMsg(i18n("Ready."));
}

void KVerbosApp::slotFileSaveAs()
{
  slotStatusMsg(i18n("Saving file with a new filename..."));

  KURL url=KFileDialog::getSaveURL(QDir::currentDirPath(),
        i18n("*|All Files"), this, i18n("Save As"));
  if(!url.isEmpty())
  {
    doc->saveDocument(url);
    fileOpenRecent->addURL(url);
    setCaption(url.fileName(),doc->isModified());
  }

  slotStatusMsg(i18n("Ready."));
}

void KVerbosApp::slotFileClose()
{
  slotStatusMsg(i18n("Closing file..."));

  close();

  slotStatusMsg(i18n("Ready."));
}

void KVerbosApp::slotFilePrint()
{
  slotStatusMsg(i18n("Printing..."));

  QPrinter printer;
  if (printer.setup(this))
  {
    view->print(&printer);
  }

  slotStatusMsg(i18n("Ready."));
}

void KVerbosApp::slotFileQuit()
{
	slotStatusMsg(i18n("Exiting..."));
	saveOptions();
	// close the first window, the list makes the next one the first again.
	// This ensures that queryClose() is called on each window to ask for closing
	KMainWindow* w;
	// In der Dokumentation konnte ich leider nicht finden, wozu diese memberliste gut ist.
	if(memberList)
	{
		for(w=memberList->first(); w!=0; w=memberList->next())
		{
			// only close the window if the closeEvent is accepted. If the user presses Cancel on the saveModified() dialog,
			// the window and the application stay open.
			if(!w->close())
				break;
		}
	}
	slotStatusMsg(i18n("Ready."));
}

void KVerbosApp::slotEditCut()
{
  slotStatusMsg(i18n("Cutting selection..."));

  slotStatusMsg(i18n("Ready."));
}

void KVerbosApp::slotEditCopy()
{
  slotStatusMsg(i18n("Copying selection to clipboard..."));

  slotStatusMsg(i18n("Ready."));
}

void KVerbosApp::slotEditPaste()
{
  slotStatusMsg(i18n("Inserting clipboard contents..."));

  slotStatusMsg(i18n("Ready."));
}

void KVerbosApp::slotStatusMsg(const QString &text)
{
  ///////////////////////////////////////////////////////////////////
  // change status message permanently
  statusBar()->clear();
  statusBar()->changeItem(text, ID_STATUS_MSG);
}

// Gibt die Anzahl der Verben in der Statuszeile aus
void KVerbosApp::slotStatusVerbMsg(int n)
{
  QString s;
  s.setNum(n,10);
  s = i18n("Number of verbs: ") + s;
  statusBar()->changeItem(s, ID_VERBS_MSG);
}

// Gibt die Anzahl der geübten Verben in der Statuszeile aus.
void KVerbosApp::slotStatusTrainedMsg(int n)
{
  QString s;
  s.setNum(n,10);
  s = i18n("Trained: ") + s;
  statusBar()->changeItem(s, ID_TRAINED_MSG);
}

// Gibt die Anzahl der richtig geübten Verben aus.
void KVerbosApp::slotStatusCorrectMsg(int n)
{
  QString s;
  s.setNum(n,10);
  s = i18n("Correct: ") + s;
  statusBar()->changeItem(s, ID_CORRECT_MSG);
}

// öffnet einen Dialog mit dem neue Verben erfasst werden können.
void KVerbosApp::slotEditErfassen()
{
	spanishVerbList* pList = getDocument()->getList();
	KErfassen* pDlgErfassen = new KErfassen(*pList, -1, this, "erfassen", true);
	pDlgErfassen->exec();
	if (pDlgErfassen->wasChanged())
	{
		*pList = pDlgErfassen->getList();
		doc->setModified();
	};
	delete pDlgErfassen;
	pDlgErfassen = 0;
	// Anzahl in der Statuszeile anpassen
	emit testAnzahl(doc->getAnzahl());
}

// öffnet einen Dialog mit dem die Verbliste bearbeitet werden kann.
void KVerbosApp::slotEditBearbeiten()
{
	KVerbEdit* pDlgEdit = new KVerbEdit(*(getDocument()->getList()), this, "edit the verbs", true);
	pDlgEdit->exec();
	if (pDlgEdit->wasChanged())
	{
		doc->setModified(true);
		*(getDocument()->getList()) = pDlgEdit->getList();
	}
	delete pDlgEdit;
	pDlgEdit = 0;
	// Anzahl in der Statuszeile anpassen
	emit testAnzahl(doc->getAnzahl());
}

void KVerbosApp::slotSettingsOptions()
{
  KVerbosOptions* pDlgEdit = new KVerbosOptions(this, "bearbeiten", true);
  pDlgEdit->CheckBox1_1->setChecked(timeOptions[presente][0]);
  pDlgEdit->CheckBox2_1->setChecked(timeOptions[presente][1]);
  pDlgEdit->CheckBox3_1->setChecked(timeOptions[presente][2]);
  //
  pDlgEdit->CheckBox1_2->setChecked(timeOptions[1][0]);
  pDlgEdit->CheckBox2_2->setChecked(timeOptions[1][1]);
  pDlgEdit->CheckBox3_2->setChecked(timeOptions[1][2]);
  //
  pDlgEdit->CheckBox1_3->setChecked(timeOptions[2][0]);
  pDlgEdit->CheckBox2_3->setChecked(timeOptions[2][1]);
  //
  pDlgEdit->CheckBox1_4->setChecked(timeOptions[3][0]);
  pDlgEdit->CheckBox2_4->setChecked(timeOptions[3][1]);
  pDlgEdit->CheckBox3_4->setChecked(timeOptions[3][2]);
  //
  pDlgEdit->CheckBox1_5->setChecked(timeOptions[4][0]);
  pDlgEdit->CheckBox2_5->setChecked(timeOptions[4][1]);
  pDlgEdit->CheckBox3_5->setChecked(timeOptions[4][2]);
  //
  pDlgEdit->CheckBox1_6->setChecked(timeOptions[5][0]);
  pDlgEdit->CheckBox2_6->setChecked(timeOptions[5][1]);
  pDlgEdit->CheckBox3_6->setChecked(timeOptions[5][2]);
  //
  pDlgEdit->CheckBox1_7->setChecked(timeOptions[6][0]);
  pDlgEdit->CheckBox2_7->setChecked(timeOptions[6][1]);
  pDlgEdit->CheckBox3_7->setChecked(timeOptions[6][2]);
  //
  pDlgEdit->CheckBox1_8->setChecked(timeOptions[subFuturo][0]);
  pDlgEdit->CheckBox2_8->setChecked(timeOptions[subFuturo][1]);
  pDlgEdit->CheckBox3_8->setChecked(timeOptions[subFuturo][2]);
  //
  pDlgEdit->CheckBox1_9->setChecked(timeOptions[imperativo][0]);
  //
  pDlgEdit->RadioStrictCorr->setChecked(correctStrict);
  pDlgEdit->RadioIgnoreAcc->setChecked(!correctStrict);
  //
  pDlgEdit->CheckBoxKFeeder->setChecked(useKFeeder);
  if (!useKFeeder)
  {
    pDlgEdit->CheckBoxSplash->setDisabled(true);
    pDlgEdit->CheckBoxMode->setDisabled(true);
  };
  pDlgEdit->CheckBoxSplash->setChecked(showKFeederSplash);
  pDlgEdit->CheckBoxMode->setChecked(automaticKFeeder);
  //
  if (pDlgEdit->exec() == 1)
  {
    timeOptions[presente][0] = pDlgEdit->CheckBox1_1->isChecked();
    timeOptions[presente][1] = pDlgEdit->CheckBox2_1->isChecked();
    timeOptions[presente][2] = pDlgEdit->CheckBox3_1->isChecked();
    //
    timeOptions[1][0] = pDlgEdit->CheckBox1_2->isChecked();
    timeOptions[1][1] = pDlgEdit->CheckBox2_2->isChecked();
    timeOptions[1][2] = pDlgEdit->CheckBox3_2->isChecked();
    //
    timeOptions[2][0] = pDlgEdit->CheckBox1_3->isChecked();
    timeOptions[2][1] = pDlgEdit->CheckBox2_3->isChecked();
    //
    timeOptions[3][0] = pDlgEdit->CheckBox1_4->isChecked();
    timeOptions[3][1] = pDlgEdit->CheckBox2_4->isChecked();
    timeOptions[3][2] = pDlgEdit->CheckBox3_4->isChecked();
    //
    timeOptions[4][0] = pDlgEdit->CheckBox1_5->isChecked();
    timeOptions[4][1] = pDlgEdit->CheckBox2_5->isChecked();
    timeOptions[4][2] = pDlgEdit->CheckBox3_5->isChecked();
    //
    timeOptions[5][0] = pDlgEdit->CheckBox1_6->isChecked();
    timeOptions[5][1] = pDlgEdit->CheckBox2_6->isChecked();
    timeOptions[5][2] = pDlgEdit->CheckBox3_6->isChecked();
    //
    timeOptions[6][0] = pDlgEdit->CheckBox1_7->isChecked();
    timeOptions[6][1] = pDlgEdit->CheckBox2_7->isChecked();
    timeOptions[6][2] = pDlgEdit->CheckBox3_7->isChecked();
    //
    timeOptions[subFuturo][0] = pDlgEdit->CheckBox1_8->isChecked();
    timeOptions[subFuturo][1] = pDlgEdit->CheckBox2_8->isChecked();
    timeOptions[subFuturo][2] = pDlgEdit->CheckBox3_8->isChecked();
    //
    timeOptions[imperativo][0] = pDlgEdit->CheckBox1_9->isChecked();
    //
    correctStrict = pDlgEdit->RadioStrictCorr->isChecked();
    //
    showKFeederSplash = pDlgEdit->CheckBoxSplash->isChecked();
    // testen ob die Art der Futteranimationen geändert werden soll. Diese soll nur geändert
    // werden, wenn der KFeeder läuft und auch weiterhin laufen soll.

    if ((automaticKFeeder != pDlgEdit->CheckBoxMode->isChecked()) && useKFeeder &&
         (pDlgEdit->CheckBoxKFeeder->isChecked() == true))
    {
      kfeederControl->setAutomatic(pDlgEdit->CheckBoxMode->isChecked());
    };
    automaticKFeeder = pDlgEdit->CheckBoxMode->isChecked();
    // prüfen, ob der KFeeder angehalten oder gestartet werden soll.
    if ((pDlgEdit->CheckBoxKFeeder->isChecked() == true) && (!useKFeeder))
    {
      // kfeeder starten
      if (kfeederControl == 0)
        kfeederControl = new KFeederControl(kverbosCounter, showKFeederSplash, automaticKFeeder);
    }
    else
      if ((pDlgEdit->CheckBoxKFeeder->isChecked() == false) && (useKFeeder))
      {
        // kfeeder anhalten
        disconnect(view, SIGNAL(informKFeeder(int)), this, SLOT(slotFeedIt(int)));
        delete kfeederControl;
        kfeederControl = 0;
      };
    useKFeeder = pDlgEdit->CheckBoxKFeeder->isChecked();
  };
  delete pDlgEdit;
  pDlgEdit = 0;
}

void KVerbosApp::getTimeOptions(bool op[][3])
{
	for (int i=0; i<MAX_TIEMPOS; i++)
		for (int j=0; j<MAX_SUBTYPES; j++)
			op[i][j] = timeOptions[i][j];
}

void KVerbosApp::slotSettingsUsername()
{
	bool ok;
	QString n = KInputDialog::getText(i18n("Enter User Name"), i18n("Please enter your name:"),
                        QString::null , &ok, this).simplifyWhiteSpace();
        if (ok)
        {
		statusBar()->changeItem(i18n("user: ") + n, ID_USERNAME_MSG);
		doc->setNewUser(n);
		userResults->setEnabled(true);
		if (kfeederControl != 0)
			kfeederControl->reset();
        }
}

void KVerbosApp::slotUserResults()
{
	KResult* pDlgResult = new KResult(doc->getUser(), this, "username", true);
	pDlgResult->exec();
}

void KVerbosApp::slotFeedIt(int n)
{
	if (useKFeeder && (kfeederControl != 0))
		kfeederControl->addN(n);
}

#include "kverbos.moc"
