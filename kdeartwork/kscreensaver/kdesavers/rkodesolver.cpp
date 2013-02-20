//============================================================================
//
// Ordinary differential equation solver using the Runge-Kutta method.
// $Id: rkodesolver.cpp 305067 2004-04-20 05:19:52Z hausmann $
// Copyright (C) 2004 Georg Drenkhahn
//
// This file is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License version 2 as published by the
// Free Software Foundation.
//
//============================================================================

#include <kdebug.h>
#include "rkodesolver.h"

template<typename T>
RkOdeSolver<T>::RkOdeSolver(const T &x,
                            const std::valarray<T>& y,
                            const T &dx,
                            const T &eps)
   : mX(x)
{
   Y(y);
   dX(dx);
   Eps(eps);
}

// virtual dtor
template<typename T>
RkOdeSolver<T>::~RkOdeSolver(void)
{}

// accessors

template<typename T>
void
RkOdeSolver<T>::dX(const T &a)
{
   if (a < 0.0)
   {
      kdDebug() << "RkOdeSolver: dx was negative, made it positive" << endl;
      mStep = -a;
   }
   else if (a == 0.0)
   {
      mStep = 0.001;            // a very arbitrary value
      kdDebug() << "RkOdeSolver: dx == 0, set it to " << mStep << endl;
   }
   else
   {
      mStep = a;
   }
}

template<typename T>
void
RkOdeSolver<T>::Eps(const T &a)
{
   if (a < 0.0)
   {
      kdDebug() << "RkOdeSolver: eps was negative, made it positive" << endl;
      mEps = -a;
   }
   else if (a == 0.0)
   {
      mEps = 1e-5;              // a very arbitrary value
      kdDebug() << "RkOdeSolver: eps == 0, set it to 1e-5" << endl;
   }
   else
   {
      mEps = a;
   }
}

template<typename T>
void
RkOdeSolver<T>::Y(const std::valarray<T> &a)
{
   mY.resize(a.size());
   mY = a;
}


// public member functions

template<typename T>
void
RkOdeSolver<T>::integrate(const T &deltaX)
{
   if (deltaX == 0)
   {
      return;                   // nothing to integrate
   }

   // init dydx if uninitialised
   if (mDydx.size() != mY.size())
   {
      mDydx.resize(mY.size());
      mDydx = f(mX,mY);
   }

   static const unsigned int maxiter = 10000;
   const T x2 = mX + deltaX;

   unsigned int iter;
   for (iter=0;
        iter<maxiter && rkStepCheck(x2-mX) == false;
        ++iter)
   {}

   if (iter>maxiter)
   {
      kdDebug() << "RkOdeSolver: More than " << maxiter
                << " iterations in RkOdeSolver::integrate" << endl;
      // TODO throw exeption here
   }
}


// private member functions

template<typename T>
bool
RkOdeSolver<T>::rkStepCheck(const T& dx_requested)
{
   static const T safety =  0.9;
   static const T pshrnk = -0.25;
   static const T pgrow  = -0.2;

   // reduce step size by no more than a factor 10
   static const T shrinkLimit = 0.1;
   // enlarge step size by no more than a factor 5
   static const T growthLimit = 5.0;
   // errmax_sl = 6561.0
   static const T errmax_sl = pow(shrinkLimit/safety, 1.0/pshrnk);
   // errmax_gl = 1.89e-4
   static const T errmax_gl = pow(growthLimit/safety, 1.0/pgrow);

   static const unsigned int maxiter = 100;

   if (dx_requested == 0)
   {
      return true;              // integration done
   }

   std::valarray<T> ytmp(mY.size());
   std::valarray<T> yerr(mY.size());
   std::valarray<T> t(mY.size());

   bool stepSizeWasMaximal;
   T dx;
   if (std::abs(dx_requested) > mStep)
   {
      stepSizeWasMaximal = true;
      dx = dx_requested>0 ? mStep : -mStep;
   }
   else
   {
      stepSizeWasMaximal = false;
      dx = dx_requested;
   }

   // generic scaling factor
   std::valarray<T> yscal = std::abs(mY) + std::abs(dx*mDydx) + 1e-15;

   unsigned int iter = 0;
   T errmax = 0;
   do
   {
      if (errmax >= 1.0)
      {
         // reduce step size
         dx *= errmax<errmax_sl ? safety * pow(errmax, pshrnk) : shrinkLimit;
         stepSizeWasMaximal = true;
         if (mX == mX + dx)
         {
            // stepsize below numerical resolution
            kdDebug() << "RkOdeSolver: stepsize underflow in rkStepCheck"
                      << endl;
            // TODO throw exeption here
            exit(0);
         }
         // new dx -> update scaling vector
         yscal = std::abs(mY) + std::abs(dx*mDydx) + 1e-15;
      }

      ytmp   = rkStep(dx, yerr); // try to make a step forward
      t      = std::abs(yerr/yscal); // calc the error vector
      errmax = t.max()/mEps;    // calc the rel. maximal error
      ++iter;
   } while (iter < maxiter && errmax >= 1.0);

   if (iter >= maxiter)
   {
      kdDebug() << "RkOdeSolver: too many iterations in rkStepCheck" << endl;
      // TODO throw exeption here
      exit(0);
   }

   if (stepSizeWasMaximal == true)
   {
      // estimate next step size if used step size was maximal
      mStep  =
         std::abs(dx)
         * (errmax>errmax_gl ? safety * pow(errmax, pgrow) : growthLimit);
   }
   mX    += dx;                 // make step forward
   mY     = ytmp;               // save new function values
   mDydx  = f(mX,mY);           // and update derivatives

   return std::abs(dx) < std::abs(dx_requested);
}

template<typename T>
std::valarray<T>
RkOdeSolver<T>::rkStep(const T& dx, std::valarray<T>& yerr) const
{
   static const T
      a2=0.2, a3=0.3, a4=0.6, a5=1.0, a6=0.875,
      b21=0.2,
      b31=3.0/40.0,       b32=9.0/40.0,
      b41=0.3,            b42=-0.9,        b43=1.2,
      b51=-11.0/54.0,     b52=2.5,         b53=-70.0/27.0, b54=35.0/27.0,
      b61=1631.0/55296.0, b62=175.0/512.0, b63=575.0/13824.0,
      b64=44275.0/110592.0, b65=253.0/4096.0,
      c1=37.0/378.0, c3=250.0/621.0, c4=125.0/594.0, c6=512.0/1771.0,
      dc1=c1-2825.0/27648.0,  dc3=c3-18575.0/48384.0,
      dc4=c4-13525.0/55296.0, dc5=-277.0/14336.0, dc6=c6-0.25;

   std::valarray<T> ak2 = f(mX + a2*dx,
                            mY + dx*b21*mDydx);             // 2. step
   std::valarray<T> ak3 = f(mX + a3*dx,
                            mY + dx*(b31*mDydx + b32*ak2)); // 3.step
   std::valarray<T> ak4 = f(mX + a4*dx,
                            mY + dx*(b41*mDydx + b42*ak2
                                     + b43*ak3));           // 4.step
   std::valarray<T> ak5 = f(mX + a5*dx,
                            mY + dx*(b51*mDydx + b52*ak2
                                     + b53*ak3 + b54*ak4)); // 5.step
   std::valarray<T> ak6 = f(mX + a6*dx,
                            mY + dx*(b61*mDydx + b62*ak2
                                     + b63*ak3 + b64*ak4
                                     + b65*ak5));           // 6.step
   yerr      = dx*(dc1*mDydx + dc3*ak3 + dc4*ak4 + dc5*ak5 + dc6*ak6);
   return mY + dx*( c1*mDydx +            c3*ak3 +  c4*ak4 +  c6*ak6);
}


// explicite instantiations
//template RkOdeSolver<long double>;
template class RkOdeSolver<double>;
//template RkOdeSolver<float>;
