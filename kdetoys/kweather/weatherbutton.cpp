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

#include "weatherbutton.h"

#include <qpainter.h>

#include <kapplication.h>
#include <kcursor.h>
#include <kglobalsettings.h>
#include <kiconeffect.h>
#include <kicontheme.h>
#include <kipc.h>
#include <kstandarddirs.h>

WeatherButton::WeatherButton( QWidget *parent, const char *name )
    : QButton( parent, name ), m_highlight( false )
{
    setBackgroundOrigin( AncestorOrigin );

    connect( kapp, SIGNAL( settingsChanged( int ) ),
       SLOT( slotSettingsChanged( int ) ) );
    connect( kapp, SIGNAL( iconChanged( int ) ),
       SLOT( slotIconChanged( int ) ) );

    kapp->addKipcEventMask( KIPC::SettingsChanged );
    kapp->addKipcEventMask( KIPC::IconChanged );

    slotSettingsChanged( KApplication::SETTINGS_MOUSE );
}

void WeatherButton::drawButton( QPainter *p )
{
    drawButtonLabel(p);
}

void WeatherButton::drawButtonLabel( QPainter *p )
{
    if (!pixmap())
    {
        return;
    }

    QPixmap pix = m_highlight? m_activeIcon : m_normalIcon;

    if (isOn() || isDown())
    {
        pix = pix.convertToImage().smoothScale(pix.width() - 2,
                                               pix.height() - 2);
    }

    int h = height();
    int w = width();
    int ph = pix.height();
    int pw = pix.width();
    int margin = 3;
    QPoint origin(margin / 2, margin / 2);

    if (ph < (h - margin))
    {
        origin.setY((h - ph) / 2);
    }

    if (pw < (w - margin))
    {
        origin.setX((w - pw) / 2);
    }

    p->drawPixmap(origin, pix);
}


void WeatherButton::setPixmap( const QPixmap &pix )
{
    QButton::setPixmap( pix );
    generateIcons();
}

void WeatherButton::generateIcons()
{
    if ( !pixmap() )
        return;

    QImage image = pixmap()->convertToImage();
    image = image.smoothScale( pixmapSize(), QImage::ScaleMin );

    KIconEffect effect;

    m_normalIcon = effect.apply( image, KIcon::Panel, KIcon::DefaultState );
    m_activeIcon = effect.apply( image, KIcon::Panel, KIcon::ActiveState );
}

void WeatherButton::slotSettingsChanged( int category )
{
    if ( category != KApplication::SETTINGS_MOUSE ) return;

    bool changeCursor = KGlobalSettings::changeCursorOverIcon();

    if ( changeCursor )
        setCursor( KCursor::handCursor() );
    else
        unsetCursor();
}

void WeatherButton::slotIconChanged( int group )
{
    if ( group != KIcon::Panel )
        return;

    generateIcons();
    repaint( false );
}

void WeatherButton::enterEvent( QEvent *e )
{
    m_highlight = true;

    repaint( false );
    QButton::enterEvent( e );
}

void WeatherButton::leaveEvent( QEvent *e )
{
    m_highlight = false;

    repaint( false );
    QButton::enterEvent( e );
}

void WeatherButton::resizeEvent( QResizeEvent * )
{
    generateIcons();
}

QPoint WeatherButton::pixmapOrigin() const
{
    QSize point = margin()/2;
    QPoint origin( point.width(), point.height() );

    return origin;
}

#include "weatherbutton.moc"

// vim:ts=4:sw=4:et
