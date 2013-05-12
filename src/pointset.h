/***************************************************************************
                          pointset.h  -  description
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

#ifndef POINTSET_H
#define POINTSET_H

#include <q3canvas.h>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <QPixmap>
#include <Q3PointArray>
#include <QList>

#include "point.h"
#include "pointsetstyles.h"
#include "mmsubs.h"
#include "geometrywindowitem.h"

const QString AxesPointSetName = QString("Axes");
const QString ScalePointSetName = QString("Scale");
const QString DefaultCurveName = QString("Curve1");
const QString DefaultMeasureName = QString("Measure1");

class Transform;

// this class contains all points and drawing information for a particular pointset
class PointSet
{
  public:

    // need default constructor since the pointsets are collected into a value list
    PointSet();
    ~PointSet();

    // remove shared objects before deleting pointsets. do not call while objects are shared with other pointsets
    void deleteShared(void);
    
    // add point to pointset, not worrying about keeping the pointset single valued (which is for
    // curves only). if new point is on the line between two points then insert it between
    // those two points (desired behavior for curve and measure pointsets, which happens to not affect
    // axes and scale pointsets)
    void addPoint(Q3Canvas* canvas, Point* point, QList<QRect>* updateRectList);

    // remove a point from this pointset
    void removePoint(Point* point, QList<QRect>* updateRectList);

    // remove all points, and their lines, probably since this pointset is about to be deleted
    void removePoints(void);
    
    // name of pointset
    void setName(QString name) { m_name = name; }
    const QString name(void) const { return m_name; }

    // return iterator for looping through the points in this pointset
    const PointList* points(void) const;
    
    // return number of points in this pointset
    int pointCount(void);

    // style get and set methods. setStyle should be followed by global screen refresh
    PointSetStyle style(void) const { return m_style; }
    void setStyle(PointSetStyle style);

    // export get and set methods
    bool getExport(void) const { return m_export; }
    void setExport(bool xport) { m_export = xport; }

    // return the name of this pointset, filtering out any embedded delimiters for easier parsing
    // downstream (which would be performed by external applications)
    QString exportCurveHeader(ExportSettings xport);
    
    // given an x value, return the corresponding y value, interpolating if necessary
    QString exportCurvePoint(double x, CoordSettings coord, bool useInterpolation, int yPrecisio);
    
    // export this pointset with one x-y pair on each line. header and footer are handled elsewhere.
    // the x and y values will be exported with their respective numeric precisions
    QString exportCurveAll(ExportSettings xport, int xPrecision, int yPrecision);

    // show or hide all points
    void show(void);
    void hide(void);
    void setVisible(bool visible);
    
    // display debug information for this pointset
    void dump(void);

    // call this after creating new set of pointsets, to attach existing points to new pointset
    void attachPointsToPointSet(void);

    // apply the transformation to set graph coordinates of the points in the curve and measure pointsets
    void updateGraphCoordinates(CoordSettings coord, const Transform* transform);
    
    // return bounds on the coordinates for this pointset, and true if
    // at least one point was found
    bool pointSetGraphLimits(double *xThetaMin, double *xThetaMax, double *yRMin, double *yRMax);
    bool pointSetScreenLimits(double *xMin, double *xMax, double *yMin, double *yMax);

    // merge the x values of this pointset into a sorted list having unique x values. although
    // this returns numeric x values with full precision, their exported precision must be
    // specified so no adjacent exported x values will have the same value (breaks postprocessing
    // tools such as sql databases)
    void mergeUniqueXValues(QList<double>* list, int xPrecision);

    // serialize
    void serializeWrite(QDataStream &s) const;
    void serializeRead(QDataStream &s, Q3Canvas* canvas);

    // return the geometry info for this pointset
    void geometryInfo(bool validTransform, bool cartesian, int* pNextRow,
      GeometryWindowItemList& rInfo);

    // get and set for modified flag
    bool modifiedGeometry(void) { return m_modifiedGeometry; }
    void setModifiedGeometry(bool mod) { m_modifiedGeometry = mod; }

    // for curve pointsets connected as Single Valued Functions, since the graph coordinates
    // of all points are updated, we must reconnect any points that were reordered. remember,
    // a single valued function has only a single value per xTheta value, so the lines cannot overlap
    void forceSingleValued(CoordSettings coord, const Transform* transform,
      QList<QRect>* updateRectList);

    // return point coordinates of this pointset
    Q3PointArray pointSetCoordinates(void);

    // convert an x or y coordinate to log scale if necessary
    bool adjustForLogScale(Scale scale, double* pointXY);

    // thumbnail for digitize toolbar comboboxes
    QPixmap thumbnail();
    
  private:

    // adding and removing lines involves updating update area and point pointers
    void addLine(Q3Canvas* canvas, Point* pFrom, Point* pTo, QList<QRect>* updateRectList);
    void removeLine(Point* pFrom, Point* pTo, QList<QRect>* updateRectList);

    // true if pointset is single valued
    bool singleValued(CoordSettings coord, const Transform* transform);

    // return the area and distance geometry info for this pointset
    void geometryInfoArea(int* pNextRow, GeometryWindowItemList& rInfo);
    void geometryInfoDistance(int* pNextRow, GeometryWindowItemList& rInfo, bool cartesian);
    void geometryInfoDistancePass1(int i, PointListIterator &itrNext,
      double xLast, double yLast, double x, double y,
      double distance, double totalDistance, int* pNextRow, GeometryWindowItemList& rInfo);
    
    // user-specified name of this pointset
    QString m_name;

    // true/false to include/exclude the curve pointset during export
    bool m_export;
    
    // shape and line style of this pointset
    PointSetStyle m_style;

    // points belonging to this pointset. each point is owned by exactly one pointset, and lives
    // from when the user explicitly creates the point until the user explicitly deletes the point
    PointList m_points;

    // lines belonging to this pointset. points are sorted by ordinate and then linked by lines. all
    // lines are replaced if any of the following occurs (1) a point is added (2) a point is
    // deleted (3) the line style changes (4) the document is loaded. this approach may be a bit
    // slower, but it greatly simplifies the code, and realistically there should not be more
    // than a couple of hundred points per pointset so execution time is trivial
    Q3PtrList<Q3CanvasLine> m_lines;

    // modified flag is true when geometry info for this pointset is not the same as
    // the contents of the corresponding geometry info window, for curves and measures
    bool m_modifiedGeometry;
};

#endif // POINTSET_H
