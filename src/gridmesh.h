/***************************************************************************
                          gridmesh.h  -  description
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

#ifndef GRIDMESH_H
#define GRIDMESH_H

#include "digitdef.h"
//Added by qt3to4:
#include <QList>

// angle spacing for ellipses is constant until constrained by minimum pixel spacing
const double angleSpacingDeg = 1.0; // total # canvas lines is 360*#grids/spacing
const double angleSpacingGrad = angleSpacingDeg * 400.0 / 360.0;
const double angleSpacingRad = angleSpacingDeg * 3.1415926535 / 180.0;

// minimum pixel spacing for ellipses. value of 4 should work well with 72 dots per inch
const double pixelSpacing = 4;

class Transform;

struct GridlineScreen
{
  QPoint start;
  QPoint stop;

  // true if line represents a radial line
  bool R;
};

typedef QList<GridlineScreen> GridlinesScreen;

// this class computes the screen coordinates of the gridlines. this is called by
// qcanvas-writing code, or by grid removal code which needs to know the locations
// of the gridlines
class GridMesh
{
  public: 

    GridMesh();
    ~GridMesh();                                                 

    // grid lines are rebuilt after change to any of the arguments of this function. they
    // start out hidden and remain so until showGrid is called
    GridlinesScreen makeGridLines(const Transform* transform,
      const CoordSettings coord, const GridMeshSettings grid);

  private:

    // create grid lines for evenly spaced values of x, y, theta and r
    GridlinesScreen makeGridX(const Transform* transform,
      const CoordSettings coord, const GridMeshSettings grid);
    GridlinesScreen makeGridY(const Transform* transform,
      const CoordSettings coord, const GridMeshSettings grid);
    GridlinesScreen makeGridTheta(const Transform* transform,
      const CoordSettings coord, const GridMeshSettings grid);
    GridlinesScreen makeGridR(const Transform* transform,
      const CoordSettings coord, const GridMeshSettings grid);

    // constant spacing for our hokey ellipse drawing algorithm
    double angleSpacing(ThetaUnits unit);
};

#endif // GRIDMESH_H
