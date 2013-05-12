/***************************************************************************
                          segments.cpp  -  description
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
#include <qapplication.h>
//Added by qt3to4:
#include <QList>
#include <QProgressDialog>

#include "segments.h"
#include "segment.h"
#include "digitdebug.h"
#include "main.h"
#include "pointsetstyles.h"
#include "discretize.h"

Segments::Segments(Q3Canvas* canvas) :
  canvas(canvas)
{
  DigitDebug::ctor(QString("segments ") + QString::number((ulong) this, 16));
  
  segments.setAutoDelete(true);
}

Segments::~Segments()
{
  DigitDebug::dtor(QString("segments ") + QString::number((ulong) this, 16));
  
  segments.clear(); // autoDelete is on
}

int Segments::adjacentRuns(bool* columnBool, int yStart, int yStop, int height)
{
  int runs = 0;
  bool inRun = false;
  for (int y = yStart - 1; y <= yStop + 1; y++)
  {
    if ((0 <= y) && (y < height))
    {
      if (!inRun && columnBool [y])
      {
        inRun = true;
        ++runs;
      }
      else if (inRun && !columnBool [y])
        inRun = false;
    }
  }

  return runs;
}

Segment* Segments::adjacentSegment(Segment** lastSegment, int yStart, int yStop, int height)
{
  for (int y = yStart - 1; y <= yStop + 1; y++)
    if ((0 <= y) && (y < height))
      if (lastSegment [y])
        return lastSegment [y];

  return 0;
}

int Segments::adjacentSegments(Segment** lastSegment, int yStart, int yStop, int height)
{
  int segments = 0;
  bool inSegment = false;
  for (int y = yStart - 1; y <= yStop + 1; y++)
  {
    if ((0 <= y) && (y < height))
    {
      if (!inSegment && lastSegment [y])
      {
        inSegment = true;
        ++segments;
      }
      else if (inSegment && !lastSegment [y])
        inSegment = false;
    }
  }

  return segments;
}

QList<QPoint> Segments::fillPoints(SegmentSettings seg)
{
  QList<QPoint> list;
  
  for (Segment* segment = segments.first(); segment; segment = segments.next())
  {
    ASSERT_ENGAUGE(segment != 0);
    list += segment->fillPoints(seg);
  }

  return list;
}
    
void Segments::finishRun(bool* lastBool, bool* nextBool,
  Segment** lastSegment, Segment** currSegment,
  int x, int yStart, int yStop, int height, SegmentSettings set, int* madeLines)
{
  // when looking at adjacent columns, include pixels that touch diagonally since
  // those may also diagonally touch nearby runs in the same column (which would indicate
  // a branch)
  DigitDebug::scanning(QString("column ") + QString::number(x) +
    QString(", rows ") + QString::number(yStart) + QString(" to ") + QString::number(yStop) +
    QString(", runs on left ") + QString::number(adjacentRuns(nextBool, yStart, yStop, height)) +
    QString(", runs on right ") + QString::number(adjacentSegments(lastSegment, yStart, yStop, height)));
  
  // count runs that touch on the left
  if (adjacentRuns(lastBool, yStart, yStop, height) > 1)
    return;
  // count runs that touch on the right
  if (adjacentRuns(nextBool, yStart, yStop, height) > 1)
    return;
  
  Segment* seg;
  if (adjacentSegments(lastSegment, yStart, yStop, height) == 0)
  {
    // this is the start of a new segment
    seg = new Segment(canvas, (int) (0.5 + (yStart + yStop) / 2.0));
    CHECK_PTR_ENGAUGE(seg);

    segments.append(seg);
  }
  else
  {
    // this is the continuation of an existing segment
    seg = adjacentSegment(lastSegment, yStart, yStop, height);

    ++(*madeLines);
    ASSERT_ENGAUGE(seg != 0);
    seg->appendColumn(x, (int) (0.5 + (yStart + yStop) / 2.0), set);
  }

  for (int y = yStart; y <= yStop; y++)
    currSegment [y] = seg;
}

void Segments::loadBool(const Discretize* discretize, bool* columnBool,
  QImage* image, int x)
{
  ASSERT_ENGAUGE(image != 0);
  for (int y = 0; y < image->height(); y++)
    if (x < 0)
      columnBool [y] = false;
    else
      columnBool [y] = discretize->processedPixelIsOn(*image, x, y);
}

void Segments::loadSegment(Segment** columnSegment, int height)
{
  for (int y = 0; y < height; y++)
    columnSegment [y] = 0;
}

void Segments::makeSegments(QImage &imageProcessed, SegmentSettings seg)
{
  segments.clear(); // autoDelete is on

  // statistics that show up in debug spew
  int madeLines = 0;
  int shortLines = 0; // lines rejected since their segments are too short
  int foldedLines = 0; // lines rejected since they could be into other lines

  // debugging with modal progress dialog box is problematic so make switchable
  const bool useDlg = true;

  // for each new column of pixels, loop through the runs. a run is defined as
  // one or more colored pixels that are all touching, with one uncolored pixel or the
  // image boundary at each end of the set. for each set in the current column, count
  // the number of runs it touches in the adjacent (left and right) columns. here is
  // the pseudocode:
  //   if ((L > 1) || (R > 1))
  //     "this run is at a branch point so ignore the set"
  //   else
  //     if (L == 0)
  //       "this run is the start of a new segment"
  //     else
  //       "this run is appended to the segment on the left
  int width = imageProcessed.width();
  int height = imageProcessed.height();

  QProgressDialog* dlg;
  if (useDlg)
  {

    dlg = new QProgressDialog("Scanning segments in image", "Cancel", 0, width);
    CHECK_PTR_ENGAUGE(dlg);
    dlg->setCaption(QString("Progress"));
    dlg->show();
  }

  bool* lastBool = new bool [height];
  CHECK_PTR_ENGAUGE(lastBool);
  bool* currBool = new bool [height];
  CHECK_PTR_ENGAUGE(currBool);
  bool* nextBool = new bool [height];
  CHECK_PTR_ENGAUGE(nextBool);
  Segment** lastSegment = new Segment* [height];
  CHECK_PTR_ENGAUGE(lastSegment);
  Segment** currSegment = new Segment* [height];
  CHECK_PTR_ENGAUGE(currSegment);

  Discretize discretize;
  loadBool(&discretize, lastBool, &imageProcessed, -1);
  loadBool(&discretize, currBool, &imageProcessed, 0);
  loadBool(&discretize, nextBool, &imageProcessed, 1);
  loadSegment(lastSegment, height);

  for (int x = 0; x < width; x++)
  {
    if (useDlg)
    {
      // update progress bar
      dlg->setValue(x);
      qApp->processEvents();

      if (dlg->wasCanceled())
        // quit scanning. only existing segments will be available
        break;
    }

    matchRunsToSegments(x, height, lastBool, lastSegment, currBool, currSegment, nextBool, seg,
      &madeLines, &foldedLines, &shortLines);

    // get ready for next column
    scrollBool(lastBool, currBool, height);
    scrollBool(currBool, nextBool, height);
    if (x + 1 < width)
      loadBool(&discretize, nextBool, &imageProcessed, x + 1);
    scrollSegment(lastSegment, currSegment, height);
  }

  if (useDlg)
  {
    dlg->setValue(width);
    delete dlg;
  }

  DigitDebug::scanning(QString("segment lines created ") + QString::number(madeLines) +
    QString(", too short so removed ") + QString::number(shortLines) +
    QString(", folded together ") + QString::number(foldedLines));

  delete[] lastBool;
  delete[] currBool;
  delete[] nextBool;
  delete[] lastSegment;
  delete[] currSegment;
}

void Segments::matchRunsToSegments(int x, int height, bool* lastBool, Segment** lastSegment,
  bool* currBool, Segment** currSegment, bool* nextBool, SegmentSettings seg,
  int* madeLines, int* foldedLines, int* shortLines)
{
  loadSegment(currSegment, height);

  int yStart = 0;
  bool inRun = false;
  for (int y = 0; y < height; y++)
  {
    if (!inRun && currBool [y])
    {
      inRun = true;
      yStart = y;
    }

    if ((y + 1 >= height) || !currBool [y + 1])
    {
      if (inRun)
        finishRun(lastBool, nextBool, lastSegment, currSegment, x, yStart, y, height, seg,
        madeLines);

      inRun = false;
    }
  }

  removeUnneededLines(lastSegment, currSegment, height, foldedLines, shortLines, seg);
}

void Segments::removeUnneededLines(Segment** lastSegment, Segment** currSegment, int height,
  int* foldedLines, int* shortLines, SegmentSettings seg)
{
  Segment* segLast = 0;
  for (int yLast = 0; yLast < height; yLast++)
  {
    if (lastSegment [yLast] && (lastSegment [yLast] != segLast))
    {
      segLast = lastSegment [yLast];

      // if the segment is found in the current column then it is still in work so postpone processing
      bool found = false;
      for (int yCur = 0; yCur < height; yCur++)
        if (segLast == currSegment [yCur])
        {
          found = true;
          break;
        }

      if (!found)
      {
        ASSERT_ENGAUGE(segLast != 0);
        if (segLast->length() < (seg.minPoints - 1) * seg.pointSeparation)
        {
          // remove whole segment since it is too short
          *shortLines += segLast->lineCount();
          segments.remove(segLast); // autoDelete is on
        }
        else
          // keep segment, but try to fold lines
          segLast->removeUnneededLines(foldedLines);
      }
    }
  }
}

void Segments::scrollBool(bool* left, bool* right, int height)
{
  for (int y = 0; y < height; y++)
    left [y] = right [y];
}

void Segments::scrollSegment(Segment** left, Segment** right, int height)
{
  for (int y = 0; y < height; y++)
    left [y] = right [y];
}

void Segments::showSegments(bool toggle, SegmentSettings seg, QList<QRect>* updateRectList)
{
  int minPixels = (seg.minPoints - 1) * seg.pointSeparation;

  Segment* segment;
  for (segment = segments.first(); segment; segment = segments.next())
  {
    // no matter the value of toggle, never show segments that are too short
    ASSERT_ENGAUGE(segment != 0);
    if (segment->length() < minPixels)
      segment->showSegment(false, seg, updateRectList);
    else
      segment->showSegment(toggle, seg, updateRectList);
  }
}

void Segments::setSettings(SegmentSettings settings)
{
  Segment* segment;
  for (segment = segments.first(); segment; segment = segments.next())
  {
    ASSERT_ENGAUGE(segment != 0);
    segment->setSettings(settings);
  }
}

void Segments::dump()
{
  for (Segment* segment = segments.first(); segment; segment = segments.next())
  {
    ASSERT_ENGAUGE(segment != 0);
    segment->dump();
  }
}
