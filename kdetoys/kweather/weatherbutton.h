/* This file is part of the KDE project
   Copyright (C) 2003-2004 Nadeem Hasan <nhasan@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef WEATHERBUTTON_H
#define WEATHERBUTTON_H

#include <qbutton.h>
#include <qpixmap.h>

class WeatherButton : public QButton
{
  Q_OBJECT

  public:
    WeatherButton( QWidget *parent, const char *name );
    void setPixmap( const QPixmap &pix );

  protected:
    void drawButton( QPainter *p );
    void drawButtonLabel( QPainter *p );
    QSize margin() const { return QSize( 3, 3 ); }
    QSize pixmapSize() const { return size() - margin()*2; }
    QPoint pixmapOrigin() const;
    void generateIcons();

    void enterEvent( QEvent *e );
    void leaveEvent( QEvent *e );
    void resizeEvent( QResizeEvent *e );

    bool m_highlight;
    QPixmap m_normalIcon;
    QPixmap m_activeIcon;

  protected slots:
    void slotSettingsChanged( int category );
    void slotIconChanged( int group );
};

#endif // WEATHERBUTTON_H

// vim:ts=4:sw=4:et
