// -*- C++ -*-
//
// pageNumber.h
//
// Part of KVIEWSHELL - A framework for multipage text/gfx viewers
//
// (C) 2004 Stefan Kebekus
// Distributed under the GPL

// Add header files alphabetically

#ifndef PAGENUMBER_H
#define PAGENUMBER_H

#include <qglobal.h>


/** \brief Class to represent a page number
    
The class PageNumber is really nothing but an alias for Q_UINT16, and
can be casted to and from Q_UINT16. It is used in kviewshell to remind
the programmer of the convention that page numbers start at '1' (for
'first page'), and that the value '0' means 'illegal page number' or
'no page number'. Accordingly, the value '0' is also named
PageNumber::invalidPage, and there is a trivial method isInvalid()
that checks if the page number is 0.

@author Stefan Kebekus <kebekus@kde.org>
@version 1.0 0
*/

class PageNumber
{
 public:
  enum pageNums {
    invalidPage   = 0 /*! Invalid page number */
  };

  /** The default constructor sets the page number to 'invalidPage' */
  PageNumber() {pgNum = invalidPage;}

  /** \brief Constructor that sets the page number

  @param num page number that is set initially
  */
  PageNumber(Q_UINT16 num) {pgNum = num;}

  /** \brief this method implements typecasts from Q_UINT16 */
  PageNumber &operator=(const Q_UINT16 p) { pgNum = p; return *this; }

  /** \brief This method implements typecasts to Q_UINT16 */
  operator Q_UINT16() const { return pgNum; }

  /** \brief Checks if the page number is invalid

  @returns true, if pgNum != invalidPage, i.e., does not equal 0
  */
  bool isValid() const {return (pgNum != invalidPage);}

 private:
  /** \brief Single number that represents the page number */
  Q_UINT16 pgNum;
};

#endif
