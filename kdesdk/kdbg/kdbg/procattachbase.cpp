#include <kdialog.h>
#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file '../../exper/kdbg/procattachbase.ui'
**
** Created: Wed May 20 21:24:33 2009
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "procattachbase.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qtoolbutton.h>
#include <qheader.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a ProcAttachBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
ProcAttachBase::ProcAttachBase( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "ProcAttachBase" );
    setSizeGripEnabled( TRUE );
    ProcAttachBaseLayout = new QHBoxLayout( this, 10, 6, "ProcAttachBaseLayout"); 

    layout7 = new QVBoxLayout( 0, 0, 6, "layout7"); 

    layout6 = new QHBoxLayout( 0, 0, 6, "layout6"); 

    filterLabel = new QLabel( this, "filterLabel" );
    layout6->addWidget( filterLabel );

    filterEdit = new QLineEdit( this, "filterEdit" );
    filterEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)5, 0, 0, filterEdit->sizePolicy().hasHeightForWidth() ) );
    filterEdit->setMaxLength( 20 );
    layout6->addWidget( filterEdit );

    filterClear = new QToolButton( this, "filterClear" );
    layout6->addWidget( filterClear );
    layout7->addLayout( layout6 );

    processList = new QListView( this, "processList" );
    processList->addColumn( tr2i18n( "Command" ) );
    processList->addColumn( tr2i18n( "PID" ) );
    processList->addColumn( tr2i18n( "PPID" ) );
    processList->setMinimumSize( QSize( 300, 200 ) );
    processList->setAllColumnsShowFocus( TRUE );
    layout7->addWidget( processList );

    layout3 = new QHBoxLayout( 0, 0, 6, "layout3"); 

    buttonRefresh = new QPushButton( this, "buttonRefresh" );
    buttonRefresh->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, buttonRefresh->sizePolicy().hasHeightForWidth() ) );
    layout3->addWidget( buttonRefresh );
    spacingBtns = new QSpacerItem( 242, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout3->addItem( spacingBtns );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setEnabled( FALSE );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    layout3->addWidget( buttonOk );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
    layout3->addWidget( buttonCancel );
    layout7->addLayout( layout3 );
    ProcAttachBaseLayout->addLayout( layout7 );
    languageChange();
    resize( QSize(560, 416).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( buttonRefresh, SIGNAL( clicked() ), this, SLOT( refresh() ) );
    connect( filterEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( filterEdited(const QString&) ) );
    connect( filterClear, SIGNAL( clicked() ), filterEdit, SLOT( clear() ) );
    connect( processList, SIGNAL( selectionChanged() ), this, SLOT( selectedChanged() ) );

    // tab order
    setTabOrder( filterEdit, processList );
    setTabOrder( processList, buttonRefresh );
    setTabOrder( buttonRefresh, buttonOk );
    setTabOrder( buttonOk, buttonCancel );

    // buddies
    filterLabel->setBuddy( filterEdit );
}

/*
 *  Destroys the object and frees any allocated resources
 */
ProcAttachBase::~ProcAttachBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ProcAttachBase::languageChange()
{
    setCaption( tr2i18n( "Attach to Process" ) );
    filterLabel->setText( tr2i18n( "&Filter or PID:" ) );
    QWhatsThis::add( filterEdit, tr2i18n( "Type the name of the process or its process ID (PID) here to reduce the number of entries in the list." ) );
    filterClear->setText( tr2i18n( "..." ) );
    QToolTip::add( filterClear, tr2i18n( "Clear filter" ) );
    QWhatsThis::add( filterClear, tr2i18n( "Use this button to clear the filter text so that all processes are displayed." ) );
    processList->header()->setLabel( 0, tr2i18n( "Command" ) );
    processList->header()->setLabel( 1, tr2i18n( "PID" ) );
    processList->header()->setLabel( 2, tr2i18n( "PPID" ) );
    QWhatsThis::add( processList, tr2i18n( "<p>This list displays all processes that are currently running. You must select the process that you want KDbg to attach to. Use the <b>Filter or PID</b> edit box to reduce the number of entries in this list.<p>The text in the <i>Command</i> column is usually, but not always, the command that was used to start the process. The <i>PID</i> column shows the process ID. The <i>PPID</i> column shows the process ID of the parent process. Additional columns show more information about the processes that is also available via the system's <i>ps</i> command.</p><p>The list is not updated automatically. Use the <b>Refresh</b> button to update it.</p>" ) );
    buttonRefresh->setText( tr2i18n( "&Refresh" ) );
    QWhatsThis::add( buttonRefresh, tr2i18n( "This button updates the list of processes." ) );
    buttonOk->setText( tr2i18n( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    QWhatsThis::add( buttonOk, tr2i18n( "You must select a process from the list. Then click the <b>OK</b> button to attach to that process." ) );
    buttonCancel->setText( tr2i18n( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
}

#include "procattachbase.moc"
