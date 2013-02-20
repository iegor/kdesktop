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

#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

#include <qdatetime.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qkeycode.h>
#include <qlayout.h>
#include <qtextstream.h>

#include <kapplication.h>
#include <kiconloader.h>
#include <kmenubar.h>
#include <kpopupmenu.h>
#include <ktoolbar.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <kstringhandler.h>
#include <kpushbutton.h>
#include <kstdguiitem.h>

#include "ArchiveInfoWidget.h"
#include "BackupDlg.h"
#include "BackupOptDlg.h"
#include "BackupProfile.h"
#include "BackupProfileInfoWidget.h"
#include "BackupProfileManager.h"
#include "FileInfoWidget.h"
#include "FormatOptDlg.h"
#include "ImageCache.h"
#include "IndexDlg.h"
#include "InfoShellWidget.h"
#include "KDatMainWindow.h"
#include "Node.h"
#include "Options.h"
#include "OptionsDlg.h"
#include "Range.h"
#include "Tape.h"
#include "TapeDrive.h"
#include "TapeFileInfoWidget.h"
#include "TapeInfoWidget.h"
#include "TapeManager.h"
#include "Util.h"
#include "VerifyDlg.h"
#include "VerifyOptDlg.h"
#include "kdat.h"
#include "ktreeview.h"
#include <klocale.h>
#include <qsplitter.h>
#include <kstatusbar.h>

#include "KDatMainWindow.moc"

KDatMainWindow* KDatMainWindow::_instance = 0;

KDatMainWindow* KDatMainWindow::getInstance()
{
    if ( _instance == 0 ) {
        _instance = new KDatMainWindow();
    }
    return _instance;
}

#define KDAT_HORIZONTAL_LAYOUT
KDatMainWindow::KDatMainWindow()
        : KMainWindow(0), _destroyed( FALSE )
{
#ifdef KDAT_HORIZONTAL_LAYOUT      /* 2002-01-20 LEW */
    resize( 600, 600 );            /* was 600 by 400 */
#else
    resize( 600, 600 );
#endif /* KDAT_HORIZONTAL_LAYOUT */

    setIconText( i18n( "KDat: <no tape>" ) );
    setCaption( i18n( "KDat: <no tape>" ) );

    // Create object popup menus.
    _tapeDriveMenu = new QPopupMenu();
    _tapeDriveMenu->insertItem( i18n( "Mount Tape" )         , this, SLOT( fileMountTape() ) );
    _tapeDriveMenu->insertItem( i18n( "Recreate Tape Index" ), this, SLOT( fileIndexTape() ) );
    _tapeDriveMenu->insertSeparator();
    _tapeDriveMenu->insertItem( i18n( "Format Tape..." ), this, SLOT( fileFormatTape() ) );

    _archiveMenu = new QPopupMenu();
    _archiveMenu->insertItem( i18n( "Delete Archive" ), this, SLOT( fileDeleteArchive() ) );

    _mountedArchiveMenu = new QPopupMenu();
    _mountedArchiveMenu->insertItem( i18n( "Verify..." )     , this, SLOT( fileVerify() ) );
    _mountedArchiveMenu->insertItem( i18n( "Restore..." )    , this, SLOT( fileRestore() ) );
    _mountedArchiveMenu->insertSeparator();
    _mountedArchiveMenu->insertItem( i18n( "Delete Archive" ), this, SLOT( fileDeleteArchive() ) );

    _mountedTapeFileMenu = new QPopupMenu();
    _mountedTapeFileMenu->insertItem( i18n( "Verify..." ) , this, SLOT( fileVerify() ) );
    _mountedTapeFileMenu->insertItem( i18n( "Restore..." ), this, SLOT( fileRestore() ) );

    _localFileMenu = new QPopupMenu();
    _localFileMenu->insertItem( i18n( "Backup..." ), this, SLOT( fileBackup() ) );

    _tapeMenu = new QPopupMenu();
    _tapeMenu->insertItem( i18n( "Delete Tape Index" ), this, SLOT( fileDeleteIndex() ) );

    _backupProfileRootMenu = new QPopupMenu();
    _backupProfileRootMenu->insertItem( i18n( "Create Backup Profile" ), this, SLOT( fileNewBackupProfile() ) );

    _backupProfileMenu = new QPopupMenu();
    _backupProfileMenu->insertItem( i18n( "Backup..." ), this, SLOT( fileBackup() ) );
    _backupProfileMenu->insertSeparator();
    _backupProfileMenu->insertItem( i18n( "Delete Backup Profile" ), this, SLOT( fileDeleteBackupProfile() ) );

    _fileMenu = new QPopupMenu;
    _fileMenu->insertItem( i18n( "Backup..." )            , this, SLOT( fileBackup() ) );
    _fileMenu->insertItem( i18n( "Restore..." )           , this, SLOT( fileRestore() ) );
    _fileMenu->insertItem( i18n( "Verify..." )            , this, SLOT( fileVerify() ) );
    _fileMenu->insertItem( i18n( "Mount Tape" )           , this, SLOT( fileMountTape() ) );
    _fileMenu->insertItem( i18n( "Recreate Tape Index" )  , this, SLOT( fileIndexTape() ) );
    _fileMenu->insertItem( i18n( "Create Backup Profile" ), this, SLOT( fileNewBackupProfile() ) );
    _fileMenu->insertSeparator();
    _fileMenu->insertItem( i18n( "Delete Archive" )       , this, SLOT( fileDeleteArchive() ) );
    _fileMenu->insertItem( i18n( "Delete Index" )         , this, SLOT( fileDeleteIndex() ) );
    _fileMenu->insertItem( i18n( "Delete Backup Profile" ), this, SLOT( fileDeleteBackupProfile() ) );
    _fileMenu->insertItem( i18n( "Format Tape..." )       , this, SLOT( fileFormatTape() ) );
    _fileMenu->insertSeparator();
    _fileMenu->insertItem( SmallIcon("exit"), i18n( "&Quit" )                , this, SLOT( fileQuit() ), CTRL + Key_Q );

    _editMenu = new QPopupMenu;
    _editMenu->insertItem( SmallIcon("configure"), i18n( "Configure KDat..." )       , this, SLOT( editPreferences() ) );

    _menu = new KMenuBar( this );
    _menu->insertItem( i18n( "&File" ), _fileMenu );
    _menu->insertItem( i18n( "&Settings" ), _editMenu );
    _menu->insertSeparator();
    QString about = i18n( "KDat Version %1\n\nKDat is a tar-based tape archiver.\n\nCopyright (c) 1998-2000 Sean Vyain\nCopyright (c) 2001-2002 Lawrence Widman\nkdat@cardiothink.com" ).arg( KDAT_VERSION );
    _menu->insertItem( i18n( "&Help" ), helpMenu( about ) );

    _toolbar = new KToolBar( this );

    _toolbar->insertButton( *ImageCache::instance()->getTapeUnmounted(), 0, SIGNAL( clicked( int ) ), this, SLOT( fileMountTape() ), TRUE, i18n( "Mount/unmount tape" ) );

    _toolbar->insertSeparator();

    _toolbar->insertButton( *ImageCache::instance()->getBackup() , 1, SIGNAL( clicked( int ) ), this, SLOT( fileBackup() ) , TRUE, i18n( "Backup" ) );
    _toolbar->setButtonIconSet( 1, BarIconSet("kdat_backup"));
    _toolbar->insertButton( *ImageCache::instance()->getRestore(), 2, SIGNAL( clicked( int ) ), this, SLOT( fileRestore() ), TRUE, i18n( "Restore" ) );
    _toolbar->setButtonIconSet( 2, BarIconSet("kdat_restore"));
    _toolbar->insertButton( *ImageCache::instance()->getVerify() , 3, SIGNAL( clicked( int ) ), this, SLOT( fileVerify() ) , TRUE, i18n( "Verify" ) );
    _toolbar->setButtonIconSet( 3, BarIconSet("kdat_verify"));
    addToolBar( _toolbar );

    _statusBar = new KStatusBar( this );
    _statusBar->insertItem( i18n( "Ready." ), 0 );

#ifdef KDAT_HORIZONTAL_LAYOUT      /* 2002-01-20 LEW */
    _panner = new QSplitter( QSplitter::Horizontal, this, "panner");
#else
    _panner = new QSplitter( QSplitter::Vertical, this, "panner");
#endif /* KDAT_HORIZONTAL_LAYOUT */

    // Create info viewers.
    InfoShellWidget* infoShell = new InfoShellWidget( _panner );
    _tapeInfo = new TapeInfoWidget( infoShell );
    _archiveInfo = new ArchiveInfoWidget( infoShell );
    _backupProfileInfo = new BackupProfileInfoWidget( infoShell );
    _tapeFileInfo = new TapeFileInfoWidget( infoShell );
    _fileInfo = new FileInfoWidget( infoShell );

    // Now set up the tree
    _tree = new KTreeView( _panner );
    _tree->setExpandButtonDrawing( TRUE );

#ifdef KDAT_HORIZONTAL_LAYOUT      /* 2002-01-20 LEW */
    _tree->setMinimumWidth( 300 );
    _panner->moveToFirst( _tree );
#else
    _tree->setMinimumHeight( 300 );
#endif /* KDAT_HORIZONTAL_LAYOUT */

    connect( _tree, SIGNAL( expanding( KTreeViewItem*, bool& ) ), this, SLOT( localExpanding( KTreeViewItem*, bool& ) ) );
    connect( _tree, SIGNAL( expanded( int ) ), this, SLOT( localExpanded( int ) ) );
    connect( _tree, SIGNAL( collapsed( int ) ), this, SLOT( localCollapsed( int ) ) );
    connect( _tree, SIGNAL( selected( int ) ), this, SLOT( localSelected( int ) ) );
    connect( _tree, SIGNAL( highlighted( int ) ), this, SLOT( localHighlighted( int ) ) );
    connect( _tree, SIGNAL( popupMenu( int, const QPoint& ) ), this, SLOT( localPopupMenu( int, const QPoint& ) ) );

    setCentralWidget( _panner );

    _tree->insertItem( _tapeDriveNode = new TapeDriveNode() );
    _tree->insertItem( _rootNode = new RootNode() );
    _tree->insertItem( _backupProfileRootNode = new BackupProfileRootNode() );
    _tree->insertItem( new TapeIndexRootNode() );

    connect( TapeDrive::instance(), SIGNAL( sigStatus( const QString & ) ), this, SLOT( status( const QString & ) ) );

    setTapePresent( FALSE );

    connect( Options::instance(), SIGNAL( sigTapeDevice() ), this, SLOT( slotTapeDevice() ) );

    connect( TapeManager::instance(), SIGNAL( sigTapeMounted() )  , this, SLOT( slotTapeMounted() ) );
    connect( TapeManager::instance(), SIGNAL( sigTapeUnmounted() ), this, SLOT( slotTapeUnmounted() ) );

    configureUI( 0 );
}

KDatMainWindow::~KDatMainWindow()
{
    _destroyed = TRUE;

    if ( Options::instance()->getLockOnMount() ) {
        TapeDrive::instance()->unlock();
    }
    delete _tapeDriveMenu;
    delete _archiveMenu;
    delete _mountedArchiveMenu;
    delete _mountedTapeFileMenu;
    delete _localFileMenu;
    delete _tapeMenu;
    delete _backupProfileRootMenu;
    delete _backupProfileMenu;

}

void KDatMainWindow::popupTapeDriveMenu( const QPoint& p )
{
    // Configure menu before popping up.
    if ( TapeManager::instance()->getMountedTape() ) {
        _tapeDriveMenu->changeItem( i18n( "Unmount Tape" ), _tapeDriveMenu->idAt( 0 ) );
        _tapeDriveMenu->setItemEnabled( _tapeDriveMenu->idAt( 1 ), TRUE );
    } else {
        _tapeDriveMenu->changeItem( i18n( "Mount Tape" ), _tapeDriveMenu->idAt( 0 ) );
        _tapeDriveMenu->setItemEnabled( _tapeDriveMenu->idAt( 1 ), FALSE );
    }

    _tapeDriveMenu->popup( p );
}

void KDatMainWindow::popupArchiveMenu( const QPoint& p )
{
    _archiveMenu->popup( p );
}

void KDatMainWindow::popupMountedArchiveMenu( const QPoint& p )
{
    _mountedArchiveMenu->popup( p );
}

void KDatMainWindow::popupMountedTapeFileMenu( const QPoint& p )
{
    _mountedTapeFileMenu->popup( p );
}

void KDatMainWindow::popupLocalFileMenu( const QPoint& p )
{
    // Configure menu before popping up.
    _localFileMenu->setItemEnabled( _localFileMenu->idAt( 0 ), ( TapeManager::instance()->getMountedTape() != 0 ) && ( !TapeDrive::instance()->isReadOnly() ) );

    _localFileMenu->popup( p );
}

void KDatMainWindow::popupTapeMenu( const QPoint& p )
{
    _tapeMenu->popup( p );
}

void KDatMainWindow::popupBackupProfileRootMenu( const QPoint& p )
{
    _backupProfileRootMenu->popup( p );
}

void KDatMainWindow::popupBackupProfileMenu( const QPoint& p )
{
    // Configure menu before popping up.
    _backupProfileMenu->setItemEnabled( _backupProfileMenu->idAt( 0 ), ( TapeManager::instance()->getMountedTape() != 0 ) && ( !TapeDrive::instance()->isReadOnly() ) );

    _backupProfileMenu->popup( p );
}

void KDatMainWindow::hideInfo()
{
    _archiveInfo->hide();
    _backupProfileInfo->hide();
    _tapeInfo->hide();
    _tapeFileInfo->hide();
    _fileInfo->hide();
}

void KDatMainWindow::showTapeInfo( Tape* tape )
{
    assert( tape );

    hideInfo();

    _tapeInfo->setTape( tape );
    _tapeInfo->show();
}

void KDatMainWindow::showArchiveInfo( Archive* archive )
{
    assert( archive );

    hideInfo();

    _archiveInfo->setArchive( archive );
    _archiveInfo->show();
}

void KDatMainWindow::showBackupProfileInfo( BackupProfile* backupProfile )
{
    assert( backupProfile );

    hideInfo();

    _backupProfileInfo->setBackupProfile( backupProfile );
    _backupProfileInfo->show();
}

void KDatMainWindow::showTapeFileInfo( File* file )
{
    assert( file );

    hideInfo();

    _tapeFileInfo->setFile( file );
    _tapeFileInfo->show();
}

void KDatMainWindow::showFileInfo( const QString & name )
{
    assert( !name.isNull() );

    hideInfo();

    _fileInfo->setFile( name );
    _fileInfo->show();
}

void KDatMainWindow::localExpanding( KTreeViewItem* item, bool& allow )
{
    ((Node*)item)->expanding( allow );
}

void KDatMainWindow::localExpanded( int index )
{
    ((Node*)_tree->itemAt( index ))->expanded();
}

void KDatMainWindow::localCollapsed( int index )
{
    ((Node*)_tree->itemAt( index ))->collapsed();
}

void KDatMainWindow::localSelected( int index )
{
    Node* item = (Node*)_tree->itemAt( index );

    /* 2002-01-30 LEW: RG and I don't like the behavior in which
       the subtree expands or contracts when a directory is selected
       or unselected.
       so make the tree look the same when we are done....
       The goal is to redraw the icon to the left of the item.
       There's gotta be a better way to do this. */
    if ( item->isExpanded() ) {
      _tree->collapseItem( index );
      _tree->expandItem( index );   // 2002-01-30 LEW
    } else {
      _tree->expandItem( index );
      _tree->collapseItem( index ); // 2002-01-30 LEW
    }
    // repaint();  // this doesn't work   2002-01-30 LEW
    /* 2002-01-30 LEW: RG and I don't like this behavior */
}

void KDatMainWindow::localHighlighted( int index )
{
    Node* item = (Node*)_tree->itemAt( index );

    if ( item ) {
        item->selected();
    }

    configureUI( TapeManager::instance()->getMountedTape() );
}

void KDatMainWindow::localPopupMenu( int index, const QPoint& p )
{
    Node* item = (Node*)_tree->itemAt( index );
    item->popupMenu( p );
}

void KDatMainWindow::fileBackup()
{
    BackupProfile backupProfile;

    /* 2002-01-28 LEW */
    // To the i18n translator: I apologize for this long text on the last day to submit
    // i18n changes.  But, the program isn't very usable without this information.  Thanks. LEW
    // (later today):  now the program works correctly!  Why?  Beats me!
    QString msg = i18n("KDat will dump your files properly to tape, but may not be able\n"
                       "to restore them. To restore your files by hand, you need to know\n"
                       "the name of the *non-rewinding* version of your tape device %1.\n"
			).arg(Options::instance()->getTapeDevice());
    QString msg2 = i18n("For example, if your device is /dev/st0, the non-rewinding version\n"
                       "is /dev/nst0. If your device name doesn't look like that, type\n"
                       "\"ls -l %2\" in a terminal window to see the real name of your\n"
                       "tape drive. Substitute that name for /dev/nst0 below.\n"
                       "Open a terminal window and type the following:\n"
                       "  tar tfv /dev/nst0; tar tfv /dev/nst0\n"
                       "  tar xfv /dev/nst0\n"
                       "The third call to \"tar\" will retrieve your data into your\n"
                       "current directory. Please let us know if this happens to you!\n"
                       " - KDat Maintenance Team\n"
			).arg(Options::instance()->getTapeDevice());
    msg = msg.append(msg2);
    KMessageBox::sorry( this, msg);
    /* 2002-01-28 LEW */

    if ( ( _tree->getCurrentItem() ) && ( ((Node*)_tree->getCurrentItem())->isType( Node::BackupProfileNodeType ) ) ) {
        BackupProfile* bp = ((BackupProfileNode*)_tree->getCurrentItem())->getBackupProfile();
        backupProfile.setArchiveName( bp->getArchiveName() );
        backupProfile.setWorkingDirectory( bp->getWorkingDirectory() );
        backupProfile.setAbsoluteFiles( bp->getAbsoluteFiles() );
        backupProfile.setOneFilesystem( bp->isOneFilesystem() );
        backupProfile.setIncremental( bp->isIncremental() );
        backupProfile.setSnapshotFile( bp->getSnapshotFile() );
        backupProfile.setRemoveSnapshot( bp->getRemoveSnapshot() );
    } else {
        QString name;
        name = i18n( "Archive created on %1" ).arg( KGlobal::locale()->formatDate(QDate::currentDate(), true) );
        name = name.stripWhiteSpace();

        QStringList files;
        getBackupFiles( files );

        backupProfile.setArchiveName( name );
        backupProfile.setAbsoluteFiles( files );
        backupProfile.setOneFilesystem( TRUE );
        backupProfile.setIncremental( FALSE );
        backupProfile.setSnapshotFile( "snapshot" );
        backupProfile.setRemoveSnapshot( FALSE );
    }

    int ret = 0;
    BackupOptDlg dlg( &backupProfile, this );
    if ( dlg.exec() == QDialog::Accepted ) {
        // Begin backup.
        status( i18n( "Performing backup..." ) );
        int size = calcBackupSize( dlg.getWorkingDirectory(), dlg.isOneFilesystem(), dlg.getRelativeFiles(), dlg.isIncremental(), dlg.getSnapshotFile(), dlg.getRemoveSnapshot() );

        // Check to see whether user aborted the backup
        if ( size < 0) {
	  status( i18n( "Backup canceled." ) );
	  return;
	}

        // Make sure the archive will fit on the tape.
        int tapeSize = 1;
        QPtrListIterator<Archive> i( TapeManager::instance()->getMountedTape()->getChildren() );
        if ( i.toLast() != NULL ) {
            tapeSize = i.current()->getEndBlock();
            tapeSize = (int)((float)tapeSize / 1024.0 * (float)Options::instance()->getTapeBlockSize()) + 1;
        }
        if ( tapeSize + size >= TapeManager::instance()->getMountedTape()->getSize() ) {
            // Warn user that tape is probably too short.
            QString msg;
            msg = i18n( "WARNING: The estimated archive size is %1 KB but "
                        "the tape has only %2 KB of space!\n"
                        "Back up anyway?" )
              .arg(KGlobal::locale()->formatNumber(size, 0))
              .arg(KGlobal::locale()->formatNumber(TapeManager::instance()->getMountedTape()->getSize() - tapeSize, 0 ));
            int result = KMessageBox::warningContinueCancel( this,
                               msg, i18n("Backup"), i18n("Backup") );
            if ( result != KMessageBox::Continue) {
                status( i18n( "Backup canceled." ) );
                return;
            }
        }

        if ( TapeDrive::instance()->getFile() < 0 ) {
            // Rewind tape.
            status( i18n( "Rewinding tape..." ) );
            if ( !TapeDrive::instance()->rewind() ) {
                KMessageBox::error( this,
                      i18n("Cannot rewind tape.\nBackup aborted."),
                      i18n("Backup Error"));
                status( i18n( "Backup aborted." ) );
                return;
            }
        }

        // Go to end of tape.
        status( i18n( "Skipping to end of tape..." ) );
        if ( !TapeDrive::instance()->nextFile( TapeManager::instance()->getMountedTape()->getChildren().count() + 1 - TapeDrive::instance()->getFile() ) ) {
            KMessageBox::error( this,
                      i18n("Cannot get to end of tape.\nBackup aborted."),
                      i18n("Backup Error"));
            status( i18n( "Backup aborted." ) );
            return;
        }

        status( i18n( "Backup in progress..." ) );
        BackupDlg backupDlg( dlg.getArchiveName(), dlg.getWorkingDirectory(), dlg.getRelativeFiles(), dlg.isOneFilesystem(), dlg.isIncremental(), dlg.getSnapshotFile(), dlg.getRemoveSnapshot(), size, TapeManager::instance()->getMountedTape(), this );
        ret = backupDlg.exec();

//        status( i18n( "Rewinding tape..." ) );
//        TapeDrive::instance()->rewind();
//        if ( !TapeDrive::instance()->prevFile( 1 ) ) {
//            status( i18n( "Rewinding tape..." ) );
//            TapeDrive::instance()->rewind();
//        }
    }

    // All done.
    if ( ret == QDialog::Accepted ) {
        status( i18n( "Backup complete." ) );
    } else {
        status( i18n( "Backup aborted." ) );
    }
}

void KDatMainWindow::fileRestore()
{
    doVerify( TRUE );
}

void KDatMainWindow::fileVerify()
{
    doVerify( FALSE );
}

void KDatMainWindow::doVerify( bool restore )
{
    RangeableNode*      rangeableNode = 0;
    MountedArchiveNode* archiveNode = 0;
    QStringList files;
    Archive* archive = 0;
    // Check for marked files first.
    for ( int i = _tapeDriveNode->childCount() - 1; i >= 0; i-- ) {
        if ( ( ((SelectableNode*)_tapeDriveNode->childAt( i ))->isSelected() ) ||
             ( ((SelectableNode*)_tapeDriveNode->childAt( i ))->hasSelectedChildren() )) {
            archiveNode = (MountedArchiveNode*)_tapeDriveNode->childAt( i );
            archive = archiveNode->getArchive();
	    /* 2002-01-30 LEW */
#ifdef DEBUG
	    printf("KDatMainWindow::doVerify: %d node of %s: ", i,
		   (SelectableNode*)_tapeDriveNode->getText().latin1() );
            if( ((SelectableNode*)_tapeDriveNode->childAt( i ))->isSelected() ){
	      printf("is completely selected\n");
            } else {
	      printf("is partially selected\n");
            }
#endif /* DEBUG */
	    /* 2002-01-30 LEW */
            break;
        }
    }

    if ( !archiveNode ) {
        if ( ((Node*)_tree->getCurrentItem())->isType( Node::RangeableNodeType ) ) {
            rangeableNode = (RangeableNode*)_tree->getCurrentItem();
            Node* parent = rangeableNode;
            for ( ; !parent->isType( Node::MountedArchiveNodeType ); parent = (Node*)parent->getParent() );
            assert( parent );
            archive = ((MountedArchiveNode*)parent)->getArchive();
        }
    }

    assert( archive );
    QPtrListIterator<Archive> it( archive->getTape()->getChildren() );
    int fileno;
    for ( fileno = 1; ( it.current() ) && ( it.current() != archive ); ++it, fileno++ );

    assert( archiveNode || rangeableNode );
    assert( fileno > -1 );

    RangeList ranges;
    if ( rangeableNode ) {
        QPtrListIterator<Range> it( rangeableNode->getRanges() );
        for ( ; it.current(); ++it ) {
            ranges.addRange( it.current()->getStart(), it.current()->getEnd() );
        }
        if ( rangeableNode->isType( Node::MountedArchiveNodeType ) ) {
            // Make sure the mounted archive node has populated its children.
            archiveNode = (MountedArchiveNode*)rangeableNode;
            if ( archiveNode->childCount() == 0 ) {
                bool dummy = TRUE;
                archiveNode->expanding( dummy );
            }

            for ( int i = rangeableNode->childCount() - 1; i >= 0; i-- ) {
                if ( ((Node*)rangeableNode->childAt( i ))->isType( Node::MountedTapeDirectoryNodeType ) ) {
                    files.append( ((MountedTapeDirectoryNode*)rangeableNode->childAt( i ))->getFullPath() );
                } else if ( ((Node*)rangeableNode->childAt( i ))->isType( Node::MountedTapeFileNodeType ) ) {
                    files.append( ((MountedTapeFileNode*)rangeableNode->childAt( i ))->getFullPath() );
                } else {
                    assert( FALSE );
                }
            }
        } else if ( rangeableNode->isType( Node::MountedTapeDirectoryNodeType ) ) {
            files.append( ((MountedTapeDirectoryNode*)rangeableNode)->getFullPath() );
        } else if ( rangeableNode->isType( Node::MountedTapeFileNodeType ) ) {
            files.append( ((MountedTapeFileNode*)rangeableNode)->getFullPath() );
        } else {
            assert( FALSE );
        }
    } else {
        // Compile a list of files to verify/restore.
        QPtrStack<RangeableNode> stack;

        // Make sure the mounted archive node has populated its children.
        if ( archiveNode->childCount() == 0 ) {
            bool dummy = TRUE;
            archiveNode->expanding( dummy );
        }

        for ( int i = archiveNode->childCount() - 1; i >= 0; i-- ) {
            stack.push( (RangeableNode*)archiveNode->childAt( i ) );
        }
        RangeableNode* sel = 0;
        while ( stack.count() > 0 ) {
            sel = stack.pop();
            if ( sel->isSelected() ) {
                QPtrListIterator<Range> it( sel->getRanges() );
                for ( ; it.current(); ++it ) {
                    ranges.addRange( it.current()->getStart(), it.current()->getEnd() );
                }

                if ( sel->isType( Node::MountedTapeDirectoryNodeType ) ) {
                    files.append( ((MountedTapeDirectoryNode*)sel)->getFullPath() );
                } else if ( sel->isType( Node::MountedTapeFileNodeType ) ) {
                    files.append( ((MountedTapeFileNode*)sel)->getFullPath() );
                } else {
                    assert( FALSE );
                }
            } else if ( sel->hasSelectedChildren() ) {
                for ( int i = sel->childCount() - 1; i >= 0; i-- ) {
                    stack.push( (RangeableNode*)sel->childAt( i ) );
                }
            }
        }
    }

    char buf[1024];
    VerifyOptDlg dlg( getcwd( buf, 1024 ), files, restore, this );
    if ( dlg.exec() == QDialog::Accepted ) {
        if ( restore ) {
            status( i18n( "Restore in progress..." ) );
        } else {
            status( i18n( "Verify in progress..." ) );
        }
        VerifyDlg verifyDlg( dlg.getWorkingDirectory(), fileno, ranges, restore, this );
        int ret = verifyDlg.exec();

        if ( ret == QDialog::Accepted ) {
            if ( restore ) {
                status( i18n( "Restore complete." ) );
            } else {
                status( i18n( "Verify complete." ) );
            }
        } else {
            if ( restore ) {
                status( i18n( "Restore aborted." ) );
            } else {
                status( i18n( "Verify aborted." ) );
            }
        }
    }
}

void KDatMainWindow::fileMountTape()
{
    static QString msg;

    // construct helpful error message (same as in fileFormatTape())
    msg = i18n("There appears to be no tape in the drive %1. Please\n"
               "check \"Edit->Preferences\" to make sure the\n"
               "correct device is selected as the tape drive (e.g.\n"
               "/dev/st0). If you hear the tape drive moving, wait\n"
               "until it stops and then try mounting it again.")
                 .arg(Options::instance()->getTapeDevice());

    if ( !TapeManager::instance()->getMountedTape() ) {
        if ( Options::instance()->getLoadOnMount() ) {
            if ( !TapeDrive::instance()->load() ) {
                KMessageBox::sorry( this, msg);
                return;
            }
        }

        if ( TapeDrive::instance()->isTapePresent() ) {
            setTapePresent( TRUE );
        } else {
            KMessageBox::sorry( this, msg);
        }
    } else {
        setTapePresent( FALSE );
    }
}

void KDatMainWindow::fileIndexTape()
{
    int result = KMessageBox::warningContinueCancel( this,
		       i18n( "The current tape index will be overwritten, continue?" ),
                       i18n( "Index Tape"), i18n("Overwrite"));
    if ( result == KMessageBox::Continue) {
        TapeManager::instance()->getMountedTape()->clear();
        IndexDlg dlg( TapeManager::instance()->getMountedTape(), this );
        if ( dlg.exec() == QDialog::Accepted ) {
            QString title;
            title = i18n( "KDat: %1" ).arg( TapeManager::instance()->getMountedTape()->getName() );
            setCaption( title );
            setIconText( title );

            status( i18n( "Index complete." ) );
        } else {
            status( i18n( "Index aborted." ) );
        }
    }
}

void KDatMainWindow::fileDeleteArchive()
{
    Node* sel = (Node*)_tree->getCurrentItem();
    if ( ( !sel ) || ( !sel->isType( Node::ArchiveNodeType ) && !sel->isType( Node::MountedArchiveNodeType ) ) ) {
        KMessageBox::sorry( this, i18n( "No archive is selected.\n"
                                        "In order to delete an archive, the archive to be deleted "
                                        "must be selected in the tree first." ));
        return;
    }

    // Find the selected archive and see if it has any archives after it.
    Archive* archive = 0;
    if ( sel->isType( Node::ArchiveNodeType ) ) {
        archive = ((ArchiveNode*)sel)->getArchive();
    } else if ( sel->isType( Node::MountedArchiveNodeType ) ) {
        archive = ((MountedArchiveNode*)sel)->getArchive();
    }
    assert( archive );

    Tape*    tape    = archive->getTape();
    QPtrListIterator<Archive> i( tape->getChildren() );
    for ( ; i.current(); ++i ) {
        if ( i.current() == archive ) {
            break;
        }
    }
    assert( i.current() );

    ++i;
    if ( i.current() ) {
        // There are other archives after this one on the tape.
        QString list;
        for ( ; i.current(); ++i ) {
            list.append( "\n        " );
            list.append( i.current()->getName() );
        }

        QString msg =
             i18n( "An archive cannot be removed from the middle of the tape. If\nthe archive '%1' is deleted then\nthe following archives will also be deleted:\n%2\n\nDelete all listed archives?" ).arg(archive->getName()).arg(list);
        int result = KMessageBox::warningContinueCancel( this,
             msg, i18n("Delete Archive"), i18n("Delete All"));
        if (result == KMessageBox::Continue) {
            tape->removeChild( archive );
            emit status( i18n( "Archives deleted." ) );
            if ( _tree->getCurrentItem() ) {
                ((Node*)_tree->getCurrentItem())->selected();
            }
            configureUI( TapeManager::instance()->getMountedTape() );
        }
    } else {
        // This is the last (known) archive on the tape.
        QString msg =
             i18n( "Really delete the archive '%1'?" ).arg(archive->getName());
        int result = KMessageBox::warningContinueCancel( this,
                              msg, i18n("Delete Archive"), i18n("Delete"));
        if (result == KMessageBox::Continue) {
            tape->removeChild( archive );
            emit status( i18n( "Archive deleted." ) );
            if ( _tree->getCurrentItem() ) {
                ((Node*)_tree->getCurrentItem())->selected();
            }
            configureUI( TapeManager::instance()->getMountedTape() );
        }
    }
}

void KDatMainWindow::fileDeleteIndex()
{
    Node* sel = (Node*)_tree->getCurrentItem();
    if ( ( !sel ) || ( !sel->isType( Node::TapeNodeType ) ) ) {
        KMessageBox::sorry( this, i18n( "No tape index is selected.\n"
                  "In order to delete a tape index, the tape index to be deleted "
                  "must be selected in the tree first." ));
        return;
    }

    Tape* tape = ((TapeNode*)sel)->getTape();
    assert( tape );
    if ( tape == TapeManager::instance()->getMountedTape() ) {
        KMessageBox::sorry( this, i18n( "Tape is still mounted. "
                  "The index for a mounted tape cannot be deleted.\n"
                  "Unmount the tape and try again." ));
        return;
    }

    QString msg =
         i18n( "Really delete the index for '%1'?" ).arg(tape->getName());
    int result = KMessageBox::warningContinueCancel( this,
                    msg, i18n("Delete Tape Index"), i18n("Delete"));
    if (result == KMessageBox::Continue) {
        TapeManager::instance()->removeTape( tape );
        emit status( i18n( "Tape index deleted." ) );
        if ( _tree->getCurrentItem() ) {
            ((Node*)_tree->getCurrentItem())->selected();
        }
        configureUI( TapeManager::instance()->getMountedTape() );
    }
}

void KDatMainWindow::fileFormatTape()
{
    static QString msg;

    // construct helpful error message (same as in fileMountTape())
    msg = i18n("There appears to be no tape in the drive %1. Please\n"
               "check \"Edit->Preferences\" to make sure the\n"
               "correct device is selected as the tape drive (e.g.\n"
               "/dev/st0). If you hear the tape drive moving, wait\n"
               "until it stops and then try mounting it again.")
                 .arg(Options::instance()->getTapeDevice());

    if ( !TapeDrive::instance()->isTapePresent() ) {
        KMessageBox::sorry( this, msg );
        return;
    }

    if ( TapeDrive::instance()->isReadOnly() ) {
        KMessageBox::sorry( this, i18n( "The tape in the drive is write protected.\nPlease disable write protection and try again." ));
        return;
    }

    int result = KMessageBox::warningContinueCancel( this,
                     i18n( "All data currently on the tape will be lost.\n"
                           "Are you sure you want to continue?" ),
                     i18n("Format Tape"), i18n("Format"));
    if (result == KMessageBox::Continue ) {
        QString name;
        name = i18n( "Tape created on %1" ).arg(  KGlobal::locale()->formatDate(QDate::currentDate(), true) );
        FormatOptDlg dlg( name.stripWhiteSpace(), this );
        if ( dlg.exec() != QDialog::Accepted ) {
            return;
        }

        // Delete old index file.
        if ( TapeManager::instance()->getMountedTape() ) {
            TapeManager::instance()->removeTape( TapeManager::instance()->getMountedTape() );
            TapeManager::instance()->unmountTape();
        }

        Tape* tape = new Tape();
        tape->setName( dlg.getName() );
        tape->setSize( dlg.getSize() );

        status( i18n( "Formatting tape..." ) );
        tape->format();
        TapeManager::instance()->addTape( tape );
        status( i18n( "Format complete." ) );

        setTapePresent( FALSE, FALSE );
        setTapePresent( TRUE, FALSE );
    }
}

void KDatMainWindow::fileNewBackupProfile()
{
    BackupProfile* backupProfile = new BackupProfile();

    // Pick a unique name.
    QString name;
    for ( int i = 1; ; i++ ) {
        name = i18n( "Backup Profile %1").arg( i );
        QStringList list = BackupProfileManager::instance()->getBackupProfileNames();
        QStringList::Iterator it = list.begin();
        for ( ; it != list.end(); ++it ) {
            if ( name == *it ) {
                break;
            }
        }
        if ( it == list.end() ) {
            // Name is unique.
            break;
        }
    }

    QStringList files;
    getBackupFiles( files );

    backupProfile->setName( name );
    backupProfile->setArchiveName( i18n( "Archive" ) );
    backupProfile->setWorkingDirectory( Util::longestCommonPath( files ) );
    if ( !backupProfile->getWorkingDirectory() ) {
        backupProfile->setWorkingDirectory( "/" );
    }
    backupProfile->setAbsoluteFiles( files );
    backupProfile->setOneFilesystem( TRUE );
    backupProfile->setIncremental( FALSE );
    backupProfile->setSnapshotFile( "snapshot" );
    backupProfile->setRemoveSnapshot( FALSE );
    backupProfile->save();

    BackupProfileManager::instance()->addBackupProfile( backupProfile );

    _backupProfileRootNode->setSelected( backupProfile );
}

void KDatMainWindow::fileDeleteBackupProfile()
{
    Node* sel = (Node*)_tree->getCurrentItem();
    if ( ( !sel ) || ( !sel->isType( Node::BackupProfileNodeType ) ) ) {
        KMessageBox::sorry( this, i18n( "In order to delete a backup profile, the backup profile to be deleted "
                                        "must be selected in the tree first." ));
        return;
    }

    BackupProfile* backupProfile = ((BackupProfileNode*)sel)->getBackupProfile();
    assert( backupProfile );

    QString msg =
         i18n("Really delete backup profile '%1'?").arg(backupProfile->getName());
    int result = KMessageBox::warningContinueCancel( this,
                        msg, i18n("Delete Backup Profile"), i18n("Delete"));
    if (result == KMessageBox::Continue) {
        BackupProfileManager::instance()->removeBackupProfile( backupProfile );
        emit status( i18n( "Backup profile deleted." ) );
        if ( _tree->getCurrentItem() ) {
            ((Node*)_tree->getCurrentItem())->selected();
        }
        configureUI( TapeManager::instance()->getMountedTape() );
    }
}

void KDatMainWindow::fileQuit()
{
    KApplication::kApplication()->quit();
}

void KDatMainWindow::editPreferences()
{
    OptionsDlg dlg( this );
    dlg.exec();
}

void KDatMainWindow::help()
{
    KApplication::kApplication()->invokeHelp( );
}

void KDatMainWindow::setTapePresent( bool tapePresent, bool eject )
{
    if ( TapeManager::instance()->getMountedTape() ) {
        if ( !tapePresent ) {
            TapeManager::instance()->unmountTape();

            if ( ( eject ) && ( Options::instance()->getLockOnMount() ) ) {
                TapeDrive::instance()->unlock();
            }

            if ( ( eject ) && ( Options::instance()->getEjectOnUnmount() ) ) {
                TapeDrive::instance()->eject();
            }

            status( i18n( "Tape unmounted." ) );
        }
    } else {
        if ( tapePresent ) {
            status( i18n( "Reading tape header..." ) );
            Tape* tape = TapeDrive::instance()->readHeader();

            if ( tape ) {
                TapeManager::instance()->mountTape( tape );
            } else {
                if ( TapeDrive::instance()->isReadOnly() ) {
                    KMessageBox::sorry( this, i18n( "This tape has not been formatted by KDat." ));
                } else {
                    int result = KMessageBox::questionYesNo( this,
                          i18n( "This tape has not been formatted by KDat.\n\nWould you like to format it now?" ), QString::null, i18n("Format"), i18n("Do Not Format"));
                    if (result == KMessageBox::Yes) {
                        fileFormatTape();
                        return;
                    }
                }
            }

            if ( Options::instance()->getLockOnMount() ) {
                TapeDrive::instance()->lock();
            }

            status( i18n( "Tape mounted." ) );
        }
    }
}

void KDatMainWindow::status( const QString & msg )
{
    _statusBar->changeItem( msg, 0 );
    KApplication::kApplication()->processEvents();
}

void KDatMainWindow::show()
{
    KMainWindow::show();

    hideInfo();
}

// 2002-01-21 LEW: returns backup size in KB, or -1 if user chose
// to abort the backup.
int KDatMainWindow::calcBackupSize( const QString& workingDir, bool local, const QStringList& files,
                          bool incremental, const QString& snapshot, bool removeSnapshot )
{
    int return_value;

    chdir( QFile::encodeName(workingDir) );

    bool useSnapshot = !snapshot.isEmpty() && !removeSnapshot;

    int tmp = 0;
    if ( incremental && !removeSnapshot ) {
        QFile snap( snapshot );
        if ( snap.exists() && snap.open( IO_ReadOnly ) ) {
            QTextStream t( &snap );
            t >> tmp;
        } else {
            useSnapshot = FALSE;
        }
    }
    QDateTime mtime;
    mtime.setTime_t( tmp );

    int size = 0;

    /* 2002-01-24 LEW:  start of backup-cancel dialog */
    create_backup_dialog();
    stop_flag = FALSE;    // initialize the flag that tells us whether use
                          // cancelled the backup via the dialog.
    /* 2002-01-24 LEW:  end of backup-cancel dialog */

    QStringList filesTmp = files;
    QStringList::Iterator ii = filesTmp.begin();
    for ( ; ii != filesTmp.end(); ++ii ) {

        // Is this a normal file, or a directory?
        QFileInfo finfo( *ii );
        if ( !finfo.isDir() ) {
            if ( ( !useSnapshot ) || ( finfo.lastModified() > mtime ) ) {
                size += finfo.size() / 512;
                if ( finfo.size() % 512 ) {
                    size++;
                }
            }
            continue;
        }

        QPtrStack<QString> dirStack;
        dirStack.push( new QString( *ii ) );

        // If stay on one file system, get device of starting directory.
        dev_t device = 0;
        struct stat info;
        if ( local ) {
            if ( lstat( QFile::encodeName(*ii), &info ) != 0 ) {
                device = 0;
            } else {
                device = info.st_dev;
            }
        }

        QString msg;
        // msg.truncate( 4095 );
        QDir dir;
        const QFileInfoList* infoList;
        while ( !dirStack.isEmpty() ) {
	    if( stop_flag == TRUE ) break;
            QString* path = dirStack.pop();
            msg = i18n("Estimating backup size: %1, %2" )
             .arg(Util::kbytesToString( size / 2 ))
             .arg(KStringHandler::csqueeze(*path, 60));
            status( msg );
            KApplication::kApplication()->processEvents();
            dir.setPath( *path );
            infoList = dir.entryInfoList( QDir::Hidden | QDir::Files | QDir::Dirs, 0 );
            if ( infoList ) {
                QFileInfoListIterator i( *infoList );
                for ( ; i.current(); ++i ) {
                    if ( ( i.current()->fileName() != "." ) && ( i.current()->fileName() != ".." ) ) {
                        size++;
                        if ( i.current()->isSymLink() ) {
                        } else if ( i.current()->isDir() ) {
                            if ( local ) {
                                if ( lstat( QFile::encodeName(i.current()->absFilePath()), &info ) == 0 ) {
                                    if ( device == info.st_dev ) {
                                        dirStack.push( new QString( i.current()->absFilePath() ) );
                                    }
                                }
                            } else {
                                dirStack.push( new QString( i.current()->absFilePath() ) );
                            }
                        } else if ( ( !useSnapshot ) || ( i.current()->lastModified() > mtime ) ) {
                            size += i.current()->size() / 512;
                            if ( i.current()->size() % 512 ) {
                                size++;
                            }
                        }
                    }
                }
            }
        }
    }

    delete _backupdialog;   // we're done, so zap this widget

    // Convert size in tar blocks to size in kbytes.
    return_value = size / 2;

    if( stop_flag == TRUE ) return_value = -1;

    return return_value;
}

void KDatMainWindow::getBackupFiles( QStringList& files )
{
    if ( !_rootNode->isSelected() && !_rootNode->hasSelectedChildren() ) {
        // Backup the entire subtree.
        if ( ( _tree->getCurrentItem() ) && ( ((Node*)_tree->getCurrentItem())->isType( Node::ArchiveableNodeType ) ) ) {
            files.append( ((ArchiveableNode*)_tree->getCurrentItem())->getFullPath() );
        }
    } else {
        // Compile a list of files to backup.
        QPtrStack<ArchiveableNode> stack;
        stack.push( _rootNode );
        ArchiveableNode* sel = 0;
        while ( stack.count() > 0 ) {
            sel = stack.pop();
            if ( sel->isSelected() ) {
                files.append( sel->getFullPath() );
            } else if ( sel->hasSelectedChildren() ) {
                for ( int i = sel->childCount() - 1; i >= 0; i-- ) {
                    stack.push( (ArchiveableNode*)sel->childAt( i ) );
                }
            }
        }
    }
}

void KDatMainWindow::setBackupFiles( const QStringList& files )
{
    _rootNode->setSelected( FALSE );

    QString tmp;
    QStringList filesTmp = files;
    QStringList::Iterator it = filesTmp.begin();
    for ( ; it != filesTmp.end(); ++it ) {
        ArchiveableNode* n = _rootNode;
        while ( n ) {
            if ( n->getFullPath() == *it ) {
                n->setSelected( TRUE );
                n = 0;
            } else {
                if ( n->childCount() == 0 ) {
                    bool dummy = TRUE;
                    n->expanding( dummy );
                }
                int i;
                for ( i = n->childCount() - 1; i >=0; i-- ) {
                    tmp = ((ArchiveableNode*)n->childAt( i ))->getFullPath();
                    if ( tmp == (*it).left( tmp.length() ) ) {
                        n = (ArchiveableNode*)n->childAt( i );
                        break;
                    }
                }
                if ( i < 0 ) {
                    n = 0;
                }
            }
        }
    }
}

void KDatMainWindow::slotTapeDevice()
{
    setTapePresent( FALSE );
}

void KDatMainWindow::slotTapeMounted()
{
    configureUI( TapeManager::instance()->getMountedTape() );
}

void KDatMainWindow::slotTapeUnmounted()
{
    configureUI( 0 );
}

void KDatMainWindow::configureUI( Tape* tape )
{
    if ( _destroyed ) {
        return;
    }

    Node* sel  = (Node*)_tree->getCurrentItem();

    // Title bar
    if ( tape ) {
        QString title;
        title = i18n( "KDat: %1" ).arg(tape->getName());
        setCaption( title );
        setIconText( title );
    } else {
        setCaption( i18n( "KDat: <no tape>" ) );
        setIconText( i18n( "KDat: <no tape >" ) );
    }

    // Backup
    bool canBackup = ( tape ) && ( !TapeDrive::instance()->isReadOnly() ) &&
        ( ( ( sel ) && ( sel->isType( Node::ArchiveableNodeType ) || sel->isType( Node::BackupProfileNodeType ) ) ) ||
          ( _rootNode->isSelected() || _rootNode->hasSelectedChildren() ) );
    _toolbar->setItemEnabled( 1, canBackup );
    _fileMenu->setItemEnabled( _fileMenu->idAt( 0 ), canBackup );

    // Restore/verify
    bool canRestore = ( tape ) && ( sel ) && sel->isType( Node::RangeableNodeType );
    if ( !canRestore ) {
        for ( int i = _tapeDriveNode->childCount() - 1; i >= 0; i-- ) {
            if ( ( ((SelectableNode*)_tapeDriveNode->childAt( i ))->isSelected() ) ||
                 ( ((SelectableNode*)_tapeDriveNode->childAt( i ))->hasSelectedChildren() )) {
                canRestore = TRUE;
                break;
            }
        }
    }
    if ( canRestore ) {
      // 2002-01-30 LEW: check Node *sel because we can have canRestore==true
      // even if sel==NULL when a child is selected (see loop above).
      if( sel != (Node *)0x0 )
      {
        for ( Node* parent = (Node*)sel->getParent();
              ( parent ) && ( parent->getParent() );
              parent = (Node*)parent->getParent() ) {
            if ( parent->isType( Node::TapeNodeType ) ) {
                canRestore = FALSE;
            }
        }
      }
      // 2002-01-30 LEW: see if we ever get here...
      else
	{
	printf("Found sel==0x0 in %s %d\n", __FILE__, __LINE__);
	}
      // 2002-01-30 LEW

    }
    _toolbar->setItemEnabled( 2, canRestore );
    _toolbar->setItemEnabled( 3, canRestore );
    _fileMenu->setItemEnabled( _fileMenu->idAt( 1 ), canRestore );
    _fileMenu->setItemEnabled( _fileMenu->idAt( 2 ), canRestore );

    // Mount/unmount tape
    if ( tape ) {
        _toolbar->setButtonPixmap( 0, *ImageCache::instance()->getTapeMounted() );
        _fileMenu->changeItem( i18n( "Unmount Tape" ), _fileMenu->idAt( 3 ) );
    } else {
        _toolbar->setButtonPixmap( 0, *ImageCache::instance()->getTapeUnmounted() );
        _fileMenu->changeItem( i18n( "Mount Tape" ), _fileMenu->idAt( 3 ) );
    }

    // Index tape
    _fileMenu->setItemEnabled( _fileMenu->idAt( 4 ), tape != NULL );

    // Delete archive
    _fileMenu->setItemEnabled( _fileMenu->idAt( 7 ), ( sel ) && sel->isType( Node::ArchiveNodeType ) );

    // Delete index
    _fileMenu->setItemEnabled( _fileMenu->idAt( 8 ), ( sel ) && sel->isType( Node::TapeNodeType ) );

    // Delete backup profile
    _fileMenu->setItemEnabled( _fileMenu->idAt( 9 ), ( sel ) && sel->isType( Node::BackupProfileNodeType ) );

    // Format tape
    _fileMenu->setItemEnabled( _fileMenu->idAt( 10 ), TapeManager::instance()->getMountedTape() && !TapeDrive::instance()->isReadOnly() );
}

void KDatMainWindow::readProperties( KConfig* config )
{
  QValueList<int> sizes;
  sizes << config->readNumEntry( "panner", 50 );
  _panner->setSizes( sizes );
}

void KDatMainWindow::saveProperties( KConfig* config )
{
    config->writeEntry( "panner", _panner->sizes().first());
}

// Create the dialog that lets user cancel estimation of backup size
void KDatMainWindow::create_backup_dialog()
{
  /* 2002-01-24 LEW:  start of backup-cancel dialog */
  static QString stop_button_text, stop_button_caption;
  stop_button_text =
    i18n("Click \"CANCEL\" to stop the backup process.\n"
	 "For example, you may quickly see that the size of\n"
	 "the files you selected will exceed the size of the\n"
	 "backup tape, and may then decide to stop and remove\n"
	 "some files from your list of files to backup.\n\n"
         "Click \"Continue\" to remove this message while\n"
         "continuing the backup.");
  stop_button_caption = i18n("Stop estimating backup size");

        // gotta put a callback in here.  Probably need to create a
        // dialog object  2002-01-22 LEW
  _backupdialog = new KDialog( this, "backupdialog", false );
  _backupdialog->setCaption( stop_button_caption );
  //  _backupdialog->resize(370,200);  /* 2002-01-28 LEW */
  _backupdialog->resize(370,260);

  _lbl = new QLabel( stop_button_text, _backupdialog );
  //  _lbl->setGeometry( QRect( 30, 20, 350, 140 ) );  /* 2002-01-28 LEW */
  _lbl->setGeometry( QRect( 30, 20, 350, 200 ) );

  _cancel = new KPushButton( KStdGuiItem::cancel(), _backupdialog );
  _cancel->setFixedSize( 80, _cancel->sizeHint().height() );
  _cancel->setEnabled( TRUE );
  /* 2002-01-24 LEW: looks like we can't increase the button width
     to accomodate a wider message :( */
  //  _cancel->setGeometry( QRect( 50, 170, 0, 0 ) );   /* 2002-01-28 LEW */
  _cancel->setGeometry( QRect( 50, 230, 0, 0 ) );
  connect( _cancel, SIGNAL( clicked() ), this, SLOT( backupCancel() ) );

  _continue = new KPushButton( KStdGuiItem::cont(), _backupdialog );
  _continue->setFixedSize( 80, _continue->sizeHint().height() );
  _continue->setEnabled( TRUE );
  _continue->setDefault( TRUE );
  //  _continue->setGeometry( QRect( 200, 170, 0, 0 ) );   /* 2002-01-28 LEW */
  _continue->setGeometry( QRect( 200, 230, 0, 0 ) );
  connect( _continue, SIGNAL( clicked() ), this, SLOT( backupContinue() ) );

  _backupdialog->show();
}

// stop calculating the backup size and hide the dialog screen
void KDatMainWindow::backupCancel()
{
  stop_flag = TRUE;
  _backupdialog->hide();
}

// continue calculating the backup size and hide the dialog screen
void KDatMainWindow::backupContinue()
{
  _backupdialog->hide();
}
