/***************************************************************************
                          kresult.cpp  -  description
                             -------------------
    begin                : Sat Jan 5 2002
    copyright            : (C) 2002 by Arnold Kraschinski
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

#include "kresult.h"
#include <qlabel.h>
#include <qtable.h>
#include <qlistview.h>
#include <qvaluestack.h>

#include <klocale.h>

KResult::KResult(KVerbosUser* pU, QWidget* parent, const char* name, bool modal, WFlags fl )
: QResult(parent, name, modal, fl)
{
	int sessions;
	QString date[10];
	int result[10][2];
	pU->getResults(sessions, date, result);
	QString s;
	s.setNum(sessions);
	TextLabelSessions->setText(s);
	TextLabelUser->setText(pU->getName());
	TableResult->horizontalHeader()->setLabel(1, i18n("trained"));
	TableResult->horizontalHeader()->setLabel(2, i18n("correct in %"));
	TableResult->horizontalHeader()->setLabel(0, i18n("date"));	
	for (int row=0; row<10; row++)
	{
		TableResult->setText(row, 0, date[row]);
		TableResult->setText(row, 1, s.setNum(result[row][1])+i18n(" verbs"));
		TableResult->setText(row, 2, s.setNum(result[row][0])+"%");
	};
	TableResult->adjustColumn(0);
	TableResult->adjustColumn(1);
	TableResult->adjustColumn(2);

	// Die Spaltenbreiten anpassen
	ListViewVerben->setColumnWidthMode(0, QListView::Maximum);
	ListViewVerben->setColumnWidthMode(1, QListView::Maximum);

	verbenListe* pVL = pU->getListe();
	verbenListe::Iterator it;
	QValueStack<QString> right;
	QValueStack<QString> wrong;
	for (it = pVL->begin(); it!=pVL->end(); ++it)
	{
		if ((*it).status == RICHTIGGEMACHT)
			right.push((*it).verb);
		else
		if ((*it).status == FEHLERHAFT)
			wrong.push((*it).verb);
	};
	while (!right.isEmpty() && !wrong.isEmpty())
	{
		QListViewItem* pItem;
		pItem = new QListViewItem( ListViewVerben, right.pop(), wrong.pop());
		ListViewVerben->insertItem(pItem);
	};
	while (!right.isEmpty())
	{
		QListViewItem* pItem;
		pItem = new QListViewItem( ListViewVerben, right.pop(), "");
		ListViewVerben->insertItem(pItem);
	};
	while (!wrong.isEmpty())
	{
		QListViewItem* pItem;
		pItem = new QListViewItem( ListViewVerben, "", wrong.pop());
		ListViewVerben->insertItem(pItem);
	};
}

KResult::~KResult()
{
}

void KResult::slotDlgEnde()
{
	done(0);
}


