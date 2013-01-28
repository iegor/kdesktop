/* This file is part of the KDE project

   Copyright (C) 2003 Georg Robbers <Georg.Robbers@urz.uni-hd.de>

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

#include "arkplugin.h"

#include <kapplication.h>
#include <kstandarddirs.h>
#include <kmimetype.h>
#include <kdebug.h>
#include <kaction.h>
#include <kinstance.h>
#include <klocale.h>
#include <konq_popupmenu.h>
#include <kpopupmenu.h>
#include <kgenericfactory.h>
#include <kurl.h>
#include <kio/netaccess.h>

#include <qdir.h>
#include <qcstring.h>
#include <qsignalmapper.h>
#include <qobject.h>

typedef KGenericFactory<ArkMenu, KonqPopupMenu> ArkMenuFactory;
K_EXPORT_COMPONENT_FACTORY( libarkplugin, ArkMenuFactory("arkplugin") )

ArkMenu::ArkMenu( KonqPopupMenu * popupmenu, const char *name, const QStringList& /* list */ )
                : KonqPopupMenuPlugin( popupmenu, name),
                m_compAsMapper( 0 ), m_addToMapper( 0 ), m_conf( 0 )
{
    if ( ( QCString( kapp->name() ) == "kdesktop" && !kapp->authorize("editable_desktop_icons" ) )
         || ( KStandardDirs::findExe( "ark" ).isNull() ) )
        return;

    m_conf = new KConfig( "arkrc" );
    m_conf->setGroup( "ark" );

    if ( !m_conf->readBoolEntry( "KonquerorIntegration", true ) )
        return;

    KGlobal::locale()->insertCatalogue("ark_plugin");

    extMimeTypes();
    KFileItemList itemList = popupmenu->fileItemList();
    for ( KFileItem *item = itemList.first(); item; item = itemList.next() )
    {
        m_urlList.append( item->url() );
        m_urlStringList.append( item->url().url() );
    }
    m_dir = popupmenu->url().url() + "/";
    unsigned int itemCount = m_urlList.count();
    KFileItemListIterator it( itemList );
    KFileItem * item;
    bool hasArchives = false;
    bool hasOther = false;
    while ( ( item = it.current() ) != 0 )
    {
        ++it;
        if ( m_extractMimeTypes.contains( item->mimetype() ) )
        {
            hasArchives = true;
        }
        else
        {
            hasOther = true;
        }

        if ( hasArchives && hasOther )
            break;
    }

    QString ext;
    KActionMenu * actionMenu;
    KAction * action;
    if ( hasOther &&  itemList.first()->name()!="." && popupmenu->protocolInfo().supportsWriting() )	// don't try to compress if we right click on a folder without files selected
    {
        compMimeTypes();
        actionMenu = new KActionMenu( i18n( "Compress" ), "ark", actionCollection(), "ark_compress_menu"  );
        m_ext = m_conf->readEntry( "LastExtension", ".tar.gz" );
        if ( itemCount == 1 )
        {
            item = itemList.first();
            m_name = itemList.first()->name();
            action = new KAction( i18n( "Compress as %1" ).arg( m_name + m_ext ), 0, this,
                                    SLOT( slotCompressAsDefault() ), actionCollection() );
        }
        else
        {
            action = new KAction( KMimeType::mimeType( m_conf->readEntry(
                                    "LastMimeType", "application/x-tgz" ) )->comment(),
                                    0, this, SLOT( slotCompressAsDefault() ), actionCollection() );
        }
        actionMenu->insert( action );

        m_compAsMenu = new KActionMenu( i18n( "Compress As" ), actionCollection(), "arkcmpasmnu" );
        actionMenu->insert( m_compAsMenu );


        m_addToMenu = new KActionMenu( i18n( "Add To" ), actionCollection(), "arkaddtomnu" );
        if ( itemList.first()->url().isLocalFile() )
            actionMenu->insert( m_addToMenu );

        connect( m_compAsMenu->popupMenu(), SIGNAL( aboutToShow() ),
                this, SLOT( slotPrepareCompAsMenu() ) );
        connect( m_addToMenu->popupMenu(), SIGNAL( aboutToShow() ),
                this, SLOT( slotPrepareAddToMenu() ) );


        action = new KAction( i18n( "Add to Archive..." ), 0, this,
                                SLOT( slotAdd() ), actionCollection() );
        actionMenu->insert( action );
        addAction( actionMenu );
    }

    if ( !hasOther && hasArchives )
    {
        if ( popupmenu->protocolInfo().supportsWriting() )
        {
            actionMenu = new KActionMenu( i18n( "Extract" ), "ark", actionCollection(), "ark_extract_menu"  );

            action = new KAction( i18n( "Extract Here" ), 0, this,
                                    SLOT( slotExtractHere() ), actionCollection() );
            actionMenu->insert( action );
            // stolen from arkwidget.cpp
            if ( itemCount == 1 )
            {
                QString targetName = itemList.first()->name();
                stripExtension( targetName );
                action = new KAction( i18n( "Extract to %1" ).arg( targetName ), 0, this,
                                        SLOT( slotExtractToSubfolders() ), actionCollection() );
            }
            else
            {
                action = new KAction( i18n( "Extract to Subfolders" ), 0, this,
                                        SLOT( slotExtractToSubfolders() ), actionCollection() );
            }
            actionMenu->insert( action );
            action = new KAction( i18n( "Extract To..." ), 0 , this,
                                    SLOT( slotExtractTo() ), actionCollection() );
            actionMenu->insert( action );
            addAction( actionMenu );
        }
        else
        {
           action = new KAction( i18n( "Extract To..." ), "ark", 0,  this, SLOT( slotExtractTo() ), actionCollection(), "ark_extract_menu" );
           addAction( action );
        }
    }
    addSeparator();
}

ArkMenu::~ArkMenu()
{
    delete m_conf;
}

void ArkMenu::slotPrepareCompAsMenu()
{
    disconnect( m_compAsMenu->popupMenu(), SIGNAL( aboutToShow() ),
                this, SLOT( slotPrepareCompAsMenu() ) );

    KAction * action;
    m_compAsMapper = new QSignalMapper( this, "compAsMapper" );
    QString ext;
    QStringList newExt;
    unsigned int counter = 0;
    QCString actionName;
    QStringList::Iterator eit;
    QStringList::Iterator mit;
    mit = m_archiveMimeTypes.begin();
    for ( ; mit != m_archiveMimeTypes.end(); ++mit )
    {
        newExt = KMimeType::mimeType(*mit)->patterns();
        eit = newExt.begin();
        (*eit).remove( '*' );
        if ( *eit == ".tar.bz" )                   // tbz mimetype, has tar.bz as first entry :}
            *eit = ".tar.bz2";
        if ( m_urlList.count() == 1 )
        {
            action = new KAction( m_name + (*eit), 0, m_compAsMapper,
                    SLOT( map() ), actionCollection() );
        }
        else
        {
            ext = KMimeType::mimeType(*mit)->comment();
            action = new KAction( ext, 0, m_compAsMapper,
                    SLOT( map() ), actionCollection() );
        }

        m_compAsMenu->insert( action );
        m_compAsMapper->setMapping( action, counter );

        ++counter;
        ++eit;
        while( eit != newExt.end() )
        {
            (*eit).remove( '*' );
            ++eit;
            ++counter;
        }
        m_extensionList += newExt;
    }

    connect( m_compAsMapper, SIGNAL( mapped( int ) ), SLOT( slotCompressAs( int ) ) );
}

void ArkMenu::slotPrepareAddToMenu()
{
    disconnect( m_addToMenu->popupMenu(), SIGNAL( aboutToShow() ),
                this, SLOT( slotPrepareAddToMenu() ) );


    if ( m_extensionList.isEmpty() ) // is filled in slotPrepareCompAsMenu
        slotPrepareCompAsMenu();

    unsigned int counter = 0;
    KAction * action;
    m_addToMapper = new QSignalMapper( this, "addToMapper" );
    QCString actionName;
    QStringList::Iterator mit;
    KURL archive;
    QDir dir( m_urlList.first().directory() );
    QStringList entries = dir.entryList();
    QStringList::Iterator uit = entries.begin();
    for ( ; uit != entries.end(); ++uit )
    {
        for ( mit = m_extensionList.begin(); mit != m_extensionList.end(); ++mit )
            if ( (*uit).endsWith(*mit) )
            {
                action = new KAction( *uit, 0, m_addToMapper,
                                      SLOT( map() ), actionCollection() );
                m_addToMenu->insert( action );
                m_addToMapper->setMapping( action, counter );
                archive.setPath( *uit );
                m_archiveList << archive;
                counter++;
                break;
            }
    }
    connect( m_addToMapper, SIGNAL( mapped( int ) ), SLOT( slotAddTo( int ) ) );
}

void ArkMenu::compMimeTypes()
{
    unsigned int itemCount = m_urlList.count();

    bool havegz = false;
    if ( !KStandardDirs::findExe( "gzip" ).isNull() && m_conf->readBoolEntry( "UseGz", true ) )
    {
        havegz = true;
        //.gz can only compress one file, not multiple 
        if ( itemCount == 1 ) m_archiveMimeTypes << "application/x-gzip";
    }

    bool havebz2 = false;
    if ( !KStandardDirs::findExe( "bzip2" ).isNull() && m_conf->readBoolEntry( "UseBzip2", true ) )
    {
        havebz2 = true;
        //.bz2 can only compress one file, not multiple
        if ( itemCount == 1 ) m_archiveMimeTypes << "application/x-bzip2";
    }

    bool havelzop = false;
    if ( !KStandardDirs::findExe( "lzop" ).isNull() && m_conf->readBoolEntry( "UseLzop", false ) )
    {
        havelzop = true;
        m_archiveMimeTypes << "application/x-lzop";
    }

    if ( !KStandardDirs::findExe( "tar" ).isNull() && m_conf->readBoolEntry( "UseTar", true ) )
    {
        m_archiveMimeTypes << "application/x-tar";
        if ( havegz )
            m_archiveMimeTypes << "application/x-tgz";
        if ( havebz2 )
        {
            m_archiveMimeTypes << "application/x-tbz";
            m_archiveMimeTypes << "application/x-tbz2";
        }
        if ( havelzop )
            m_archiveMimeTypes << "application/x-tzo";
    }

    if ( !KStandardDirs::findExe( "lha" ).isNull() && m_conf->readBoolEntry( "UseLha", false ) )
        m_archiveMimeTypes << "application/x-lha";

    if ( !KStandardDirs::findExe( "zip" ).isNull() && m_conf->readBoolEntry( "UseZip", true ) )
    {
        m_archiveMimeTypes << "application/x-zip";

        if ( m_conf->readBoolEntry( "UseJar", false ) )
            m_archiveMimeTypes << "application/x-jar";
    }

    if ( !KStandardDirs::findExe( "rar" ).isNull() && m_conf->readBoolEntry( "UseRar", true ) )
        m_archiveMimeTypes << "application/x-rar" << "application/x-rar-compressed";

    if ( !KStandardDirs::findExe( "7z" ).isNull() && m_conf->readBoolEntry( "Use7z", true ) )
        m_archiveMimeTypes << "application/x-7z";
    else if ( !KStandardDirs::findExe( "7za" ).isNull() && m_conf->readBoolEntry( "Use7za", true ) )
        m_archiveMimeTypes << "application/x-7z";
    else if ( !KStandardDirs::findExe( "7zr" ).isNull() && m_conf->readBoolEntry( "Use7zr", true ) )
        m_archiveMimeTypes << "application/x-7z";

    if ( !KStandardDirs::findExe( "zoo" ).isNull() && m_conf->readBoolEntry( "UseZoo", false ) )
        m_archiveMimeTypes << "application/x-zoo";

    if ( !KStandardDirs::findExe( "compress" ).isNull() && m_conf->readBoolEntry( "UseCompress", false ) )
        m_archiveMimeTypes << "application/x-compress";

    if ( !KStandardDirs::findExe( "bzip" ).isNull() && m_conf->readBoolEntry( "UseBzip", false ) )
        m_archiveMimeTypes << "application/x-bzip";

    if ( !KStandardDirs::findExe( "ar" ).isNull() && m_conf->readBoolEntry( "UseAr", false ) )
        m_archiveMimeTypes << "application/x-archive";
}

void ArkMenu::extMimeTypes()
{
    bool havegz = false;
    if ( !KStandardDirs::findExe( "gunzip" ).isNull() )
    {
        havegz = true;
        m_extractMimeTypes << "application/x-gzip";
	m_extractMimeTypes << "application/x-gzpostscript";
    }

    bool havebz2 = false;
    if ( !KStandardDirs::findExe( "bunzip2" ).isNull() )
    {
        havebz2 = true;
        m_extractMimeTypes << "application/x-bzip2";
    }

    bool havelzop = false;
    if ( !KStandardDirs::findExe( "lzop" ).isNull() )
    {
        havelzop = true;
        m_extractMimeTypes << "application/x-lzop";
    }

    if ( !KStandardDirs::findExe( "tar" ).isNull() )
    {
        m_extractMimeTypes << "application/x-tar";
        if ( havegz )
            m_extractMimeTypes << "application/x-tgz";
        if ( havebz2 )
        {
            m_extractMimeTypes << "application/x-tbz";
            m_extractMimeTypes << "application/x-tbz2";
        }
        if ( havelzop )
            m_extractMimeTypes << "application/x-tzo";
    }

    if ( !KStandardDirs::findExe( "lha" ).isNull() )
        m_extractMimeTypes << "application/x-lha";

    if ( !KStandardDirs::findExe( "zip" ).isNull() )
        m_extractMimeTypes << "application/x-zip" << "application/x-jar";

    if ( !KStandardDirs::findExe( "unrar" ).isNull() )
        m_extractMimeTypes << "application/x-rar" << "application/x-rar-compressed";

    if ( !KStandardDirs::findExe( "7z" ).isNull() || !KStandardDirs::findExe( "7za" ).isNull() || !KStandardDirs::findExe( "7zr" ).isNull() )
        m_extractMimeTypes << "application/x-7z";

    if ( !KStandardDirs::findExe( "zoo" ).isNull() )
        m_extractMimeTypes << "application/x-zoo";

    if ( !KStandardDirs::findExe( "uncompress" ).isNull() )
        m_extractMimeTypes << "application/x-compress";

    if ( !KStandardDirs::findExe( "bunzip" ).isNull() )
        m_extractMimeTypes << "application/x-bzip";

    if ( !KStandardDirs::findExe( "ar" ).isNull() )
        m_extractMimeTypes << "application/x-archive";
}

void ArkMenu::stripExtension( QString & name )
{
    QStringList patternList = KMimeType::findByPath( name )->patterns();
    QStringList::Iterator it = patternList.begin();
    QString ext;
    for ( ; it != patternList.end(); ++it )
    {
        ext = (*it).remove( '*' );
        if ( name.endsWith( ext ) )
        {
            name = name.left( name.findRev( ext ) ) + '/';
            break;
        }
    }
}

void ArkMenu::slotCompressAs( int pos )
{
    QCString name;
    QString extension, mimeType;
    KURL target;
    QStringList filelist( m_urlStringList );

    //if KMimeType returns .ZIP, .7Z or .RAR. convert them to lowercase
    if ( m_extensionList[ pos ].contains ( ".ZIP" ) )
         m_extensionList[ pos ] = ".zip";

    if ( m_extensionList[ pos ].contains ( ".RAR" ) )
         m_extensionList[ pos ] = ".rar";

    if ( m_extensionList[ pos ].contains ( ".7Z" ) )
         m_extensionList[ pos ] = ".7z";

    if ( filelist.count() == 1)
         target = filelist.first() + m_extensionList[ pos ];
    else
    {
        target = m_dir + i18n("Archive") + m_extensionList[ pos ];
        int i=1;
        while ( KIO::NetAccess::exists(target, true, 0 ) )
        {
            target = m_dir + i18n("Archive %1").arg(i) + m_extensionList[ pos ];
            i++;
        }
    }
    compressAs( filelist, target );

    extension = m_extensionList[ pos ];
    m_conf->setGroup( "ArkPlugin" );
    m_conf->writeEntry( "LastExtension", extension );

    QStringList extensions;
    QStringList::Iterator eit;
    QStringList::Iterator mit = m_archiveMimeTypes.begin();
    bool done = false;
    for ( ; mit != m_archiveMimeTypes.end() && !done; ++mit )
    {
        extensions = KMimeType::mimeType(*mit)->patterns();
        eit = extensions.begin();
        for ( ; eit != extensions.end(); ++eit )
        {
            (*eit).remove( '*' );
            if ( (*eit) == extension )
            {
                m_conf->writeEntry( "LastMimeType", *mit );
                done = true;
                break;
            }
        }
    }
    m_conf->sync();
}

void ArkMenu::slotCompressAsDefault()
{
    KURL name;

    if ( m_urlStringList.count() == 1)
         name = m_urlStringList.first() + m_ext;
    else
    {
        name = m_dir + i18n("Archive") + m_ext;
        int i=1;
        while ( KIO::NetAccess::exists(name, true, 0 ) )
        {
            name = m_dir + i18n("Archive %1").arg(i) + m_ext;
            i++;
        }
    }
    compressAs( m_urlStringList, name );
}

// make work for URLs
void ArkMenu::compressAs( const QStringList &name, const KURL & compressed )
{
    QStringList args;
    args << "--add-to";
    args += name;
    args << compressed.url();
    kapp->kdeinitExec( "ark", args );
}

void ArkMenu::slotAddTo( int pos )
{
    QStringList args( m_urlStringList );
    args.prepend( "--add-to" );

    KURL archive( m_urlStringList.first() );
    archive.setPath( archive.directory( false ) );
    archive.setFileName( m_archiveList[ pos ].fileName() );

    args << archive.url();
    kapp->kdeinitExec( "ark", args );
}

void ArkMenu::slotAdd()
{
    QStringList args( m_urlStringList );
    args.prepend( "--add" );

    kapp->kdeinitExec( "ark", args );
}

void ArkMenu::slotExtractHere()
{
    for ( QValueList<KURL>::ConstIterator it = m_urlList.constBegin();
          it != m_urlList.constEnd();
          ++it )
    {
        QStringList args;
        KURL targetDirectory = ( *it ).url();
        targetDirectory.setPath( targetDirectory.directory() );
        args << "--extract-to"  << targetDirectory.url() << ( *it ).url();
        kapp->kdeinitExec( "ark", args );
    }
}

void ArkMenu::slotExtractToSubfolders()
{
    for ( QStringList::ConstIterator it = m_urlStringList.constBegin();
          it != m_urlStringList.constEnd();
          ++it )
    {
        KURL targetDir;
        QString dirName;
        QStringList args;

        targetDir = *it;
        dirName = targetDir.path();
        stripExtension( dirName );
        targetDir.setPath( dirName );
        args << "--extract-to"  << targetDir.url() << *it;
        kapp->kdeinitExec( "ark", args );
    }
}

void ArkMenu::slotExtractTo()
{
    for ( QStringList::ConstIterator it = m_urlStringList.constBegin();
          it != m_urlStringList.constEnd();
          ++it )
    {
        QStringList args;
        args << "--extract" << *it;
        kapp->kdeinitExec( "ark", args );
    }
}

#include "arkplugin.moc"
