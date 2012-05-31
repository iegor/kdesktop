	/*

	Copyright (C) 1998 Stefan Westerfeld
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

#ifndef __EXECDLG_H_
#define __EXECDLG_H_

#include "structure.h"
#include <qdialog.h>
#include <qtimer.h>
#include <qlabel.h>
#include <qscrollbar.h>
#include <qlayout.h>

class ExecDlg :public QDialog {
	Q_OBJECT
public:
	QTimer *cpuusagetimer;
	QLabel *cpuusagelabel;
	QVBoxLayout *mainlayout,*sliderlayout;
	ExecutableStructure *structure;

	void start();
	void done(int r);
	ExecDlg(QWidget *parent, ExecutableStructure *structure);
			
protected slots:
	void updateCpuUsage();
	void guiServerTick();
	void saveSession();
	void help();

signals:
	void ready();
};

#endif
