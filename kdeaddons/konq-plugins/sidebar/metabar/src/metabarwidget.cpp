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
#include "configdialog.h"

#include "defaultplugin.h"
#include "settingsplugin.h"
#include "remoteplugin.h"
#include "httpplugin.h"

#include <qwidget.h>
#include <qlayout.h>
#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qvaluelist.h>
#include <qurl.h>
#include <qbuffer.h>

#include <khtmlview.h>
#include <kapplication.h>
#include <kstandarddirs.h>
#include <kurl.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kicontheme.h>
#include <ktrader.h>
#include <klocale.h>
#include <krun.h>
#include <kdesktopfile.h>
#include <kpropertiesdialog.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kshortcut.h>
#include <kmimetype.h>
#include <kcmoduleinfo.h>
#include <kmdcodec.h>

#include <kparts/browserextension.h>

#include <dom2_events.h>
#include <dom2_views.h>
#include <dom_doc.h>
#include <dom_element.h>
#include <dom_string.h>
#include <html_element.h>
#include <html_misc.h>
#include <html_image.h>
#include <css_value.h>

#include <dcopref.h>
#include <dcopclient.h>

#define EVENT_TYPE DOM::DOMString("click")
#define ACTIVATION 1

MetabarWidget::MetabarWidget(QWidget *parent, const char *name) : QWidget(parent, name)
{
  skip = false;
  loadComplete = false;

  currentItems = new KFileItemList;
  currentItems->setAutoDelete(true);
  
  config = new KConfig("metabarrc");
  
  dir_watch = new KDirWatch();
  connect(dir_watch, SIGNAL(dirty(const QString&)), this, SLOT(slotUpdateCurrentInfo(const QString&)));
  connect(dir_watch, SIGNAL(created(const QString&)), this, SLOT(slotUpdateCurrentInfo(const QString&)));
  connect(dir_watch, SIGNAL(deleted(const QString&)), this, SLOT(slotDeleteCurrentInfo(const QString&)));

  html = new KHTMLPart(this, "metabarhtmlpart");
  html->setJScriptEnabled(true);
  html->setPluginsEnabled(true);
  html->setCaretVisible(false);
  html->setDNDEnabled(false);
  html->setJavaEnabled(false);
  html->view()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  html->view()->hide();
  
  connect(html->browserExtension(), SIGNAL(openURLRequest( const KURL &, const KParts::URLArgs & )), this, SLOT(handleURLRequest(const KURL &, const KParts::URLArgs &)));
  connect(html, SIGNAL(completed()), this, SLOT(loadCompleted()));
  connect(html, SIGNAL(popupMenu(const QString &, const QPoint &)), this, SLOT(slotShowPopup(const QString&, const QPoint &)));
  
  functions = new MetabarFunctions(html, this);
  
  currentPlugin = 0;
  defaultPlugin = new DefaultPlugin(html, functions);  
  HTTPPlugin *httpPlugin = new HTTPPlugin(html, functions);
  
  //plugins.setAutoDelete(true);
  plugins.insert("settings", new SettingsPlugin(html, functions));
  plugins.insert("remote", new RemotePlugin(html, functions));
  //plugins.insert("trash", new TrashPlugin(html, functions));
  plugins.insert("http", httpPlugin);
  plugins.insert("https", httpPlugin);
  
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(html->view());
  
  popup = new KPopupMenu(0);
  KAction *configAction = new KAction(i18n("Configure %1...").arg("Metabar"), "configure", KShortcut(), this, SLOT(slotShowConfig()), html->actionCollection(), "configure");
  configAction->plug(popup);
  
  KAction *reloadAction = new KAction(i18n("Reload Theme"), "reload", KShortcut(), this, SLOT(setTheme()), html->actionCollection(), "reload");
  reloadAction->plug(popup);

  setTheme();
}

MetabarWidget::~MetabarWidget()
{
  config->sync();
  delete config;

  delete dir_watch;
  delete currentItems;
}

void MetabarWidget::setFileItems(const KFileItemList &items, bool check)
{
  if(!loadComplete){
    return;
  }

  if(skip){
    skip = false;
    return;
  }

  if(check){
    int count = items.count();
    
    KURL url(getCurrentURL());
    
    KFileItem *newItem = items.getFirst();
    KFileItem *oldItem = currentItems->getFirst();
  
    if(count == 0){
      if(oldItem){
        if(oldItem->url() == url){
          return;
        }
      }
      
      currentItems->clear();
      currentItems->append(new KFileItem(S_IFDIR, KFileItem::Unknown, url, true));
    }
    
    else if(count == 1){
      if(newItem){
        if(newItem->url().isEmpty()){
          return;
        }
        
        if(currentItems->count() == items.count() && oldItem){
          if(newItem->url() == oldItem->url()){
            return;
          }
        }
      }
          
      currentItems = new KFileItemList(items);
    }
    
    else{
      if(currentItems && *currentItems == items){
        return;
      }
      currentItems = new KFileItemList(items);
    }
  }
  else{
    currentItems = new KFileItemList(items);
  }
  
  if(currentPlugin){
    currentPlugin->deactivate();
  }
  
  QString protocol = currentItems->getFirst()->url().protocol();
  currentPlugin = plugins[protocol];
  
  if(!currentPlugin){
    currentPlugin = defaultPlugin;
  }
  
  ProtocolPlugin::activePlugin = currentPlugin;
  currentPlugin->setFileItems(*currentItems);
}

QString MetabarWidget::getCurrentURL()
{
  DCOPRef ref(kapp->dcopClient()->appId(), this->topLevelWidget()->name());
  DCOPReply reply = ref.call("currentURL()");
    
  if (reply.isValid()) {
    QString url;
    reply.get(url, "QString");
      
    if(!url.isNull() && !url.isEmpty()){
      return url;
    }
  }
  return 0;
}

void MetabarWidget::openURL(const QString &url)
{
  DCOPRef ref(kapp->dcopClient()->appId(), this->topLevelWidget()->name());
  DCOPReply reply = ref.call("openURL", url);
}

void MetabarWidget::openTab(const QString &url)
{
  DCOPRef ref(kapp->dcopClient()->appId(), this->topLevelWidget()->name());
  DCOPReply reply = ref.call("newTab", url);
}

void MetabarWidget::callAction(const QString &action)
{
  DCOPRef ref(kapp->dcopClient()->appId(), QString(this->topLevelWidget()->name()).append("/action/").append(action).utf8());
  if(ref.call("enabled()")){
    ref.call("activate()");
  }
}

void MetabarWidget::loadLinks()
{
  config->setGroup("General");
  QStringList links = config->readListEntry("Links");
  
  if(links.count() == 0){
    functions->hide("links");
  }
  else{
    functions->show("links");
  
    DOM::HTMLDocument doc = html->htmlDocument();
    DOM::HTMLElement node = doc.getElementById("links");
    
    if(!node.isNull()){
      DOM::DOMString innerHTML;
    
      for(QStringList::Iterator it = links.begin(); it != links.end(); ++it){
        config->setGroup("Link_" + (*it));     
        addEntry(innerHTML, config->readEntry("Name"), config->readEntry("URL"), config->readEntry("Icon", "folder"));
      }
  
      node.setInnerHTML(innerHTML);
    }
    
    functions->adjustSize("links");
  }
}

void MetabarWidget::loadCompleted()
{
  DOM::HTMLDocument doc = html->htmlDocument();

  DOM::NodeList i18n_a_list = doc.getElementsByTagName("a");
  for(uint i = 0; i < i18n_a_list.length(); i++){
    DOM::HTMLElement node = static_cast<DOM::HTMLElement>(i18n_a_list.item(i));
    if(!node.isNull()){
      if(node.hasAttribute("i18n")){
        QString text = node.innerText().string();
        node.setInnerText(DOM::DOMString(i18n(text.utf8().data())));
      }
      
      if(node.hasAttribute("image")){
        QString icon = node.getAttribute("image").string();
        QString url = getIconPath(icon);
        QString style = QString("background-image: url(%1);").arg(url);
        
        node.setAttribute("style", style);
      }
    }
  }
  
  DOM::NodeList i18n_ul_list = doc.getElementsByTagName("ul");
  for(uint i = 0; i < i18n_ul_list.length(); i++){
    DOM::HTMLElement node = static_cast<DOM::HTMLElement>(i18n_ul_list.item(i));
    if(!node.isNull()){
      if(node.hasAttribute("i18n")){
        QString text = node.innerText().string();
        node.setInnerText(DOM::DOMString(i18n(text.utf8().data())));
      }
    }
  }

  config->setGroup("General");
  QString file = locate("data", QString("metabar/themes/%1/default.css").arg(config->readEntry("Theme", "default")));
  if(file.isNull()){
    file = locate("data", QString("metabar/themes/default/default.css"));
  }
  
  QFile cssfile(file);
  if(cssfile.open(IO_ReadOnly)){  
    QTextStream stream( &cssfile );
    QString tmp = stream.read();
    cssfile.close();
    
    tmp.replace("./", KURL::fromPathOrURL(file).directory(false));    
    html->setUserStyleSheet(tmp);
  }
  
  loadComplete = true;
  html->view()->setFrameShape(config->readBoolEntry("ShowFrame", true) ? QFrame::StyledPanel : QFrame::NoFrame);
  html->view()->show();
  
  if(currentItems && !currentItems->isEmpty()){
    setFileItems(*currentItems, false);
  }
  else{
    QString url = getCurrentURL();
    KFileItem *item = new KFileItem(KFileItem::Unknown, KFileItem::Unknown, KURL(url), true);
    KFileItemList list;
    list.append(item);
    setFileItems(list, false);
  }
  
  loadLinks();
}

void MetabarWidget::handleURLRequest(const KURL &url, const KParts::URLArgs &args)
{  
  if(!currentPlugin){
    return;
  }

  QString protocol = url.protocol();
  
  if(currentPlugin->handleRequest(url)){
    return;
  }
  
  if(protocol == "desktop"){
    QString path = url.path();
    
    if(KDesktopFile::isDesktopFile(path)){
      KRun::run(new KDesktopFile(path, true), KURL::List());
    }
  }
  
  else if(protocol == "kcmshell"){
    QString module = url.path().remove('/');
    
    KRun::runCommand("kcmshell " + module);
  }
  
  else if(protocol == "action"){
    QString action = url.url().right(url.url().length() - 9);    
    if(action.startsWith("metabar/")){
      QString newact = action.right(action.length() - 8);
      
      if(newact == "share"){
        slotShowSharingDialog();
      }
    }
    else{
      callAction(action);
    }
  }
  
  else if(protocol == "preview"){
    if(currentItems && !currentItems->isEmpty()){
      KFileItem *item = currentItems->getFirst();
      
      DOM::HTMLDocument doc = html->htmlDocument();
      DOM::HTMLElement node = static_cast<DOM::HTMLElement>(doc.getElementById("preview"));
      DOM::HTMLImageElement image = static_cast<DOM::HTMLImageElement>(doc.getElementById("previewimage"));
      
      if(!node.isNull()){
        skip = true; //needed to prevent some weired reload
      
        DOM::DOMString innerHTML;
        innerHTML += QString("<ul style=\"width: %1px; height: %1px\">").arg(image.width(), image.height());
        innerHTML += "<object class=\"preview\" type=\"";
        innerHTML += item->mimetype();
        innerHTML += "\" data=\"";
        innerHTML += item->url().url();
        innerHTML += "\" width=\"";
        innerHTML += QString().setNum(image.width());
        innerHTML += "\" height=\"";
        innerHTML += QString().setNum(image.height());
        innerHTML += "\" /></ul>";
        node.setInnerHTML(innerHTML);
      }
    }
  }
  
  else if(protocol == "more"){
    QString name = url.host();
    
    DOM::HTMLDocument doc = html->htmlDocument();
    DOM::NodeList list = doc.getElementsByName(name);
    DOM::HTMLElement element = static_cast<DOM::HTMLElement>(doc.getElementById(name));
    bool showMore = true;
    
    for(uint i = 0; i < list.length(); i++){
      DOM::HTMLElement node = static_cast<DOM::HTMLElement>(list.item(i));
      if(!node.isNull()){   
        DOM::HTMLElement parent = static_cast<DOM::HTMLElement>(node.parentNode());
        DOM::CSSStyleDeclaration style = parent.style();
        DOM::DOMString display = style.getPropertyValue("display");
        DOM::DOMString newDisplay = display == "none" ? "block" : "none";
        
        style.setProperty("display", newDisplay, "important");
        
        showMore = display == "block";
      }
    }
    
    if(element.id().string().startsWith("hidden")){
      QString style = QString("background-image: url(%1);").arg(getIconPath(showMore ? "1downarrow" : "1uparrow"));
      element.setInnerText( showMore ? i18n("More") : i18n("Less") );
      element.setAttribute("style", style);
    }
    
    DOM::HTMLElement parent = static_cast<DOM::HTMLElement>(element.parentNode().parentNode());
    functions->adjustSize(parent.id());
  }
  
  else if(protocol == "function"){
    functions->handleRequest(url);
  }

  else if(protocol == "configure"){
    slotShowConfig();
  }
  
  else if(protocol == "openwith"){
    if(currentItems && !currentItems->isEmpty()){
      KFileItem *item = currentItems->getFirst();
      
      KRun::displayOpenWithDialog(KURL::List(item->url()), false);
    }
  }
  
  else{
    if(args.newTab()){
      openTab(url.url());
    }
    else{
      openURL(url.url());
    }
  }
}

QString MetabarWidget::getIconPath(const QString &name)
{
  QPixmap icon = SmallIcon(name);
  
  QByteArray data;
  QBuffer buffer(data);
  buffer.open(IO_WriteOnly);
  icon.save(&buffer, "PNG");
  
  return QString::fromLatin1("data:image/png;base64,%1").arg(KCodecs::base64Encode(data));  
}

void MetabarWidget::slotShowSharingDialog()
{
  if(currentItems && currentItems->count() == 1){
    KPropertiesDialog *dialog = new KPropertiesDialog(currentItems->first(), 0, 0, true);
    dialog->showFileSharingPage();
  }
}

void MetabarWidget::slotShowConfig()
{
  ConfigDialog *config_dialog = new ConfigDialog(this);
  if(config_dialog->exec() == QDialog::Accepted){
    config->reparseConfiguration();
    
    setFileItems(*currentItems, false);
    loadLinks();
    
    setTheme();
    
    html->view()->setFrameShape(config->readBoolEntry("ShowFrame", true) ? QFrame::StyledPanel : QFrame::NoFrame);
  }
  
  delete config_dialog;
}

void MetabarWidget::slotShowPopup(const QString &url, const QPoint &point)
{
  popup->exec(point);
}

void MetabarWidget::slotUpdateCurrentInfo(const QString &path)
{
  if(currentItems){
    KFileItem *item = new KFileItem(KFileItem::Unknown, KFileItem::Unknown, KURL(path), true);
    
    if(currentItems->count() == 1){
      currentItems->clear();
      currentItems->append(item);
    }
    
    setFileItems(*currentItems, false);
  }
}

void MetabarWidget::slotDeleteCurrentInfo(const QString&)
{
  if(currentItems && currentItems->count() == 1){
    QString url = getCurrentURL();
    KURL currentURL;
     
    if(currentItems){
      currentURL = currentItems->getFirst()->url();
    }
    
    if(!currentURL.isEmpty() && KURL(url) != currentURL){
      if(dir_watch->contains(currentURL.path())){
        dir_watch->removeDir(currentURL.path());
      }
      dir_watch->addDir(url);
          
      KFileItem *item = new KFileItem(KFileItem::Unknown, KFileItem::Unknown, url, true);
      
      currentItems->clear();
      currentItems->append(item);
        
      setFileItems(*currentItems, false);
    }
  }
}

void MetabarWidget::addEntry(DOM::DOMString &html, const QString name, const QString url, const QString icon, const QString id, const QString nameatt, bool hidden)
{
  html += "<ul";
  
  if(hidden){
    html += " style=\"display: none;\"";
  }
  
  html += "><a";
  
  if(!id.isNull() && !id.isEmpty()){
    html += " id=\"";
    html += id;
    html += "\"";
  }
  
  if(!nameatt.isNull() && !nameatt.isEmpty()){
    html += " name=\"";
    html += nameatt;
    html += "\"";
  }
  
  html += " href=\"";
  html += url;
  html += "\" onClick=\"this.blur();\" style=\"background-image: url(";
  html += getIconPath(icon);
  html += ");\">";
  html += name;
  html += "</a></ul>";
}

void MetabarWidget::setTheme()
{
  loadComplete = false;

  config->setGroup("General");
  QString file = locate("data", QString("metabar/themes/%1/layout.html").arg(config->readEntry("Theme", "default")));
  
  html->openURL(KURL(file));
}

#include "metabarwidget.moc"
