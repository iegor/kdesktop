//============================================================================
//
// 3-dim real vector class
// $Id: vec3.h 304354 2004-04-16 23:42:39Z kniederk $
// Copyright (C) 2004 Georg Drenkhahn
//
// This file is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License version 2 as published by the
// Free Software Foundation.
//
//============================================================================

#ifndef VEC3_H
#define VEC3_H

#include <valarray>

/** @brief 3-dimensional real vector
 *
 * Implements regular 3 dimensional (space) vectors including the common inner
 * scalar product (2 norm) and the cross product.  @a T may be any integer or
 * float data type which is an acceptable template argument of std::valarray. */
template<typename T>
class vec3 : public std::valarray<T>
{
  public:
   /** Default constructor */
   vec3();
   /** Constructor with initial element values */
   vec3(const T&, const T&, const T&);
   /** Copy constructor */
   vec3(const std::valarray<T>&);
   /** Copy constructor */
   vec3(const std::slice_array<T>&);

   /** Normalize the vector to have a norm of 1.  @return Normalized vector if
    * length is non-zero and otherwise the zero vector. */
   vec3& normalize();

   /** Rotate the vector (*this) in positive mathematical direction around the
    * direction given by @a r.  The norm of @a r specifies the rotation angle in
    * radians.
    * @param r Rotation vector.
    * @return Rotated vector. */
   vec3& rotate(const vec3& r);

   /*--- static funcions ---*/

   /** @param a first vector
    * @param b second vector
    * @return Cosine of the angle between @a a and @a b.  If norm(@a a)==0 or
    * norm(@a b)==0 the global variable errno is set to EDOM and NAN (or
    * std::numeric_limits<T>::quiet_NaN()) is returned. */
   static T cos_angle(const vec3& a, const vec3& b);

   /** @brief Returns the angle between vectors @c a and @a b but with respect
    * to a preferred rotation direction @a c.
    *
    * @param a First vector for angle.  Must be | @a a |>0 otherwises NAN is
    * returned.
    * @param b Second vector for angle.  Must be | @a b |>0 otherwises NAN is
    * returned.
    * @param c Indicates the rotation direction. @a c can be any vector which is
    * not part of the plane spanned by @a a and @a b.  If | @a c | = 0 the
    * smalest possible angle angle is returned.
    * @return Angle in radians between 0 and 2*Pi or NAN if | @a a |=0 or | @a b
    * |=0.
    *
    * For @a a not parallel to @a b and @a a not antiparallel to @a b the 2
    * vectors @a a,@a b span a unique plane in the 3-dimensional space.  Let @b
    * n<sub>1</sub> and @b n<sub>2</sub> be the two possible normal vectors for
    * this plane with |@b n<sub>i</sub> |=1, i={1,2} and @b n<sub>1</sub> = -@b
    * n<sub>2</sub> .
    *
    * Let further @a a and @a b enclose an angle alpha in [0,Pi], then there is
    * one i in {1,2} so that (alpha*@b n<sub>i</sub> x @a a) * @a b = 0.  This
    * means @a a rotated by the rotation vector alpha*@b n<sub>i</sub> is
    * parallel to @a b.  One could also rotate @a a by (2*Pi-alpha)*(-@b
    * n<sub>i</sub>) to acomplish the same transformation with
    * ((2*Pi-alpha)*(-@b n<sub>i</sub>) x @a a) * @a b = 0
    *
    * The vector @a c defines the direction of the normal vector to take as
    * reference.  If @a c * @b n<sub>i</sub> > 0 alpha is returned and otherwise
    * 2*Pi-alpha.  If @a a parallel to @a b or @a a parallel to @a b the choice
    * of @a c does not matter. */
   static T angle(const vec3& a, const vec3& b, const vec3& c);

   /*--- static inline funcions ---*/

   /** Norm of argument vector.
    * @param a vector.
    * @return | @a a | */
   static T norm(const vec3& a);

   /** Angle between @a a and @a b.
    * @param a fist vector.  Must be | @a a | > 0 otherwises NAN is returned.
    * @param b second vector.  Must be | @a b | > 0 otherwises NAN is returned.
    * @return Angle in radians between 0 and Pi or NAN if | @a a | = 0 or | @a b
    * | = 0. */
   static T angle(const vec3& a, const vec3& b);

   /** Cross product of @a a and @a b.
    * @param a fist vector.
    * @param b second vector.
    * @return Cross product of argument vectors @a a x @a b. */
   static vec3 crossprod(const vec3& a, const vec3& b);

   /** Normalized version of argument vector.
    * @param a vector.
    * @return @a a / | @a a | for | @a a | > 0 and otherwise the zero vector
    * (=@a a).  In the latter case the global variable errno is set to EDOM. */
   static vec3 normalized(vec3 a);
};

/*--- inline member functions ---*/

template<typename T>
inline vec3<T>::vec3()
   : std::valarray<T>(3)
{}

template<typename T>
inline vec3<T>::vec3(const T& a, const T& b, const T& c)
   : std::valarray<T>(3)
{
   (*this)[0] = a;
   (*this)[1] = b;
   (*this)[2] = c;
}

template<typename T>
inline vec3<T>::vec3(const std::valarray<T>& a)
   : std::valarray<T>(a)
{
}

template<typename T>
inline vec3<T>::vec3(const std::slice_array<T>& a)
   : std::valarray<T>(a)
{
}

/*--- inline non-member operators ---*/

/** @param a first vector summand
 * @param b second vector summand
 * @return Sum vector of vectors @a a and @a b. */
template<typename T>
inline vec3<T> operator+(vec3<T> a, const vec3<T>& b)
{
   a += b;   /* valarray<T>::operator+=(const valarray<T>&) */
   return a;
}

/** @param a first vector multiplicant
 * @param b second vector multiplicant
 * @return Scalar product of vectors @a a and @a b. */
template<typename T>
inline T operator*(vec3<T> a, const vec3<T>& b)
{
   a *= b;   /* valarray<T>::operator*=(const T&) */
   return a.sum();
}

/** @param a scalar multiplicant
 * @param b vector operand
 * @return Product vector of scalar @a a and vector @a b. */
template<typename T>
inline vec3<T> operator*(const T& a, vec3<T> b)
{
   b *= a;    /* valarray<T>::operator*=(const T&) */
   return b;
}

/** @param a vector operand
 * @param b scalar multiplicant
 * @return Product vector of scalar @a b and vector @a a. */
template<typename T>
inline vec3<T> operator*(vec3<T> a, const T& b)
{
   return b*a; /* vec3<T>::operator*(const T&, vec3<T>) */
}

/*--- static inline funcions ---*/

template<typename T>
inline T vec3<T>::norm(const vec3<T>& a)
{
   return sqrt(a*a);
}

template<typename T>
inline T vec3<T>::angle(const vec3<T>& a, const vec3<T>& b)
{
   // returns NAN if cos_angle() returns NAN (TODO: test this case)
   return acos(cos_angle(a,b));
}

template<typename T>
inline vec3<T> vec3<T>::crossprod(const vec3<T>& a, const vec3<T>& b)
{
   return vec3<T>(
      a[1]*b[2] - a[2]*b[1],
      a[2]*b[0] - a[0]*b[2],
      a[0]*b[1] - a[1]*b[0]);
}

template<typename T>
inline vec3<T> vec3<T>::normalized(vec3<T> a)
{
   return a.normalize();
}

#endif
