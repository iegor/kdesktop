/***************************************************************************
	kjprefs.cpp  -  Preferences-Dialog for KJ�ol-Skinloader
	--------------------------------------------------------
	Maintainer: Stefan Gehn <sgehn@gmx.net>

 ***************************************************************************/

// local includes
#include "kjprefs.h"
#include "kjprefs.moc"
#include "kjloader.h"
#include "kjwidget.h"
#include "kjvis.h"
#include "parser.h"

// system includes
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qslider.h>
#include <qpixmap.h>
#include <qtabwidget.h>
#include <qtextbrowser.h>
#include <qfileinfo.h>
#include <qstringlist.h>
#include <qregexp.h>

#include <knuminput.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kio/job.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kmimemagic.h>
#include <knotifyclient.h>
#include <kprocess.h>
#include <kstandarddirs.h>
#include <kglobalsettings.h>
#include <kfontcombo.h>
#include <kcolorcombo.h>

static QString expand(QString s);

KJPrefs::KJPrefs(QObject* parent)
	: CModule(i18n("K-Jöfol Skins"), i18n("Skin Selection For the K-Jöfol Plugin"), "style", parent)
{
	cfg = KGlobal::config();

	QVBoxLayout *vbox = new QVBoxLayout(this);
	vbox->setAutoAdd(true);
	vbox->setSpacing( 0 );
	vbox->setMargin( 0 );

	mTabWidget = new QTabWidget( this, "mTabWidget" );

	mSkinselectorWidget = new KJSkinselector ( mTabWidget, "mSkinselectorWidget" );
	mGuiSettingsWidget = new KJGuiSettings ( mTabWidget, "mGuiSettingsWidget" );

	mTabWidget->insertTab( mSkinselectorWidget, i18n("&Skin Selector") );
	mTabWidget->insertTab( mGuiSettingsWidget, i18n("O&ther Settings") );

	connect ( mSkinselectorWidget->mSkins, SIGNAL(activated(const QString&)), SLOT(showPreview(const QString&)) );
	connect ( mSkinselectorWidget->installButton, SIGNAL(clicked()), this, SLOT(installNewSkin()) );
	connect ( mSkinselectorWidget->mRemoveButton, SIGNAL(clicked()), this, SLOT(removeSelectedSkin()) );

	reopen(); // fill the skinlist and draw a preview
}


void KJPrefs::reopen() // reload config and set stuff in dialog
{
//	kdDebug(66666) << "[KJPrefs] reopen()" << endl;

	cfg->setGroup("KJofol-Skins");

//	mGuiSettingsWidget->timeCountdown->setChecked( cfg->readBoolEntry("TimeCountMode", false) );
	mGuiSettingsWidget->displayTooltips->setChecked( cfg->readBoolEntry("DisplayTooltips", true) );
	mGuiSettingsWidget->displaySplash->setChecked( cfg->readBoolEntry("DisplaySplashScreen", true) );

	mGuiSettingsWidget->minPitch->setValue( cfg->readNumEntry("minimumPitch", 50) );
	mGuiSettingsWidget->maxPitch->setValue( cfg->readNumEntry("maximumPitch", 200) );
	mGuiSettingsWidget->visTimerValue->setValue( cfg->readNumEntry("VisualizationSpeed", 30) );

	mGuiSettingsWidget->useSysFont->setChecked( cfg->readBoolEntry("Use SysFont", false) );
	mGuiSettingsWidget->cmbSysFont->setCurrentFont(
		cfg->readEntry("SysFont Family", KGlobalSettings::generalFont().family()) );
	QColor tmpColor = QColor(255,255,255);
	mGuiSettingsWidget->cmbSysFontColor->setColor(
		cfg->readColorEntry("SysFont Color", &tmpColor));

	// TODO somehow honor both config-entries, I want a custom mode
	switch ( cfg->readNumEntry("TitleScrollSpeed", 400 ) )
	{
		case 800:
			mGuiSettingsWidget->titleScrollSpeed->setValue(1);
			break;
		case 400:
			mGuiSettingsWidget->titleScrollSpeed->setValue(2);
			break;
		case 200:
			mGuiSettingsWidget->titleScrollSpeed->setValue(3);
			break;
	}

	switch ( cfg->readNumEntry("AnalyzerType", KJVisScope::FFT ) )
	{
		case KJVisScope::Null:
			mGuiSettingsWidget->visNone->setChecked(true);
			mGuiSettingsWidget->visScope->setChecked(false);
			mGuiSettingsWidget->visAnalyzer->setChecked(false);
			break;

		case KJVisScope::FFT:
			mGuiSettingsWidget->visNone->setChecked(false);
			mGuiSettingsWidget->visScope->setChecked(false);
			mGuiSettingsWidget->visAnalyzer->setChecked(true);
			break;

		case KJVisScope::Mono:
			mGuiSettingsWidget->visNone->setChecked(false);
			mGuiSettingsWidget->visScope->setChecked(true);
			mGuiSettingsWidget->visAnalyzer->setChecked(false);
			break;
	}

	QStringList skins;
	QStringList skinLocations = KGlobal::dirs()->findDirs("data", "noatun/skins/kjofol");
	// iterate through all paths where Noatun is searching for kjofol-skins
	for (uint i = 0; i < skinLocations.count(); ++i )
	{
		QStringList skinDirs = QDir(skinLocations[i]).entryList();
		// iterate trough all dirs (normally, users can fsck every dir-struct *g*) containing a skin
		for (uint k = 2; k < skinDirs.count(); ++k )
		{
			QDir skinDirCnt = QDir ( skinLocations[i]+skinDirs[k], "*.rc", QDir::Name|QDir::IgnoreCase, QDir::Files );
			// make a list of all .rc-files in a skindir
			QStringList rcFiles = skinDirCnt.entryList();
			// iterate trough all those rc.-files in a skindir
			for (uint j = 0; j < rcFiles.count(); j++ )
			{
//				kdDebug(66666) << "found: " <<  rcFiles[j].latin1() << endl;
				skins += ( rcFiles[j] );
			}
		}
	}

	skins.sort();

	QString loaded = cfg->readEntry("SkinResource", locate("data", "noatun/skins/kjofol/kjofol/kjofol.rc") );
	loaded = loaded.mid(loaded.findRev("/")+1);  // remove path
	loaded = loaded.left( loaded.length() - 3 ); // remove ".rc"

	mSkinselectorWidget->mSkins->clear();

	int index = 0;
	for (QStringList::Iterator i=skins.begin(); i!=skins.end(); ++i)
	{
		*i = (*i).left( (*i).length() - 3 );
		mSkinselectorWidget->mSkins->insertItem(*i);
		if ( (*i) == loaded )
			index = mSkinselectorWidget->mSkins->count()-1; // save index no. to set active item later on
	}

	mSkinselectorWidget->mSkins->setCurrentItem(index);

	showPreview( mSkinselectorWidget->mSkins->currentText() );
}


void KJPrefs::save()
{
// 	kdDebug(66666) << k_funcinfo << "called." << endl;
	QString skin=::expand ( mSkinselectorWidget->mSkins->currentText() );

	// first load skin and then save config to prevent
	// reloading a broken skin after a crash
	KJLoader *l=KJLoader::kjofol;
	if (l)
		l->loadSkin(skin);

	cfg->setGroup("KJofol-Skins");

	cfg->writeEntry("SkinResource", skin);
//	cfg->writeEntry("TimeCountMode", timeCountMode() );
	cfg->writeEntry("DisplayTooltips", displayTooltips() );
	cfg->writeEntry("DisplaySplashScreen", displaySplash() );

	cfg->writeEntry("TitleScrollSpeed", titleMovingUpdates() );
	cfg->writeEntry("TitleScrollAmount", titleMovingDistance() );
	cfg->writeEntry("AnalyzerType", (int)visType() );
	cfg->writeEntry("minimumPitch", minimumPitch() );
	cfg->writeEntry("maximumPitch", maximumPitch() );
	cfg->writeEntry("VisualizationSpeed", visTimerValue() );

	cfg->writeEntry("Use SysFont", mGuiSettingsWidget->useSysFont->isChecked());
	cfg->writeEntry("SysFont Family", mGuiSettingsWidget->cmbSysFont->currentFont());
// 	kdDebug(66666) << k_funcinfo << "currentfont=" << mGuiSettingsWidget->cmbSysFont->currentFont() << endl;
	cfg->writeEntry("SysFont Color", mGuiSettingsWidget->cmbSysFontColor->color());

	cfg->sync();

	emit configChanged();
}

QString KJPrefs::skin( void ) const
{
	// return full path to currently loaded skin
	return ::expand( mSkinselectorWidget->mSkins->currentText() );
}

int KJPrefs::minimumPitch( void ) const
{
	return mGuiSettingsWidget->minPitch->value();
}

int KJPrefs::maximumPitch( void ) const
{
	return mGuiSettingsWidget->maxPitch->value();
}

int KJPrefs::visTimerValue ( void ) const
{
	return mGuiSettingsWidget->visTimerValue->value();
}

int KJPrefs::titleMovingUpdates ( void ) const
{
	switch ( mGuiSettingsWidget->titleScrollSpeed->value() )
	{
		case 1:
			return 800;
		case 2:
			return 400;
		case 3:
			return 200;
		default:
			return 400; // emergency exit :)
	}
}

float KJPrefs::titleMovingDistance ( void ) const
{
	switch ( mGuiSettingsWidget->titleScrollSpeed->value() )
	{
		case 1:
			return 0.2f;
		case 2:
			return 0.5f;
		case 3:
			return 1.0f;
		default:
			return 0.5f; // emergency exit :)
	}
}

int KJPrefs::visType ( void ) const
{
	if ( mGuiSettingsWidget->visNone->isChecked() ) // No Vis
		return KJVisScope::Null;
	else if ( mGuiSettingsWidget->visScope->isChecked() ) // MonoScope
		return KJVisScope::Mono;
	else if ( mGuiSettingsWidget->visAnalyzer->isChecked() ) // FFT Analyzer
		return KJVisScope::FFT;
	else
		return KJVisScope::StereoFFT; //Null; // emergency exit :)
}

void KJPrefs::setVisType ( int vis )
{
	switch ( vis )
	{
		case KJVisScope::Null:
			mGuiSettingsWidget->visNone->setChecked(true);
			mGuiSettingsWidget->visScope->setChecked(false);
			mGuiSettingsWidget->visAnalyzer->setChecked(false);
			break;

		case KJVisScope::FFT:
			mGuiSettingsWidget->visNone->setChecked(false);
			mGuiSettingsWidget->visScope->setChecked(false);
			mGuiSettingsWidget->visAnalyzer->setChecked(true);
			break;

		case KJVisScope::StereoFFT:
			mGuiSettingsWidget->visNone->setChecked(false);
			mGuiSettingsWidget->visScope->setChecked(false);
			mGuiSettingsWidget->visAnalyzer->setChecked(false);
			break;

		case KJVisScope::Mono:
			mGuiSettingsWidget->visNone->setChecked(false);
			mGuiSettingsWidget->visScope->setChecked(true);
			mGuiSettingsWidget->visAnalyzer->setChecked(false);
			break;
	}
	save(); // not sure if that's a good idea or doing saving by hand in here
}


bool KJPrefs::useSysFont( void ) const
{
	return mGuiSettingsWidget->useSysFont->isChecked();
}

void KJPrefs::setUseSysFont( bool mode )
{
	mGuiSettingsWidget->useSysFont->setChecked( mode );
	save(); // not sure if that's a good idea or doing saving by hand in here
}

QFont KJPrefs::sysFont(void) const
{
	QString family = mGuiSettingsWidget->cmbSysFont->currentFont();
//	kdDebug(66666) << k_funcinfo << "family=" << family << endl;
	return QFont( family );
}

void KJPrefs::setSysFont(QFont &fnt)
{
	mGuiSettingsWidget->cmbSysFont->setCurrentFont( fnt.family() );
}

QColor KJPrefs::sysFontColor(void) const
{
	return mGuiSettingsWidget->cmbSysFontColor->color();
}

void KJPrefs::sysFontColor(QColor &c)
{
	mGuiSettingsWidget->cmbSysFontColor->setColor( c );
}

bool KJPrefs::displayTooltips( void ) const
{
	return mGuiSettingsWidget->displayTooltips->isChecked();
}

bool KJPrefs::displaySplash( void ) const
{
	return mGuiSettingsWidget->displaySplash->isChecked();
}


void KJPrefs::showPreview(const QString &_skin)
{
	Parser p;
	p.open( ::expand(_skin) );

	QImage image = p.image(p["BackgroundImage"][1]);
	if (!image.isNull())
	{
		mPixmap.convertFromImage(image);
		mPixmap.setMask( KJWidget::getMask(image) );
	}
	else
		mPixmap=QPixmap();

	mSkinselectorWidget->mPreview->setPixmap(mPixmap);
	mSkinselectorWidget->mAboutText->setText(p.about());
	mSkinselectorWidget->updateGeometry();
}


/* =================================================================================== */


void KJPrefs::installNewSkin( void )
{
	bool skinInstalled = false; // flag showing wether a skindir got installed
	KURL src, dst; // sourcedir and destinationdir for skin-installation

	KURL srcFile ( mSkinselectorWidget->mSkinRequester->url() );

	//kdDebug(66666) << "file to work on: " << srcFile.path().latin1() << endl;

	if ( !srcFile.isValid() || srcFile.isEmpty() ) // stop working on broken URLs
	{
		kdDebug(66666) << "srcFile is malformed or empty !!!" << endl;
		return;
	}

	if ( !srcFile.isLocalFile() )  // TODO: Download file into tmp dir + unpack afterwards
	{
		KMessageBox::sorry ( this, i18n("Non-Local files are not supported yet") );
		return;
	}

	// Determine file-format trough mimetype (no stupid .ext test)
	KMimeMagicResult * result = KMimeMagic::self()->findFileType( srcFile.path() );

	if ( !result->isValid() )
	{
		kdDebug(66666) << "Could not determine filetype of srcFile !!!" << endl;
		return;
	}

	if ( result->mimeType() != "application/x-zip" )
	{
		KMessageBox::error ( this, i18n("The selected file does not appear to be a valid zip-archive") );
		return;
	}

	// create a dir with name of the skinarchive
	// path to unpack to: pathToTmp/filename.ext/
	QString tmpUnpackPath = locateLocal("tmp", srcFile.fileName()+"/" );
	kdDebug(66666) << "tmpUnpackPath: " << tmpUnpackPath.latin1() << endl;

	// Our extract-process, TODO: wanna have kio_(un)zip instead :)
	KShellProcess proc;

	// "unzip -d whereToUnpack whatToUnpack"
	proc << "unzip -d " << proc.quote(tmpUnpackPath) << " " << proc.quote(srcFile.path());
	kdDebug(66666) << "unzip -d " << tmpUnpackPath.latin1() << " " << srcFile.path().latin1() << endl;

	proc.start( KProcess::Block, KProcess::NoCommunication );

	// "unzip" spits out errorcodes > 0 only, 0 on success
	if ( proc.exitStatus() != 0 )
	{
		KMessageBox::error ( this, i18n("Extracting skin-archive failed") );
		// FIXME: Do I have to wait for the job to finish?
		// I'd say no because I don't care about the temp-dir
		// anyway after leaving this method :)
		KIO::del( tmpUnpackPath );
		return;
	}

	QDir tmpCnt = QDir ( tmpUnpackPath );
	tmpCnt.setFilter ( QDir::Dirs );

	QStringList dirList = tmpCnt.entryList();
	// Iterate trough all subdirs of tmpUnpackPath (including "."!)
	for ( unsigned int i = 0; i < dirList.count(); i++ )
	{
		// FIXME: is the following portable?
		if ( dirList[i] == ".." )
			continue;

		QDir tmpSubCnt = QDir( tmpUnpackPath + dirList[i], "*.rc;*.RC;*.Rc;*.rC", QDir::Name|QDir::IgnoreCase, QDir::Files );
		kdDebug(66666) << "Searching for *.rc in " << QString(tmpUnpackPath+dirList[i]).latin1() << endl;

		// oh, no .rc file in current dir, let's go to next dir in list
		if ( tmpSubCnt.count() == 0 )
			continue;

		src = KURL::encode_string(tmpUnpackPath+dirList[i]);
		dst = KURL::encode_string(locateLocal("data","noatun/skins/kjofol/")); // destination to copy skindir into

		if ( dirList[i] == "." ) // zip did not contain a subdir, we have to create one
		{
			// skindir is named like the archive without extension (FIXME: extension is not stripped from name)

			int dotPos = srcFile.fileName().findRev('.');
			if ( dotPos > 0 ) // found a dot -> (hopefully) strip the extension
			{
				dst.addPath( srcFile.fileName().left(dotPos) );
			}
			else // we don't seem to have any extension, just append the archivename
			{
				dst.addPath( srcFile.fileName() );
			}

			kdDebug(66666) << "want to create: " << dst.path().latin1() << endl;

			if ( !dst.isValid() )
			{
				KMessageBox::error ( this,
					i18n("Installing new skin failed: Destination path is invalid.\n"
						"Please report a bug to the K-Jöfol maintainer") );
				KIO::del( tmpUnpackPath );
				return;
			}
			KIO::mkdir( dst );
		}

		if ( !src.isValid() || !dst.isValid() )
		{
			KMessageBox::error ( this,
				i18n("Installing new skin failed: Either source or destination path is invalid.\n"
					"Please report a bug to the K-Jöfol maintainer") );
		}
		else
		{
			kdDebug(66666) << "src: " << src.path().latin1() << endl;
			kdDebug(66666) << "dst: " << dst.path().latin1() << endl;
			KIO::Job *job = KIO::copy(src,dst);
			connect ( job, SIGNAL(result(KIO::Job*)), this, SLOT(slotResult(KIO::Job*)) );
			skinInstalled = true;
		}
	} // END iterate trough dirList

	if ( !skinInstalled )
	{
		KMessageBox::sorry ( this, i18n("No new skin has been installed.\nMake sure the archive contains a valid K-Jöfol skin") );
	}
	else
	{
		KMessageBox::information ( this, i18n("The new skin has been successfully installed") );
	}

	KIO::del( tmpUnpackPath );
}


void KJPrefs::removeSelectedSkin( void )
{
	QString question = i18n("Are you sure you want to remove %1?\n"
		"This will delete the files installed by this skin ").
		arg ( mSkinselectorWidget->mSkins->currentText() );

	cfg->setGroup("KJofol-Skins");
	QString loadedSkin = cfg->readEntry("SkinResource", "kjofol");
//	kdDebug(66666) << "loaded Skin Name: " << QFileInfo(loadedSkin).baseName().latin1() << endl;

        int r = KMessageBox::warningContinueCancel ( this, question, i18n("Confirmation"), KStdGuiItem::del() );
	if ( r != KMessageBox::Continue )
		return;

	bool deletingCurrentSkin = ( mSkinselectorWidget->mSkins->currentText() == QFileInfo(loadedSkin).baseName() );

	// Now find the dir to delete !!!

	QString dirToDelete = QString ("");
	QStringList skinLocations = KGlobal::dirs()->findDirs("data", "noatun/skins/kjofol");

	// iterate through all paths where Noatun is searching for kjofol-skins
	for (uint i = 0; i < skinLocations.count(); ++i )
	{
		QStringList skinDirs = QDir(skinLocations[i]).entryList();

		// iterate trough all dirs containing a skin
		for (uint k = 0; k < skinDirs.count(); ++k )
		{
			QDir skinDirCnt = QDir ( skinLocations[i]+skinDirs[k], "*.rc", QDir::Name|QDir::IgnoreCase, QDir::Files );
			 // make a list of all .rc-files in a skindir
			QStringList rcFiles = skinDirCnt.entryList();

			// iterate trough all those rc.-files in a skindir
			for (uint j = 0; j < rcFiles.count(); j++ )
			{
				if ( rcFiles[j].left(rcFiles[j].length()-3) == mSkinselectorWidget->mSkins->currentText() ) // found skinname.rc :)
				{
					dirToDelete = QString ( skinLocations[i]+skinDirs[k] );
					kdDebug(66666) << "FOUND SKIN @ " << dirToDelete.latin1() << endl;
				}
			}
		}
	}

	if ( dirToDelete.length() != 0 )
	{
		kdDebug(66666) << "Deleting Skindir: " << dirToDelete.latin1() << endl;
		KIO::Job *job = KIO::del( dirToDelete, false, true );
		connect ( job, SIGNAL(result(KIO::Job*)), this, SLOT(slotResult(KIO::Job*)) );
	}

	int item = -1;
	// Fallback to kjofol-skin (the default one) if we've deleted the current skin
	if ( deletingCurrentSkin )
	{
		for ( int i = 0; i < mSkinselectorWidget->mSkins->count(); i++ )
		{ // FIXME: no check wether "kjofol" is ever found, well, it HAS to be in the list
			if ( mSkinselectorWidget->mSkins->text(i) == "kjofol" )
				item = i;
		}
	}
	else
		item = mSkinselectorWidget->mSkins->currentItem();

	if ( item != -1 )
		mSkinselectorWidget->mSkins->setCurrentItem( item );

	// update configuration
	if ( deletingCurrentSkin )
		save();
}

void KJPrefs::slotResult(KIO::Job *job )
{
	if ( job->error() )
	{
		job->showErrorDialog(this);
	}
	else
	{
		// Reload Skinlist
		reopen();
	}
}


/* =================================================================================== */


// takes name of rc-file without .rc at the end and returns full path to rc-file
static QString expand(QString s)
{
//	kdDebug(66666) << "expand( "<< s.latin1() << " )" << endl;

	QStringList skinLocations = KGlobal::dirs()->findDirs("data", "noatun/skins/kjofol");

	// iterate through all paths where Noatun is searching for kjofol-skins
	for (uint i = 0; i < skinLocations.count(); ++i )
	{
		QStringList skinDirs = QDir(skinLocations[i]).entryList();

		// iterate trough all dirs containing a skin
		for (uint k = 0; k < skinDirs.count(); ++k )
		{
			QDir skinDirCnt = QDir ( skinLocations[i]+skinDirs[k], "*.rc", QDir::Name|QDir::IgnoreCase, QDir::Files );
			// make a list of all .rc-files in a skindir
			QStringList rcFiles = skinDirCnt.entryList();

			// iterate trough all those rc.-files in a skindir
			for (uint j = 0; j < rcFiles.count(); j++ )
			{
				if ( rcFiles[j].left(rcFiles[j].length()-3) == s ) // found $s.rc :)
				{
//					kdDebug(66666) << "expand() found: " << QString(skinLocations[i]+skinDirs[k]+"/"+rcFiles[j]).latin1() << endl;
					return (skinLocations[i]+skinDirs[k]+"/"+rcFiles[j]);
				}
			}
		}
	}
	return QString();
}

QString filenameNoCase(const QString &filename, int badNodes)
{
	QStringList names=QStringList::split('/', filename);
	QString full;
	int number=(int)names.count();
	for (QStringList::Iterator i=names.begin(); i!=names.end(); ++i)
	{
		full+="/";
		if (number<=badNodes)
		{
			QDir d(full);
			QStringList files=d.entryList();
			files=files.grep(QRegExp("^"+ (*i) + "$", false));
			if (!files.count())
				return "";
			*i=files.grep(*i, false)[0];
		}

		full+=*i;

		number--;
	}

	if (filename.right(1)=="/")
		full+="/";
	return full;
}
