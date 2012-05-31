// pageSize.cpp
//
// Part of KVIEWSHELL - A framework for multipage text/gfx viewers
//
// (C) 2002-2003 Stefan Kebekus
// Distributed under the GPL

// Add header files alphabetically

#include <config.h>

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <math.h>
#include <qstringlist.h>

#include "pageSize.h"
#include "units.h"

struct pageSizeItem 
{
  const char *name;
  float width;         // in mm
  float height;        // in mm
  const char *preferredUnit;
};

#define defaultMetricPaperSize 4 // Default paper size is "DIN A4"
#define defaultImperialPaperSize 8 // Default paper size is "US Letter"

static pageSizeItem staticList[] = { {"DIN A0",    841.0, 1189.0, "mm"}, 
				     {"DIN A1",    594.0, 841.0, "mm"}, 
				     {"DIN A2",    420.0, 594.0, "mm"}, 
				     {"DIN A3",    297.0, 420.0, "mm"}, 
				     {"DIN A4",    210.0, 297.0, "mm"},
				     {"DIN A5",    148.5, 210.0, "mm"}, 
				     {"DIN B4",    250.0, 353.0, "mm"}, 
				     {"DIN B5",    176.0, 250.0, "mm"},
				     {"US Letter", 215.9, 279.4, "in"}, 
				     {"US Legal",  215.9, 355.6, "in"},
				     {0,  0.0, 0.0, 0} // marks the end of the list.
};


pageSize::pageSize()
{
  currentSize = defaultPageSize();
  pageWidth.setLength_in_mm(staticList[currentSize].width);
  pageHeight.setLength_in_mm(staticList[currentSize].height);
}


pageSize::pageSize(const SimplePageSize& s)
{
  pageWidth = s.width();
  pageHeight = s.height();
  
  rectifySizes();
  reconstructCurrentSize();
}


bool pageSize::setPageSize(const QString& name)
{
  // See if we can recognize the string
  QString currentName;
  for(int i=0; staticList[i].name != 0; i++) {
    currentName = staticList[i].name;
    if (currentName == name) {
      currentSize = i;  
      // Set page width/height accordingly
      pageWidth.setLength_in_mm(staticList[currentSize].width);
      pageHeight.setLength_in_mm(staticList[currentSize].height);
      emit(sizeChanged(*this));
      return true;
    }
  }

  // Check if the string contains 'x'. If yes, we assume it is of type
  // "<number>x<number>". If yes, the first number is interpreted as
  // the width in mm, the second as the height in mm
  if (name.find('x') >= 0) {
    bool wok, hok;
    float pageWidth_tmp  = name.section('x',0,0).toFloat(&wok);
    float pageHeight_tmp = name.section('x',1,1).toFloat(&hok);
    if (wok && hok) {
      pageWidth.setLength_in_mm(pageWidth_tmp);
      pageHeight.setLength_in_mm(pageHeight_tmp);
      
      rectifySizes();
      reconstructCurrentSize();
      emit(sizeChanged(*this));
      return true;
    }
  }
  
  // Check if the string contains ','. If yes, we assume it is of type
  // "<number><unit>,<number><uni>". The first number is supposed to
  // be the width, the second the height.
  if (name.find(',') >= 0) {
    bool wok, hok;
    float pageWidth_tmp  = distance::convertToMM(name.section(',',0,0), &wok);
    float pageHeight_tmp = distance::convertToMM(name.section(',',1,1), &hok);
    if (wok && hok) {
      pageWidth.setLength_in_mm(pageWidth_tmp);
      pageHeight.setLength_in_mm(pageHeight_tmp);
      
      rectifySizes();
      reconstructCurrentSize();
      emit(sizeChanged(*this));
      return true;
    } 
  }  
  
  // Last resource. Set the default, in case the string is
  // unintelligible to us.
  currentSize = defaultPageSize();
  pageWidth.setLength_in_mm(staticList[currentSize].width);
  pageHeight.setLength_in_mm(staticList[currentSize].height);
  kdError(1223) << "pageSize::setPageSize: could not parse '" << name << "'. Using " << staticList[currentSize].name << " as a default." << endl;
  emit(sizeChanged(*this));
  return false;
}


void pageSize::setPageSize(double width, double height)
{
  SimplePageSize oldPage = *this;

  pageWidth.setLength_in_mm(width);
  pageHeight.setLength_in_mm(height);
  
  rectifySizes();
  reconstructCurrentSize();
  if ( !isNearlyEqual(oldPage))
    emit(sizeChanged(*this));
}


void pageSize::setPageSize(const QString& width, const QString& _widthUnits, const QString& height, const QString& _heightUnits)
{
  SimplePageSize oldPage = *this;

  double w = width.toFloat();
  double h = height.toFloat();

  QString widthUnits = _widthUnits;
  if ((widthUnits != "cm") && (widthUnits != "mm") && (widthUnits != "in")) {
    kdError(1223) << "Unrecognized page width unit '" << widthUnits << "'. Assuming mm" << endl;
    widthUnits = "mm";
  }
  pageWidth.setLength_in_mm(w);
  if (widthUnits == "cm")
    pageWidth.setLength_in_cm(w);
  if (widthUnits == "in")
    pageWidth.setLength_in_inch(w);
  
  QString heightUnits = _heightUnits;
  if ((heightUnits != "cm") && (heightUnits != "mm") && (heightUnits != "in")) {
    kdError(1223) << "Unrecognized page height unit '" << widthUnits << "'. Assuming mm" << endl;
    heightUnits = "mm";
  }
  pageHeight.setLength_in_mm(h);
  if (heightUnits == "cm")
    pageHeight.setLength_in_cm(h);
  if (heightUnits == "in")
    pageHeight.setLength_in_inch(h);

  rectifySizes();
  reconstructCurrentSize();
  if ( !isNearlyEqual(oldPage))
    emit(sizeChanged(*this));
}


pageSize &pageSize::operator= (const pageSize &src)
{
  SimplePageSize oldPage = *this;

  currentSize = src.currentSize;
  pageWidth   = src.pageWidth;
  pageHeight  = src.pageHeight;

  if ( !isNearlyEqual(oldPage))
    emit(sizeChanged(*this));
  return *this;
}


void pageSize::rectifySizes()
{
  // Now do some sanity checks to make sure that values are not
  // outrageous. We allow values between 5cm and 50cm.
  if (pageWidth.getLength_in_mm() < 50)
    pageWidth.setLength_in_mm(50.0);
  if (pageWidth.getLength_in_mm() > 1200)
    pageWidth.setLength_in_mm(1200);
  if (pageHeight.getLength_in_mm() < 50)
    pageHeight.setLength_in_mm(50);
  if (pageHeight.getLength_in_mm() > 1200)
    pageHeight.setLength_in_mm(1200);
  return;
}


QString pageSize::preferredUnit() const
{
  if (currentSize >= 0)
    return staticList[currentSize].preferredUnit;

  // User-defined size. Give a preferred unit depening on the locale.
  if (KGlobal::locale()-> measureSystem() == KLocale::Metric)
    return "mm";
  else
    return "in";
}


QString pageSize::widthString(const QString& unit) const
{
  QString answer = "--";

  if (unit == "cm")
    answer.setNum(pageWidth.getLength_in_cm());
  if (unit == "mm")
    answer.setNum(pageWidth.getLength_in_mm());
  if (unit == "in")
    answer.setNum(pageWidth.getLength_in_inch());

  return answer;
}


QString pageSize::heightString(const QString& unit) const
{
  QString answer = "--";

  if (unit == "cm")
    answer.setNum(pageHeight.getLength_in_cm());
  if (unit == "mm")
    answer.setNum(pageHeight.getLength_in_mm());
  if (unit == "in")
    answer.setNum(pageHeight.getLength_in_inch());

  return answer;
}


QStringList pageSize::pageSizeNames() 
{
  QStringList names;
  
  for(int i=0; staticList[i].name != 0; i++) 
    names << staticList[i].name;

  return names;
}


QString pageSize::formatName() const
{
  if (currentSize >= 0)
    return staticList[currentSize].name;
  else 
    return QString::null;
}


int pageSize::getOrientation() const
{
  if (currentSize == -1) {
    kdError(1223) << "pageSize::getOrientation: getOrientation called for page format that does not have a name." << endl;
    return 0;
  }

  if (pageWidth.getLength_in_mm() == staticList[currentSize].width)
    return 0;
  else
    return 1;
}


void pageSize::setOrientation(int orient)
{
  if (currentSize == -1) {
    kdError(1223) << "pageSize::setOrientation: setOrientation called for page format that does not have a name." << endl;
    return;
  }
  
  if (orient == 1) {
    pageWidth.setLength_in_mm(staticList[currentSize].height);
    pageHeight.setLength_in_mm(staticList[currentSize].width);
  } else {
    pageWidth.setLength_in_mm(staticList[currentSize].width);
    pageHeight.setLength_in_mm(staticList[currentSize].height);
  }
  emit(sizeChanged(*this));
}


QString pageSize::serialize() const
{
  if ((currentSize >= 0) && (fabs(staticList[currentSize].height-pageHeight.getLength_in_mm()) <= 0.5))
    return staticList[currentSize].name;
  else 
    return QString("%1x%2").arg(pageWidth.getLength_in_mm()).arg(pageHeight.getLength_in_mm());
}


void pageSize::reconstructCurrentSize()
{
  for(int i=0; staticList[i].name != 0; i++) {
    if ((fabs(staticList[i].width - pageWidth.getLength_in_mm()) <= 2) && (fabs(staticList[i].height - pageHeight.getLength_in_mm()) <= 2)) {
      currentSize = i;
      pageWidth.setLength_in_mm(staticList[currentSize].width);
      pageHeight.setLength_in_mm(staticList[currentSize].height);
      return;
    }
    if ((fabs(staticList[i].height - pageWidth.getLength_in_mm()) <= 2) && (fabs(staticList[i].width - pageHeight.getLength_in_mm()) <= 2)) {
      currentSize = i;
      pageWidth.setLength_in_mm(staticList[currentSize].height);
      pageHeight.setLength_in_mm(staticList[currentSize].width);
      return;
    }
  }
  currentSize = -1;
  return;
}

int pageSize::defaultPageSize()
{
  // FIXME: static_cast<QPrinter::PageSize>(KGlobal::locale()->pageSize())
  //        is the proper solution here.  Then you can determine the values
  //        without using your hardcoded table too!
  if (KGlobal::locale()-> measureSystem() == KLocale::Metric)
    return defaultMetricPaperSize;
  else
    return defaultImperialPaperSize;
}

#include "pageSize.moc"

