/***************************************************************************
                          siglistview.h  -  description
                             -------------------
    begin                : Fri Jul 19 2002
    copyright            : (C) 2002 by Scott Wheeler
    email                : wheeler@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SIGLISTVIEW_H
#define SIGLISTVIEW_H

#include <klistview.h>
#include <kdebug.h>

#include <qfile.h>
#include <qdom.h>

class SigListViewItem;

/** 
 * SigListView is implemented as a singleton, and as such has a private
 * constructor.  You can access the instance of SigListView through the
 * instance method.  This is implemented as a singleton not to provide
 * global access, but because things will break horribly were there to
 * be two instances of this widget created.
*/

class SigListView : public KListView  
{
    Q_OBJECT
public: 
    static SigListView *instance(QWidget *parent = 0, const char *name = 0);

    void load();
    void save();

    /** 
     * This method should be used for all creation of SigListViewItems. 
     */
    SigListViewItem *createItem();
    SigListViewItem *currentItem();
    const SigListViewItem *currentItem() const;

public slots:
    /**
     * Override the destructive default.
     */
    void clear() {}

protected:
    SigListView(QWidget *parent = 0, const char *name = 0);
    virtual ~SigListView();

private:
    static SigListView *listView;
    
    QFile file;
    QDomDocument doc;
    QDomElement rootElement;
};

#endif
