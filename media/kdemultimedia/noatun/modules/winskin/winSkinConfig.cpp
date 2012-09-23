#include <noatun/pref.h>

#include <klocale.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <kglobal.h>
#include <kconfig.h>
#include <qslider.h>
#include <qframe.h>
#include <qstringlist.h>
#include <kfile.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kurlrequester.h>
#include <kurlrequesterdlg.h>
#include <qdir.h>

#include "waSkin.h"
#include "waInfo.h"
#include "waSkinManager.h"
#include "winSkinConfig.h"

WinSkinConfig::WinSkinConfig(QWidget * parent, WaSkinManager *waSkinManager) :
        CModule(i18n("Winskin"),
	        i18n("Skin Selection for the Winskin Plugin"),
	        "style",
	        parent)
{
    // Make a token horizontal layout box
    vbox = new QVBoxLayout(this);
    vbox->setSpacing( 6 );
    vbox->setMargin( 0 );

    // Add a simple list of skins, populated in WinSkinConfig::reopen()
    skin_list = new QListBox(this, "skin_list");
    vbox->addWidget(skin_list);

    QHBoxLayout* hbox = new QHBoxLayout( 0, 6, 6 );

    QPushButton* buttonInstall = new QPushButton( i18n("&Install New Skin..."), this );
    hbox->addWidget(buttonInstall);

    buttonRemove = new QPushButton( i18n("&Remove Skin"), this );
    buttonRemove->setEnabled(false);
    hbox->addWidget(buttonRemove);
    vbox->addLayout(hbox);

    connect( skin_list, SIGNAL(highlighted(const QString &)), this, SLOT(selected()));
    connect( buttonInstall, SIGNAL(clicked()), this, SLOT(install()));
    connect( buttonRemove, SIGNAL(clicked()), this, SLOT(remove()));
    connect(waSkinManager, SIGNAL(updateSkinList()), this, SLOT(reopen()));

    mWaSkinManager = waSkinManager;

    QGroupBox *settingsBox = new QGroupBox( 1, Vertical, i18n("Settings"), this );
    vbox->addWidget(settingsBox);

    QHBox *box = new QHBox(settingsBox);
    QLabel *label = new QLabel(i18n("T&itle scrolling speed:"), box);
    new QLabel(i18n("None"), box);

    scrollSpeed = new QSlider(box);
    label->setBuddy(scrollSpeed);
    scrollSpeed->setMinimumSize( QSize( 80, 0 ) );
    scrollSpeed->setMinValue( 0 );
    scrollSpeed->setMaxValue( 50 );
    scrollSpeed->setPageStep( 1 );
    scrollSpeed->setOrientation( QSlider::Horizontal );
    scrollSpeed->setTickmarks( QSlider::NoMarks );

	label = new QLabel(i18n("Fast"), box);

    reopen();
}

void WinSkinConfig::save()
{
    KConfig *config=KGlobal::config();
    config->setGroup("Winskin");
    config->writeEntry("CurrentSkin", skin_list->currentText());
    config->writeEntry("ScrollDelay", scrollSpeed->value());
    config->sync();

    if (skin_list->currentText() != orig_skin) {
        _waskin_instance->loadSkin(skin_list->currentText());
        orig_skin = skin_list->currentText();
    }
    else
    {
      _waskin_instance->skinInfo()->scrollerSetup();
    }
}

void WinSkinConfig::reopen() {
    // Wipe out the old list
    skin_list->clear(); 

    // Get a list of skins
    QStringList skins = mWaSkinManager->availableSkins();

    // This loop adds them all to our skin list
    for(unsigned int x = 0;x < skins.count();x++) {
        // Add ourselves to the list
        skin_list->insertItem(skins[x]);
    }

    // Figure out our current skin
    QString orig_skin = mWaSkinManager->currentSkin();

    // Where is that skin in our big-list-o-skins?
    QListBoxItem *item = skin_list->findItem(orig_skin);

    if (item) {
        // Aha, found it... make it the currently selected skin
        skin_list->setCurrentItem( item );
    }
    else {
        // Er, it's not there... select the current item
        // Maybe this should emit a warning? Oh well, it's not harmful
        skin_list->setCurrentItem( 0 );
    }
	
    KConfig *config=KGlobal::config();
    config->setGroup("Winskin");
	scrollSpeed->setValue(config->readNumEntry("ScrollDelay", 15));
}

void WinSkinConfig::selected()
{
    buttonRemove->setEnabled(mWaSkinManager->skinRemovable( skin_list->currentText() ));
}

void WinSkinConfig::install()
{
   QString url;

    // Ask the user for directory containing a skin
    KURLRequesterDlg* udlg = new KURLRequesterDlg( QString::null, this, "udlg", true );
    udlg->urlRequester()->setFilter(mWaSkinManager->skinMimeTypes().join(" "));
    udlg->urlRequester()->setMode( KFile::File | KFile::Directory | KFile::ExistingOnly );

    if( udlg->exec() == QDialog::Accepted ) {
        url = udlg->urlRequester()->url();
        mWaSkinManager->installSkin( url );
    }
}

void WinSkinConfig::remove()
{
    // Is there any item selected ??
    if( skin_list->currentText().isEmpty() )
        return;

    // We can't remove every skin
    if( !mWaSkinManager->skinRemovable( skin_list->currentText() ) ) {
        KMessageBox::information( this, i18n("You cannot remove this skin.") );
        // Reload skin list, perhaps the skin is already removed!
        return;
    }

    // Ask the user first
    if( KMessageBox::warningContinueCancel( this,
        i18n("<qt>Are you sure you want to remove the <b>%1</b> skin?</qt>").arg( skin_list->currentText() ), QString::null, KStdGuiItem::del() )
        == KMessageBox::Continue ) {

        mWaSkinManager->removeSkin( skin_list->currentText() );
        reopen();
    }
}

#include <winSkinConfig.moc>
