/***************************************************************************
                          digittool.cpp  -  description
                             -------------------
    begin                : Tue Nov 5 2002
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

#include <stdio.h>

#include <qobject.h>
#include <qmessagebox.h>
#include <qapplication.h>
//Added by qt3to4:
#include <QList>
#include <QMouseEvent>
#include <Q3PtrList>
#include <QKeyEvent>

#include "digittool.h"
#include "digitdoc.h"
#include "digitview.h"
#include "axispointdlg.h"
#include "scalebardlg.h"
#include "segment.h"
#include "digitdebug.h"
#include "main.h"


// define the tool list before its members
Q3PtrList<DigitTool> DigitTool::toolList;

// define the members of the tool list
static PointAxisTool axisTool;
static PointCurveTool curveTool;
static PointMatchTool matchTool;
static PointMeasureTool measureTool;
static ScaleTool scaleTool;
static SegmentTool segmentTool;
static SelectTool selectTool;
    
DigitTool::DigitTool(DigitizeState state) :
  m_state(state)
{
  DigitDebug::ctor(QString("digittool ") + QString::number((ulong) this, 16));
  
  toolList.append(this);
}

DigitTool::~DigitTool()
{
  DigitDebug::dtor(QString("digittool ") + QString::number((ulong) this, 16));
}

DigitTool* DigitTool::findTool(DigitizeState state)
{
  DigitTool* tool;
  for (tool = toolList.first(); tool; tool = toolList.next())
  {
    ASSERT_ENGAUGE(tool != 0);
    if (tool->state() == state)
      return tool;
  }

  FATAL_ENGAUGE(QObject::tr("could not find tool for state %1")
    .arg(state));
  return 0;
}

bool DigitTool::editAxisPoint(DigitView* view, double* x, double* y)
{
  // if first selected item is an axis point, edit its properties
  while (true)
  {
    ASSERT_ENGAUGE(view != 0);
    AxisPointDlg* dlg = new AxisPointDlg(view->document(), *x, *y);
    CHECK_PTR_ENGAUGE(dlg);
    int rtn = dlg->exec();
    if (rtn == QDialog::Accepted)
    {
      // get validated graph coordinates
      *x = dlg->x();
      *y = dlg->y();
    }

    delete dlg;

    if ((rtn != QDialog::Accepted) ||
      (view->document()->validAxisPoint(*x, *y)))
      return rtn == QDialog::Accepted;
    else
    {
      ASSERT_ENGAUGE(view->document() != 0);
      QMessageBox::warning(0, view->document()->title(),
        QObject::tr("Log scale coordinates must be greater than zero"));
    }
  }
}

bool DigitTool::editScalePoint(DigitView* view, double* length)
{
  // if first selected item is a scale bar point, edit its properties
  while (true)
  {
    ASSERT_ENGAUGE(view != 0);
    ASSERT_ENGAUGE(view->document() != 0);
    ScaleBarDlg* dlg = new ScaleBarDlg(view->document(), *length);
    CHECK_PTR_ENGAUGE(dlg);
    int rtn = dlg->exec();
    if (rtn == QDialog::Accepted)
    {
      // get validated graph coordinates
      sscanf(dlg->length(), "%lg", length);
    }

    delete dlg;

    if ((rtn != QDialog::Accepted) ||
      (*length > 0.0))
      return rtn == QDialog::Accepted;
    else
      QMessageBox::warning(0, view->document()->title(),
        QObject::tr("Scale bar lengths must have positive nonzero length"));
  }
}

void DigitTool::keyPressEvent(DigitView* view, QKeyEvent* e)
{
  // handle the arrow keys
  ASSERT_ENGAUGE(view != 0);

  ASSERT_ENGAUGE(e != 0);
  switch (e->key())
  {
  case Qt::Key_Up:
    view->moveSelection(QPoint(0, -1));
    break;
  case Qt::Key_Down:
    view->moveSelection(QPoint(0, +1));
    break;
  case Qt::Key_Left:
    view->moveSelection(QPoint(-1, 0));
    break;
  case Qt::Key_Right:
    view->moveSelection(QPoint(+1, 0));
    break;
  default:
    e->ignore();
    break;
  }
}

void DigitTool::keyReleaseEvent(DigitView* view, QKeyEvent* e)
{
  ASSERT_ENGAUGE(e != 0);
  switch (e->key())
  {
  case Qt::Key_Up:
  case Qt::Key_Down:
  case Qt::Key_Left:
  case Qt::Key_Right:
    // update lines between points since order may have changed
    ASSERT_ENGAUGE(view != 0);
    ASSERT_ENGAUGE(view->document() != 0);
    view->document()->finishMove();
    break;
  default:
    // let base class handle this
    e->ignore();
    break;
  }
}

PointAxisTool::PointAxisTool() :
  DigitTool(AxisState)
{
  DigitDebug::ctor(QString("pointaxistool ") + QString::number((ulong) this));
}

PointAxisTool::~PointAxisTool()
{
  DigitDebug::dtor(QString("pointaxistool ") + QString::number((ulong) this));
}

void PointAxisTool::contentsMouseDoubleClickEvent(DigitView*, QMouseEvent*)
{
}

void PointAxisTool::contentsMouseMoveEvent(DigitView* view, QMouseEvent* e)
{
  ASSERT_ENGAUGE(view != 0);
  ASSERT_ENGAUGE(view->document() != 0);
  ASSERT_ENGAUGE(e != 0);
  view->document()->trackCursor(e->pos());
}

void PointAxisTool::contentsMousePressEvent(DigitView* view, QMouseEvent* e)
{
  ASSERT_ENGAUGE(view != 0);
  ASSERT_ENGAUGE(view->document() != 0);
  ASSERT_ENGAUGE(e != 0);
  if (!view->document()->bounds().contains(e->pos()))
  {
    QApplication::beep();
    return;
  }

  if (view->document()->axisPointCount() > 2)
  {
    QMessageBox::warning(0, view->document()->title(),
      QObject::tr("No more than three axis points can be created"));
  }
  else
  {
    Point* p;
    p = view->document()->addPoint(e->pos().x(), e->pos().y());

    double x = 0.0, y = 0.0;
    if (editAxisPoint(view, &x, &y))
      view->document()->setAxisPoint(p, x, y);
    else
      view->document()->removeGestatingPoint(p);

    view->document()->sendUpdateGeometry();
  }
}

void PointAxisTool::contentsMouseReleaseEvent(DigitView*, QMouseEvent*)
{
}

void PointAxisTool::keyPressEvent(DigitView* view, QKeyEvent* e)
{
  DigitTool::keyPressEvent(view, e);
}

void PointAxisTool::keyReleaseEvent(DigitView* view, QKeyEvent* e)
{
  DigitTool::keyReleaseEvent(view, e);
}

PointCurveTool::PointCurveTool() :
  DigitTool(CurveState)
{
  DigitDebug::ctor(QString("pointcurvetool ") + QString::number((ulong) this));
}

PointCurveTool::~PointCurveTool()
{
  DigitDebug::dtor(QString("pointcurvetool ") + QString::number((ulong) this));
}

void PointCurveTool::contentsMouseDoubleClickEvent(DigitView*, QMouseEvent*)
{
}

void PointCurveTool::contentsMouseMoveEvent(DigitView* view, QMouseEvent* e)
{
  ASSERT_ENGAUGE(view != 0);
  ASSERT_ENGAUGE(view->document() != 0);
  ASSERT_ENGAUGE(e != 0);
  view->document()->trackCursor(e->pos());
}

void PointCurveTool::contentsMousePressEvent(DigitView* view, QMouseEvent* e)
{
  ASSERT_ENGAUGE(view != 0);
  ASSERT_ENGAUGE(view->document() != 0);
  ASSERT_ENGAUGE(e != 0);
  if (!view->document()->bounds().contains(e->pos()))
  {
    QApplication::beep();
    return;
  }

  view->document()->addPoint(e->pos().x(), e->pos().y());

  view->document()->sendUpdateGeometry();
}

void PointCurveTool::contentsMouseReleaseEvent(DigitView*, QMouseEvent*)
{
}

void PointCurveTool::keyPressEvent(DigitView* view, QKeyEvent* e)
{
  DigitTool::keyPressEvent(view, e);
}

void PointCurveTool::keyReleaseEvent(DigitView* view, QKeyEvent* e)
{
  DigitTool::keyReleaseEvent(view, e);
}

PointMatchTool::PointMatchTool() :
  DigitTool(PointMatchState)
{
  DigitDebug::ctor(QString("pointmatchtool ") + QString::number((ulong) this));
}

PointMatchTool::~PointMatchTool()
{
  DigitDebug::dtor(QString("pointmatchtool ") + QString::number((ulong) this));
}

void PointMatchTool::contentsMouseDoubleClickEvent(DigitView*, QMouseEvent*)
{
}

void PointMatchTool::contentsMouseMoveEvent(DigitView* view, QMouseEvent* e)
{
  ASSERT_ENGAUGE(view != 0);
  ASSERT_ENGAUGE(view->document() != 0);
  ASSERT_ENGAUGE(e != 0);
  view->document()->trackCursor(e->pos());

  view->document()->highlightCandidateMatchPoint(e->pos());
}

void PointMatchTool::contentsMousePressEvent(DigitView* view, QMouseEvent* e)
{
  ASSERT_ENGAUGE(view != 0);
  ASSERT_ENGAUGE(view->document() != 0);
  ASSERT_ENGAUGE(e != 0);
  if (!view->document()->bounds().contains(e->pos()))
  {
    QApplication::beep();
    return;
  }

  if (!view->document()->matchSamplePoint(e->pos()))
    QMessageBox::warning(0, view->document()->title(),
      QObject::tr("Place the cursor over a sample point. Once it is highlighted, click once to start point matching"));
}

void PointMatchTool::contentsMouseReleaseEvent(DigitView*, QMouseEvent*)
{
}

void PointMatchTool::keyPressEvent(DigitView* view, QKeyEvent*e)
{
  // handle the arrow keys
  ASSERT_ENGAUGE(e != 0);
  if ((e->key() == Qt::Key_Up) || (e->key() == Qt::Key_Down) ||
    (e->key() == Qt::Key_Left) || (e->key() == Qt::Key_Right) ||
    (e->key() == Qt::Key_Space))
   {
    ASSERT_ENGAUGE(view != 0);
    ASSERT_ENGAUGE(view->document() != 0);
    if ((e->key() == Qt::Key_Up) || (e->key() == Qt::Key_Right))
      view->document()->matchPointAccept();
    else if (e->key() == Qt::Key_Space)
      view->document()->matchPointReject();
    else
      view->document()->matchPointRemove();
  }
  else
    // let base class handle this
    e->ignore();
}

void PointMatchTool::keyReleaseEvent(DigitView* view, QKeyEvent* e)
{
  DigitTool::keyReleaseEvent(view, e);
}

PointMeasureTool::PointMeasureTool() :
  DigitTool(MeasureState)
{
  DigitDebug::ctor(QString("pointmeasuretool ") + QString::number((ulong) this));
}

PointMeasureTool::~PointMeasureTool()
{
  DigitDebug::dtor(QString("pointmeasuretool ") + QString::number((ulong) this));
}

void PointMeasureTool::contentsMouseDoubleClickEvent(DigitView*, QMouseEvent*)
{
}

void PointMeasureTool::contentsMouseMoveEvent(DigitView* view, QMouseEvent* e)
{
  ASSERT_ENGAUGE(view != 0);
  ASSERT_ENGAUGE(view->document() != 0);
  ASSERT_ENGAUGE(e != 0);
  view->document()->trackCursor(e->pos());
}

void PointMeasureTool::contentsMousePressEvent(DigitView* view, QMouseEvent* e)
{
  ASSERT_ENGAUGE(view != 0);
  ASSERT_ENGAUGE(view->document() != 0);
  ASSERT_ENGAUGE(e != 0);
  if (!view->document()->bounds().contains(e->pos()))
  {
    QApplication::beep();
    return;
  }

  view->document()->addPoint(e->pos().x(), e->pos().y());

  view->document()->sendUpdateGeometry();
}

void PointMeasureTool::contentsMouseReleaseEvent(DigitView*, QMouseEvent*)
{
}

void PointMeasureTool::keyPressEvent(DigitView* view, QKeyEvent* e)
{
  DigitTool::keyPressEvent(view, e);
}

void PointMeasureTool::keyReleaseEvent(DigitView* view, QKeyEvent* e)
{
  DigitTool::keyReleaseEvent(view, e);
}

ScaleTool::ScaleTool() :
  DigitTool(ScaleState),
  drawingBar(false)
{
  DigitDebug::ctor(QString("scaletool ") + QString::number((ulong) this));
}

ScaleTool::~ScaleTool()
{
  DigitDebug::dtor(QString("scaletool ") + QString::number((ulong) this));
}

void ScaleTool::contentsMouseDoubleClickEvent(DigitView*, QMouseEvent*)
{
}

void ScaleTool::contentsMouseMoveEvent(DigitView* view, QMouseEvent* e)
{
  ASSERT_ENGAUGE(view != 0);
  ASSERT_ENGAUGE(view->document() != 0);
  ASSERT_ENGAUGE(e != 0);
  view->document()->trackCursor(e->pos());

  if (drawingBar)
  {
    view->document()->removeGestatingPoint(pointEnd);

    pointEnd = view->document()->addPoint(e->pos().x(), e->pos().y());
  }
}

void ScaleTool::contentsMousePressEvent(DigitView* view, QMouseEvent* e)
{
  ASSERT_ENGAUGE(view != 0);
  ASSERT_ENGAUGE(view->document() != 0);
  ASSERT_ENGAUGE(e != 0);
  if (!view->document()->bounds().contains(e->pos()))
  {
    QApplication::beep();
    return;
  }

  if (view->document()->scalePointCount() > 0)
  {
    QMessageBox::warning(0, view->document()->title(),
      QObject::tr("No more than one scale bar can be created"));
  }
  else
  {
    drawingBar = true;
    cursorDown = e->pos();

    pointStart = view->document()->addPoint(cursorDown.x(), cursorDown.y());

    // end point starts out collocated with start point, but then is dragged around.
    // we specify an arbitrary length for setScalePoint which will be replaced after mouse release
    pointEnd = view->document()->addPoint(cursorDown.x(), cursorDown.y());
  }
}

void ScaleTool::contentsMouseReleaseEvent(DigitView* view, QMouseEvent* e)
{
  if (drawingBar)
  {
    drawingBar = false;

    ASSERT_ENGAUGE(view != 0);
    ASSERT_ENGAUGE(view->document() != 0);
    ASSERT_ENGAUGE(e != 0);

    // sanity checks
    if (cursorDown == e->pos())
    {
      QMessageBox::warning(0, view->document()->title(),
        QObject::tr("Scale bar is drawn by clicking and dragging. You must drag to a new point"));

      view->document()->removeGestatingPoint(pointStart);
      view->document()->removeGestatingPoint(pointEnd);

      view->document()->sendUpdateGeometry();

      return;
    }

    double length = 1.0;
    if (editScalePoint(view, &length))
    {
      // there is no single scale bar object, instead there are just two points so arbitrarily
      // set origin to be first point and put second point on x axis
      view->document()->setScalePoint(pointStart, 0.0, 0.0);
      view->document()->setScalePoint(pointEnd, length, 0.0);
    }
    else
    {
      view->document()->removeGestatingPoint(pointStart);
      view->document()->removeGestatingPoint(pointEnd);
    }

    view->document()->sendUpdateGeometry();
  }
}

void ScaleTool::keyPressEvent(DigitView* view, QKeyEvent* e)
{
  DigitTool::keyPressEvent(view, e);
}

void ScaleTool::keyReleaseEvent(DigitView* view, QKeyEvent* e)
{
  DigitTool::keyReleaseEvent(view, e);
}

SegmentTool::SegmentTool() :
  DigitTool(SegmentState),
  activeSegment(0)
{
  DigitDebug::ctor(QString("segmenttool ") + QString::number((ulong) this));
}

SegmentTool::~SegmentTool()
{
  DigitDebug::dtor(QString("segmenttool ") + QString::number((ulong) this));
}

void SegmentTool::activateSegment(DigitView* view, QMouseEvent* e)
{
  // look for segment under the cursor. other drawing objects are ignored
  ASSERT_ENGAUGE(view != 0);
  ASSERT_ENGAUGE(e != 0);
  Q3CanvasItemList list = view->collisionsFiltered(e->pos(), Rtti_SegmentLine);

  if (list.count() > 0)
  {
    // item found under cursor. for speed and reduced flicker, only update the selection
    // if the new segment line is not owned by the active segment
    SegmentLine* line = (SegmentLine*) list.first();

    ASSERT_ENGAUGE(line != 0);
    if (activeSegment != line->segment())
    {

      // replace the single selected segment line by all of the lines owned by the active segment
      activeSegment = line->segment();
      view->selectSegment(activeSegment);
    }
  }
  else
  {
    // no item found under cursor
    view->unselect();
    activeSegment = 0;
  }
}

void SegmentTool::contentsMouseDoubleClickEvent(DigitView*, QMouseEvent*)
{
}

void SegmentTool::contentsMouseMoveEvent(DigitView* view, QMouseEvent* e)
{
  ASSERT_ENGAUGE(view != 0);
  ASSERT_ENGAUGE(view->document() != 0);
  ASSERT_ENGAUGE(e != 0);
  view->document()->trackCursor(e->pos());

  activateSegment(view, e);
}

void SegmentTool::contentsMousePressEvent(DigitView* view, QMouseEvent* e)
{
  ASSERT_ENGAUGE(view != 0);
  ASSERT_ENGAUGE(view->document() != 0);
  ASSERT_ENGAUGE(e != 0);
  if (!view->document()->bounds().contains(e->pos()))
  {
    QApplication::beep();
    return;
  }

  activateSegment(view, e);

  if (activeSegment)
  {
    QList<QPoint> list = activeSegment->fillPoints(view->document()->segmentSettings());
    QList<QPoint>::iterator itr;
    for (itr = list.begin(); itr != list.end(); ++itr)
    {
       view->document()->addPoint((*itr).x(), (*itr).y());
    }
    
    view->document()->sendUpdateGeometry();
  }
}

void SegmentTool::contentsMouseReleaseEvent(DigitView*, QMouseEvent*)
{
}

void SegmentTool::keyPressEvent(DigitView* view, QKeyEvent* e)
{
  DigitTool::keyPressEvent(view, e);
}

void SegmentTool::keyReleaseEvent(DigitView* view, QKeyEvent* e)
{
  DigitTool::keyReleaseEvent(view, e);
}

SelectTool::SelectTool() :
  DigitTool(SelectState),
  selectMode(None)
{
}

SelectTool::~SelectTool()
{
}

void SelectTool::contentsMouseDoubleClickEvent(DigitView*, QMouseEvent*)
{
}

void SelectTool::contentsMouseMoveEvent(DigitView* view, QMouseEvent* e)
{
  ASSERT_ENGAUGE(view != 0);
  ASSERT_ENGAUGE(view->document() != 0);
  ASSERT_ENGAUGE(e != 0);
  view->document()->trackCursor(e->pos());

  QPoint cursorNext = e->pos();

  switch (selectMode)
  {
    case None:
      break;
    case Move:
      view->moveSelection(cursorNext - cursorLast);
      break;
    case AreaSelect:
      view->showAreaSelectMove(cursorDown, cursorLast, cursorNext);
      break;
  }

  cursorLast = cursorNext;
}

void SelectTool::contentsMousePressEvent(DigitView* view, QMouseEvent* e)
{
  ASSERT_ENGAUGE(e != 0);
  cursorDown = e->pos();
  cursorLast = e->pos();

  // look for points under the cursor
  ASSERT_ENGAUGE(view != 0);
  Q3CanvasItemList list = view->collisionsFiltered(cursorDown, Rtti_Point);
  if (!list.empty())
  {
    // click was on item
    if (e->button() == Qt::RightButton)
    {
      if (list.first()->rtti() == Rtti_Point)
      {
        Point* p = (Point *) list.first();
        if (p->pointSet()->name() == AxesPointSetName)
        {
          double x = p->xThetaGraph();
          double y = p->yRGraph();
          if (editAxisPoint(view, &x, &y))
          {
            ASSERT_ENGAUGE(view->document() != 0);
            view->document()->setAxisPoint(p, x, y);
          }
        }
        else if (p->pointSet()->name() == ScalePointSetName)
        {
          double length = p->xThetaGraph();
          if (editScalePoint(view, &length))
          {
            ASSERT_ENGAUGE(view->document() != 0);
            view->document()->setScalePoint(p, length, 0.0);
          }
        }
      }
    }
    else if (e->state() == Qt::ShiftModifier)
    {
      // shift click causes list to be added to, or removed from, the selection
      view->selectItemsToggle(&list);
    }
    else
    {
      // regular click starts a move
      selectMode = Move;
      if (!view->pointIsSelected(list.first()))
        // replace existing selection if any
        view->selectItemsReplace(&list);
    }
  }
  else
  {
    if ((e->state() & Qt::ControlModifier) != 0)
    {
      // this is the start of a click and drag operation, and the control button was 
      // pressed earlier. this branch is needed for the case when the control button
      // was pressed when the focus was elsewhere, since the case when the focus was
      // on the view was handled in SelectTool::keyPressEvent (although this still
      // makes a redundant call to startShowOnlySelectedCurve)
      ASSERT_ENGAUGE(view != 0);
      ASSERT_ENGAUGE(view->document() != 0);
      view->document()->startShowOnlySelectedCurve();
    }

    // start area selection
    selectMode = AreaSelect;
  }
}

void SelectTool::contentsMouseReleaseEvent(DigitView* view, QMouseEvent* e)
{
  Q3CanvasItemList list;
  ASSERT_ENGAUGE(e != 0);
  QPoint cursorNext = e->pos();

  switch (selectMode)
  {
    case None:
      break;
    case Move:
      ASSERT_ENGAUGE(view != 0);
      ASSERT_ENGAUGE(view->document() != 0);
      view->document()->finishMove();
      break;
    case AreaSelect:
      ASSERT_ENGAUGE(view != 0);
      view->showAreaSelectEnd(cursorDown, cursorLast);
      Q3CanvasItemList list = view->collisionsFiltered(cursorDown, cursorNext, Rtti_Point);
      view->selectItemsReplace(&list);
      break;
  }

  selectMode = None;
}

void SelectTool::keyPressEvent(DigitView* view, QKeyEvent* e)
{
  // ideally there would be one key for showing the selected curve, and another
  // key for showing the selected measure. although the control button works fine
  // for the selected curve (which is more important), there is apparently no useful
  // available button for showing the selected measure:
  //   shift - already used for adding new selected points to existing selected points
  //   alt - latches so it requires user to always push it twice to correspond to
  //         the key press/release sequence of other keys
  //   meta - interacts with operating system (in linux at least)
  //   letters - C for curve and M for measure does not work since holding down
  //             a letter key causes autorepeat which results in many key events per
  //             second which then causes flicker on the screen as widgets disappear
  //             and reappear
  // so, due to lack of available options there is no equivalent for showing the
  // selected measure

  // was the just pressed key the control button?
  if (e->key() == Qt::Key_Control)
  {
    // this branch is reached when the control button is pressed while the view has
    // focus. the separate case of pressing the control button with focus elsewhere 
    // followed by click and drag in this view, is handled in 
    // SelectTool::contentsMousePressEvent
    ASSERT_ENGAUGE(view != 0);
    ASSERT_ENGAUGE(view->document() != 0);
    view->document()->startShowOnlySelectedCurve();

    e->ignore();
  }
  else 
  {
    DigitTool::keyPressEvent(view, e);
  }
}

void SelectTool::keyReleaseEvent(DigitView* view, QKeyEvent* e)
{
  // was the just released key the control button?
  if (e->key() == Qt::Key_Control)
  {
    ASSERT_ENGAUGE(view != 0);
    ASSERT_ENGAUGE(view->document() != 0);
    view->document()->stopShowOnlySelectedCurve();

    e->ignore();
  }
  else
  {
    DigitTool::keyReleaseEvent(view, e);
  }
}
