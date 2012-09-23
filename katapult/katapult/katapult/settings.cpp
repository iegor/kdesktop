#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file '/home/doros/src/local/kde/katapult/katapult/settings.ui'
**
** Created: Fri Feb 18 12:48:28 2005
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "settings.h"

#include <qvariant.h>
#include <qlistbox.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a MyDialog1 as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
MyDialog1::MyDialog1( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "MyDialog1" );
    setSizeGripEnabled( TRUE );
    MyDialog1Layout = new QGridLayout( this, 1, 1, 11, 6, "MyDialog1Layout"); 

    listBox = new QListBox( this, "listBox" );
    listBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)7, 0, 0, listBox->sizePolicy().hasHeightForWidth() ) );

    MyDialog1Layout->addWidget( listBox, 0, 0 );

    tabWidget = new QTabWidget( this, "tabWidget" );

    Widget2 = new QWidget( tabWidget, "Widget2" );
    tabWidget->insertTab( Widget2, QString("") );

    Widget3 = new QWidget( tabWidget, "Widget3" );
    tabWidget->insertTab( Widget3, QString("") );

    MyDialog1Layout->addWidget( tabWidget, 0, 1 );

    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 

    buttonHelp = new QPushButton( this, "buttonHelp" );
    buttonHelp->setAutoDefault( TRUE );
    Layout1->addWidget( buttonHelp );
    Horizontal_Spacing2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( Horizontal_Spacing2 );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout1->addWidget( buttonOk );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
    Layout1->addWidget( buttonCancel );

    MyDialog1Layout->addMultiCellLayout( Layout1, 1, 1, 0, 1 );
    languageChange();
    resize( QSize(597, 364).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
MyDialog1::~MyDialog1()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void MyDialog1::languageChange()
{
    setCaption( tr2i18n( "MyDialog1" ) );
    listBox->clear();
    listBox->insertItem( tr2i18n( "New Item" ) );
    tabWidget->changeTab( Widget2, tr2i18n( "Tab" ) );
    tabWidget->changeTab( Widget3, tr2i18n( "Tab" ) );
    buttonHelp->setText( tr2i18n( "&Help" ) );
    buttonHelp->setAccel( QKeySequence( tr2i18n( "F1" ) ) );
    buttonOk->setText( tr2i18n( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    buttonCancel->setText( tr2i18n( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
}

#include "settings.moc"
