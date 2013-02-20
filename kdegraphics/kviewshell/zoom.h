// -*- C++ -*-
// zoom.h
//
// Part of KVIEWSHELL - A framework for multipage text/gfx viewers
//
// (C) 2002 Stefan Kebekus
// Distributed under the GPL

// Add header files alphabetically

#ifndef ZOOM_H
#define ZOOM_H

#include <qobject.h>
#include <qstringlist.h>


class Zoom : public QObject
{
Q_OBJECT

public:
  /** Initializs the zoom with a default of 100% */
  Zoom();

  /** Returns a list like "33%", "100%", etc. If you call
      zoomNames() more than once, it is guaranteed that the same
      list of strings will be returned. */
  QStringList zoomNames() const { return valueNames; }

  float       zoomIn();
  float       zoomOut();
  float       value() const { return _zoomValue; }

public slots:
  void        setZoomValue(float);
  void        setZoomValue(const QString &);

  void        setZoomFitWidth(float zoom);
  void        setZoomFitHeight(float zoom);
  void        setZoomFitPage(float zoom);

signals:
  void        zoomNamesChanged(const QStringList &);
  void        zoomNameChanged(const QString &);
  void        valNoChanged(int);

private:
  float       _zoomValue;
  QStringList valueNames;
  // This will be the number of the current value in the generated QStringList.
  int         valNo;
};

#endif
