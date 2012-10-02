/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer <matthias.kiefer@gmx.de>
		2002-2005  by Stanislav Visnovsky <visnovsky@kde.org>
  Copyright (C) 2006 by Nicolas GOUTTE <goutte@kde.org>

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt. If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.

**************************************************************************** */

#include "kbabelview.h"

#include "catalogfileplugin.h"
#include "toolaction.h"
#include "kbabelsettings.h"
#include "kbprojectsettings.h"

#include <qlayout.h>

#include <qlabel.h>
#include <qframe.h>
#include <qfileinfo.h>
#include <qvariant.h>
#include <qwhatsthis.h>
#include <qdragobject.h>
#include <qpopupmenu.h>
#include <qstylesheet.h>
#include <qtabwidget.h>
#include <qtextview.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qvbox.h>

#include <dcopclient.h>
#include <kdeversion.h>
#include <kcharsets.h>
#include <kcmenumngr.h>
#include <kconfigdialog.h>
#include <kdatatool.h>
#include <kglobal.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kcursor.h>
#include <kfiledialog.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kled.h>
#include <klistbox.h>
#include <kio/netaccess.h>
#include <knotifyclient.h>
#include <ktempfile.h>
#include <kspell.h>
#include <kwin.h>
#include <kstdaccel.h>
#include <kurldrag.h>
#include <kglobalsettings.h>

#include "resources.h"

#include "editcmd.h"
#include "finddialog.h"
#include "gotodialog.h"
#include "headereditor.h"
#include "hidingmsgedit.h"
#include "kbabeldictbox.h"
#include "kbcatalog.h"
#include "msgfmt.h"
#include "spelldlg.h"
#include "regexpextractor.h"
#include "projectprefwidgets.h"
#include "kbabel.h"
#include "kbprojectmanager.h"

#include "commentview.h"
#include "contextview.h"
#include "kbcataloglistview.h"
#include "charselectview.h"
#include "taglistview.h"
#include "sourceview.h"
#include "errorlistview.h"

#include "version.h"


#define ID_DROP_OPEN          1
#define ID_DROP_OPEN_TEMPLATE 2

#define MAX_HISTORY 50

using namespace KBabel;

QPtrList<KBabelView> *KBabelView::viewList = 0;

KBabelView::KBabelView(KBCatalog* catalog,KBabelMW *parent, Project::Ptr project)
    : QWidget(parent)
    , _redirectedBackSearch (false)
    , _project (project)
    , m_mainwindow (parent)
    , m_sourceview (0)
{
    if(!viewList)
        viewList = new QPtrList<KBabelView>;

    viewList->append(this);

    if (catalog == 0)
      kdFatal(KBABEL) << "catalog==0" << endl;

    _catalog=catalog;
    _catalog->registerView(this);

    _config = KSharedConfig::openConfig ("kbabelrc");

    KConfigGroupSaver gs(_config,"Editor");
    bool buildLeds=! KBabelSettings::ledInStatusbar();

    _fuzzyLed=0;
    _untransLed=0;
    _errorLed=0;
    _currentIndex=0;
    _currentPos.item=1;	// to ensure update
    _currentPos.form=0;
    _gotoDialog=0;
    _findDialog=0;
    _replaceDialog=0;
    _replaceAskDialog=0;
    _tagsMenu=0;
    _argsMenu=0;
    _autoSearchTempDisabled=false;
    _diffEnabled=false;
    _loadingDiffFile=false;
    _diffing=false;
    _dontBeep=false;
    
    m_overwrite = false;

    autoSaveTimer = 0;

    _showTryLaterBox=true;
    _tagExtractor = new RegExpExtractor( catalog->tagSettings().tagExpressions );
    _argExtractor = new RegExpExtractor( catalog->tagSettings().argExpressions );

    _editingDocumentation=false;

    _autocheckTools.clear();
    _autocheckTools.setAutoDelete(true);

    spell.posDict.setAutoDelete(true);
    spell.active=false;

    spell2.kspell = 0;
    spell2.config = 0;
    
    setAcceptDrops(true);
    
    // this widget does not contain anything, we should hide it
    hide();

    dictBox = new KBabelDictBox(this,"dictBox");
    QWhatsThis::add(dictBox,
        i18n("<qt><p><b>Search results</b></p>"
        "<p>This part of the window shows the results of searching in "
        "dictionaries.<p>"
        "<p>In the top is displayed the number of entries found and "
        "where the currently displayed entry is found. Use the buttons "
        "at the bottom to navigate through the search results.</p>"
        "<p>Search is either started automatically when switching to "
        "another entry in the editor window or by choosing the desired "
        "dictionary in <b>Dictionaries->Find...</b>.</p>"
        "<p>The common options can be configured in the preferences dialog "
	"in section <b>Search</b> and the options for the different "
	"dictionaries can be changed with "
	"<b>Settings->Configure Dictionary</b>.</p></qt>"));

    initDockWidgets();

    msgstrEdit->setReadOnly(true);
    dictBox->setEnabled(false);

    connect(this, SIGNAL(signalNewFileOpened(KURL)),
            m_cataloglistview, SLOT(slotNewFileOpened()));
    
    connect(msgstrEdit,SIGNAL(signalUndoCmd(KBabel::EditCommand*)),this,SLOT(forwardMsgstrEditCmd(KBabel::EditCommand*)));
    connect(msgstrEdit,SIGNAL(textChanged()),this
            ,SIGNAL(signalMsgstrChanged()));
    
    connect(msgstrEdit,SIGNAL(textChanged(const QString&)),m_cataloglistview
            ,SLOT(msgstrChanged(const QString&)));
    
    connect(this,SIGNAL(signalMsgstrChanged()),this,SLOT(autoCheck()));
    connect(msgstrEdit,SIGNAL(currentFormChanged(uint)), this
	    ,SLOT(msgstrPluralFormChanged(uint)));

    connect(msgidLabel,SIGNAL(cursorPositionChanged(int,int))
            , this, SIGNAL(signalCursorPosChanged(int,int)));
    connect(msgstrEdit,SIGNAL(cursorPositionChanged(int,int))
            , this, SIGNAL(signalCursorPosChanged(int,int)));

    connect(dictBox,SIGNAL(searchStarted()),this
            ,SLOT(forwardSearchStart()));
    connect(dictBox, SIGNAL(progressStarts(const QString&)), this
            ,SLOT(forwardProgressStart(const QString&)));
    connect(dictBox,SIGNAL(progressed(int)),this
            ,SIGNAL(signalProgress(int)));
    connect(dictBox,SIGNAL(searchStopped()),this
            ,SLOT(forwardSearchStop()));
    connect(dictBox,SIGNAL(progressEnds()),this
            ,SIGNAL(signalClearProgressBar()));

    connect(dictBox,SIGNAL(modulesChanged()),this,
		    SIGNAL(signalDictionariesChanged()));
    connect(dictBox,SIGNAL(errorInModule(const QString&)),this
            ,SLOT(showError(const QString&)));

    connect(_catalog,SIGNAL(signalSettingsChanged(KBabel::IdentitySettings)),
            this, SLOT(setNewLanguage()));

    connect(_catalog,SIGNAL(signalNumberOfFuzziesChanged(uint)),
            this, SLOT(checkFuzzies()));
    connect(_catalog,SIGNAL(signalNumberOfUntranslatedChanged(uint)),
            this, SLOT(checkUntranslated()));

    if(buildLeds)
    {
       connect(this,SIGNAL(signalFuzzyDisplayed(bool))
			   ,this,SLOT(toggleFuzzyLed(bool)));
       connect(this,SIGNAL(signalUntranslatedDisplayed(bool))
			   ,this,SLOT(toggleUntransLed(bool)));
       connect(this,SIGNAL(signalFaultyDisplayed(bool))
			   ,this,SLOT(toggleErrorLed(bool)));
    }

    _dropMenu = new QPopupMenu(this);
    _dropMenu->insertItem(i18n("Menu item", "Open"),ID_DROP_OPEN);
    _dropMenu->insertItem(i18n("Open Template"),ID_DROP_OPEN_TEMPLATE);

    readSettings(_config);
    readProject(_project);

    connect (project, SIGNAL(signalSpellcheckSettingsChanged()),
	this, SLOT(updateProjectSettings()));

    if(!_catalog->currentURL().isEmpty())
    {
       newFileOpened(_catalog->isReadOnly());
    }

    // take over the language settings from the catalog
    setNewLanguage();
}

KBabelView::~KBabelView()
{
   viewList->remove(this);
   if(viewList->isEmpty())
   {
       delete viewList;
       viewList=0;
   }

   _catalog->removeView(this);

   // check if this view was the last view and delete the catalog if necessary
   if(!_catalog->hasView())
   {
      delete _catalog;
   }

   delete _argExtractor;
   delete _tagExtractor;

   if( spell2.kspell )
   {
	spell2.kspell = 0;
	delete spell2.config;
	spell2.config = 0;
   }
}

void KBabelView::initDockWidgets()
{
    // setup main dock widget - original text
    QWidget *tempWidget=new QWidget(this,"msgidWidget");
    tempWidget->setMinimumSize(350,150);

    QVBoxLayout *layout=new QVBoxLayout(tempWidget);

    msgidLabel = new HidingMsgEdit(2, this, 0, tempWidget,"msgidLabel");
    msgidLabel->installEventFilter(this);
    msgidLabel->setReadOnly(true);
    msgidLabel->setDiffMode(true);
    KCursor::setAutoHideCursor(msgidLabel,true);

    msgidLabel->setText(i18n("KBabel Version %1\n"
"Copyright 1999-%2 by KBabel developers.\n"
" Matthias Kiefer <kiefer@kde.org>\n"
" Stanislav Visnovsky <visnovsky@kde.org>\n"
" Marco Wegner <dubbleu@web.de>\n"
" Dwayne Bailey <dwayne@translate.org.za>\n"
" Andrea Rizzi <rizzi@kde.org>\n\n"
"Any comments, suggestions, etc. should be sent to the mailing list <kbabel@kde.org>.\n\n"
"This program is licensed under the terms of the GNU GPL.\n\n"
"Special thanks to Thomas Diehl for many hints to the GUI\n"
"and the behavior of KBabel and to Stephan Kulow, who always\n"
"lends me a helping hand.\n\n"
"Many good ideas, especially for the Catalog Manager, are taken\n"
"from KTranslator by Andrea Rizzi.").arg(VERSION).arg(2006));

    QLabel *label=new QLabel(msgidLabel,i18n("O&riginal string (msgid):"),tempWidget);

    QHBoxLayout* hb=new QHBoxLayout(layout);
    hb->addSpacing(KDialog::marginHint());
    hb->addWidget(label);

    layout->addWidget(msgidLabel);
    layout->setStretchFactor(msgidLabel,1);

    QWhatsThis::add(tempWidget,
       i18n("<qt><p><b>Original String</b></p>\n\
<p>This part of the window shows the original message\n\
of the currently displayed entry.</p></qt>"));

    KDockWidget* mainDock;
    mainDock = m_mainwindow->createDockWidget( "Original", QPixmap ());
    //i18n: translators: Dock window caption
    mainDock->setCaption(i18n("Original Text"));
    mainDock->setGeometry(50, 50, 100, 100);
    // forbit docking abilities of mainDock itself
    mainDock->setEnableDocking(KDockWidget::DockFullSite);

    mainDock->setWidget(tempWidget);
    m_mainwindow->setMainDockWidget(mainDock); // master dockwidget
    mainDock->show ();
    m_mainwindow->setView (mainDock);
    connect (this, SIGNAL (signalCopy ()), this, SLOT (textCopy ()));
    connect (this, SIGNAL (signalCut ()), this, SLOT (textCut ()));
    connect (this, SIGNAL (signalPaste ()), this, SLOT (textPaste ()));
    connect (this, SIGNAL (signalSelectAll ()), this, SLOT (selectAll ()));

    KDockWidget* comment_dock = m_mainwindow->createDockWidget( "Comment", QPixmap ());
    //i18n: translators: Dock window caption
    comment_dock->setCaption(i18n("Comment"));
    comment_dock->setGeometry(50, 50, 100, 100);
    comment_dock->setEnableDocking(KDockWidget::DockCorner);
    m_commentview = new CommentView(_catalog, comment_dock, _project);
    comment_dock->setWidget(m_commentview);
    comment_dock->manualDock( mainDock,              // dock target
        KDockWidget::DockRight, // dock site
        20 );                  // relation target/this (in percent)
    comment_dock->show ();
    connect (this, SIGNAL (signalCopy ()), m_commentview, SLOT (textCopy ()));
    connect (this, SIGNAL (signalCut ()), m_commentview, SLOT (textCut ()));
    connect (this, SIGNAL (signalPaste ()), m_commentview, SLOT (textPaste ()));
    connect (this, SIGNAL (signalSelectAll ()), m_commentview, SLOT (textSelectAll ()));
    m_commentview->installEventFilter( this );

    // build the msgstr widget
    tempWidget=new QWidget(this,"msgstrWidget");
    tempWidget->setMinimumSize(350,150);

    layout=new QVBoxLayout(tempWidget);

    // if undefined number of plural forms, use 1
    int pf = _catalog->defaultNumberOfPluralForms();
    if( pf < 1 ) 
	pf = 1;
	
    msgstrEdit = new HidingMsgEdit( pf,this,spell2.kspell,tempWidget,"msgstrEdit");
    msgstrEdit->installEventFilter(this);
    KCursor::setAutoHideCursor(msgstrEdit,true);

    label=new QLabel(msgstrEdit,i18n("Trans&lated string (msgstr):"),tempWidget);

    hb=new QHBoxLayout(layout);
    hb->setSpacing(KDialog::spacingHint());

    hb->addSpacing(KDialog::marginHint());
    hb->addWidget(label);
    hb->addStretch(1);

    if(! KBabelSettings::ledInStatusbar())
    {
       _fuzzyLed = new KLed(Qt::red,KLed::Off,KLed::Sunken,KLed::Rectangular
			   ,tempWidget);
       _fuzzyLed->setFixedSize(15,12);
       label = new QLabel(i18n("fuzzy"),tempWidget);
       hb->addWidget(_fuzzyLed);
       hb->addWidget(label);

       hb->addSpacing(KDialog::spacingHint());

       _untransLed = new KLed(Qt::red,KLed::Off,KLed::Sunken,KLed::Rectangular
					   ,tempWidget);
       _untransLed->setFixedSize(15,12);
       label = new QLabel(i18n("untranslated"),tempWidget);
       hb->addWidget(_untransLed);
       hb->addWidget(label);

       hb->addSpacing(KDialog::marginHint());

       _errorLed = new KLed(Qt::red,KLed::Off,KLed::Sunken,KLed::Rectangular
					   ,tempWidget);
       _errorLed->setFixedSize(15,12);
       label = new QLabel(i18n("faulty"),tempWidget);
       hb->addWidget(_errorLed);
       hb->addWidget(label);

       hb->addSpacing(KDialog::marginHint());

       hb->addStretch(1);

       // ### TODO: perhaps it should be moreprecise where the setting can be changed
       QString ledMsg=i18n("<qt><p><b>Status LEDs</b></p>\n"
       "<p>These LEDs display the status of the currently displayed message.\n"
       "You can change their color in the preferences dialog section\n"
       "<b>Editor</b> on page <b>Appearance</b></p></qt>");
       QWhatsThis::add(_fuzzyLed,ledMsg);
       QWhatsThis::add(_untransLed,ledMsg);
       QWhatsThis::add(_errorLed,ledMsg);
    }

    layout->addWidget(msgstrEdit);
    layout->setStretchFactor(msgstrEdit,1);

    QWhatsThis::add(tempWidget,
       i18n("<qt><p><b>Translation Editor</b></p>\n\
<p>This editor displays and lets you edit the translation of the currently displayed message.<p></qt>"));


    KDockWidget* msgstr_dock = m_mainwindow->createDockWidget( "Msgstr", QPixmap ());
    //i18n: translators: Dock window caption
    msgstr_dock->setCaption(i18n("Translated String"));
    msgstr_dock->setEnableDocking(KDockWidget::DockCorner);
    msgstr_dock->setWidget(tempWidget);
    msgstr_dock->manualDock( mainDock,              // dock target
        KDockWidget::DockBottom, // dock site
        50 );                  // relation target/this (in percent)
    msgstr_dock->show ();

    KDockWidget* dock = m_mainwindow->createDockWidget( "Search", QPixmap ());
    //i18n: translators: Dock window caption
    dock->setCaption(i18n("the search (noun)","Search"));
    //i18n: translators: Dock tab caption
    dock->setTabPageLabel(i18n("the search (noun)","Se&arch"));
    dock->setGeometry(50, 50, 100, 100);
    dock->setWidget(dictBox);
    dock->manualDock( comment_dock,  // dock target
        KDockWidget::DockBottom, // dock site
        20 );                  // relation target/this (in percent)
    dock->show ();

    KDockWidget* tools = dock;

    dock = m_mainwindow->createDockWidget( "PO context", QPixmap ());
    //i18n: translators: Dock window caption
    dock->setCaption(i18n("PO Context"));
    //i18n: translators: Dock tab caption
    dock->setTabPageLabel(i18n("PO C&ontext"));
    dock->setGeometry(50, 50, 100, 100);
    ContextView * m_contextview = new ContextView(_catalog, dock, _project);
    dock->setWidget(m_contextview);
    dock->manualDock( tools,  // dock target
        KDockWidget::DockCenter, // dock site
        20 );                  // relation target/this (in percent)
    dock->show ();

    dock = m_mainwindow->createDockWidget( "Charselector", QPixmap ());
    //i18n: translators: Dock window caption
    dock->setCaption(i18n("Character Table"));
    //i18n: translators: Dock tab caption
    dock->setTabPageLabel(i18n("C&hars"));
    dock->setGeometry(50, 50, 100, 100);
    m_charselectorview = new CharacterSelectorView(_catalog, dock, _project);
    dock->setWidget(m_charselectorview);
    dock->manualDock( tools,  // dock target
        KDockWidget::DockCenter, // dock site
        20 );                  // relation target/this (in percent)
    dock->show ();


    dock = m_mainwindow->createDockWidget( "Tag List", QPixmap ());
    //i18n: translators: Dock window caption
    dock->setCaption(i18n("Tag List"));
    //i18n: translators: Dock tab caption
    dock->setTabPageLabel(i18n("Tags"));
    dock->setGeometry(50, 50, 100, 100);
    TagListView* m_taglistview = new TagListView(_catalog, dock, _project);
    dock->setWidget(m_taglistview);
    dock->manualDock( tools,  // dock target
        KDockWidget::DockCenter, // dock site
        20 );                  // relation target/this (in percent)
    dock->show ();

    dock = m_mainwindow->createDockWidget( "Source Context", QPixmap ());
    //i18n: translators: Dock window caption
    dock->setCaption(i18n("Source Context"));
    //i18n: translators: Dock tab caption
    dock->setTabPageLabel(i18n("Source"));
    dock->setGeometry(50, 50, 100, 100);
    m_sourceview = new SourceView(_catalog, dock, _project);
    dock->setWidget(m_sourceview);
    dock->manualDock( tools,  // dock target
        KDockWidget::DockCenter, // dock site
        20 );                  // relation target/this (in percent)
    dock->show ();
    
    KDockWidget* translist_dock = m_mainwindow->createDockWidget( "Translation List", QPixmap ());
    translist_dock->setCaption(i18n("Translation List"));
    translist_dock->setGeometry(50, 50, 100, 100);
    translist_dock->setEnableDocking(KDockWidget::DockFullSite);
    m_cataloglistview = new KBCatalogListView(_catalog, translist_dock, _project);
    translist_dock->setWidget(m_cataloglistview);
    translist_dock->manualDock( mainDock, KDockWidget::DockTop,100);
    translist_dock->show ();

    dock = m_mainwindow->createDockWidget( "Error List", QPixmap ());
    //i18n: translators: Dock window caption
    dock->setCaption(i18n("Error List"));
    //i18n: translators: Dock tab caption
    dock->setTabPageLabel(i18n("Errors"));
    dock->setGeometry(50, 50, 100, 100);
    ErrorListView* m_errorlistview = new ErrorListView(_catalog, dock, _project);
    dock->setWidget(m_errorlistview);
    dock->manualDock( tools, // dock target
       KDockWidget::DockCenter, // dock site
       20 );
    dock->show ();
    
    connect(m_cataloglistview,SIGNAL(signalSelectionChanged(const KBabel::DocPosition&))
        ,this,SLOT(gotoEntry(const KBabel::DocPosition&)));
    connect(this,SIGNAL(signalDisplayed(const KBabel::DocPosition&))
        ,m_commentview,SLOT(gotoEntry(const KBabel::DocPosition&)));
    connect(this,SIGNAL(signalDisplayed(const KBabel::DocPosition&))
        ,m_contextview,SLOT(gotoEntry(const KBabel::DocPosition&)));
    connect(this,SIGNAL(signalDisplayed(const KBabel::DocPosition&))
        ,m_taglistview,SLOT(gotoEntry(const KBabel::DocPosition&)));
    connect(this,SIGNAL(signalDisplayed(const KBabel::DocPosition&))
        ,m_sourceview,SLOT(gotoEntry(const KBabel::DocPosition&)));
    connect(this,SIGNAL(signalDisplayed(const KBabel::DocPosition&))
        ,m_errorlistview,SLOT(gotoEntry(const KBabel::DocPosition&)));
    connect(this,SIGNAL(signalFaultyDisplayed(bool))
        ,m_errorlistview,SLOT(updateView()));
    connect(m_charselectorview, SIGNAL( characterDoubleClicked(QChar) )
	,this, SLOT( insertChar(QChar) ));
    connect(m_taglistview,SIGNAL(signalTagSelected(const QString&))
	, this, SLOT(insertTagFromTool(const QString&)));
    connect(m_taglistview,SIGNAL(signalHighlightedTagChanged(int))
	, this, SLOT(skipToTagFromTool(int)));
    connect(this, SIGNAL(signalNextTag(int))
	, m_taglistview, SLOT(highlightTag(int)));
    connect(m_commentview,SIGNAL(signalCursorPosChanged(int,int))
	, m_mainwindow, SLOT(updateCursorPosition(int,int)));
}

KBabelView *KBabelView::viewForURL(const KURL& url, const QString project)
{
    if(url.isEmpty())
        return 0;

    if(!viewList)
        return 0;

    KURL u = url;
    u.cleanPath();

    QPtrListIterator<KBabelView> it(*viewList);
    KBabelView *view=0;
    while( it.current() && !view)
    {
        KURL cu = it.current()->currentURL();
        cu.cleanPath();

        if(cu == u && it.current()->project()==project)
        {
            view = it.current();
        }

        ++it;
    }

    return view;
}

KBabelView *KBabelView::emptyView(const QString)
{
    if(!viewList)
        return 0;

    QPtrListIterator<KBabelView> it(*viewList);
    KBabelView *view=0;
    while( it.current() && !view)
    {
        KURL cu = it.current()->currentURL();
	if( cu.isEmpty() )
	    return it.current();
        ++it;
    }

    return 0;
}

KURL KBabelView::currentURL() const
{
   return _catalog->currentURL();
}

bool KBabelView::isLastView() const
{
    return _catalog->isLastView();
}

bool KBabelView::isModified() const
{
    return _catalog->isModified();
}

bool KBabelView::isReadOnly() const
{
    return _catalog->isReadOnly();
}

bool KBabelView::isOverwriteMode() const
{
   return m_overwrite;
}


void KBabelView::setOverwriteMode(bool ovr)
{
   m_overwrite = ovr;
   msgstrEdit->setOverwriteMode(ovr);
   m_commentview->setOverwriteMode(ovr);
}


bool KBabelView::isSearching() const
{
   return dictBox->isSearching();
}


void KBabelView::update(EditCommand* cmd, bool undo)
{
   if((int)_currentIndex==cmd->index())
   {
      emitEntryState();

      if(cmd->part()==Msgstr)
      {
         msgstrEdit->processCommand(cmd,undo);
	 emit signalMsgstrChanged();
      }
   }
}



void KBabelView::readSettings(KConfig* config)
{

   if(KBabelSettings::autoUnsetFuzzy())
   {
      connect(msgstrEdit,SIGNAL(textChanged())
		  ,this,SLOT(autoRemoveFuzzyStatus()));
   }

   setupAutoCheckTools();

   KConfigGroupSaver saver(config,"Editor");

   _diffEnabled = config->readBoolEntry("AutoDiff", false);
   emit signalDiffEnabled(_diffEnabled);

   if(_fuzzyLed)
   {
      _fuzzyLed->setColor(KBabelSettings::ledColor());
   }
   if(_untransLed)
   {
      _untransLed->setColor(KBabelSettings::ledColor());
   }
   if(_errorLed)
   {
      _errorLed->setColor(KBabelSettings::ledColor());
   }

   msgstrEdit->setCleverEditing(KBabelSettings::cleverEditing());

   msgstrEdit->setHighlightBg(KBabelSettings::highlightBackground());
   msgidLabel->setHighlightBg(KBabelSettings::highlightBackground());

   msgstrEdit->setHighlightSyntax(KBabelSettings::highlightSyntax());
   msgidLabel->setHighlightSyntax(KBabelSettings::highlightSyntax());

   msgstrEdit->setQuotes(KBabelSettings::enableQuotes());
   msgidLabel->setQuotes(KBabelSettings::enableQuotes());

   msgstrEdit->setSpacePoints(KBabelSettings::whitespacePoints());
   msgidLabel->setSpacePoints(KBabelSettings::whitespacePoints());

   msgstrEdit->setFont(KBabelSettings::msgFont());
   msgidLabel->setFont(KBabelSettings::msgFont());

   msgstrEdit->setBgColor(KBabelSettings::backgroundColor());
   msgidLabel->setBgColor(KBabelSettings::backgroundColor());

   msgstrEdit->setHighlightColors(KBabelSettings::quotedColor(),KBabelSettings::errorColor()
           ,KBabelSettings::cformatColor(),KBabelSettings::accelColor(),KBabelSettings::tagColor());
   msgidLabel->setHighlightColors(KBabelSettings::quotedColor(),KBabelSettings::errorColor()
           ,KBabelSettings::cformatColor(),KBabelSettings::accelColor(),KBabelSettings::tagColor());

   msgidLabel->setDiffDisplayMode( KBabelSettings::diffAddUnderline()
           ,KBabelSettings::diffDelStrikeOut() );
   msgidLabel->setDiffColors(KBabelSettings::diffAddColor(), KBabelSettings::diffDelColor() );
}

void KBabelView::updateProjectSettings()
{
    readProject(_project);
}

void KBabelView::readProject(Project::Ptr project)
{
    _spellcheckSettings = project->spellcheckSettings();

   if( _spellcheckSettings.onFlySpellcheck )
   {

    // if there is a spellchecker already, free it
    if( spell2.kspell ) 
    {
	// ensure the spellchecker is not used anymore
	msgstrEdit->setSpellChecker(0L);

	// free it
	spell2.kspell->cleanUp();
	
	delete spell2.kspell;
	spell2.kspell = 0;
    }
    
    spell2.config = new KSpellConfig(0L, "tempSpellConfig");
    spell2.config->setNoRootAffix(_spellcheckSettings.noRootAffix);
    spell2.config->setRunTogether(_spellcheckSettings.runTogether);
    spell2.config->setClient(_spellcheckSettings.spellClient);
    spell2.config->setEncoding(_spellcheckSettings.spellEncoding);
    spell2.config->setDictionary(_spellcheckSettings.spellDict);

    spell2.kspell= new KSpell(this, "", this, SLOT(dummy(KSpell *)),
                              spell2.config, false, false);
    if(spell2.kspell->status() == KSpell::Error)
        kdWarning(KBABEL) << "Something's wrong with KSpell, can't start on-the-fly checking" << endl;
    else
    {
	kdDebug() << "On the fly spellchecker: " 
	    << spell2.kspell << endl;
	msgstrEdit->setSpellChecker(spell2.kspell);
    }

    // spell2.kspell->setAutoDelete(true); // let KSpell handle delete
    //on-the-fly spellcheck end
   }
   else
   {
    // turn off spellchecker
    msgstrEdit->setSpellChecker(0);
    // invalidate the current settings, to make sure they are updated when needed
    _spellcheckSettings.valid = false;
   }

   dictBox->readSettings(project->config());
   
    m_sourceview->setProject(project);
}

void KBabelView::saveSettings()
{
   KConfigGroupSaver saver(_config,"Editor");

   _config->writeEntry("AutoDiff",_diffEnabled);

   dictBox->saveSettings( _project->config() );

   _catalog->savePreferences();

   _config->sync();
   _project->config()->sync();

   KBabelSettings::writeConfig();
}

void KBabelView::updateSettings()
{
    msgstrEdit->setFont(KBabelSettings::msgFont());
    msgidLabel->setFont(KBabelSettings::msgFont());

    msgidLabel->setDiffDisplayMode( KBabelSettings::diffAddUnderline()
           ,KBabelSettings::diffDelStrikeOut());
    msgidLabel->setDiffColors(KBabelSettings::diffAddColor(), KBabelSettings::diffDelColor());

    if( _diffEnabled && !_catalog->currentURL().isEmpty() )
    {
	diffShowOrig ();
	diff();
    }

   msgstrEdit->setBgColor(KBabelSettings::backgroundColor());
   msgidLabel->setBgColor(KBabelSettings::backgroundColor());

   msgstrEdit->setHighlightColors(KBabelSettings::quotedColor(),KBabelSettings::errorColor()
           ,KBabelSettings::cformatColor(),KBabelSettings::accelColor(),KBabelSettings::tagColor());
   msgidLabel->setHighlightColors(KBabelSettings::quotedColor(),KBabelSettings::errorColor()
           ,KBabelSettings::cformatColor(),KBabelSettings::accelColor(),KBabelSettings::tagColor());

   msgidLabel->setDiffDisplayMode( KBabelSettings::diffAddUnderline()
           ,KBabelSettings::diffDelStrikeOut());
   msgidLabel->setDiffColors(KBabelSettings::diffAddColor(), KBabelSettings::diffDelColor());

    if(_fuzzyLed)
    {
        _fuzzyLed->setColor(KBabelSettings::ledColor());
    }
    if(_untransLed)
    {
        _untransLed->setColor(KBabelSettings::ledColor());
    }
    if(_errorLed)
    {
        _errorLed->setColor(KBabelSettings::ledColor());
    }

    disconnect(msgstrEdit,SIGNAL(textChanged())
			 ,this,SLOT(autoRemoveFuzzyStatus()));

   if(KBabelSettings::autoUnsetFuzzy())
   {
         connect(msgstrEdit,SIGNAL(textChanged())
			 ,this,SLOT(autoRemoveFuzzyStatus()));
   }

   msgstrEdit->setCleverEditing(KBabelSettings::cleverEditing());

   msgstrEdit->setHighlightBg(KBabelSettings::highlightBackground());
   msgidLabel->setHighlightBg(KBabelSettings::highlightBackground());
   msgstrEdit->setHighlightSyntax(KBabelSettings::highlightSyntax());
   msgidLabel->setHighlightSyntax(KBabelSettings::highlightSyntax());

   msgstrEdit->setQuotes(KBabelSettings::enableQuotes());
   msgidLabel->setQuotes(KBabelSettings::enableQuotes());

   msgstrEdit->setSpacePoints(KBabelSettings::whitespacePoints());
   msgidLabel->setSpacePoints(KBabelSettings::whitespacePoints());

   msgstrEdit->setFont(KBabelSettings::msgFont());
   msgidLabel->setFont(KBabelSettings::msgFont());

   msgstrEdit->setBgColor(KBabelSettings::backgroundColor());
   msgidLabel->setBgColor(KBabelSettings::backgroundColor());

   msgstrEdit->setHighlightColors(KBabelSettings::quotedColor(),KBabelSettings::errorColor()
           ,KBabelSettings::cformatColor(),KBabelSettings::accelColor(),KBabelSettings::tagColor());
   msgidLabel->setHighlightColors(KBabelSettings::quotedColor(),KBabelSettings::errorColor()
           ,KBabelSettings::cformatColor(),KBabelSettings::accelColor(),KBabelSettings::tagColor());

   msgidLabel->setDiffDisplayMode( KBabelSettings::diffAddUnderline()
           ,KBabelSettings::diffDelStrikeOut());
   msgidLabel->setDiffColors(KBabelSettings::diffAddColor(), KBabelSettings::diffDelColor());

   // if errors should not use special color, reset the color of the text
   if(! KBabelSettings::autoCheckColorError())
   {
	msgstrEdit->setCurrentColor( MsgMultiLineEdit::NormalColor);
   }

    if(_fuzzyLed)
    {
        _fuzzyLed->setColor(KBabelSettings::ledColor());
    }
    if(_untransLed)
    {
        _untransLed->setColor(KBabelSettings::ledColor());
    }
    if(_errorLed)
    {
        _errorLed->setColor(KBabelSettings::ledColor());
    }

    emit ledColorChanged(KBabelSettings::ledColor());

   setupAutoCheckTools();

   if( _catalog->numberOfEntries() > 0 )
	autoCheck(false);

}

void KBabelView::setupAutoCheckTools()
{
   _autocheckTools.clear();

   kdDebug () << "Autocheck tools: " << KBabelSettings::autoCheckTools().join(",") << endl;

   if(!KBabelSettings::autoCheckTools().isEmpty() )
   {
	QValueList<KDataToolInfo> tools = ToolAction::validationTools();

	QValueList<KDataToolInfo>::Iterator it;
	for( it=tools.begin(); it!=tools.end() ; ++it )
	{
	    if(KBabelSettings::autoCheckTools().contains((*it).service()->library()) )
	    {
		// maybe we can reuse the tools
		KDataTool* t = (*it).createTool();

		if( t ) _autocheckTools.append( t );
	    }
	}
   }
}

void KBabelView::setRMBEditMenu(QPopupMenu* popup)
{
	msgidLabel->setContextMenu( popup );
	msgstrEdit->setContextMenu( popup );
	KContextMenuManager::insert(this,popup);
}

void KBabelView::setRMBSearchMenu(QPopupMenu* popup)
{
   dictBox->setRMBMenu(popup);
}


void KBabelView::saveView(KConfig *)
{
}

void KBabelView::restoreView(KConfig *)
{
}

void KBabelView::saveSession(KConfig* config)
{
   QString focus;
   int line=0,col=0;
   if(msgstrEdit->hasFocus())
   {
      focus="msgstr";
      msgstrEdit->getCursorPosition(&line,&col);
   }
   else if(msgidLabel->hasFocus())
   {
      focus="msgid";
      msgidLabel->getCursorPosition(&line,&col);
   }
   else if(dictBox->hasFocus())
   {
      focus="searchbox";
   }

   config->writeEntry("Focus",focus);
   config->writeEntry("CursorLine",line);
   config->writeEntry("CursorCol",col);
   config->writeEntry("Index",_currentIndex);
   config->writeEntry("PluralForm",_currentPos.form);

   config->writePathEntry("URL",currentURL().url());

   config->writeEntry("AutoDiff",_diffEnabled);

   if( _spellcheckSettings.valid )
   {
	KConfigGroupSaver (config, "Spellcheck");
	config->writeEntry("NoRootAffix",_spellcheckSettings.noRootAffix);
	config->writeEntry("RunTogether",_spellcheckSettings.runTogether);
	config->writeEntry("SpellEncoding",_spellcheckSettings.spellEncoding);
	config->writeEntry("SpellClient",_spellcheckSettings.spellClient);
	config->writeEntry("SpellDictionary",_spellcheckSettings.spellDict);
	config->writeEntry("RememberIgnored",_spellcheckSettings.rememberIgnored);
	config->writePathEntry("IgnoreURL",_spellcheckSettings.ignoreURL);
   }

   saveView(config);
}

void KBabelView::restoreSession(KConfig* config)
{
   QString url=config->readPathEntry("URL");

   if(!url.isEmpty())
   {
      open(KURL( url ), QString::null, false,true);
   }


   _diffEnabled = config->readBoolEntry("AutoDiff", false);
   emit signalDiffEnabled(_diffEnabled);

   msgidLabel->setDiffDisplayMode( KBabelSettings::diffAddUnderline()
           , KBabelSettings::diffDelStrikeOut() );
   msgidLabel->setDiffColors( KBabelSettings::diffAddColor()
	   , KBabelSettings::diffDelColor() );

   // go to last displayed entry
   DocPosition pos;
   pos.item=config->readNumEntry("Index");
   pos.form=config->readNumEntry("PluralForm");
   gotoEntry(pos);

   QString focus=config->readEntry("Focus");
   int line=config->readNumEntry("CursorLine");
   int col=config->readNumEntry("CursorCol");
   if(focus=="msgstr")
   {
      msgstrEdit->setFocus();
      msgstrEdit->setCursorPosition(line,col);
   }
   else if(focus=="msgid")
   {
      msgidLabel->setFocus();
      msgstrEdit->setCursorPosition(line,col);
   }
   else if(focus=="searchbox")
   {
      dictBox->setFocus();
   }

   restoreView(config);
}

void KBabelView::newFileOpened(bool readOnly)
{
    // disable editing for empty files
    if(_catalog->numberOfEntries() == 0)
	readOnly = true;

    if(_gotoDialog)
    {
        _gotoDialog->setMax(_catalog->numberOfEntries());
    }

    dictBox->setDisabled(readOnly);
    msgstrEdit->setReadOnly(readOnly);
    msgstrEdit->setFocus();

    QString caption=_catalog->package();
    if(readOnly)
        caption+=i18n(" [readonly]");
    emit signalChangeCaption(caption);
    emit signalNewFileOpened(_catalog->currentURL());

    dictBox->setEditedPackage(_catalog->packageDir()+_catalog->packageName());
    dictBox->setEditedFile(_catalog->currentURL().url());

    _editingDocumentation = _catalog->isGeneratedFromDocbook();

    _backHistory.clear();
    emit signalBackHistory(false);
    _forwardHistory.clear();
    emit signalForwardHistory(false);

    DocPosition pos;
    pos.item=0;
    pos.form=0;
    _autoSearchTempDisabled=true;
    gotoEntry(pos,false);
    _autoSearchTempDisabled=false;

    emit signalDisplayed(pos);

   if(isActiveWindow() && KBabelSettings::autoSearch())
   {
      startSearch(true);
   }
}

void KBabelView::open()
{
   open(KURL());
}

void KBabelView::open(const KURL& _url, const QString & package, bool checkIfModified, bool newView)
{
#if KDE_IS_VERSION( 3, 5, 0)
   KURL url = KIO::NetAccess::mostLocalURL(_url,this);
#else
   KURL url = _url;
#endif
   url.cleanPath();

   KURL cu = currentURL();
   cu.cleanPath();
   if(checkIfModified && !url.isEmpty() && cu==url)
   {
       KWin::activateWindow(topLevelWidget()->winId());
       return;
   }

   stopSearch();

   if(!checkIfModified || checkModified())
   {
       if(url.isEmpty())
       {
            QString filename;
            if ((url = KFileDialog::getOpenURL(currentURL().url(), CatalogImportPlugin::availableImportMimeTypes().join(" ")
                            ,this)).isEmpty())
            {
                return;
            }
       }
       // deactive editor
       /*setEnabled(false);
       setCursor(KCursor::waitCursor());*/

       KBabelView *v = viewForURL(url,_project->filename());

       if(v && v != this)
       {
    	   if( newView )
	   {
		// unregister old catalog
		_catalog->removeView(this);
		if( !_catalog->hasView() )
		    delete _catalog;

		// setup new one
		_catalog = v->catalog();
		_catalog->registerView(this);
		newFileOpened(_catalog->isReadOnly());
		return;
	   }
	   else {
	        KWin::activateWindow(v->topLevelWidget()->winId());
        	return;
	   }
       }

       ConversionStatus stat=_catalog->openURL(url, package);

       switch(stat)
       {
          case OK:
          {
               break;
          }
          case RECOVERED_HEADER_ERROR: // Old name HEADER_ERROR
          {
                KMessageBox::sorry(this,
                    i18n("There was an error while reading the file header. "
                    "Please check the header." ));
                editHeader();
                break;
          }
          case PARSE_ERROR:
          {
              KMessageBox::error(this
                 ,i18n("Error while trying to read file:\n %1\n"
                       "Maybe it is not a valid PO file.").arg(url.prettyURL()));
              break;
          }
          case NO_ENTRY_ERROR:
          {
              KMessageBox::error(this
                  ,i18n("Error while reading the file:\n %1\n"
                  "No entry found.").arg(url.prettyURL()));
              break;
          }
          case RECOVERED_PARSE_ERROR:
          {
              QString msg=i18n(
                    "The file contained syntax errors and an attempt has been "
                    "made to recover it.\n"
                    "Please check the questionable entries by using "
                    "Go->Next error");
             KMessageBox::sorry(this,msg);
             emitEntryState();
             break;
          }
          case NO_PERMISSIONS:
          {
             KMessageBox::error(this,i18n(
               "You do not have permissions to read file:\n %1").arg(url.prettyURL()));
             break;
          }
          case NO_FILE:
          {
             KMessageBox::error(this,i18n(
                 "You have not specified a valid file:\n %1").arg(url.prettyURL()));
             break;
          }
	  case NO_PLUGIN:
	  {
             KMessageBox::error(this,i18n(
                 "KBabel cannot find a corresponding plugin for the MIME type of the file:\n %1").arg(url.prettyURL()));
             break;
	  }
	  case UNSUPPORTED_TYPE:
	  {
             KMessageBox::error(this,i18n(
                 "The import plugin cannot handle this type of the file:\n %1").arg(url.prettyURL()));
             break;
	  }
	  case STOPPED:
              break;
          default:
          {
             KMessageBox::error(this,i18n(
                  "Error while trying to open file:\n %1").arg(url.prettyURL()));
             break;
          }

       }
       /*
       //activate editor
       setEnabled(true);
       setCursor(KCursor::arrowCursor());*/

       _autoSaveDelay = _catalog->saveSettings( ).autoSaveDelay;
       if ( _autoSaveDelay ) {
         if ( !autoSaveTimer ) {
           autoSaveTimer = new QTimer( this, "AUTOSAVE TIMER" );
           connect( autoSaveTimer, SIGNAL( timeout( ) ), this, SLOT( slotAutoSaveTimeout( ) ) );
         }
         autoSaveTimer->stop( );
         autoSaveTimer->start( 1000 * 60 * _autoSaveDelay );
       }

   }
}

void KBabelView::revertToSaved()
{
    if(isModified())
    {
        // ### TODO: "Cancel" should be the default
        if(KMessageBox::warningContinueCancel(this
                    ,i18n("All changes will be lost if the file "
                        "is reverted to its last saved state.")
                    ,i18n("Warning"),i18n("&Revert"))
                == KMessageBox::Cancel)
        {
            return;
        }
    }

    open(_catalog->currentURL(),QString::null,false);
}

void KBabelView::openTemplate(const KURL& openURL, const KURL& saveURL)
{
   stopSearch();

   if(checkModified())
   {
       // deactive editor
       /*setEnabled(false);
       setCursor(KCursor::waitCursor());*/


       ConversionStatus stat=_catalog->openURL(openURL,saveURL);

       switch(stat)
       {
           case OK:
           {
               break;
           }
           case RECOVERED_HEADER_ERROR:
           {
               // For a template, recoverable errors are disqualifying
                KMessageBox::sorry(this,
                    i18n("There was an error while reading the file header of file:\n %1")
                    .arg(openURL.prettyURL()));
                break;
           }
           case PARSE_ERROR:
           {
               KMessageBox::error(this
                       ,i18n("Error while trying to read file:\n %1\n"
                       "Maybe it is not a valid PO file.").arg(openURL.prettyURL()));
               break;
           }
           case NO_ENTRY_ERROR:
           {
               KMessageBox::error(this
                    ,i18n("Error while reading the file:\n %1\n"
                       "No entry found.").arg(openURL.prettyURL()));
               break;
           }
           case RECOVERED_PARSE_ERROR:
           {
               // For a template, recoverable errors are disqualifying
               KMessageBox::sorry(this,
                   i18n("Minor syntax errors were found while reading file:\n %1")
                   .arg(openURL.prettyURL()));
               break;
	  }
	  case NO_PERMISSIONS:
	  {
	      KMessageBox::error(this,i18n("You do not have permissions to read file:\n %1").arg(openURL.prettyURL()));
	      break;
	  }
	  case NO_FILE:
	  {
	      KMessageBox::error(this,i18n("You have not specified a valid file:\n %1").arg(openURL.prettyURL()));
	     break;
	  }
	  case NO_PLUGIN:
	  {
             KMessageBox::error(this,i18n(
                 "KBabel cannot find a corresponding plugin for the MIME type of the file:\n %1").arg(openURL.prettyURL()));
             break;
	  }
	  case UNSUPPORTED_TYPE:
	  {
             KMessageBox::error(this,i18n(
                 "The import plugin cannot handle this type of the file:\n %1").arg(openURL.prettyURL()));
             break;
	  }
          case STOPPED:
            break;
	  default:
	  {
	     KMessageBox::error(this,i18n("Error while trying to open file:\n %1").arg(openURL.prettyURL()));
	     break;
	  }

       }

       /*
       //activate editor
       setEnabled(true);

       setCursor(KCursor::arrowCursor());*/
   }
}

bool KBabelView::saveFile(bool syntaxCheck)
{
    if(_catalog->isReadOnly())
   {
      return saveFileAs();
   }
   else
   {
      ConversionStatus stat=_catalog->saveFile();

      int whatToDo = -1;

      switch(stat)
      {
	 case OK:
	 {
	    informDictionary();
	    if(syntaxCheck && _catalog->saveSettings().autoSyntaxCheck  )
	       return checkSyntax(true,false);
	    else
	       return true;
	 }
	 case NO_PERMISSIONS:
	 {
	    whatToDo=KMessageBox::warningContinueCancel(this,
	     i18n("You do not have permission to write to file:\n%1\n"
		  "Do you want to save to another file or cancel?").arg(_catalog->currentURL().prettyURL()),
	     i18n("Error"),KStdGuiItem::save());
	     break;
	 }
	 case NO_PLUGIN:
	 {
             KMessageBox::error(this,i18n(
                 "KBabel cannot find a corresponding plugin for the MIME type of file:\n %1").arg(_catalog->currentURL().prettyURL()));
             break;
	 }
	 case UNSUPPORTED_TYPE:
	 {
             KMessageBox::error(this,i18n(
                 "The export plugin cannot handle this type of file:\n %1").arg(_catalog->currentURL().prettyURL()));
             break;
	 }
	 case BUSY:
	 {
             KMessageBox::error(this,i18n(
                 "KBabel has not finished the last operation yet.\n"
		 "Please wait."));
             break;
	 }
	 case STOPPED:
             break;
	 default:
	 {
	    whatToDo=KMessageBox::warningContinueCancel(this,
	     i18n("An error occurred while trying to write to file:\n%1\n"
		  "Do you want to save to another file or cancel?").arg(_catalog->currentURL().prettyURL()),
	     i18n("Error"),KStdGuiItem::save());
	     break;
	 }
      }
      switch(whatToDo)
      {
	 case KMessageBox::Continue:
	     return saveFileAs();
	 default:
	     return false;

      }

   }

   return true;
}

bool KBabelView::saveFileAs(KURL url, bool syntaxCheck)
{
   bool newName=false;
   if(url.isEmpty())
   {
       if((url = KFileDialog::getSaveURL(currentURL().url(), CatalogExportPlugin::availableExportMimeTypes().join(" "),this)).isEmpty())
       {
	  return false;
       }
       newName=true;
   }

   if (KIO::NetAccess::exists(url, false, this))
   {
       if(KMessageBox::warningContinueCancel(this,QString("<qt>%1</qt>").arg(i18n("The file %1 already exists. "
						       "Do you want to overwrite it?").arg(url.prettyURL())),i18n("Warning"),i18n("&Overwrite"))==KMessageBox::Cancel)
      {
	 return false;
      }

   }

   bool wasReadOnly=_catalog->isReadOnly();

   ConversionStatus stat=_catalog->saveFileAs(url,true);


   // if the file was not saved sucessfully ask for saving to another file
   if(stat!=OK)
   {
      bool cancelLoop=false; // flag, if the saving loop should be canceled
      do
      {
	  // select the right error message
	  QString message;
	  switch(stat)
	  {
	      case NO_PERMISSIONS:
	      {
		  message=i18n("You do not have permission to write to file:\n%1\n"
			       "Do you want to save to another file or cancel?").arg(url.prettyURL());
		 break;
	      }
	      case NO_FILE:
	      {
		 message=i18n("You have specified a folder:\n%1\n"
			      "Do you want to save to another file or cancel?").arg(url.prettyURL());
		 break;
	      }
	      case NO_PLUGIN:
	      {
	         message=i18n("KBabel cannot find a corresponding plugin for the MIME type of the file:\n %1").arg(url.prettyURL());
                 break;
	      }
	      case UNSUPPORTED_TYPE:
	      {
                 message=i18n(
                    "The export plugin cannot handle this type of the file:\n %1").arg(url.prettyURL());
                 break;
	      }
	      default:
	      {
		  message=i18n("An error occurred while trying to write to file:\n%1\n"
			       "Do you want to save to another file or cancel?").arg(url.prettyURL());
		  break;
	      }
	  }

	  // now ask, if the user wants to save to another file or cancel
	  switch(KMessageBox::warningContinueCancel(this,message,i18n("Error"),KStdGuiItem::save()))
	  {
	     // save to another file
	     case KMessageBox::Continue:
	     {
		// ask for new filename
		if ((url = KFileDialog::getSaveURL(currentURL().url(), CatalogExportPlugin::availableExportMimeTypes().join(" "), this)).isEmpty())
		{
		   // if no filename was given cancel all
		   return false;
		}

		if (KIO::NetAccess::exists(url, false, this))
		{
		    if(KMessageBox::warningContinueCancel(this,i18n("The file %1 already exists.\n"
								    "Do you want to overwrite it?").arg(url.prettyURL()),i18n("Warning"),i18n("&Overwrite"))==KMessageBox::Continue)
		   {
		      stat=_catalog->saveFileAs(url);
		      if(stat!=OK)
			 cancelLoop=false;
		      else
			cancelLoop=true;
		   }
		}
		else
		{
		   stat=_catalog->saveFileAs(url);
		   if(stat!=OK)
		      cancelLoop=false;
		   else
		     cancelLoop=true;
		}
		break;
	     }
	     default: // the user do not want to save to another file
		return false;
	  }
      }
      while(!cancelLoop);
   }


   if(wasReadOnly)
   {
      msgstrEdit->setReadOnly(false);
   }

   emit signalChangeCaption(_catalog->package());

   if(newName)
   {
      dictBox->setEditedPackage(_catalog->packageName());
      dictBox->setEditedFile(_catalog->currentURL().url());
   }

   // after save put current edit into dictionary as well
   informDictionary();

   if(syntaxCheck && _catalog->saveSettings().autoSyntaxCheck)
     return checkSyntax(true,false);
   else
     return true;
}

bool KBabelView::saveFileSpecial()
{
    QString tmpname;
    bool result = false;
    
    {
	// generate temporary name
	KTempFile tmp;
	tmp.setAutoDelete (true);
	tmpname = tmp.name ();
    }
    
    {
	// make sure the project is freed (pointer going out of scope)
	KBabel::Project::Ptr project = KBabel::ProjectManager::open (tmpname);
    
	project->setSettings( _catalog->saveSettings() );
	KConfigDialog *_prefDialog = new KConfigDialog(this, "project dialog", project->settings(),
                  KDialogBase::IconList, KDialogBase::Cancel|KDialogBase::Ok|KDialogBase::Help);

	_prefDialog->setCaption( i18n("Special Save Settings") );
	_prefDialog->setHelp("preferences_save");

	SavePreferences* _prefWidget = new SavePreferences(_prefDialog);
	_prefWidget->setAutoSaveVisible(false);
	_prefDialog->addPage(_prefWidget, i18n("title of page in preferences dialog","Save")
            , "filesave"
            , i18n("Options for File Saving"));

	if( _prefDialog->exec() == QDialog::Accepted )
	{
	    SaveSettings settings = project->saveSettings();
	    SaveSettings originalSettings = _catalog->saveSettings();
	    _catalog->setSettings(settings);
	    result = saveFileAs();

	    _catalog->setSettings(originalSettings);
	}
    }
    
    QFile::remove( tmpname );

    return result;
}

bool KBabelView::checkSyntax()
{
   return checkSyntax(false,false);
}

bool KBabelView::checkSyntax(bool msgOnlyAtError,bool question)
{
   if(currentURL().isEmpty())
      return false;

   bool returnCode=true;
   QString output;

   Msgfmt::Status result=_catalog->checkSyntax( output );

   const QStringList outputLines = QStringList::split("\n",output);
   
   switch(result)
   {
      case Msgfmt::Ok:
      {
          if(!msgOnlyAtError)
          {
              KMessageBox::informationList( this, i18n("The file is syntactically correct.\n\n"
                  "Output of \"msgfmt --statistics\":\n"), outputLines );
          }
          returnCode=true;
          break;
      }
      case Msgfmt::Unsupported:
      {
          if(!msgOnlyAtError)
          {
    		KMessageBox::sorry(this
        	    ,i18n("You can use gettext tools only for checking PO files."));
	  }
	  returnCode=true;
	  break;
      }
      case Msgfmt::HeaderError:
      case Msgfmt::SyntaxError:
      {
          QString msg = ( result == Msgfmt::SyntaxError )
              ? i18n("msgfmt detected a syntax error.\n")
              : i18n("msgfmt detected a header syntax error.\n");

          if(question)
          {
              msg += i18n("\nDo you want to continue or cancel and edit the file again?");
              msg += "\n\n";
              msg += i18n("Output of \"msgfmt --statistics\":\n");
              switch( KMessageBox::warningContinueCancelList( this, msg,
                    outputLines, i18n("Warning"), KStdGuiItem::cont()) )
              {
                    case KMessageBox::Continue:
                        returnCode=true;
                        break;
                    default:
                        returnCode=false;
              }
          }
          else
          {
#if KDE_IS_VERSION ( 3, 4, 0 )
              msg += "\n";
              msg += i18n("Please edit the file again.");
              msg += "\n\n";
              msg += i18n("Output of \"msgfmt --statistics\":\n");
              KMessageBox::errorList( this, msg, outputLines );
#else
              msg += i18n("Output of \"msgfmt --statistics\":\n");
              msg += output;
              msg += "\n\n";
              msg += i18n("Please edit the file again.");
              KMessageBox::error( this, msg );
#endif
              returnCode=false;
          }
          break;
      }
      case Msgfmt::NoExecutable:
      case Msgfmt::Error:
      {
            QString msg = i18n("While trying to check syntax with msgfmt an error occurred.\n"
                "Please make sure that you have installed\n"
                "the GNU gettext package properly.");
            if(question)
            {
                msg += i18n("\nDo you want to continue or cancel and edit the file again?");
                switch( KMessageBox::warningContinueCancelList( this, msg,
                outputLines, i18n("Warning"), KStdGuiItem::cont() ))
                {
                    case KMessageBox::Continue:
                        returnCode=true;
                        break;
                    default:
                        returnCode=false;
                }
            }
            else
            {
#if KDE_IS_VERSION ( 3, 4, 0 )
                 msg += "\n";
                 msg += i18n("Please edit the file again.");
                 KMessageBox::errorList( this, msg, outputLines );
#else
                 msg += output;
                 msg += "\n\n";
                 msg += i18n("Please edit the file again.");
                 KMessageBox::error( this, msg );
#endif
                 returnCode=false;
            }

            break;
      }
   }

   emitEntryState();

   return returnCode;
}

bool KBabelView::checkAll()
{
	if(currentURL().isEmpty())
		return false;

	bool a,badresult=false;

	QValueList<KDataToolInfo> tools = ToolAction::validationTools();

	QValueList<KDataToolInfo>::ConstIterator entry = tools.begin();
	for( ; entry != tools.end(); ++entry )
	{
	    KDataTool* tool = (*entry).createTool();
	    if( tool )
	    {
		a = _catalog->checkUsingTool(tool,entry==tools.begin());
		badresult = badresult || !a;
		delete tool;
	    }
	}

	QString output;

	a = (_catalog->checkSyntax(output, false)!=Msgfmt::Ok);
	badresult=badresult||a;

	emitEntryState();

	if(!badresult)
	{
		KMessageBox::information(this
					,i18n("No mismatch has been found.")
					, i18n("Title in Dialog: Perform all checks","Perform All Checks"));
	}
	else
	{
		int index=0;
		DocPosition pos;
		if(!_catalog->hasError(0,pos))
			index = _catalog->nextError(0, pos);
		if(index>=0)
		{
		      gotoEntry(pos);
		}

		KMessageBox::error(this
					,i18n("Some mismatches have been found.\n"
			"Please check the questionable entries by using "
			"Go->Next error")
					, i18n("Title in Dialog: Perform all checks","Perform All Checks"));

	}

	return !badresult;
}



bool KBabelView::checkModified()
{
   bool flag=true;

   if(isModified())
   {
      switch(KMessageBox::warningYesNoCancel(this,
      i18n("The document contains unsaved changes.\n"
           "Do you want to save your changes or discard them?"),i18n("Warning"),
      KStdGuiItem::save(),KStdGuiItem::discard()))
      {
	 case KMessageBox::Yes:
	 {
	    flag = saveFile(false);
	    if(flag && _catalog->saveSettings().autoSyntaxCheck)
	    {
	       flag = checkSyntax(true,true);
	    }
	    break;
	 }
	 case KMessageBox::No:
	     flag = true;
	     break;
	 default:   // canceled
	    flag = false;
	    break;
      }
   }

   return flag;
}

void KBabelView::updateEditor(int form, bool delay)
{
   msgstrEdit->blockSignals(true);

   if(KBabelSettings::autoUnsetFuzzy() && !msgstrEdit->isModified())
   {
       disconnect(msgstrEdit,SIGNAL(textChanged()),this,SLOT(autoRemoveFuzzyStatus()));
   }

   msgidLabel->setText(_catalog->msgid(_currentIndex), _catalog->msgctxt(_currentIndex));
   msgidLabel->repaint();

   msgstrEdit->setText(_catalog->msgstr(_currentIndex));
   msgstrEdit->showForm( form );
   msgstrEdit->repaint();
   m_cataloglistview->setSelectedItem(_currentIndex);

   if(KBabelSettings::autoUnsetFuzzy() && _catalog->isFuzzy(_currentIndex))
   {
      connect(msgstrEdit,SIGNAL(textChanged()),this,SLOT(autoRemoveFuzzyStatus()));
   }

   msgstrEdit->blockSignals(false);

   autoCheck(false);

   // no need to display diff if this message wasn't translated
   if(_diffEnabled && !(_catalog->isUntranslated(_currentIndex)))
   {
       autoDiff();
   }

   if(isActiveWindow() && KBabelSettings::autoSearch()
                   && !_autoSearchTempDisabled)
   {
      if (delay)
      {
          QTimer::singleShot(0, this, SLOT (startSearch()));
      }
      else
      {
    	    startSearch();
      }
   }

   msgstrEdit->setFocus();
   msgstrEdit->setModified(false);
}


void KBabelView::undo()
{
   if(!_catalog->isUndoAvailable())
      return;

   int newIndex=_catalog->undo();

   if(newIndex != (int)_currentIndex)
   {
	DocPosition pos;
	pos.item=newIndex;
	pos.form=0;
	gotoEntry(pos);
   }
}

void KBabelView::redo()
{
   if(!_catalog->isRedoAvailable())
      return;

   int newIndex=_catalog->redo();

   if(newIndex != (int)_currentIndex)
   {
	DocPosition pos;
	pos.item=newIndex;
	pos.form=0;
	gotoEntry(pos);
   }
}

void KBabelView::textCut()
{
   if(msgstrEdit->hasFocus())
   {
      msgstrEdit->cut();
   }
}

void KBabelView::textCopy()
{
   if(msgstrEdit->hasSelectedText())
   {
      msgstrEdit->copy();
   }
   else if(msgidLabel->hasSelectedText())
   {
      msgidLabel->copy();
   }
   else if(dictBox->isVisible() && dictBox->hasSelectedText())
   {
      dictBox->copy();
   }
}

void KBabelView::textPaste()
{
    msgstrEdit->paste();
}


bool KBabelView::findNext()
{
	if(!_findDialog)
		return false;

	if( !_redirectedBackSearch && _findDialog->findOpts().backwards )
	{
	    _redirectedBackSearch = true;
	    bool res = findPrev();
	    _redirectedBackSearch = false;
	    return res;
	}

	DocPosition pos;
	pos.item=_currentIndex;
	pos.form=0;

	if( m_commentview->hasFocus() ) {
		pos.part = Comment;
		pos.offset = m_commentview->currentIndex();
	}
	else
	if(msgidLabel->hasFocus() ) {
		pos.part = Msgid;
		pos.offset = msgidLabel->currentIndex();
	}
	else {
		pos.part = Msgstr;
		pos.offset = msgstrEdit->currentIndex();
		pos.form = msgstrEdit->currentForm();
	}

	_findStartPos=pos;
	_findBreakAtEnd=false;

	return findNext_internal(pos);
}

bool KBabelView::findPrev()
{
	if(!_findDialog)
		return false;

	if( !_redirectedBackSearch && _findDialog->findOpts().backwards )
	{
	    _redirectedBackSearch = true;
	    bool res = findNext();
	    _redirectedBackSearch = false;
	    return res;
	}

	DocPosition pos;
	pos.item=_currentIndex;

	if( m_commentview->hasFocus()) {
		pos.part = Comment;
		pos.offset = m_commentview->currentIndex();
	}
	else
	if(msgidLabel->hasFocus() ) {
		pos.part = Msgid;
		pos.offset = msgidLabel->currentIndex();
	}
	else {
		pos.part = Msgstr;
		pos.offset = msgstrEdit->currentIndex();
		pos.form = msgstrEdit->currentForm();
	}

	_findStartPos=pos;
	_findBreakAtEnd=false;

	return findPrev_internal(pos);
}


bool KBabelView::findNext_internal(DocPosition& pos, bool forReplace, bool gui)
{
	FindOptions opts;
	if(forReplace)
		opts = _replaceDialog->replaceOpts();
	else
		opts = _findDialog->findOpts();
	int len=0;
	deselectAll();

	bool success=false;
	if( !opts.askFile ) // for find in all files ignore return to the beginning
	if(!_findBreakAtEnd && !(success=_catalog->findNext(&opts,pos,len))) {
		int r;
		if(forReplace) {
			_replaceWasAtEnd=true;
			_findBreakAtEnd=true;
			if(gui) {
				r = KMessageBox::questionYesNo(this,
				i18n("<qt>%n replacement made.<br>End of document reached.<br>Continue from the beginning?</qt>",
				     "<qt>%n replacements made.<br>End of document reached.<br>Continue from the beginning?</qt>",
				     _replacesTotal), QString::null, KStdGuiItem::cont(), KStdGuiItem::cancel());
			}
			else {
				r = KMessageBox::Yes;
			}
		}
		else {
			r = KMessageBox::questionYesNo(this,i18n("End of document reached.\n"
				"Continue from the beginning?"), QString::null, KStdGuiItem::cont(), KStdGuiItem::cancel());
		}
		if(r == KMessageBox::Yes) {
			if(opts.inMsgid && !forReplace)
				pos.part = Msgid;
			else if(opts.inComment)
				pos.part = Comment;
			else
				pos.part = Msgstr;

			pos.item = 0;
			pos.offset = 0;
			pos.form = 0;

		}
		else
			return false;
	}


	if(!success && !_catalog->findNext(&opts,pos,len)) { // reached end the second time
	    if( !opts.askFile )
	    {
		if(forReplace) {
			KMessageBox::information(this,i18n("%n replacement made","%n replacements made",_replacesTotal));
		}
		else {
			KMessageBox::information(this,i18n("Search string not found."));
		}
		return false;
	    }
	    else
	    {
		if( opts.askForNextFile )
		{
		    int r = KMessageBox::questionYesNo(this,i18n("End of document reached.\n"
			"Continue in the next file?"), QString::null, KStdGuiItem::cont(), KStdGuiItem::cancel());
		    if( r != KMessageBox::Yes ) return false;

		}
		if( isModified() && !opts.askForSave ) saveFile();
		DCOPClient *client = kapp->dcopClient();
		QByteArray data, replyData;
		QCString replyType;
		bool morefiles = false;	// more files to lookup in
		if( !client->call( _fileSource,"CatalogManagerIFace",
			"findNextFile()", data, replyType, replyData) ) kdDebug(KBABEL) << "unable to call, reply type is " << replyType << endl;
		else if( replyType == "QCString" )
		{
		    QDataStream rep( replyData, IO_ReadOnly);
		    QCString f;
		    rep >> f;
		    QString foundFile = QString::fromUtf8(f);
		    morefiles = !f.isEmpty() && !f.isNull();
		    if( morefiles )
		    {
			emit open( KURL(foundFile) );
			pos.item = 0;
			pos.part = Msgid;
			pos.offset = 0;
			_catalog->findNext(&opts,pos,len);
		    } else {
			if( f.isNull() ) // is there possibility to get a new file?
			{
			    // no, this is the end finally
			    if( forReplace )
				KMessageBox::information(this,i18n("%n replacement made","%n replacements made",_replacesTotal));
			    else
				KMessageBox::information(this,i18n("Search string not found."));
			    return false;
			}
			else
			{
			    // there can be a new file, let user know
			    showTryLaterMessageBox();
			    if( forReplace ) return true; // let replace dialog stay
			    else return false;
			}
		    }
		} else {
		    KMessageBox::error(this,i18n("DCOP communication with Catalog Manager failed."));
		    return false;
		}
	    }
	}
	if(gui) {
		if( _currentIndex != pos.item
		  || (pos.part==Msgstr && msgstrEdit->currentForm() != pos.form) )
		  {
			gotoEntry(pos);
		  }

		int line,col,endline,endcol;
		switch(pos.part) {
			case Msgid:
				msgidLabel->selectAll(false);
				msgidLabel->setFocus();
				msgidLabel->offset2Pos(pos.offset,line,col);
				msgidLabel->offset2Pos(pos.offset+len,endline,endcol);

				msgidLabel->setSelection(line,col,endline,endcol);
				msgidLabel->setCursorPosition(endline,endcol);

				_lastFoundString=msgidLabel->selectedText();
				break;
			case Msgstr:
				msgstrEdit->selectAll(false);
				msgstrEdit->setFocus();
				msgstrEdit->offset2Pos(pos.offset,line,col);
				msgstrEdit->offset2Pos(pos.offset+len,endline,endcol);

				msgstrEdit->setSelection(line,col,endline,endcol);
				msgstrEdit->setCursorPosition(endline,endcol);

				_lastFoundString=msgstrEdit->selectedText();
				break;
			case Comment:
			{
				m_mainwindow->makeWidgetDockVisible (m_commentview);
				_lastFoundString=  m_commentview->selectText (pos.offset, pos.offset+len);
				break;
			}
			case UndefPart:
				break;
		}
	}

	if(forReplace) {
		kdDebug(KBABEL) << "This is forReplace" << endl;
		_replaceLen=len;

		bool finished=false;
		// check if we had reached the beginning before and now are before our starting position
		if(_replaceWasAtEnd) {
			if( pos.item > _findStartPos.item ) {
				finished=true;
			}
			else if(pos.item == _findStartPos.item) {
				if(pos.part==Msgstr && !opts.inComment && pos.offset >= _findStartPos.offset+_replaceExtraOffset)
					finished=true;
				else if(pos.part==Comment && pos.offset >= _findStartPos.offset+_replaceExtraOffset)
					finished=true;
			}
		}

		if(finished) {
			KMessageBox::information(this,i18n("%n replacement made","%n replacements made",_replacesTotal));
			return false;
		}

	}

	return true;
}

bool KBabelView::findPrev_internal(DocPosition& pos, bool forReplace, bool gui)
{
	FindOptions opts;
	if(forReplace)
		opts = _replaceDialog->replaceOpts();
	else
		opts = _findDialog->findOpts();

	int len=0;

	deselectAll();
	bool success=false;
	if(!_findBreakAtEnd && !(success=_catalog->findPrev(&opts,pos,len))) {
		int r;

		if(forReplace) {
			_replaceWasAtEnd=true;
			_findBreakAtEnd=true;
			if(gui) {
				r = KMessageBox::questionYesNo(this,
				i18n("<qt>%n replacement made.<br>Beginning of document reached.<br>Continue from the end?</qt>",
				     "<qt>%n replacements made.<br>Beginning of document reached.<br>Continue from the end?</qt>",
				     _replacesTotal),QString::null, KStdGuiItem::cont(), KStdGuiItem::cancel());
			}
			else {
				r = KMessageBox::Yes;
			}

		}
		else {
			r = KMessageBox::questionYesNo(this,i18n("Beginning of document reached.\n"
				"Continue from the end?"), QString::null, KStdGuiItem::cont(), KStdGuiItem::cancel());
		}
		if(r == KMessageBox::Yes) {
			pos.item = _catalog->numberOfEntries()-1;

			if(opts.inComment) {
				pos.part = Comment;
				pos.offset = _catalog->comment(pos.item).length();
			}
			else if(opts.inMsgstr || forReplace) {
				pos.part = Msgstr;
				if( _catalog->msgstr(pos.item).empty() ) pos.offset=0;
				else pos.offset = _catalog->msgstr(pos.item).last().length();
				pos.form = _catalog->msgstr(pos.item).count()-1;
			}
			else {
				pos.part = Msgid;
				//FIXME: we should search in msgid plural forms
				pos.offset = _catalog->msgid(pos.item).first().length();
			}
		}
		else
			return false;
	}

	// start at the end
	if(!success && !_catalog->findPrev(&opts,pos,len)) {
		if(forReplace) {
			KMessageBox::information(this,i18n("%n replacement made","%n replacements made",_replacesTotal));
		}
		else {
			KMessageBox::information(this,i18n("Search string not found."));
		}
		return false;
	}
	else if(gui) {
		if(_currentIndex != pos.item
		    || (pos.part==Msgstr && msgstrEdit->currentForm() != pos.form) )
			gotoEntry(pos);

		int line,col,endline,endcol;
		switch(pos.part) {
			case Msgid:
				msgidLabel->selectAll(false);
				msgidLabel->setFocus();
				msgidLabel->offset2Pos(pos.offset+len,line,col);
				msgidLabel->offset2Pos(pos.offset,endline,endcol);

				msgidLabel->setSelection(line,col,endline,endcol);
				msgidLabel->setCursorPosition(endline,endcol);

				_lastFoundString=msgidLabel->selectedText();
				break;
			case Msgstr:
				msgstrEdit->selectAll(false);
				msgstrEdit->setFocus();
				msgstrEdit->offset2Pos(pos.offset+len,line,col);
				msgstrEdit->offset2Pos(pos.offset,endline,endcol);

				msgstrEdit->setSelection(line,col,endline,endcol);
				msgstrEdit->setCursorPosition(endline,endcol);

				_lastFoundString=msgstrEdit->selectedText();
				break;
			case Comment:
				m_mainwindow->makeWidgetDockVisible (m_commentview);
				_lastFoundString=m_commentview->
				    selectText(pos.offset, pos.offset+len);
				break;
			case UndefPart:
				break;
		}
	}

	if(forReplace) {
		_replaceLen=len;

		bool finished=false;
		// check if we had reached the beginning before and now are before our starting position
		if(_replaceWasAtEnd) {
			if( pos.item < _findStartPos.item ) {
				finished=true;
			}
			else if(pos.item == _findStartPos.item) {
				if(pos.part==Comment && !opts.inMsgstr && pos.offset < _findStartPos.offset+_replaceExtraOffset)
					finished=true;
				else if(pos.part==Msgstr && pos.offset < _findStartPos.offset+_replaceExtraOffset)
					finished=true;
			}
		}

		if(finished) {
			KMessageBox::information(this,i18n("%n replacement made","%n replacements made",_replacesTotal));
			return false;
		}

	}


	return true;
}


void KBabelView::find()
{
	Part hadFocus;
	if(msgidLabel->hasFocus())
		hadFocus=Msgid;
	else if(m_commentview->hasFocus())
		hadFocus=Comment;
	else
		hadFocus=Msgstr;

	if( !_findDialog ) {
		_findDialog = new FindDialog(false,this);
	}

	QString marked;
	if(msgstrEdit->hasFocus()) {
		marked=msgstrEdit->selectedText();
		msgstrEdit->selectAll(false);
	}
	else if(m_commentview->hasFocus()) {
		marked=m_commentview->selectedText();
		m_commentview->textDeselectAll();
	}
	else if(msgidLabel->hasFocus()) {
		marked=msgidLabel->selectedText();
		msgidLabel->selectAll(false);
	}


	if(marked==_lastFoundString)
		marked="";

	if( _findDialog->exec(marked) == QDialog::Accepted ) {
		DocPosition pos;
		FindOptions opts=_findDialog->findOpts();
		opts.askFile = false; // do not search in more files
		_findDialog->setFindOpts(opts);

		if(opts.fromCursor) {
			_findBreakAtEnd=false;
			pos.item=_currentIndex;

			if(hadFocus == Comment && opts.inComment) {
				pos.part = Comment;
				pos.offset = m_commentview->currentIndex();
			}
			else
			if( hadFocus == Msgid && opts.inMsgid) {
				pos.part = Msgid;
				pos.offset = msgidLabel->currentIndex();
			}
			else {
				pos.part = Msgstr;
				pos.offset = msgstrEdit->currentIndex();
			}
		}
		else {
			_findBreakAtEnd=true;

			if(opts.backwards) {
				pos.item=_catalog->numberOfEntries();
				if(opts.inComment)
					pos.part=Comment;
				else if(opts.inMsgstr)
					pos.part=Msgstr;
				else
					pos.part=Msgid;

				pos.offset=1000; // set to a high number
			}
			else {
				pos.item=0;
				if(opts.inMsgid)
					pos.part=Msgid;
				else if(opts.inMsgstr)
					pos.part=Msgstr;
				else
					pos.part=Comment;

				pos.offset=0;
			}
		}

		if( opts.backwards ) {
			_findStartPos=pos;
			findPrev_internal(pos);
		}
		else {
			_findStartPos=pos;
			findNext_internal(pos);
		}
	}
}

void KBabelView::replace()
{
	_replacesTotal=0;
	_replaceLen=0;
	_replaceWasAtEnd=false;
	_replaceExtraOffset=0;

	Part hadFocus;
	if(msgidLabel->hasFocus())
		hadFocus=Msgid;
	else if(m_commentview->hasFocus())
		hadFocus=Comment;
	else
		hadFocus=Msgstr;

	if( !_replaceDialog ) {
	_replaceDialog = new FindDialog(true,this);
	}
	QString marked;
	if(msgstrEdit->hasFocus()) {
		marked=msgstrEdit->selectedText();
		msgstrEdit->selectAll(false);
	}
	else if(m_commentview->hasFocus()) {
		marked=m_commentview->selectedText();
		m_commentview->textDeselectAll();
	}
	else if(msgidLabel->hasFocus()) {
		marked=msgidLabel->selectedText();
		msgidLabel->selectAll(false);
	}

	if(marked==_lastFoundString)
		marked="";

	if( _replaceDialog->exec(marked) == QDialog::Accepted ) {
		KBabel::ReplaceOptions opts=_replaceDialog->replaceOpts();
		if(opts.fromCursor) {
			_findBreakAtEnd=false;
			_replacePos.item=_currentIndex;

			if(hadFocus==Comment && opts.inComment) {
				_replacePos.part = Comment;
				_replacePos.offset = m_commentview->currentIndex();
			}
			else
			{
				_replacePos.part = Msgstr;
				_replacePos.offset = msgstrEdit->currentIndex();
			}
		}
		else {
			_findBreakAtEnd=true;
			if(opts.backwards) {
				_replacePos.item=_catalog->numberOfEntries();
				if(opts.inComment)
					_replacePos.part=Comment;
				else
					_replacePos.part=Msgstr;

				_replacePos.offset=1000; // set to a high number
			}
			else {
				_replacePos.item=0;
				if(opts.inMsgstr)
					_replacePos.part=Msgstr;
				else
					_replacePos.part=Comment;

				_replacePos.offset=0;
			}
		}

		// do not ask for next file from catalog manager
		opts.askFile = false;
		_replaceDialog->setReplaceOpts(opts);

		bool success;

		if( opts.backwards ) {
			_findStartPos=_replacePos;
			success = findPrev_internal(_replacePos,true,opts.ask);
		}
		else {
			_findStartPos=_replacePos;
			success = findNext_internal(_replacePos,true,opts.ask);
		}



		if(success) {
			if(!_replaceAskDialog) {
				_replaceAskDialog = new ReplaceDialog(this);
				connect(_replaceAskDialog,SIGNAL(replace()),this,SLOT(replaceNext()));
				connect(_replaceAskDialog,SIGNAL(next()),this,SLOT(findNextReplace()));
				connect(_replaceAskDialog,SIGNAL(replaceAll()),this,SLOT(replaceAll()));
			}

	      if(opts.ask) {
				_replaceAskDialog->exec();
			}
			else
				replaceAll();
		}

	}
}


void KBabelView::replaceNext()
{
    _replacesTotal++;

    KBabel::ReplaceOptions opts=_replaceDialog->replaceOpts();

    // calculate the diff to original offset due to replacing a string
    // in the starting item
    if(_findStartPos.item == _replacePos.item ) {
        if((opts.backwards && !_replaceWasAtEnd)
                || (!opts.backwards && _replaceWasAtEnd)) {
            _replaceExtraOffset += (opts.replaceStr.length()-_replaceLen);
        }
    }

   Part part;
   uint form = 0;
   QString str;

   if(_replacePos.part==Msgstr) {
        part=Msgstr;
        str = _catalog->msgstr(_replacePos.item).first()
            .mid(_replacePos.offset,_replaceLen);
	form=_replacePos.form;
    }
    else if(_replacePos.part==Comment) {
        part = Comment;
        str = _catalog->comment(_replacePos.item)
            .mid(_replacePos.offset,_replaceLen);
    }
    else {
       kdWarning() << "msgid can not be changed in KBabelView::replaceNext()"
                   << endl;
       return;
}

   _catalog->applyBeginCommand( _replacePos.item,part,this);

   DelTextCmd* delCmd = new DelTextCmd(_replacePos.offset,str,form);
   delCmd->setPart(part);
   delCmd->setIndex(_replacePos.item);
   _catalog->applyEditCommand(delCmd,0);

   InsTextCmd* insCmd = new InsTextCmd(_replacePos.offset,opts.replaceStr,form);
   insCmd->setPart(part);
   insCmd->setIndex(_replacePos.item);
   _catalog->applyEditCommand(insCmd,0);

   _catalog->applyEndCommand( _replacePos.item,part,this);

   // now find next string
    bool success;

    if( opts.backwards ) {
        success = findPrev_internal(_replacePos,true);
    }
    else {
        _replacePos.offset+=opts.replaceStr.length();
        success = findNext_internal(_replacePos,true);
    }

    if(!success) {
        if(_replaceAskDialog && _replaceAskDialog->isVisible())
            _replaceAskDialog->hide();
    }

}


void KBabelView::replaceAll()
{
	if(_replaceAskDialog && _replaceAskDialog->isVisible())
		_replaceAskDialog->hide();

	KBabel::ReplaceOptions opts=_replaceDialog->replaceOpts();
	bool success=true;

	_catalog->applyBeginCommand(_replacePos.item,Msgstr,this);

	while(success)
	{
		kapp->processEvents(100);

		_replacesTotal++;

		// calculate the diff to original offset due to replacing a string
		// in the starting item
		if(_findStartPos.item == _replacePos.item ) {
			if((opts.backwards && !_replaceWasAtEnd) || (!opts.backwards && _replaceWasAtEnd)) {
				_replaceExtraOffset += (opts.replaceStr.length()-_replaceLen);
			}
		}

		Part part;
		uint form=0;
		QString str;

		if(_replacePos.part==Msgstr) {
			part=Msgstr;
			form=_replacePos.form;
			str = _catalog->msgstr(_replacePos.item).first().mid(_replacePos.offset,_replaceLen);
		}
		else if(_replacePos.part==Comment) {
			part = Comment;
			str = _catalog->comment(_replacePos.item).mid(_replacePos.offset,_replaceLen);
		}
		else {
			kdWarning() << "msgid can not be changed in KBabelView::replaceNext()" << endl;
			return;
		}

		DelTextCmd* delCmd = new DelTextCmd(_replacePos.offset,str,form);
		delCmd->setPart(part);
		delCmd->setIndex(_replacePos.item);
		_catalog->applyEditCommand(delCmd,0);

		InsTextCmd* insCmd = new InsTextCmd(_replacePos.offset,opts.replaceStr,form);
		insCmd->setPart(part);
		insCmd->setIndex(_replacePos.item);
		_catalog->applyEditCommand(insCmd,0);


		// now find next string
		if( opts.backwards ) {
			success = findPrev_internal(_replacePos,true,false);
		}
		else {
			_replacePos.offset+=opts.replaceStr.length();
			success = findNext_internal(_replacePos,true,false);
		}
	}

	_catalog->applyEndCommand(_replacePos.item,Msgstr,this);
}

void KBabelView::findNextReplace()
{
	bool success;
	KBabel::ReplaceOptions opts=_replaceDialog->replaceOpts();

	if( opts.backwards ) {
		success = findPrev_internal(_replacePos,true);
	}
	else {
		_replacePos.offset++;
		success = findNext_internal(_replacePos,true);
	}

	if(!success) {
		if(_replaceAskDialog && _replaceAskDialog->isVisible())
			_replaceAskDialog->hide();
	}

}

void KBabelView::findInFile(QCString fileSource, FindOptions options)
{
	DocPosition pos;
	pos.item=0;
	pos.part=Msgid;
	pos.offset=0;
	_findStartPos=pos;
	_findBreakAtEnd=true; // do not start from the beginning at the end
	_showTryLaterBox=true;

	// delete dontDisplayAgain from configuration
	KConfig* config = KGlobal::config();
	KConfigGroupSaver saver(config,"Notification Messages");
	config->writeEntry("waitForNextFile",true);

	// set that there can be more files
	options.askFile = true;

	_fileSource = fileSource;

	if( !_findDialog ) _findDialog = new FindDialog(false,this);
	_findDialog->setFindOpts(options);
	findNext_internal(pos);
}

void KBabelView::replaceInFile(QCString fileSource, KBabel::ReplaceOptions options)
{
	_replacePos.item=0;
	_replacePos.part=Msgid;
	_replacePos.offset=0;

	_replacesTotal=0;
	_replaceLen=0;
	_replaceWasAtEnd=false;
	_replaceExtraOffset=0;

	_findBreakAtEnd=true;
	_showTryLaterBox=true;

	// delete dontDisplayAgain from configuration
	KConfig* config = KGlobal::config();
	KConfigGroupSaver saver(config,"Notification Messages");
	config->writeEntry("waitForNextFile",true);

	// set that there can be more files
	options.askFile = true;

	_fileSource = fileSource;

	if( !_replaceDialog ) _replaceDialog = new FindDialog(true,this);
	_replaceDialog->setReplaceOpts(options);

	bool success;

	success = findNext_internal(_replacePos,true);

	if(!success) kdDebug(KBABEL) << "Not found in file where catalog manager told us. This should not happen" << endl;
	else {
	    if(!_replaceAskDialog) {
		_replaceAskDialog = new ReplaceDialog(this);
		connect(_replaceAskDialog,SIGNAL(replace()),this,SLOT(replaceNext()));
		connect(_replaceAskDialog,SIGNAL(next()),this,SLOT(findNextReplace()));
		connect(_replaceAskDialog,SIGNAL(replaceAll()),this,SLOT(replaceAll()));
	    }

	    if(options.ask) {
		_replaceAskDialog->exec();
	    }
	    else replaceAll();
	}
}


void KBabelView::deselectAll()
{
   msgstrEdit->selectAll(false);
// FIXME:   commentEdit->selectAll(false);
   msgidLabel->selectAll(false);
}

void KBabelView::selectAll()
{
   if(msgstrEdit->hasFocus())
   {
      msgstrEdit->selectAll();
   }
// FIXME:   else if(commentEdit->hasFocus())
// FIXME:   {
// FIXME:      commentEdit->selectAll();
// FIXME:   }
   else if(msgidLabel->hasFocus())
   {
      msgidLabel->selectAll();
   }
}

void KBabelView::clear()
{
   if(msgstrEdit->hasFocus())
   {
      msgstrEdit->clear();
   }
// FIXME:   else if(commentEdit->hasFocus())
// FIXME:   {
// FIXME:      commentEdit->clear();
// FIXME:   }
}

void KBabelView::msgid2msgstr()
{
   // FIXME: should care about plural forms
   QString text = _catalog->msgid(_currentIndex).first();

   // this is KDE specific:
   if(text.find("_: NAME OF TRANSLATORS\\n")==0)
   {
       text=_catalog->identitySettings().authorLocalizedName;
   }
   else if(text.find("_: EMAIL OF TRANSLATORS\\n")==0)
   {
       text=_catalog->identitySettings().authorEmail;
   }
   else if(_catalog->isGeneratedFromDocbook() && text.find("ROLES_OF_TRANSLATORS")==0)
   {
       text="<othercredit role=\\\"translator\\\">\n"
       "<firstname></firstname><surname></surname>\n"
       "<affiliation><address><email>"+_catalog->identitySettings().authorEmail+"</email></address>\n"
       "</affiliation><contrib></contrib></othercredit>";
   }
   else if(_catalog->isGeneratedFromDocbook() && text.find("CREDIT_FOR_TRANSLATORS")==0)
   {
       text="<para>"+_catalog->identitySettings().authorLocalizedName+"\n"+
        "<email>"+_catalog->identitySettings().authorEmail+"</email></para>";
   }
   else if(text.contains(_catalog->miscSettings().singularPlural))
   {
       text.replace(_catalog->miscSettings().singularPlural,"");
   }
   // end of KDE specific part


   QRegExp reg=_catalog->miscSettings().contextInfo;
   if(text.contains(reg))
   {
      text.replace(reg,"");
   }

   modifyMsgstrText(0,text,true);

   msgstrEdit->setCursorPosition(0,0);
}

void KBabelView::search2msgstr()
{
   modifyMsgstrText(0,dictBox->translation(),true);

   msgstrEdit->setCursorPosition(0,0);
}


void KBabelView::gotoFirst()
{
    DocPosition pos;
    pos.item=0;
    pos.form=0;
    gotoEntry(pos);
}

void KBabelView::gotoLast()
{
    DocPosition pos;
    pos.item=_catalog->numberOfEntries()-1;
    pos.form=0;
    gotoEntry(pos);
}

void KBabelView::gotoNext()
{
    DocPosition pos;
    pos.item=_currentIndex;
    pos.form=msgstrEdit->currentForm();

    if( (int)pos.form+1 < _catalog->defaultNumberOfPluralForms()
	&& _catalog->pluralForm(_currentIndex)==Gettext )
    {
	pos.form++;
    }
    else
    {
	// check, if we are already showing the last entry
	if(_currentIndex>=_catalog->numberOfEntries()-1)
	{
    	    return;
	}
	else
	{
	    pos.item++;
	    pos.form=0;
	}
    }

    gotoEntry(pos);
}

void KBabelView::gotoPrev()
{
    DocPosition pos;
    pos.item=_currentIndex;
    pos.form=msgstrEdit->currentForm();
   // check, if we are already showing the first entry
    if(_currentIndex==0 && pos.form==0)
    {
       return;
    }
    else
    {
	if( pos.form==0 )
	{
	    pos.item--;
	    if( _catalog->pluralForm(pos.item)==Gettext )
		pos.form=_catalog->defaultNumberOfPluralForms()-1;
	} else pos.form--;

	gotoEntry(pos);
    }

}

void KBabelView::gotoEntry()
{
   if( !_gotoDialog )
   {
      _gotoDialog = new GotoDialog(_catalog->numberOfEntries(),this);
   }

   _gotoDialog->exec();
   if( _gotoDialog->result() )
   {
      int number=_gotoDialog->number()-1;
      int max=_catalog->numberOfEntries()-1;

      if(number>max)
          number=max;
      else
          if(number<0)
              number=0;

      DocPosition pos;
      pos.item=number;
      pos.form=0;
      gotoEntry(pos);
   }

}

void KBabelView::gotoNextFuzzy()
{
   DocPosition pos;
   if( _catalog->nextFuzzy(_currentIndex,pos) >= 0 )
   {
      gotoEntry(pos);
   }
}

void KBabelView::gotoPrevFuzzy()
{
   DocPosition pos;
   if( _catalog->prevFuzzy(_currentIndex,pos) >= 0 )
   {
      gotoEntry(pos);
   }
}

void KBabelView::gotoNextError()
{
   DocPosition pos;
   if(_catalog->nextError(_currentIndex,pos) >= 0 )
   {
      _dontBeep=true;
      gotoEntry(pos);
      _dontBeep=false;
   }
}

void KBabelView::gotoPrevError()
{
   DocPosition pos;
   if(_catalog->prevError(_currentIndex,pos) >= 0)
   {
      _dontBeep=true;
      gotoEntry(pos);
      _dontBeep=false;
   }
}

void KBabelView::gotoNextFuzzyOrUntrans()
{
   DocPosition fuzzyPos,untransPos;

   int fuzzyIndex=_catalog->nextFuzzy(_currentIndex,fuzzyPos);
   int untransIndex=_catalog->nextUntranslated(_currentIndex,untransPos);

   if(fuzzyIndex<0 && untransIndex<0 ) return;

   if(fuzzyIndex<0)
   {
      gotoEntry(untransPos);
      return;
   }
   if(untransIndex<0)
   {
      gotoEntry(fuzzyPos);
      return;
   }

   if( fuzzyIndex<untransIndex
	|| (fuzzyIndex==untransIndex && fuzzyPos.form<untransPos.form))
   {
     gotoEntry(fuzzyPos);
   }
   else
   {
     gotoEntry(untransPos);
   }
}

void KBabelView::gotoPrevFuzzyOrUntrans()
{
   DocPosition fuzzyPos,untransPos;

   int fuzzyIndex=_catalog->prevFuzzy(_currentIndex,fuzzyPos);
   int untransIndex=_catalog->prevUntranslated(_currentIndex,untransPos);

   if(fuzzyIndex<0 && untransIndex<0 ) return;

   if(fuzzyIndex<0)
   {
      gotoEntry(untransPos);
      return;
   }
   if(untransIndex<0)
   {
      gotoEntry(fuzzyPos);
      return;
   }

   if( fuzzyIndex>untransIndex
	|| (fuzzyIndex==untransIndex && fuzzyPos.form>untransPos.form))
   {
     gotoEntry(fuzzyPos);
   }
   else
   {
     gotoEntry(untransPos);
   }
}

void KBabelView::gotoNextUntranslated()
{
   DocPosition pos;
   if(_catalog->nextUntranslated(_currentIndex,pos)>=0)
   {
      gotoEntry(pos);
   }
}

void KBabelView::gotoPrevUntranslated()
{
   DocPosition pos;
   if(_catalog->prevUntranslated(_currentIndex,pos)>=0)
   {
      gotoEntry(pos);
   }
}


void KBabelView::gotoEntry(const DocPosition& pos, bool updateHistory)
{
   // clear up statusbar
   emit signalChangeStatusbar("");

   if(updateHistory)
   {
      if(_forwardHistory.count()>0)
      {
          emit signalForwardHistory(false);
      }
      _forwardHistory.clear();
      _backHistory.append(_currentIndex);
      
      if(_backHistory.count()==1)
      {
           emit signalBackHistory(true);
      }
      else if(_backHistory.count()>MAX_HISTORY)
      {
          _backHistory.remove(_backHistory.begin());
      }
   }

   informDictionary();

   _currentPos=pos;
   _currentIndex=pos.item;
   updateEditor(pos.form,true);
   emitEntryState();
   updateTags();
   updateArgs();
}

void KBabelView::msgstrPluralFormChanged ( uint index )
{
    _currentPos.form = index;
    emit signalDisplayed (_currentPos);
}

void KBabelView::backHistory()
{
   if(_backHistory.isEmpty())
   {
      kdDebug(KBABEL) << "KBabelView::backHistory called without any history." << endl;
      return;
   }

   _forwardHistory.append(_currentIndex);
   uint index=_backHistory.last();
   _backHistory.remove(_backHistory.fromLast());

   DocPosition pos;
   pos.item=index;
   pos.form=0;
   gotoEntry(pos,false);

   if(_backHistory.count()==0)
   {
      emit signalBackHistory(false);
   }
   if(_forwardHistory.count()==1)
   {
      emit signalForwardHistory(true);
   }
}

void KBabelView::forwardHistory()
{
   if(_forwardHistory.isEmpty())
   {
      kdDebug(KBABEL) << "KBabelView::forwardHistory called without any history." << endl;
      return;
   }

   _backHistory.append(_currentIndex);
   uint index=_forwardHistory.last();
   _forwardHistory.remove(_forwardHistory.fromLast());

   DocPosition pos;
   pos.item=index;
   pos.form=0;
   gotoEntry(pos,false);

   if(_forwardHistory.count()==0)
   {
      emit signalForwardHistory(false);
   }
   if(_backHistory.count()==1)
   {
      emit signalBackHistory(true);
   }
}

void KBabelView::removeFuzzyStatus()
{
   bool newState =  !_catalog->isFuzzy(_currentIndex);
   _catalog->setFuzzy(_currentIndex,newState);
   
   // ensure we will update the translation memory as needed
   msgstrEdit->setModified (true);
   emit signalFuzzyDisplayed(newState);
}


void KBabelView::editHeader()
{
   HeaderEditor* editor=_catalog->headerEditor();

   int editHeight=editor->height();
   int editWidth=editor->width();
   int w=width();
   int h=height();

   int x=w/2-editWidth/2;
   int y=h/2-editHeight/2;

   editor->move(mapToGlobal(QPoint(x,y)));

   editor->show();
   editor->raise();

}


void KBabelView::stopSearch()
{
   dictBox->stopSearch();
}

void KBabelView::startSearch()
{
   startSearch(false);
}

void KBabelView::startSearch(bool delay)
{
   QString msg = _catalog->msgid(_currentIndex,true).first();
   QRegExp reg=_catalog->miscSettings().contextInfo;
   if(msg.contains(reg))
   {
      msg.replace(reg,"");
   }

   dictBox->setActiveModule(KBabelSettings::defaultModule());
   if(delay)
   {
      dictBox->startDelayedSearch(msg);
   }
   else
   {
      dictBox->startSearch(msg);
   }
}

void KBabelView::startSearch(const QString module)
{
   // FIXME: should care about plural forms
   QString msg = _catalog->msgid(_currentIndex,true).first();
   QRegExp reg=_catalog->miscSettings().contextInfo;
   if(msg.contains(reg))
   {
      msg.replace(reg,"");
   }

   dictBox->setActiveModule(module);
   dictBox->startSearch(msg);

}

void KBabelView::startSelectionSearch()
{
    startSelectionSearch(KBabelSettings::defaultModule());
}

void KBabelView::startSelectionSearch(const QString module)
{
   dictBox->setActiveModule(module);

   if(msgidLabel->hasSelectedText())
   {
      // TODO: should we care about end of lines?
      dictBox->startSearch(msgidLabel->selectedText());
   }
   else if(msgstrEdit->hasSelectedText())
   {
      dictBox->startTranslationSearch(msgstrEdit->selectedText());
   }
   else
   {
       // should care about plural forms
       QString msg = _catalog->msgid(_currentIndex,true).first();
       QRegExp reg=_catalog->miscSettings().contextInfo;
       if(msg.contains(reg))
       {
          msg.replace(reg,"");
       }

       dictBox->startSearch(msg);
   }
}


void KBabelView::emitEntryState()
{
   // flag, so I don't have to always change the color of the text
   static bool isError=false;

   emit signalDisplayed(_currentPos);

   emit signalFirstDisplayed(_currentIndex==0, msgstrEdit->currentForm()==0);
   emit signalLastDisplayed((unsigned)(_currentIndex+1)==_catalog->numberOfEntries(), (signed)(msgstrEdit->currentForm())==_catalog->numberOfPluralForms(_currentIndex)-1);

   bool fuzzy=_catalog->isFuzzy(_currentIndex);
   bool untrans=_catalog->isUntranslated(_currentIndex);
   emit signalFuzzyDisplayed(fuzzy);
   emit signalUntranslatedDisplayed(untrans);
   emit signalFuzzyAfterwards(_catalog->hasFuzzyAfterwards(_currentIndex));
   emit signalUntranslatedAfterwards(_catalog->hasUntranslatedAfterwards(_currentIndex));
   emit signalFuzzyInFront(_catalog->hasFuzzyInFront(_currentIndex));
   emit signalUntranslatedInFront(_catalog->hasUntranslatedInFront(_currentIndex));

   emit signalErrorAfterwards(_catalog->hasErrorAfterwards(_currentIndex));
   emit signalErrorInFront(_catalog->hasErrorInFront(_currentIndex));

   DocPosition pos;
   if( _catalog->hasError(_currentIndex,pos) != isError )
   {
      isError = !isError;

      emit signalFaultyDisplayed(isError);

      if(isError)
      {
         QPalette palette=msgstrEdit->palette();
         palette.setColor( QColorGroup::Text, red );

	 if( _catalog->itemStatus(_currentIndex).contains("syntax error"))
         {
             msgstrEdit->setCurrentColor( MsgMultiLineEdit::ErrorColor );
         }
         else
	 if( !_catalog->itemStatus(_currentIndex).isEmpty() && KBabelSettings::autoCheckColorError())
         {
             msgstrEdit->setCurrentColor( MsgMultiLineEdit::ErrorColor );
         }
      }
      else
      {
         msgstrEdit->setCurrentColor( MsgMultiLineEdit::NormalColor );
      }
   }

}

void KBabelView::checkFuzzies()
{
   emit signalFuzzyAfterwards(_catalog->hasFuzzyAfterwards(_currentIndex));
   emit signalFuzzyInFront(_catalog->hasFuzzyInFront(_currentIndex));
}

void KBabelView::checkUntranslated()
{
   emit signalUntranslatedAfterwards(_catalog->hasUntranslatedAfterwards(_currentIndex));
   emit signalUntranslatedInFront(_catalog->hasUntranslatedInFront(_currentIndex));
}

void KBabelView::autoRemoveFuzzyStatus()
{
   // only at first text change remove fuzzy status
   disconnect(msgstrEdit,SIGNAL(textChanged()),this,SLOT(autoRemoveFuzzyStatus()));

   //removeFuzzyStatus();
}

void KBabelView::toggleFuzzyLed(bool on)
{
   if(!_fuzzyLed)
     return;

   if(on)
   {
      if(_fuzzyLed->state()==KLed::Off)
      {
         _fuzzyLed->on();
      }
   }
   else
   {
      if(_fuzzyLed->state()==KLed::On)
          _fuzzyLed->off();
   }
}

void KBabelView::toggleUntransLed(bool on)
{
   if(!_untransLed)
     return;

   if(on)
   {
      if(_untransLed->state()==KLed::Off)
         _untransLed->on();
   }
   else
   {
      if(_untransLed->state()==KLed::On)
         _untransLed->off();
   }
}

void KBabelView::toggleErrorLed(bool on)
{
   if(!_errorLed)
     return;

   if(on)
   {
      if(_errorLed->state()==KLed::Off)
      {
         _errorLed->on();
      }
   }
   else
   {
      if(_errorLed->state()==KLed::On)
          _errorLed->off();
   }
}




void KBabelView::showError(const QString& message)
{
    KMessageBox::error(this,message);
}

void KBabelView::dragEnterEvent(QDragEnterEvent *event)
{
    // accept uri drops only
    event->accept(KURLDrag::canDecode(event));
}

void KBabelView::dropEvent(QDropEvent *event)
{
    KURL::List uri;

    // see if we can decode a URI.. if not, just ignore it
    if (KURLDrag::decode(event, uri))
    {
       processUriDrop(uri , mapToGlobal(event->pos()));
    }
}


bool KBabelView::eventFilter( QObject* object, QEvent* event)
{
   if(event->type() ==  QEvent::DragEnter)
   {
      QDragEnterEvent* e = (QDragEnterEvent*) event;
      if(KURLDrag::canDecode(e))
      {
         e->accept(true);
         return true;
      }
   }
   else if(event->type() == QEvent::Drop)
   {
        KURL::List uri;
      // see if we can decode a URI.. if not, just ignore it
      if (KURLDrag::decode((QDropEvent*)event, uri))
      {
          processUriDrop(uri ,( (QWidget*)object)->mapToGlobal( ( (QDropEvent*)event )->pos()));
          return true;
      }
   }
   else if(event->type() == QEvent::KeyPress)
   {
      // This is a little workaround to use CTRL+ALT+Home, CTRL+ALT+End, Undo keys
      // to go to the first and the last entry. Because otherwise
      // CTRL+Home and CTRL+End and Undo are caught by QTextEdit
      QKeyEvent *ke = (QKeyEvent*)event;

      if(ke->key() == Key_Home  && ke->state() == (AltButton | ControlButton))
      {
         gotoFirst();
         return true;
      }
      else if(ke->key() == Key_End
                   && ke->state() == (AltButton | ControlButton))
      {
         gotoLast();
         return true;
      }
      else if( KShortcut(KKey(ke)) == KStdAccel::undo()  )
      {
         undo();
	 return true;
      }
      else if( KShortcut(KKey(ke)) == KStdAccel::redo()  )
      {
         redo();
	 return true;
      }
      else if( ke->key() == Key_Insert )
      {
        m_mainwindow->toggleEditMode();
	return true;
      }
   }

   return false;
}

void KBabelView::processUriDrop(KURL::List& uriList, const QPoint& pos)
{
     // if we have two entries, the chance is high, that it
     // is a drag from the catalog manager
     if(uriList.count() == 2)
     {
        int result = _dropMenu->exec(pos);
        switch(result)
        {
           case ID_DROP_OPEN:
           {
              KURL first(uriList.first());
              KURL second(uriList.last());

              if( KIO::NetAccess::exists(first, true, this) )
              {
                 open(first);
              }
              else
              {
                 openTemplate(second,first);
              }
              break;
           }
           case ID_DROP_OPEN_TEMPLATE:
           {
              open(uriList.last());
              break;
           }
        }
     }
     else
     {
        // okay, we have one URI.. process it
        KURL url( uriList.first() );

         // load in the file
         open(url);
     }
}

void KBabelView::forwardMsgstrEditCmd(EditCommand* cmd)
{
/*
   if(cmd->terminator()!=0)
   {
	kdDebug(KBABEL) << QString::number(cmd->terminator()) << endl;
   }
   else
   {
      DelTextCmd* delcmd = (DelTextCmd*) cmd;
      kdDebug(KBABEL) << QString::number(delcmd->offset)+":"+delcmd->str+"|" << endl;
   }
*/
   bool fuzzyRemoved=false;
   if(KBabelSettings::autoUnsetFuzzy() && _catalog->isFuzzy(_currentIndex) )
   {
      fuzzyRemoved=true;

      _catalog->applyBeginCommand(_currentIndex,Msgstr,this);

      removeFuzzyStatus();
   }

   cmd->setPart(Msgstr);
   cmd->setIndex(_currentIndex);

   bool wasUntranslated=_catalog->isUntranslated(_currentIndex);

   _catalog->applyEditCommand(cmd,this);

   if( fuzzyRemoved )
   {
      _catalog->applyEndCommand(_currentIndex,Msgstr,this);
   }


   bool isUntranslated=_catalog->isUntranslated(_currentIndex);

   if(wasUntranslated && !isUntranslated)
      emit signalUntranslatedDisplayed(false);
   else if(!wasUntranslated && isUntranslated)
      emit signalUntranslatedDisplayed(true);

}

void KBabelView::autoCheck()
{
    autoCheck(true);
}


void KBabelView::autoCheck(bool onlyWhenChanged)
{
   if( !_autocheckTools.isEmpty() )
   {
        QStringList oldStatus = _catalog->itemStatus(_currentIndex);

	QStringList status = _catalog->itemStatus( _currentIndex,true, _autocheckTools );

	// if there is more than one view, the status changes only in
	// one view, so we have to update always.
	if(_catalog->isLastView() && onlyWhenChanged && oldStatus == status)
		return;

	if( !status.isEmpty() )
	{
	    QString msg = "";

            // ### TODO: whynot use i18n("context",text) directly?
	    KLocale* locale=KGlobal::locale();

	    for( QStringList::iterator it=status.begin() ; it != status.end() ; ++it )
	    {
		if( msg.isEmpty() ) msg = locale->translate("what check found errors",(*it).utf8());
		else msg += ", "+locale->translate("what check found errors",(*it).utf8());
	    }

	    //i18n: translators: Status bar text that automatic checks have found some errors
	    emit signalChangeStatusbar(i18n("1 error: %1", "%n errors: %1", status.size ()).arg(msg));
	    emit signalFaultyDisplayed(true);

	    if(KBabelSettings::autoCheckColorError())
	    {
		msgstrEdit->setCurrentColor( MsgMultiLineEdit::ErrorColor );
	    }

   	    if(KBabelSettings::beepOnError() && !_dontBeep)
	    {
		if(onlyWhenChanged)
		{
		    if(oldStatus != status && oldStatus.isEmpty() )
		    {
			KNotifyClient::beep();
		    }
		}
		else if(isActiveWindow())
		{
			KNotifyClient::beep();
		}
	    }
	}
	else if( _catalog->itemStatus(_currentIndex).isEmpty() )
	{
	    _catalog->removeFromErrorList(_currentIndex);

	    emit signalFaultyDisplayed(false);

	    if(KBabelSettings::autoCheckColorError())
	    {
		msgstrEdit->setCurrentColor( MsgMultiLineEdit::NormalColor );
	    }
	}
   }
}

void KBabelView::spellcheckAll()
{
	spell.what2Check=All;
	spellcheck();
}

void KBabelView::spellcheckAllMulti()
{
	spell.what2Check=AllMulti;
	spellcheck();
}


void KBabelView::spellcheckFromCursor()
{
	spell.what2Check=End;
	spellcheck();
}


void KBabelView::spellcheckCurrent()
{
	spell.what2Check=Current;
	spellcheck();
}

void KBabelView::spellcheckFromCurrent()
{
	spell.what2Check=BeginCurrent;
	spellcheck();
}


void KBabelView::spellcheckMarked()
{
	if(!msgstrEdit->hasSelectedText())
	{
		return;
	}

	spell.what2Check=Marked;
	spellcheck();
}


void KBabelView::spellcheckCommon()
{
	SpellDlg *spellDlg = new SpellDlg(msgstrEdit->hasSelectedText(),this
					,"SpellDlg");

	if(spellDlg->exec())
	{
		if(spellDlg->all())
			spell.what2Check=All;
		else if(spellDlg->current())
			spell.what2Check=Current;
		else if(spellDlg->begin())
			spell.what2Check=Begin;
		else if(spellDlg->end())
			spell.what2Check=End;
		else if(spellDlg->marked())
			spell.what2Check=Marked;
		else if(spellDlg->beginCurrent())
			spell.what2Check=BeginCurrent;
		else
		{
			kdError() << "unhandled option in spell dialog" << endl;
			return;
		}

		spellcheck();
	}

	delete spellDlg;
}

void KBabelView::addSpellcheckWords( uint pos, QString text, uint index, uint form )
{
    // special format chars
    QString spclChars="abfnrtv'\"?\\";
    QChar accelMarker=_catalog->miscSettings().accelMarker;

    uint textLength=text.length();
    do
    {
	QString word="";
	bool wordBegin=false;
	while(!wordBegin && pos < textLength)
	{
	    QChar c=text[pos];
	    if(c.isLetter() || c==accelMarker)
	    {
		wordBegin=true;
	    }
	    else if( c == '\\')
	    {
		if(pos+1 < textLength && spclChars.contains(text[pos+1]) )
		{
		    pos+=2;
		}
		else
		{
		    // consider it to be unnecessary escaped character
		    pos++;
		}
	    }
	    else
	    {
		pos++;
	    }
	}
	int begin=pos;

	bool wordEnd=false;
	while(!wordEnd && pos < textLength)
	{
	    if(text[pos].isLetter() || text[pos]=='-' || (text[pos]=='\'' && !spell.config->dictionary().startsWith("malti")))
	    {
		word+=text[pos];
		pos++;
	    }
	    else if(text[pos]==accelMarker)
	    {
		pos++;
	    }
	    else if(text[pos]=='\n')
	    {
		// newline without \n counts as nothing
		pos++;
	    }
	    else
	    {
		wordEnd=true;
	    }
	}

	int end=pos;

	// remove '-' and accelMarker at the end of a word
	while(text[end]=='-' || (text[pos]=='\'' && !spell.config->dictionary().startsWith("malti"))
        		|| text[end]==accelMarker)
	{
	    end--;
	    word.truncate(word.length()-1);
	}

	if(!word.isEmpty())
	{
	    spell.wordList.append(word);
	    Position *pos = new Position;
	    pos->index=index;
	    pos->form=form;
	    pos->pos=begin;
	    pos->end=end;
	    spell.posDict.append(pos);
	}
    }
    while(pos < textLength);
}

void KBabelView::spellcheck()
{
	if(isReadOnly() || spell.active)
		return;

	spell.wordList.clear();
	spell.posDict.clear();
	spell.ignoreList.clear();
	spell.newIgnoreList.clear();

	spell.misspelled=0;
	spell.replaced=0;
	spell.lastIndex=0;
	spell.posCorrection=0;
	spell.lastPos=0;
	spell.inWordCorrection=0;

	if( !_spellcheckSettings.valid ) 
	{
	    _spellcheckSettings = _project->spellcheckSettings();
	}

	spell.config = new KSpellConfig(this,"tempSpellConfig");
	spell.config->setNoRootAffix(_spellcheckSettings.noRootAffix);
	spell.config->setRunTogether(_spellcheckSettings.runTogether);
	spell.config->setClient(_spellcheckSettings.spellClient);
	spell.config->setEncoding(_spellcheckSettings.spellEncoding);
	spell.config->setDictionary(_spellcheckSettings.spellDict);

	if(spell.what2Check==Marked)
	{
	    spell.lastIndex=_currentIndex;

	    _tagExtractor->setString(msgstrEdit->selectedText());
	    QString marked=_tagExtractor->plainString(true);

	    addSpellcheckWords(msgstrEdit->beginOfMarkedText(),marked
		,_currentIndex,msgstrEdit->currentForm());
	}
	else
	{
	    uint first=0;
	    uint last=_catalog->numberOfEntries()-1;
	    QString text;

	    bool emitProgress=false;

	    if(spell.what2Check==All || spell.what2Check==Begin
		|| spell.what2Check==End || spell.what2Check==AllMulti
		|| spell.what2Check==BeginCurrent)
	    {
		emitProgress=true;
	    }

	    if(spell.what2Check==Begin)
	    {
		first=0;
		last=_currentIndex-1;
	    }
	    else if(spell.what2Check==End)
	    {
		first=_currentIndex+1;
		last=_catalog->numberOfEntries()-1;

		int pos=msgstrEdit->currentIndex();
		int form=msgstrEdit->currentForm();

		QStringList msgs = _catalog->msgstr(_currentIndex);
		_tagExtractor->setString((*msgs.at(form)));
		text=_tagExtractor->plainString(true);
		addSpellcheckWords( pos, text, _currentIndex, form++ );

		for( QStringList::Iterator i=msgs.at(form++) ; i!=msgs.end(); i++)
		{
		    _tagExtractor->setString(*i);
		    text=_tagExtractor->plainString(true);
		    addSpellcheckWords( pos, text, _currentIndex, form++ );
		}
	    }
	    else if(spell.what2Check==BeginCurrent)
	    {
		first=_currentIndex;
		last=_catalog->numberOfEntries()-1;
	    }
	    else if(spell.what2Check!=All && spell.what2Check!=AllMulti)
	    {
		first=last=_currentIndex;
	    }

	    if(emitProgress)
	    {
		emit signalResetProgressBar(i18n("Preparing spell check"),100);
		kapp->processEvents(100);
	    }

	    uint total=last-first+1;
	    uint lastPercent=0;
	    for(uint i=first; i <= last; i++)
	    {
		if(emitProgress && 100*i/ QMAX(total,1) > lastPercent)
		{
		    lastPercent++;
		    emit signalProgress(lastPercent);

		    kapp->processEvents(100);
		}

		QStringList msgs=_catalog->msgstr(i);
		uint formCounter=0;
		for(QStringList::Iterator j=msgs.begin() ; j!=msgs.end() ; ++j)
		{
		    _tagExtractor->setString(*j);
		    text=_tagExtractor->plainString(true);
		    addSpellcheckWords(0,text,i,formCounter++);
		}
	    }

	    if(spell.what2Check==Begin)
	    {
		int pos=msgstrEdit->currentIndex();
		int form=msgstrEdit->currentForm();

		QStringList msgs = _catalog->msgstr(_currentIndex);
		_tagExtractor->setString((*msgs.at(form)).left(pos));
		text=_tagExtractor->plainString(true);
		addSpellcheckWords( 0, text, _currentIndex, form++ );

		for( QStringList::Iterator i=msgs.at(form++) ; i!=msgs.end(); i++)
		{
		    _tagExtractor->setString(*i);
		    text=_tagExtractor->plainString(true);
		    addSpellcheckWords( 0, text, _currentIndex, form++ );
		}
	    }

	    if(emitProgress)
	    {
		emit signalClearProgressBar();
	    }
	}

	if(!spell.wordList.isEmpty())
	{
		spell.active=true;
		_dontBeep=true;

		spell.kspell= new KSpell (this, i18n("Spellcheck"),
					  this, SLOT(spellStart(KSpell *)), spell.config, true, true);
		if( spell.kspell->status() == KSpell::Error )
		{
		    KMessageBox::error( this, i18n("KBabel cannot start spell checker. "
			"Please verify your KDE installation.") );
		    return;
		}

		connect(spell.kspell, SIGNAL(death()),this, SLOT(spellCleanDone()));

		connect(spell.kspell, SIGNAL(misspelling(const QString &, const QStringList &
						, unsigned int)), this
					, SLOT(spellMisspelled(const QString &, const QStringList &, unsigned int)));

		connect(spell.kspell, SIGNAL(corrected(const QString &, const QString &, unsigned int))
					, this, SLOT(spellCorrected(const QString &, const QString &, unsigned int)));

    		connect(spell.kspell,SIGNAL(ignoreall(const QString &))
                    , this, SLOT(spellAddIgnore(const QString &)));

		connect(spell.kspell, SIGNAL(done(bool))
					, this, SLOT(spellResult(bool)));

		spell.kspell->setAutoDelete(true); // let KSpell handle delete
	}
	else
	{
		KMessageBox::information(this,i18n(
					"No relevant text has been found for spell checking."));
	}
}

void KBabelView::spellStart(KSpell *)
{
	// set ignored words
	if(_spellcheckSettings.rememberIgnored)
	{
		QString urlString = _spellcheckSettings.ignoreURL;
		if(urlString.contains("@PACKAGE@"))
		{
			urlString.replace("@PACKAGE@",_catalog->packageName());
		}
                // ### TODO: correctly set the URL; support for MostLocalURL
		KURL url(urlString);
		if(url.isLocalFile())
		{
			QFile file(url.path());
			if(file.open(IO_ReadOnly))
			{
				QTextStream stream(&file);
				stream.setEncoding(QTextStream::UnicodeUTF8);
				QString contents = stream.read();
				file.close();

				spell.ignoreList = QStringList::split('\n',contents);
			}
			else if(file.exists())
			{
				KMessageBox::sorry(this,
				            i18n("Error opening the file that contains words "
				            "to ignore during spell checking:\n"
							"%1").arg(file.name()));
			}
		}
		else
		{
			KMessageBox::sorry(this,
			            i18n("Only local files are allowed for saving "
			                 "ignored words to during spell checking:\n"
							 "%1").arg(urlString));
		}

		if(spell.ignoreList.count() > 0)
		{
			emit signalResetProgressBar(i18n("Preparing spell check"),100);
			kapp->processEvents(100);

			uint total = spell.ignoreList.count();
			uint oldPercent=0;
			uint counter=0;
			QStringList::Iterator it;
			for(it=spell.ignoreList.begin(); it != spell.ignoreList.end(); ++it)
			{
				counter++;
				if(counter/total > oldPercent)
				{
					oldPercent++;
					emit signalProgress(oldPercent);
					kapp->processEvents(100);
				}

				spell.kspell->ignore(*it);
			}

			emit signalClearProgressBar();
		}
	}

	spell.kspell->checkList(&spell.wordList);
}


bool KBabelView::markMisspelled(const QString &orig, unsigned int pos)
{
	Position *p = spell.posDict.at(pos);
	if(!p)
	{
		kdError() << "not a valid position: " << pos << endl;
		return false;
	}

	if(p->index != _currentIndex || (p->index==_currentIndex && p->form!=msgstrEdit->currentForm()))
	{
	    DocPosition pos;
	    pos.item=p->index;
	    pos.form=p->form;
	    gotoEntry(pos);
	}


	if(p->index != spell.lastIndex)
	{
		spell.lastIndex=p->index;
		spell.posCorrection=0;
	}
	if(pos != spell.lastPos)
	{
		spell.lastPos=pos;
		spell.inWordCorrection=0;
	}

	int x=0;
	int y=0;

	int begin=p->pos+spell.posCorrection-spell.inWordCorrection;
	int end=p->end+spell.posCorrection-spell.inWordCorrection;

	// check if this is the correct word
	QString text = *_catalog->msgstr(p->index).at(p->form);
	text=text.mid(begin,end-begin);
	QChar accelMarker=_catalog->miscSettings().accelMarker;

	if(text.contains(accelMarker))
	{
		text.replace(accelMarker,"");
	}
	if(text.contains('\n'))
	{
		text.replace("\n","");
	}

	bool textOk=true;
	if(text != orig)
	{
		// if text and orig are not the same,
		// maybe it was a word with hyphens
		int n=text.contains('-');
		n+=text.contains('\'');
		if( n > 0 )
		{
			// re-get the original text since we replace some things above
			QString text = *_catalog->msgstr(p->index).at(p->form);
			text=text.mid(begin,end-begin);

			bool textFound=false;
			int i = 0;
			int e=-1;
			while(!textFound && i <= n)
			{
				int lastPos=e+1;
				e = text.find('-',lastPos);
            			int tmp = text.find('\'',lastPos);
            			if(e < 0 && tmp > 0)
            			{
                		    e=tmp;
            			}
            			else if(e > 0 && tmp > 0 && tmp < e)
            			{
                		    e=tmp;
            			}

				if(e<0) e=text.length();

				QString w=text.mid(lastPos,e-lastPos);
				if(w.contains(accelMarker))
				{
					w.replace(accelMarker,"");
				}
				if(text.contains('\n'))
				{
					text.replace("\n","");
				}
				if( w == orig)
				{
					textFound=true;
					end=begin+e;
					begin=begin+lastPos;
				}

				i++;
			}

			if(!textFound)
			{
				textOk=false;
			}
		}
		else
		{
			textOk=false;
		}
	}

	int beginx, beginy;

	msgstrEdit->offset2Pos(end,y,x);
	msgstrEdit->offset2Pos(begin,beginy,beginx);
	msgstrEdit->setSelection(beginy,beginx,y,x);

	if(!textOk)
	{
		text = *_catalog->msgstr(p->index).at(p->form);
		text=text.mid(begin,end-begin);
		kdDebug(KBABEL) << "Sync error: given: " << orig << " have: " << text << endl;
		cancelSpellcheck();

		KMessageBox::error(this,i18n(
				"There seems to be an error with the synchronization "
				"of the spell checking process and KBabel.\n"
				"Please check that you have set the correct settings for "
				"your language for spell checking.\n"
				"If you have, and this problem is reproducible, please "
				"send a detailed bug report (your spell checking options, "
				"what file you have checked and what to do to reproduce "
				"the problem) by using Help->Report Bug..."));
	}

	return textOk;
}

void KBabelView::spellMisspelled(const QString &orig, const QStringList &, unsigned int pos)
{
	kdDebug(KBABEL) << "misspelled: " << orig << " pos: " << pos << endl;

	spell.misspelled++;

	markMisspelled(orig,pos);
}

void KBabelView::spellCorrected(const QString &orig, const QString &word, unsigned int pos)
{
	if(orig != word)
	{
		QString newWord(word);
		kdDebug(KBABEL) << "corrected: " << orig << " " << newWord
				  << " pos: " << pos << endl;

		if(spell.replaced==0)
		{
			// handle the spell check as one action
			int index;
			Position *p = spell.posDict.at(pos);
			if(p)
			{
				index=p->index;
			}
			else
			{
				index=_currentIndex;
			}

			_catalog->applyBeginCommand(index,Msgstr,this);
		}

		spell.replaced++;


		if(markMisspelled(orig,pos))
		{
			QString marked=msgstrEdit->selectedText();
			spell.origWords.append(marked);

			if(marked.contains("\n") && !newWord.contains('\n'))
			{
				QString s1=newWord;
				s1.replace(" ","\n");

				// if only a newline has been replaced with a white space
				if(s1==marked)
				{
					newWord.replace(" "," \n");
				}

			}
			// check if the old word had an accelerator. If yes and the new
			// word has no accelerator, try to add the accelerator for
			// the same char else add in at the same position
			QChar accelMarker=_catalog->miscSettings().accelMarker;
			if(marked.contains(accelMarker) && !newWord.contains(accelMarker))
			{
				int b=marked.find(accelMarker);
				QChar accel=marked[b+1];
				int nb=newWord.find(accel,0,false);
				if(nb>=0)
				{
					newWord.insert(nb,accelMarker);
				}
				// if the new word does not contain the old accel char
				// set the accelerator to the same position as the old
				else
				{
					if((uint)b >= newWord.length())
						b = 0;
					newWord.insert(b,accelMarker);
				}
			}

			spell.newWords.append(newWord);

			msgstrEdit->cut();
			int row=0;
			int col=0;
			msgstrEdit->getCursorPosition(&row,&col);
			msgstrEdit->insertAt(newWord,row,col,true);

			int newCorrection = newWord.length() - marked.length();
			spell.posCorrection += newCorrection;
			spell.inWordCorrection += newCorrection;

			// now store the new position
			Position *p = spell.posDict.at(pos);
			if(p)
			{
				p->end=p->end+newCorrection;
			}
		}
	}
}


void KBabelView::spellResult(bool flag)
{
	kdDebug(KBABEL) << "spellResult: " << flag << endl;


	if(spell.replaced > 0)
	{
		// close the spell check action
		_catalog->applyEndCommand(spell.lastIndex,Msgstr,this);
	}


	if(flag)
	{
		emit signalChangeStatusbar(i18n("Spellcheck: %n word replaced","Spellcheck: %n words replaced",spell.replaced));

		if(spell.misspelled==0)
		{
			KMessageBox::information(this,i18n(
							"Spellcheck successfully finished.\n"
							"No misspelled words have been found."));
		}
		else if(spell.replaced > 0 && spell.what2Check!=Current
                                   && spell.what2Check!=Marked)
		{
			QStringList list;
			QStringList::Iterator origIt;
			QStringList::Iterator newIt;
			origIt=spell.origWords.begin();
			newIt=spell.newWords.begin();

			for(;origIt != spell.origWords.end()
							&& newIt != spell.newWords.end(); origIt++,newIt++)
			{
				list.append(*origIt+" -> "+*newIt);
			}

		}

		if( spell.what2Check!=AllMulti && spell.misspelled!=0 )
		    KMessageBox::information(this,i18n("Spellcheck: %n word replaced","Spellcheck: %n words replaced",spell.replaced));

		if(_spellcheckSettings.rememberIgnored)
		{
			if(spell.newIgnoreList.count() > 0)
			{
				KURL url(_spellcheckSettings.ignoreURL);
				if(url.isLocalFile())
				{
					QFile file(url.path());
					if(file.open(IO_WriteOnly|IO_Append))
					{
						QStringList::Iterator it;
						QTextStream stream(&file);
						stream.setEncoding(QTextStream::UnicodeUTF8);

						for(it=spell.newIgnoreList.begin();
                                it!=spell.newIgnoreList.end();
										++it)
						{
							stream << *it << "\n";
						}

						file.close();
					}
				}
				else
				{
					kdDebug(KBABEL) << "only local files are supported for storing"
							  << "ignored words" << endl;
				}
			}
		}
	}
	else
	{
		emit signalChangeStatusbar(i18n("Spellcheck canceled"));
		if(spell.replaced > 0)
			undo();
	}

	int s=spell.kspell->dlgResult();
	spell.kspell->cleanUp();

	emit signalSpellcheckDone(s);
	QTimer::singleShot(0,this,SLOT(cleanUpSpellStruct()));
}


void KBabelView::spellCleanDone()
{
	kdDebug(KBABEL) << "spellCleanDone" << endl;

	// if the pointer is cleared, you have finished correcly
	if( !spell.kspell ) return;

	KSpell::spellStatus status = spell.kspell->status();

	if(status == KSpell::Error || status == KSpell::Crashed)
	{
	    cleanUpSpellStruct();
	}

	if(status == KSpell::Error)
	{
            KMessageBox::sorry(this, i18n("The spell checker program could not be started.\n"
                "Please make sure you have the spell checker program properly "
                "configured and in your PATH."));
	}
	else if(status == KSpell::Crashed)
	{
		KMessageBox::sorry(this, i18n("The spell checker program seems to have crashed."));
	}
}

void KBabelView::cleanUpSpellStruct()
{
    kdDebug(KBABEL) << "Cleaning structure" << endl;
    // spell.kspell is set to be autodeleted
    spell.kspell = 0;
    delete spell.config;
    spell.config=0;
    spell.wordList.clear();
    spell.posDict.clear();
    spell.origWords.clear();
    spell.newWords.clear();
    spell.ignoreList.clear();
    spell.newIgnoreList.clear();
    spell.active = false;
    _dontBeep=false;
}

void KBabelView::cancelSpellcheck()
{
	spell.active=false;
}

void KBabelView::spellAddIgnore(const QString &word)
{
    if(!spell.ignoreList.contains(word))
    {
        spell.newIgnoreList.append(word);
    }
}

void KBabelView::forwardSearchStart()
{
   emit signalResetProgressBar(i18n("Searching"),100);
   emit signalSearchActive(true);
}

void KBabelView::forwardSearchStop()
{
   emit signalClearProgressBar();
   emit signalSearchActive(false);
}

void KBabelView::forwardProgressStart(const QString& msg)
{
   emit signalResetProgressBar(msg,100);
}

void KBabelView::slotAutoSaveTimeout( )
{
  if ( isModified( ) )
  {
    autoSaveTimer->stop( );
    saveFile( false );
    autoSaveTimer->start( 1000 * 60 * _autoSaveDelay );
  }
}

void KBabelView::useProject (Project::Ptr project)
{
    // FIXME: close the current project first
    disconnect (_project, SIGNAL(signalSpellcheckSettingsChanged()),
	this, SLOT(updateProjectSettings()));
    
    _project = project;
    _catalog->useProject(_project);

    readProject(_project);

    connect (project, SIGNAL(signalSpellcheckSettingsChanged()),
	this, SLOT(updateProjectSettings()));
}

#include "kbabelview.moc"
