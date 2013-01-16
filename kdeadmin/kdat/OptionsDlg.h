// KDat - a tar-based DAT archiver
// Copyright (C) 1998-2000  Sean Vyain, svyain@mail.tds.net
// Copyright (C) 2001-2002  Lawrence Widman, kdat@cardiothink.com
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef _OptionsDlg_h_
#define _OptionsDlg_h_

#include <kdialogbase.h>

class QPushButton;
class OptionsDlgWidget;

/**
 * @short Display/edit user preferences.
 */
class OptionsDlg : public KDialogBase {
    Q_OBJECT
private slots:
    void slotOK();
    void slotApply();
    void slotCancel();
    void slotChanged();
public:
    /**
     * Create a new options dialog.
     *
     * @param parent The parent widget of the dialog.
     * @param name   The name of the dialog.
     */
    OptionsDlg( QWidget* parent = 0, const char* name = 0 );

    /**
     * Destroy the options dialog.
     */
    ~OptionsDlg();
private:
    bool configChanged;
	OptionsDlgWidget *_baseWidget;
    QPushButton* apply;
};

#endif
