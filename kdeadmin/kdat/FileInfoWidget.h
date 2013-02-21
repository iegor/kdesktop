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

#ifndef _FileInfoWidget_h_
#define _FileInfoWidget_h_

#include <qwidget.h>

class QLabel;

/**
 * @short Display information about a local file.
 */
class FileInfoWidget : public QWidget {
    Q_OBJECT
    QLabel* _fileName;
    QLabel* _ctime;
    QLabel* _mtime;
    QLabel* _atime;
    QLabel* _size;
    QLabel* _owner;
    QLabel* _group;
public:
    /**
     * Create a new file info widget.
     *
     * @param parent The parent widget.
     * @param name   The name of this widget.
     */
    FileInfoWidget( QWidget* parent = 0, const char* name = 0 );

    /**
     * Destroy the file info widget.
     */
    ~FileInfoWidget();

    /**
     * Display the information for the given file.
     */
    void setFile( const QString & name );
};

#endif
