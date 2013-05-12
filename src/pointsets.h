/***************************************************************************
                          pointsets.h  -  description
                             -------------------
    begin                : Sat Nov 9 2002
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

#ifndef POINTSETS_H
#define POINTSETS_H

#include <qstringlist.h>
#include <qdatastream.h>
//Added by qt3to4:
#include <Q3TextStream>
#include <Q3PointArray>
#include <QList>
#include <QPixmap>

#include "pointset.h"
#include "mmsubs.h"
#include "geometrywindowitem.h"

class Q3Canvas;
class Transform;

// the pointset list is stored in a value list, while the pointset points are
// stored in pointer lists. This enables making a new copy of the
// pointset list for PointSetsDlg to modify, with the original kept if Cancel
// is selected, otherwise the new copy is kept if OK is selected. Further
// information can be found in the PointList comments
typedef QList<PointSet> PointSetList;
typedef QList<PointSet>::const_iterator PointSetListConstIterator;
typedef QList<PointSet>::iterator PointSetListIterator;

// this container class manages the pointsets in a document. an instance of
// this class can be copied for editing since the pointset list is value based.
// each pointset has pointers to its associated points (on the heap) so
// copying this container class does not duplicate the points
class PointSets
{
  public: 

    PointSets();
    ~PointSets();

    // remove shared objects before deleting pointsets. do not call while objects are shared with other pointsets
    void deleteShared(void);
    
    // add a curve or measure pointset to the pointset list. the axis and scale bar pointsets 
    // are automatically added during construction
    void addCurve(QString name);
    void addMeasure(QString name);

    // return the axes or scale bar pointset
    const PointSet* axes(void) { return &axesPointSet; }
    const PointSet* scale(void) { return &scalePointSet; }

    // add a point to an axes, curve, measure or scale pointset
    void addPointAxes(Q3Canvas* canvas, Point* p, QList<QRect>* updateRectList);
    void addPointCurve(Q3Canvas* canvas, Point* p, QString name, QList<QRect>* updateRectList);
    void addPointMeasure(Q3Canvas* canvas, Point* p, QString name, QList<QRect>* updateRectList);
    void addPointScale(Q3Canvas* canvas, Point* p, QList<QRect>* updateRectList);
    
    // remove a point from its pointset. returns true if point belonged to axes pointset
    bool removePoint(Point* point, QList<QRect>* updateRectList);

    // remove all points, and their lines, for the selected curve or measure pointsets
    void removePointsForSelectedCurves(QStringList* selectedPointSets);
    void removePointsForSelectedMeasures(QStringList* selectedMeasures);

    // remove curve or measure pointset from the pointset list
    void removeCurve(QString name);
    void removeMeasure(QString name);

    // get curve and measure names from pointset list. these will be shown in combobox
    void curveNames(QStringList* list);
    void measureNames(QStringList* list);

    // move the specified curve or measure up or down in the order
    void moveCurveDown(QString name);
    void moveCurveUp(QString name);
    void moveMeasureDown(QString name);
    void moveMeasureUp(QString name);

    // rename a pointset. this make no sense for the axes or scale bar pointsets
    // since they have fixed names
    void setName(QString oldName, QString newName);

    // number of points in specified curve or measure pointset
    int pointCountCurve(QString name);
    int pointCountMeasure(QString name);

    // get and set methods for accessing single pointset style
    PointSetStyle axesStyle(void) { return axesPointSet.style(); }
    PointSetStyle curveStyle(QString name);
    PointSetStyle measureStyle(QString name);
    PointSetStyle scaleStyle(void) { return scalePointSet.style(); }
    void setAxesStyle(PointSetStyle pointSetStyle) { axesPointSet.setStyle(pointSetStyle); }
    void setCurveStyle(QString name, PointSetStyle pointSetStyle);
    void setMeasureStyle(QString name, PointSetStyle pointSetStyle);
    void setScaleStyle(PointSetStyle pointSetStyle) { scalePointSet.setStyle(pointSetStyle); }

    // get methods for accessing multiple pointset styles
    const PointSetStyleList curveStyles();
    const PointSetStyleList measureStyles();

    // show particular pointset, or all pointsets
    void showAxesPoints(void);
    void showScalePoints(void);
    void showCurvePoints(QString curve);
    void showMeasurePoints(QString measure);
    void showAllPoints(void);
    
    // debug these pointsets
    void dump(void);

    // call this after creating new set of pointsets, to attach existing points to new pointsets
    void attachPointsToPointSets(void);

    // return bounds on the coordinates for just axis, or all, pointsets, and true if
    // there were any coordinates
    bool axisSetGraphLimits(CoordSettings coord, const Transform* transform,
      double* xThetaMin, double* xThetaMax,
      double* yRMin, double* yRMax);
    bool pointSetGraphLimits(CoordSettings coord, const Transform* transform, 
      double* xThetaMin, double* xThetaMax,
      double* yRMin, double* yRMax);

    // apply the transformation to set graph coordinates of the points in the curves and 
    // measures. if the operation could cause points to be reordered to keep a curve 
    // single-valued then the optional update list should be used
    void updateGraphCoordinates(CoordSettings coord, const Transform* transform,
      QList<QRect>* updateRectList);

    // return coordinate range for specified pointset
    void singlePointSetGraphLimits(PointSet* p, bool* first,
      double* xThetaMin, double* xThetaMax, double* yRMin, double* yRMax);
    void singlePointSetScreenLimits(PointSet* p, bool* first,
      double* xMin, double* xMax, double* yMin, double* yMax);
      
    // serialize
    void serializeWrite(QDataStream &s) const;
    void serializeRead(QDataStream &s, Q3Canvas* canvas);
    
    // export the curves into a text stream
    void exportToStream(Q3TextStream& str, CoordSettings coord, GridMeshSettings grid,
      ExportSettings xport);
    
    // lists of curves to be included and excluded from export
    QStringList exportIncluded(void);
    QStringList exportExcluded(void);

    // include and exclude a curve from export
    void exportInclude(QString curve);
    void exportExclude(QString curve);

    // return the geometry info for a particular curve or measure pointset
    void geometryInfoCurve(bool validTransform, bool cartesian, QString name,
      GeometryWindowItemList& rInfo);
    void geometryInfoMeasure(bool validTransform, bool cartesian, QString name,
      GeometryWindowItemList& rInfo);

    // has geometry of indicated curve or measure pointset changed
    bool modifiedGeometryCurve(QString name);
    bool modifiedGeometryMeasure(QString name);

    // return point coordinates of specified curve
    Q3PointArray curveCoordinates(QString name);

    // thumbnails for digitize toolbar comboboxes
    QPixmap curveThumbnail(QString name);
    QPixmap measureThumbnail(QString name);

  private:

    enum PointSetType
    {
      AxisPointSet,
      ScalePointSet,
      CurvePointSet,
      MeasurePointSet
    };

    // show specified pointset, and hide all others
    void showSinglePointSet(PointSetType type, QString name = QString(""));

    // find a curve or measure pointset in the pointset list
    PointSet* findCurve(QString name);
    PointSet* findMeasure(QString name);

    // first part of each header has optional gnuplot delimiter, and 'x' or 'theta'
    QString exportHeaderPrefix(CoordSettings coord, ExportSettings xport);

    // return list of x values in ascending order
    QList<double> ascendingXValuesList(CoordSettings coord,
      GridMeshSettings grid, ExportSettings xport, int xPrecision);

    // curves can be exported together in multiple columns, or one after the other in the same column
    void exportToStreamAllCurvesTogether(Q3TextStream& str, CoordSettings coord, GridMeshSettings grid,
      ExportSettings xport, int xPrecision, int yPrecision);
    void exportToStreamEachCurveSeparately(Q3TextStream& str, CoordSettings coord, GridMeshSettings grid,
      ExportSettings xport, int xPrecision, int yPrecision);

    // select smallest possible precision values, to reduce clutter in export, while still
    // including the significant digits
    void selectXYPrecisionsForExport(CoordSettings coord, int* xPrecision, int* yPrecision);

    // adjust, if necessary, the precision in the x or y direction
    void adjustXOrYPrecisionForExport(Scale scale, double vScreenMin, double vScreenMax,
      double vGraphMin, double vGraphMax, int* precision);
  
    // single axis pointset
    PointSet axesPointSet;

    // single scale bar pointset
    PointSet scalePointSet;

    // pointset list containing one or more curve pointsets
    PointSetList curveList;

    // pointset list containing one or more measure pointsets
    PointSetList measureList;
};

#endif // POINTSETS_H
