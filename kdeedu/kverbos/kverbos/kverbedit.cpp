/***************************************************************************
                          kverbedit.cpp  -  description
                             -------------------
    begin                : Sat Dec 15 2001
    copyright            : (C) 2001 by Arnold Kraschinski
    email                : arnold.k67@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qlistview.h>

#include "kverbedit.h"
#include "kerfassen.h"

/** An die Dialogklasse wird direkt der Zeiger auf die Verbliste bergeben, damit
 *  die Liste der Verben bearbeitet werden kann. Zu Beginn mssen alle Verben
 *  aus der Liste in eine dialogeigene Liste und die QListView bertragen werden.
 */
KVerbEdit::KVerbEdit(spanishVerbList& l, QWidget* parent /*= 0*/, const char* name /*= 0*/, bool modal /*= FALSE*/, WFlags fl /*= 0*/)
: QVerbEdit(parent, name, modal, fl), list(l)
{
	// Die Spaltenbreiten anpassen
	ListView1->setColumnWidthMode(0, QListView::Manual);
	ListView1->setColumnWidthMode(1, QListView::Manual);
	int br = width() / 20 * 9;
	QString brstr;
	brstr.setNum(br);
	ListView1->setColumnWidth(0, br);
	ListView1->setColumnWidth(1, br);
	// Eine Kopie der Liste erstellen und die Liste in die Ansicht bringen
	changed = false;
	listToView();
}

KVerbEdit::~KVerbEdit()
{
}

// kopiert die Liste in die ListView.
void KVerbEdit::listToView()
{
	ListView1->clear();
	if (!list.isEmpty())
	{
		spanishVerbList::Iterator it;
		for (it = list.begin(); it != list.end(); ++it)
		{
			QListViewItem* pItem;
			pItem = new QListViewItem( ListView1, (*it).getSpanish(), (*it).getForeign());
			ListView1->insertItem(pItem);
		};
	};
}


// !!SLOTS!! Ab hier werden die slots der Klasse implementiert. !!SLOTS!!

// Wird durch den OK-Button aufgerufen und beendet den Dialog.
// �derungen werden bernommen.
void KVerbEdit::slotDlgEnde()
{
	done(0);
}

// Wird durch den Cancel-Button aufgerufen und beendet den Dialog ohne
// �derungen zu bernehmen.
void KVerbEdit::slotCancel()
{
	done(0);
}

// Ein Verb in der Liste bearbeiten oder neue Verben in die Liste aufnehmen.
// Feststellen welches Verb in der Liste markiert ist und dieses Verb aus der Liste suchen.
// An den Dialog wird ein Listeniterator bergeben.
void KVerbEdit::slotEditVerb()
{
	QListViewItem* pi = ListView1->selectedItem();
	// it's a sorted list, so binary search works.
	// v is not a real verb it is only used for the search. v doesn't have any forms but
	// that isn't important to compare.
	int m = -1;
	if (pi != 0)
	{
		verbSpanish v;
		v.setSpanish(pi->text(0));
		v.setForeign(pi->text(1));
		int left = 0;
		int right = list.count() -1;
		m = 0;
		while (!(v == list[m])&&(left <= right))
		{
			m = (left + right) / 2;
			if (v < list[m])
			{
				right = m - 1;
			}
			else
			{
				left = m + 1;
			};
		};
		// It shouldn't occur that the verb doesn't exist.
		if (!(v == list[m]))
			m = -1;
	};
	KErfassen* pDlgErfassen = new KErfassen(list, m, this, "erfassen", true);
	pDlgErfassen->exec();
	changed = changed || pDlgErfassen->wasChanged();
	if (pDlgErfassen->wasChanged())
	{
		list = pDlgErfassen->getList();
		listToView();
	};
	delete pDlgErfassen;
	pDlgErfassen = 0;
}

// Das in der Listenansicht ausgew�lte Verb wird gel�cht.
void KVerbEdit::slotDelete()
{
	QListViewItem* pi = ListView1->selectedItem();
	if (0 != pi)
	{
		// Das Listenelement hersuchen
		bool found = false;
		spanishVerbList::Iterator it = list.begin();
		while ((it != list.end()) && (!found))
		{
			if (((*it).getSpanish() == pi->text(0)) && ((*it).getForeign() == pi->text(1)))
			{
				found = true;
			}
			else
				it++;
		};
		if (found)
		{
			list.remove(it);
			// Das Verb auch aus der Listenansicht streichen
			ListView1->takeItem(pi);
			changed = true;
		};
	};
}
