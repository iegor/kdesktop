/***************************************************************************
 *   Copyright (C) 2005 by Florian Roth   *
 *   florian@synatic.net   *
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
 
#include "protocolplugin.h"

#include <qbuffer.h>

#include <kimageio.h>
#include <kmdcodec.h>
#include <kiconloader.h>
#include <klocale.h>

#include <html_document.h>
#include <html_image.h>

ProtocolPlugin* ProtocolPlugin::activePlugin = 0;

ProtocolPlugin::ProtocolPlugin(KHTMLPart *html, MetabarFunctions *functions, const char* name) : QObject(html, name), m_html(html), m_functions(functions)
{
}

ProtocolPlugin::~ProtocolPlugin()
{
}
    
void ProtocolPlugin::setFileItems(const KFileItemList &items)
{
  m_items = items;
  
  killJobs();
  
  DOM::HTMLDocument doc = m_html->htmlDocument();
  DOM::HTMLElement actions = doc.getElementById("actions");
  DOM::HTMLElement applications = doc.getElementById("open");
  DOM::HTMLElement info = doc.getElementById("info");
  DOM::HTMLElement preview = doc.getElementById("preview");
  DOM::HTMLElement bookmarks = doc.getElementById("bookmarks");
  
  DOM::HTMLImageElement icon = (DOM::HTMLImageElement) doc.getElementById("icon");
  DOM::HTMLElement name = doc.getElementById("name");
  DOM::HTMLElement type = doc.getElementById("type");
  DOM::HTMLElement size = doc.getElementById("size");
  
  if(!icon.isNull()){
    QPixmap pix;
    if(m_items.count() == 1){
      pix = m_items.getFirst()->pixmap(KIcon::SizeLarge);
    }
    else{
      pix = DesktopIcon("kmultiple", KIcon::SizeLarge);
    }
  
    QByteArray data;
    QBuffer buffer(data);
    buffer.open(IO_WriteOnly);
    pix.save(&buffer, "PNG");
    QString icondata = QString::fromLatin1("data:image/png;base64,%1").arg(KCodecs::base64Encode(data));
    
    icon.setSrc(icondata);
  }
  
  if(!name.isNull()){
    if(m_items.count() == 1){
      name.setInnerText(m_items.getFirst()->name());
    }
    else{
      name.setInnerText(i18n("%1 Elements").arg(m_items.count()));
    }
  }
  
  if(!type.isNull()){
    if(m_items.count() == 1){
      KFileItem *item = m_items.getFirst();
      KFileItem *that = const_cast<KFileItem *>(item);
  
      type.setInnerText(that->determineMimeType()->comment());
    }
    else{
      int files = 0;
      int dirs  = 0;
    
      for(KFileItemListIterator it(m_items); it.current(); ++it){        
        if((*it)->isDir()){
          dirs++;
        }
        else{
          files++;
        }
      }

      type.setInnerText(i18n("%1 Folders, %2 Files").arg(dirs).arg(files));
    }

    
  }

  if(!size.isNull()){
    KIO::filesize_t s = 0;
    
    for(KFileItemListIterator it(m_items); it.current(); ++it){
      s += (*it)->size();
    }

    size.setInnerText(KIO::convertSize(s));
  }
  
  if(!actions.isNull()){
    loadActions(actions);
    m_functions->adjustSize("actions");
  }
    
  if(!applications.isNull()){
    loadApplications(applications);
    m_functions->adjustSize("open");
  }
  
  if(!info.isNull()){
    loadInformation(info);
    m_functions->adjustSize("info");
  }
  
  if(!preview.isNull()){
    loadPreview(preview);
    m_functions->adjustSize("preview");
  }
  
  if(!bookmarks.isNull()){
    loadBookmarks(bookmarks);
    m_functions->adjustSize("bookmarks");
  }
  
  doc.updateRendering();
}

bool ProtocolPlugin::isActive()
{
  return activePlugin == this;
}

#include "protocolplugin.moc"
