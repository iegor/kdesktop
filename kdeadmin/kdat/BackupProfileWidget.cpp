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

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>

#include <kapplication.h>

#include "BackupProfile.h"
#include "BackupProfileWidget.h"
#include "Util.h"
#include <klocale.h>

#include "BackupProfileWidget.moc"

BackupProfileWidget::BackupProfileWidget( QWidget* parent, const char* name )
        : KTabCtl( parent, name )
{
    QWidget* one = new QWidget( this );
    addTab( one, i18n( "Backup" ) );

    QLabel* lbl1 = new QLabel( i18n( "Archive name:" ), one );
    lbl1->setFixedSize( lbl1->sizeHint() );

    _archiveName = new QLineEdit( one );
    _archiveName->setFixedHeight( _archiveName->sizeHint().height() );

    QLabel* lbl2 = new QLabel( i18n( "Working folder:" ), one );
    lbl2->setFixedSize( lbl2->sizeHint() );

    _workingDir = new QComboBox( FALSE, one );
    _workingDir->setFixedHeight( _workingDir->sizeHint().height() );

    QLabel* lbl3 = new QLabel( i18n( "Backup files:" ), one );
    lbl3->setFixedHeight( lbl3->sizeHint().height() );

    _files = new QListBox( one );

    QWidget* two = new QWidget( this );
    addTab( two, i18n( "Tar Options" ) );

    _oneFilesystem = new QCheckBox( i18n( "Stay on one filesystem" ), two );
    _oneFilesystem->setFixedHeight( _oneFilesystem->sizeHint().height() );

    _incremental = new QCheckBox( i18n( "GNU listed incremental" ), two );
    _incremental->setFixedHeight( _incremental->sizeHint().height() );
    connect( _incremental, SIGNAL( toggled( bool ) ), this, SLOT( slotIncrementalToggled( bool ) ) );

    _snapshotLabel = new QLabel( i18n( "Snapshot file:" ), two );
    _snapshotLabel->setFixedSize( _snapshotLabel->sizeHint() );

    _snapshotFile = new QLineEdit( two );
    _snapshotFile->setFixedHeight( _snapshotFile->sizeHint().height() );

    _removeSnapshot = new QCheckBox( i18n( "Remove snapshot file before backup" ), two );
    _removeSnapshot->setFixedHeight( _removeSnapshot->sizeHint().height() );

    slotIncrementalToggled( FALSE );

    QVBoxLayout* l1 = new QVBoxLayout( one, 8, 4 );

    QHBoxLayout* l1_1 = new QHBoxLayout();
    l1->addLayout( l1_1 );
    l1_1->addWidget( lbl1 );
    l1_1->addWidget( _archiveName, 1 );

    QHBoxLayout* l1_2 = new QHBoxLayout();
    l1->addLayout( l1_2 );
    l1_2->addWidget( lbl2 );
    l1_2->addWidget( _workingDir, 1 );

    l1->addWidget( lbl3 );
    l1->addWidget( _files, 1 );

    QVBoxLayout* l2 = new QVBoxLayout( two, 8, 4 );
    l2->addWidget( _oneFilesystem );
    l2->addWidget( _incremental );

    QHBoxLayout* l2_1 = new QHBoxLayout();
    l2->addLayout( l2_1 );
    l2_1->addSpacing( 20 );
    l2_1->addWidget( _snapshotLabel );
    l2_1->addWidget( _snapshotFile, 1 );

    QHBoxLayout* l2_2 = new QHBoxLayout();
    l2->addLayout( l2_2 );
    l2_2->addSpacing( 20 );
    l2_2->addWidget( _removeSnapshot );

    l2->addStretch( 1 );

    connect( _archiveName   , SIGNAL( textChanged( const QString & ) ), this, SLOT( slotTextChanged( const QString & ) ) );
    connect( _workingDir    , SIGNAL( activated( const QString & ) )  , this, SLOT( slotWorkingDirActivated( const QString & ) ) );
    connect( _oneFilesystem , SIGNAL( toggled( bool ) )           , this, SLOT( slotToggled( bool ) ) );
    connect( _incremental   , SIGNAL( toggled( bool ) )           , this, SLOT( slotIncrementalToggled( bool ) ) );
    connect( _snapshotFile  , SIGNAL( textChanged( const QString & ) ), this, SLOT( slotTextChanged( const QString & ) ) );
    connect( _removeSnapshot, SIGNAL( toggled( bool ) )           , this, SLOT( slotToggled( bool ) ) );
}

BackupProfileWidget::~BackupProfileWidget()
{
}

void BackupProfileWidget::slotTextChanged( const QString & )
{
    emit sigSomethingChanged();
}

void BackupProfileWidget::slotToggled( bool )
{
    emit sigSomethingChanged();
}

void BackupProfileWidget::slotIncrementalToggled( bool set )
{
    _snapshotLabel->setEnabled( set );
    _snapshotFile->setEnabled( set );
    _removeSnapshot->setEnabled( set );

    emit sigSomethingChanged();
}

void BackupProfileWidget::slotWorkingDirActivated( const QString & text )
{
    while ( FALSE == _relativeFiles.isEmpty() ) {
      QString my_first = _relativeFiles.first();
      _relativeFiles.remove( my_first );
    }
    _files->clear();

    QStringList::Iterator i = _absoluteFiles.begin();
    int remove = text.length();
    if ( remove > 1 ) {
        remove++;
    }
    for ( ; i != _absoluteFiles.end(); ++i ) {
        QString fn = *i;
        fn.remove( 0, remove );
        if ( fn.isEmpty() ) {
            fn = ".";
        }
        _files->insertItem( fn );
        _relativeFiles.append( fn );
    }

    emit sigSomethingChanged();
}

void BackupProfileWidget::setBackupProfile( BackupProfile* backupProfile )
{
    // Set the archive name.
    _archiveName->setText( backupProfile->getArchiveName() );

    setAbsoluteFiles( backupProfile->getAbsoluteFiles() );

    if ( !backupProfile->getWorkingDirectory().isNull() ) {
        for ( int ii = 0; ii < _workingDir->count(); ii++ ) {
            QString one = _workingDir->text( ii );
            QString two = backupProfile->getWorkingDirectory();
	      // if ( _workingDir->text( ii ) == backupProfile->getWorkingDirectory() ) {
            if( one == two ){
                _workingDir->setCurrentItem( ii );
                break;
            }
        }
    }

    //    slotWorkingDirActivated( _workingDir->currentText() );
    QString one = _workingDir->currentText();
    slotWorkingDirActivated( one );

    _oneFilesystem->setChecked( backupProfile->isOneFilesystem() );
    _incremental->setChecked( backupProfile->isIncremental() );
    _snapshotFile->setText( backupProfile->getSnapshotFile() );
    _removeSnapshot->setChecked( backupProfile->getRemoveSnapshot() );

    slotIncrementalToggled( backupProfile->isIncremental() );
}

void BackupProfileWidget::setAbsoluteFiles( const QStringList& files )
{
    // Copy the string list.
    _absoluteFiles = files;

    QString prefix = Util::longestCommonPath( files );

    _workingDir->clear();
    for ( int pos = prefix.length(); pos > 0; pos = prefix.findRev( '/', pos - 1 ) ) {
        _workingDir->insertItem( prefix.left( pos ) );
    }
    _workingDir->insertItem( "/" );
    _workingDir->setCurrentItem( 0 );

    slotWorkingDirActivated( _workingDir->currentText() );
}

QString BackupProfileWidget::getArchiveName()
{
    return _archiveName->text();
}

QString BackupProfileWidget::getWorkingDirectory()
{
    if ( _workingDir->count() > 0 ) {
        return _workingDir->currentText();
    } else {
        return 0;
    }
}

const QStringList& BackupProfileWidget::getRelativeFiles()
{
    return _relativeFiles;
}

const QStringList& BackupProfileWidget::getAbsoluteFiles()
{
    return _absoluteFiles;
}

bool BackupProfileWidget::isOneFilesystem()
{
    return _oneFilesystem->isChecked();
}

bool BackupProfileWidget::isIncremental()
{
    return _incremental->isChecked();
}

QString BackupProfileWidget::getSnapshotFile()
{
    return _snapshotFile->text();
}

bool BackupProfileWidget::getRemoveSnapshot()
{
    return _removeSnapshot->isChecked();
}
