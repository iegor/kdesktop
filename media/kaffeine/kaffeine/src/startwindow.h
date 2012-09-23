/*
 * startwindow.h
 *
 * Copyright (C) 2004-2005 Jürgen Kofler <kaffeine@gmx.net>
 * Copyright (C) 2006-2007 Christophe Thommeret <hftom@free.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qtoolbutton.h>
#include <qstring.h>
#include <qptrlist.h>
#include <qlabel.h>

class MToolButton : public QToolButton
{
	Q_OBJECT

public:
	MToolButton( QWidget *parent, const QString& targ );
	~MToolButton();

	QString target;

private slots:
	void mClicked();

signals:
	void executed( const QString& );
};



class SLabel : public QLabel
{
	Q_OBJECT

public:
	SLabel( QWidget *parent );
	~SLabel() {}

protected:
	void paintEvent( QPaintEvent * );
};



class StartWindow : public QWidget
{
	Q_OBJECT

public:
	StartWindow( QWidget *parent = 0, const char *name = 0 );
	virtual ~StartWindow();

	void registerTarget( const QString& uiName, const QString& pixName, const QString& targetName );
	void unregisterTarget( const QString& targetName );
	void execTarget( int index );

protected:
	virtual void resizeEvent(QResizeEvent*);

private:
	void arrangeButtons( QSize e );

	QPtrList<MToolButton> buttons;
	QWidget *panel;
	SLabel *label1, *label2;

signals:
	void execTarget( const QString& );
};

#endif /* STARTWINDOW_H */
