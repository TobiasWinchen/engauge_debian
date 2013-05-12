/***************************************************************************
                          segment.cpp  -  description
                             -------------------
    begin                : Wed Nov 20 2002
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

#include <qimage.h>
#include <qbitmap.h>
//Added by qt3to4:
#include <QList>

#include <math.h>

#include "segment.h"
#include "digitdef.h"
#include "pointsetstyles.h"
#include "digitdebug.h"
#include "main.h"

Segment::Segment(Q3Canvas* canvas, int y) :
  canvas(canvas),
  yLast(y),
  m_length(0.0)
{
  DigitDebug::ctor(QString("segment ") + QString::number((ulong) this, 16));
  
  m_lines.setAutoDelete(true);
}

Segment::~Segment()
{
  DigitDebug::dtor(QString("segment ") + QString::number((ulong) this, 16));
  
  m_lines.clear(); // autoDelete is on
}

void Segment::appendColumn(int x, int y, SegmentSettings seg)
{
  SegmentLine* line = new SegmentLine(canvas, this);
  CHECK_PTR_ENGAUGE(line);
  line->setPoints(x - 1, yLast, x, y);
  line->setPen(PointSetStyles::instance().pointSetPen(seg.lineColor, seg.lineSize));
  line->setZ(ZSegments);

  // do not show this line or its segment. this is handled later
  
  m_lines.append(line);

  // update total length using distance formula
  m_length += sqrt((1.0) * (1.0) + (y - yLast) * (y - yLast));

  yLast = y;
}

void Segment::createAcceptablePoint(bool* pFirst, QList<QPoint>* pList,
  double* xPrev, double* yPrev, double x, double y)
{
  int iOld = (int) (*xPrev + 0.5);
  int jOld = (int) (*yPrev + 0.5);
  int i = (int) (x + 0.5);
  int j = (int) (y + 0.5);
  
  if (*pFirst || (iOld != i) || (jOld != j))
  {
    *xPrev = x;
    *yPrev = y;

    ASSERT_ENGAUGE(pList != 0);
    pList->append(QPoint(i, j));
  }

  *pFirst = false;
}

void Segment::removeUnneededLines(int* foldedLines)
{
  // pathological case is y=0.001*x*x, since the small slope can fool a naive algorithm
  // into optimizing away all but one point at the origin and another point at the far right.
  // from this we see that we cannot simply throw away points that were optimized away since they
  // are needed later to see if we have diverged from the curve
  SegmentLine* lineOlder = 0;
  SegmentLine* line;
  QList<QPoint> removedPoints;
  for (line = m_lines.first(); line; line = m_lines.next())
  {
    if (lineOlder != 0)
    {
      double xLeft = lineOlder->startPoint().x();
      double yLeft = lineOlder->startPoint().y();
      double xInt = lineOlder->endPoint().x();
      double yInt = lineOlder->endPoint().y();
      ASSERT_ENGAUGE(line != 0);
      double xRight = line->endPoint().x();
      double yRight = line->endPoint().y();

      if (pointIsCloseToLine(xLeft, yLeft, xInt, yInt, xRight, yRight) &&
        pointsAreCloseToLine(xLeft, yLeft, removedPoints, xRight, yRight))
      {
        // remove intermediate point, by removing older line and stretching new line to first point
        ++(*foldedLines);
        removedPoints.append(QPoint((int) xInt, (int) yInt));
        m_lines.remove(lineOlder); // autoDelete is on
        line->setPoints((int) xLeft, (int) yLeft, (int) xRight, (int) yRight);
      }
      else
        // keeping this intermediate point and clear out the removed points list
        removedPoints.clear();
    }

    lineOlder = line;
  }
}

bool Segment::pointIsCloseToLine(double xLeft, double yLeft, double xInt, double yInt,
  double xRight, double yRight)
{
  double xProj, yProj;
  projectpointontoline(xInt, yInt, xLeft, yLeft, xRight, yRight, &xProj, &yProj);

  return (
    (xInt - xProj) * (xInt - xProj) +
    (yInt - yProj) * (yInt - yProj) < 0.5 * 0.5);
}

bool Segment::pointsAreCloseToLine(double xLeft, double yLeft, QList<QPoint> removedPoints,
  double xRight, double yRight)
{
  QList<QPoint>::iterator itr;
  for (itr = removedPoints.begin(); itr != removedPoints.end(); ++itr)
    if (!pointIsCloseToLine(xLeft, yLeft, (double) (*itr).x(), (double) (*itr).y(), xRight, yRight))
      return false;

  return true;
}

void Segment::showSegment(bool toggle, SegmentSettings seg, QList<QRect>* updateRectList)
{
  SegmentLine* line;
  for (line = m_lines.first(); line; line = m_lines.next())
  {
    ASSERT_ENGAUGE(line != 0);
    line->setVisible(toggle);

    if (toggle)
    {
      QRect updateRect = QRect(
        line->startPoint(),
        line->endPoint()).normalize();
      updateRect.setLeft(updateRect.left() - seg.lineSize);
      updateRect.setTop(updateRect.top() - seg.lineSize);
      updateRect.setRight(updateRect.right() + seg.lineSize);
      updateRect.setBottom(updateRect.bottom() + seg.lineSize);

      updateRectList->append(updateRect);
    }
  }
}

Q3CanvasItemList Segment::lines()
{
  Q3CanvasItemList list;

  SegmentLine* line;
  for (line = m_lines.first(); line; line = m_lines.next())
    list.append((Q3CanvasLine*) line);

  return list;
}

QList<QPoint> Segment::fillPoints(SegmentSettings seg)
{
  if (seg.fillCorners)
    return fillPointsFillingCorners(seg);
  else
    return fillPointsWithoutFillingCorners(seg);
}

QList<QPoint> Segment::fillPointsFillingCorners(SegmentSettings seg)
{
  QList<QPoint> list;
          
  ASSERT_ENGAUGE(m_lines.first() != 0);
  if (m_lines.count() > 0)
  {
    double xLast = (double) m_lines.first()->startPoint().x();
    double yLast = (double) m_lines.first()->startPoint().y();
    double x, y;

    // variables for createAcceptablePoint
    double xPrev = m_lines.first()->startPoint().x();
    double yPrev = m_lines.first()->startPoint().y();
    
    for (SegmentLine* line = m_lines.first(); line; line = m_lines.next())
    {
      bool firstPointOfLineSegment = true;
      
      ASSERT_ENGAUGE(line != 0);
      double xNext = (double) line->endPoint().x();
      double yNext = (double) line->endPoint().y();

      // distance formula
      double segmentLength = sqrt((xNext - xLast) * (xNext - xLast) + (yNext - yLast) * (yNext - yLast));

      // loop since we might need to insert multiple points within a single line. this
      // is the case when removeUnneededLines has consolidated many segment lines
      double distanceLeft = segmentLength;
      double s = 0.0;
      do
      {
        // coordinates of new point
        x = (1.0 - s) * xLast + s * xNext;
        y = (1.0 - s) * yLast + s * yNext;

        createAcceptablePoint(&firstPointOfLineSegment, &list, &xPrev, &yPrev, x, y);

        distanceLeft -= seg.pointSeparation;

        s += seg.pointSeparation / segmentLength;
      } while (distanceLeft >= seg.pointSeparation);  

      xLast = xNext;
      yLast = yNext;
    }

    // create one more point at end of last segment, if a point was not already created there
    bool firstPointOfLineSegment = true;
    createAcceptablePoint(&firstPointOfLineSegment, &list, &xPrev, &yPrev, xLast, yLast);
  }
  
  return list;
}

QList<QPoint> Segment::fillPointsWithoutFillingCorners(SegmentSettings seg)
{
  QList<QPoint> list;

  ASSERT_ENGAUGE(m_lines.first() != 0);
  if (m_lines.count() > 0)
  {
    double xLast = (double) m_lines.first()->startPoint().x();
    double yLast = (double) m_lines.first()->startPoint().y();
    double x, xNext;
    double y, yNext;
    double distanceCompleted = 0.0;

    // variables for createAcceptablePoint
    bool firstPoint = true;
    double xPrev = m_lines.first()->startPoint().x();
    double yPrev = m_lines.first()->startPoint().y();

    for (SegmentLine* line = m_lines.first(); line; line = m_lines.next())
    {
      ASSERT_ENGAUGE(line != 0);
      xNext = (double) line->endPoint().x();
      yNext = (double) line->endPoint().y();

      // distance formula
      double segmentLength = sqrt((xNext - xLast) * (xNext - xLast) + (yNext - yLast) * (yNext - yLast));
      if (segmentLength > 0.0)
      {
        // loop since we might need to insert multiple points within a single line. this
        // is the case when removeUnneededLines has consolidated many segment lines
        while (distanceCompleted <= segmentLength)
        {
          double s = distanceCompleted / segmentLength;
        
          // coordinates of new point
          x = (1.0 - s) * xLast + s * xNext;
          y = (1.0 - s) * yLast + s * yNext;

          createAcceptablePoint(&firstPoint, &list, &xPrev, &yPrev, x, y);

          distanceCompleted += seg.pointSeparation;
        }

        distanceCompleted -= segmentLength;
      }
      
      xLast = xNext;
      yLast = yNext;
    }
  }

  return list;
}

void Segment::setSettings(SegmentSettings settings)
{
  for (SegmentLine* line = m_lines.first(); line; line = m_lines.next())
  {
    ASSERT_ENGAUGE(line != 0);
    line->setPen(PointSetStyles::instance().pointSetPen(settings.lineColor, settings.lineSize));
  }
}

void Segment::dump()
{
  ASSERT_ENGAUGE(m_lines.first() != 0);
  ASSERT_ENGAUGE(m_lines.last() != 0);
  qDebug("segment %lx lines %d length %g start (%d, %d) stop (%d, %d)",
    (unsigned long) this, m_lines.count(), length(),
    m_lines.first()->startPoint().x(),
    m_lines.first()->startPoint().y(),
    m_lines.last()->endPoint().x(),
    m_lines.last()->endPoint().y());
}
    
SegmentLine::SegmentLine(Q3Canvas* canvas, Segment* segment) :
  Q3CanvasLine(canvas),
  m_segment(segment)
{
}

SegmentLine::~SegmentLine()
{
}
