// units.cpp
//
// Part of KVIEWSHELL - A framework for multipage text/gfx viewers
//
// (C) 2003 Stefan Kebekus
// Distributed under the GPL

// Add header files alphabetically

#include <config.h>

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <math.h>
#include <qstringlist.h>

#include "units.h"

class unitOfDistance
{
 public:
  float       mmPerUnit;
  const char* name;
};

unitOfDistance distanceUnitTable[] = {
  // Metric units
  {1.0, "mm"},
  {1.0, "millimeter"},
  {10.0, "cm"},
  {10.0, "centimeter"},
  {100.0*10.0, "m"},
  {100.0*10.0, "meter"},

  // Imperial units
  {25.4, "in"},
  {25.4, "inch"},

  // Typographical units
  {2540.0/7227.0, "pt"},  // TeX points. 7227points = 254cm
  {25.4/72.0, "bp"},      // big points, 1/72 inch as used in Postscript
  {25.4/6.0, "pc"},       // pica = 1/6 inch
  {25.4/6.0, "pica"},
  {25.4*0.0148, "dd"},    // didot points = 0.0148 inches
  {25.4*0.0148, "didot"},
  {25.4*0.178, "cc"},     // cicero points = 0.178 inches
  {25.4*0.178, "cicero"},

  {0.0, 0},
};




float distance::convertToMM(const QString &distance, bool *ok)
{
  //  kdDebug() << "convertToMM( " << distance << " )" << endl;

  float MMperUnit = 0.0;
  int unitPos = 0; // position of the unit in the string

  // Check for various known units, and store the beginning position
  // of the unit in 'unitPos', so that distance[0..unitPos] will hold
  // the value. Store the number of mm per unit in 'MMperUnit'.
  for(int i=0; MMperUnit==0.0 && distanceUnitTable[i].name != 0; i++) {
    unitPos = distance.findRev(distanceUnitTable[i].name);
    if (unitPos != -1)
      MMperUnit = distanceUnitTable[i].mmPerUnit;
  }

  // If no unit has been found -> error message, set *ok to false and
  // return 0.0.
  if (MMperUnit == 0.0) {
    kdError() << "distance::convertToMM: no known unit found in the string '" << distance << "'." << endl;
    if (ok)
      *ok = false;
    return 0.0;
  }

  QString val = distance.left(unitPos).simplifyWhiteSpace();
  return MMperUnit*val.toFloat(ok);
}
