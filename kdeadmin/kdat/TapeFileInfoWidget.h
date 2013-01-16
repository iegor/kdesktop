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

#ifndef _TapeFileInfoWidget_h_
#define _TapeFileInfoWidget_h_

#include <qwidget.h>

class QLabel;

class File;

/**
 * @short Display information about a tape file.
 */
class TapeFileInfoWidget : public QWidget {
    Q_OBJECT
    File*      _file;
    QLabel*    _fileName;
    QLabel*    _mtime;
    QLabel*    _startRecord;
    QLabel*    _endRecord;
    QLabel*    _size;
public:
    /**
     * Create a new tape file info widget.
     *
     * @param parent The parent widget.
     * @param name   The name of this widget.
     */
    TapeFileInfoWidget( QWidget* parent = 0, const char* name = 0 );

    /**
     * Destroy the tape file info widget.
     */
    ~TapeFileInfoWidget();

    /**
     * Display the information for the given tape file.
     */
    void setFile( File* file );
};

#endif
