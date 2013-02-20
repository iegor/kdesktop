/*
    kiconedit - a small graphics drawing program for the KDE
    Copyright ( C ) 1998  Thomas Tanghus ( tanghus@kde.org )
    Copyright ( C ) 2002  Nadeem Hasan ( nhasan@kde.org )

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or ( at your option ) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef PALLETTETOOLBAR_H
#define PALLETTETOOLBAR_H

#include <ktoolbar.h>

class QBoxLayout;
class QLabel;
class KSysColors;
class KCustomColors;

class PaletteToolBar : public KToolBar
{
  Q_OBJECT

  public:
    PaletteToolBar( QWidget *parent, const char *name = 0L );

  signals:
    void newColor( uint c );

  public slots:
    virtual void setOrientation( Orientation o );
    void previewChanged( const QPixmap &p );
    void addColors( uint n, uint *c );
    void addColor( uint color );
    void setPreviewBackground( QPixmap pixmap );
    void setPreviewBackground( const QColor& colour );
    void currentColorChanged(uint color);

  private:
    QBoxLayout *m_layout;
    QLabel *m_lblPreview;
    QLabel *m_currentColorView;
    KSysColors *m_sysColors;
    KCustomColors *m_customColors;
};

#endif // PALLETTETOOLBAR_H

/* vim: et sw=2 ts=2
*/

