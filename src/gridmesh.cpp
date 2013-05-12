/***************************************************************************
                          gridmesh.cpp  -  description
                             -------------------
    begin                : Mon Dec 8 2003
    copyright            : (C) 2003 by Mark Mitchell
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

#include "gridmesh.h"
#include "transform.h"
#include "digitdebug.h"
#include "main.h"

GridMesh::GridMesh()
{
  DigitDebug::ctor(QString("gridmesh ") + QString::number((ulong) this, 16));
}

GridMesh::~GridMesh()
{
  DigitDebug::dtor(QString("gridmesh ") + QString::number((ulong) this, 16));
}

GridlinesScreen GridMesh::makeGridLines(const Transform* transform,
  const CoordSettings coord, const GridMeshSettings grid)
{
  ASSERT_ENGAUGE(transform != 0);
  if (transform->validAxes())
  {
    if (coord.frame == Cartesian)
      return makeGridX(transform, coord, grid) + makeGridY(transform, coord, grid);
    else
      return makeGridTheta(transform, coord, grid) + makeGridR(transform, coord, grid);
  }

  GridlinesScreen emptyList;
  return emptyList;
}

GridlinesScreen GridMesh::makeGridX(const Transform* transform,
  const CoordSettings coord, const GridMeshSettings grid)
{
  int i;
  int xStartS, yStartS, xStopS, yStopS;
  double xG = grid.startX;
  GridlineScreen gridline;
  GridlinesScreen gridlines;
  for (i = 0; i < grid.countX; i++)
  {
    ASSERT_ENGAUGE(transform != 0);
    transform->xThetaYRToScreen(coord, xG, grid.startY, &xStartS, &yStartS);
    transform->xThetaYRToScreen(coord, xG, grid.stopY, &xStopS, &yStopS);

    gridline.start.setX(xStartS);
    gridline.start.setY(yStartS);
    gridline.stop.setX(xStopS);
    gridline.stop.setY(yStopS);
    gridline.R = false;

    gridlines.append (gridline);

    if (coord.xThetaScale == Linear)
      xG += grid.stepX;
    else
      xG *= grid.stepX;
  }

  return gridlines;
}

GridlinesScreen GridMesh::makeGridY(const Transform* transform,
  const CoordSettings coord, const GridMeshSettings grid)
{
  int i;
  int xStartS, yStartS, xStopS, yStopS;
  double yG = grid.startY;
  GridlineScreen gridline;
  GridlinesScreen gridlines;
  for (i = 0; i < grid.countY; i++)
  {
    ASSERT_ENGAUGE(transform != 0);
    transform->xThetaYRToScreen(coord, grid.startX, yG, &xStartS, &yStartS);
    transform->xThetaYRToScreen(coord, grid.stopX, yG, &xStopS, &yStopS);

    gridline.start.setX(xStartS);
    gridline.start.setY(yStartS);
    gridline.stop.setX(xStopS);
    gridline.stop.setY(yStopS);
    gridline.R = false;

    gridlines.append (gridline);

    if (coord.yRScale == Linear)
      yG += grid.stepY;
    else
      yG *= grid.stepY;
  }

  return gridlines;
}

GridlinesScreen GridMesh::makeGridTheta(const Transform* transform,
  const CoordSettings coord, const GridMeshSettings grid)
{
  int i;
  int xStartS, yStartS, xStopS, yStopS;
  double thetaG = grid.startX;
  GridlineScreen gridline;
  GridlinesScreen gridlines;
  for (i = 0; i < grid.countX; i++)
  {
    ASSERT_ENGAUGE(transform != 0);
    transform->xThetaYRToScreen(coord, thetaG, grid.startY, &xStartS, &yStartS);
    transform->xThetaYRToScreen(coord, thetaG, grid.stopY, &xStopS, &yStopS);

    gridline.start.setX(xStartS);
    gridline.start.setY(yStartS);
    gridline.stop.setX(xStopS);
    gridline.stop.setY(yStopS);
    gridline.R = false;

    thetaG += grid.stepX;
  }

  return gridlines;
}

GridlinesScreen GridMesh::makeGridR(const Transform* transform,
  const CoordSettings coord, const GridMeshSettings grid)
{
  int i;
  int xStartS = 0, yStartS = 0, xStopS = 1, yStopS = 1;
  double rG = grid.startY;
  GridlineScreen gridline;
  GridlinesScreen gridlines;
  for (i = 0; i < grid.countY; i++)
  {
    // for polar coordinates we simply piecewise define the elliptical arc until motivated
    // to implement a better drawing algorithm. segments will be evenly spaced in angle
    // some pdf documents describing alternative algorithms are found in the doc directory.
    // it would have been elegant to use QCanvasEllipses but those are axis-aligned.
    double delta = angleSpacing(coord.thetaUnits);
    bool first = true;
    for (double angle = grid.startX; angle < grid.stopX; angle += delta)
    {
      ASSERT_ENGAUGE(transform != 0);
      transform->xThetaYRToScreen(coord, angle, rG, &xStopS, &yStopS);

      if (first)
      {
        xStartS = xStopS;
        yStartS = yStopS;
      }
      else
      {
        if (dvmag3 (xStopS - xStartS, yStopS - yStartS, 0.0) >= pixelSpacing)
        {
          gridline.start.setX(xStartS);
          gridline.start.setY(yStartS);
          gridline.stop.setX(xStopS);
          gridline.stop.setY(yStopS);
          gridline.R = true;

          xStartS = xStopS;
          yStartS = yStopS;
        }
      }

      first = false;
    }

    if (coord.yRScale == Linear)
      rG += grid.stepY;
    else
      rG *= grid.stepY;
  }

  return gridlines;
}

double GridMesh::angleSpacing(ThetaUnits units)
{
  switch (units)
  {
    case ThetaDegrees:
    default:
      return angleSpacingDeg;
    case ThetaGradians:
      return angleSpacingGrad;
    case ThetaRadians:
      return angleSpacingRad;
  }
}
