/** 
 * Copyright (C) 2003 the KGhostView authors. See file AUTHORS.
 * 	
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <kdialogbase.h>

class QLabel;
class QTextEdit;
class KURLLabel;

class LogWindow : public KDialogBase
{
    Q_OBJECT

public:
    LogWindow( const QString& caption,
               QWidget* parent = 0, const char* name = 0 );

public slots:
    void append( const QString& message );
    void clear(); 
    void setLabel( const QString&, bool showConfigureGSLink );

private slots:
    void emitConfigureGS();

signals:
    void configureGS();

private:
    QLabel*      _errorIndication;
    QTextEdit*   _logView;
    KURLLabel*   _configureGS;
};

#endif

// vim:sw=4:sts=4:ts=8:sta:tw=78:noet
