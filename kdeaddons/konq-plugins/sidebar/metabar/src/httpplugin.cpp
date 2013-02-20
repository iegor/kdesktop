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
 
#include "metabarwidget.h"
#include "httpplugin.h"

#include <kbookmark.h>
#include <kstandarddirs.h>
#include <kapplication.h>
#include <kicontheme.h>
#include <khtmlview.h>
#include <kurl.h>
#include <klocale.h>

#include <dcopref.h>
#include <dcopclient.h>

#include <qregexp.h>
#include <qfile.h>

#include <dom_node.h>
#include <html_inline.h>

HTTPPlugin::HTTPPlugin(KHTMLPart* html, MetabarFunctions *functions, const char *name) : ProtocolPlugin (html, functions, name)
{
}

HTTPPlugin::~HTTPPlugin()
{
  //delete bookmarkManager;
}

void HTTPPlugin::deactivate()
{  
  m_functions->hide("actions");
  m_functions->hide("info");
}

void HTTPPlugin::killJobs()
{
}

void HTTPPlugin::loadInformation(DOM::HTMLElement node)
{  
  /*DOM::DOMString innerHTML;
  innerHTML += "<form action=\"find:///\" method=\"GET\">";
  innerHTML += "<ul>";
  innerHTML += i18n("Keyword");
  innerHTML += " <input onFocus=\"this.value = ''\" type=\"text\" name=\"find\" id=\"find_text\" value=\"";
  innerHTML += i18n("Web search");
  innerHTML += "\"></ul>";
  innerHTML += "<ul><input type=\"submit\" id=\"find_button\" value=\"";
  innerHTML += i18n("Find");
  innerHTML += "\"></ul>";
  innerHTML += "</form>";
  
  node.setInnerHTML(innerHTML);
  m_functions->show("info");*/

  m_functions->hide("info");
}

void HTTPPlugin::loadActions(DOM::HTMLElement node)
{
  m_functions->hide("actions");
}

void HTTPPlugin::loadApplications(DOM::HTMLElement node)
{
   m_functions->hide("open");
}

void HTTPPlugin::loadPreview(DOM::HTMLElement node)
{
   m_functions->hide("preview");
}

void HTTPPlugin::loadBookmarks(DOM::HTMLElement node)
{
   m_functions->hide("bookmarks");
}

bool HTTPPlugin::handleRequest(const KURL &url)
{
  if(url.protocol() == "find"){
    QString keyword = url.queryItem("find");
    QString type = url.queryItem("type");

    if(!keyword.isNull() && !keyword.isEmpty()){
      KURL url("http://www.google.com/search");
      url.addQueryItem("q", keyword);
      
      DCOPRef ref(kapp->dcopClient()->appId(), m_html->view()->topLevelWidget()->name());
      DCOPReply reply = ref.call("openURL", url.url());
    }
    
    return true;
  }
  return false;
}

#include "httpplugin.moc"
