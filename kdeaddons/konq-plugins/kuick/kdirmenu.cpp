/* This file is part of the KDE project
   Copyright (C) 2001 Holger Freyther <freyther@yahoo.com>
   Icon stroing inspired by Matthias Elters browser_mnu.* (currently not used)

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; version 2
	 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include <qiconset.h>
#include <qdir.h>
#include <qfileinfo.h>

#include <kaction.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <kio/global.h>
#include <klocale.h>
#include <kurl.h>
#include <ksimpleconfig.h>
#include "kdirmenu.h"

#define CICON(a) (*_icons)[a]

QMap<QString, QPixmap> *KDirMenu::_icons = 0;

KDirMenu::KDirMenu ( QWidget *parent, const KURL &_src,
            const QString &_path, const QString &_name, bool /*showfile*/ )
    : QPopupMenu(parent),
      path(_path),
      name(_name),
      src( _src ),
      action( 0 )
{
    children.setAutoDelete( true );
    initIconMap( );
    connect( this, SIGNAL( aboutToShow( ) ), this, SLOT( slotAboutToShow( ) ) );
    connect( this, SIGNAL( aboutToHide( ) ), this, SLOT( slotAboutToHide( ) ) );
    children.clear(); // just in case

    QFileInfo fileInfo(path);
    if (( src.path() != path || !src.isLocalFile()) && fileInfo.isWritable())
        action = new KAction(name, 0, this, SLOT(new_slot( ) ), this);
}
KDirMenu::~KDirMenu( ) {
    delete action;
    clear( );
    children.clear( );
}
void KDirMenu::insert( KDirMenu *submenu, const QString &_path ) {
    static const QIconSet folder = SmallIconSet("folder");
    QString escapedPath = _path;
    QString completPath=path+'/'+_path;
    // parse .directory if it does exist
    if (QFile::exists(completPath + "/.directory")) {

        KSimpleConfig c(completPath + "/.directory", true);
        c.setDesktopGroup();
        QString iconPath = c.readEntry("Icon");

        if ( iconPath.startsWith("./") )
           iconPath = _path + '/' + iconPath.mid(2);
        QPixmap icon;
        icon = KGlobal::iconLoader()->loadIcon(iconPath,
                                               KIcon::Small, KIcon::SizeSmall,
                                               KIcon::DefaultState, 0, true);
        if(icon.isNull())
           icon = CICON("folder");
        insertItem( icon, escapedPath.replace( "&", "&&" ), submenu );
    }
    else
        insertItem( folder, escapedPath.replace( "&", "&&" ), submenu );
    children.append( submenu );
    connect(submenu, SIGNAL(fileChosen(const QString &)),
            this, SLOT(slotFileSelected(const QString &)));
}

void KDirMenu::slotAboutToShow( ) {

    // ok, prepare the dir: list all dirs and insert the new menus
    if (count() >= 1) return;

    //Precaution: if not a directory, exit, in case some path in KMetaMenu
    //isn't checked right
    if ( !QFileInfo(path).isDir() )
        return;

    if ( action )
        action->plug( this );
    else
	setItemEnabled( insertItem( name ), false );

    // all dirs writeable and readable
    QDir dir(path, QString::null,
             QDir::Name | QDir::DirsFirst | QDir::IgnoreCase,
             QDir::Dirs | QDir::Readable | QDir::Executable);

    const QFileInfoList* dirList = dir.entryInfoList();
    if ( !dirList || dirList->isEmpty() ) {
        if ( action )
            action->setEnabled( false );
	return;
    }

    insertSeparator( );

    if (dirList->count() == 2) {
        insertItem(i18n("No Sub-Folders"), 0);
        setItemEnabled(0, false);
        return;
    }

    static const QString& dot = KGlobal::staticQString( "." );
    static const QString& dotdot = KGlobal::staticQString( ".." );

    for ( QFileInfoListIterator it( *dirList ); *it; ++it ) {
        QString fileName = (*it)->fileName();
        if ( fileName == dot || fileName == dotdot )
            continue;
            
        KURL u;
        u.setPath((*it)->absFilePath());
        if (kapp->authorizeURLAction("list", u, u))
        {
           insert(new KDirMenu(this, src, (*it)->absFilePath(), name),
			KIO::decodeFileName( fileName ));
	}
    }
}
void KDirMenu::slotAboutToHide( ) {

}
void KDirMenu::initIconMap()
{
    if(_icons) return;

//    kdDebug(90160) << "PanelBrowserMenu::initIconMap" << endl;

    _icons = new QMap<QString, QPixmap>;

    _icons->insert("folder", SmallIcon("folder"));
    _icons->insert("unknown", SmallIcon("mime_empty"));
    _icons->insert("folder_open", SmallIcon("folder_open"));
    _icons->insert("kdisknav", SmallIcon("kdisknav"));
    _icons->insert("kfm", SmallIcon("kfm"));
    _icons->insert("terminal", SmallIcon("terminal"));
    _icons->insert("txt", SmallIcon("txt"));
    _icons->insert("exec", SmallIcon("exec"));
    _icons->insert("chardevice", SmallIcon("chardevice"));
}
void KDirMenu::slotFileSelected(const QString &_path ){
    emit fileChosen( _path );
}

void KDirMenu::new_slot() {
    emit fileChosen(path );
}

#include "kdirmenu.moc"
