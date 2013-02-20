   /*
 *  This file is part of the KDE project
 *  Copyright (C) 2002 Ian Reinhart Geiser <geiseri@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2.0 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 *  $Id: sidebarwidget.cpp 465369 2005-09-29 14:33:08Z mueller $
 */
 
 #include "sidebarwidget.h"  
 #include <qscrollview.h>
   
   sidebarwidget::sidebarwidget(QWidget* parent, const char* name) :
   	sidebarwidgetbase(parent,name)
   {
   	view = new QVBox(reportGrid->viewport());
	reportGrid->addChild(view);
	reportGrid->setResizePolicy(QScrollView::AutoOneFit);
   }
   
    sidebarwidget::~sidebarwidget()
    {
    
    }
    
    void sidebarwidget::addWidget(QWidget *w, const QString &s)
    {
    	w->setFixedWidth(reportGrid->visibleWidth ());
    }
    
    QWidget *sidebarwidget::viewport()
    {
    	return view;
    }

#include "sidebarwidget.moc"
    
