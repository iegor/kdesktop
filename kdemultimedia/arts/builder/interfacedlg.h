	/*

	Copyright (C) 1998-2001 Stefan Westerfeld
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

#ifndef __INTERFACEDLG_H_
#define __INTERFACEDLG_H_

#include "structure.h"
#include "structureport.h"
#include <qdialog.h>
#include <qlistbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <vector>

class InterfaceDlg :public QDialog {
	Q_OBJECT
protected:
	QListBox *listbox;

public:
	InterfaceDlg(QWidget *parent);

	std::string interfaceName();
	void update(const std::string& interface, const std::string& indent);
	void update();

public slots:
	void raise();
	void lower();
	void rename();
	void help();
};

#endif
