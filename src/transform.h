/***************************************************************************
                          transform.h  -  description
                             -------------------
    begin                : Tue Oct 29 14:43:44 PST 2002
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

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <qobject.h>

#include "mmsubs.h"
#include "digitdef.h"

class PointSet;
class DigitDoc;

// class that maintains the screen-to-world transformation. it is derived from QObject solely
// to get access to the tr function
class Transform : public QObject
{
  Q_OBJECT
  
  public:
      
    Transform(DigitDoc* doc);
    ~Transform();

    // number of axes and scale bar points defined
    int axisPointCount(void) { return m_axisPointCount; }
    int scalePointCount(void) { return m_scalePointCount; }

    // try to compute the screen to/from graph coordinate transformation
    void computeAxesTransformation(CoordSettings coord, const PointSet* pointSet);
    void computeScaleTransformation(CoordSettings coord, const PointSet* pointSet);

    // adjust mid-valued log coordinates so we can handle axes points with any log coordinates
    void adjust_midvalued_log_coord(Scale scale, double* c0, double* c1, double* c2);
    void adjust_midvalued_log_coords(CoordSettings coord, double r_graph [3] [3]);

    // true when the axes or scale bar transformations have been correctly and completely defined
    bool validAxes(void) const { return m_validAxes; }
    bool validScale(void) const { return m_validScale; }

    // full matrix transformations containing screen/graph, cartesian/polar, log/linear
    // transformations
    void xThetaYRToScreen(CoordSettings coord, double xTheta, double yR,
      int* xScreen, int* yScreen) const;
    void xThetaYRToScreen(CoordSettings coord, double xTheta, double yR,
      double* xScreen, double* yScreen) const;
    void screenToXThetaYR(CoordSettings coord, int xScreen, int yScreen,
      double *xTheta, double *yR) const;

    // basis vectors in x and y directions
    void xBasisScreen(CoordSettings coord, double *xScreen, double *yScreen) const;
    void yBasisScreen(CoordSettings coord, double *xScreen, double *yScreen) const;
    
    // debug this transformation
    void dump(void);
        
  private:

    DigitDoc* doc;

    enum LogLinReturn
    {
      L2L_SUCCESS,
      L2L_NONPOSITIVE_COORDINATE,
      L2L_NO_SPREAD
    };
    
    // compute a and b that map log to/from linear
    Transform::LogLinReturn computeLogToFromLinear(CoordSettings coord, double r_graph [3] [3]);
    Transform::LogLinReturn computeLogToFromLinear(Scale scale, double logOffset, double r [3], double* a, double* b);

    // matrix transformations without cartesian/polar and linear/log transformations
    void graphToScreen(double xGraph, double yGraph, double* xScreen, double* yScreen) const;
    void screenToGraph(int xScreen, int yScreen, double* xGraph, double* yGraph) const;

    // perform linear/log transformations
    void linearToLog(CoordSettings coord, double* xTheta, double* yR) const;
    void logToLinear(CoordSettings coord, double* xTheta, double* yR) const;
        
    // number of axis and scale point points
    int m_axisPointCount;
    int m_scalePointCount;
    
    // flags to indicate when screenToGraph and graphToScreen are defined, which means
    // there are three non-collinear axis points (for axes transformation) or
    // two scale bar points (for scale bar transformation)
    bool m_validAxes;
    bool m_validScale;

    // matrix to convert vector from screen coordinates to graph coordinates
    double m_screenToGraph [3] [3];

    // matrix to convert vector from graph coordinates to screen coordinates
    double m_graphToScreen [3] [3];

    // log to linear transformation keeps the same coordinate at two points, but all
    // other points are warped. example: 1->1 and 100->100, but 50.5->10 (since 50.5 is
    // midpoint of untransformed values and 10 is midpoint of transformed values). this
    // transformation works out to xT = x0U * exp (log (x1T / x0T) * (xU - x0U) / (x1U - x0U))
    // which is recast as xT = a * exp (b * xU). inverse transformation is xU = log (xT / a) / b.
    // the two points selected for the boundary conditions are the pair with the widest separation
    // in value
    double m_aX, m_aY;
    double m_bX, m_bY;
};

#endif // TRANSFORM_H
