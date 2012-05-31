// -*- C++ -*-
// SizePreview.h
//
// Part of KVIEWSHELL - A framework for multipage text/gfx viewers
//
// (C) 2002 Stefan Kebekus
// Distributed under the GPL

// Add header files alphabetically

#ifndef SIZEPREVIEW_H
#define SIZEPREVIEW_H

#include <qpixmap.h>
#include <qwidget.h>

class SimplePageSize;

class SizePreview : public QWidget
{
  Q_OBJECT

public:
  SizePreview( QWidget *parent, const char* name=0, WFlags f=0 );

public slots:
  // Sets the size.
  void setSize(const SimplePageSize&);

  // or = 0 means "Portrait", anything else means "Landscape"
  void setOrientation(int ori); 

protected:
  void paintEvent(QPaintEvent*);
  void resizeEvent(QResizeEvent*);

private:
  int orientation; // 0 = portrait, other = landscape

  // Both must be positive at all times!
  float _width; // in mm
  float _height; // in mm

  QPixmap pixmap;
};

#endif
