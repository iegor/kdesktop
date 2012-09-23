/*
    KDE Draw - a small graphics drawing program for the KDE
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)
    Copyright (C) 2002  Nadeem Hasan ( nhasan@kde.org )

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

#include <qlabel.h>
#include <qlayout.h>
#include <qgroupbox.h>

#include <klocale.h>
#include <knuminput.h>

#include "kresize.h"

KResizeWidget::KResizeWidget( QWidget* parent, const char* name, 
    const QSize& size ) : QWidget( parent, name )
{
  QHBoxLayout* genLayout = new QHBoxLayout( this );

  QGroupBox* group = new QGroupBox( i18n( "Size" ), this );
  group->setColumnLayout( 0, Qt::Horizontal );
  genLayout->addWidget( group );

  QHBoxLayout* layout = new QHBoxLayout( group->layout(), 6 );

  m_width = new KIntSpinBox( 1, 200, 1, 1, 10, group );
  m_width->setValue( size.width() );
  layout->addWidget( m_width, 1 );

  QLabel* label = new QLabel( "X", group );
  layout->addWidget( label );

  m_height = new KIntSpinBox( 1, 200, 1, 1, 10, group);
  m_height->setValue( size.height() );
  layout->addWidget( m_height, 1 );

  setMinimumSize( 200, 100 );
}

KResizeWidget::~KResizeWidget()
{
}

const QSize KResizeWidget::getSize()
{
  return QSize( m_width->value(), m_height->value() );
}

KResizeDialog::KResizeDialog( QWidget* parent, const char* name, 
    const QSize size )
    : KDialogBase( parent, name, true, i18n( "Select Size" ), Ok|Cancel )
{
  m_resize = new KResizeWidget( this, "resize widget", size );

  setMainWidget( m_resize );
}

KResizeDialog::~KResizeDialog()
{
}

const QSize KResizeDialog::getSize()
{
  return m_resize->getSize();
}

#include "kresize.moc"
