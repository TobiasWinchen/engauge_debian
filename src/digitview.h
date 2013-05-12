/***************************************************************************
                          digitview.h  -  description
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

#ifndef DIGITVIEW_H
#define DIGITVIEW_H

#include <q3canvas.h>
#include <qwidget.h>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <Q3PointArray>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QRubberBand>

#include "digitdef.h"

class DigitDoc;
class Segment;
class Point;

// display one view of the canvas belonging to the active document
class DigitView : public Q3CanvasView
{
  Q_OBJECT

  friend class DigitDoc;

  public:

    DigitView(DigitDoc* doc, QWidget* parent, const char* name, Qt::WindowFlags wFlags);
    ~DigitView();

    // returns a pointer to the document connected to the view
    DigitDoc* document() const { return m_doc; }
    
    // method to redraw the document contents if it has been modified
    void update(DigitView* sender, QRect boundingRect);

    // show rubber band rectangle around selected area
    void showAreaSelectMove(QPoint cursorDown, QPoint cursorLast, QPoint cursorNext);
    void showAreaSelectEnd(QPoint cursorDown, QPoint cursorLast);
    
    // show rubber band line segment to preview scale bar
    void showScaleBarMove(QPoint cursorDown, QPoint cursorLast, QPoint cursorNext);
    void showScaleBarEnd(QPoint cursorDown, QPoint cursorLast);    

    // return a list of objects in the specified region
    Q3CanvasItemList collisionsFiltered(QPoint p, int rtti);
    Q3CanvasItemList collisionsFiltered(QPoint p1, QPoint p2, int rtti);
    Q3CanvasItemList collisionsFiltered(Q3CanvasItemList listIn, int rtti);
    
    // move everything that is selected. attached lines are moved also 
    void moveSelection(QPoint delta);

    // select and unselect items
    void selectItemsReplace(Q3CanvasItemList* list);
    void selectSegment(Segment* segment);
    void selectItemsToggle(Q3CanvasItemList* list); // select unselected items and unselect selected items
    void unselect(void);

    // return true if point is currently selected
    bool pointIsSelected(Q3CanvasItem* item);

    // return true if anything is currently selected
    bool isSelection(void) { return selectionList.count() > 0; }

    // zoom get. set is performed by slotZoomChange
    int zoom(void) { return m_zoom; }

    // tell all views to add or remove a sample match point
    void addSampleMatchPoint(const Q3PointArray &samplePointPixels);
    void removeSampleMatchPoint(void);

  public slots:

    void slotZoomChange(const QString &zoom);

  protected:

    // event handlers
    virtual void closeEvent(QCloseEvent* e);
    virtual void contentsMouseDoubleClickEvent(QMouseEvent* e);
    virtual void contentsMouseMoveEvent(QMouseEvent* e);
    virtual void contentsMousePressEvent(QMouseEvent* e);
    virtual void contentsMouseReleaseEvent(QMouseEvent* e);
    virtual void keyPressEvent(QKeyEvent* e);
    virtual void keyReleaseEvent(QKeyEvent* e);

  private:

    DigitDoc* m_doc;
    
    // draw rubber band rectangle (around selection area) or line segment (to show future scale bar),
    // described by two points. the rubber band appears as a set of inverted pixels. external
    // code is responsible for calling this an even number of times. the rectangle and line
    // are hidden until needed, at which point the coordinates are set and they are made visible
    void drawRubberBandRectangle(QPoint corner1, QPoint corner2);
    void drawRubberBandLine(QPoint start, QPoint end);
    QRubberBand m_rubberBandRect;
    QRubberBand m_rubberBandLine;

    // items selected in this view
    Q3PtrList<Q3CanvasItem> m_selectedItems;

    // selection list
    Q3CanvasItemList selectionList;

    // only Points can be selected and moved. verify this before making an upcast, returning
    // zero if canvas item is not a point
    Point* safeUpcast(Q3CanvasItemList::iterator itr);

    // handles around the select items or sample match point. there are either 0 or 8 items in this list
    Q3PtrList<Q3CanvasRectangle> m_handles;

    // add handles around selectionList and remove current handles
    void addHandles(const QRect &bounds);
    void removeHandles(void);

    // move handles by translating
    void moveHandles(QPoint delta);

    // transform to/from zoomed coordinates by applying world transform
    QPoint convertZoom(QPoint p, bool to);
    QRect convertZoom(QRect r, bool to);
    
    // return bounding rectangle for a list of canvas items
    QRect listBounds(Q3CanvasItemList* list);

    // zoom factor as a percentage. zooms other than 100 will suffer performance penalty
    // locally and in the Qt library because extra transformations must be performed
    int m_zoom;
};

#endif // DIGITVIEW_H
