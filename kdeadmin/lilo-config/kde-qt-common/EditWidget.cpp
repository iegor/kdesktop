/* EditWidget.cpp
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
#include "EditWidget.moc"
#include "ui.h"
EditWidget::EditWidget(QString const label, QString const text, bool isFile, QWidget *parent, const char *name, WFlags f, bool allowLines):QHBox(allowLines, parent, name, f)
{
	setMargin(SPACE_MARGIN);
	setSpacing(SPACE_INSIDE);
	lbl=new QLabel(label, this);
	setStretchFactor(lbl, 0);
	line=new QLineEdit(text, this);
	setStretchFactor(line, 1);
	connect(line, SIGNAL(textChanged(const QString &)), SIGNAL(textChanged(const QString &)));
	connect(line, SIGNAL(returnPressed()), SIGNAL(returnPressed()));
	lbl->setBuddy(line);
	if(isFile) {
		select=new QPushButton(_("Select..."), this);
		connect(select, SIGNAL(clicked()), SLOT(selectFileClicked()));
		select->resize(select->minimumSizeHint());
		setStretchFactor(select, 0);
	} else
		select=0;
}
void EditWidget::selectFileClicked()
{
    QString filename=FileDlg::getOpenFileName(QString::null, QString::null, this);
    if(!filename.isEmpty())
	line->setText(filename);
}
QSize EditWidget::sizeHint() const
{
	int w=2*SPACE_MARGIN+lbl->sizeHint().width()+SPACE_INSIDE+line->sizeHint().width();
	int h=lbl->sizeHint().height();
	if(h<line->sizeHint().height())
		h=line->sizeHint().height();
	if(select!=0) {
		w+=SPACE_INSIDE+select->sizeHint().width();
		if(h<select->sizeHint().height())
			h=select->sizeHint().height();
	}
	return QSize(w, h);
}
QSize EditWidget::minimumSizeHint() const
{
	int w=2*SPACE_MARGIN+lbl->minimumSizeHint().width()+SPACE_INSIDE+line->minimumSizeHint().width();
	int h=lbl->minimumSizeHint().height();
	if(h<line->minimumSizeHint().height())
		h=line->minimumSizeHint().height();
	if(select!=0) {
		w+=SPACE_INSIDE+select->minimumSizeHint().width();
		if(h<select->minimumSizeHint().height())
			h=select->minimumSizeHint().height();
	}
	return QSize(w, h);
}
