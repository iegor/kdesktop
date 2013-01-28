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
 
#include "defaultplugin.h"
#include "metabarwidget.h"

#include <kurl.h>
#include <kstandarddirs.h>
#include <kicontheme.h>
#include <khtmlview.h>
#include <klocale.h>
#include <kapplication.h>
#include <kdesktopfile.h>
#include <ktrader.h>
#include <krun.h>
#include <kfilemetainfo.h>
#include <kconfig.h>
#include <kimageio.h>
#include <kdebug.h>
#include <kmdcodec.h>

#include <kio/previewjob.h>

#include <dcopclient.h>
#include <dcopref.h>

#include <qdir.h>
#include <qfile.h>
#include <qrect.h>
#include <qpoint.h>
#include <qbuffer.h>

#include <dom_string.h>
#include <html_image.h>

#define EVENT_TYPE DOM::DOMString("click")
#define MODIFICATION 1

DefaultPlugin::DefaultPlugin(KHTMLPart* html, MetabarFunctions *functions, const char *name) : ProtocolPlugin (html, functions, name)
{
  services = new ServiceLoader(m_html->view(), "serviceloader");
  
  preview_job = 0;
}

DefaultPlugin::~DefaultPlugin()
{
}

void DefaultPlugin::killJobs(){
  if(preview_job){  
    preview_job->kill();
    preview_job = 0;
  }
}

void DefaultPlugin::deactivate()
{  
  m_functions->hide("actions");
  m_functions->hide("open");
  m_functions->hide("info");
  m_functions->hide("preview");
}

void DefaultPlugin::loadActions(DOM::HTMLElement node)
{ 
  KFileItem *item = m_items.getFirst();
  KURL url = item->url();
  
  KConfig config("metabarrc", true, false);
  config.setGroup("General");
  
  KConfig iconConfig(locate("data", "metabar/iconsrc"));
  iconConfig.setGroup("Icons");
  
  DOM::DOMString innerHTML;
  
  QStringList actions = config.readListEntry("Actions");
  int maxActions = config.readNumEntry("MaxActions", 5);
  int actionCount = 0;
  
  for(QStringList::Iterator it = actions.begin(); it != actions.end(); ++it){
    if((*it).startsWith("metabar/")){
      if((*it).right((*it).length() - 8) == "share"){
        MetabarWidget::addEntry(innerHTML, i18n("Share"), "action://" + *it, "network", QString::null, actionCount < maxActions ? QString::null : QString("hiddenaction"), actionCount >= maxActions);
        actionCount++;
      }
    }
    else{
      DCOPRef action(kapp->dcopClient()->appId(), QCString(m_html->view()->topLevelWidget()->name()).append("/action/").append((*it).utf8()));

      if(!action.isNull()){
        if(action.call("enabled()")){
          QString text = action.call("plainText()");
          QString icon = iconConfig.readEntry(*it, action.call("icon()"));
          
          MetabarWidget::addEntry(innerHTML, text, "action://" + *it, icon, QString::null, actionCount < maxActions ? QString::null : QString("hiddenaction"), actionCount >= maxActions);
          actionCount++;
        }
      }
    }
  }
  
  config.setGroup("General");
  if(config.readBoolEntry("ShowServicemenus", true)){
    services->loadServices(*(m_items.getFirst()), innerHTML, actionCount);
  }

  if(actionCount == 0) {
    m_functions->hide("actions");
  }
  else{
    if(actionCount > maxActions){
      MetabarWidget::addEntry(innerHTML, i18n("More"), "more://hiddenaction", "1downarrow", "hiddenaction");
    }
    
    node.setInnerHTML(innerHTML);
    m_functions->show("actions");
  }
}

void DefaultPlugin::loadApplications(DOM::HTMLElement node)
{
  if(m_items.count() == 1){
    KFileItem *item = m_items.getFirst();
    KURL url = item->url();
    
    QDir dir(url.path());
    dir = dir.canonicalPath();
  
    if(item->isDir() || dir.isRoot()){
      m_functions->hide("open");
      node.setInnerHTML(DOM::DOMString());
    }
    else{
      DOM::DOMString innerHTML;
      
      if(KDesktopFile::isDesktopFile(item->url().path())){ //Desktop file
        KDesktopFile desktop(url.path(), TRUE);
        
        if(desktop.hasApplicationType ()){                    
          MetabarWidget::addEntry(innerHTML, i18n("Run %1").arg(desktop.readName()), "desktop://" + url.path(), desktop.readIcon());
          
          m_functions->show("open");
        }
    
        else{
          m_functions->hide("open");
        }
      }
      else{ //other files
        KTrader::OfferList offers;
        
        offers = KTrader::self()->query(item->mimetype(), "Type == 'Application'");
        if(!offers.isEmpty()){
          KConfig config("metabarrc", true, false);
          config.setGroup("General");
        
          int id = 0;
          int max = config.readNumEntry("MaxEntries", 5);
          
          runMap.clear();      
          KTrader::OfferList::ConstIterator it = offers.begin();
          
          for(; it != offers.end(); it++){
            QString nam;
            nam.setNum(id);
            
            bool hide = id >= max;
            MetabarWidget::addEntry(innerHTML, (*it)->name(), "exec://" + nam, (*it)->icon(), QString::null, hide ? QString("hiddenapp") : QString::null, hide);
            
            runMap.insert(id, *it);
            id++;
          }
          
          if(id > max){
            MetabarWidget::addEntry(innerHTML, i18n("More"), "more://hiddenapp", "1downarrow", "hiddenapp");
          }
        }
        else{
          MetabarWidget::addEntry(innerHTML, i18n("Choose Application"), "openwith:///", "run");
        }
      }
      
      node.setInnerHTML(innerHTML);
      
      m_functions->show("open");
    }
  }
  else{
    m_functions->hide("open");
  }
}

void DefaultPlugin::loadInformation(DOM::HTMLElement node)
{
  if(m_items.count() == 1){
    KFileItem *item = m_items.getFirst();
    KFileItem *that = const_cast<KFileItem *>(item);
    
    DOM::DOMString innerHTML;
    innerHTML += "<ul class=\"info\"><b>" + i18n("Type") + ": </b>";
    innerHTML += that->determineMimeType()->comment();
    innerHTML += "</ul><ul class=\"info\"><b>" + i18n("Size") + ": </b>";
    innerHTML += KIO::convertSize(item->size());
    innerHTML += "</ul><ul class=\"info\"><b>" + i18n("User") + ": </b>";
    innerHTML += item->user();
    innerHTML += "</ul><ul class=\"info\"><b>" + i18n("Group") + ": </b>";
    innerHTML += item->group();
    innerHTML += "</ul><ul class=\"info\"><b>" + i18n("Permissions") + ": </b>";
    innerHTML += (item->permissionsString());
    innerHTML += "</ul><ul class=\"info\"><b>" + i18n("Modified") + ": </b>";
    innerHTML += item->timeString(KIO::UDS_MODIFICATION_TIME);
    innerHTML += "</ul><ul class=\"info\"><b>" + i18n("Accessed") + ": </b>";
    innerHTML += item->timeString(KIO::UDS_ACCESS_TIME);
    innerHTML += "</ul>";
        
    if(item->isLink()){
      innerHTML += "<ul class=\"info\"><b>" + i18n("Linktarget") + ": </b>";
      innerHTML += item->linkDest();
      innerHTML += "</ul>";
    }
        
    if(!item->isDir()){
      const KFileMetaInfo &metaInfo = item->metaInfo();
      if(metaInfo.isValid()){
        QStringList groups = metaInfo.supportedGroups();
        
        int id = 0;
        QString nam;
        
        for(QStringList::ConstIterator it = groups.begin(); it != groups.end(); ++it){
          KFileMetaInfoGroup group = metaInfo.group(*it);
          if(group.isValid()){
            nam.setNum(id);
            
            innerHTML += "<ul class=\"info\"><a class=\"infotitle\" id=\"info_" + nam + "\" href=\"more://info_" + nam + "\">" + group.translatedName() + "</a></ul>";
            
            QStringList keys = group.supportedKeys();
          
            for(QStringList::ConstIterator it = keys.begin(); it != keys.end(); ++it){
              const KFileMetaInfoItem metaInfoItem = group.item(*it);
              
              if(metaInfoItem.isValid()){
              
                innerHTML += "<ul class=\"info\" style=\"display:none\"><b name=\"info_" + nam + "\">" + metaInfoItem.translatedKey() + ": </b>";
                innerHTML += metaInfoItem.string();
                innerHTML += "</ul>";
              }
            }
            
            id++;
          }
        }
      }
    }
        
    node.setInnerHTML(innerHTML);
  }
  else{
    KIO::filesize_t size = 0;
    int files = 0;
    int dirs  = 0;
    
    for(KFileItemListIterator it(m_items); it.current(); ++it){
      size += (*it)->size();
      
      if((*it)->isDir()){
        dirs++;
      }
      else{
        files++;
      }
    }
 
    DOM::DOMString innerHTML;
    innerHTML += "<ul class=\"info\"><b>" + i18n("Size") + ": </b>";
    innerHTML += KIO::convertSize(size);
    innerHTML += "</ul><ul class=\"info\"><b>" + i18n("Files") + ": </b>";
    innerHTML += QString().setNum(files);
    innerHTML += "</ul><ul class=\"info\"><b>" + i18n("Folders") + ": </b>";
    innerHTML += QString().setNum(dirs);
        innerHTML += "</ul><ul class=\"info\"><b>" + i18n("Total Entries") + ": </b>";
    innerHTML += QString().setNum(m_items.count());
    innerHTML += "</ul>";    
    node.setInnerHTML(innerHTML);
  }
  m_functions->show("info");
}

void DefaultPlugin::loadPreview(DOM::HTMLElement node)
{
  if(m_items.count() == 1){
    KFileItem *item = m_items.getFirst();
    KURL url = item->url();

    QDir dir(url.path());
    dir = dir.canonicalPath();
    
    if(item->isDir() || dir.isRoot()){
      m_functions->hide("preview");
    }
    else{
      if(item->mimetype().startsWith("audio/")){
        DOM::DOMString innerHTML("<ul><a class=\"previewdesc\" href=\"preview:///\">");
        innerHTML += i18n("Click to start preview");
        innerHTML += "</a></ul>";
        node.setInnerHTML(innerHTML);
        
        //m_functions->show("preview");
      }
      else{
        DOM::DOMString innerHTML("<ul style=\"text-align-center\"><nobr>");
        innerHTML += i18n("Creating preview");
        innerHTML += "</nobr></ul>";
        node.setInnerHTML(innerHTML);
        
        //m_functions->show("preview");
      
        preview_job = KIO::filePreview(KURL::List(url), m_html->view()->width() - 30);
          
        connect(preview_job, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)), this, SLOT(slotSetPreview(const KFileItem*, const QPixmap&)));
        connect(preview_job, SIGNAL(failed(const KFileItem *)), this, SLOT(slotPreviewFailed(const KFileItem *)));
        connect(preview_job, SIGNAL(result(KIO::Job *)), this, SLOT(slotJobFinished(KIO::Job *)));
      }
      
      m_functions->show("preview");
    }
  }
  else{
    m_functions->hide("preview");
  }
}

void DefaultPlugin::loadBookmarks(DOM::HTMLElement node)
{
  m_functions->hide("bookmarks");
}

void DefaultPlugin::slotSetPreview(const KFileItem *item, const QPixmap &pix)
{
  DOM::HTMLDocument doc = m_html->htmlDocument();
  DOM::HTMLElement node = doc.getElementById("preview");
  
  QByteArray data;
  QBuffer buffer(data);
  buffer.open(IO_WriteOnly);
  pix.save(&buffer, "PNG");
  
  QString src = QString::fromLatin1("data:image/png;base64,%1").arg(KCodecs::base64Encode(data));  
  bool media = item->mimetype().startsWith("video/");

  DOM::DOMString innerHTML;
  
  innerHTML += QString("<ul style=\"height: %1px\"><a class=\"preview\"").arg(pix.height() + 15);
  
  if(media){
    innerHTML += " href=\"preview:///\"";
  }
  innerHTML +="><img id=\"previewimage\" src=\"";
  innerHTML += src;
  innerHTML += "\" width=\"";
  innerHTML += QString().setNum(pix.width());
  innerHTML += "\" height=\"";
  innerHTML += QString().setNum(pix.height());
  innerHTML += "\" /></a></ul>";
  
  if(media){
    innerHTML += "<ul><a class=\"previewdesc\" href=\"preview:///\">" + i18n("Click to start preview") + "</a></ul>";
  }
  
  node.setInnerHTML(innerHTML);
  
  //script.append("adjustPreviewSize(" + height_str + ");");
  m_functions->show("preview");
  m_functions->adjustSize("preview");
}

void DefaultPlugin::slotPreviewFailed(const KFileItem *item)
{
  DOM::HTMLDocument doc = m_html->htmlDocument();
  DOM::HTMLElement preview = static_cast<DOM::HTMLElement>(doc.getElementById("preview_image"));
  if(!preview.isNull()){
    preview.setAttribute(DOM::DOMString("src"), DOM::DOMString());
  }
  
  m_functions->hide("preview");
}

void DefaultPlugin::slotJobFinished(KIO::Job *job)
{
  if(preview_job && job == preview_job){
    preview_job = 0;
  }
}

bool DefaultPlugin::handleRequest(const KURL &url)
{
  QString protocol = url.protocol();

  if(protocol == "exec"){
    int id = url.host().toInt();
        
    QMap<int,KService::Ptr>::Iterator it = runMap.find(id);
    if(it != runMap.end()){
    
      KFileItem *item = m_items.getFirst();
      if(item){
        KRun::run( **it, KURL::List(item->url()));
        return true;
      }
    }
  }
  
  else if(protocol == "service"){
    QString name = url.url().right(url.url().length() - 10);
    
    services->runAction(name);
    return true;
  }
  
  else if(protocol == "servicepopup"){
    QString id = url.host();
    
    DOM::HTMLDocument doc = m_html->htmlDocument();
    DOM::HTMLElement node = static_cast<DOM::HTMLElement>(doc.getElementById("popup" + id));

    if(!node.isNull()){
      QRect rect = node.getRect();
      QPoint p = m_html->view()->mapToGlobal(rect.bottomLeft());
      
      services->showPopup(id, p);
    }
    
    return true;
  }
  
  return false;
}

#include "defaultplugin.moc"
