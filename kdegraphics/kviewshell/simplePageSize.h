// -*- C++ -*-
//
// simplePageSize.h
//
// Part of KVIEWSHELL - A framework for multipage text/gfx viewers
//
// (C) 2002-2004 Stefan Kebekus
// Distributed under the GPL


#ifndef SIMPLEPAGESIZE_H
#define SIMPLEPAGESIZE_H

#include "length.h"

#include <qsize.h>

class QString;
class QStringList;


/** \brief This class represents phyiscal page sizes.

This class represents page sizes. It contains nothing but two numbers,
the page width, and page height, and a few utility functions that
convert page sizes to pixel sizes and to compute the aspect ratio. A
page with width<=1mm or height<=1mm is considered invalid. pageSize is
a more elaborate class that is derived from SimplePageSize and knows
about standard paper sizes.

@author Stefan Kebekus <kebekus@kde.org>
@version 1.0 0
*/

class SimplePageSize
{
 public:
  /** Constructs an invalid SimplePageSize, with size 0x0mm */
  SimplePageSize() { pageWidth.setLength_in_mm(0.0); pageHeight.setLength_in_mm(0.0); }

  /** Constructs a SimplePagesize with given page width and height in
      mm. Recall that if width or height is less or equal than 1mm,
      the page size is considered 'invalid' by the isValid()
      method.
      
      @param width
      @param height
  */
  SimplePageSize(const Length& width, const Length& height) { pageWidth = width; pageHeight = height; }

  /** \brief Sets the page width and height
      
  If width or height is less or equal than 1mm, the page size is
  considered 'invalid' by the isValid() method.
  
  @param width
  @param height
  */
  virtual void setPageSize(const Length& width, const Length& height) { pageWidth = width; pageHeight = height; }

  /** \brief Returns the page width. */
  Length width() const { return pageWidth; }

  /** \brief Returns the page height. */
  Length height() const { return pageHeight; }

  /** \brief Aspect ratio

  @returns if the paper size is valid, this method returns the ratio
  width/height. Returns 1.0 otherwise. */
  double      aspectRatio() const { return isValid() ? (pageWidth/pageHeight) : 1.0; }

  /** \brief Converts the physical page size to a pixel size

  @param resolution in dots per inch

  @returns the pixel size, represented by a QSize. If the page size is
      invalid, the result is undefined. */
  QSize       sizeInPixel(double resolution) const {return QSize( (int)(resolution*pageWidth.getLength_in_inch() + 0.5), 
								  (int)(resolution*pageHeight.getLength_in_inch() + 0.5)); }

  /** \brief Zoom value required to scale to a certain height

  If the pageSize is valid, this method returns the zoom value
  required to scale the page size down to 'height' pixels on the
  currently used display. The method uses QPaintDevice::x11AppDpiY()
  to find the resolution of the display. If the pageSize is invalid,
  an error message is printed, and an undefined value is returned.
      
  @param height target height in pixels

  @returns the zoom value required to scale the page size down to
  'height' pixels. If the pageSize is invalid, an undefined value is
  returned.
  */
  double      zoomForHeight(Q_UINT32 height) const;
  
  /** \brief Zoom value required to scale to a certain height 

  If the pageSize is valid, this method returns the zoom value
  required to scale the page size down to 'width' pixels on the
  currently used display. The method uses QPaintDevice::x11AppDpiX()
  to find the resolution of the display. If the pageSize is invalid,
  an error message is printed, and an undefined value is returned.
  
  @param width target width in pixels
  
  @returns the zoom value required to scale the page size down to
  'width' pixels. If the pageSize is invalid, an undefined value is
  returned.
  */
  double      zoomForWidth(Q_UINT32 width) const;

  /** \brief Returns a zoom to fit into a certain page size

  This method computes the larget zoom value such that *this, zoomed
  by the computed values fits into the page size 'target'. If *this or
  if target are invalid, or is this->isSmall() is true, an undefined
  value is returned. If height or width of this is nearly 0.0, a
  floating point exception may occur.
  */
  double      zoomToFitInto(const SimplePageSize &target) const;
  
  /** \brief Validity check
      
  @returns 'True' if the page width and height are both larger than
  1mm */
  bool isValid() const { return ( (pageWidth.getLength_in_mm() > 1.0) && (pageHeight.getLength_in_mm() > 1.0) ); }

  /** \brief Validity check: 
      
  @returns 'True' if the page ares is less than 1.0 square mm
  */
  bool isSmall() const { return (pageWidth.getLength_in_mm()*pageHeight.getLength_in_mm() < 1.0); }
  
  /** \brief Approximate equality

  @param size pageSize object to compare this object with

  @returns 'True' if height and width of the two obejcts differ by at
      most 2mm, 'false' otherwise
  */
  bool isNearlyEqual(const SimplePageSize &size) const {return (pageWidth.isNearlyEqual(size.pageWidth) && pageHeight.isNearlyEqual(size.pageHeight)); }

  /** Test if paper size is higher than wide

  @returns 'True' if the paper size is higher than wide
   */
  bool isPortrait() const { return (pageHeight >= pageWidth);}

  /** Rotates by 90 degrees

  @returns a SimplePageSize with height and width swapped. The
  original instance is unchanged.
   */
  SimplePageSize rotate90() const { return  SimplePageSize(pageHeight, pageWidth);}
  
 protected:
  Length pageWidth;
  Length pageHeight;
};


#endif
