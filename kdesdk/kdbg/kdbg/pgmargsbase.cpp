#include <kdialog.h>
#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file '../../exper/kdbg/pgmargsbase.ui'
**
** Created: Wed May 20 21:24:33 2009
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "pgmargsbase.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qheader.h>
#include <qlistview.h>
#include <qlistbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a PgmArgsBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
PgmArgsBase::PgmArgsBase( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "PgmArgsBase" );
    setSizeGripEnabled( TRUE );
    PgmArgsBaseLayout = new QHBoxLayout( this, 11, 6, "PgmArgsBaseLayout"); 

    layout1 = new QVBoxLayout( 0, 0, 6, "layout1"); 

    tabWidget = new QTabWidget( this, "tabWidget" );

    argsPage = new QWidget( tabWidget, "argsPage" );
    argsPageLayout = new QHBoxLayout( argsPage, 11, 6, "argsPageLayout"); 

    layout2 = new QVBoxLayout( 0, 0, 6, "layout2"); 

    labelArgs = new QLabel( argsPage, "labelArgs" );
    layout2->addWidget( labelArgs );

    programArgs = new QLineEdit( argsPage, "programArgs" );
    layout2->addWidget( programArgs );

    layout3 = new QHBoxLayout( 0, 0, 6, "layout3"); 

    insertFile = new QPushButton( argsPage, "insertFile" );
    layout3->addWidget( insertFile );

    insertDir = new QPushButton( argsPage, "insertDir" );
    layout3->addWidget( insertDir );
    spacer1 = new QSpacerItem( 61, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout3->addItem( spacer1 );
    layout2->addLayout( layout3 );
    spacer2 = new QSpacerItem( 81, 180, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout2->addItem( spacer2 );
    argsPageLayout->addLayout( layout2 );
    tabWidget->insertTab( argsPage, QString::fromLatin1("") );

    wdPage = new QWidget( tabWidget, "wdPage" );
    wdPageLayout = new QHBoxLayout( wdPage, 11, 6, "wdPageLayout"); 

    layout6 = new QVBoxLayout( 0, 0, 6, "layout6"); 

    wdEdit = new QLineEdit( wdPage, "wdEdit" );
    layout6->addWidget( wdEdit );

    layout5 = new QHBoxLayout( 0, 0, 6, "layout5"); 

    wdBrowse = new QPushButton( wdPage, "wdBrowse" );
    layout5->addWidget( wdBrowse );
    spacer4 = new QSpacerItem( 321, 31, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout5->addItem( spacer4 );
    layout6->addLayout( layout5 );
    spacer5 = new QSpacerItem( 111, 161, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout6->addItem( spacer5 );
    wdPageLayout->addLayout( layout6 );
    tabWidget->insertTab( wdPage, QString::fromLatin1("") );

    envPage = new QWidget( tabWidget, "envPage" );
    envPageLayout = new QHBoxLayout( envPage, 11, 6, "envPageLayout"); 

    layout9 = new QHBoxLayout( 0, 0, 6, "layout9"); 

    layout7 = new QVBoxLayout( 0, 0, 6, "layout7"); 

    envLabel = new QLabel( envPage, "envLabel" );
    layout7->addWidget( envLabel );

    envVar = new QLineEdit( envPage, "envVar" );
    layout7->addWidget( envVar );

    envList = new QListView( envPage, "envList" );
    envList->addColumn( tr2i18n( "Name" ) );
    envList->header()->setClickEnabled( FALSE, envList->header()->count() - 1 );
    envList->addColumn( tr2i18n( "Value" ) );
    envList->header()->setClickEnabled( FALSE, envList->header()->count() - 1 );
    envList->setSelectionMode( QListView::Single );
    layout7->addWidget( envList );
    layout9->addLayout( layout7 );

    layout8 = new QVBoxLayout( 0, 0, 6, "layout8"); 

    buttonModify = new QPushButton( envPage, "buttonModify" );
    layout8->addWidget( buttonModify );

    buttonDelete = new QPushButton( envPage, "buttonDelete" );
    layout8->addWidget( buttonDelete );
    spacer6 = new QSpacerItem( 51, 141, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout8->addItem( spacer6 );
    layout9->addLayout( layout8 );
    envPageLayout->addLayout( layout9 );
    tabWidget->insertTab( envPage, QString::fromLatin1("") );

    xsldbgOptionsPage = new QWidget( tabWidget, "xsldbgOptionsPage" );
    xsldbgOptionsPageLayout = new QHBoxLayout( xsldbgOptionsPage, 11, 6, "xsldbgOptionsPageLayout"); 

    xsldbgOptions = new QListBox( xsldbgOptionsPage, "xsldbgOptions" );
    xsldbgOptions->setSelectionMode( QListBox::Multi );
    xsldbgOptionsPageLayout->addWidget( xsldbgOptions );
    tabWidget->insertTab( xsldbgOptionsPage, QString::fromLatin1("") );
    layout1->addWidget( tabWidget );

    layout4 = new QHBoxLayout( 0, 0, 6, "layout4"); 

    buttonHelp = new QPushButton( this, "buttonHelp" );
    buttonHelp->setAutoDefault( TRUE );
    layout4->addWidget( buttonHelp );
    spacer3 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout4->addItem( spacer3 );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    layout4->addWidget( buttonOk );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
    layout4->addWidget( buttonCancel );
    layout1->addLayout( layout4 );
    PgmArgsBaseLayout->addLayout( layout1 );
    languageChange();
    resize( QSize(528, 410).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( insertFile, SIGNAL( clicked() ), this, SLOT( browseArgFile() ) );
    connect( wdBrowse, SIGNAL( clicked() ), this, SLOT( browseWd() ) );
    connect( buttonModify, SIGNAL( clicked() ), this, SLOT( modifyVar() ) );
    connect( buttonDelete, SIGNAL( clicked() ), this, SLOT( deleteVar() ) );
    connect( envList, SIGNAL( selectionChanged() ), this, SLOT( envListCurrentChanged() ) );
    connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( invokeHelp() ) );
    connect( insertDir, SIGNAL( clicked() ), this, SLOT( browseArgDir() ) );

    // tab order
    setTabOrder( envVar, envList );
    setTabOrder( envList, buttonModify );
    setTabOrder( buttonModify, buttonDelete );
    setTabOrder( buttonDelete, programArgs );
    setTabOrder( programArgs, insertFile );
    setTabOrder( insertFile, insertDir );
    setTabOrder( insertDir, buttonHelp );
    setTabOrder( buttonHelp, buttonOk );
    setTabOrder( buttonOk, buttonCancel );
    setTabOrder( buttonCancel, tabWidget );
    setTabOrder( tabWidget, wdEdit );
    setTabOrder( wdEdit, wdBrowse );

    // buddies
    labelArgs->setBuddy( programArgs );
    envLabel->setBuddy( envVar );
}

/*
 *  Destroys the object and frees any allocated resources
 */
PgmArgsBase::~PgmArgsBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void PgmArgsBase::languageChange()
{
    setCaption( tr2i18n( "Program Arguments" ) );
    labelArgs->setText( tr2i18n( "Run <i>%1</i> with these arguments:" ) );
    QWhatsThis::add( programArgs, tr2i18n( "Specify the arguments with which the program shall be invoked for this debugging session. You specify the arguments just as you would on the command line, that is, you can even use quotes and environment variables, for example:<p><tt>--message 'start in: ' $HOME</tt>" ) );
    insertFile->setText( tr2i18n( "Insert &file name..." ) );
    insertFile->setAccel( QKeySequence( tr2i18n( "Alt+F" ) ) );
    QWhatsThis::add( insertFile, tr2i18n( "Browse for a file; the full path name will be inserted at the current cursor location in the edit box above." ) );
    insertDir->setText( tr2i18n( "Insert &directory name..." ) );
    insertDir->setAccel( QKeySequence( tr2i18n( "Alt+D" ) ) );
    QWhatsThis::add( insertDir, tr2i18n( "Browse for a directory; the full path name will be inserted at the current cursor location in the edit box above." ) );
    tabWidget->changeTab( argsPage, tr2i18n( "&Arguments" ) );
    QWhatsThis::add( wdEdit, tr2i18n( "Specify here the initial working directory where the program is run." ) );
    wdBrowse->setText( tr2i18n( "&Browse..." ) );
    wdBrowse->setAccel( QKeySequence( tr2i18n( "Alt+B" ) ) );
    QWhatsThis::add( wdBrowse, tr2i18n( "Browse for the initial working directory where the program is run." ) );
    tabWidget->changeTab( wdPage, tr2i18n( "&Working Directory" ) );
    envLabel->setText( tr2i18n( "Environment variables (<tt>NAME=value</tt>):" ) );
    QWhatsThis::add( envVar, tr2i18n( "To add a new environment variable or to modify one, specify it here in the form <tt>NAME=value</tt> and click <b>Modify</b>." ) );
    envList->header()->setLabel( 0, tr2i18n( "Name" ) );
    envList->header()->setLabel( 1, tr2i18n( "Value" ) );
    QWhatsThis::add( envList, tr2i18n( "Environment variables that are set <i>in addition</i> to those that are inherited are listed in this table. To add new environment variables, specify them as <tt>NAME=value</tt> in the edit box above and click <b>Modify</b>. To modify a value, select it in this list and click <b>Modify</b>. To delete an environment variable, select it in this list and click <b>Delete</b>." ) );
    buttonModify->setText( tr2i18n( "&Modify" ) );
    buttonModify->setAccel( QKeySequence( tr2i18n( "Alt+M" ) ) );
    QWhatsThis::add( buttonModify, tr2i18n( "Enters the environment variable that is currently specified in the edit box into the list. If the named variable is already in the list, it receives a new value; otherwise, a new entry is created." ) );
    buttonDelete->setText( tr2i18n( "&Delete" ) );
    buttonDelete->setAccel( QKeySequence( tr2i18n( "Alt+D" ) ) );
    QWhatsThis::add( buttonDelete, tr2i18n( "Deletes the selected environment variable from the list. This cannot be used to delete environment variables that are inherited." ) );
    tabWidget->changeTab( envPage, tr2i18n( "&Environment" ) );
    tabWidget->changeTab( xsldbgOptionsPage, tr2i18n( "&xsldbg Options" ) );
    buttonHelp->setText( tr2i18n( "&Help" ) );
    buttonHelp->setAccel( QKeySequence( tr2i18n( "F1" ) ) );
    buttonOk->setText( tr2i18n( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    buttonCancel->setText( tr2i18n( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
}

#include "pgmargsbase.moc"
