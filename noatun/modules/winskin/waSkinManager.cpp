#include <kglobal.h>
#include <qfileinfo.h>
#include <qregexp.h>
#include <kstandarddirs.h>
#include <qdir.h>
#include <kdebug.h>
#include <kmimetype.h>
#include <kio/job.h>
#include <kurl.h>
#include <kio/netaccess.h>
#include <kzip.h>

#include "waSkinManager.h"
#include "waSkinModel.h"

WaSkinManager::WaSkinManager() : DCOPObject("WaSkinManager") {
}

WaSkinManager::~WaSkinManager() {
}

QStringList WaSkinManager::availableSkins() {
	QStringList skinDirs = KGlobal::dirs()->findDirs("data", "noatun/skins/winamp");
	QStringList skin_list;
	
	// This loop adds them all to our skin list
	for(unsigned int x = 0;x < skinDirs.count();x++) {
		QDir skinQDir(skinDirs[x]);

		// We only want directories, although there shouldn't be anything else
		skinQDir.setFilter( QDir::Dirs );
		// I guess name is as good as any
		skinQDir.setSorting( QDir::Name );

		for (unsigned int y = 0;y < skinQDir.count();y++) {
			QStringList skins = skinQDir.entryList(QDir::Dirs, QDir::Name);

			// We really don't care for '.' and '..'
			if (skinQDir[y][0] != (char)'.') {
				// Add ourselves to the list, using our directory name
				skin_list += skinQDir[y];
 			}
		}
	}

	return skin_list;
}

QString WaSkinManager::currentSkin() {
	return mCurrentSkin;
}

QString WaSkinManager::defaultSkin() {
	return "Winamp";
}

bool WaSkinManager::loadSkin(QString skinName) {
	QStringList skins = KGlobal::dirs()->findDirs("data", "noatun/skins/winamp/" + skinName);

	if (!skins.count())
		mCurrentSkin = defaultSkin();
	else
		mCurrentSkin = skinName;

	return _waskinmodel_instance->load(skins[0]);
}

bool WaSkinManager::installSkin(QString _url) {
	QString location = KGlobal::dirs()->saveLocation("data", "noatun/skins/winamp");
	KURL url(_url);
	QString mimetype = KMimeType::findByURL(_url)->name();

	if (mimetype == "inode/directory")
	{
		KIO::Job *job = KIO::copy(url, location, !url.isLocalFile());
		connect(job, SIGNAL(result(KIO::Job *)), this, SIGNAL(updateSkinList()));
		return true;
	}
	else if ((mimetype == "interface/x-winamp-skin") || (mimetype == "application/x-zip"))
	{
		if (!url.isLocalFile())
			return false;

		QString base_path;
		base_path = location + "/" + QFileInfo(url.path()).baseName().replace(QRegExp("_"), " ");
		KIO::Job *job = KIO::copy("zip:" + url.path(), base_path);
		connect(job, SIGNAL(result(KIO::Job *)), this, SIGNAL(updateSkinList()));

		return true;
	}

	return false;
}

bool WaSkinManager::removeSkin(QString skinName) {
	if (!skinRemovable(skinName))
		return false;

	QStringList skins = KGlobal::dirs()->findDirs("data", "noatun/skins/winamp/" + skinName);

	KIO::Job *job = KIO::del(KURL(skins[0]), false, false);
	connect(job, SIGNAL(result(KIO::Job *)), this, SIGNAL(updateSkinList()));

	return true;
}

bool WaSkinManager::skinRemovable(QString skinName) {
	QStringList skins = KGlobal::dirs()->findDirs("data", "noatun/skins/winamp/" + skinName);

	if (!skins.count())
		return false;

	QFileInfo info(skins[0]);
	return info.isWritable();
}

QStringList WaSkinManager::skinMimeTypes() {
	QStringList temp;

	temp.append("interface/x-winamp-skin");
	temp.append("application/x-zip");
	temp.append("inode/directory");

	return temp;
}

#include "waSkinManager.moc"
