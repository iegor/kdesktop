/*
 *  This file is part of the KDE libraries
 *  Copyright (c) 2001 Michael Goffioul <kdeprint@swing.be>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation.
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
 **/

#ifndef KMWLOCAL_H
#define KMWLOCAL_H

#include "kmwizardpage.h"

#include <qstringlist.h>

class QLineEdit;
class KListView;
class QListViewItem;

class KMWLocal : public KMWizardPage
{
	Q_OBJECT
public:
	KMWLocal(QWidget *parent = 0, const char *name = 0);

	bool isValid(QString&);
	void updatePrinter(KMPrinter*);
	void initPrinter(KMPrinter*);

protected slots:
	void slotPortSelected(QListViewItem*);
	void slotTextChanged( const QString& );

protected:
	void initialize();
	QListViewItem* lookForItem( const QString& );

protected:
	KListView	*m_ports;
	QLineEdit	*m_localuri;
	QStringList	m_uris;
	QListViewItem	*m_parents[4];
	bool		m_initialized;
	bool m_block;
};

#endif
