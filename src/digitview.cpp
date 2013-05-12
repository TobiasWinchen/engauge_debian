/***************************************************************************
                          digitview.cpp  -  description
                             -------------------
    begin                : Tue Oct 29 14:43:44 PST 2002
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

//Added by qt3to4:
#include <Q3PointArray>
#include <QCloseEvent>
#include <QPixmap>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWhatsThis>

#include "digitview.h"
#include "digitdoc.h"
#include "digittool.h"
#include "transform.h"
#include "segment.h"
#include "digitdebug.h"
#include "main.h"
#include "img/bannerview.xpm"
#include "defaultsettings.h"

DigitView::DigitView(DigitDoc* doc, QWidget* parent, const char* name, Qt::WindowFlags wFlags) :
 Q3CanvasView(doc->canvas(), parent, name, wFlags),
 m_doc(doc),
 m_rubberBandRect(QRubberBand::Rectangle, this),
 m_rubberBandLine(QRubberBand::Line, this),
 m_zoom(100)
{
  DigitDebug::ctor(QString("digitview ") + QString::number((ulong) this, 16));

  m_rubberBandRect.hide();
  m_rubberBandLine.hide();

  QPixmap bannerIcon(bannerview_xpm);
  setIcon(bannerIcon);
  
  // turn on mouse tracking so cursor coordinates can be displayed even with mouse not pressed
  viewport()->setMouseTracking(true);

  // set focus policy so focusIn events are caught under ms windows. linux
  // catches those events without any help
  setFocusPolicy(Qt::StrongFocus);
  
  m_handles.setAutoDelete(true);
    
  QWhatsThis::add(this, QString(tr("View of the current document\n\n"
    "If document is a graph, add three axis points to define the graph "
    "coordinates, and then add curve points to digitize the curves\n\n"
    "If document is a map, add a scale bar to define the graph "
    "coordinates, and then add measure points to measure the geometry")));
}

DigitView::~DigitView()
{
  DigitDebug::dtor(QString("digitview ") + QString::number((ulong) this, 16));
}

void DigitView::drawRubberBandRectangle(QPoint corner1, QPoint corner2)
{
  // Next line gives error 'QPainter::begin: Widget painting can only begin as a result of a paintEvent'
  //  QPainter p(viewport());

  // Next two lines give error 'QPainter::setPen: Painter not active'
  //  p.setPen(QColor(255, 255, 255));
  //  p.setPen(Qt::DotLine);

  //p.setRasterOp(NotROP);  

  if (m_zoom != 100)
  {
    corner1 = convertZoom(corner1, true);
    corner2 = convertZoom(corner2, true);
  }
  
  // once a scroll movement has occurred the transformation from contents to viewport
  // is no longer the identity matrix. To satisfy the general case, perform the
  // transformation
  //  QPoint c1V = contentsToViewport(corner1);
  //  QPoint c2V = contentsToViewport(corner2);

  m_rubberBandRect.setGeometry (QRect (corner1, corner2).normalized ());
  m_rubberBandRect.show ();

  // Next line gives error 'QPainter::drawRects: Painter not active'
  //  p.drawRect(QRect(c1V, c2V));
}

void DigitView::drawRubberBandLine(QPoint corner1, QPoint corner2)
{
  // Next line gives error 'QPainter::begin: Widget painting can only begin as a result of a paintEvent'
  QPainter p(viewport());

  // Next two lines give error 'QPainter::setPen: Painter not active'
  //  p.setPen(QColor(255, 255, 255));
  //  p.setPen(Qt::DotLine);
  //p.setRasterOp(NotROP);

  if (m_zoom != 100)
  {
    corner1 = convertZoom(corner1, true);
    corner2 = convertZoom(corner2, true);
  }

  // once a scroll movement has occurred the transformation from contents to viewport
  // is no longer the identity matrix. To satisfy the general case, perform the
  // transformation
  //  QPoint c1V = contentsToViewport(corner1);
  //  QPoint c2V = contentsToViewport(corner2);

  m_rubberBandLine.setGeometry (QRect (corner1, corner2).normalized ());
  m_rubberBandLine.show ();

  // Next line gives error 'QPainter::drawLine: Painter not active'
  //  p.drawLine(c1V, c2V);
}

void DigitView::showAreaSelectMove(QPoint cursorDown, QPoint cursorLast, QPoint cursorNext)
{
  if (cursorDown != cursorLast) // remove previous selection rectangle
    drawRubberBandRectangle(cursorDown, cursorLast);
  drawRubberBandRectangle(cursorDown, cursorNext);
}   

void DigitView::showAreaSelectEnd(QPoint cursorDown, QPoint cursorLast)
{
  if (cursorDown != cursorLast) // remove previous selection rectangle
    m_rubberBandRect.hide ();
}

void DigitView::showScaleBarMove(QPoint cursorDown, QPoint cursorLast, QPoint cursorNext)
{
  if (cursorDown != cursorLast) // remove previous selection line segment
    drawRubberBandLine(cursorDown, cursorLast);
  drawRubberBandLine(cursorDown, cursorNext);
}

void DigitView::showScaleBarEnd(QPoint cursorDown, QPoint cursorLast)
{
  if (cursorDown != cursorLast) // remove previous selection line segment
    m_rubberBandLine.hide ();
}

void DigitView::update(DigitView* sender, QRect boundingRect)
{
  if (sender != this)
	{
    // do not use updateContents since that produces bad flicker. perhaps that
    // function is repainting the background, but our background is static
    if (m_zoom == 100)
		{
      repaintContents(boundingRect, false);
		}
		else
		{
      repaintContents(convertZoom(boundingRect, true), false);
		}
	}
}

void DigitView::closeEvent(QCloseEvent*)
{
  // prevent closeEvent processing locally. That is performed in the DigitMain event filter
}

void DigitView::contentsMouseDoubleClickEvent(QMouseEvent* e)
{
  ASSERT_ENGAUGE(m_doc != 0);
  DigitTool* tool = DigitTool::findTool(m_doc->digitizeState());
  if (tool)
	{
    if (m_zoom == 100)
		{
      tool->contentsMouseDoubleClickEvent(this, e);
		}
    else
    {
      QMouseEvent e100(e->type(), convertZoom(e->pos(), false),
        e->globalPos(), e->button(), e->state());

      tool->contentsMouseDoubleClickEvent(this, &e100);
    }
	}
}

void DigitView::contentsMouseMoveEvent(QMouseEvent* e)
{
  ASSERT_ENGAUGE(m_doc != 0);
  DigitTool* tool = DigitTool::findTool(m_doc->digitizeState());
  if (tool)
	{
    if (m_zoom == 100)
		{
      tool->contentsMouseMoveEvent(this, e);
		}
    else
    {
      QMouseEvent e100(e->type(), convertZoom(e->pos(), false),
        e->globalPos(), e->button(), e->state());

      tool->contentsMouseMoveEvent(this, &e100);
    }
	}
}

void DigitView::contentsMousePressEvent(QMouseEvent* e)
{
  // grab focus from previous widget. even after clicking elsewhere the zoom
  // combobox does not relinquish focus automatically, and...
  // 1) user is left wondering why pressing Delete suddenly has no effect
  // 2) user has to somehow think of tabbing to move focus from combobox
  if (!hasFocus())
    setFocus();

  ASSERT_ENGAUGE(m_doc != 0);
  DigitTool* tool = DigitTool::findTool(m_doc->digitizeState());
  if (tool)
	{
    if (m_zoom == 100)
		{
      tool->contentsMousePressEvent(this, e);
		}
		else
    {
      QMouseEvent e100(e->type(), convertZoom(e->pos(), false),
        e->globalPos(), e->button(), e->state());

      tool->contentsMousePressEvent(this, &e100);
    }
	}
}

void DigitView::contentsMouseReleaseEvent(QMouseEvent* e)
{
  ASSERT_ENGAUGE(m_doc != 0);
  DigitTool* tool = DigitTool::findTool(m_doc->digitizeState());
  if (tool)
	{
    if (m_zoom == 100)
		{
      tool->contentsMouseReleaseEvent(this, e);
		}
    else
    {
      QMouseEvent e100(e->type(), convertZoom(e->pos(), false),
        e->globalPos(), e->button(), e->state());

      tool->contentsMouseReleaseEvent(this, &e100);
    }
	}
}

void DigitView::keyPressEvent(QKeyEvent* e)
{
  if (m_doc != 0)
  {
    DigitTool* tool = DigitTool::findTool(m_doc->digitizeState());
    if (tool)
      tool->keyPressEvent(this, e);
  }
}

void DigitView::keyReleaseEvent(QKeyEvent* e)
{
  if (m_doc != 0)
  {
    DigitTool* tool = DigitTool::findTool(m_doc->digitizeState());
    if (tool)
      tool->keyReleaseEvent(this, e);
  }
}

QPoint DigitView::convertZoom(QPoint p, bool to)
{
  double x, y;
  if (to)
    worldMatrix().map((double) p.x(), (double) p.y(), &x, &y);
  else
    inverseWorldMatrix().map((double) p.x(), (double) p.y(), &x, &y);

  return QPoint((int) (x + 0.5), (int) (y + 0.5));
}

QRect DigitView::convertZoom(QRect r, bool to)
{
  double xTL, yTL, xBR, yBR;
  if (to)
  {
    worldMatrix().map((double) r.left(), (double) r.top(), &xTL, &yTL);
    worldMatrix().map((double) r.right(), (double) r.bottom(), &xBR, &yBR);
  }
  else
  {
    inverseWorldMatrix().map((double) r.left(), (double) r.top(), &xTL, &yTL);
    inverseWorldMatrix().map((double) r.right(), (double) r.bottom(), &xBR, &yBR);
  }

  return QRect(
    QPoint((int) (xTL + 0.5), (int) (yTL + 0.5)),
    QPoint((int) (xBR + 0.5), (int) (yBR + 0.5)));
}

QRect DigitView::listBounds(Q3CanvasItemList* list)
{
  int xMin = 0, yMin = 0, xMax = 1, yMax = 1;
  bool first = true;
  Q3CanvasItemList::iterator itr;
  ASSERT_ENGAUGE(list != 0);
  for (itr = list->begin(); itr != list->end(); ++itr)
  {
    ASSERT_ENGAUGE((*itr) != 0);
    QRect bounds = (*itr)->boundingRect();

    if (first || (bounds.left() < xMin))
      xMin = bounds.left();
    if (first || (bounds.top() < yMin))
      yMin = bounds.top();
    if (first || (bounds.right() > xMax))
      xMax = bounds.right();
    if (first || (bounds.bottom() > yMax))
      yMax = bounds.bottom();

    first = false;
  }

  return QRect(QPoint(xMin, yMin), QPoint(xMax, yMax)).normalize();
}

void DigitView::addHandles(const QRect &bounds)
{
  m_handles.clear();

  // show eight handles that represent the bounding rectangle for the selected items. we
  // do not put handles on each point since the points can be too small to fit the
  // handles, and also that would be slower
  for (int i = 0; i < 9; i++)
  {
    QSize handleSize = DefaultSettings::instance().getHandleSize();
    int handleWidth = handleSize.width();
    int handleHeight = handleSize.height();
    
    // looping clockwise starting at top right point
    int x, y;
    if (i < 3)
      x = bounds.left() + bounds.width();
    else if ((i == 3) || (i == 7))
      x = bounds.left() + (bounds.width() + 1) / 2 - handleWidth / 2;
    else
      x = bounds.left() - handleWidth;
    if ((1 < i) && (i < 5))
      y = bounds.top() + bounds.height();
    else if ((i == 1) || (i == 5))
      y = bounds.top() + (bounds.height() + 1) / 2 - handleHeight / 2;
    else
      y = bounds.top() - handleHeight;
    ASSERT_ENGAUGE(document() != 0);
    Q3CanvasRectangle* handle = new Q3CanvasRectangle(x, y, handleWidth, handleHeight, document()->canvas());
    CHECK_PTR_ENGAUGE(handle);
    handle->setBrush(QBrush(Qt::green));
    handle->setPen(QPen(Qt::black, 1));
    handle->setZ(ZHandles);
    handle->show();

    update(0, handle->boundingRect());

    m_handles.append(handle);
  }
}

void DigitView::removeHandles()
{
  Q3PtrListIterator<Q3CanvasRectangle> itr(m_handles);
  Q3CanvasRectangle* handle;
  while ((handle = itr.current()) != 0)
  {
    ASSERT_ENGAUGE(handle != 0);
    QRect bounds = handle->boundingRect();

    m_handles.remove(handle); // autoDelete is on

    update(0, bounds);
  }
}

void DigitView::moveHandles(QPoint delta)
{
  Q3PtrListIterator<Q3CanvasRectangle> itr(m_handles);
  Q3CanvasRectangle* handle;
  while ((handle = itr.current()) != 0)
  {
    ASSERT_ENGAUGE(handle != 0);
    QRect rectOld = handle->boundingRect();

    handle->moveBy((double) delta.x(), (double) delta.y());

    update(0, rectOld);
    update(0, handle->boundingRect());

    ++itr;
  }
}

Q3CanvasItemList DigitView::collisionsFiltered(QPoint p, int rtti)
{
  ASSERT_ENGAUGE(document() != 0);
  ASSERT_ENGAUGE(document()->canvas() != 0);
  Q3CanvasItemList list = document()->canvas()->collisions(p);

  return collisionsFiltered(list, rtti);
}

Q3CanvasItemList DigitView::collisionsFiltered(QPoint p1, QPoint p2, int rtti)
{
  ASSERT_ENGAUGE(document() != 0);
  ASSERT_ENGAUGE(document()->canvas() != 0);
  Q3CanvasItemList list = document()->canvas()->collisions(QRect(p1, p2).normalize());

  return collisionsFiltered(list, rtti);
}

Q3CanvasItemList DigitView::collisionsFiltered(Q3CanvasItemList listIn, int rtti)
{
  // filter out all but the desired types
  Q3CanvasItemList listOut;

  Q3CanvasItemList::iterator itrIn;
  for (itrIn = listIn.begin(); itrIn != listIn.end(); ++itrIn)
  {
    ASSERT_ENGAUGE((*itrIn) != 0);
    if ((*itrIn)->rtti() == rtti)
      listOut.append(*itrIn);
  }

  return listOut;
}

void DigitView::unselect()
{
  removeHandles();

  selectionList.clear();
  ASSERT_ENGAUGE(m_doc != 0);
  m_doc->selectionListChanged();
}

void DigitView::selectSegment(Segment* segment)
{
  ASSERT_ENGAUGE(segment != 0);
  Q3CanvasItemList list = segment->lines();

  selectItemsReplace(&list);
}

void DigitView::selectItemsReplace(Q3CanvasItemList* list)
{
  unselect();

  selectionList = *list;
  ASSERT_ENGAUGE(m_doc != 0);
  m_doc->selectionListChanged();

  removeHandles();
  if (selectionList.count() > 0)
    addHandles(listBounds(&selectionList));
}

void DigitView::selectItemsToggle(Q3CanvasItemList* list)
{
  Q3CanvasItemList::iterator itr;

  ASSERT_ENGAUGE(list != 0);
  for (itr = list->begin(); itr != list->end(); ++itr)
  {
    Q3CanvasItemList::iterator itrS = selectionList.find(*itr);
    if (itrS == selectionList.end())
      selectionList.append(*itr);
    else
      selectionList.remove(itrS);
  }

  ASSERT_ENGAUGE(m_doc != 0);
  m_doc->selectionListChanged();

  removeHandles();
  if (selectionList.count() > 0)
    addHandles(listBounds(&selectionList));
}

void DigitView::moveSelection(QPoint delta)
{
  moveHandles(delta);

  Q3CanvasItemList::iterator itr;
  for (itr = selectionList.begin(); itr != selectionList.end(); ++itr)
  {
    Point* p = safeUpcast(itr);
    if (p != 0)
    {
      QRect boundsLineLeft, boundsPoint, boundsLineRight;

      if (p->linePrev())
        boundsLineLeft = p->boundsLineLeft();
      boundsPoint = p->boundingRect();
      if (p->lineNext())
        boundsLineRight = p->boundsLineRight();

      p->moveBy((double) delta.x(), (double) delta.y());

      if (p->linePrev())
      {
        update(0, boundsLineLeft);
        update(0, p->boundsLineLeft());
      }
      update(0, boundsPoint);
      update(0, p->boundingRect());
      if (p->lineNext())
      {
        update(0, boundsLineRight);
        update(0, p->boundsLineRight());
      }
    }
  }
}

Point* DigitView::safeUpcast(Q3CanvasItemList::iterator itr)
{
  if ((*itr)->rtti() == Rtti_Point)
    return (Point*) (*itr);
  else
    return 0;
}

bool DigitView::pointIsSelected(Q3CanvasItem* item)
{
  return (selectionList.find(item) != selectionList.end());
}

void DigitView::addSampleMatchPoint(const Q3PointArray &samplePointPixels)
{
  // remove stale handles
  removeHandles();

  // add new handles
  //ASSERT_ENGAUGE(samplePointPixels);
  addHandles(samplePointPixels.boundingRect());
}

void DigitView::removeSampleMatchPoint(void)
{
  removeHandles();
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void DigitView::slotZoomChange(const QString &zoom)
{
  QString stripped(zoom);

  if (stripped.right(1) == QString("%"))
    stripped.remove(stripped.length() - 1, 1);

  m_zoom = stripped.toInt();

  double scale = (double) m_zoom / 100.0;
  QMatrix ms;
  ms.scale(scale, scale);
  setWorldMatrix(ms);

  updateContents();
}
