//============================================================================
//
// Ordinary differential equation solver using the Runge-Kutta method.
// $Id: rkodesolver.h 304354 2004-04-16 23:42:39Z kniederk $
// Copyright (C) 2004 Georg Drenkhahn
//
// This file is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License version 2 as published by the
// Free Software Foundation.
//
//============================================================================

#ifndef RKODESOLVER_H
#define RKODESOLVER_H

// STL headers
#include <valarray>

/** @brief Solver class to integrate a first-order ordinary differential
 * equation (ODE) by means of a 6. order Runge-Kutta method.
 *
 * The ODE system must be given as the derivative
 * dy/dx = f(x,y)
 * with x in R and y in R^n.
 *
 * Within this class the function f() is a pure virtual function, which must be
 * reimplemented in a derived class.
 *
 * No other special data type for vectors or matrices are needed besides the STL
 * class std::valarray. */
template<typename T>
class RkOdeSolver
{
  public:
   /** @brief Constructor
    * @param x Initial integration parameter
    * @param y Initial function values of function to integrate
    * @param dx Initial guess for step size.  Will be automatically adjusted to
    * guarantee required precision.
    * @param eps Relative precision
    *
    * Initialises the solver with start conditions. */
   RkOdeSolver(const T& x=0.0,
               const std::valarray<T>& y=std::valarray<T>(0),
               const T& dx=0,
               const T& eps=1e-6);
   /** @brief Destructor */
   virtual ~RkOdeSolver(void);

   /** @brief Integrates the ordinary differential equation from the current x
    * value to x+@a dx.
    * @param dx x-interval size to integrate over starting from x.  dx may be
    * negative.
    *
    * The integration is performed by calling rkStepCheck() repeatedly until the
    * desired x value is reached. */
   void integrate(const T& dx);

   // Accessors

   // get/set x value
   /** @brief Get current x value.
    * @return Reference of x value. */
   const T& X(void) const;
   /** @brief Set current x value.
    * @param a The value to be set. */
   void X(const T& a);

   // get/set y value
   /** @brief Get current y value.
    * @return Reference of y vector. */
   const std::valarray<T>& Y(void) const;
   /** @brief Set current y values.
    * @param a The vector to be set. */
   void Y(const std::valarray<T>& a);

   /** @brief Get current dy/dx value.
    * @return Reference of dy/dx vector. */
   const std::valarray<T>& dYdX(void) const;

   // get/set dx value
   /** @brief Get current estimated step size dX.
    * @return Reference of dX value. */
   const T& dX(void) const;
   /** @brief Set estimated step size dX.
    * @param a The value to be set. */
   void dX(const T& a);

   // get/set eps value
   /** @brief Get current presision.
    * @return Reference of precision value. */
   const T& Eps(void) const;
   /** @brief Set estimated presision.
    * @param a The value to be set. */
   void Eps(const T& a);

  protected:
   // purely virtual function which is integrated
   /** @brief ODE function
    * @param x Integration value
    * @param y Function value
    * @return Derivation
    *
    * This purely virtual function returns the value of dy/dx for the given
    * parameter values of x and y. */
   virtual std::valarray<T>
      f(const T& x, const std::valarray<T>& y) const = 0;

  private:
   /** @brief Perform one integration step with a tolerable relative error given
    * by ::mErr.
    * @param dx Maximal step size, may be positive or negative depending on
    * integration direction.
    * @return Flag indicating if made absolute integration step was equal |@a dx
    * | (true) less than |@a dx | (false).
    *
    * A new estimate for the maximum next step size is saved to ::mStep.  The
    * new values for x, y and f are saved in ::mX, ::mY and ::mDydx. */
   bool rkStepCheck(const T& dx);
   /** @brief Perform one Runge-Kutta integration step forward with step size
    * ::mStep
    * @param dx Step size relative to current x value.
    * @param yerr Reference to vector in which the estimated error made in y is
    * returned.
    * @return The y value after the step at x+@a dx.
    *
    * Stored current x,y values are not adjusted. */
   std::valarray<T> rkStep(const T& dx, std::valarray<T>& yerr) const;

   /** current x value */
   T mX;
   /** current y value */
   std::valarray<T> mY;
   /** current value of dy/dx */
   std::valarray<T> mDydx;

   /** allowed relative error */
   T mEps;
   /** estimated step size for next Runge-Kutta step */
   T mStep;
};

// inline accessors

template<typename T>
inline const T&
RkOdeSolver<T>::X(void) const
{
   return mX;
}

template<typename T>
inline void
RkOdeSolver<T>::X(const T &a)
{
   mX = a;
}

template<typename T>
inline const std::valarray<T>&
RkOdeSolver<T>::Y(void) const
{
   return mY;
}

template<typename T>
inline const std::valarray<T>&
RkOdeSolver<T>::dYdX(void) const
{
   return mDydx;
}

template<typename T>
inline const T&
RkOdeSolver<T>::dX(void) const
{
   return mStep;
}

template<typename T>
inline const T&
RkOdeSolver<T>::Eps(void) const
{
   return mEps;
}

#endif
