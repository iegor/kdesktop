/* images.h
**
** Copyright (C) 2000,2001 by Bernhard Rosenkraenzer
**
** Contributions by M. Laurent and W. Bastian.
**
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/
#ifndef _IMAGES_H_
#define _IMAGES_H_ 1
#include <lilo.h>
#include <qwidget.h>
#include <qmultilineedit.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qvbox.h>
#include "EditWidget.h"
class Images:public QWidget
{
	Q_OBJECT
public:
	Images(liloconf *l=0, QWidget *parent=0, const char *name=0);
	~Images();
	void setCfg(liloconf *l) { lilo=l; };
    void makeReadOnly();
public slots:
	void update();
	void saveChanges();
private slots:
	void probeClicked();
	void dfltClicked();
	void detailsClicked();
	void checkClicked();
	void addKrnlClicked();
	void addOSClicked();
	void removeClicked();
	void imageSelected(const QString &i);
signals:
	void configChanged();
private:
	liloconf	*lilo;
	QString		previous;
	QString		current;
	QHBoxLayout	*layout;
	QListBox	*images;
	QVBox		*parameters;
	EditWidget	*image;
	EditWidget	*label;
	EditWidget	*root;
	EditWidget	*initrd;
	EditWidget	*append;
	QVBox		*actions;
	QPushButton	*dflt;
	QPushButton	*details;
	QPushButton	*probe;
	QPushButton	*check;
	QPushButton	*addKrnl;
	QPushButton	*addOS;
	QPushButton	*remove;
};
#endif
