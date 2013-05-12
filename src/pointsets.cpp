/***************************************************************************
                          pointsets.cpp  -  description
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

#include <math.h>

#include "pointsets.h"
#include "transform.h"
#include "digitdebug.h"
#include "main.h"
#include "defaultsettings.h"
#include "geometrywindowitem.h"
//Added by qt3to4:
#include <Q3TextStream>
#include <Q3PointArray>
#include <QList>
#include <QPixmap>

PointSets::PointSets()
{
  DigitDebug::ctor(QString("pointsets ") + QString::number((ulong) this, 16));

  DefaultSettings& rSettings = DefaultSettings::instance();
  
  axesPointSet.setStyle(rSettings.getAxesStyle());
  axesPointSet.setName(AxesPointSetName);

  scalePointSet.setStyle(rSettings.getScaleStyle());
  scalePointSet.setName(ScalePointSetName);
}

PointSets::~PointSets()
{
  DigitDebug::dtor(QString("pointsets ") + QString::number((ulong) this, 16));
}

void PointSets::deleteShared()
{
  axesPointSet.deleteShared();

  scalePointSet.deleteShared();
  
  PointSetListIterator itr;
  for (itr = curveList.begin(); itr != curveList.end(); ++itr)
    (*itr).deleteShared();

  for (itr = measureList.begin(); itr != measureList.end(); ++itr)
    (*itr).deleteShared();
}

void PointSets::addCurve(QString name)
{
  PointSet pointSet;

  pointSet.setStyle(DefaultSettings::instance().getCurveStyle(curveList.count()));
  pointSet.setName(name);

  curveList.append(pointSet);
}

void PointSets::addMeasure(QString name)
{
  PointSet pointSet;

  pointSet.setStyle(DefaultSettings::instance().getMeasureStyle(measureList.count()));
  pointSet.setName(name);

  measureList.append(pointSet);
}

PointSet* PointSets::findCurve(QString name)
{
  PointSetListConstIterator itr;

  for (itr = curveList.begin(); itr != curveList.end(); ++itr)
    if ((*itr).name() == name)
      return (PointSet*) &(*itr);

  // pointset is probably in the process of getting renamed
  //qWarning("unable to find curve name %s", name.latin1());
  return 0;
}

PointSet* PointSets::findMeasure(QString name)
{
  PointSetListConstIterator itr;

  for (itr = measureList.begin(); itr != measureList.end(); ++itr)
    if ((*itr).name() == name)
      return (PointSet*) &(*itr);

  // pointset is probably in the process of getting renamed
  //qWarning("unable to find measure name %s", name.latin1());
  return 0;
}

void PointSets::addPointAxes(Q3Canvas* canvas, Point* p, QList<QRect>* updateRectList)
{
  axesPointSet.addPoint(canvas, p, updateRectList);
}

void PointSets::addPointScale(Q3Canvas* canvas, Point* p, QList<QRect>* updateRectList)
{
  scalePointSet.addPoint(canvas, p, updateRectList);
}

void PointSets::addPointCurve(Q3Canvas* canvas, Point* p, QString name, QList<QRect>* updateRectList)
{
  PointSet* pointSet = findCurve(name);
  if (pointSet != 0)
    pointSet->addPoint(canvas, p, updateRectList);
}

void PointSets::addPointMeasure(Q3Canvas* canvas, Point* p, QString name, QList<QRect>* updateRectList)
{
  PointSet* pointSet = findMeasure(name);
  if (pointSet != 0)
    pointSet->addPoint(canvas, p, updateRectList);
}

bool PointSets::removePoint(Point* p, QList<QRect>* updateRectList)
{
  ASSERT_ENGAUGE(p != 0);
  if (p->pointSet() == &axesPointSet)
  {
    axesPointSet.removePoint(p, updateRectList);
    return true;
  }
  else if (p->pointSet() == &scalePointSet)
  {
    scalePointSet.removePoint(p, updateRectList);
    return true;
  }
  else
  {
    PointSetListIterator itr;

    for (itr = curveList.begin(); itr != curveList.end(); ++itr)
      if (p->pointSet() == &(*itr))
      {
        (*itr).removePoint(p, updateRectList);
        return true;
      }

    for (itr = measureList.begin(); itr != measureList.end(); ++itr)
      if (p->pointSet() == &(*itr))
      {
        (*itr).removePoint(p, updateRectList);
        return true;
      }
  }

  FATAL_ENGAUGE(QObject::tr("curve or measure not found for point (%1,%2)")
    .arg(p->xScreen())
    .arg(p->yScreen()));
  return false;
}

void PointSets::removePointsForSelectedCurves(QStringList* selectedCurves)
{
  PointSetListIterator itr;

  for (itr = curveList.begin(); itr != curveList.end(); ++itr)
    if (selectedCurves->find((*itr).name()) != selectedCurves->end())
      (*itr).removePoints();
}

void PointSets::removePointsForSelectedMeasures(QStringList* selectedMeasures)
{
  PointSetListIterator itr;

  for (itr = measureList.begin(); itr != measureList.end(); ++itr)
    if (selectedMeasures->find((*itr).name()) != selectedMeasures->end())
      (*itr).removePoints();
}

void PointSets::removeCurve(QString name)
{
  PointSetListIterator itr;

  for (itr = curveList.begin(); itr != curveList.end(); ++itr)
    if (name == (*itr).name())
    {
      curveList.remove(itr);
      return;
    }

  FATAL_ENGAUGE("unable to remove curve");
}

void PointSets::removeMeasure(QString name)
{
  PointSetListIterator itr;

  for (itr = measureList.begin(); itr != measureList.end(); ++itr)
    if (name == (*itr).name())
    {
      measureList.remove(itr);
      return;
    }

  FATAL_ENGAUGE("unable to remove measure");
}

int PointSets::pointCountCurve(QString name)
{
  PointSet* pointSet = findCurve(name);
  if (pointSet == 0)
    return 0;
  else
    return pointSet->pointCount();
}

int PointSets::pointCountMeasure(QString name)
{
  PointSet* pointSet = findMeasure(name);
  if (pointSet == 0)
    return 0;
  else
    return pointSet->pointCount();
}

void PointSets::moveCurveDown(QString name)
{
  PointSetListIterator itr;

  for (itr = curveList.begin(); itr != curveList.end(); ++itr)
    if (name == (*itr).name())
    {
      PointSet pointSet = (*itr);
      itr = curveList.remove(itr);
      ++itr;
      curveList.insert(itr, pointSet);
      return;
    }

  FATAL_ENGAUGE("unable to move curve down");
}

void PointSets::moveMeasureDown(QString name)
{
  PointSetListIterator itr;

  for (itr = measureList.begin(); itr != measureList.end(); ++itr)
    if (name == (*itr).name())
    {
      PointSet pointSet = (*itr);
      itr = measureList.remove(itr);
      ++itr;
      measureList.insert(itr, pointSet);
      return;
    }

  FATAL_ENGAUGE("unable to move measure down");
}

void PointSets::moveCurveUp(QString name)
{
  PointSetListIterator itr;

  for (itr = curveList.end(); itr != curveList.begin(); --itr)
    if (itr != curveList.end())
    {
      if (name == (*itr).name())
      {
        PointSet pointSet = (*itr);
        itr = curveList.remove(itr);
        --itr;
        curveList.insert(itr, pointSet);
        return;
      }
    }

  FATAL_ENGAUGE("unable to move curve down");
}

void PointSets::moveMeasureUp(QString name)
{
  PointSetListIterator itr;

  for (itr = measureList.end(); itr != measureList.begin(); --itr)
    if (itr != measureList.end())
    {
      if (name == (*itr).name())
      {
        PointSet pointSet = (*itr);
        itr = measureList.remove(itr);
        --itr;
        measureList.insert(itr, pointSet);
        return;
      }
    }

  FATAL_ENGAUGE("unable to move measure down");
}

void PointSets::setName(QString oldName, QString newName)
{
  ASSERT_ENGAUGE(oldName != AxesPointSetName);
  ASSERT_ENGAUGE(oldName != ScalePointSetName);

  PointSet* pointSet = findCurve(oldName);
  if (pointSet)
  {
    pointSet->setName(newName);
    return;
  }

  pointSet = findMeasure(oldName);
  if (pointSet)
  {
    pointSet->setName(newName);
    return;
  }
}

void PointSets::curveNames(QStringList* list)
{
  PointSetListIterator itr;

  ASSERT_ENGAUGE(list != 0);
  list->clear();

  for (itr = curveList.begin(); itr != curveList.end(); ++itr)
    list->append((*itr).name());
}

void PointSets::measureNames(QStringList* list)
{
  PointSetListIterator itr;

  ASSERT_ENGAUGE(list != 0);
  list->clear();

  for (itr = measureList.begin(); itr != measureList.end(); ++itr)
    list->append((*itr).name());
}

PointSetStyle PointSets::curveStyle(QString name)
{
  PointSet* pointSet = findCurve(name);
  ASSERT_ENGAUGE(pointSet != 0);
  return pointSet->style();
}

PointSetStyle PointSets::measureStyle(QString name)
{
  PointSet* pointSet = findMeasure(name);
  ASSERT_ENGAUGE(pointSet != 0);
  return pointSet->style();
}

void PointSets::setCurveStyle(QString name, PointSetStyle pointSetStyle)
{
  PointSet* pointSet = findCurve(name);
  ASSERT_ENGAUGE(pointSet != 0);
  pointSet->setStyle(pointSetStyle);
}

void PointSets::setMeasureStyle(QString name, PointSetStyle pointSetStyle)
{
  PointSet* pointSet = findMeasure(name);
  ASSERT_ENGAUGE(pointSet != 0);
  pointSet->setStyle(pointSetStyle);
}

const PointSetStyleList PointSets::curveStyles()
{
  PointSetStyleList list;

  PointSetListIterator itr;
  for (itr = curveList.begin(); itr != curveList.end(); ++itr)
    list.append((*itr).style());

  return list;   
}

const PointSetStyleList PointSets::measureStyles()
{
  PointSetStyleList list;

  PointSetListIterator itr;
  for (itr = measureList.begin(); itr != measureList.end(); ++itr)
    list.append((*itr).style());

  return list;   
}

void PointSets::showSinglePointSet(PointSetType type, QString name)
{
  axesPointSet.setVisible(type == AxisPointSet);
  scalePointSet.setVisible(type == ScalePointSet);

  PointSetListIterator itr;

  for (itr = curveList.begin(); itr != curveList.end(); ++itr)
    (*itr).setVisible((type == CurvePointSet) ? (name == (*itr).name()) : false);
  for (itr = measureList.begin(); itr != measureList.end(); ++itr)
    (*itr).setVisible((type == MeasurePointSet) ? (name == (*itr).name()) : false);
}

void PointSets::showAxesPoints(void)
{
  showSinglePointSet(AxisPointSet);
}

void PointSets::showScalePoints(void)
{
  showSinglePointSet(ScalePointSet);
}

void PointSets::showCurvePoints(QString curve)
{
  showSinglePointSet(CurvePointSet, curve);
}

void PointSets::showMeasurePoints(QString measure)
{
  showSinglePointSet(MeasurePointSet, measure);
}

void PointSets::showAllPoints(void)
{
  axesPointSet.show();
  scalePointSet.show();

  PointSetListIterator itr;

  for (itr = curveList.begin(); itr != curveList.end(); ++itr)
    (*itr).show();
  for (itr = measureList.begin(); itr != measureList.end(); ++itr)
    (*itr).show();
}

void PointSets::dump()
{
  axesPointSet.dump();
  scalePointSet.dump();

  PointSetListIterator itr;

  for (itr = curveList.begin(); itr != curveList.end(); ++itr)
    (*itr).dump();

  for (itr = measureList.begin(); itr != measureList.end(); ++itr)
    (*itr).dump();
}

void PointSets::attachPointsToPointSets()
{
  axesPointSet.attachPointsToPointSet();
  scalePointSet.attachPointsToPointSet();

  PointSetListIterator itr;

  for (itr = curveList.begin(); itr != curveList.end(); ++itr)
    (*itr).attachPointsToPointSet();

  for (itr = measureList.begin(); itr != measureList.end(); ++itr)
    (*itr).attachPointsToPointSet();
}

bool PointSets::axisSetGraphLimits(CoordSettings coord, const Transform* transform,
  double* xThetaMin, double* xThetaMax, double* yRMin, double* yRMax)
{
  // update
  QList<QRect> updateRectList;
  updateGraphCoordinates(coord, transform, &updateRectList);

  // get limits
  return axesPointSet.pointSetGraphLimits(xThetaMin, xThetaMax, yRMin, yRMax);
}

bool PointSets::pointSetGraphLimits(CoordSettings coord, const Transform* transform,
  double* xThetaMin, double* xThetaMax, double* yRMin, double* yRMax)
{
  // update and get axis limits
  bool first = axisSetGraphLimits(coord, transform, xThetaMin, xThetaMax, yRMin, yRMax);
  singlePointSetGraphLimits(&scalePointSet, &first, xThetaMin, xThetaMax, yRMin, yRMax);

  PointSetListIterator itr;

  for (itr = curveList.begin(); itr != curveList.end(); ++itr)
    singlePointSetGraphLimits(&(*itr), &first, xThetaMin, xThetaMax, yRMin, yRMax);

  for (itr = measureList.begin(); itr != measureList.end(); ++itr)
    singlePointSetGraphLimits(&(*itr), &first, xThetaMin, xThetaMax, yRMin, yRMax);

  return !first;
}

void PointSets::singlePointSetGraphLimits(PointSet* p, bool* first,
  double* xThetaMin, double* xThetaMax, double* yRMin, double* yRMax)
{
  double xtmin, xtmax, yrmin, yrmax;

  ASSERT_ENGAUGE(p != 0);
  if (p->pointSetGraphLimits(&xtmin, &xtmax, &yrmin, &yrmax))
  {
    if (*first || (xtmin < *xThetaMin))
      *xThetaMin = xtmin;
    if (*first || (xtmax > *xThetaMax))
      *xThetaMax = xtmax;
    if (*first || (yrmin < *yRMin))
      *yRMin = yrmin;
    if (*first || (yrmax > *yRMax))
      *yRMax = yrmax;

    *first = false;
  }
}

void PointSets::singlePointSetScreenLimits(PointSet* p, bool* first,
  double* xMin, double* xMax, double* yMin, double* yMax)
{
  double xmin, xmax, ymin, ymax;

  ASSERT_ENGAUGE(p != 0);
  if (p->pointSetScreenLimits(&xmin, &xmax, &ymin, &ymax))
  {
    if (*first || (xmin < *xMin))
      *xMin = xmin;
    if (*first || (xmax > *xMax))
      *xMax = xmax;
    if (*first || (ymin < *yMin))
      *yMin = ymin;
    if (*first || (ymax > *yMax))
      *yMax = ymax;

    *first = false;
  }
}

void PointSets::updateGraphCoordinates(CoordSettings coord, const Transform* transform,
  QList<QRect>* updateRectList)
{
  // apply transformation
  PointSetListIterator itr;

  for (itr = curveList.begin(); itr != curveList.end(); ++itr)
  {
    (*itr).updateGraphCoordinates(coord, transform);

    (*itr).forceSingleValued(coord, transform, updateRectList);
  }

  for (itr = measureList.begin(); itr != measureList.end(); ++itr)
    (*itr).updateGraphCoordinates(coord, transform);
}

void PointSets::serializeWrite(QDataStream &s) const
{
  axesPointSet.serializeWrite(s);
  scalePointSet.serializeWrite(s);

  PointSetListConstIterator itr;

  s << (const Q_INT32 &) curveList.count();
  for (itr = curveList.begin(); itr != curveList.end(); ++itr)
    (*itr).serializeWrite(s);
  s << (const Q_INT32 &) measureList.count();
  for (itr = measureList.begin(); itr != measureList.end(); ++itr)
    (*itr).serializeWrite(s);
}

void PointSets::serializeRead(QDataStream &s, Q3Canvas* canvas)
{
  axesPointSet.serializeRead(s, canvas);
  axesPointSet.attachPointsToPointSet();
  scalePointSet.serializeRead(s, canvas);
  scalePointSet.attachPointsToPointSet();

  int i;
  int count;
  s >> (Q_INT32 &) count;
  for (i = 0; i < count; i++)
  {
    PointSet pointSet;
    pointSet.serializeRead(s, canvas);
    curveList.append(pointSet);
    curveList.last().attachPointsToPointSet();
  }
  s >> (Q_INT32 &) count;
  for (i = 0; i < count; i++)
  {
    PointSet pointSet;
    pointSet.serializeRead(s, canvas);
    measureList.append(pointSet);
    measureList.last().attachPointsToPointSet();
  }
}

void PointSets::exportToStream(Q3TextStream& str, CoordSettings coord, GridMeshSettings grid,
  ExportSettings xport)
{
  int xPrecision, yPrecision;

  selectXYPrecisionsForExport(coord, &xPrecision, &yPrecision);

  if (xport.layout == AllCurvesOnEachLine)
    exportToStreamAllCurvesTogether(str, coord, grid, xport, xPrecision, yPrecision);
  else
    exportToStreamEachCurveSeparately(str, coord, grid, xport, xPrecision, yPrecision);
}

void PointSets::exportToStreamAllCurvesTogether(Q3TextStream& str, CoordSettings coord,
  GridMeshSettings grid, ExportSettings xport, int xPrecision, int yPrecision)
{
  // list of x values in ascending order
  QList<double> xUsed = ascendingXValuesList(coord, grid, xport, xPrecision);

  if (xport.header != HeaderNone)
  {
    // header
    str << exportHeaderPrefix(coord, xport);
    for (PointSetList::iterator itrG = curveList.begin(); itrG != curveList.end(); ++itrG)
      if ((*itrG).getExport())
      {
        str << delimiter(xport.delimiters);
        str << (*itrG).exportCurveHeader(xport);
      }
    str << "\n";
  }

  // if only allowing raw data, then forgo interpolation
  bool useInterpolation = (xport.pointsSelection != XYFromAllCurves);

  // loop through x values with one per line
  QString xNew;
  QList<double>::iterator itrX;
  for (itrX = xUsed.begin(); itrX != xUsed.end(); ++itrX)
  {
    xNew.setNum(*itrX, 'g', xPrecision);

    str << xNew;
    for (PointSetList::iterator itrG = curveList.begin(); itrG != curveList.end(); ++itrG)
      if ((*itrG).getExport())
      {
        str << delimiter(xport.delimiters);
        str << (*itrG).exportCurvePoint(*itrX, coord, useInterpolation, yPrecision);
      }

    str << "\n";
  }
}

void PointSets::exportToStreamEachCurveSeparately(Q3TextStream& str, CoordSettings coord,
  GridMeshSettings grid, ExportSettings xport, int xPrecision, int yPrecision)
{
  // list of x values in ascending order
  QList<double> xUsed = ascendingXValuesList(coord, grid, xport, xPrecision);

  for (PointSetList::iterator itrG = curveList.begin(); itrG != curveList.end(); ++itrG)
    if ((*itrG).getExport())
    {
      if (xport.header != HeaderNone)
      {
        // header
        str << exportHeaderPrefix(coord, xport);
        str << delimiter(xport.delimiters);
        str << (*itrG).exportCurveHeader(xport);
        str << "\n";
      }

      if (xport.pointsSelection == XYFromAllCurves)
        str << (*itrG).exportCurveAll(xport, xPrecision, yPrecision);
      else
      {
        // loop through x values with one per line
        const bool useInterpolation = true;
        QString xNew;
        QList<double>::iterator itrX;
        for (itrX = xUsed.begin(); itrX != xUsed.end(); ++itrX)
        {
          str << xNew.setNum(*itrX, 'g', xPrecision);
          str << delimiter(xport.delimiters);
          str << (*itrG).exportCurvePoint(*itrX, coord, useInterpolation, yPrecision);
          str << "\n";
        }
      }

      if (xport.header == HeaderGnuplot)
      {
        // gnuplot expects two blank records between successive data blocks
        str << "\n\n";
      }
    }
}

QString PointSets::exportHeaderPrefix(CoordSettings coord, ExportSettings xport)
{
  QString prefix;

  if (xport.header == HeaderGnuplot)
    prefix += "#";

  if (coord.frame == Cartesian)
    prefix += xport.xLabel;
  else
    prefix += xport.thetaLabel;

  return prefix;
}

QList<double> PointSets::ascendingXValuesList(CoordSettings coord,
  GridMeshSettings grid, ExportSettings xport, int xPrecision)
{
  QList<double> list;

  switch (xport.pointsSelection)
  {
    case XFromAllCurves:
    case XYFromAllCurves:
      {
        for (PointSetList::iterator itrG = curveList.begin(); itrG != curveList.end(); ++itrG)
          (*itrG).mergeUniqueXValues(&list, xPrecision);
      }
      break;
    case XFromFirstCurve:
      // for simplicity for the user and in code, we use x values from the
      // first curve, not the first exported curve according to the export flags
      (*curveList.begin()).mergeUniqueXValues(&list, xPrecision);
      break;
    case XFromGridLines:
      {
        double x = grid.startX;
        for (int i = 0; i < grid.countX; i++)
        {
          list.append(x);

          if (coord.xThetaScale == Linear)
            x += grid.stepX;
          else
            x *= grid.stepX;
        }
      }
      break;
  }

  return list;
}

QStringList PointSets::exportIncluded()
{
  QStringList list;

  PointSetListConstIterator itr;

  for (itr = curveList.begin(); itr != curveList.end(); ++itr)
    if ((*itr).getExport())
      list.append((*itr).name());

  return list;
}

QStringList PointSets::exportExcluded()
{
  QStringList list;

  PointSetListConstIterator itr;

  for (itr = curveList.begin(); itr != curveList.end(); ++itr)
    if (!(*itr).getExport())
      list.append((*itr).name());

  return list;
}

void PointSets::exportInclude(QString curve)
{
  PointSetListIterator itr;

  for (itr = curveList.begin(); itr != curveList.end(); ++itr)
    if (curve == (*itr).name())
    {
      (*itr).setExport(true);
      return;
    }
}

void PointSets::exportExclude(QString curve)
{
  PointSetListIterator itr;

  for (itr = curveList.begin(); itr != curveList.end(); ++itr)
    if (curve == (*itr).name())
    {
      (*itr).setExport(false);
      return;
    }
}

void PointSets::geometryInfoCurve(bool validTransform, bool cartesian,
  QString name, GeometryWindowItemList& rInfo)
{
  PointSetListIterator itr;
  int nextRow = 0;

  for (itr = curveList.begin(); itr != curveList.end(); ++itr)
    if (name == (*itr).name())
      (*itr).geometryInfo(validTransform, cartesian, &nextRow, rInfo);
    else
      (*itr).setModifiedGeometry(true);
}

void PointSets::geometryInfoMeasure(bool validTransform, bool cartesian,
  QString name, GeometryWindowItemList& rInfo)
{
  PointSetListIterator itr;
  int nextRow = 0;

  for (itr = measureList.begin(); itr != measureList.end(); ++itr)
    if (name == (*itr).name())
      (*itr).geometryInfo(validTransform, cartesian, &nextRow, rInfo);
    else
      (*itr).setModifiedGeometry(true);
}

bool PointSets::modifiedGeometryCurve(QString name)
{
  PointSetListIterator itr;

  for (itr = curveList.begin(); itr != curveList.end(); ++itr)
    if (name == (*itr).name())
      return (*itr).modifiedGeometry();

  return false;
}

bool PointSets::modifiedGeometryMeasure(QString name)
{
  PointSetListIterator itr;

  for (itr = measureList.begin(); itr != measureList.end(); ++itr)
    if (name == (*itr).name())
      return (*itr).modifiedGeometry();

  return false;
}

Q3PointArray PointSets::curveCoordinates(QString name)
{
  PointSetListIterator itr;

  for (itr = curveList.begin(); itr != curveList.end(); ++itr)
    if (name == (*itr).name())
      return (*itr).pointSetCoordinates();

  Q3PointArray empty;
  return empty;
}

QPixmap PointSets::curveThumbnail(QString name)
{
  PointSet* pointSet = findCurve(name);
  ASSERT_ENGAUGE(pointSet != 0);
  return pointSet->thumbnail();
}

QPixmap PointSets::measureThumbnail(QString name)
{
  PointSet* pointSet = findMeasure(name);
  ASSERT_ENGAUGE(pointSet != 0);
  return pointSet->thumbnail();
}

void PointSets::selectXYPrecisionsForExport(CoordSettings coord, int* xPrecision, int* yPrecision)
{
  // these defaults are the same as QString and QTextStream. however, they are inadequate when
  // the resolution is much smaller than the range, like when numeric date on the x axis
  // ranges from 38242 (9/12/2004) to 38243 (9/13/2004) and the resolution is 0.0033 days/pixel
  *xPrecision = 6;
  *yPrecision = 6;

  // increase the precision if necessary so all significant digits are included
  if ((coord.xThetaScale == Linear) ||
    (coord.yRScale == Linear))
  {
    PointSetListIterator itr;
    
    bool first = true;
    double xThetaMin, xThetaMax, yRMin, yRMax;
    for (itr = curveList.begin(); itr != curveList.end(); ++itr)
      singlePointSetGraphLimits(&(*itr), &first, &xThetaMin, &xThetaMax, &yRMin, &yRMax);

    first = true;
    double xMin, xMax, yMin, yMax;
    for (itr = curveList.begin(); itr != curveList.end(); ++itr)
      singlePointSetScreenLimits(&(*itr), &first, &xMin, &xMax, &yMin, &yMax);

    adjustXOrYPrecisionForExport(coord.xThetaScale, xMin, xMax, xThetaMin, xThetaMax, xPrecision);
    adjustXOrYPrecisionForExport(coord.yRScale, yMin, yMax, yRMin, yRMax, yPrecision);
  }
}

void PointSets::adjustXOrYPrecisionForExport(Scale scale, double vScreenMin, double vScreenMax,
  double vGraphMin, double vGraphMax, int* precision)
{
  if ((scale == Linear) &&
    (vScreenMax - vScreenMin > 0))
  {   
    double vLargestMagnitude = (dabs(vGraphMax) > dabs(vGraphMin)) ?
      dabs(vGraphMax) :
      dabs(vGraphMin);

    double vResolution = (vGraphMax - vGraphMin) / (vScreenMax - vScreenMin);

    // minimum resolution is computed so that moving by a single pixel (which corresponds to
    // jump in graph coordinates of 1-pixel x vResolution) should cause a change in at least
    // one digit of the largest possible exported value (which is vLargestMagnitude)
    int minPrecision = 2 + (int) log10 (vLargestMagnitude / vResolution);

    ASSERT_ENGAUGE(precision != 0);
    if (minPrecision > *precision)
      *precision = minPrecision;
  }
}
