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

#include "serviceloader.h"

#include <qdir.h>
#include <qvaluelist.h>
#include <qptrlist.h>

#include <dcopclient.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kactioncollection.h>
#include <kiconloader.h>
#include <kaction.h>
#include <kshortcut.h>
#include <ksimpleconfig.h>
#include <kglobal.h>
#include <kstandarddirs.h>


ServiceLoader::ServiceLoader(QWidget *parent, const char *name) : QObject(parent, name)
{  
  popups.setAutoDelete(true);
}

ServiceLoader::~ServiceLoader()
{
}

void ServiceLoader::loadServices(const KFileItem item, DOM::DOMString &html, int &count)
{
  popups.clear();
  
  KURL url = item.url();
  QString mimeType = item.mimetype();
  QString mimeGroup = mimeType.left(mimeType.find('/'));
  
  urlList.clear();
  urlList.append(url);
  
  QStringList dirs = KGlobal::dirs()->findDirs( "data", "konqueror/servicemenus/" );
  KConfig config("metabarrc", true, false);
  config.setGroup("General");
  int maxActions = config.readNumEntry("MaxActions");
  bool matchAll = false; // config.readBoolEntry("MatchAll");
  
  int id = 0;
  QString idString;
  
  for(QStringList::Iterator dit = dirs.begin(); dit != dirs.end(); ++dit){
    idString.setNum(id);
  
    QDir dir(*dit);
    QStringList entries = dir.entryList("*.desktop", QDir::Files);
    
    for(QStringList::Iterator eit = entries.begin(); eit != entries.end(); ++eit){
      KSimpleConfig cfg( *dit + *eit, true );
      cfg.setDesktopGroup();
      
      if(cfg.hasKey("X-KDE-ShowIfRunning" )){
        const QString app = cfg.readEntry( "X-KDE-ShowIfRunning" );
        if(!kapp->dcopClient()->isApplicationRegistered(app.utf8())){
          continue;
        }
      }

      if(cfg.hasKey("X-KDE-Protocol")){
        const QString protocol = cfg.readEntry( "X-KDE-Protocol" );
        if(protocol != url.protocol()){
          continue;
        }
      }
      
      else if(url.protocol() == "trash"){
        continue;
      }

      if(cfg.hasKey("X-KDE-Require")){
        const QStringList capabilities = cfg.readListEntry( "X-KDE-Require" );
        if (capabilities.contains( "Write" )){
          continue;
        }
      }

      if ( cfg.hasKey( "Actions" ) && cfg.hasKey( "ServiceTypes" ) ){
          const QStringList types = cfg.readListEntry( "ServiceTypes" );
          const QStringList excludeTypes = cfg.readListEntry( "ExcludeServiceTypes" );
          bool ok = false;

          for (QStringList::ConstIterator it = types.begin(); it != types.end() && !ok; ++it){
            bool checkTheMimetypes = false;
          
            if(matchAll){
            // first check if we have an all mimetype
              if (*it == "all/all" || *it == "allfiles"){
                checkTheMimetypes = true;
              }
  
              // next, do we match all files?
              if (!ok && !item.isDir() && *it == "all/allfiles"){
                checkTheMimetypes = true;
              }
            }

            // if we have a mimetype, see if we have an exact or a type globbed match
            if(!ok && (!mimeType.isEmpty() && *it == mimeType) ||
              (!mimeGroup.isEmpty() && ((*it).right(1) == "*" && (*it).left((*it).find('/')) == mimeGroup)))
            {
              checkTheMimetypes = true;
            }

            if(checkTheMimetypes){
              ok = true;
              
              for(QStringList::ConstIterator itex = excludeTypes.begin(); itex != excludeTypes.end(); ++itex){
                if( ((*itex).right(1) == "*" && (*itex).left((*itex).find('/')) == mimeGroup) ||
                    ((*itex) == mimeType))
                {
                  ok = false;
                  break;
                }
              }
            }
          }
        if (ok){
          const QString priority = cfg.readEntry("X-KDE-Priority");
          const QString submenuName = cfg.readEntry( "X-KDE-Submenu" );
          bool usePopup = false;
          KPopupMenu *popup;
          
          if(!submenuName.isEmpty()){
            usePopup = true;
            
            if(popups[submenuName]){
              popup = popups[submenuName];
            }
            else{              
              MetabarWidget::addEntry(html, submenuName, "servicepopup://" + idString, "1rightarrow", "popup" + idString, count < maxActions ? QString::null : QString("hiddenaction"), count >= maxActions);
              
              popup = new KPopupMenu();
              popups.insert(idString, popup);
              
              count++;
            }
          }
          
          QValueList<KDEDesktopMimeType::Service> list = KDEDesktopMimeType::userDefinedServices( *dit + *eit, url.isLocalFile());
          
          for (QValueList<KDEDesktopMimeType::Service>::iterator it = list.begin(); it != list.end(); ++it){
          
            if(usePopup){
              KAction *action = new KAction((*it).m_strName, (*it).m_strIcon, KShortcut(), this, SLOT(runAction()), popup, idString.utf8());
              action->plug(popup);
            }
            else{
              MetabarWidget::addEntry(html, (*it).m_strName, "service://" + idString, (*it).m_strIcon, QString::null, count < maxActions ? QString::null : QString("hiddenaction"), count >= maxActions);
              count++;
            }
            
            services.insert(idString, *it);
            id++;
            idString.setNum(id);
          }
        }
      }
    }
  }
}

void ServiceLoader::runAction()
{    
  KDEDesktopMimeType::Service s = services[sender()->name()];
  if(!s.isEmpty()){
    KDEDesktopMimeType::executeService(urlList, s);
  }
}

void ServiceLoader::runAction(const QString& name)
{
  KDEDesktopMimeType::Service s = services[name];
  if(!s.isEmpty()){
    KDEDesktopMimeType::executeService(urlList, s);
  }
}

void ServiceLoader::showPopup(const QString &popup, const QPoint &point)
{
  KPopupMenu *p = popups[popup];
  if(p){
    p->exec(point);
  }
}

#include "serviceloader.moc"
