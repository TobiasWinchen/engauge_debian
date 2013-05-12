/***************************************************************************
                          grid.cpp  -  description
                             -------------------
    begin                : Tue Nov 19 2002
    copyright            : (C) 2002 by Mark Mitchell
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

#include "grid.h"
#include "gridmesh.h"
#include "transform.h"
#include "digitdebug.h"
#include "main.h"
#include "pointsetstyles.h"

Grid::Grid(GridType gridType, Q3Canvas* canvas) :
  m_gridType(gridType),
  m_canvas(canvas),
  m_grid(false)
{
  DigitDebug::ctor(QString("grid ") + QString::number((ulong) this, 16));
  
  m_lines.setAutoDelete(true);
}

Grid::~Grid()
{
  DigitDebug::dtor(QString("grid ") + QString::number((ulong) this, 16));
  
  m_lines.clear(); // autoDelete is on
}

void Grid::makeGridLines(const Transform* transform, const CoordSettings coord,
  const GridMeshSettings grid)
{
  // remove all old grid lines from previous calls
  m_lines.clear(); // autoDelete is on

  GridMesh gridMesh;
  GridlinesScreen gridlines = gridMesh.makeGridLines(transform, coord, grid);
  GridlinesScreen::iterator itr;
  for (itr = gridlines.begin(); itr != gridlines.end(); ++itr)
  {
    Q3CanvasLine* line = new Q3CanvasLine(m_canvas);
    CHECK_PTR_ENGAUGE(line);
    line->setPoints((*itr).start.x(), (*itr).start.y(), (*itr).stop.x(), (*itr).stop.y());
    line->setPen(gridlinePen((*itr).R));
    line->setZ(ZGridLines);

    m_lines.append(line);
  }
}

QPen Grid::gridlinePen(bool R)
{
  // line width other than 1 is not implemented in Qt 2.3
  const int ONLY_AVAILABLE_WIDTH = 1;

  if (m_gridType == GridTypeRemoval)
    return PointSetStyles::instance().pointSetPen(DigitBlack, ONLY_AVAILABLE_WIDTH);
  else
    if (R)
      return PointSetStyles::instance().pointSetPen(DigitBlack, ONLY_AVAILABLE_WIDTH, Qt::DotLine);
    else
      return PointSetStyles::instance().pointSetPen(DigitBlack, ONLY_AVAILABLE_WIDTH, Qt::DashLine);
}

void Grid::showGrid(bool toggle)
{
  m_grid = toggle;

  Q3CanvasLine* line;
  for (line = m_lines.first(); line; line = m_lines.next())
  {
    ASSERT_ENGAUGE(line != 0);
    line->setVisible(toggle);
  }
}
