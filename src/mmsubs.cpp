/***************************************************************************
                          mmsubs.cpp  -  description
                             -------------------
    begin                : Sat Nov 2 2002
    copyright            : (C) 2002 by
    email                : mmdigitizer@gmail.com

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <math.h>
#include <stdlib.h>

#include <qmutex.h>

//#include <qmessagebox.h>
//#include <qstring.h>

#include "main.h"
#include "mmsubs.h"

#define max(a,b) ((a>b)?a:b)

const double PI = 3.1415926535;
const double DOUBLEONE = (double) 1;

/*******************************************************************************/
int adjust_count (bool linearAxis, double start, double stop,
  double inc)
{
  int count;
  if (linearAxis)
    count = (int) (1 + (stop - start) / inc);
  else
  {
    if ((start <= 0) || (inc <= 0.0))
      count = 1;
    else
      count = (int) (1 + log10 (stop / start) / log10 (inc));
  }
  return count;
}

/*******************************************************************************/
double adjust_inc (bool linearAxis, int roundoffPower, double start, double stop,
  int count)
{
  double inc;
  if (linearAxis)
  {
    if (count > 1)
      inc = (stop - start) / (count - 1);
    else
      inc = stop - start;
  }
  else
  {
    if (start <= 0.0) // eliminate divide by zero
      inc = 1.0;
    else
    {
      if (count > 1)
        inc = pow (stop / start, (double) 1.0 / (count - 1));
      else
        inc = stop / start;
    }
  }
  return roundofftopower (inc, roundoffPower);
}

/*******************************************************************************/
double adjust_start (bool linearAxis, int roundoffPower, double stop, double inc,
  int count)
{
  double start;
  if (linearAxis)
    start = stop - inc * (count - 1);
  else
    start = stop / pow (inc, (double) (count - 1));
  return roundofftopower (start, roundoffPower);
}

/*******************************************************************************/
double adjust_stop (bool linearAxis, int roundoffPower, double start, double inc,
  int count)
{
  double stop;
  if (linearAxis)
    stop = start + inc * (count - 1);
  else

    stop = start * pow (inc, (double) (count - 1));
  return roundofftopower (stop, roundoffPower);
}

/*******************************************************************************/
double angleleft (double x0, double y0, double x1, double y1, double x2, double y2)
{
  // return angle on the left side of the vertex at the juntion of line segments
  // (x0,y0) to (x1,y1), and (x1,y1) to (x2,y2)
  double vectora [3], vectorb [3], vectoraxb [3];
  vectora [0] = x1 - x0;
  vectora [1] = y1 - y0;
  vectora [2] = 0;
  vectorb [0] = x2 - x1;
  vectorb [1] = y2 - y1;
  vectorb [2] = 0;
  dcross (vectora, vectorb, vectoraxb);
  if (vectoraxb [2] > 0)
    return PI - dangle (vectora, vectorb);
  else
    return PI + dangle (vectora, vectorb);
}

/*******************************************************************************/
double angleright (double x0, double y0, double x1, double y1, double x2, double y2)
{
  // return angle on the right side of the vertex at the juntion of line segments
  // (x0,y0) to (x1,y1), and (x1,y1) to (x2,y2)
  return 2.0 * PI - angleleft (x0, y0, x1, y1, x2, y2);
}

/*******************************************************************************/
void axisScale(double x_min_in, double x_max_in, bool linearAxis,
  double* x_start, double* x_stop, double* x_delta, int* x_count)
// given the min and max values on an axis, picks start, delta and stop
// values that nicely bound the low and high values. key in algorithm
// is to minimize number of significant digits in start, delta and stop
{
  double x_min, x_max;
  const double range_epsilon = 0.00000000001;
  double x_average, x_average_rounded_up, x_range;
  int ndigit_range;

  // define number of digits of precision. although value of 10 seems
  // desirable, the sprintf statements elsewhere in this file, which
  // operate on values with the specified precision, just lose it
  // for more than 8 digits. example '%.7lg' on 40.000005 gives 40.00001
  const int nDigitsPrecision = 8;

  // sort the input values
  if (x_min_in > x_max_in)
  {
    x_min = x_max_in;
    x_max = x_min_in;
  }
  else
  {
    x_min = x_min_in;
    x_max = x_max_in;
  }

  // scale the coordinates logarithmically if log flag is set
  if (!linearAxis)
  {
    ASSERT_ENGAUGE(x_min > 0);
    ASSERT_ENGAUGE(x_max > 0);
    x_min = log10(x_min);
    x_max = log10(x_max);
  }

  // round off average to first significant digit of range
  x_average = (x_min + x_max) / 2.0;
  x_range = x_max - x_min;
  if (x_range == 0)
  x_range = dabs (x_average / 10.0); // for null range use arbitrary range
  ndigit_range = valuePower (x_range);
  *x_delta = pow ((double) 10.0, (double) ndigit_range);
  x_average_rounded_up = *x_delta * floor ((x_average + *x_delta / 2.0) / *x_delta);


  if (x_range > range_epsilon)
  {
    // adjust stepsize if more points are needed, accounting for roundoff
    while (dabs (x_range / *x_delta) <= 2.000001)
      *x_delta /= 2.0;
  }

  // go down until min point is included
  *x_start = x_average_rounded_up;
  while (*x_start > x_min)
    *x_start -= *x_delta;

  // go up until max point is included
  *x_stop = x_average_rounded_up;
  while (*x_stop < x_max)
    *x_stop += *x_delta;

  *x_count = 1 + (int) floor ((*x_stop - *x_start) / *x_delta + 0.5);

  if (!linearAxis)
  {
    // convert from log scale back to linear scale
    *x_start = pow((double) 10.0, *x_start);
    *x_stop = pow((double) 10.0, *x_stop);
    *x_delta = pow((double) 10.0, *x_delta);
  }

  // roundoff to eliminate epsilons of 10^-10
  int roundoffPower = valuePower (*x_delta) - nDigitsPrecision;
  *x_start = roundofftopower(*x_start, roundoffPower);
  *x_stop = roundofftopower(*x_stop, roundoffPower);
  *x_delta = roundofftopower(*x_delta, roundoffPower);
}

/*******************************************************************************/
bool clippedsegmentisvisible(double xTopLeft, double yTopLeft, double xTopRight,
  double yTopRight, double xBottomRight, double yBottomRight, double xBottomLeft,
  double yBottomLeft, double* xStart, double* yStart, double* xStop, double* yStop)
{
  // clip the line segment within a quadrilateral, then return true if it is
  // still visible. ASSUMPTION - quadrilateral is convex
  bool startInside, stopInside;
  startInside = pointinsidebox(*xStart, *yStart, xTopLeft, yTopLeft, xTopRight,
    yTopRight, xBottomRight, yBottomRight, xBottomLeft, yBottomLeft);
  stopInside = pointinsidebox(*xStop, *yStop, xTopLeft, yTopLeft, xTopRight,
    yTopRight, xBottomRight, yBottomRight, xBottomLeft, yBottomLeft);

  if (startInside)
  {
    if (stopInside)
      return true;
    else
    {
      // need to clip since stop point is outside of quadrilateral
      double sLine1, sLine2;
      if (intersecttwolines (*xStart, *yStart, *xStop, *yStop,
        xTopLeft, yTopLeft, xTopRight, yTopRight, &sLine1, &sLine2) &&
      (0.0 <= sLine1) && (sLine1 <= 1.0) && (0.0 <= sLine2) && (sLine2 <= 1.0))
      {
        *xStop = (1 - sLine1) * (*xStart) + sLine1 * (*xStop);
        *yStop = (1 - sLine1) * (*yStart) + sLine1 * (*yStop);
      }
      else if (intersecttwolines (*xStart, *yStart, *xStop, *yStop,
        xTopRight, yTopRight, xBottomRight, yBottomRight, &sLine1, &sLine2) &&
      (0.0 <= sLine1) && (sLine1 <= 1.0) && (0.0 <= sLine2) && (sLine2 <= 1.0))
      {
        *xStop = (1 - sLine1) * (*xStart) + sLine1 * (*xStop);
        *yStop = (1 - sLine1) * (*yStart) + sLine1 * (*yStop);
      }
      else if (intersecttwolines (*xStart, *yStart, *xStop, *yStop,
        xBottomRight, yBottomRight, xBottomLeft, yBottomLeft, &sLine1, &sLine2) &&
      (0.0 <= sLine1) && (sLine1 <= 1.0) && (0.0 <= sLine2) && (sLine2 <= 1.0))
      {
        *xStop = (1 - sLine1) * (*xStart) + sLine1 * (*xStop);
        *yStop = (1 - sLine1) * (*yStart) + sLine1 * (*yStop);
      }
      else if (intersecttwolines (*xStart, *yStart, *xStop, *yStop,
        xBottomLeft, yBottomLeft, xTopLeft, yTopLeft, &sLine1, &sLine2) &&
      (0.0 <= sLine1) && (sLine1 <= 1.0) && (0.0 <= sLine2) && (sLine2 <= 1.0))
      {
        *xStop = (1 - sLine1) * (*xStart) + sLine1 * (*xStop);
        *yStop = (1 - sLine1) * (*yStart) + sLine1 * (*yStop);
      }
      return true;
    }
  }
  else if (stopInside)
  {
    // need to clip since start point is outside of quadrilateral
    double sLine1, sLine2;
    if (intersecttwolines (*xStart, *yStart, *xStop, *yStop,
      xTopLeft, yTopLeft, xTopRight, yTopRight, &sLine1, &sLine2) &&
      (0.0 <= sLine1) && (sLine1 <= 1.0) && (0.0 <= sLine2) && (sLine2 <= 1.0))
    {
      *xStart = (1 - sLine1) * (*xStart) + sLine1 * (*xStop);
      *yStart = (1 - sLine1) * (*yStart) + sLine1 * (*yStop);
    }
    else if (intersecttwolines (*xStart, *yStart, *xStop, *yStop,
      xTopRight, yTopRight, xBottomRight, yBottomRight, &sLine1, &sLine2) &&
      (0.0 <= sLine1) && (sLine1 <= 1.0) && (0.0 <= sLine2) && (sLine2 <= 1.0))
    {
      *xStart = (1 - sLine1) * (*xStart) + sLine1 * (*xStop);
      *yStart = (1 - sLine1) * (*yStart) + sLine1 * (*yStop);
    }
    else if (intersecttwolines (*xStart, *yStart, *xStop, *yStop,
      xBottomRight, yBottomRight, xBottomLeft, yBottomLeft, &sLine1, &sLine2) &&
      (0.0 <= sLine1) && (sLine1 <= 1.0) && (0.0 <= sLine2) && (sLine2 <= 1.0))
    {
      *xStart = (1 - sLine1) * (*xStart) + sLine1 * (*xStop);
      *yStart = (1 - sLine1) * (*yStart) + sLine1 * (*yStop);
    }
    else if (intersecttwolines (*xStart, *yStart, *xStop, *yStop,
      xBottomLeft, yBottomLeft, xTopLeft, yTopLeft, &sLine1, &sLine2) &&
      (0.0 <= sLine1) && (sLine1 <= 1.0) && (0.0 <= sLine2) && (sLine2 <= 1.0))
    {
      *xStart = (1 - sLine1) * (*xStart) + sLine1 * (*xStop);
      *yStart = (1 - sLine1) * (*yStart) + sLine1 * (*yStop);
    }
    return true;
  }
  else
    // no part of line segment is visible
    return false;
}

/*******************************************************************************/
bool compute_linear_to_log (double* v, double vMin, double vMax)
//
// converts linear scale coordinate to log scale coordinate.
// example: vmin=1, v=50.5, vmax=100 gives v=10
{
  if (*v <= 0)
    return false;
  if (vMin <= 0)
    return false;
  if (vMax <= 0)
    return false;
  if (vMin >= vMax)
    return false;

  *v = pow((double) 10.0, log10(vMin) + (log10(vMax) - log10(vMin)) *
  (*v - vMin) / (vMax - vMin));

  return true;
}

/*******************************************************************************/
bool compute_log_to_linear (double* v, double vMin, double vMax)
//
// converts log scale coordinate to linear scale coordinate.
// example: vmin=1, v=10, vmax=100 gives v=50.5
{
  if (*v <= 0)
    return false;
  if (vMin <= 0)
    return false;
  if (vMax <= 0)
    return false;
  if (vMin >= vMax)

    return false;

  *v = vMin + (vMax - vMin) * (log10(*v) - log10(vMin)) /
  (log10(vMax) - log10(vMin));


  return true;
}

/*******************************************************************************/
int compute_screen_to_graph (double r_graph [3] [3], double r_screen [3] [3],
                   double a [3] [3], double a_inverse [3] [3])
// computes the transformation to get graph coordinates given
// screen coordinates, where
//    G      S
//   r  = T r
//
// since this transformation is defined using three points (each in both
// graph and screen coordinates), we are solving
//      G   G   G         S   S   S
//   (r1  r2  r3 ) = T (r1  r2  r3 )
// so
//          -1
//   T = G S
//
// we will assume that the computed transformation has all of the appropriate
// y sign convention transformations, skews, scalings, rotations and translations
//
// note that we are NOT using (x,y,z) coordinates where z=constant. instead, so
// that 3x3 transformations can perform translation, we ARE using (x,y,1) coordinates
{
  int iworkn [3];
  double workn [3], worknn [3] [3];
  double r_screen_inverse [3] [3];
#ifdef NDEBUG
  double r_screen_debug [3] [3];
  double r_graph_debug [3] [3];
  double unity_matrix [3] [3];
#endif

  ASSERT_ENGAUGE(r_graph [2] [0] == 1.0);
  ASSERT_ENGAUGE(r_graph [2] [1] == 1.0);
  ASSERT_ENGAUGE(r_graph [2] [2] == 1.0);

  ASSERT_ENGAUGE(r_screen [2] [0] == 1.0);
  ASSERT_ENGAUGE(r_screen [2] [1] == 1.0);
  ASSERT_ENGAUGE(r_screen [2] [2] == 1.0);

  if (!dinverse((double*) r_screen, 3, 3, iworkn, (double*) r_screen_inverse,
    workn, (double*) worknn))
    return S2G_BAD_SCREEN_COORDINATES;

  dmlt3x3(a, r_graph, r_screen_inverse);

  if (!dinverse((double*) a, 3, 3, iworkn, (double*) a_inverse,
    workn, (double*) worknn))
    return S2G_BAD_GRAPH_COORDINATES;

#ifdef NDEBUG
/*  for debugging, verify that original graph vectors actually get converted to
  the original screen vectors, and vice versa. also verify a was inverted */
  dmlt3x3(r_graph_debug, a, r_screen);
  dmlt3x3(r_screen_debug, a_inverse, r_graph);

  dmlt3x3(unity_matrix, a, a_inverse);
#endif

  return S2G_SUCCESS;
}

/*******************************************************************************/
void convertfromcartesian(mmUnits units, double x, double y, 
  double originRadius,
  double *xTheta, double *yR)
/* convert cartesian x and y to polar theta and r if the units are polar,
   otherwise just pass the coordinates through transparently. note how the
   temp variable allows making the output parameters the same as the inputs
   (example is convertcartesiantopolar(x, y, &x, &y)) */
{
  double radiusTemp;
  if (units == mmCartesian)
  {
    *xTheta = x;
    *yR = y;
  }
  else
  {
    radiusTemp = sqrt(x * x + y * y);
    *xTheta = datan2safe (y, x);
    if (units == mmDegrees)
      *xTheta *= 180.0 / PI;
    else if (units == mmGradians)
      *xTheta *= 200.0 / PI;
    *yR = radiusTemp + originRadius;
   }
}

/*******************************************************************************/
void converttocartesian(mmUnits units, double xTheta, double yR, 
  double originRadius,
  double *x, double *y)
/* convert polar theta and r to cartesian x and y if the units are polar,
   otherwise just pass the coordinates through transparently. note how the
   temp variable allows making the output parameters the same as the inputs
   (example convertpolartocartesian(x, y, &x, &y)) */
{
  double angleTemp;
  if (units == mmCartesian)
  {
    *x = xTheta;
    *y = yR;
  }
  else
  {
    angleTemp = xTheta;
    if (units == mmDegrees)
      angleTemp *= PI / 180.0;
    else if (units == mmGradians)
      angleTemp *= PI / 200.0;
    *x = (yR - originRadius) * cos(angleTemp);
    *y = (yR - originRadius) * sin(angleTemp);
   }
}

/*******************************************************************************/
double dabs (double x)
/* returns absolute value of double precision value */
{
  if (x > 0)
    return (x);
  else
    return (-x);
}

/*******************************************************************************/
double dangle (double r1 [3], double r2 [3])
/*    returns the angle between vectors r1 and r2 (radians) */

{
  double r1mag, r2mag, rdot12, cosine, value;

  r1mag = dvmag1 (r1);
  r2mag = dvmag1 (r2);
  rdot12 = ddot (r1, r2);
  cosine = rdot12 / (r1mag * r2mag);
  if (dabs (cosine) < 1.0)
     value = acos (cosine);
  else
     if (cosine > 1.0)
       value = 0.0;
     else
       value = PI;

  return (value);
}

/*******************************************************************************/

double dangle_4_quadrant (double rfrom [3], double rabout [3], double rto [3])
/*    returns the angle (radians) required to rotate the from
      vector (rfrom), about the about vector (rabout), into the
      rabout-rto plane. a right-handed rotation is used, giving
      results between 0 and 2*pi
*/
{
      double pfrom [3], pto [3], value;
      double rtemp [3], rhalfpi [3];

/*    project the from and to vectors perpendicular to rabout */
      dcross (rabout, rfrom, rtemp);
      dcross (rtemp, rabout, pfrom);
      dcross (rabout, rto, rtemp);
      dcross (rtemp, rabout, pto);

/*    give answer between 0 and pi */
      value = dangle (pfrom, pto);

/*    go to quadrants 3 and 4 if rto is past pi */
      dcross (rabout, rfrom, rhalfpi);
      if (ddot (rhalfpi, rto) < 0.0)
     return (value);
    else
         return (2.0 * PI - value);
}

/*******************************************************************************/
double dasinsafe (double arg)
/*    bounds the input argument before returning dasinsafe */
{
  double value;

  if (arg < -1.0)
    value = -PI / 2.0;
    else
  {
    if (arg > 1.0)
      value = PI / 2.0;
    else
      value = asin (arg);
  }
  return (value);
}

/*******************************************************************************/
double datan2safe (double y, double x)
/*    checks for zero input arguments before returning atan */
{
  double value;

  if (x * x + y * y > 1.0e-8)
    value = atan2 (y, x);
    else
        value = 0.0;

  return (value);
}

/*******************************************************************************/
void dcross (double r1 [3], double r2 [3], double r3 [3])
/*    cross product of first vector with second, into third  */
{
      r3 [0] = r1 [1] * r2 [2] - r1 [2] * r2 [1];
      r3 [1] = r1 [2] * r2 [0] - r1 [0] * r2 [2];
      r3 [2] = r1 [0] * r2 [1] - r1 [1] * r2 [0];
}

/*******************************************************************************/
double ddot (double r1 [3], double r2 [3])
/*    dot product of 2 vectors */
{
      return (r1 [0] * r2 [0] + r1 [1] * r2 [1] + r1 [2] * r2 [2]);
}

/*******************************************************************************/
double degrees2radians (double argument)
/*    convert argument in degrees to radians */
{
      return (argument * atan((double) 1.0) / 45.0);
}

/*******************************************************************************/
bool dinverse (double* a, int n, int np, int* iworkn,
  double* y, double* workn, double* worknn)
//     finds the inverse of a square nxn matrix a, returning it
//     in the square nxn matrix y. algorithm is from "numerical recipes".
//     workn and worknn are the work areas. note that everything is
//     double precision (see cinverse for complex). kscr is the screen
//     output file number
//
//     uses routines dlubksb and dludcmp
{
  double column [3];

  for (int i = 0; i < n; i++)

    for (int j = 0; j < n; j++)
      worknn [FOLD2DINDEX(i, j, np)] = a [FOLD2DINDEX(i, j, np)];

  // decompose the matrix
  if (!dludcmp (worknn, n, np, iworkn, workn))
    return false;

  // find inverse by columns
  for (int j = 0; j < n; j++)
  {
    column [0] = 0;
    column [1] = 0;
    column [2] = 0;

    column [j] = 1;

    if (!dlubksb (worknn, n, np, iworkn, column))
      return false;

    y [FOLD2DINDEX(0, j, np)] = column[0];
    y [FOLD2DINDEX(1, j, np)] = column[1];
    y [FOLD2DINDEX(2, j, np)] = column[2];
  }

  return true;
}

/*******************************************************************************/
double distancetoline(double x, double y, double x1, double y1,
  double x2, double y2)
{
  // project (x,y) onto line between (x1,y1) and (x2,y2), then return
  // distance between (x,y) and the projected point (xP,yP)
  double xP, yP;
  projectpointontoline(x, y, x1, y1, x2, y2, &xP, &yP);
  return sqrt((xP - x) * (xP - x) + (yP - y) * (yP - y));
}
  
/*******************************************************************************/
void dlincom (double cin1, double rin1 [3], double cin2, double rin2 [3],
        double rout [3])
/*    takes a linear combination of 2 vectors to produce a third.
      idea borrowed surreptiously from j. hoffman and f. morinigo.
      input
         rin1,rin2    first and second input vectors
         cin1,cin2    coefficients of the input vectors
      output
         rout         = cin1 * rin1 + cin2 * rin2
*/
{
  int i;

  for (i = 0; i < 3; i++)
    rout [i] = cin1 * rin1 [i] + cin2 * rin2 [i];
}

/*******************************************************************************/
bool dlubksb (double* a, int n, int np, int* index, double* b)
//     solves a*x=b. returns true if successful
//     input
//        a           lu decomposition matrix from dludcmp. order is nxn
//        n           used size of vectors and matrix a
//        np          dimensioned size of vectors and matrix a
//        index       permutation vector from dludcmp
//        b           b vector in a*x=b
//     output
//        b           x vector in a*x=b
{
  double sum;
  int i, j, k, ipermuted;

  k = -1;
  for (i = 0; i < n; i++)
  {
    ipermuted = index [i];
    sum = b [ipermuted];
    b [ipermuted] = b [i];
    if (k != -1)
      for (j = k; j < i; j++)
        sum -= a [FOLD2DINDEX(i, j, np)] * b [j];
    else
      if (sum != 0.0)
        k = i;
    b [i] = sum;
  }

  for (i = n - 1; i >= 0; i--)
  {
    sum = b [i];
    for (j = i + 1; j < n; j++)
      sum -= a [FOLD2DINDEX(i, j, np)] * b [j];

    if (a [FOLD2DINDEX(i, i, np)] == 0.0)
      return false;
    b [i] = sum / a [FOLD2DINDEX(i, i, np)];
  }

  return true;
}

/*******************************************************************************/
bool dludcmp (double* a, int n, int np, int* index, double* workn)
//     returns lu decomposition of nxn matrix a, and true if successful
//     input
//        a           matrix to decompose
//        n           used size of vectors and matrix a
//        np          dimensioned size of vectors and matrix a
//     output
//        index       permutation vector
//     internal
//        workn       temporary storage matrix
{
  double tiny = 1.0e-12; // info at http://www.math.byu.edu/~schow/work/IEEEFloatingPoint.htm
  double alargest, term, sum;
  int i, j, k, ilargest;

  for (i = 0; i < n; i++)
  {
    alargest = 0.0;
    for (j = 0; j < n; j++)
      alargest = max (alargest, fabs (a [FOLD2DINDEX(i, j, np)]));
    if (alargest == 0.0)
      return false;
    workn [i] = 1.0 / alargest;
  }
  for (j = 0; j < n; j++)
  {
    for (i = 0; i < j; i++)
    {
      sum = a [FOLD2DINDEX(i, j, np)];
      for (k = 0; k < i; k++)
        sum -= a [FOLD2DINDEX(i, k, np)] * a [FOLD2DINDEX(k, j, np)];
      a [FOLD2DINDEX(i, j, np)] = sum;
    }
    alargest = 0.0;
    ilargest = j;
    for (i = j; i < n; i++)
    {
      sum = a [FOLD2DINDEX(i, j, np)];
      for (k = 0; k < j; k++)
        sum -= a [FOLD2DINDEX(i, k, np)] * a [FOLD2DINDEX(k, j, np)];
      a [FOLD2DINDEX(i, j, np)] = sum;
      term = workn [i] * fabs (sum);
      if (term >= alargest)
      {
        ilargest = i;
        alargest = term;
      }
    }
    if (j != ilargest)
    {
      for (k = 0; k < n; k++)
      {
        term = a [FOLD2DINDEX(ilargest, k, np)];
        a [FOLD2DINDEX(ilargest, k, np)] = a [FOLD2DINDEX(j, k, np)];
        a [FOLD2DINDEX(j, k, np)] = term;
      }
      workn [ilargest] = workn [j];
    }
    index [j] = ilargest;
    if (a [FOLD2DINDEX(j, j, np)] == 0.0)
      a [FOLD2DINDEX(j, j, np)] = tiny;
    if (j != n - 1)
    {
      term = 1.0 / a [FOLD2DINDEX(j, j, np)];
      for (i = j + 1; i < n; i++)
        a [FOLD2DINDEX(i, j, np)] *= term;
    }
  }

  return true;
}

/*******************************************************************************/
void dmlt3x1 (double z [3], const double x [3] [3], const double y [3])
/*    multiply 3x3 matrix, x, by vector y, into vector z */

{
  double sum;
  int nrow, nentry;

  for (nrow = 0; nrow < 3; nrow++)
  {
    sum = 0.0;
        for (nentry = 0; nentry < 3; nentry++)
      sum = sum + x [nrow] [nentry] * y [nentry];
    z [nrow] = sum;
  }
}

/*******************************************************************************/
void dmlt3x3 (double z [3] [3], const double x [3] [3], const double y [3] [3])
/*    multiply 2 3x3 matrices, x and y, into z */

{
  int nrow, ncol, nentry;
  double sum;

    for (nrow = 0; nrow < 3; nrow++)
    for (ncol = 0; ncol < 3; ncol++)
    {
      sum = 0.0;
            for (nentry = 0; nentry < 3; nentry++)
               sum = sum + x [nrow] [nentry] * y [nentry] [ncol];
            z [nrow] [ncol] = sum;
    }
}

/*******************************************************************************/
void double_interpolate (int lengthRaw, double *xRaw, double *yRaw,
             int lengthInt, double *xInt, double *yInt)
/* linearly interpolate raw curve into interpolated curve. only yInt is output.
   this function implicitly assumes that the xRaw array is monotonically
   increasing */
{
  int iRaw = 1;
  for (int iInt = 0; iInt < lengthInt; iInt++)
  {
    if (lengthRaw < 1)
    {
      // not enough raw points to interpolate, so fill with zeros
      yInt [iInt] = 0.0;
    }
    else if (lengthRaw == 1)
    {
      // single raw point applies everywhere
      yInt [iInt] = yRaw [0];
    }
    else
    {
      // bracket iInt'th point by iRaw-1 and iRaw
      while ((xInt [iInt] > xRaw [iRaw]) && (iRaw < lengthRaw - 1))
        ++iRaw;
      ASSERT_ENGAUGE(xRaw [iRaw - 1] <= xRaw [iRaw]); // monotonically increasing?
      double denominator = xRaw [iRaw] - xRaw [iRaw - 1];
      if (denominator == 0.0)
      {
        /* dilemma - both raw x values are the same but raw y values may
           not be the same. use the first y value and keep it simple so
           user will be able to figure out what happened */
        yInt [iInt] = yRaw [iRaw - 1];
      }
      else
      {

        /* normal interpolation */
        yInt [iInt] = yRaw [iRaw - 1] + (xInt [iInt] - xRaw [iRaw - 1]) *
        (yRaw [iRaw] - yRaw [iRaw - 1]) / denominator;
      }
    }
  }
}

/*******************************************************************************/
double drandf ()
/*    interface with the system dependent random number generator */
{
  return ((double) rand ()) / ((double) RAND_MAX);
}

/*******************************************************************************/
void drandgt (double* randg1, double* randg2)
/*    gaussian numbers with tails included.  drawbacks are slower
      speed and that two, versus one, gaussian numbers are returned.
      value of pi is not critical (independent of calling program)
*/
{
  double r, u1, u2, theta;


  do

  {
    u1 = drandf ();
  } while (u1 == 0.0);
  u2 = drandf ();
  r = sqrt (- 2.0 * log (u1));
  theta = 2.0 * PI * u2;
  *randg1 = r * cos (theta);
  *randg2 = r * sin (theta);
}

/*******************************************************************************/
void dtranspose (double rin [3] [3], double rout [3] [3])
/*    perform transform of 3x3 matrix. input matrix can be same as output */
{
  double temp;

    rout [0] [0] = rin [0] [0];
    temp = rin [1] [0];
    rout [1] [0] = rin [0] [1];
    rout [0] [1] = temp;
    temp = rin [2] [0];
    rout [2] [0] = rin [0] [2];
    rout [0] [2] = temp;
    rout [1] [1] = rin [1] [1];
    temp = rin [2] [1];
    rout [2] [1] = rin [1] [2];
    rout [1] [2] = temp;
    rout [2] [2] = rin [2] [2];
}

/*******************************************************************************/
void dtrrotate (double angle, double vecabout [3], double transform [3] [3])
/*    returns a 3x3 transform matrix which rotates about an
      arbitrary vector

      derivation starts with
         vecend=v*cos(angle)*sin(phi)*Q+
                v*sin(angle)*sin(phi)*R+
                v*cos(phi)*P
      where
         P=vecabout
         Q=vecstartxP/(v*sin(phi))
         R=(vecstartxP)xvecstart/(v*sin(phi))
      and the terms are separated out to give
         vecend=transform*vecstart

      input

         angle         angle of rotaton (rads)
         vecabout      axis of rotation
      output
         transform     3x3 rotation transformation matrix
*/
{
  double c, s, px, py, pz, rabout [3];

    c = cos (angle);
    s = sin (angle);
    dunitize (vecabout, rabout);
    px = rabout [0];
    py = rabout [1];
    pz = rabout [2];
    transform [0] [0] = c * (pz * pz + py * py) + px * px;
    transform [1] [0] = -c * py * px + s * pz + px * py;
    transform [2] [0] = -c * pz * px - s * py + px * pz;
    transform [0] [1] = -c * px * py - s * pz + py * px;
    transform [1] [1] = c * (px * px + pz * pz) + py * py;
    transform [2] [1] = -c * pz * py + s * px + py * pz;
    transform [0] [2] = -c * px * pz + s * py + pz * px;
    transform [1] [2] = -c * py * pz - s * px + pz * py;
    transform [2] [2] = c * (py * py + px * px) + pz * pz;
}

/*******************************************************************************/
void dunitize (double r [3], double ur [3])
/*    unitizes a vector into another. input and output vectors can
      be the same vector */
{
  double rmag;
  int i;

    rmag = dvmag1 (r);
    for (i = 0; i < 3; i++)
    ur [i] = r [i] / rmag;
}

/*******************************************************************************/
double dvmag1 (double r [3])
/*    vector magnitude of 1 vector with 3 components */
{
      return (sqrt (r [0] * r [0] + r [1] * r [1] + r [2] * r [2]));
}

/*******************************************************************************/
double dvmag3 (double x, double y, double z)
/*    vector magnitude of 3 components of a vector */
{
      return (sqrt (x * x + y * y + z * z));
}

/*******************************************************************************/
double functionarea(int nPoints, double* x, double* y)
/* return the area under the point-wise defined function using unoptimized
   trapezoid rule integration */
{
  double sum = 0.0;
  for (int i = 0; i < nPoints - 1; i++)
    sum += (x [i + 1] - x [i]) * (y [i + 1] + y [i]) / 2.0;

  return sum;
}

/*******************************************************************************/
bool intersecttwolines (double xLine1a, double yLine1a, double xLine1b, double yLine1b,
    double xLine2a, double yLine2a, double xLine2b, double yLine2b, double* sLine1Int,
  double* sLine2Int)
/*    intersect two line segments defined using the two-point equation.
      return value is true if an intersection occurred. The parameter sLine1Int
    says where the intersection occurred along the first line, where sLine1Int=0
    is at point a and sLine1Int=1 is at point b. The parameter sLine2Int
    works the same but for the second line */

{
  /* parameterize the two-point lines as x=(1-s)*xa+s*xb, y=(1-s)*ya+s*yb and
     then intersect to get s = numerator / denominator */
  double denominator, numeratorLine1, numeratorLine2;

  denominator =
    (yLine2b - yLine2a) * (xLine1b - xLine1a) -
    (yLine1b - yLine1a) * (xLine2b - xLine2a);

  if (dabs(denominator) < 0.000001)
    return false; // either zero or an infinite number of points intersect

  numeratorLine1 =
    (yLine1a - yLine2a) * (xLine2b - xLine2a) -
    (yLine2b - yLine2a) * (xLine1a - xLine2a);
  numeratorLine2 =
    (yLine1b - yLine1a) * (xLine2a - xLine1a) -
    (yLine2a - yLine1a) * (xLine1b - xLine1a);

  *sLine1Int = numeratorLine1 / denominator;
  *sLine2Int = numeratorLine2 / denominator;

  return true;
}

/*******************************************************************************/
int lstchm1 (char cstring [])
/*    returns the position of the last nonblank charater in a string.

      special case - if string is blank then -1 is returned */
{
  int ich, numch;

    numch = strlen (cstring);
    for (ich = numch - 1; numch > 0; numch--)
    if (cstring [ich] != ' ')
      return (ich);
  return (-1);
}

/*******************************************************************************/
bool multidimensionalminimization(double** xVectors, double* yVector,
  int ndim, double tolerance, double minFunction(double*), int iterationmax,
  int* iteration)
/* minimize the multivariable function minFunction. it is defined as
     double minFunction(double* minVector)

   where minVector has ndim elements. on input xVectors contains ndim+1
   unique vectors, and yVector contains the corresponding minFunction
   values. on output, xVectors contains the minimized ndim+1 vectors, and
   yVector contains the corresponding minFunction values


   following code could be used to initialize before calling this function
    // allocate xVectors [0:ndim+1] [0:ndim]

    xVectors = (double**) malloc ((ndim + 1) * sizeof (double));
    for (i = 0; i <= ndim; i++)
      xVectors [i] = (double*) malloc (ndim * sizeof (double));
    for (j = 0; j <= ndim; j++)
    {
      for (i = 0; i < ndim; i++)
        xVectors [j] [i] = 0.0;
      if (j < ndim)

      xVectors [j] [j] = 1.0;
    }
    yVector = (double*) malloc ((ndim + 1) * sizeof (double));
    for (j = 0; j <= ndim; j++)
      yVector [j] = myFunction (xVectors [j]);

   algorithm iterates until the function range (y(highest)-y(lowest)) is
   less than the specified tolerance, or the maximum iteration limit has
   been reached. algorithm success is returned as true or false
*/
{
#define ALPHA 1.0
#define BETA 0.5

#define false 0
#define GAMMA 2.0
#define NMAX 20
#define true 1

  double pr [NMAX], prr [NMAX], pbar [NMAX];
  double ypr, yprr, rtolerance;
  int ilow, ihighest, nexthighest, i, j;
  int ndimplus1 = ndim + 1;

  *iteration = 0;
  while (true) /* iterate until convergence criteria are met */
  {
    ilow = 0;
    if (yVector [0] > yVector [1])
    {
      ihighest = 0;
      nexthighest = 1;
    }
    else
    {
      ihighest = 1;
      nexthighest = 0;
    }
    for (i = 0; i < ndimplus1; i++)
    {
      if (yVector [i] < yVector [ilow])
        ilow = i;
      if (yVector [i] > yVector [ihighest])

      {
        nexthighest = ihighest;
        ihighest = i;
      }
      else if (yVector [i] > yVector [nexthighest])
        if (i != ihighest) nexthighest = i;
    }
    rtolerance = fabs (yVector [ihighest] - yVector [ilow]);
    if (rtolerance < tolerance)
      return true;

    if ((*iteration)++ >= iterationmax)
      return false;
    for (j = 0; j < ndim; j++)
      pbar [j] = 0.0;
    for (i = 0; i < ndimplus1; i++)
      if (i != ihighest)
        for (j = 0; j < ndim; j++)
          pbar [j] += xVectors [i] [j];
    for (j = 0; j < ndim; j++)

    {
      pbar [j] /= ndim;
      pr [j] = (1.0 + ALPHA) * pbar [j] - ALPHA * xVectors [ihighest] [j];
    }
    ypr = minFunction (pr);
    if (ypr <= yVector [ilow])
    {
      for (j = 0; j < ndim; j++)
        prr [j] = GAMMA * pr [j] + (1.0 - GAMMA) * pbar [j];
      yprr = minFunction (prr);
      if (yprr < yVector [ilow])
      {
        for (j = 0; j < ndim; j++)
          xVectors [ihighest] [j] = prr [j];
          yVector [ihighest] = yprr;
      }
      else
      {
        for (j = 0; j < ndim; j++)
          xVectors [ihighest] [j] = pr [j];
        yVector [ihighest] = ypr;
      }
    }
    else if (ypr >= yVector [nexthighest])
    {
      if (ypr < yVector [ihighest])
      {
        for (j = 0; j < ndim; j++)
          xVectors [ihighest] [j] = pr [j];
        yVector [ihighest] = ypr;
      }
      for (j = 0; j < ndim; j++)
        prr [j] = BETA * xVectors [ihighest] [j] + (1.0 - BETA) * pbar [j];
      yprr = minFunction (prr);
      if (yprr < yVector [ihighest])
      {
        for (j = 0; j < ndim; j++)
          xVectors [ihighest] [j] = prr [j];
        yVector [ihighest] = yprr;
      }
      else
      {
        for (i = 0; i < ndimplus1; i++)
          if (i != ilow)
          {
            for (j = 0; j < ndim; j++)
            {
              pr [j] = (xVectors [i] [j] + xVectors [ilow] [j]) / 2.0;
              xVectors [i] [j] = pr [j];
            }
            yVector [i] = minFunction (pr);
          }
      }
    }
    else

    {
      for (j = 0; j < ndim; j++)
        xVectors [ihighest] [j] = pr [j];
      yVector [ihighest] = ypr;
    }
  }
  return false;
}

/*******************************************************************************/
QRgb pixelRGB(const QImage* image, int x, int y)
{
/* grab rgb pixel from 1 bit image */
  ASSERT_ENGAUGE(image != 0);
  switch (image->depth())
  {
    case 1:
      return pixelRGB1(image, x, y);
    case 8:
      return pixelRGB8(image, x, y);
    default:
      return pixelRGB32(image, x, y);
  }
}

/*******************************************************************************/
QRgb pixelRGB1(const QImage* image1Bit, int x, int y)
{
/* grab pixel from 1 bit image at specified position. each pixel is either
   black or white

   see detailed description of QImage class to understand this */
  ASSERT_ENGAUGE(image1Bit != 0);
  unsigned int bit;
  if (image1Bit->bitOrder () == QImage::LittleEndian)
    bit = *(image1Bit->scanLine (y) + (x >> 3)) & (1 << (x & 7));
  else
    bit = *(image1Bit->scanLine (y) + (x >> 3)) & (1 << (7 - (x & 7)));

  unsigned int tableIndex = ((bit == 0) ? 0 : 1);
  return image1Bit->color(tableIndex);
}

/*******************************************************************************/
QRgb pixelRGB8(const QImage* image8Bit, int x, int y)
{
/* grab rgb pixel from 8 bit image at specified position

  see detailed description of QImage class to understand this */
  ASSERT_ENGAUGE(image8Bit != 0);
  unsigned int tableIndex = *(image8Bit->scanLine(y) + x);
  return image8Bit->color(tableIndex);
}

/*******************************************************************************/
QRgb pixelRGB32(const QImage* image32Bit, int x, int y)
{
/* grab rgb pixel from 32 bit image at specified position

  see detailed description of QImage class to understand this */
  ASSERT_ENGAUGE(image32Bit != 0);
  unsigned int* p = (unsigned int *)image32Bit->scanLine(y) + x;
  return *p;
}

/*******************************************************************************/
bool pointinsidebox(double xI, double yI, double x0, double y0,
  double x1, double y1, double x2, double y2, double x3, double y3)
/*  return true if and only if the point I is inside the four-cornered box. the
    box does not have to be rectangular. ASSUMPTION - quadrilateral is convex */
{
  /* point is not surrounded if there is a 180 degree region around it that
     has no corner points in it */
  double angle0, angle1, angle2, angle3;
  double angleMin, angleMax;

  angle0 = datan2safe(y0 - yI, x0 - xI);
  angle1 = datan2safe(y1 - yI, x1 - xI);
  angle2 = datan2safe(y2 - yI, x2 - xI);
  angle3 = datan2safe(y3 - yI, x3 - xI);


  angleMin = angle0;
  angleMax = angle0;

  if (angle1 < angleMin)
  {
    if (angle1 < angleMin - PI)
      angleMax = max(angleMax, angle1 + 2.0 * PI);
    else
      angleMin = angle1;
  }
  else
    angleMax = max(angleMax, angle1);
  if (angle2 < angleMin)
  {
    if (angle2 < angleMin - PI)
      angleMax = max(angleMax, angle2 + 2.0 * PI);
    else
      angleMin = angle2;
  }
  else
    angleMax = max(angleMax, angle2);
  if (angle3 < angleMin)
  {
    if (angle3 < angleMin - PI)
      angleMax = max(angleMax, angle3 + 2.0 * PI);
    else
      angleMin = angle3;
  }
  else
    angleMax = max(angleMax, angle3);


  return (dabs(angleMax - angleMin) > PI);
}

/*******************************************************************************/
double polygonarea(int nPoints, double* x, double* y)
/* wrapper function around polygonarearecurse that prevents duplicate calculations by
   using previous results if they still apply. this method is thread safe although it
   has static variables */
{ 
  // inputs and output from previous invocation
  static int nPointsLast = 0;
  static double* xLast = 0;
  static double* yLast = 0;
  static double areaLast = 0.0;
  
  QMutex mutex;
  mutex.lock();

  // have the inputs changed?
  bool changed = (nPoints != nPointsLast);
  if (!changed)
  {
    for (int i = 0; i < nPoints; i++)
      if ((x [i] != xLast [i]) ||
        (y [i] != yLast [i]))
      {
        changed = true;
        break;
      }
  }

  if (changed)
  {
    // remove previous allocation
    if (xLast != 0)
    {
      delete xLast;
      xLast = 0;
    }
    if (yLast != 0)
    {
      delete yLast;
      yLast = 0;
    }

    // save new inputs, reallocating memory
    nPointsLast = nPoints;
    areaLast = 0.0;
    if (nPointsLast > 0)
    {
      xLast = new double [nPointsLast];
      yLast = new double [nPointsLast];
      for (int i = 0; i < nPointsLast; i++)
      {
        xLast [i] = x [i];
        yLast [i] = y [i];
      }

      // compute new area
      areaLast = polygonarearecurse (nPointsLast, xLast, yLast, 0);
    }
  }

  mutex.unlock();

  return areaLast;
}

/*******************************************************************************/
double polygonarearecurse(int nPoints, double* x, double* y, int level)
/* return the area under the possibly multiply-connected polygon. if unable to allocate
   memory then return negative area so user will know value is incorrect */
{
  /* break up polygon into simply connected smaller parts, if the polygon is
     not already simply connected. note - this function closes the polygon
     internally - so the last point should NOT be the same as the first point.
     example, nPoints = 3/4 for a triangle/square

     this function no longer uses memcpy since negative lengths cause crashes in osx

     this function is slow, and since it is called for window focus event that can cause
     sluggish responses. in fact, this effect is probably the cause of crashes in osx.
     therefore, the polygonareaoptimizer wrapper function should be used */
  const double NEGATIVE_AREA = -1.0;

//  QString inputs = QString("level: %1, points: %2\n").arg(level).arg(nPoints);
//  for (int iii = 0; iii < nPoints; iii++)
//  {
//    inputs += QString("[%1,%2]").arg(x[iii]).arg(y[iii]);
//    if ((iii % 10 == 9) && (iii != nPoints - 1))
//      inputs += QString("\n");
//  }
//  QMessageBox::critical(0, QString("polygonarea"), inputs);
//  fprintf (stderr,  "%s\n\n", inputs.latin1());

  if (nPoints < 3)
    return 0.0;

  for (int linea = 0; linea < nPoints - 1; linea++)
    for (int lineb = linea + 1; lineb < nPoints; lineb++)
    {
      int linebp1 = lineb + 1;
      if (linebp1 >= nPoints)
        linebp1 -= nPoints;

      double sLineaInt, sLinebInt;
      if (intersecttwolines (x [linea], y [linea], x [linea + 1],
        y [linea + 1], x [lineb], y [lineb], x [linebp1],
        y [linebp1], &sLineaInt, &sLinebInt) && (0 < sLineaInt) &&
        (sLineaInt < 1) && (0 < sLinebInt) && (sLinebInt < 1))
      {
        /* lines between points linea and linea+1, and lineb and
           lineb+1, cross each other. break up each line to create
           two new polygons */

        /* a pathological case occurs when the number of points in either of
           the two subareas equals the nPoints (nPointsArea1 or nPointsArea2 is zero).
           in this case, this function will recurse forever because the smaller subareas
           are not actually smaller. this infinite recursion indeed happens on the mac powerpc
           for the corners.png sample file. applying some algebra to the constraints that
           neither nPointsArea1 nor nPointsArea2 can equal nPoints gives the interesting
           result that the difference lineb-linea can never be 1 nor nPoints-1. after some
           reflection, this makes sense for two reasons:
           1) if nPointsArea1 equals nPoints then nPointsArea2 equals two, or
              if nPointsArea2 equals nPoints then nPointsArea1 equals two, and we
              can only subdivide the areas into triangles
           2) this case corresponds to a vertex, and not the intersection of two closed polygons */
        int nPointsArea1 = (linea + 1) + (1) + (nPoints - lineb - 1);
        int nPointsArea2 = (lineb - linea) + (1);

        if ((nPoints != nPointsArea1) &&
          (nPoints != nPointsArea2))
        {
          /* recurse*/
          int iFrom, iTo;
                
          /* all combinations of memory allocation failures are handled */
          double* xArea1 = 0;
          double* yArea1 = 0;
          double* xArea2 = 0;
          double* yArea2 = 0;
        
          /* recurse for first area */
          double area1 = NEGATIVE_AREA;
          xArea1 = new double [nPointsArea1];
          yArea1 = new double [nPointsArea1];
          if ((xArea1 != 0) && (yArea1 != 0))
          {
            for (iFrom = 0, iTo = 0; iTo < linea + 1; iFrom++, iTo++)
            {
              xArea1 [iTo] = x [iFrom];
              yArea1 [iTo] = y [iFrom];
          }
            xArea1 [linea + 1] = (1.0 - sLineaInt) * x [linea] + sLineaInt * x [linea + 1];
            yArea1 [linea + 1] = (1.0 - sLineaInt) * y [linea] + sLineaInt * y [linea + 1];
            for (iFrom = lineb + 1, iTo = linea + 2; iTo < nPointsArea1; iFrom++, iTo++)
            {
              xArea1 [iTo] = x [iFrom];
              yArea1 [iTo] = y [iFrom];
            }
            area1 = polygonarearecurse (nPointsArea1, xArea1, yArea1, level + 1);
          }
        
          /* recurse for second area */
          double area2 = NEGATIVE_AREA;
          xArea2 = new double [nPointsArea2];
          yArea2 = new double [nPointsArea2];
          if ((xArea1 != 0) && (yArea1 != 0) &&
            (xArea2 != 0) && (yArea2 != 0))
          {
            for (iFrom = linea + 1, iTo = 0; iTo < lineb - linea; iFrom++, iTo++)
            {
              xArea2 [iTo] = x [iFrom];
              yArea2 [iTo] = y [iFrom];
            }
            xArea2 [lineb - linea] = xArea1 [linea + 1];
            yArea2 [lineb - linea] = yArea1 [linea + 1];
            area2 = polygonarearecurse (nPointsArea2, xArea2, yArea2, level + 1);
          }

          /* allocated arrays can be safely released now */
          if (xArea1)
            delete[] xArea1;
          if (yArea1)
            delete[] yArea1;        
          if (xArea2)
            delete[] xArea2;
          if (yArea2)
            delete[] yArea2;

          if ((area1 < 0.0) || (area2 < 0.0))
            return NEGATIVE_AREA;

          return area1 + area2;
        }
      }
    }

  // area of a general simply-connected polygon
  double columnleft = 0.0, columnright = 0.0;
  int i, ip1;
  for (i = 0; i < nPoints; i++)
  {
    ip1 = (i + 1) % nPoints;

    // cast the integers to double so integer overflow does not give negative column values
    columnleft += ((double) x [ip1]) * ((double) y [i]);
    columnright += ((double) x [i]) * ((double) y [ip1]);
  }

  return dabs(columnleft - columnright) / 2.0;
}

/*******************************************************************************/
void projectpointontoline(double xCenter, double yCenter, double xStart,
  double yStart, double xStop, double yStop, double* xProjection,
  double* yProjection)
/* find closest point to (x, y) on the line segment from (xStart, yStart) to
   (xStop, yStop). closest point is kept between the start and stop point */
{
  if (dabs (yStart - yStop) < 0.000001)
  {
    /* special case - line segment is vertical */
    *yProjection = yStart;
    double s = (xCenter - xStart) / (xStop - xStart);
    if (s < 0)
      *xProjection = xStart;
    else if (s > 1)
      *xProjection = xStop;
    else
      *xProjection = (1.0 - s) * xStart + s * xStop;
  }
  else
  {
    /* general case - compute slope and intercept of line through
       (xCenter, yCenter) */
    double slope = (xStop - xStart) / (yStart - yStop);
    double yintercept = yCenter - slope * xCenter;

    /* intersect center point line (slope-intercept form) with start-stop 
       line (parametric form x=(1-s)*x1+s*x2, y=(1-s)*y1+s*y2) */
    double s = (slope * xStart + yintercept - yStart) /
      (yStop - yStart + slope * (xStart - xStop));

    if (s < 0)
    {
      *xProjection = xStart;
      *yProjection = yStart;
    }
    else if (s > 1)
    {
      *xProjection = xStop;
      *yProjection = yStop;
    }
    else
    {
      *xProjection = (1.0 - s) * xStart + s * xStop;
      *yProjection = (1.0 - s) * yStart + s * yStop;
    }
  }
}

/*******************************************************************************/
double radians2degrees (double argument)
/*    convert argument in radians to degrees */
{
    return (argument * 45.0 / atan((double) 1.0));
}

/*******************************************************************************/
double roundofftopower(double arg, int roundoffPower)
{
  double powerOf10 = pow ((double) 10, roundoffPower);
  return powerOf10 * floor (arg / powerOf10 + 0.5);
}

/*******************************************************************************/
void setPixelRGB(QImage* image, int x, int y, QRgb q)
{
/* set rgb pixel in 1 bit image */
  ASSERT_ENGAUGE(image != 0);
  switch (image->depth())
  {
    case 1:
      setPixelRGB1(image, x, y, q);
      return;
    case 8:
      setPixelRGB8(image, x, y, q);
      return;
    case 32:
      setPixelRGB32(image, x, y, q);
      return;
  }
}

/*******************************************************************************/
void setPixelRGB1(QImage* image1Bit, int x, int y, QRgb q)
{
/* set rgb pixel in 1 bit image at specified position

  see detailed description of QImage class to understand this */
  ASSERT_ENGAUGE(image1Bit != 0);
  for (int index = 0; index < image1Bit->numColors(); index++)
    if (q == image1Bit->color(index))
    {
      if (image1Bit->bitOrder () == QImage::LittleEndian)
      {
        *(image1Bit->scanLine (y) + (x >> 3)) &= ~(1 << (x & 7));
        if (index > 0)
          *(image1Bit->scanLine (y) + (x >> 3)) |= index << (x & 7);
      }
      else
      {
        *(image1Bit->scanLine (y) + (x >> 3)) &= ~(1 << (7 - (x & 7)));
        if (index > 0)
          *(image1Bit->scanLine (y) + (x >> 3)) |= index << (7 - (x & 7));
      }
      return;
    }
}

/*******************************************************************************/
void setPixelRGB8(QImage* image8Bit, int x, int y, QRgb q)
{
/* set rgb pixel in 8 bit image at specified position

  see detailed description of QImage class to understand this */
  ASSERT_ENGAUGE(image8Bit != 0);
  for (int index = 0; index < image8Bit->numColors(); index++)
    if (q == image8Bit->color(index))
    {
      *(image8Bit->scanLine(y) + x) = index;
      return;
    }
}

/*******************************************************************************/
void setPixelRGB32(QImage* image32Bit, int x, int y, QRgb q)
{
/* set rgb pixel in 32 bit image at specified position

  see detailed description of QImage class to understand this */
  ASSERT_ENGAUGE(image32Bit != 0);
  int* p = (int *)image32Bit->scanLine(y) + x;
  *p = q;
}

/*******************************************************************************/
int valuePower(double value)
{
  // compute power of 10 for input value, rounding down to nearest
  // integer solution of value>=10**solution
  const int minPower = -30; // MAX_DOUBLE is 10^38

  double avalue = dabs(value);
  if (avalue < pow(10.0, minPower))
    return minPower;
  else
    return (int) floor (log10 (avalue));
}
