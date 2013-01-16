// $Id: BackupOptDlg.cpp 465369 2005-09-29 14:33:08Z mueller $
//
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

#include <qlayout.h>
#include <kpushbutton.h>
#include <kstdguiitem.h>

#include <kapplication.h>

#include "BackupOptDlg.h"
#include "BackupProfileWidget.h"
#include <klocale.h>

#include "BackupOptDlg.moc"

BackupOptDlg::BackupOptDlg( BackupProfile* backupProfile, QWidget* parent, const char* name )
        : QDialog( parent, name, TRUE )
{
    setIconText( i18n( "KDat: Backup Options" ) );
    setCaption( i18n( "KDat: Backup Options" ) );

    resize( 400, 300 );

    _profile = new BackupProfileWidget( this );
    _profile->setBackupProfile( backupProfile );

    KPushButton* ok = new KPushButton( KStdGuiItem::ok(), this );
    ok->setFixedSize( 80, ok->sizeHint().height() );
    connect( ok, SIGNAL( clicked() ), this, SLOT( accept() ) );
    KPushButton* cancel = new KPushButton( KStdGuiItem::cancel(), this );
    cancel->setFixedSize( 80, ok->sizeHint().height() );
    connect( cancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

    QVBoxLayout* l1 = new QVBoxLayout( this, 8, 4 );
    l1->addWidget( _profile, 1 );

    QHBoxLayout* l1_1 = new QHBoxLayout();
    l1->addLayout( l1_1 );
    l1_1->addStretch( 1 );
    l1_1->addWidget( ok );
    l1_1->addWidget( cancel );

    ok->setDefault( TRUE );
}

BackupOptDlg::~BackupOptDlg()
{
}

QString BackupOptDlg::getArchiveName()
{
    return _profile->getArchiveName();
}

QString BackupOptDlg::getWorkingDirectory()
{
    return _profile->getWorkingDirectory();
}

const QStringList& BackupOptDlg::getRelativeFiles()
{
    return _profile->getRelativeFiles();
}

bool BackupOptDlg::isOneFilesystem()
{
    return _profile->isOneFilesystem();
}

bool BackupOptDlg::isIncremental()
{
    return _profile->isIncremental();
}

QString BackupOptDlg::getSnapshotFile()
{
    return _profile->getSnapshotFile();
}

bool BackupOptDlg::getRemoveSnapshot()
{
    return _profile->getRemoveSnapshot();
}
