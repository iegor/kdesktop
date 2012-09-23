// units.h
//
// Part of KVIEWSHELL - A framework for multipage text/gfx viewers
//
// (C) 2003 Stefan Kebekus
// Distributed under the GPL

// Add header files alphabetically

#ifndef UNITS_H
#define UNITS_H

class QString;

class distance {
 public:
  // This method converts a string that gives a distance in one of the
  // commonly used units, such as "12.3mm", "12 inch" or "15 didot" to
  // millimeters. For a complete list of supported units, see the
  // static lists that are hardcoded in "units.cpp".
  //
  // If the conversion is not possible *ok is set to "false" and an
  // undefined value is returned. If the unit could not be recognized,
  // an error message is printed via kdError(). Otherwise, *ok is set
  // to true.
  //
  // It is possible in rare circumstances that ok is set to true
  // although the string is malformed.
  //
  // It is fine to set ok to 0.
  static float convertToMM(const QString &distance, bool *ok=0);
};

#endif
