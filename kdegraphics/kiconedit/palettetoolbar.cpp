/*
    kiconedit - a small graphics drawing program for the KDE
    Copyright ( C ) 1998  Thomas Tanghus (  tanghus@kde.org )
    Copyright ( C ) 2002  Nadeem Hasan (  nhasan@kde.org )

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (  at your option ) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <qlabel.h>
#include <qlayout.h>
#include <qwhatsthis.h>
#include <qpainter.h>

#include <klocale.h>
#include <kdebug.h>

#include "kiconcolors.h"
#include "palettetoolbar.h"

PaletteToolBar::PaletteToolBar( QWidget *parent, const char *name )
    : KToolBar( parent, name )
{
  QWidget *base = new QWidget( this );

  QBoxLayout::Direction d = orientation() == Qt::Horizontal? 
      QBoxLayout::LeftToRight : QBoxLayout::TopToBottom;
  m_layout = new QBoxLayout( base, d, 2, 6 );

  m_lblPreview = new QLabel( base );
  m_lblPreview->setFrameStyle( QFrame::Panel|QFrame::Sunken );
  m_lblPreview->setFixedHeight( 64 );
  m_lblPreview->setAlignment( Qt::AlignHCenter|Qt::AlignVCenter );
  QWhatsThis::add(m_lblPreview, i18n( "Preview\n\nThis is a 1:1 preview"
      " of the current icon" ) );
  m_layout->addWidget( m_lblPreview );

  m_currentColorView = new QLabel( base );
  m_currentColorView->setFrameStyle( QFrame::Panel|QFrame::Sunken );
  m_currentColorView->setFixedHeight( 24 );
  m_currentColorView->setAlignment( Qt::AlignHCenter|Qt::AlignVCenter );
  QWhatsThis::add(m_currentColorView, i18n( "Current color\n\nThis is the currently selected color" ) );
  m_layout->addWidget( m_currentColorView );

  QVBoxLayout *vlayout = new QVBoxLayout( m_layout, 0 );
  QLabel *l = new QLabel( i18n( "System colors:" ), base );
  vlayout->addWidget( l );
  m_sysColors = new KSysColors( base );
  QWhatsThis::add(m_sysColors, i18n( "System colors\n\nHere you can select"
      " colors from the KDE icon palette" ) );

  vlayout->addWidget( m_sysColors );

  connect( m_sysColors, SIGNAL( newColor(uint) ), 
      SIGNAL( newColor(uint) ) );

  vlayout = new QVBoxLayout( m_layout, 0 );
  l = new QLabel( i18n( "Custom colors:" ), base );
  vlayout->addWidget( l );
  m_customColors = new KCustomColors( base );
  QWhatsThis::add(m_customColors, i18n( "Custom colors\n\nHere you can"
      " build a palette of custom colors.\nDouble-click on a box to edit"
      " the color" ) );

  vlayout->addWidget( m_customColors );

  connect( m_customColors, SIGNAL( newColor(uint) ), 
      SIGNAL( newColor(uint) ) );
  connect( this, SIGNAL( newColor(uint)),
      this, SLOT(currentColorChanged(uint)));
  currentColorChanged(OPAQUE_MASK|0);

  setEnableContextMenu( false );
  setMovingEnabled( false );
}

void PaletteToolBar::setOrientation( Orientation o )
{
   if(  barPos() == Floating )
     o = o == Qt::Vertical ? Qt::Horizontal : Qt::Vertical;

  QBoxLayout::Direction d = o == Qt::Horizontal? QBoxLayout::LeftToRight
        : QBoxLayout::TopToBottom;
  m_layout->setDirection( d );

  QDockWindow::setOrientation( o );
}

void PaletteToolBar::previewChanged(  const QPixmap &p )
{
  m_lblPreview->setPixmap( p );
}

void PaletteToolBar::addColors( uint n, uint *c )
{
    m_customColors->clear();
    for( uint i = 0; i < n; i++ )
        addColor( c[ i ] );
}

void PaletteToolBar::addColor( uint color )
{
    if( !m_sysColors->contains( color ) )
        m_customColors->addColor( color );
}

void PaletteToolBar::setPreviewBackground( QPixmap pixmap )
{
    m_lblPreview->setBackgroundPixmap(pixmap);
}

void PaletteToolBar::setPreviewBackground( const QColor& colour )
{
    m_lblPreview->setBackgroundColor(colour);
}

void PaletteToolBar::currentColorChanged(uint color)
{
  if(qAlpha(color) == 255)
  {
    m_currentColorView->setBackgroundColor(color);
  }
  else
  {
    // Show the colour as if drawn over a checkerboard pattern
    const int squareWidth = 8;
    const uint lightColour = qRgb(255, 255, 255);
    const uint darkColour = qRgb(127, 127, 127);

    QPixmap pm(2 * squareWidth, 2 * squareWidth);
    QPainter p(&pm);

    double alpha = qAlpha(color) / 255.0;

    int r = int(qRed(color) * alpha + (1 - alpha) * qRed(lightColour) + 0.5);
    int g = int(qGreen(color) * alpha + (1 - alpha) * qGreen(lightColour) + 0.5);
    int b = int(qBlue(color) * alpha + (1 - alpha) * qBlue(lightColour) + 0.5);

    uint squareColour = qRgb(r, g, b);

    p.setPen(Qt::NoPen);
    p.setBrush(squareColour);
    p.drawRect(0, 0, squareWidth, squareWidth);
    p.drawRect(squareWidth, squareWidth, squareWidth, squareWidth);

    r = int(qRed(color) * alpha + (1 - alpha) * qRed(darkColour) + 0.5);
    g = int(qGreen(color) * alpha + (1 - alpha) * qGreen(darkColour) + 0.5);
    b = int(qBlue(color) * alpha + (1 - alpha) * qBlue(darkColour) + 0.5);

    squareColour = qRgb(r, g, b);

    p.setBrush(squareColour);
    p.drawRect(squareWidth, 0, squareWidth, squareWidth);
    p.drawRect(0, squareWidth, squareWidth, squareWidth);

    p.end();

    m_currentColorView->setBackgroundPixmap(pm);
  }
}

#include "palettetoolbar.moc"

/* vim: et sw=2 ts=2
*/

