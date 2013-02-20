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
 
#ifndef METABAR_FUNCTIONS_H
#define METABAR_FUNCTIONS_H

#include <dom_string.h>

#include <qtimer.h>
#include <qmap.h>

#include <kurl.h>
#include <khtml_part.h>

class MetabarFunctions : public QObject
{
  Q_OBJECT
  
  public:
    MetabarFunctions(KHTMLPart *html, QObject *parent = 0, const char* name=0);
    ~MetabarFunctions();
    
    void toggle(DOM::DOMString item);
    void adjustSize(DOM::DOMString item);
    void hide(DOM::DOMString item);
    void show(DOM::DOMString item);
    void handleRequest(const KURL &url);
    
  protected:
    KHTMLPart *m_html;
    
  private:
    QTimer *timer;
    
    QMap<QString, int> resizeMap;
    int getHeight(DOM::HTMLElement &element);
    
  private slots:
    void animate();
};

#endif
