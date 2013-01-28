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
 
#ifndef _PROTOCOL_PLUGIN_H
#define _PROTOCOL_PLUGIN_H

#include <kfileitem.h>
#include <khtml_part.h>

#include <html_element.h>

#include "metabarfunctions.h"

class ProtocolPlugin : public QObject{
  Q_OBJECT

  public:
    static ProtocolPlugin* activePlugin;
  
    ProtocolPlugin(KHTMLPart *html, MetabarFunctions *functions, const char* name = 0);
    ~ProtocolPlugin();
    
    void setFileItems(const KFileItemList &items);
    bool isActive();
    
    virtual bool handleRequest(const KURL &url) = 0;
    virtual void deactivate() = 0;
    
  protected:
    virtual void killJobs() = 0;
    
    virtual void loadActions(DOM::HTMLElement node) = 0;
    virtual void loadApplications(DOM::HTMLElement node) = 0;
    virtual void loadInformation(DOM::HTMLElement node) = 0;
    virtual void loadPreview(DOM::HTMLElement node) = 0;
    virtual void loadBookmarks(DOM::HTMLElement node) = 0;
  
    KFileItemList m_items;
    KHTMLPart *m_html;
    MetabarFunctions *m_functions;
};

#endif
