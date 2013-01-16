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

#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>

#include <kapplication.h>
#include <kpushbutton.h>
#include <kstdguiitem.h>

#include "BackupProfile.h"
#include "BackupProfileInfoWidget.h"
#include "BackupProfileWidget.h"
#include "KDatMainWindow.h"
#include "Options.h"
#include <klocale.h>

#include "BackupProfileInfoWidget.moc"

BackupProfileInfoWidget::BackupProfileInfoWidget( QWidget* parent, const char* name )
        : QWidget( parent, name ),
          _backupProfile( 0 )
{
    QLabel* lbl1 = new QLabel( i18n( "Backup profile name:" ), this );

    int max = lbl1->sizeHint().width();

    lbl1->setFixedSize( max, lbl1->sizeHint().height() );

    _name = new QLineEdit( this );
    _name->setFixedHeight( _name->sizeHint().height() );

    _profile = new BackupProfileWidget( this );

    QPushButton* getSelection = new QPushButton( i18n( "Files >>" ), this );
    getSelection->setFixedSize( 80, getSelection->sizeHint().height() );

    QPushButton* setSelection = new QPushButton( i18n( "<< Files" ), this );
    setSelection->setFixedSize( 80, setSelection->sizeHint().height() );

    _apply = new KPushButton( KStdGuiItem::apply(), this );
    _apply->setFixedSize( 80, _apply->sizeHint().height() );
    _apply->setEnabled( FALSE );

    QVBoxLayout* l1 = new QVBoxLayout( this, 4, 4 );

    QHBoxLayout* l1_1 = new QHBoxLayout();
    l1->addLayout( l1_1 );
    l1_1->addWidget( lbl1 );
    l1_1->addWidget( _name, 1 );

    l1->addWidget( _profile, 1 );

    QHBoxLayout* l1_2 = new QHBoxLayout();
    l1->addLayout( l1_2 );
    l1_2->addWidget( setSelection );
    l1_2->addWidget( getSelection );
    l1_2->addStretch( 1 );
    l1_2->addWidget( _apply );

    connect( setSelection, SIGNAL( clicked() )                 , this, SLOT( slotSetSelection() ) );
    connect( getSelection, SIGNAL( clicked() )                 , this, SLOT( slotGetSelection() ) );
    connect( _name       , SIGNAL( textChanged( const QString & ) ), this, SLOT( slotTextChanged( const QString & ) ) );
    connect( _profile    , SIGNAL( sigSomethingChanged() )     , this, SLOT( slotSomethingChanged() ) );
    connect( _apply      , SIGNAL( clicked() )                 , this, SLOT( slotApply() ) );
}

BackupProfileInfoWidget::~BackupProfileInfoWidget()
{
}

void BackupProfileInfoWidget::setBackupProfile( BackupProfile* backupProfile )
{
    _backupProfile = backupProfile;

    if ( !_backupProfile ) {
        return;
    }

    _name->setText( _backupProfile->getName() );

    _profile->setBackupProfile( _backupProfile );
}

bool BackupProfileInfoWidget::isModified()
{
    if ( _backupProfile->getName() != _name->text() ) {
        return TRUE;
    }

    if ( _profile->getArchiveName() != _backupProfile->getArchiveName() ) {
        return TRUE;
    }

    QString one = _backupProfile->getWorkingDirectory();
    QString two = _profile->getWorkingDirectory();
    // 7/31/01: this breaks
    //    if ( _profile->getWorkingDirectory() != _backupProfile->getWorkingDirectory() ) {
    if( one != two ){
        return TRUE;
    }

    if ( _profile->getAbsoluteFiles().count() != _backupProfile->getAbsoluteFiles().count() ) {
        return TRUE;
    }

    QStringList list1 = _profile->getAbsoluteFiles();
    QStringList list2 = _backupProfile->getAbsoluteFiles();
    QStringList::Iterator i = list1.begin();
    QStringList::Iterator j = list2.begin();
    for ( ; i != list1.end(); ++i ) {
        for ( ; j != list2.end(); ++j ) {
            if ( *i == *j ) {
                break;
            }
        }
        if ( j == list2.end() ) {
            // Could not find i.current() in j => lists are not equal.
            return TRUE;
        }
    }

    if ( _profile->isOneFilesystem() != _backupProfile->isOneFilesystem() ) {
        return TRUE;
    }

    if ( _profile->isIncremental() != _backupProfile->isIncremental() ) {
        return TRUE;
    }

    if ( _profile->getSnapshotFile() != _backupProfile->getSnapshotFile() ) {
        return TRUE;
    }

    if ( _profile->getRemoveSnapshot() != _backupProfile->getRemoveSnapshot() ) {
        return TRUE;
    }

    return FALSE;
}

void BackupProfileInfoWidget::slotTextChanged( const QString & )
{
    if ( !_backupProfile ) {
        return;
    }

    _apply->setEnabled( isModified() );
}

void BackupProfileInfoWidget::slotSomethingChanged()
{
    if ( !_backupProfile ) {
        return;
    }

    _apply->setEnabled( isModified() );
}

void BackupProfileInfoWidget::slotApply()
{
    if ( !_backupProfile ) {
        return;
    }

    _backupProfile->setName( _name->text() );
    _backupProfile->setArchiveName( _profile->getArchiveName() );
    _backupProfile->setWorkingDirectory( _profile->getWorkingDirectory() );
    _backupProfile->setAbsoluteFiles( _profile->getAbsoluteFiles() );
    _backupProfile->setOneFilesystem( _profile->isOneFilesystem() );
    _backupProfile->setIncremental( _profile->isIncremental() );
    _backupProfile->setSnapshotFile( _profile->getSnapshotFile() );
    _backupProfile->setRemoveSnapshot( _profile->getRemoveSnapshot() );
    
    _backupProfile->save();

    _apply->setEnabled( FALSE );
}

void BackupProfileInfoWidget::slotSetSelection()
{
    KDatMainWindow::getInstance()->setBackupFiles( _profile->getAbsoluteFiles() );
}

void BackupProfileInfoWidget::slotGetSelection()
{
    QStringList files;
    KDatMainWindow::getInstance()->getBackupFiles( files );
    _profile->setAbsoluteFiles( files );
}
