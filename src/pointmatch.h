/***************************************************************************
                          pointmatch.h  -  description
                             -------------------
    begin                : Tue Jan 6 2003
    copyright            : (C) 2003 by 
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

#ifndef POINTMATCH_H
#define POINTMATCH_H

#include <qimage.h>
#include <qcolor.h>
#include <qpoint.h>
#include <q3intdict.h>
#include <q3ptrlist.h>
#include <qthread.h>
//Added by qt3to4:
#include <QList>
#include <Q3PtrCollection>
#include <QPolygon>

#include "digitdef.h"
#include "digitdebug.h"

#include "fftw3.h"

class Match;

// pointer list that also sorts Match objects by xTheta coordinate
template <class T>
class MatchPtrList: public Q3PtrList<T>
{
  protected:

    virtual int compareItems(Q3PtrCollection::Item item1, Q3PtrCollection::Item item2)
    {
      PointMatchTriplet* p1 = (PointMatchTriplet*) item1;
      PointMatchTriplet* p2 = (PointMatchTriplet*) item2;
      if (p1->correlation > p2->correlation)
        return -1;
      else if (p1->correlation < p2->correlation)
        return 1;
      else
        return 0;
    }
};

// the point lists are pointer lists (versus value lists) so that, for brief
// intervals, there can be multiple pointsets that share the same points. Further
// information can be found in the PointSets comments
typedef MatchPtrList<PointMatchTriplet> PointMatchList;

// this class searches through the original or processed image and 
// isolates candidate match points. the matches are sorted by
// quality of match
class PointMatch
{
  public:
  
    PointMatch();
    ~PointMatch();

    // recursively isolate the "on" pixels around (x,y). return true if pixel at (x,y) was on
    bool isolateSampleMatchPoint(QPolygon * samplePointPixels,
      const QImage &image, PointMatchSettings settings,
      int xStart, int yStart, int x, int y);

    // create document points that match the sample point under the cursor
    static void matchSamplePoint(const QImage &imageProcessed,
                                 PointMatchSettings settings, 
                                 const QPolygon &samplePointPixels,
                                 const QPolygon &pointsExisting, 
                                 QList<PointMatchTriplet>* pointsCreated);

};

#endif // POINTMATCH_H
