/***************************************************************************
                          transform.cpp  -  description
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

#include <qmessagebox.h>

#include <math.h>

#include "transform.h"
#include "point.h"
#include "pointset.h"
#include "mmsubs.h"
#include "digitdoc.h"
#include "digitview.h"
#include "digitdebug.h"
#include "main.h"

Transform::Transform(DigitDoc* doc) :
  doc(doc),
  m_axisPointCount(0),
  m_scalePointCount(0),
  m_validAxes(false),
  m_validScale(false),
  m_aX(0.0),
  m_aY(0.0),
  m_bX(0.0),
  m_bY(0.0)
{
  DigitDebug::ctor(QString("transform ") + QString::number((ulong) this, 16));
}

Transform::~Transform()
{
  DigitDebug::dtor(QString("transform ") + QString::number((ulong) this, 16));
}

void Transform::adjust_midvalued_log_coord(Scale scale, double* c0, double* c1, double* c2)
{
  if (scale == Log)
  {
    double cMin = *c0;
    double cMax = cMin;
    if (*c1 < cMin)
      cMin = *c1;
    if (*c1 > cMax)
      cMax = *c1;
    if (*c2 < cMin)
      cMin = *c2;
    if (*c2 > cMax)
      cMax = *c2;
    if ((cMin < *c0) && (*c0 < cMax))
      *c0 = cMin + (cMax - cMin) * (log(*c0) - log(cMin)) / (log(cMax) - log(cMin));
    if ((cMin < *c1) && (*c1 < cMax))
      *c1 = cMin + (cMax - cMin) * (log(*c1) - log(cMin)) / (log(cMax) - log(cMin));
    if ((cMin < *c2) && (*c2 < cMax))
      *c2 = cMin + (cMax - cMin) * (log(*c2) - log(cMin)) / (log(cMax) - log(cMin));
  }
}

void Transform::adjust_midvalued_log_coords(CoordSettings coord, double r_graph [3] [3])
{
  // this function does nothing for linear coordinates. for log coordinates, this
  // function adjusts the middle coordinate since compute_screen_to_graph assumes
  // the coordinates are linear. if the middle log coordinate does not equal the
  // min or max log coordinates, an undesired skew will appear. if two of the axis
  // points share the same log coordinate, the skew will not appear.
  //
  // the only function called after this function that accesses the middle log coordinate
  // will be compute_screen_to_graph
  adjust_midvalued_log_coord(coord.xThetaScale, &r_graph [0] [0], &r_graph [0] [1], &r_graph [0] [2]);
  adjust_midvalued_log_coord(coord.yRScale, &r_graph [1] [0], &r_graph [1] [1], &r_graph [1] [2]);
}

Transform::LogLinReturn Transform::computeLogToFromLinear(CoordSettings coord, double r_graph [3] [3])
{
  LogLinReturn rtn;

  if ((rtn = computeLogToFromLinear(coord.xThetaScale, 0.0, r_graph [0], &m_aX, &m_bX)) != L2L_SUCCESS)
    return rtn;
  if ((rtn = computeLogToFromLinear(coord.yRScale, coord.originRadius, r_graph [1], &m_aY, &m_bY)) != L2L_SUCCESS)
    return rtn;

  return L2L_SUCCESS;
}

Transform::LogLinReturn Transform::computeLogToFromLinear(Scale scale, double logOffset, double r [3],
  double* a, double* b)
{
  if (scale == Log)
  {
    bool first = true;
    double xyMin = 0.0, xyMax = 0.0;
    for (int i = 0; i < 3; i++)
    {
      double rOffset = r [i] + logOffset;
       
      if (first || (rOffset < xyMin))
        xyMin = rOffset;
      if (first || (rOffset > xyMax))
        xyMax = rOffset;
      first = false;
    }

    if (xyMin == xyMax)
      return L2L_NO_SPREAD;
    if (xyMin <= 0.0)
      return L2L_NONPOSITIVE_COORDINATE;

    *b = log (xyMax / xyMin) / (xyMax - xyMin);
    *a = xyMin * exp (- (*b) * xyMin);
  }

  return L2L_SUCCESS;
}

void Transform::computeAxesTransformation(CoordSettings coord, const PointSet* pointSet)
{
  double r_graph [3] [3];
  double r_screen [3] [3];
  ASSERT_ENGAUGE(pointSet != 0);
  const PointList* points = pointSet->points();
  PointListIterator itr(*points);

  m_axisPointCount = 0;
  m_validAxes = false;

  while (itr)
  {
    if (m_axisPointCount == 3)
      FATAL_ENGAUGE("we have more than three axis points, and so are about to overflow an array");

    ASSERT_ENGAUGE((*itr) != 0);
    if ((*itr)->graphCoordsAreDefined())
    {
      double xGraph, yGraph;
      converttocartesian(mmUnitize(coord), (*itr)->xThetaGraph(), (*itr)->yRGraph(),
        coord.originRadius,
        &xGraph, &yGraph);

      r_graph [0] [m_axisPointCount] = xGraph;
      r_graph [1] [m_axisPointCount] = yGraph;
      r_graph [2] [m_axisPointCount] = 1.0;

      r_screen [0] [m_axisPointCount] = (*itr)->xScreen();
      r_screen [1] [m_axisPointCount] = (*itr)->yScreen();
      r_screen [2] [m_axisPointCount] = 1.0;

      ++m_axisPointCount;
    }
    ++itr;
  }

  if (m_axisPointCount == 3)
  {
    adjust_midvalued_log_coords(coord, r_graph);
    
    int rtnS2G = compute_screen_to_graph(r_graph, r_screen,
      m_screenToGraph, m_graphToScreen);

    m_validAxes = false;

    if (rtnS2G == S2G_SUCCESS)
    {
      int rtnL2L = computeLogToFromLinear(coord, r_graph);

      switch(rtnL2L)
      {
        case L2L_SUCCESS:
          break;
        case L2L_NONPOSITIVE_COORDINATE:
          ASSERT_ENGAUGE(doc != 0);
          doc->forwardPermanentMsg(QString(tr("One or more axis points must be fixed.")));

          QMessageBox::warning (0, doc->title(),
          QString(tr("An axis point has a graph coordinate that is zero or negative, which is not "
          "consistent with log scale.")));
          return;
        case L2L_NO_SPREAD:
          ASSERT_ENGAUGE(doc != 0);
          doc->forwardPermanentMsg(QString(tr("One or more axis points must be fixed.")));

          QMessageBox::warning (0, doc->title(),
          QString(tr("The axis points cannot have identical x or y values with log scale.")));
          return;
      }
    }

    m_validAxes = (rtnS2G == S2G_SUCCESS);

    switch(rtnS2G)
    {
      case S2G_SUCCESS:
        ASSERT_ENGAUGE(doc != 0);
        doc->forwardPermanentMsg(QString(tr("The three axis points are correctly defined.")));
        break;
      case S2G_BAD_GRAPH_COORDINATES:
        ASSERT_ENGAUGE(doc != 0);
        doc->forwardPermanentMsg(QString(tr("One or more axis points must be fixed.")));

        QMessageBox::warning (0, doc->title(),
        QString(tr("The graph coordinates of the three axis points cannot be colocated or collinear.\n"
        "Change to Select mode, then right click on an axis point to fix the graph coordinates.")));
        break;
      case S2G_BAD_SCREEN_COORDINATES:
        ASSERT_ENGAUGE(doc != 0);
        doc->forwardPermanentMsg(QString(tr("One or more axis points must be fixed.")));

        QMessageBox::warning (0, doc->title(),
        QString(tr("The three axis points cannot be colocated or collinear.\n"
        "Click and drag an axis point to define the coordinate frame.")));
        break;
    }
  }
  else
  {
    QString text;
    if (m_axisPointCount == 1)
      text = QString(tr("One axis point is defined. Need two more."));
    else
      text = QString(tr("Two axis points are defined. Need one more."));
    ASSERT_ENGAUGE(doc != 0);
    doc->forwardPermanentMsg(text);
  }
}

void Transform::computeScaleTransformation(CoordSettings coord, const PointSet* pointSet)
{
  double r_graph [3] [3];
  double r_screen [3] [3];
  ASSERT_ENGAUGE(pointSet != 0);
  const PointList* points = pointSet->points();
  PointListIterator itr(*points);

  m_scalePointCount = 0;
  m_validScale = false;

  while (itr)
  {
    if (m_scalePointCount == 2)
      FATAL_ENGAUGE("we have more than two scale bar points, and so are about to overflow an array");

    ASSERT_ENGAUGE((*itr) != 0);
    if ((*itr)->graphCoordsAreDefined())
    {
      double xGraph, yGraph;
      converttocartesian(mmUnitize(coord), (*itr)->xThetaGraph(), (*itr)->yRGraph(),
        coord.originRadius,
        &xGraph, &yGraph);

      r_graph [0] [m_scalePointCount] = xGraph;
      r_graph [1] [m_scalePointCount] = yGraph;
      r_graph [2] [m_scalePointCount] = 1.0;

      r_screen [0] [m_scalePointCount] = (*itr)->xScreen();
      r_screen [1] [m_scalePointCount] = (*itr)->yScreen();
      r_screen [2] [m_scalePointCount] = 1.0;

      ++m_scalePointCount;
    }
    ++itr;
  }

  if (m_scalePointCount == 2)
  {
    // create virtual third point along a virtual axis which is just orthogonal to the virtual
    // axis between the other two points. assumes all points are in same z-value (which is 1.0) plane
    double axis1 [3], axis2 [3], z [3] = {0.0, 0.0, 1.0};
    axis1 [0] = r_graph [0] [1] - r_graph [0] [0];
    axis1 [1] = r_graph [1] [1] - r_graph [1] [0];
    axis1 [2] = r_graph [2] [1] - r_graph [2] [0];

    dcross(z, axis1, axis2);

    r_graph [0] [2] = r_graph [0] [0] + axis2 [0];
    r_graph [1] [2] = r_graph [1] [0] + axis2 [1];
    r_graph [2] [2] = 1.0;

    axis1 [0] = r_screen [0] [1] - r_screen [0] [0];
    axis1 [1] = r_screen [1] [1] - r_screen [1] [0];
    axis1 [2] = r_screen [2] [1] - r_screen [2] [0];

    dcross(z, axis1, axis2);

    r_screen [0] [2] = r_screen [0] [0] + axis2 [0];
    r_screen [1] [2] = r_screen [1] [0] + axis2 [1];
    r_screen [2] [2] = 1.0;

    int rtnS2G = compute_screen_to_graph(r_graph, r_screen,
      m_screenToGraph, m_graphToScreen);

    // log to linear transformation is not allowed when using scale bar since it would
    // be such an extremely rare special case that its benefit would be minimal, especially
    // since it would only confuse the majority of users who would not be expecting it

    m_validScale = (rtnS2G == S2G_SUCCESS);

    switch(rtnS2G)
    {
      case S2G_SUCCESS:
        ASSERT_ENGAUGE(doc != 0);
        doc->forwardPermanentMsg(QString(tr("The two scale bar points are correctly defined.")));
        break;
      case S2G_BAD_GRAPH_COORDINATES:
        ASSERT_ENGAUGE(doc != 0);
        doc->forwardPermanentMsg(QString(tr("One or more scale bar points must be fixed.")));

        QMessageBox::warning (0, doc->title(),
        QString(tr("The graph coordinates of the two scale bar points cannot be colocated.\n"
        "Change to Select mode, then right click on a scale bar point to fix the scale.")));
        break;
      case S2G_BAD_SCREEN_COORDINATES:
        ASSERT_ENGAUGE(doc != 0);
        doc->forwardPermanentMsg(QString(tr("One or more scale bar points must be fixed.")));

        QMessageBox::warning (0, doc->title(),
        QString(tr("The two scale bar points cannot be colocated.\n"
        "Click and drag a scale bar point to define the coordinate frame.")));
        break;
    }
  }
  else
  {
    QString text;
    if (m_scalePointCount == 1)
      text = QString(tr("One scale bar point is defined. Need one more."));
    ASSERT_ENGAUGE(doc != 0);
    doc->forwardPermanentMsg(text);
  }
}

void Transform::xThetaYRToScreen(CoordSettings coord, double xTheta, double yR,
  int* xScreen, int* yScreen) const
{
  double xScreenD, yScreenD;
  xThetaYRToScreen(coord, xTheta, yR, &xScreenD, &yScreenD);
  *xScreen = (int) (0.5 + xScreenD);
  *yScreen = (int) (0.5 + yScreenD);
}

void Transform::xThetaYRToScreen(CoordSettings coord, double xTheta, double yR,
  double* xScreen, double* yScreen) const
{
  if (validAxes() || validScale())
  {
    linearToLog(coord, &xTheta, &yR);

    double xGraph, yGraph;
    converttocartesian(mmUnitize(coord), xTheta, yR, coord.originRadius, &xGraph, &yGraph);

    graphToScreen(xGraph, yGraph, xScreen, yScreen);
  }
  else
  {
    // until transform is defined just use identity matrix. this branch is only executed when trying
    // to connect points with lines, and the transform is not currently defined
    *xScreen = xTheta;
    *yScreen = yR;
  }
}

void Transform::screenToXThetaYR(CoordSettings coord, int xScreen, int yScreen,
  double* xTheta, double* yR) const
{
  if (validAxes() || validScale())
  {
    double xGraph, yGraph;
    screenToGraph(xScreen, yScreen, &xGraph, &yGraph);

    convertfromcartesian(mmUnitize(coord), xGraph, yGraph, coord.originRadius, xTheta, yR);

    logToLinear(coord, xTheta, yR);
  }
  else
  {
    // until transform is defined just use identity matrix. this branch is only defined when trying
    // to connect points with lines, and the transform is not currently defined
    *xTheta = xScreen;
    *yR = yScreen;
  }
}

void Transform::graphToScreen(double xGraph, double yGraph, double* xScreen, double* yScreen) const
{
  ASSERT_ENGAUGE(validAxes() || validScale());

  double rScreen [3], rGraph [3];

  rGraph [0] = xGraph;
  rGraph [1] = yGraph;
  rGraph [2] = 1.0;

  dmlt3x1(rScreen, m_graphToScreen, rGraph);

  *xScreen = rScreen [0];
  *yScreen = rScreen [1];
}

void Transform::screenToGraph(int xScreen, int yScreen, double* xGraph, double* yGraph) const
{
  ASSERT_ENGAUGE(validAxes() || validScale());
  
  double rScreen [3], rGraph [3];

  rScreen [0] = (double) xScreen;
  rScreen [1] = (double) yScreen;
  rScreen [2] = 1.0;

  dmlt3x1(rGraph, m_screenToGraph, rScreen);

  *xGraph = rGraph [0];
  *yGraph = rGraph [1];
}

void Transform::linearToLog(CoordSettings coord, double* xTheta, double* yR) const
{
  if (coord.xThetaScale == Log)
  {
    ASSERT_ENGAUGE(m_aX != 0.0);
    ASSERT_ENGAUGE(m_bX != 0.0);
    *xTheta = log (*xTheta / m_aX) / m_bX;
  }
  if (coord.yRScale == Log)
  {
    ASSERT_ENGAUGE(m_aY != 0.0);
    ASSERT_ENGAUGE(m_bY != 0.0);
    *yR = log (*yR / m_aY) / m_bY;
  }
}

void Transform::logToLinear(CoordSettings coord, double* xTheta, double* yR) const
{
  if (coord.xThetaScale == Log)
    *xTheta = m_aX * exp (m_bX * (*xTheta));
  if (coord.yRScale == Log)
    *yR = m_aY * exp (m_bY * (*yR));
}

void Transform::xBasisScreen(CoordSettings coord, double *xScreen, double *yScreen) const
{
  // return basis vector in x direction. do not look at screen increment
  // resulting from graph increment of (0,0) to (1,0), since log scale may
  // be in effect which would crash with a zero
  double xBeforeS, yBeforeS;
  double xAfterS, yAfterS;
  xThetaYRToScreen(coord, 1.0, 1.0, &xBeforeS, &yBeforeS);
  xThetaYRToScreen(coord, 10.0, 1.0, &xAfterS, &yAfterS);
  *xScreen = xAfterS - xBeforeS;
  *yScreen = yAfterS - yBeforeS;
}

void Transform::yBasisScreen(CoordSettings coord, double *xScreen, double *yScreen) const
{
  // return basis vector in x direction. do not look at screen increment
  // resulting from graph increment of (0,0) to (1,0), since log scale may
  // be in effect which would crash with a zero
  double xBeforeS, yBeforeS;
  double xAfterS, yAfterS;
  xThetaYRToScreen(coord, 1.0, 1.0, &xBeforeS, &yBeforeS);
  xThetaYRToScreen(coord, 1.0, 10.0, &xAfterS, &yAfterS);
  *xScreen = xAfterS - xBeforeS;
  *yScreen = yAfterS - yBeforeS;
}

void Transform::dump()
{
  qDebug("transform axispoints %d scalepoints %d validaxes %s validscale %s",
    m_axisPointCount,
    m_scalePointCount,
    (m_validAxes ? "yes" : "no"),
    (m_validScale ? "yes" : "no"));
  if (m_validAxes || m_validScale)
  {
    qDebug("transform screentograph %f %f %f %f %f %f %f %f %f",
      m_screenToGraph [0] [0],    
      m_screenToGraph [0] [1],
      m_screenToGraph [0] [2],
      m_screenToGraph [1] [0],
      m_screenToGraph [1] [1],
      m_screenToGraph [1] [2],
      m_screenToGraph [2] [0],
      m_screenToGraph [2] [1],
      m_screenToGraph [2] [2]);
    qDebug("transform graphtoscreen %f %f %f %f %f %f %f %f %f",
      m_graphToScreen [0] [0],
      m_graphToScreen [0] [1],
      m_graphToScreen [0] [2],
      m_graphToScreen [1] [0],
      m_graphToScreen [1] [1],
      m_graphToScreen [1] [2],
      m_graphToScreen [2] [0],
      m_graphToScreen [2] [1],
      m_graphToScreen [2] [2]);
  }
}
