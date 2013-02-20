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
 
#ifndef _DEFAULT_PLUGIN_H_
#define _DEFAULT_PLUGIN_H_

#include "protocolplugin.h"
#include "serviceloader.h"

#include <kio/previewjob.h>

#include <qdict.h>
#include <qmap.h>

class DefaultPlugin : public ProtocolPlugin
{
  Q_OBJECT

  public:
    DefaultPlugin(KHTMLPart* html, MetabarFunctions *functions, const char *name = 0);
    ~DefaultPlugin();
    
    void deactivate();
    bool handleRequest(const KURL &url);
    
  protected:    
    void killJobs();
    
    void loadActions(DOM::HTMLElement node);
    void loadApplications(DOM::HTMLElement node);
    void loadInformation(DOM::HTMLElement node);
    void loadPreview(DOM::HTMLElement node);
    void loadBookmarks(DOM::HTMLElement node);
      
  private:
    QMap<int,KService::Ptr> runMap;
    KIO::PreviewJob *preview_job;
    
    ServiceLoader *services;
    
  private slots:
    void slotSetPreview(const KFileItem*, const QPixmap&);
    void slotPreviewFailed(const KFileItem *item);
    void slotJobFinished(KIO::Job *item);
};

#endif
