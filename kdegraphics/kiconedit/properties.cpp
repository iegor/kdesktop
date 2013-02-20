/*
    KDE Icon Editor - a small graphics drawing program for the KDE
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <kconfig.h>
#include <kdebug.h>
#include <kapplication.h>

#include "properties.h"

KIconEditProperties* KIconEditProperties::m_self = 0;

KIconEditProperties* KIconEditProperties::self()
{
    if (!m_self)
        m_self = new KIconEditProperties();
    return m_self;
}

KIconEditProperties::KIconEditProperties() : QObject()
{
    KConfig *config = kapp->config();

    config->setGroup( "Appearance" );

    m_bgMode = (QWidget::BackgroundMode)config->readNumEntry( "BackgroundMode", QWidget::FixedPixmap);
    m_bgColor = config->readColorEntry( "BackgroundColor", &gray);
    m_bgPixmap = config->readPathEntry("BackgroundPixmap");

    config->setGroup( "Grid" );
    m_pasteTransparent = config->readBoolEntry( "PasteTransparent", false );
    m_showGrid = config->readBoolEntry( "ShowGrid", true );
    m_gridScale = config->readNumEntry( "GridScaling", 10 );
    m_showRulers = config->readBoolEntry( "ShowRulers", true );

    if(config->readEntry( "TransparencyDisplayType", "Checkerboard" ) == "Checkerboard")
    {
      m_transparencyDisplayType = KIconEditGrid::TRD_CHECKERBOARD;
    }
    else
    {
      m_transparencyDisplayType = KIconEditGrid::TRD_SOLIDCOLOR;
    }

    QColor checkColor1(255, 255, 255);
    QColor checkColor2(127, 127, 127);

    m_checkerboardColor1 = config->readColorEntry( "CheckerboardColor1", &checkColor1);
    m_checkerboardColor2 = config->readColorEntry( "CheckerboardColor2", &checkColor2);

    QString checkerboardSize = config->readEntry( "CheckerboardSize", "Medium" );

    if(checkerboardSize == "Small")
    {
      m_checkerboardSize = KIconEditGrid::CHK_SMALL;
    }
    else
    if(checkerboardSize == "Medium")
    {
      m_checkerboardSize = KIconEditGrid::CHK_MEDIUM;
    }
    else
    {
      m_checkerboardSize = KIconEditGrid::CHK_LARGE;
    }

    QColor solidColor(255, 255, 255);
    m_transparencySolidColor = config->readColorEntry( "TransparencySolidColor", &solidColor);
}

KIconEditProperties::~KIconEditProperties()
{
  kdDebug(4640) << "KIconEditProperties: Deleting properties" << endl;
  m_self = 0;
}

void KIconEditProperties::save()
{
    KConfig *config = kapp->config();

    config->setGroup( "Appearance" );

    config->writeEntry("BackgroundMode", m_bgMode );
    config->writeEntry("BackgroundColor", m_bgColor );
    config->writePathEntry("BackgroundPixmap", m_bgPixmap );

    config->setGroup( "Grid" );
    config->writeEntry("PasteTransparent", m_pasteTransparent );
    config->writeEntry("ShowGrid", m_showGrid );
    config->writeEntry("GridScaling", m_gridScale );
    config->writeEntry("ShowRulers", m_showRulers );

    QString transparencyDisplayType;

    switch(m_transparencyDisplayType)
    {
      case KIconEditGrid::TRD_SOLIDCOLOR:
        transparencyDisplayType = "SolidColor";
        break;
      case KIconEditGrid::TRD_CHECKERBOARD:
      default:
        transparencyDisplayType = "Checkerboard";
        break;
    }

    config->writeEntry( "TransparencyDisplayType", transparencyDisplayType );
    config->writeEntry( "CheckerboardColor1", m_checkerboardColor1 );
    config->writeEntry( "CheckerboardColor2", m_checkerboardColor2 );

    QString checkerboardSize;

    switch(m_checkerboardSize)
    {
      case KIconEditGrid::CHK_SMALL:
        checkerboardSize = "Small";
        break;
      case KIconEditGrid::CHK_MEDIUM:
        checkerboardSize = "Medium";
        break;
      case KIconEditGrid::CHK_LARGE:
      default:
        checkerboardSize = "Large";
        break;
    }

    config->writeEntry( "CheckerboardSize", checkerboardSize );
    config->writeEntry( "TransparencySolidColor", m_transparencySolidColor );

    config->sync();
}

