/***************************************************************************
                          siglistview.cpp  -  description
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

#include "siglistview.h"
#include "siglistview.moc"

#include "siglistviewitem.h"

#include <kstandarddirs.h>
#include <klocale.h>

SigListView *SigListView::listView = 0;

////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

SigListView *SigListView::instance(QWidget *parent, const char *name)
{
    if(!listView)
	listView = new SigListView(parent, name);
    return(listView);
}

void SigListView::load()
{
    if(file.open(IO_ReadOnly) && doc.setContent(&file)) {

	// find the root element
	QDomNodeList topLevelElements = doc.childNodes();
	uint i = 0;
	while(topLevelElements.item(i).toElement().tagName() != "SigML" && i < topLevelElements.count())
	    i++;

	if(i < topLevelElements.count())
	    // if we didn't hit the end of the list
	    rootElement = topLevelElements.item(i).toElement();
	else {
	    // if we didn't find the root element, create one
	    rootElement = doc.createElement("SigML");
	    doc.appendChild(rootElement);
	}

	QDomNodeList signatures = doc.elementsByTagName("signature");
	for(i = 0; i < signatures.count(); i++)
	    (void) new SigListViewItem(this, doc, signatures.item(i).toElement());

	file.close();
    }
    // if the document could not be opened or setData failed, create the document framework
    else {
	rootElement = doc.createElement("SigML");
	doc.appendChild(rootElement);
    }
}

void SigListView::save()
{
    QListViewItemIterator it(this);
    while(it.current()) {
	SigListViewItem *item = dynamic_cast<SigListViewItem *>(it.current());
	if(item)
	    item->render();
	it++;
    }

    if(file.open(IO_WriteOnly)) {
	QTextStream stream(&file);
        stream << doc;
        file.close();
    }
}

SigListViewItem *SigListView::createItem()
{
    QDomElement element = doc.createElement("signature");
    rootElement.appendChild(element);

    SigListViewItem *item = new SigListViewItem(this, doc, element);
    return(item);
}

SigListViewItem *SigListView::currentItem()
{
    return(dynamic_cast<SigListViewItem *>(KListView::currentItem()));
}

const SigListViewItem *SigListView::currentItem() const
{
    return(dynamic_cast<SigListViewItem *>(KListView::currentItem()));
}

////////////////////////////////////////////////////////////////////////////////
// protected members
////////////////////////////////////////////////////////////////////////////////

SigListView::SigListView(QWidget *parent, const char *name) : KListView(parent, name)
{
    addColumn(i18n("Signatures"));

    QString dir = KGlobal::dirs()->saveLocation("appdata");
    if(!dir.isNull())
        file.setName(dir + "sigs.sigml");
    load();
}

SigListView::~SigListView()
{

}
