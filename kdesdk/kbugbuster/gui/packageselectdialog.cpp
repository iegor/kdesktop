#include <qlistview.h>
#include <qlayout.h>
#include <qheader.h>

#include <kdebug.h>
#include <kcompletion.h>
#include <klineedit.h>

#include "bugsystem.h"
#include "kbbprefs.h"
#include "bugserver.h"

#include "packagelvi.h"
#include "packageselectdialog.h"
#include "packageselectdialog.moc"

PackageListView::PackageListView( QWidget *parent ) :
    QListView( parent )
{
    setFocusPolicy( QWidget::StrongFocus );
}

void PackageListView::resetTyped()
{
    mTyped = "";
}

void PackageListView::keyPressEvent( QKeyEvent *e )
{
    // Disable listview text completion for now
    QListView::keyPressEvent( e );
    return;

    int k = e->key();
    if ( k == Key_Return || k == Key_Escape ) e->ignore();

    QString key = e->text();
    mTyped.append(key);
    emit typed( mTyped );
}

PackageSelectDialog::PackageSelectDialog(QWidget *parent,const char *name) :
  KDialogBase( parent, name, true, i18n("Select Product"), Ok|Cancel )
{
    QWidget *topWidget = new QWidget( this );
    setMainWidget( topWidget );

    QBoxLayout *topLayout = new QVBoxLayout( topWidget );
    QSplitter *topSplitter = new QSplitter( QSplitter::Vertical, topWidget );
    topSplitter->setOpaqueResize( true );

    topLayout->addWidget( topSplitter );

    mRecentList = new QListView( topSplitter );
    mRecentList->addColumn( i18n("Recent") );
    mRecentList->resize( mRecentList->width(), mRecentList->fontMetrics().height() *
			 KBBPrefs::instance()->mRecentPackagesCount );
    
    connect( mRecentList, SIGNAL( mouseButtonPressed( int, QListViewItem *, const QPoint &, int) ),
             SLOT( recentSelected( int, QListViewItem * ) ) );
    connect( mRecentList, SIGNAL( doubleClicked( QListViewItem * ) ),
             SLOT( slotOk() ) );

    mCompletion = new KCompletion;
    mCompletion->setCompletionMode( KGlobalSettings::CompletionAuto );

    mCompleteList = new PackageListView( topSplitter );
    mCompleteList->addColumn( i18n("Name") );
    mCompleteList->addColumn( i18n("Description") );
    mCompleteList->setRootIsDecorated(true);
    mCompleteList->setAllColumnsShowFocus( true );
    connect( mCompleteList, SIGNAL( typed( const QString & ) ),
             SLOT( completeTyped( const QString & ) ) );


    connect( mCompleteList, SIGNAL( mouseButtonPressed( int, QListViewItem *, const QPoint &, int) ),
             SLOT( completeSelected( int, QListViewItem * ) ) );
    connect( mCompleteList, SIGNAL( doubleClicked( QListViewItem * ) ),
             SLOT( slotOk() ) );

    mPackageEdit = new KLineEdit( topWidget );
    mPackageEdit->setFocus();

    topLayout->addWidget( mPackageEdit );
    connect( mPackageEdit, SIGNAL( textChanged( const QString & ) ),
             SLOT( completeTyped( const QString & ) ) );
    enableButtonOK( !mPackageEdit->text().isEmpty() );
}

PackageSelectDialog::~PackageSelectDialog()
{
    delete mCompletion;
}

void PackageSelectDialog::setRecentPackages( const QStringList &recent )
{
    mRecentList->clear();
    QStringList::ConstIterator it;
    for( it = recent.begin(); it != recent.end(); ++it ) {
        new QListViewItem( mRecentList, *it );
    }
}

void PackageSelectDialog::setPackages( const Package::List &pkgs )
{
    mCompleteList->clear();
    mCompletion->clear();
    mCompletionDict.clear();
    Package::List::ConstIterator it;
    for( it = pkgs.begin(); it != pkgs.end(); ++it ) {
        PackageLVI *item = new PackageLVI( mCompleteList, (*it), QString::null );
        QStringList components = (*it).components();

        if (components.count() > 1) {
           for( QStringList::ConstIterator cit = components.begin(); cit != components.end(); ++cit ) {
               PackageLVI *component = new PackageLVI( item, (*it), (*cit) );
               QString completionName = (*it).name() + "/" + (*cit);

               mCompletion->addItem( completionName );
               mCompletionDict.insert( completionName, component );
           }
        }

        mCompletion->addItem( (*it).name() );
        mCompletionDict.insert((*it).name(),item);
    }
}

void PackageSelectDialog::recentSelected( int, QListViewItem *item )
{
    kdDebug() << "PackageSelectDialog::recentSelected()" << endl;
    if ( item ) {
        mCompleteList->clearSelection();
        // Why does a QLineEdit->setText() call emit the textChanged() signal?
        mPackageEdit->blockSignals( true );
        mPackageEdit->setText( item->text( 0 ) );
        enableButtonOK(true);
        mPackageEdit->blockSignals( false );
    }
}

void PackageSelectDialog::completeSelected( int, QListViewItem *item )
{
    PackageLVI *lvi = dynamic_cast<PackageLVI*>(item);

    if ( lvi ) {
        mRecentList->clearSelection();
        if ( lvi->component().isEmpty() ) {
            mPackageEdit->setText( lvi->package().name() );
        }
        else {
            mPackageEdit->setText( lvi->package().name() + "/" + lvi->component() );
        }
    }
}

void PackageSelectDialog::slotOk()
{
    PackageLVI *item = (PackageLVI *)mCompleteList->selectedItem();
    if ( item ) {
        mSelectedPackage = item->package();
        mSelectedComponent = item->component();

        QString recent_key;
        if ( item->component().isEmpty() )
            recent_key = item->package().name();
        else
            recent_key = item->package().name() + "/" + item->component();

        BugServer *server = BugSystem::self()->server();
        QStringList recent = server->serverConfig().recentPackages();
        if( !recent.contains( recent_key ) ) {
            recent.prepend( recent_key );
            if ( int( recent.count() ) > KBBPrefs::instance()->mRecentPackagesCount ) {
                recent.remove( recent.last() );
            }
            kdDebug() << "RECENT: " << recent.join(",") << endl;
            server->serverConfig().setRecentPackages( recent );
        }
    } else {
        QListViewItem *recentItem = mRecentList->selectedItem();
        if ( recentItem ) {
            QStringList tokens = QStringList::split( '/', recentItem->text( 0 ) );
            mSelectedPackage = BugSystem::self()->package( tokens[0] );
            mSelectedComponent = tokens[1];
        }
    }
    mCompleteList->resetTyped();
    accept();
}

Package PackageSelectDialog::selectedPackage()
{
    return mSelectedPackage;
}

QString PackageSelectDialog::selectedComponent()
{
    return mSelectedComponent;
}

void PackageSelectDialog::completeTyped( const QString &typed )
{
    kdDebug() << "completeTyped: " << typed << endl;
    QString completed = mCompletion->makeCompletion( typed );
    kdDebug() << "completed:     " << completed << endl;
    if ( !completed.isEmpty() ) {
      mCompleteList->setSelected( mCompletionDict[ completed ], true );
      mCompleteList->ensureItemVisible( mCompletionDict[ completed ] );
    } else {
      mCompleteList->resetTyped();
    }
    enableButtonOK( !typed.isEmpty() );
}
