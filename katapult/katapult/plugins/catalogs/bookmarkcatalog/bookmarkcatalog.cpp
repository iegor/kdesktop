/***************************************************************************
 *   Copyright (C) 2005 by Joe Ferris                                      *
 *   jferris@optimistictech.com                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <kservicegroup.h>
#include <ksycocaentry.h>
#include <ksycocatype.h>
#include <kapplication.h>
#include <kbookmarkmanager.h>
#include <kbookmark.h>
#include <knuminput.h>
#include <kurlrequester.h>

#include <qstring.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qdir.h>
#include <qregexp.h>
#include <qfile.h>

#include "bookmarkcatalog.h"
#include "bookmark.h"
#include "mozillabookmark.h"
#include "settings.h"
#include "actionregistry.h"
#include "actionopenbookmark.h"

K_EXPORT_COMPONENT_FACTORY( katapult_bookmarkcatalog,
                            KGenericFactory<BookmarkCatalog>( "katapult_bookmarkcatalog" ) )

BookmarkCatalog::BookmarkCatalog(QObject *, const char *, const QStringList&)
 : CachedCatalog()
{
	manager = KBookmarkManager::userBookmarksManager();
	_minQueryLen = 1;
	ActionRegistry::self()->registerAction(new ActionOpenBookmark());
}


BookmarkCatalog::~BookmarkCatalog()
{
}

void BookmarkCatalog::initialize()
{
	if(manager != 0)
		cacheBookmarkList(manager->root());
	if(_mozEnabled)
		cacheMozillaBookmarks();
}

void BookmarkCatalog::cacheBookmarkList(KBookmarkGroup group)
{
	KBookmark bookmark = group.first();
	while(!bookmark.isNull()) {
		if (bookmark.isGroup()) {
			cacheBookmarkList(bookmark.toGroup());
		} else {
			addItem(new Bookmark(bookmark));
		}
		bookmark = group.next(bookmark);
	}
}

void BookmarkCatalog::cacheMozillaBookmarks()
{
	if(_mozAuto)
		_mozFile = detectMozillaFile();
		
	if(_mozFile.isEmpty())
		return;
		
	QFile bmFile(_mozFile);
	if(!bmFile.open(IO_ReadOnly))
		return;
		
	QString contents = bmFile.readAll();
	QRegExp rx("<A HREF=\"([^\"]+)\" [^>]+>([^<]+)</A>");
	int pos = 0;
	while(pos > -1) {
		pos = rx.search(contents, pos);
		if(pos > -1) {
			addItem(new MozillaBookmark(rx.cap(1), rx.cap(2), QPixmap()));
			pos += rx.matchedLength();
		}
	}
}

QString BookmarkCatalog::detectMozillaFile()
{
	QStringList testDirs;
	testDirs << ".firefox" << ".mozilla" << ".phoenix" << ".netscape";
	QDir home = QDir::home();
	for(QStringList::Iterator it = testDirs.begin(); it != testDirs.end(); ++it) {
		QString testDir = *it;
		if(home.exists(testDir)) {
			QDir mozDir = QDir(home.path()+"/"+testDir).canonicalPath();
			if(mozDir.dirName() != testDir && testDirs.contains(mozDir.dirName()))
				continue;
			QString path = searchMozDir(mozDir.path());
			if(!path.isEmpty())
				return path;
		}
	}
	
	return "";
}

QString BookmarkCatalog::searchMozDir(QString path)
{
	QDir dir(path);
	if(dir.exists("bookmarks.html")) {
		return path+"/bookmarks.html";
	}
	QStringList entries = dir.entryList(QDir::Dirs | QDir::NoSymLinks);
	for(QStringList::Iterator it = entries.begin(); it != entries.end(); ++it) {
		if(*it != "." && *it != ".."){
			QString result = searchMozDir(path + "/" + *it);
			if(!result.isEmpty())
				return result;
		}
	}
	return QString::null;
}

unsigned int BookmarkCatalog::minQueryLen() const
{
	return _minQueryLen;
}

void BookmarkCatalog::readSettings(KConfigBase *config)
{
	_minQueryLen = config->readUnsignedNumEntry("MinQueryLen", 3);
	_mozEnabled = config->readBoolEntry("MozEnabled", TRUE);
	_mozAuto = config->readBoolEntry("MozAuto", TRUE);
	_mozFile = config->readEntry("MozFile", "");
}

void BookmarkCatalog::writeSettings(KConfigBase *config)
{
	config->writeEntry("MinQueryLen", _minQueryLen);
	config->writeEntry("MozEnabled", _mozEnabled);
	config->writeEntry("MozAuto", _mozAuto);
	config->writeEntry("MozFile", _mozFile);
}

QWidget * BookmarkCatalog::configure()
{
	settings = new BookmarkCatalogSettings();
	
	settings->minQueryLen->setValue(_minQueryLen);
	connect(settings->minQueryLen, SIGNAL(valueChanged(int)), this, SLOT(minQueryLenChanged(int)));
	
	settings->mozEnabled->setChecked(_mozEnabled);
	connect(settings->mozEnabled, SIGNAL(toggled(bool)), this, SLOT(toggleMozEnabled(bool)));
	
	settings->mozAuto->setChecked(_mozAuto);
	connect(settings->mozAuto, SIGNAL(toggled(bool)), this, SLOT(toggleMozAuto(bool)));
	
	settings->mozManual->setChecked(!_mozAuto);
	
	settings->mozFile->setURL(_mozFile);
	connect(settings->mozFile, SIGNAL(urlSelected(const QString &)), this, SLOT(changeMozFile(const QString &)));
	
	settings->mozAuto->setEnabled(_mozEnabled);
	settings->mozManual->setEnabled(_mozEnabled);
	settings->mozFile->setEnabled(_mozEnabled && !_mozAuto);
	
	return settings;
}

void BookmarkCatalog::minQueryLenChanged(int _minQueryLen)
{
	this->_minQueryLen = _minQueryLen;
}

void BookmarkCatalog::toggleMozEnabled(bool _mozEnabled)
{
	this->_mozEnabled = _mozEnabled;
	settings->mozAuto->setEnabled(_mozEnabled);
	settings->mozManual->setEnabled(_mozEnabled);
	settings->mozFile->setEnabled(_mozEnabled && !_mozAuto);
}

void BookmarkCatalog::toggleMozAuto(bool _mozAuto)
{
	this->_mozAuto = _mozAuto;
	settings->mozFile->setEnabled(!_mozAuto);
}

void BookmarkCatalog::changeMozFile(const QString & _mozFile)
{
	this->_mozFile = _mozFile;
}

#include "bookmarkcatalog.moc"
