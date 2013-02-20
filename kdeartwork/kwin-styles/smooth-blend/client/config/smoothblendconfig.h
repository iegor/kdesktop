//////////////////////////////////////////////////////////////////////////////
// smoothblendconfig.h
// -------------------
// Config module for Smooth Blend window decoration
// -------------------
// Copyright (c) 2005 Ryan Nickell <p0z3r@users.sourceforge.net>
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef SMOOTHBLENDCONFIG_H
#define SMOOTHBLENDCONFIG_H

#include <qobject.h>

class KConfig;
class ConfigDialog;

class smoothblendConfig : public QObject {
    Q_OBJECT
public:
    smoothblendConfig(KConfig* config, QWidget* parent);
    ~smoothblendConfig();

signals:
    void changed();

public slots:
    void load(KConfig*);
    void save(KConfig*);
    void defaults();

protected slots:
    void selectionChanged(int);

private:
    KConfig *config_;
    ConfigDialog *dialog_;
};

#endif // SMOOTHBLENDCONFIG_H
