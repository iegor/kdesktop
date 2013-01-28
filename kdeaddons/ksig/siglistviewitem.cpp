/***************************************************************************
                          siglistviewitem.cpp  -  description
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

#include "siglistviewitem.h"

#include <klocale.h>
#include <kdebug.h>

#include <qregexp.h>

////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

SigListViewItem::~SigListViewItem()
{
    // remove the element from the tree
    element.parentNode().removeChild(element);
}

QString SigListViewItem::text() const
{
    return(elementText);
}

void SigListViewItem::setText(const QString &t)
{
    if(t != elementText) {
	elementText = t;
	dirty = true;
	refreshText();
    }
}

void SigListViewItem::refreshText()
{
    if(!text().isEmpty())
	KListViewItem::setText(0, text().simplifyWhiteSpace());
    else
	KListViewItem::setText(0, emptySigString);
}

void SigListViewItem::nodeToText(const QDomNode &n, QString &s)
{
    QDomNodeList children = n.childNodes();

    for(uint i = 0; i < children.count(); i++) {
	if(children.item(i).isText())
	    s.append(children.item(i).toText().data());
	else {
	    nodeToText(children.item(i), s);
	    if(children.item(i).isElement() && children.item(i).toElement().tagName() == "p") {
		s.append("\n");
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////////////
// private members
////////////////////////////////////////////////////////////////////////////////

SigListViewItem::SigListViewItem(QListView *parent, QDomDocument document, QDomElement signatureElement) : KListViewItem(parent)
{
    emptySigString = i18n("<empty signature>");

    doc = document;
    element = signatureElement;
    nodeToText(element, elementText);
    elementText.replace(QRegExp("\n$"), "");
    
    dirty = false;
    refreshText();
}

void SigListViewItem::render()
{
    if(dirty) {
	QDomNodeList children = element.childNodes();

	while(!element.firstChild().isNull()) 
	    element.removeChild(element.firstChild());

	// create new children
	QStringList lines = QStringList::split("\n", elementText, true);
	
	for(QStringList::Iterator it = lines.begin(); it != lines.end(); it++) {
	    QDomElement p = doc.createElement("p");
	    element.appendChild(p);
	    p.appendChild(doc.createTextNode(*it));
	}
	dirty = false;
    }
}
