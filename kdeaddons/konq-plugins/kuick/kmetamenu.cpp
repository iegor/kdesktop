/* This file is part of the KDE project
   Copyright (C) 2001 Holger Freyther <freyther@yahoo.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; version 2
   of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <kiconloader.h>
#include <kimproxy.h>
#include <kapplication.h>
#include <klocale.h>
#include <kconfig.h>
#include <kfiledialog.h>
#include <kurl.h>
#include <konq_popupmenu.h>

#include <qpixmap.h>
#include <qdir.h>
#include <qiconset.h>
#include <qstringlist.h>

#include "kmetamenu.h"
#include "kdirmenu.h"
#include "kimcontactmenu.h"
#include "kmetamenu.moc"

KMetaMenu::KMetaMenu( QWidget *parent, const KURL &url,
        const QString &text, const QString &key, KIMProxy *imProxy )
: QPopupMenu( parent),
  m_root( 0 ), m_home( 0 ), m_etc( 0 ), m_current( 0 ), m_browse( 0 ) {
     int recent_no;
     group = key;
     actions.setAutoDelete( TRUE );

     QStringList dirList;

     KURL u;

     u.setPath(QDir::homeDirPath());
     if ( kapp->authorizeURLAction("list", u, u) )
     {
         m_home = new KDirMenu( parent, url, u.path() , text );
         insertItem( SmallIcon( "kfm_home" ), i18n("&Home Folder"), m_home);
         dirList << u.path();

         connect(m_home, SIGNAL(fileChosen(const QString &)),
                 SLOT(slotFileChosen(const QString &) ) );
     }

     u.setPath(QDir::rootDirPath());
     if ( kapp->authorizeURLAction("list", u, u) )
     {
         m_root = new KDirMenu( parent, url, u.path() , text );
         insertItem( SmallIcon( "folder_red" ), i18n("&Root Folder"), m_root);
         dirList << u.path();

         connect(m_root, SIGNAL(fileChosen(const QString &)),
                 SLOT(slotFileChosen(const QString &) ) );
     }

     QString confDir = QDir::rootDirPath()+ "etc";
     u.setPath(confDir);
     if ( QFileInfo( confDir ).isWritable() &&
          kapp->authorizeURLAction("list", u, u) )
     {
         m_etc  = new KDirMenu( parent, url, confDir, text );
         insertItem( SmallIcon( "folder_yellow" ) ,
                 i18n("&System Configuration"), m_etc);
         dirList << confDir;

         connect(m_etc , SIGNAL(fileChosen(const QString &)),
                 SLOT(slotFileChosen(const QString &) ) );
     }

     if ( url.isLocalFile()
             && dirList.find( url.path() ) == dirList.end()
             && QFileInfo( url.path() ).isWritable()
             && QFileInfo( url.path() ).isDir() 
             && kapp->authorizeURLAction("list", url, url) )
             //Need to check whether a directory so we don't crash trying to access it
             //(#60192)
     {
         // Also add current working directory
         m_current = new KDirMenu( parent, url, url.path(), text );
         insertItem( SmallIcon( "folder" ), i18n( "&Current Folder" ),
                 m_current );

         connect(m_current, SIGNAL(fileChosen(const QString &)),
                 SLOT(slotFileChosen(const QString &) ) );
     }

    if ( imProxy )
    {
        m_contacts = new KIMContactMenu( parent, imProxy );
        int item = insertItem( SmallIconSet( "personal" ), i18n( "C&ontact" ), m_contacts );
        connect ( m_contacts, SIGNAL( contactChosen( const QString &) ), SIGNAL( contactChosen( const QString & ) ) );
        if ( !imProxy->initialize() || imProxy->fileTransferContacts().isEmpty() )
          setItemEnabled( item, false );
    }


     m_browse = new KAction(i18n("&Browse..."), 0, this, SLOT(slotBrowse()), this );
     m_browse->plug(this);
     // read the last chosen dirs
     // first set the group according to our parameter
     conf = kapp->config( );
     conf->setGroup(key );
     recent_no = conf->readNumEntry("ShowRecent", 5);
     list = conf->readPathListEntry("Paths");
     if ( list.count() > 0 )
         insertSeparator();
     int i=1;
     QStringList::Iterator it = list.begin();
     while( it != list.end() ) {
         if( i == (recent_no + 1) )
	     break;
         QDir dir( *it );
         u.setPath( *it );
         if ( !dir.exists() || !kapp->authorizeURLAction("list", u, u) ) {
             it = list.remove( it );
             continue;
         }
         QString escapedDir = *it;
         KAction *action = new KAction(escapedDir.replace("&", "&&"), 0, this, SLOT(slotFastPath()), this);
         action->plug(this );
         actions.append( action );
         ++it;
         i++;
     }
}

KMetaMenu::KMetaMenu( ){

}
KMetaMenu::~KMetaMenu(){
     delete m_root;
     delete m_home;
     delete m_etc;
     delete m_current;
     delete m_browse;
     actions.clear();
}
void KMetaMenu::slotFileChosen(const QString &path ){
     writeConfig(path );
     emit fileChosen(path );
}

void KMetaMenu::slotFastPath( ) {
     KAction *action;
     action = (KAction*) sender();
     QString text = action->plainText( );
     slotFileChosen( text );
}
void KMetaMenu::writeConfig( const QString &path){
     list.remove(path );
     list.prepend(path );
     conf->setGroup( group );
     int c = conf->readNumEntry( "ShowRecent", 5 );
     while ( list.count() > c && !list.isEmpty() )
        list.remove(list.last());
     conf->writePathEntry("Paths", list);
     conf->sync( );
}
void KMetaMenu::slotBrowse() {
     KURL dest = KFileDialog::getExistingURL();
     if( dest.isEmpty() ) return;
     slotFileChosen( dest.isLocalFile() ? dest.path() : dest.url() );
}
