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
 
#include "metabarfunctions.h"

#include <html_element.h>
#include <html_document.h>
#include <css_value.h>

#include <kconfig.h>

#include <qrect.h>

#define CSS_PRIORITY "important"
#define RESIZE_SPEED 5
#define RESIZE_STEP 2

MetabarFunctions::MetabarFunctions(KHTMLPart *html, QObject *parent, const char* name) : QObject(parent, name), m_html(html)
{
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(animate()));
}

MetabarFunctions::~MetabarFunctions()
{
  if(timer->isActive()){
    timer->stop();
  }
}

void MetabarFunctions::handleRequest(const KURL &url)
{
  QString function = url.host();
  QStringList params = QStringList::split(',', url.filename());
  
  if(function == "toggle"){
    if(params.size() == 1){
      toggle(params.first());
    }
  }
  
  else if(function == "adjustSize"){
    if(params.size() == 1){
      adjustSize(params.first());
    }
  }
  
  else if(function == "show"){
    if(params.size() == 1){
      show(params.first());
    }
  }
  
  else if(function == "hide"){
    if(params.size() == 1){
      hide(params.first());
    }
  }
}

void MetabarFunctions::toggle(DOM::DOMString item)
{
  DOM::HTMLDocument doc = m_html->htmlDocument();
  DOM::HTMLElement node = static_cast<DOM::HTMLElement>(doc.getElementById(item));
  
  if(!node.isNull()){
    DOM::NodeList children = node.childNodes();
    DOM::CSSStyleDeclaration style = node.style();
    DOM::DOMString expanded = node.getAttribute("expanded");
    
    bool isExpanded = expanded == "true";
    
    int height = 0;
    if(!isExpanded){
      height = getHeight(node);
    }
    
    DOM::DOMString att = isExpanded ? "false" : "true";
    node.setAttribute("expanded", att);
    
    KConfig config("metabarrc");
    config.setGroup("General");
    
    if(config.readBoolEntry("AnimateResize", false)){
      resizeMap[item.string()] = height;
      
      if(!timer->isActive()){
        timer->start(RESIZE_SPEED);
      }
    }
    else{
      style.setProperty("height", QString("%1px").arg(height), CSS_PRIORITY);
    }
  }
}

void MetabarFunctions::adjustSize(DOM::DOMString item)
{
  DOM::HTMLDocument doc = m_html->htmlDocument();
  DOM::HTMLElement node = static_cast<DOM::HTMLElement>(doc.getElementById(item));
  
  if(!node.isNull()){
    DOM::NodeList children = node.childNodes();
    DOM::CSSStyleDeclaration style = node.style();
    DOM::DOMString expanded = node.getAttribute("expanded");
    
    bool isExpanded = expanded == "true";
    
    if(isExpanded){
      int height = getHeight(node);
      
      KConfig config("metabarrc");
      config.setGroup("General");
    
      if(config.readBoolEntry("AnimateResize", false)){
        resizeMap[item.string()] = height;
        
        if(!timer->isActive()){
          timer->start(RESIZE_SPEED);
        }
      }
      else{
        style.setProperty("height", QString("%1px").arg(height), CSS_PRIORITY);
      }
    }
  }
}

void MetabarFunctions::animate()
{
  QMap<QString, int>::Iterator it;
  for(it = resizeMap.begin(); it != resizeMap.end(); ++it ) {
    QString id = it.key();
    int height = it.data();
    int currentHeight = 0;
    
    DOM::HTMLDocument doc = m_html->htmlDocument();
    DOM::HTMLElement node = static_cast<DOM::HTMLElement>(doc.getElementById(id));
    DOM::CSSStyleDeclaration style = node.style();
    
    QString currentHeightString = style.getPropertyValue("height").string();
    if(currentHeightString.endsWith("px")){
      currentHeight = currentHeightString.left(currentHeightString.length() - 2).toInt();
    }
    
    if(currentHeight == height){
      resizeMap.remove(id);
      
      if(resizeMap.isEmpty()){  
        timer->stop();
      }
    }
    else{
      int diff = kAbs(currentHeight - height);
      int changeValue = RESIZE_STEP;
      
      if(diff < RESIZE_STEP){
        changeValue = diff;
      }
      
      int change = currentHeight < height ? changeValue : -changeValue;
      style.setProperty("height", QString("%1px").arg(currentHeight + change), CSS_PRIORITY);
      doc.updateRendering();
    }
  }
}

void MetabarFunctions::show(DOM::DOMString item)
{
  DOM::HTMLDocument doc = m_html->htmlDocument();
  DOM::HTMLElement node = static_cast<DOM::HTMLElement>(doc.getElementById(item));
  if(!node.isNull()){
    DOM::HTMLElement parent = static_cast<DOM::HTMLElement>(node.parentNode());
    
    DOM::CSSStyleDeclaration style = parent.style();
    style.setProperty("display", "block", CSS_PRIORITY);
  }
}

void MetabarFunctions::hide(DOM::DOMString item)
{
  DOM::HTMLDocument doc = m_html->htmlDocument();
  DOM::HTMLElement node = static_cast<DOM::HTMLElement>(doc.getElementById(item));
  if(!node.isNull()){
    DOM::HTMLElement parent = static_cast<DOM::HTMLElement>(node.parentNode());
    
    DOM::CSSStyleDeclaration style = parent.style();
    style.setProperty("display", "none", CSS_PRIORITY);
  }
}

int MetabarFunctions::getHeight(DOM::HTMLElement &element)
{
  int height = 0;
  DOM::NodeList children = element.childNodes();
  for(uint i = 0; i < children.length(); i++){
    DOM::HTMLElement node = static_cast<DOM::HTMLElement>(children.item(i));
    DOM::CSSStyleDeclaration style = node.style();
    
    DOM::DOMString css_height = style.getPropertyValue("height");
    if(!css_height.isNull()){
      height += css_height.string().left(css_height.string().length() - 2).toInt();
    }
    else{
      int h = 0;
      if(!node.isNull()){
        h = node.getRect().height();
      }
      
      DOM::DOMString display = style.getPropertyValue("display");
      if(display == "none"){  
        h = 0;
      }
      else if(h == 0){
        h = 20;
      }

      height += h;
    }
  }

  return height;
}

#include "metabarfunctions.moc"
