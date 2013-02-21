// KDat - a tar-based DAT archiver
// Copyright (C) 1998-2000  Sean Vyain, svyain@mail.tds.net
// Copyright (C) 2001-2002  Lawrence Widman, kdat@cardiothink.com
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <stdlib.h>

#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <kpushbutton.h>
#include <kstdguiitem.h>

#include <kapplication.h>
#include <kglobal.h>
#include <klocale.h>

#include "Options.h"
#include "FormatOptDlg.h"

#include "FormatOptDlg.moc"

FormatOptDlg::FormatOptDlg( const QString & def, QWidget* parent, const char* name )
        : QDialog( parent, name, TRUE )
{
    setIconText( i18n( "KDat: Format Options" ) );
    setCaption( i18n( "KDat: Format Options" ) );

    QLabel* lbl1 = new QLabel( i18n( "Tape name:" ), this );
    QLabel* lbl2 = new QLabel( i18n( "Tape size:" ), this );

    int max = lbl1->sizeHint().width();
    if ( lbl2->sizeHint().width() > max ) max = lbl2->sizeHint().width();

    lbl1->setFixedSize( max, lbl1->sizeHint().height() );
    lbl2->setFixedSize( max, lbl2->sizeHint().height() );

    _entry = new QLineEdit( this );
    _entry->setText( def );
    _entry->setFixedHeight( _entry->sizeHint().height() );

    _tapeSize = new QLineEdit( this );
    _tapeSize->setFixedSize( 48, _tapeSize->sizeHint().height() );

    _tapeSizeUnits = new QComboBox( this );
    _tapeSizeUnits->setFixedSize( 48, _tapeSizeUnits->sizeHint().height() );
    _tapeSizeUnits->insertItem( "MB" );
    _tapeSizeUnits->insertItem( "GB" );

    KPushButton* ok     = new KPushButton( KStdGuiItem::ok(), this );
    ok->setFixedSize( 80, ok->sizeHint().height() );
    KPushButton* cancel = new KPushButton( KStdGuiItem::cancel(), this );
    cancel->setFixedSize( 80, cancel->sizeHint().height() );

    QVBoxLayout* l1 = new QVBoxLayout( this, 8, 4 );
    QHBoxLayout* l2 = new QHBoxLayout();
    QHBoxLayout* l3 = new QHBoxLayout();
    QHBoxLayout* l4 = new QHBoxLayout();

    l1->addLayout( l2, 0 );
    l1->addLayout( l3, 0 );
    l1->addStretch( 1 );

    l1->addLayout( l4, 0 );

    l2->addWidget( lbl1, 0 );
    l2->addWidget( _entry, 1 );

    l3->addWidget( lbl2 );
    l3->addWidget( _tapeSize );
    l3->addWidget( _tapeSizeUnits );
    l3->addStretch( 1 );

    l4->addStretch( 1 );
    l4->addWidget( ok, 0 );
    l4->addWidget( cancel, 0 );

    resize( 400, 120 );

    _entry->setFocus();
    _entry->selectAll();

    connect( _entry, SIGNAL( returnPressed() ), this, SLOT( okClicked() ) );
    connect( ok    , SIGNAL( clicked() )      , this, SLOT( okClicked() ) );
    connect( cancel, SIGNAL( clicked() )      , this, SLOT( reject() ) );

    int size = Options::instance()->getDefaultTapeSize();
    if ( ( size >= 1024*1024 ) && ( size % ( 1024*1024 ) == 0 ) ) {
        // GB
        size /= 1024*1024;
        _tapeSizeUnits->setCurrentItem( 1 );
    } else {
        // MB
        size /= 1024;
        _tapeSizeUnits->setCurrentItem( 0 );
    }
    _tapeSize->setText( KGlobal::locale()->formatNumber(size, 0) );
}

FormatOptDlg::~FormatOptDlg()
{
}

void FormatOptDlg::okClicked()
{
    _name = _entry->text();

    _size = (int)KGlobal::locale()->readNumber( _tapeSize->text() );
    if ( _tapeSizeUnits->currentItem() == 0 ) {
        // MB
        _size *= 1024;
    } else {
        // GB
        _size *= 1024*1024;
    }
    
    accept();
}

QString FormatOptDlg::getName()
{
    return _name;
}

int FormatOptDlg::getSize()
{
    return _size;
}
