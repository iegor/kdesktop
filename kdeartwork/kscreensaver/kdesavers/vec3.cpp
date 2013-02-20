//============================================================================
//
// 3-dim real vector class
// $Id: vec3.cpp 324707 2004-06-29 19:44:18Z fawcett $
// Copyright (C) 2004 Georg Drenkhahn
//
// This file is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License version 2 as published by the
// Free Software Foundation.
//
//============================================================================

#include <math.h>
#if !defined(NAN)
static inline double nan__()
{
    static const unsigned int one = 1;
    static const bool BigEndian = (*((unsigned char *) &one) == 0);

    static const unsigned char be_nan_bytes[] = { 0x7f, 0xf8, 0, 0, 0, 0, 0, 0 };
    static const unsigned char le_nan_bytes[] = { 0, 0, 0, 0, 0, 0, 0xf8, 0x7f };

    return *( ( const double * )( BigEndian ? be_nan_bytes : le_nan_bytes ) );
}
#   define NAN (::nan__())
#endif

#include <config.h>

#ifdef HAVE_NUMERIC_LIMITS
#include <limits>
#endif

#include <cerrno>
#include "vec3.h"

template<typename T>
vec3<T>& vec3<T>::normalize()
{
   T n = norm(*this);
   if (n != 0)
   {
      (*this) /= n;
   }
   else
   {
      errno = EDOM;             // indicate domain error
      // TODO: throw an exception?
   }
   return *this;
}

template<typename T>
vec3<T>& vec3<T>::rotate(const vec3<T>& r)
{
   T phi = norm(r);
   if (phi != 0)
   {
      // part of vector which is parallel to r
      vec3<T> par(r*(*this)/(r*r) * r);
      // part of vector which is perpendicular to r
      vec3<T> perp(*this - par);
      // rotation direction, size of perp
      vec3<T> rotdir(norm(perp) * normalized(crossprod(r,perp)));
      *this = par + cos(phi)*perp + sin(phi)*rotdir;
   }
   return *this;
}

/*--- static member functions ---*/

template<typename T>
T vec3<T>::cos_angle(const vec3<T>& a, const vec3<T>& b)
{
   T den = norm(a) * norm(b);
   T ret = 0;
   // if |a|=0 or |b|=0 then angle is not defined.  We return NAN in this case.
   if (den != 0.0)
   {
      ret = a*b/den;
   }
   else
   {
      errno = EDOM;             // indicate domain error
#ifdef HAVE_NUMERIC_LIMITS
      // TODO test
      ret = std::numeric_limits<T>::quiet_NaN();
#else
      ret = NAN;                // return NAN from ISO C99
#endif
   }
   return ret;
}

template<typename T>
T vec3<T>::angle(const vec3<T>& a, const vec3<T>& b, const vec3<T>& c)
{
   // if |a|=0 or |b|=0 then angle is not defined.  We return NAN in this case.
   T ang = vec3<T>::angle(a,b);
   return (crossprod(a,b)*c<0) ?
      T(2.*M_PI)-ang : ang;
}

// explicite instantiation
template class vec3<double>;
