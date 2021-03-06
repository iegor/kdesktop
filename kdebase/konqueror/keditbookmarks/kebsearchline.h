/* This file is part of the KDE project
   Copyright (C) 2005 Daniel Teske <teske@squorn.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef __kebsearchline_h
#define __kebsearchline_h

#include <klistviewsearchline.h>
#include <qobject.h>

class KEBSearchLine : public KListViewSearchLine
{
    Q_OBJECT
public:
    KEBSearchLine(QWidget *parent = 0, KListView *listView = 0, const char *name = 0);

    KEBSearchLine(QWidget *parent, const char *name);

    virtual ~KEBSearchLine();

    enum modes { EXACTLY, AND, OR } mmode;
    modes mode();
    void setMode(modes m);
    virtual void updateSearch(const QString &s = QString::null);

signals:
    void searchUpdated();

protected:

    virtual bool itemMatches(const QListViewItem *item, const QString &s) const;

private:
    mutable QString lastpattern; // what was cached
    mutable QStringList splitted; // cache of the splitted string
};

#endif
