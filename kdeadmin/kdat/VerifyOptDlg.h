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

#ifndef _VerifyOptDlg_h_
#define _VerifyOptDlg_h_

#include <qdialog.h>
#include <qstring.h>

class QLineEdit;

/**
 * @short Display/edit the information for performing a verify/restore operation.
 *
 * The verify and restore operations are similiar enough that their options
 * have been combined into a single dialog.
 */
class VerifyOptDlg : public QDialog {
    Q_OBJECT
    bool       _restore;
    QString    _workingDir;
    QLineEdit* _entry;
private slots:
    void okClicked();
    void slotBrowse();
public:
    /**
     * Create a new verify/restore options dialog.
     *
     * @param def     The default working directory.
     * @param files   The list of files that will be verified/restored.
     * @param restore TRUE means we are doing a restore, FALSE means we are doing a verify.
     * @param parent  The parent widget for the dialog.
     * @param name    The name for the dialog.
     */
    VerifyOptDlg( const QString & def, const QStringList& files, bool restore = FALSE, QWidget* parent = 0, const char* name = 0 );

    /**
     * Destroy the verify/restore options dialog.
     */
    ~VerifyOptDlg();

    /**
     * Get thre working directory entered by the user.
     *
     * @return The working directory for the verify/restore operation.
     */
    QString getWorkingDirectory();
};

#endif
