/*
    KDE Draw - a small graphics drawing program for the KDE.
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)
    Copyright ( C ) 2002  Nadeem Hasan (  nhasan@kde.org )

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

#ifndef __KRESIZE_H__
#define __KRESIZE_H__

#include <kdialogbase.h>

class KIntSpinBox;

class KResizeWidget : public QWidget
{
  Q_OBJECT

public:

  KResizeWidget( QWidget* parent, const char* name, const QSize& );
  ~KResizeWidget();

  const QSize getSize();

private:

  KIntSpinBox *m_width;
  KIntSpinBox *m_height;
};

class KResizeDialog : public KDialogBase
{
  Q_OBJECT
  
public:

  KResizeDialog( QWidget* parent, const char* name, const QSize s );
  ~KResizeDialog();

  const QSize getSize();

private:

  KResizeWidget* m_resize;
};

#endif //__KRESIZE_H__
