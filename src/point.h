/***************************************************************************
                          point.h  -  description
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

#ifndef POINT_H
#define POINT_H

#include <q3canvas.h>
#include <qrect.h>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <Q3PtrCollection>
#include <Q3PointArray>

#include "digitdef.h"
#include "pointsetstyles.h"

class PointSet;

// class used to store axis, segment, curve and measure points. Whether a point is an axis
// point or data point is determined by identifying which pointset the point is assigned to.
//
// this class was originally derived from QCanvasPolygon. Unfortunately that class
// does not support an outline (the pen is always NoPen), and we would like to offer
// the option of an empty outline since that might be easier to align.
class Point : public Q3CanvasPolygonalItem
{
  public:

    Point(int xScreen, int yScreen, Q3Canvas* canvas);
    Point(int xScreen, int yScreen, double xThetaGraph, double yRGraph, Q3Canvas* canvas);
    ~Point();

    // explicitly set the parent pointset. this allows access to the pointset style
    void setPointSet(const PointSet* pointSet);
    const PointSet* pointSet(void) { return m_pointSet; }

    // get and set methods for coordinates
    int xScreen(void) { return m_xScreen; }
    int yScreen(void) { return m_yScreen; }
    double xThetaGraph(void) { return m_xThetaGraph; }
    double yRGraph(void) { return m_yRGraph; }
    void setXThetaGraph(double xThetaGraph) { m_xThetaGraph = xThetaGraph; m_xThetaDefined = true; }
    void setYRGraph(double yRGraph) { m_yRGraph = yRGraph; m_yRDefined = true; }

    // axes and scale bar points are created, and displayed, well before their graph coordinates
    // are defined by the user (publicly) and calls to setXThetaGraph and setYRGraph (behind the scenes)
    bool graphCoordsAreDefined(void) { return m_xThetaDefined && m_yRDefined; }
    
    // move the point. this is called often during moves so graph coordinates are not updated
    virtual void moveBy(double xDelta, double yDelta);
    
    // QCanvasPolygonalItem function that returns the bounding points
    virtual Q3PointArray areaPoints(void) const;

    // get and set methods for the attached lines
    Q3CanvasLine* linePrev(void) { return m_linePrev; }
    Q3CanvasLine* lineNext(void) { return m_lineNext; }
    void setLinePrev(Q3CanvasLine* line) { m_linePrev = line; }
    void setLineNext(Q3CanvasLine* line) { m_lineNext = line; }

    // bounds of lines on left and right sides, if they exist
    QRect boundsLineLeft(void) { return m_linePrev->boundingRect(); }
    QRect boundsLineRight(void) { return m_lineNext->boundingRect(); }

    // run time type id
    int rtti(void) const { return Rtti_Point; }
        
    // debug
    void dumpPoint(void);

  protected:
  
    // QCanvasPolygonalItem function that draws the polygonal item using the pen and brush
    void drawShape(QPainter& p);
    
  private:
 
    const PointSet* m_pointSet;
    
    // screen coordinates are in pixels. They are only modified by a click-and-drag operation
    int m_xScreen;
    int m_yScreen;

    // graph coordinates are input by the user for axis points, and are a function
    // of the screen coordinates and the axis points for data points. If the cartesian flag
    // is set then xThetaGraph and yRGraph correspond to x and y respectively, otherwise they
    // correspond to theta and r respectively with the units enumerated in Units
    double m_xThetaGraph;
    double m_yRGraph;

    // lines attached to previous and next points in the pointset. each line is shared by
    // two adjacent points
    Q3CanvasLine* m_linePrev;
    Q3CanvasLine* m_lineNext;

    // true after xTheta and yR graph coordinates have been defined
    bool m_xThetaDefined;
    bool m_yRDefined;
};

// pointer list that also sorts Point objects by xTheta coordinate. compilation
// under Mandrake 10.1 (gcc version unknown) of compareItems will fail unless the 
// Point class is defined above, since the forward declaration is apparently not 
// enough and "invalid use of undefined type 'struct Point'" errors appear
template <class T>
class PointPtrList: public Q3PtrList<T>
{
  protected:

    virtual int compareItems(Q3PtrCollection::Item item1, Q3PtrCollection::Item item2)
    {
      Point* p1 = (Point*) item1;
      Point* p2 = (Point*) item2;
      if (p1->xThetaGraph() < p2->xThetaGraph())
        return -1;
      else if (p1->xThetaGraph() > p2->xThetaGraph())
        return 1;
      else
        return 0;
    }
};

// the point lists are pointer lists (versus value lists) so that, for brief
// intervals, there can be multiple pointsets that share the same points. Further
// information can be found in the PointSets comments
typedef PointPtrList<Point> PointList;
typedef Q3PtrListIterator<Point> PointListIterator;

#endif // POINT_H
