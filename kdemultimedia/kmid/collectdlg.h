/**************************************************************************

    collectdlg.h  - The collection manager dialog
    Copyright (C) 1998  Antonio Larrosa Jimenez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    Send comments and bug fixes to larrosa@kde.org
    or to Antonio Larrosa, Rio Arnoya, 10 5B, 29006 Malaga, Spain

***************************************************************************/
#ifndef _collectdlg_h_
#define _collectdlg_h_

#include <qdialog.h>
#include "songlist.h"
#include "slman.h"

class QLabel;
class KPushButton;
class QPushButton;
class QListBox;
class KURL;
class CollectionDialog : public QDialog
{
	Q_OBJECT
private:
	SLManager *slman;
	SongList *currentsl;

protected:

	virtual void resizeEvent(QResizeEvent *qre);
	void fillInSongList(void); //clear the songs list and insert in it
				// the songs in currentsl
	void addSong(const KURL &);
public:

	CollectionDialog(SLManager *slm,int selc,QWidget *parent,const char *name);

public slots:
	void collectionselected(int idx);
	void songselected(int idx);
	void newCollection();
	void copyCollection();
	void deleteCollection();
	void changeCollectionName(int idx);
	void addSong();
	void removeSong();

private:
QLabel *label;
QLabel *label2;
KPushButton *ok;
KPushButton *cancel;
QListBox *collections;
QListBox *songs;
QPushButton *newC;
QPushButton *copyC;
QPushButton *deleteC;
QPushButton *addS;
QPushButton *delS;


public:
static int selectedC;
static int selectedS;

};

#endif
