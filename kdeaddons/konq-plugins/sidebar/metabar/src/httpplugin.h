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
 
#ifndef _HTTP_PLUGIN_H_
#define _HTTP_PLUGIN_H_

#include "protocolplugin.h"

#include <kdirwatch.h>
#include <kbookmarkmanager.h>

#include <dom_string.h>

class HTTPPlugin : public ProtocolPlugin
{
  Q_OBJECT

  public:
    HTTPPlugin(KHTMLPart* html, MetabarFunctions *functions, const char *name = 0);
    ~HTTPPlugin();
    
    bool handleRequest(const KURL &url);
    void deactivate();
    
  protected:
    void loadActions(DOM::HTMLElement node);
    void loadApplications(DOM::HTMLElement node);
    void loadPreview(DOM::HTMLElement node);
    void loadBookmarks(DOM::HTMLElement node);
    void loadInformation(DOM::HTMLElement node);
    void killJobs();
};

#endif
