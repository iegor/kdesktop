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
#include "settingsplugin.h"

#include <kcmoduleinfo.h>
#include <kservice.h>
#include <klocale.h>

#include <html_document.h>
#include <html_element.h>
#include <dom_string.h>

SettingsPlugin::SettingsPlugin(KHTMLPart* html, MetabarFunctions *functions, const char *name) : ProtocolPlugin (html, functions, name)
{
  list_job = 0;
}

SettingsPlugin::~SettingsPlugin()
{
}

void SettingsPlugin::killJobs()
{  
  if(list_job){
    list_job->kill();
    list_job = 0;
  }
}

void SettingsPlugin::deactivate()
{   
  m_functions->hide("actions");
  m_functions->hide("info");
}

void SettingsPlugin::loadActions(DOM::HTMLElement node)
{
  KURL url = m_items.getFirst()->url();
  
  if(url.path().endsWith("/")){
    list_job = KIO::listDir(url, true, false);
    connect(list_job, SIGNAL(entries(KIO::Job *, const KIO::UDSEntryList &)), this, SLOT(slotGotEntries(KIO::Job *, const KIO::UDSEntryList &)));
    connect(list_job, SIGNAL(result(KIO::Job *)), this, SLOT(slotJobFinished(KIO::Job *)));
  
    m_functions->show("actions");
  }
  else{
    QString path = url.path();
    QString name = url.filename();
    
    KService::Ptr service = KService::serviceByStorageId(name);
    if(service && service->isValid()){
      KCModuleInfo kcminfo(service);
      
      DOM::DOMString innerHTML;      
      MetabarWidget::addEntry(innerHTML, i18n("Run"), "kcmshell:/" + name, kcminfo.icon());
      node.setInnerHTML(innerHTML);
      
      m_functions->show("actions");
    }
    else{
      m_functions->hide("actions");
    }
  }
}

void SettingsPlugin::loadInformation(DOM::HTMLElement node)
{
  KURL url = m_items.getFirst()->url();
  if(url.path().endsWith("/")){
    m_functions->hide("info");
  }
  else{
    QString path = url.path();
    QString name = url.filename();
    
    KService::Ptr service = KService::serviceByStorageId(name);
    if(service && service->isValid()){
      KCModuleInfo kcminfo(service);
      
      bool needsRoot = kcminfo.needsRootPrivileges();
    
      DOM::DOMString innerHTML;
      innerHTML += "<ul class=\"info\"><b>" + i18n("Name") + ": </b>";
      innerHTML += kcminfo.moduleName();
      innerHTML += "</ul><ul class=\"info\"><b>" + i18n("Comment") + ": </b>";
      innerHTML += kcminfo.comment();
      innerHTML += "</ul>";
      
      if(needsRoot){
        innerHTML += "<ul class=\"info\"><b>";
        innerHTML += i18n("Needs root privileges");
        innerHTML += "</b></ul>";
      }
      node.setInnerHTML(innerHTML);
      
      m_functions->show("info");
    }
    else{
      m_functions->hide("info");
    }
  }
}

void SettingsPlugin::loadApplications(DOM::HTMLElement node)
{
  m_functions->hide("open");
}

void SettingsPlugin::loadPreview(DOM::HTMLElement node)
{
  m_functions->hide("preview");
}

void SettingsPlugin::loadBookmarks(DOM::HTMLElement node)
{
  m_functions->hide("bookmarks");
}

bool SettingsPlugin::handleRequest(const KURL &)
{  
  return false;
}

void SettingsPlugin::slotGotEntries(KIO::Job *job, const KIO::UDSEntryList &list)
{
  if(!job){
    return;
  }
  
  DOM::HTMLDocument doc = m_html->htmlDocument();
  DOM::HTMLElement node = doc.getElementById("actions");
  DOM::DOMString innerHTML;
  
  KIO::UDSEntryList::ConstIterator it     = list.begin();
  KIO::UDSEntryList::ConstIterator it_end = list.end();
  for(; it != it_end; ++it){
    QString name;
    QString icon;
    QString url;
    long type;
  
    KIO::UDSEntry::ConstIterator atomit     = (*it).begin();
    KIO::UDSEntry::ConstIterator atomit_end = (*it).end();
    for(; atomit != atomit_end; ++atomit){
      if((*atomit).m_uds == KIO::UDS_NAME){
        name = (*atomit).m_str;
      }
      else if((*atomit).m_uds == KIO::UDS_ICON_NAME){
        icon = (*atomit).m_str;
      }
      else if((*atomit).m_uds == KIO::UDS_URL){
        url = (*atomit).m_str;
      }
      
      else if((*atomit).m_uds == KIO::UDS_FILE_TYPE){
        type = (*atomit).m_long;
      }
    }
    
    kdDebug() << url << endl;
    
    if(type == S_IFREG){
      url = "kcmshell:/" + KURL(url).filename();
    }

    MetabarWidget::addEntry(innerHTML, name, url, icon);
  }
  
  node.setInnerHTML(innerHTML);
}

void SettingsPlugin::slotJobFinished(KIO::Job *job)
{
  if(list_job && job == list_job){
    list_job = 0;

    m_functions->adjustSize("actions");
  }
}

#include "settingsplugin.moc"
