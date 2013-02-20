/***************************************************************************
                          kgallerydialog.cpp  -  description
                             -------------------
    begin                : mar abr 1 2003
    copyright            : (C) 2003 by Javier Campos
    email                : javi@asyris.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kgallerydialog.h"
#include "kgallerydialog.moc"

#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kio/netaccess.h>

#include <qlineedit.h>
#include <qlistview.h>
#include <qsplitter.h>
#include <qdom.h>
#include <qfileinfo.h>

KGalleryDialog::KGalleryDialog(QWidget *parent, const char *name )
      :KGalleryDialogBase(parent,name)
{
  configRead();
}

KGalleryDialog::~KGalleryDialog()
{
}

/** Open keduca file. This function can open a remote or local url. */
bool KGalleryDialog::openFile(const KURL &url) {
    QString tmpFile;
    bool returnval=false;
    if( KIO::NetAccess::download( url, tmpFile, this ) )
    {
        returnval=loadFile( tmpFile );
        if (returnval) {
            _currentURL = url;
            kdDebug()<<"... load successful: "<<_currentURL.url()<<endl;
        }

        KIO::NetAccess::removeTempFile( tmpFile );
    }else
        kdDebug()<<"FileRead::openFile(): download NOT successful: "<<url.url()<<endl;

    return returnval;
}

bool KGalleryDialog::loadFile( const QString &filename )
{
    QDomDocument doc("document.xml");

    QFile file(filename);

    if(!file.open(IO_ReadOnly))
    {
        return false;
    }
    doc.setContent( &file );

    if( doc.doctype().name() != "educagallery" )
    { return false; }
           
    QDomElement docElem = doc.documentElement();
    QDomNode n = docElem.firstChild();

    kdDebug()<<"... load attributes: "<< docElem.tagName() <<endl;
    
    QDomNodeList dnList = n.childNodes();
    for( unsigned int i = 0; i < dnList.count(); ++i)
    {
        QListViewItem *newItem = new QListViewItem(listDocuments);
        QDomElement serversNode = dnList.item(i).toElement();

        kdDebug()<<"... load attributes: "<< serversNode.text() <<endl;
        
        newItem->setText( 0, serversNode.  text() );
        newItem->setText( 1, serversNode.attribute( "language" ) );
        newItem->setText( 2, serversNode.attribute( "category" ) );
        newItem->setText( 3, serversNode.attribute( "type" ) );
        newItem->setText( 4, serversNode.attribute( "author" ) );
        newItem->setText( 5, serversNode.attribute( "address" ) );        
        newItem->setSelected(false);
    }

    file.close();

    return true;
}

/** Read servers */
void KGalleryDialog::configRead()
{
    KConfig *appconfig = KGlobal::config();
    QStringList servers;
    QStringList ipservers;
    QStringList::Iterator it_ipservers;
        
    appconfig->setGroup( "kgallerydialog" );
    _split->setSizes( appconfig->readIntListEntry("Splitter_size") );
    QSize defaultSize(500,400);
    resize( appconfig->readSizeEntry("Geometry", &defaultSize ) );
       
    appconfig->setGroup("Galleries Servers");
    servers    = appconfig->readListEntry ( "Servers" );
    ipservers  = appconfig->readListEntry ( "ServersIP" );

    if( ipservers.count() == 0 )
      {
      servers.append( "KEduca Main Server" );
      ipservers.append( "http://keduca.sourceforge.net/gallery/gallery.edugallery" );
      }

    it_ipservers=ipservers.begin();
    for ( QStringList::Iterator it_servers = servers.begin(); it_servers != servers.end(); ++it_servers )
    {
      QListViewItem *newItem = new QListViewItem(listServers);
      newItem->setText(0,*it_servers);
      newItem->setText(1,*it_ipservers);
      newItem->setSelected(false);
      ++it_ipservers;
    }
}

/** Write servers lists */
void KGalleryDialog::configWrite()
{
    QStringList servers;
    QStringList ipservers;
    KConfig *config = KGlobal::config();

    config->setGroup( "kgallerydialog" );
    config->writeEntry("Splitter_size", _split->sizes() );
    config->writeEntry("Geometry", size() );
    config->sync();

    config->setGroup("Galleries Servers");
    QListViewItem *item = listServers->firstChild();
    while (item) {
        servers.append( item->text(0) );
        ipservers.append( item->text(1) );
        item = item->nextSibling();
    }
    config->writeEntry("Servers", servers);
    config->writeEntry("ServersIP", ipservers);

    config->sync();
}

/** No descriptions */
void KGalleryDialog::slotButtonAdd()
{
      if( (lineName->text()).isEmpty() || (lineAddress->text()).isEmpty() )
        {
        KMessageBox::sorry(this, i18n("You need to specify a server!"));
        return;
        }
        
        QListViewItem *newItem = new QListViewItem(listServers);
        newItem->setText(0,lineName->text());
        newItem->setText(1,lineAddress->text());
        newItem->setSelected(false);              
}

/** Open selected document */
void KGalleryDialog::accept()
{
    if (getURL().isEmpty())
      KMessageBox::sorry(this, i18n("You need to specify the file to open!"));
    else
    {
      configWrite();
      KGalleryDialogBase::accept();
    }    
//    done( QDialog::Accepted );
}

/** Select Server */
void KGalleryDialog::slotServerSelected( QListViewItem *item )
{
    if (!item)
        return;

    KURL url_server = KURL( item->text(1) );

    listDocuments->clear();
    openFile( url_server );
}

/** Get keduca test url */
KURL KGalleryDialog::getURL()
{
  QListViewItem *item = listDocuments->currentItem();

  if (!item)
    return KURL();

  QString urlAddress = item->text(5);
    
  if( _currentURL.isLocalFile() && !KURL(urlAddress).isValid() )
    {
    if( !QFileInfo(urlAddress).exists() )
      urlAddress = _currentURL.directory(false,true) + urlAddress;
    kdDebug()<< "Imagen en local" <<endl;
    } else if( !_currentURL.isLocalFile() && !KURL(urlAddress).isValid() )
       urlAddress = _currentURL.protocol() + "://" + _currentURL.host() + _currentURL.directory(false,true) + urlAddress;
    
  kdDebug()<< urlAddress <<endl;
  return KURL( urlAddress );
}

/** Add url */
void KGalleryDialog::putURL(const KURL &urlFile)
{
        QListViewItem *newItem = new QListViewItem(listServers);
        newItem->setText( 1, urlFile.url() );
        newItem->setSelected(false);
}
