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
#include <time.h>

#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>

#include <kapplication.h>
#include <kglobal.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <kstdguiitem.h>

#include "Options.h"
#include "Tape.h"
#include "TapeInfoWidget.h"
#include "Util.h"

#include "TapeInfoWidget.moc"

TapeInfoWidget::TapeInfoWidget( QWidget* parent, const char* name )
        : QWidget( parent, name ),
          _tape( 0 )
{
    QLabel* lbl1 = new QLabel( i18n( "Tape name:" ), this );
    QLabel* lbl2 = new QLabel( i18n( "Tape size:" ), this );
    QLabel* lbl3 = new QLabel( i18n( "Tape ID:" ), this );
    QLabel* lbl4 = new QLabel( i18n( "Created on:" ), this );
    QLabel* lbl5 = new QLabel( i18n( "Last modified:" ), this );
    QLabel* lbl6 = new QLabel( i18n( "Archive count:" ), this );
    QLabel* lbl7 = new QLabel( i18n( "Space used:" ), this );

    int max = lbl1->sizeHint().width();
    if ( lbl2->sizeHint().width() > max ) max = lbl2->sizeHint().width();
    if ( lbl3->sizeHint().width() > max ) max = lbl3->sizeHint().width();
    if ( lbl4->sizeHint().width() > max ) max = lbl4->sizeHint().width();
    if ( lbl5->sizeHint().width() > max ) max = lbl5->sizeHint().width();
    if ( lbl6->sizeHint().width() > max ) max = lbl6->sizeHint().width();
    if ( lbl7->sizeHint().width() > max ) max = lbl7->sizeHint().width();

    lbl1->setFixedSize( max, lbl1->sizeHint().height() );
    lbl2->setFixedSize( max, lbl2->sizeHint().height() );
    lbl3->setFixedSize( max, lbl3->sizeHint().height() );
    lbl4->setFixedSize( max, lbl4->sizeHint().height() );
    lbl5->setFixedSize( max, lbl5->sizeHint().height() );
    lbl6->setFixedSize( max, lbl6->sizeHint().height() );
    lbl7->setFixedSize( max, lbl7->sizeHint().height() );

    _tapeName = new QLineEdit( this );
    _tapeName->setFixedHeight( _tapeName->sizeHint().height() );

    _tapeSize = new QLineEdit( this );
    _tapeSize->setFixedSize( 48, _tapeSize->sizeHint().height() );

    _tapeSizeUnits = new QComboBox( this );
    _tapeSizeUnits->setFixedSize( 48, _tapeSizeUnits->sizeHint().height() );
    _tapeSizeUnits->insertItem( "MB" );
    _tapeSizeUnits->insertItem( "GB" );

    _tapeID = new QLabel( "???", this );
    _tapeID->setFixedHeight( _tapeID->sizeHint().height() );

    _ctime = new QLabel( "???", this );
    _ctime->setFixedHeight( _ctime->sizeHint().height() );

    _mtime = new QLabel( "???", this );
    _mtime->setFixedHeight( _mtime->sizeHint().height() );

    _archiveCount = new QLabel( "???", this );
    _archiveCount->setFixedHeight( _archiveCount->sizeHint().height() );

    _spaceUsed = new QLabel( "???", this );
    _spaceUsed->setFixedHeight( _spaceUsed->sizeHint().height() );

    _apply = new KPushButton( KStdGuiItem::apply(), this );
    _apply->setFixedSize( 80, _apply->sizeHint().height() );
    _apply->setEnabled( FALSE );

    QVBoxLayout* l1 = new QVBoxLayout( this, 4, 4 );

    QHBoxLayout* l1_1 = new QHBoxLayout();
    l1->addLayout( l1_1 );
    l1_1->addWidget( lbl1 );
    l1_1->addWidget( _tapeName, 1 );

    QHBoxLayout* l1_2 = new QHBoxLayout();
    l1->addLayout( l1_2 );
    l1_2->addWidget( lbl2 );
    l1_2->addWidget( _tapeSize );
    l1_2->addWidget( _tapeSizeUnits );
    l1_2->addStretch( 1 );

    QHBoxLayout* l1_3 = new QHBoxLayout();
    l1->addLayout( l1_3 );
    l1_3->addWidget( lbl3 );
    l1_3->addWidget( _tapeID );

    QHBoxLayout* l1_4 = new QHBoxLayout();
    l1->addLayout( l1_4 );
    l1_4->addWidget( lbl4 );
    l1_4->addWidget( _ctime );

    QHBoxLayout* l1_5 = new QHBoxLayout();
    l1->addLayout( l1_5 );
    l1_5->addWidget( lbl5 );
    l1_5->addWidget( _mtime );

    QHBoxLayout* l1_6 = new QHBoxLayout();
    l1->addLayout( l1_6 );
    l1_6->addWidget( lbl6 );
    l1_6->addWidget( _archiveCount );

    QHBoxLayout* l1_7 = new QHBoxLayout();
    l1->addLayout( l1_7 );
    l1_7->addWidget( lbl7 );
    l1_7->addWidget( _spaceUsed );

    l1->addStretch( 1 );

    QHBoxLayout* l1_8 = new QHBoxLayout();
    l1->addLayout( l1_8 );
    l1_8->addStretch( 1 );
    l1_8->addWidget( _apply );

    connect( _tapeName     , SIGNAL( textChanged( const QString & ) ), this, SLOT( slotTextChanged( const QString & ) ) );
    connect( _tapeSize     , SIGNAL( textChanged( const QString & ) ), this, SLOT( slotTextChanged( const QString & ) ) );
    connect( _tapeSizeUnits, SIGNAL( activated( int ) )          , this, SLOT( slotActivated( int ) ) );
    connect( _apply        , SIGNAL( clicked() )                 , this, SLOT( slotApply() ) );
}

TapeInfoWidget::~TapeInfoWidget()
{
}

void TapeInfoWidget::setTape( Tape* tape )
{
    _tape = tape;

    if ( !_tape ) {
        return;
    }

    _tapeName->setText( _tape->getName() );

    int size = _tape->getSize();
    if ( ( size >= 1024*1024 ) && ( size % ( 1024*1024 ) == 0 ) ) {
        // GB
        size /= 1024*1024;
        _tapeSizeUnits->setCurrentItem( 1 );
    } else {
        // MB
        size /= 1024;
        _tapeSizeUnits->setCurrentItem( 0 );
    }
    QString tmp;
    tmp.setNum( size );
    _tapeSize->setText( tmp );

    _tapeID->setText( _tape->getID() );

    time_t tm = _tape->getCTime();
    tmp = ctime( &tm );
    tmp = tmp.stripWhiteSpace();
    _ctime->setText( tmp );

    tm = _tape->getMTime();
    tmp = ctime( &tm );
    tmp = tmp.stripWhiteSpace();
    _mtime->setText( tmp );

    tmp.setNum( _tape->getChildren().count() );
    _archiveCount->setText( tmp );

    QPtrListIterator<Archive> i( _tape->getChildren() );
    int used = 1;
    for ( ; i.current(); ++i ) {
        used += i.current()->getEndBlock();
    }
    int blockSize = Options::instance()->getTapeBlockSize();
    if ( blockSize < 1024 ) {
        used /= 1024 / blockSize;
    } else if ( blockSize > 1024 ) {
        used *= blockSize / 1024;
    }
    if ( _tape->getSize() > 0 ) {
        tmp = QString::fromLatin1( "%1 / %2 (%3%)")
          .arg(Util::kbytesToString( used ))
          .arg(Util::kbytesToString( _tape->getSize() ))
          .arg(used * 100 / _tape->getSize() );
    } else {
        tmp = Util::kbytesToString( used );
    }
    _spaceUsed->setText( tmp );
}

bool TapeInfoWidget::isModified()
{
    if ( _tape->getName() != _tapeName->text() ) {
        return TRUE;
    }

    int size = (int)KGlobal::locale()->readNumber( _tapeSize->text() );
    if ( _tapeSizeUnits->currentItem() == 0 ) {
        // MB
        size *= 1024;
    } else {
        // GB
        size *= 1024*1024;
    }

    return _tape->getSize() != size;
}

void TapeInfoWidget::slotTextChanged( const QString & )
{
    if ( !_tape ) {
        return;
    }

    _apply->setEnabled( isModified() );
}

void TapeInfoWidget::slotActivated( int )
{
    if ( !_tape ) {
        return;
    }

    _apply->setEnabled( isModified() );
}

void TapeInfoWidget::slotApply()
{
    if ( !_tape ) {
        return;
    }

    int size = (int)KGlobal::locale()->readNumber( _tapeSize->text() );
    if ( _tapeSizeUnits->currentItem() == 0 ) {
        // MB
        size *= 1024;
    } else {
        // GB
        size *= 1024*1024;
    }

    if ( _tape->getName() != _tapeName->text() ) {
        _tape->setName( _tapeName->text() );
    }

    if ( size != _tape->getSize() ) {
        _tape->setSize( size );
    }

    _apply->setEnabled( FALSE );
}
