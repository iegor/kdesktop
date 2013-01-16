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

#ifndef _TapeInfoWidget_h_
#define _TapeInfoWidget_h_

#include <qwidget.h>

class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;

class Tape;

/**
 * @short Display/edit information about a tape index.
 */
class TapeInfoWidget : public QWidget {
    Q_OBJECT
    Tape*        _tape;
    QLineEdit*   _tapeName;
    QLineEdit*   _tapeSize;
    QComboBox*   _tapeSizeUnits;
    QLabel*      _tapeID;
    QLabel*      _ctime;
    QLabel*      _mtime;
    QLabel*      _archiveCount;
    QLabel*      _spaceUsed;
    QPushButton* _apply;

    bool isModified();
private slots:
    void slotTextChanged( const QString & text );
    void slotActivated( int index );
    void slotApply();
public:
    /**
     * Create a new tape info widget.
     */
    TapeInfoWidget( QWidget* parent = 0, const char* name = 0 );

    /**
     * Destroy the tape info widget.
     */
    ~TapeInfoWidget();

    /**
     * Change the tape index that the widget displays/edits.
     *
     * @param tape The new tape index to display/edit.
     */
    void setTape( Tape* tape );
};

#endif
