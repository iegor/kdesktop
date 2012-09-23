/*
   Copyright (c) 1999-2000 Stefan Schimanski <1Stein@gmx.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef PREF_H_INCLUDED
#define PREF_H_INCLUDED

#include <noatun/pref.h>

class KListBox;
class QLabel;

class KaimanPrefDlg : public CModule
{
    Q_OBJECT
    public:
	KaimanPrefDlg( QObject *parent );
	virtual ~KaimanPrefDlg();

	virtual void save();
	virtual void reopen();
	
    public slots:
	void setSkin( QString skin );
	QString skin();

    private:
	void readSkinDir( const QString &dir );

	KListBox *_skinList;
};

#endif

