/***************************************************************************
                          point.cpp  -  description
                             -------------------
    begin                : Wed Oct 30 2002
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

#include <qpainter.h>
#include <q3pointarray.h>

#include "pointset.h"
#include "point.h"
#include "digitdebug.h"
#include "main.h"

Point::Point(int xScreen, int yScreen, Q3Canvas* canvas) :
  Q3CanvasPolygonalItem(canvas),
  m_pointSet(0),
  m_xScreen(xScreen),
  m_yScreen(yScreen),
  m_xThetaGraph(0.0),
  m_yRGraph(0.0),
  m_linePrev(0),
  m_lineNext(0),
  m_xThetaDefined(false),
  m_yRDefined(false)
{
  DigitDebug::ctor(QString("point ") + QString::number((ulong) this, 16));
  
  // this new point is not shown until setPointSet, since it is not connected to any pointset
}

Point::Point(int xScreen, int yScreen,
  double xThetaGraph, double yRGraph, Q3Canvas* canvas) :
  Q3CanvasPolygonalItem(canvas),
  m_pointSet(0),
  m_xScreen(xScreen),
  m_yScreen(yScreen),
  m_xThetaGraph(xThetaGraph),
  m_yRGraph(yRGraph),
  m_linePrev(0),
  m_lineNext(0),
  m_xThetaDefined(true),
  m_yRDefined(true)
{
  DigitDebug::ctor(QString("point ") + QString::number((ulong) this, 16));
  
  // this new point is not shown until setPointSet, since it is not connected to any pointset
}

Point::~Point()
{
  DigitDebug::dtor(QString("point ") + QString::number((ulong) this, 16));
  
  // all subclasses of QCanvasPolygonalItem must call hide in destructor
  hide();
}

void Point::setPointSet(const PointSet* pointSet)
{
  m_pointSet = pointSet;

  // show this point
  setZ(ZPoints);
  show();

  // do not call QCanvasPolygonalItem::invalidate here in an attempt to get points to appear
  // in preview window in osx. doing so will cause bizarre behavior (i.e. points disappear
  // upon closing export setup dialog)
}

Q3PointArray Point::areaPoints() const
{
  ASSERT_ENGAUGE(m_pointSet != 0);
  return PointSetStyles::instance().areaPoints(m_xScreen, m_yScreen, m_pointSet->style());
}

void Point::drawShape(QPainter &p)
{
  ASSERT_ENGAUGE(m_pointSet != 0);
  PointSetStyles::instance().drawShape(p, m_xScreen, m_yScreen, m_pointSet->style());
}

void Point::moveBy(double xDelta, double yDelta)
{
  // moving visible point causes fatal error in qt library during next repaint, so temporarily hide this point
  hide();
  
  // move this point
  Q3CanvasPolygonalItem::moveBy(xDelta, yDelta);

  m_xScreen += (int) xDelta;
  m_yScreen += (int) yDelta;

  // move the attached lines
  if (m_linePrev)
  {
    QPoint start = m_linePrev->startPoint();
    QPoint end = m_linePrev->endPoint();
    m_linePrev->setPoints(start.x(), start.y(), end.x() + (int) xDelta, end.y() + (int) yDelta);
  }
  if (m_lineNext)
  {
    QPoint start = m_lineNext->startPoint();
    QPoint end = m_lineNext->endPoint();
    m_lineNext->setPoints(start.x() + (int) xDelta, start.y() + (int) yDelta, end.x(), end.y());
  }

  // finished with move so make point visible again. see hide() above
  show();
}

void Point::dumpPoint()
{
  qDebug("point this %lx m_pointSet %lx xScreen %d yScreen %d xThetaGraph %g yRGraph %g",
    (unsigned long) this, (unsigned long) m_pointSet, m_xScreen, m_yScreen, m_xThetaGraph, m_yRGraph);
}
