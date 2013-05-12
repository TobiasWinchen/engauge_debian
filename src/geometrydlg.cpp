/***************************************************************************
                          geometrydlg.cpp  -  description
                             -------------------
    begin                : Fri Nov 1 2002
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


#include <qregexp.h>
#include <qapplication.h>
#include <q3dragobject.h>
#include <qclipboard.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <Q3GridLayout>
#include <Q3CString>
#include <QMoveEvent>
#include <QHideEvent>
#include <QKeyEvent>
#include <QWhatsThis>

#include "geometrydlg.h"
#include "digitdebug.h"
#include "digitdef.h"
#include "main.h"
#include "defaultsettings.h"

// mime types are listed in http://www.isi.edu/in-notes/iana/assignments/media-types/.
// QTextDrag specifies the "text" type, and the subtype is chosen to work with other
// applications. there seems to be no mime type/subtype pair that works with gnumeric
//
//                                  ********************
//                                  COMPATIBILITY MATRIX
//                                  ********************
//
//                        drag  copy drag     copy     drag   copy   drag   copy
// subtype                oo    oo   gnumeric gnumeric wexcel wexcel mexcel mexcel
// -------                ----  ---- -------- -------- ------ ------ ------ ------
// <unspecified>[1]       yes   yes  no       
// plain[2]               yes   yes  no       yes      yes    yes
// tab-separated-values   no    yes  no                yes    yes    no     no
// comma-separated-values                                            no     no
//
// where:
//   -an empty field means that combination was never checked
//   -oo specifies the spreadsheet (oocalc) and writing (oowrite) open office 1.0.1 apps in redhat linux 8
//   -gnumeric 1.0.9 in redhat linux 8 could paste from the clipboard, but not accept drag and drop, and
//    only if the klipper tool was brought up to manually select the proper data in the clipboard
//   -wexcel is microsoft excel 2000 in windows xp
//   -mexcel is microsoft excel in mac osx 10.3
//
// [1] the call to setSubtype was skipped
// [2] with the plain subtype, tab delimiters seemed most compatible with other applications

const QString MIME_DELIMITER = "\t";
const Q3CString MIME_SUBTYPE = "plain";

GeometryDlg::GeometryDlg(QWidget* parent, bool curve) :
  QDialog(parent, "geometrydlg", false),
  curve(curve),
  infoTable(0)
{
  DigitDebug::ctor(QString("geometrydlg " + QString::number((ulong) this, 16)));
  
  setCaption(curveReplace("Curve Geometry%1")
    .arg(freewareCaption));

  // display main window in previous position with same size
  DefaultSettings& rSettings = DefaultSettings::instance();
  if (curve)
  {
    move(rSettings.getWindowCurvePosition());
    resize(rSettings.getWindowCurveSize());
  }
  else
  {
    move(rSettings.getWindowMeasurePosition());
    resize(rSettings.getWindowMeasureSize());
  }

  layout = new Q3GridLayout(this, 1, 1);
  CHECK_PTR_ENGAUGE(layout);
 
  infoTable = new InfoTable(this);
  CHECK_PTR_ENGAUGE(infoTable);
  infoTable->setReadOnly(true);
  infoTable->setDragEnabled(true);
  infoTable->setSelectionMode(Q3Table::Single);
  infoTable->setColumnMovingEnabled(true);
  QWhatsThis::add(infoTable, QString(tr("Geometry information\n\nThis window displays geometry information such as "
    "angles, coordinates, distances and areas, for the currently active curve or measure\n\n"
    "Functional area is defined as the area under the function and above the horizontal axis "
    "(useful for curves)\n\n"
    "Polygonal area is defined as the area inside the polygon bounded by the points (useful for measures)\n\n"
    "Selected items can by copied to other applications by clicking and dragging the mouse, and then "
    "either dragging the selected cells, or copying them using Control C\n\n"
    "Columns can be rearranged by pressing the control key and clicking and dragging the mouse")));
    
  layout->addWidget(infoTable, 0, 0);
}

GeometryDlg::~GeometryDlg()
{
  DigitDebug::dtor(QString("geometrydlg " + QString::number((ulong) this, 16)));
}

void GeometryDlg::hideEvent(QHideEvent*)
{
  emit(hideGeometry());
}

void GeometryDlg::moveEvent(QMoveEvent* e)
{
  if (e)
	{
    if (curve)
		{
      DefaultSettings::instance().setWindowCurvePosition(e->pos());
		}
    else
		{
      DefaultSettings::instance().setWindowMeasurePosition(e->pos());
		}
	}
}

void GeometryDlg::resizeEvent(QResizeEvent* e)
{
  if (e)
	{
    if (curve)
		{
			DefaultSettings::instance().setWindowCurveSize(e->size());
		}
    else
		{
      DefaultSettings::instance().setWindowMeasureSize(e->size());
		}
	}
}

QString GeometryDlg::extractSelectedItems(QString delimiter)
{
  QString rtn;
  
  // extracted table is a subset of the original table with unselected rows on the top
  // and bottom, and unselected columns on the left and right, removed
  int rowBegin, colBegin; //inclusive
  int rowEnd, colEnd; // exclusive
  getSelectedItemsBounds(rowBegin, colBegin, rowEnd, colEnd);

  const QString SPACE = " "; // needed so empty corners do not cause rows or columns to be dropped
  for (int row = rowBegin; row < rowEnd; row++)
  {
    for (int col = colBegin; col < colEnd; col++)
    {
      if (infoTable->isSelected(row, col))
      {
        Q3TableItem* pItem = infoTable->item(row, col);
        if (pItem == 0)
          rtn += SPACE;
        else
          rtn += pItem->text();
      }
        
      if (col < colEnd - 1)
        rtn += delimiter;
    }
    rtn += QString("\n");
  }

  return rtn;
}

void GeometryDlg::getSelectedItemsBounds(int& rowBegin, int& colBegin, int& rowEnd, int& colEnd)
{
  ASSERT_ENGAUGE(infoTable != 0);
  rowBegin = infoTable->numRows();
  colBegin = infoTable->numCols();
  rowEnd = 0;
  colEnd = 0;
  
  for (int row = 0; row < infoTable->numRows(); row++)
    for (int col = 0; col < infoTable->numCols(); col++)
    {
      if (infoTable->isSelected(row, col))
      {
        if (row < rowBegin)
          rowBegin = row;
        if (col < colBegin)
          colBegin = col;
        if (row >= rowEnd)
          rowEnd = row + 1;
        if (col >= colEnd)
          colEnd = col + 1;
      }
    }
}

void GeometryDlg::displayGeometryInfo(GeometryWindowItemList& rInfo)
{
  if (rInfo != displayedInfo)
  {
    // one or more items have changed
    displayedInfo = rInfo;
    
    clearTable();
  
    // QTable has problems unless numbers of rows and columns are defined before any items are added
    setRowColumnCount(rInfo);

    addNewItemsToTable(rInfo);
  }
}

void GeometryDlg::clearTable()
{
  for (int row = 0; row < infoTable->numRows(); row++)
    for (int col = 0; col < infoTable->numCols(); col++)
      infoTable->clearCell(row, col);
}

void GeometryDlg::setRowColumnCount(GeometryWindowItemList& rInfo)
{
  int maxRow = 0, maxColumn = 0;
  for (GeometryWindowItemList::iterator itr = rInfo.begin(); itr != rInfo.end(); ++itr)
  {
    int row = (*itr).row();
    int col = (*itr).column();

  if (row > maxRow)
    maxRow = row;
  if (col > maxColumn)
    maxColumn = col;
  }
  ASSERT_ENGAUGE(infoTable != 0);
  infoTable->setNumRows(maxRow + 1);
  infoTable->setNumCols(maxColumn + 1);
}

void GeometryDlg::addNewItemsToTable(GeometryWindowItemList& rInfo)
{
  for (GeometryWindowItemList::iterator itr = rInfo.begin(); itr != rInfo.end(); ++itr)
  {
    int row = (*itr).row();
    int col = (*itr).column();
    QString entry = (*itr).entry();

    Q3TableItem* pTableItem;
    if ((pTableItem = infoTable->item(row, col)) != 0)
      pTableItem->setText(entry);
    else
    {
      infoTable->setItem(row, col, new Q3TableItem(infoTable, Q3TableItem::Never, entry));
    }
  }
}

QString GeometryDlg::curveReplace(const char* text)
{
  if (curve)
    return QString(tr(text));
  else
  {
    QString rtn(text);
    rtn.replace(QRegExp("curve"), QString("measure"));
    rtn.replace(QRegExp("Curve"), QString("Measure"));
    return rtn;
  }
}

GeometryDlg::InfoTable::InfoTable(GeometryDlg* parent) :
  Q3Table(parent, "info"),
  parent(parent)
{
}

Q3DragObject* GeometryDlg::InfoTable::dragObject()
{
  ASSERT_ENGAUGE(parent != 0);
  Q3TextDrag* pDrag = new Q3TextDrag(parent->extractSelectedItems(MIME_DELIMITER), this);
  pDrag->setSubtype(MIME_SUBTYPE);

  return pDrag;
}

void GeometryDlg::InfoTable::keyPressEvent(QKeyEvent* e)
{
  if ((e->key() == Qt::Key_C) && (e->state() == Qt::ControlModifier))
  {
    ASSERT_ENGAUGE(parent != 0);
    QApplication::clipboard()->setText(parent->extractSelectedItems(MIME_DELIMITER));
  }
  else
    e->ignore();
}
