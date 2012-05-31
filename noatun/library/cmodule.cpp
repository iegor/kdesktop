#include <cmodule.h>
#include <noatun/pluginloader.h>
#include <common.h>
#include <noatun/app.h>

#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <klocale.h>
#include <kdebug.h>
#include <klistview.h>
#include <qsplitter.h>
#include <qlabel.h>
#include <qdragobject.h>
#include <kurlrequester.h>
#include <kfiledialog.h>
#include <kdialog.h>
#include <klineedit.h>

#include <qtextview.h>
#include <qwhatsthis.h>

#include "mimetypetree.h"

/*****************************************************************
 * General options
 *****************************************************************/

General::General(QObject *parent)
	: CModule(i18n("General"), i18n("General Options"), "configure", parent)
{
	mLoopList=new QCheckBox(i18n("&Return to start of playlist on finish"), this);
	mLoopList->setChecked(napp->loopList());
	QWhatsThis::add(mLoopList, i18n("When the playlist is finished playing, return to the start, but do not start playing."));

	mOneInstance=new QCheckBox(i18n("Allow only one &instance of Noatun"), this);
	mOneInstance->setChecked(napp->oneInstance());
	QWhatsThis::add(mOneInstance, i18n("Starting noatun a second time will cause it to just append items from the start to the current instance."));

	mClearOnOpen = new QCheckBox(i18n("Clear playlist &when opening a file"), this);
	mClearOnOpen->setChecked(napp->clearOnOpen());
	QWhatsThis::add(mClearOnOpen, i18n("Opening a file with the global Open menu item will clear the playlist first."));

	mFastVolume=new QCheckBox(i18n("&Use fast hardware volume control"), this);
	mFastVolume->setChecked(napp->fastMixer());
	QWhatsThis::add(mFastVolume, i18n("Use the hardware mixer instead of aRts'. It affects all streams, not just Noatun's, but is a little faster."));

	mRemaining=new QCheckBox(i18n("Display &remaining play time"), this);
	mRemaining->setChecked(napp->displayRemaining());
	QWhatsThis::add(mRemaining, i18n("Counters count down towards zero, showing remaining time instead of elapsed time."));

	QLabel *titleLabel=new QLabel(i18n("Title &format:"), this);
	mTitleFormat=new KLineEdit(this);
	titleLabel->setBuddy(mTitleFormat);
	mTitleFormat->setText(napp->titleFormat());
	QWhatsThis::add(mTitleFormat, i18n(
			"Select a title to use for each file (in the playlist and user interface). "
			"Each element such as $(title) is replaced with the property with the name "
			"as given in the parentheses. The properties include, but are not limited to: "
			"title, author, date, comments and album."));

	QLabel *dlsaver=new QLabel(i18n("&Download folder:"), this);
	mDlSaver=new KURLRequester(napp->saveDirectory(), this);
	dlsaver->setBuddy(mDlSaver);
	connect( mDlSaver, SIGNAL( openFileDialog( KURLRequester * )),
		 this, SLOT( slotRequesterClicked( KURLRequester * )));
	QWhatsThis::add(mDlSaver, i18n("When opening a non-local file, download it to the selected folder."));

	mPlayOnStartup = new QButtonGroup(1, Horizontal, i18n("Play Behavior on Startup"), this);
	mPlayOnStartup->setExclusive(true);
	mPlayOnStartup->insert(
			new QRadioButton(i18n("Restore &play state"), mPlayOnStartup),
			NoatunApp::Restore
		);
	mPlayOnStartup->insert(
			new QRadioButton(i18n("Automatically play &first file"), mPlayOnStartup),
			NoatunApp::Play
		);
	mPlayOnStartup->insert(
			new QRadioButton(i18n("&Do not start playing"), mPlayOnStartup),
			NoatunApp::DontPlay
		);

	if (QButton* b = mPlayOnStartup->find(napp->startupPlayMode()))
	{
		b->toggle();
	}

	QGridLayout *layout = new QGridLayout(this, 0, KDialog::spacingHint());
	layout->setSpacing(KDialog::spacingHint());

	layout->addMultiCellWidget(mLoopList, 0, 0, 0, 1);
	layout->addMultiCellWidget(mOneInstance, 2, 2, 0, 1);
	layout->addMultiCellWidget(mClearOnOpen, 4, 4, 0, 1);
	layout->addMultiCellWidget(mFastVolume, 5, 5, 0, 1);
	layout->addMultiCellWidget(mRemaining, 6, 6, 0, 1);

	layout->addWidget(titleLabel, 7, 0);
	layout->addWidget(mTitleFormat, 7, 1);

	layout->addWidget(dlsaver, 8, 0);
	layout->addWidget(mDlSaver, 8, 1);

	layout->addMultiCellWidget(mPlayOnStartup, 9, 9, 0, 1);

	layout->setRowStretch(10, 1);
}


void General::save()
{
	napp->setLoopList(mLoopList->isChecked());
	napp->setOneInstance(mOneInstance->isChecked());
	napp->setClearOnOpen(mClearOnOpen->isChecked());
	napp->setSaveDirectory(mDlSaver->url());
	napp->setFastMixer(mFastVolume->isChecked());
	napp->setTitleFormat(mTitleFormat->text());
	napp->setDisplayRemaining(mRemaining->isChecked());
	napp->setStartupPlayMode(mPlayOnStartup->selectedId());
}


void General::slotRequesterClicked( KURLRequester * )
{
	mDlSaver->fileDialog()->setMode(
		(KFile::Mode)(KFile::Directory | KFile::ExistingOnly | KFile::LocalOnly));
}

#include "cmodule.moc"
