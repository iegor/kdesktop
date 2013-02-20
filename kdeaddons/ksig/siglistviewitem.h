/***************************************************************************
                          siglistviewitem.h  -  description
                             -------------------
    begin                : Fri Jul 12 2002
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

#ifndef SIGLISTVIEWITEM_H
#define SIGLISTVIEWITEM_H

#include "siglistview.h"

#include <qdom.h>

class SigListViewItem : public KListViewItem
{
    friend class SigListView;
public:
    virtual ~SigListViewItem();
    void setText(const QString &t);
    QString text() const;
    void refreshText();

    static void nodeToText(const QDomNode &n, QString &s);

private:
    /**
     * The constructor is private because all SigListViewItems should be
     * created using SigListView::createItem().  This is accessible to
     * SigListView because SigListView is a friend class.
     */
    SigListViewItem(QListView *parent, QDomDocument document, QDomElement signatureElement);

    /** 
     * Renders the data back to the DOM element. 
     */
    void render();

    QString emptySigString;

    QDomDocument doc;
    QDomElement element;
    QString elementText;
    bool dirty;
};

#endif
