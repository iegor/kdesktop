/*
    This file is part of KOrganizer.
    Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/
#ifndef KORG_PROJECTVIEW_H
#define KORG_PROJECTVIEW_H
// $Id: projectview.h 437921 2005-07-23 15:44:21Z kainhofe $

#include <korganizer/part.h>
#include <korganizer/calendarviewbase.h>

class ProjectView : public KOrg::Part {
    Q_OBJECT
  public:
    ProjectView(KOrg::MainWindow *, const char *);
    ~ProjectView();
    
    QString info();
    QString shortInfo();

  private slots:
    void showView();

  private:
    KOrg::BaseView *mView;
};

#endif
