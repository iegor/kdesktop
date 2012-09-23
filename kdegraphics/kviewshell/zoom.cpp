// zoom.cpp
//
// Part of KVIEWSHELL - A framework for multipage text/gfx viewers
//
// (C) 2002 Stefan Kebekus
// Distributed under the GPL

// Add header files alphabetically

#include <config.h>

#include <math.h>
#include <qstringlist.h>

#include <klocale.h>

#include "zoom.h"
#include "zoomlimits.h"

float zoomVals[] = {0.20, 0.33, 0.50, 0.75, 1.00, 1.25, 1.50, 2.00, 2.50, 0};

Zoom::Zoom()
{
  valueNames << i18n("Fit to Page Width");
  valueNames << i18n("Fit to Page Height");
  valueNames << i18n("Fit to Page");

  _zoomValue = 1.0;
  valNo      = 6; // 1.0 is 6rd entry in the list
  for(int i=0; zoomVals[i] != 0; i++) 
    valueNames << QString("%1%").arg(zoomVals[i]);
}


void Zoom::setZoomValue(const QString &cval)
{
  float fval;

  // Remove a trailing '%', if any
  QString val = cval.stripWhiteSpace();
  if (val.right(1) == "%")
    val = val.left(val.length()-1).stripWhiteSpace();

  bool ok;
  fval = val.toFloat(&ok)/100.0;

  if (ok == true)
    setZoomValue(fval);
  else {
    emit(zoomNamesChanged(valueNames));
    emit(valNoChanged(valNo));
    emit(zoomNameChanged(QString("%1%").arg((int)(_zoomValue*100.0+0.5))));
  }
}

void Zoom::setZoomFitWidth(float zoom)
{
  // Make sure _zoomValue is in the permissible range!
  if (zoom < ZoomLimits::MinZoom/1000.0)
    zoom = ZoomLimits::MinZoom/1000.0;
  if (zoom > ZoomLimits::MaxZoom/1000.0)
    zoom = ZoomLimits::MaxZoom/1000.0;
  _zoomValue = zoom;

  valNo = 0;
  emit(valNoChanged(valNo));
  emit(zoomNameChanged(QString("%1%").arg((int)(_zoomValue*100.0+0.5))));
}

void Zoom::setZoomFitHeight(float zoom)
{
  // Make sure _zoomValue is in the permissible range!
  if (zoom < ZoomLimits::MinZoom/1000.0)
    zoom = ZoomLimits::MinZoom/1000.0;
  if (zoom > ZoomLimits::MaxZoom/1000.0)
    zoom = ZoomLimits::MaxZoom/1000.0;
  _zoomValue = zoom;

  valNo = 1;
  emit(valNoChanged(valNo));
  emit(zoomNameChanged(QString("%1%").arg((int)(_zoomValue*100.0+0.5))));
}

void Zoom::setZoomFitPage(float zoom)
{
  // Make sure _zoomValue is in the permissible range!
  if (zoom < ZoomLimits::MinZoom/1000.0)
    zoom = ZoomLimits::MinZoom/1000.0;
  if (zoom > ZoomLimits::MaxZoom/1000.0)
    zoom = ZoomLimits::MaxZoom/1000.0;
  _zoomValue = zoom;

  valNo = 2;
  emit(valNoChanged(valNo));
  emit(zoomNameChanged(QString("%1%").arg((int)(_zoomValue*100.0+0.5))));
}

void Zoom::setZoomValue(float f)
{
  // Make sure _zoomValue is in the permissible range!
  if (f < ZoomLimits::MinZoom/1000.0)
    f = ZoomLimits::MinZoom/1000.0;
  if (f > ZoomLimits::MaxZoom/1000.0)
    f = ZoomLimits::MaxZoom/1000.0;
  _zoomValue = f;


  valueNames.clear();

  valueNames << i18n("Fit to Page Width");
  valueNames << i18n("Fit to Page Height");
  valueNames << i18n("Fit to Page");

  bool flag = false;
  for(int i = 0; zoomVals[i] != 0; i++) {
    if ((_zoomValue <= zoomVals[i]) && (flag == false)) {
      flag  = true;
      valNo = i + 3;
      if (fabs(_zoomValue-zoomVals[i]) > 0.01)
        valueNames << QString("%1%").arg((int)(_zoomValue*100.0+0.5));
    }
    valueNames << QString("%1%").arg((int)(zoomVals[i]*100.0+0.5));
  }

  if (flag == false) {
    valNo = valueNames.size();
    valueNames << QString("%1%").arg((int)(_zoomValue*100.0+0.5));
  }

  emit(zoomNamesChanged(valueNames));
  emit(valNoChanged(valNo));
  emit(zoomNameChanged(QString("%1%").arg((int)(_zoomValue*100.0+0.5))));
}

float Zoom::zoomIn()
{
  int i;
  for(i=0; zoomVals[i] != 0; i++) 
    if (zoomVals[i] > _zoomValue)
      return zoomVals[i];

  return zoomVals[i-1];
}

float Zoom::zoomOut()
{
  float result = zoomVals[0];

  for(int i=0; zoomVals[i] != 0; i++) 
    if (_zoomValue > zoomVals[i])
      result = zoomVals[i];

  return result;
}

#include "zoom.moc"
