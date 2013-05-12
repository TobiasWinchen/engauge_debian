/***************************************************************************
                          mmsubs.h  -  description
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

#ifndef MMSUBS_H
#define MMSUBS_H

#include <qobject.h>
#include <qimage.h>
#include <qcolor.h>

// mutually exclusive descriptions of ordinate, and also of abscissa
enum mmUnits
{
  mmCartesian,
  mmDegrees,
  mmGradians,
  mmRadians
};

#define RGB_BLACK Qt::black.rgb()
#define RGB_WHITE Qt::white.rgb()
const int S2G_SUCCESS = 0;
const int S2G_BAD_GRAPH_COORDINATES = 1;
const int S2G_BAD_SCREEN_COORDINATES = 2;

extern int adjust_count (bool linearAxis, double start,
  double stop, double inc);
extern double adjust_inc (bool linearAxis, int roundoffPower, double start,
  double stop, int count);
extern double adjust_start (bool linearAxis, int roundoffPower, double stop,
  double inc, int count);
extern double adjust_stop (bool linearAxis, int roundoffPower, double start,
  double inc, int count);
extern double angleleft (double x0, double y0, double x1, double y1, double x2, double y2);
extern double angleright (double x0, double y0, double x1, double y1, double x2, double y2);
extern void axisScale(double x_min_in, double x_max_in, bool linearAxis,
  double* x_start, double* x_stop, double* x_delta, int* x_count);
extern bool clippedsegmentisvisible(double xTopLeft, double yTopLeft, double xTopRight,
  double yTopRight, double xBottomRight, double yBottomRight, double xBottomLeft,
  double yBottomLeft, double* xStart, double* yStart, double* xStop, double* yStop);
extern bool compute_linear_to_log (double* v, double vMin, double vMax);
extern bool compute_log_to_linear (double* v, double vMin, double vMax);
extern int compute_screen_to_graph (double [3] [3], double [3] [3], double [3] [3],
  double [3] [3]);
extern void convertfromcartesian(mmUnits units, double x, double y,
  double originRadius,
  double *xTheta, double *yR);
extern void converttocartesian(mmUnits units, double xTheta, double yR,
  double originRadius,
  double *x, double *y);
extern double dabs (double);
extern double dangle (double [3], double [3]);
extern double dangle_4_quadrant (double [3], double [3], double [3]);
extern double dasinsafe (double);
extern double datan2safe (double, double);
extern void dcross (double [3], double [3], double [3]);
extern double ddot (double [3], double [3]);
extern double degrees2radians (double argument);
extern bool dinverse (double* a, int n, int np, int* iworkn, double* y,
  double* workn, double* worknn);
extern double distancetoline(double x, double y, double x1, double y1,
  double x2, double y2);
extern void dlincom (double, double [3], double, double [3], double [3]);
extern void dloadi (double [3] [3]);
extern bool dlubksb (double* a, int n, int np, int* index, double* b);
extern bool dludcmp (double* a, int n, int np, int* index, double* workn);
extern void dmlt3x1 (double [3], const double [3] [3], const double [3]);
extern void dmlt3x3 (double [3] [3], const double [3] [3], const double [3] [3]);
void double_interpolate (int lengthRaw, double *xRaw, double *yRaw,
  int lengthInt, double *xInt, double *yInt);
extern double drandf ();
extern void drandgt (double *, double *);
extern void dtranspose (double [3] [3], double [3] [3]);
extern void dtrrotate (double, double [3], double [3] [3]);
extern void dunitize (double [3], double [3]);
extern double dvmag1 (double [3]);
extern double dvmag3 (double, double, double);
extern double functionarea(int npoints, double* x, double* y);
extern bool intersecttwolines (double xLine1a, double yLine1a, double xLine1b,
  double yLine1b, double xLine2a, double yLine2a, double xLine2b, double yLine2b,
  double* sLine1Int, double* sLine2Int);
extern int lstchm1 (char *);
extern bool multidimensionalminimization(double** xVectors, double* yVector,
  int ndim, double tolerance, double minFunction(double *),
  int iterationmax, int* iteration);
extern QRgb pixelRGB(const QImage* image, int x, int y);
extern QRgb pixelRGB1(const QImage* image1Bit, int x, int y);
extern QRgb pixelRGB8(const QImage* image8Bit, int x, int y);
extern QRgb pixelRGB32(const QImage* image32Bit, int x, int y);
extern bool pointinsidebox(double xI, double yI, double x0, double y0,
  double x1, double y1, double x2, double y2, double x3, double y3);
extern double polygonarearecurse(int npoints, double* x, double* y, int level);
extern double polygonarea(int npoints, double* x, double* y);
extern void projectpointontoline(double xCenter, double yCenter, double xStart,
  double yStart, double xStop, double yStop, double* xProjection,
  double* yProjection);
extern double radians2degrees (double argument);
extern double roundofftopower(double, int);
extern void setPixelRGB(QImage* image, int x, int y, QRgb q);
extern void setPixelRGB1(QImage* image1Bit, int x, int y, QRgb q);
extern void setPixelRGB8(QImage* image8Bit, int x, int y, QRgb q);
extern void setPixelRGB32(QImage* image32Bit, int x, int y, QRgb q);
extern int valuePower(double value);

/* functions that can handle variably dimensioned arrays use the FOLD2DINDEX
   macro to manually fold the array indices */
#define FOLD2DINDEX(i,j,jmax) ((i)*(jmax)+j)

#endif // MMSUBS_H

