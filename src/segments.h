/***************************************************************************
                          segments.h  -  description
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

#ifndef SEGMENTS_H
#define SEGMENTS_H

#include <q3canvas.h>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <QList>

#include "digitdef.h"

class Segment;
class Discretize;

// this class is a container for all of the segments. a segment is a set
// of linked lines that run along a curve in the original image. the
// main complication is that curves in the original image cross each other
// and other things like grid lines. rather than invent artificial
// intelligence (the next software project), we rely on the user to link
// multiple segments together to get points along the entire curve length
class Segments
{
  public: 

    Segments(Q3Canvas* canvas);
    ~Segments();

    // return segment fill points for all segments, for previewing
    QList<QPoint> fillPoints(SegmentSettings seg);

    // segments are built when the original image is loaded. they start out hidden
    // and remain so until showSegments is called  
    void makeSegments(QImage &imageProcessed, SegmentSettings seg);

    // turn segments on/off
    void showSegments(bool toggle, SegmentSettings seg, QList<QRect>* updateRectList);

    // get and set methods for segment settings
    void setSettings(SegmentSettings settings);

    // debug these segments
    void dump(void);
        
  private:

    Q3Canvas* canvas;
    
    // return the number of runs adjacent to the pixels from yStart to yStop (inclusive)
    int adjacentRuns(bool* columnBool, int yStart, int yStop, int height);

    // find the single segment pointer among the adjacent pixels from yStart-1 to yStop+1
    Segment* adjacentSegment(Segment** lastSegment, int yStart, int yStop, int height);

    // return the number of segments adjacent to the pixels from yStart to yStop (inclusive)
    int adjacentSegments(Segment** lastSegment, int yStart, int yStop, int height);

    // process a run of pixels. if there are fewer than two adjacent pixel runs on
    // either side, this run will be added to an existing segment, or the start of
    // a new segment
    void finishRun(bool* lastBool, bool* nextBool,
      Segment** lastSegment, Segment** currSegment,
      int x, int yStart, int yStop, int height, SegmentSettings seg, int* madeLines);

    // initialize one column of boolean flags using the pixels of the specified column
    void loadBool(const Discretize* discretize, bool* columnBool, QImage* image, int x);
      
    // initialize one column of segment pointers
    void loadSegment(Segment** columnSegment, int height);

    // identify the runs in a column, and connect them to segments
    void matchRunsToSegments(int x, int height, bool* lastBool, Segment** lastSegment,
      bool* currBool, Segment** currSegment, bool* nextBool, SegmentSettings seg,
      int* madeLines, int* foldedLines, int* shortLines);

    // remove unneeded lines belonging to segments that just finished in the previous column.
    // the results of this function are displayed in the debug spew of makeSegments
    void removeUnneededLines(Segment** lastSegment, Segment** currSegment, int height,
      int* foldedLines, int* shortLines, SegmentSettings seg);
    
    // scroll the boolean flags of the right column into the left column
    void scrollBool(bool* left, bool* right, int height);

    // scroll the segment pointers of the right column into the left column
    void scrollSegment(Segment** left, Segment** right, int height);

    // segments produced by scanning the image
    Q3PtrList<Segment> segments;
};

#endif // SEGMENTS_H
