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

#include "kuick_plugin.h"
#include "kuick_plugin.moc"
#include "kdirmenu.h"

#include <kapplication.h>
#include <kaction.h>
#include <kinstance.h>
#include <kiconloader.h>
#include <kimproxy.h>
#include <klocale.h>
#include <konq_popupmenu.h>
#include <kmessagebox.h>
#include <kgenericfactory.h>
#include <qobject.h>
#include <kio/jobclasses.h>
#include <kio/job.h>
#include <kurl.h>

typedef KGenericFactory<KTestMenu, KonqPopupMenu> KTestMenuFactory;
K_EXPORT_COMPONENT_FACTORY( libkuickplugin, KTestMenuFactory("kuick_plugin") )

KTestMenu::KTestMenu( KonqPopupMenu *popupmenu, const char *name, const QStringList& /*list*/ ) : KonqPopupMenuPlugin( popupmenu, name) {
    popup= popupmenu ;
    meta_copy_mmu = 0L;
    meta_move_mmu = 0L;
    my_action = new KAction( "kuick_plugin", 0, this, SLOT( slotPopupMaeh( ) ), actionCollection( ), "Do some funky stuff"  );
    addAction( my_action );
    addSeparator();
    //popupmenu->addMerge();
    connect( popup, SIGNAL(aboutToShow() ), this, SLOT(slotPrepareMenu( ) ) );
    m_imProxy = KIMProxy::instance( kapp->dcopClient() );
}
KTestMenu::~KTestMenu( ){
     delete meta_copy_mmu;
     delete meta_move_mmu;
}
void KTestMenu::slotPopupMaeh( ){

}
void KTestMenu::slotStartCopyJob( const QString &path ) {
     KURL url = KURL::fromPathOrURL( path );
     KIO::CopyJob *copy;
     copy = KIO::copy( popup->popupURLList(), url);
     copy->setAutoErrorHandlingEnabled( true );
}
void KTestMenu::slotStartMoveJob( const QString &path) {
     KURL url = KURL::fromPathOrURL( path );
     KIO::CopyJob *move;
     move = KIO::move( popup->popupURLList(), url );
     move->setAutoErrorHandlingEnabled( true );
}

void KTestMenu::slotFileTransfer( const QString &uid ) {
    m_imProxy->sendFile( uid, popup->popupURLList().first() );
}

void KTestMenu::slotPrepareMenu( ) { // now it's time to set up the menu...
// search for the dummy entry 'kuick_plugin' stores it index reomev it plug copy at the position
  KGlobal::locale()->insertCatalogue("kuick_plugin");

  bool isKDesktop = QCString(  kapp->name() ) == "kdesktop";

  for(int i= popup->count(); i >=1; i--) {
    int id = popup->idAt( i );
    QString text = popup->text( id );
    if( text.contains("kuick_plugin") ) {
      popup->removeItem( id );
      if (isKDesktop && !kapp->authorize("editable_desktop_icons"))
      {
         // Remove seperator as well
         id = popup->idAt( i-1 );
         if (popup->text( id ).isEmpty())
            popup->removeItem( id );
         break;
      }
      meta_copy_mmu = new KMetaMenu(popup, popup->url(),
              i18n("&Copy Here") , "kuick-copy", m_imProxy );
      popup->insertItem(i18n("Copy To"), meta_copy_mmu, -1, i   );
      connect( meta_copy_mmu, SIGNAL(fileChosen(const QString &) ), 
          SLOT(slotStartCopyJob(const QString & )) );
      
      connect( meta_copy_mmu, SIGNAL( contactChosen( const QString & ) ),
          SLOT( slotFileTransfer( const QString & )) );
          
      if( popup->protocolInfo().supportsMoving() ){
        meta_move_mmu = new KMetaMenu(popup, popup->url(),
              i18n("&Move Here"), "kuick-move");
        popup->insertItem(i18n("Move To"), meta_move_mmu, -1, i+1 );
        connect( meta_move_mmu, SIGNAL(fileChosen(const QString &) ), 
            SLOT(slotStartMoveJob(const QString & )) );
      }
      break;
    }
  }
}
