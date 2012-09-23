	/*

	Copyright (C) 1998-1999 Stefan Westerfeld
                            stefan@space.twc.de

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

    */

#ifndef __PORTPOSDLG_H_
#define __PORTPOSDLG_H_

#include "structure.h"
#include "structureport.h"
#include <qdialog.h>
#include <qlistbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <vector>

class PortPosDlg :public QDialog {
	Q_OBJECT
protected:
	Structure *structure;
	QListBox *listbox;

	std::vector<StructurePort *> listports;
public:
	PortPosDlg(QWidget *parent, Structure *structure);

	void update();
public slots:
	void raise();
	void lower();
	void rename();
	void help();
};

#endif
