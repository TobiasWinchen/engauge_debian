/***************************************************************************
                          pointmatch.cpp  -  description
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

#include <qobject.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qapplication.h>
//Added by qt3to4:
#include <QFile>
#include <QList>
#include <QTextStream>

#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef WIN32
#include <Windows.h>
#endif

#include "main.h"
#include "mmsubs.h"
#include "digitdebug.h"
#include "discretize.h"
#include "pointmatch.h"
#include "pointmatchthread.h"

// Directions for isolateSampleMatchPoint. Since that algorithm goes in a counterclockwise pattern these
// are in clockwise order
enum Direction {
  DIR_LEFT,      // First try going right up   (which is to the back right in the reference frame of a left       vector)
  DIR_LEFT_DOWN, // First try going up         (which is to the back right in the reference frame of a left/down  vector)
  DIR_DOWN,      // First try going left up    (which is to the back right in the reference frame of a down       vector)
  DIR_RIGHT_DOWN,// First try going left       (which is to the back right in the reference frame of a right/down vector)
  DIR_RIGHT,     // First try going left down  (which is to the back right in the reference frame of a right      vector)
  DIR_RIGHT_UP,  // First try going down       (which is to the back right in the reference frame of a right/up   vector)
  DIR_UP,        // First try going right down (which is to the back right in the reference frame of a up         vector)
  DIR_LEFT_UP,   // First try going right      (which is to the back right in the reference frame of a left/right vector)
  NUMBER_DIRECTIONS
};

PointMatch::PointMatch()
{
  DigitDebug::ctor(QString("pointmatch ") + QString::number((ulong) this, 16));
}

PointMatch::~PointMatch()
{
  DigitDebug::dtor(QString("pointmatch ") + QString::number((ulong) this, 16));
}

bool PointMatch::isolateSampleMatchPoint(QPolygon* samplePointPixels,
  const QImage &image, PointMatchSettings settings,
  int x, int y)
{
#ifdef LOG_MATCH_POINT
  QFile data ("dbg.log");
  QTextStream *out;
  if (data.open (QFile::WriteOnly | QFile::Truncate)) {
    out = new QTextStream (&data);
  }
  const char *directions [] = {
      "left      ",
      "left/down ",
      "down      ",
      "right/down",
      "right     ",
      "right/up  ",
      "up        ",
      "left/up   "};
#endif

  if ((x < 0) || (y < 0) || (image.width() <= x) || (image.height() <= y))
    return false; // out of bounds

  Discretize discretize;
  if (!discretize.processedPixelIsOn(image, x, y))
    return false; // pixel is off

  int xCenter = x, yCenter = y;
  int xLeft   = xCenter - settings.pointSize / 2;
  int yTop    = yCenter - settings.pointSize / 2;
  int xRight  = xCenter + settings.pointSize / 2;
  int yBottom = yCenter + settings.pointSize / 2;
  int xMin = (xLeft   > 0              ) ? xLeft   : 0;
  int yMin = (yTop    > 0              ) ? yTop    : 0;
  int xMax = (xRight  < image.width () ) ? xRight  : image.width ();
  int yMax = (yBottom < image.height ()) ? yBottom : image.height ();

  // Go to the right until just after edge or the right side, whichever comes first
  do {
    if (discretize.processedPixelIsOn(image, x, y)) {
      ++x;
    } else {
      break;
    }
  } while (x < xMax);
  --x; // Now jump from the outside back to the inside of the right side

  int xStart  = x, yStart = y;

  // Go counterclockwise around (xCenter,yCenter), along the edge of the selected points or the
  // settings.pointSize boundary, until we reach (xStart,yStart) again
  ASSERT_ENGAUGE(samplePointPixels != 0);
  Direction previousDirection = DIR_LEFT; // Start with this to get first try going up right
  while ((samplePointPixels->size () == 0) ||(x != xStart) || (y != yStart)) {

#ifdef LOG_MATCH_POINT
    *out << " direction=" << directions [previousDirection] << " x=" << x << " y=" << y << endl;
#endif

    // Try the directions other than previousDirection until we find the first that has an on-pixel
    for (int directionOffset = 5; directionOffset < 5 + NUMBER_DIRECTIONS; directionOffset++) {

      // Compute next direction from previous direction and offset
      Direction nextDirection = (Direction) ((previousDirection + directionOffset) % NUMBER_DIRECTIONS);

      int xNext = x, yNext = y;
      switch (nextDirection) {
      case DIR_LEFT:
        xNext = x - 1;
        break;
      case DIR_LEFT_DOWN:
        xNext = x - 1;
        yNext = y + 1;
        break;
      case DIR_DOWN:
        yNext = y + 1;
        break;
      case DIR_RIGHT_DOWN:
        xNext = x + 1;
        yNext = y + 1;
        break;
      case DIR_RIGHT:
        xNext = x + 1;
        break;
      case DIR_RIGHT_UP:
        xNext = x + 1;
        yNext = y - 1;
        break;
      case DIR_UP:
        yNext = y - 1;
        break;
      case DIR_LEFT_UP:
        xNext = x - 1;
        yNext = y - 1;
        break;
      case NUMBER_DIRECTIONS:
        break;
      }

      if ((xMin <= xNext) && (xNext < xMax) &&
          (yMin <= yNext) && (yNext < yMax)) {

        if (discretize.processedPixelIsOn(image, xNext, yNext)) {

          // We just found the first on-pixel. Save it and exit the inner loop
          int newSize = samplePointPixels->size() + 1;
          samplePointPixels->resize(newSize);
          samplePointPixels->setPoint(newSize - 1, xNext, yNext);
          x = xNext;
          y = yNext;
          previousDirection = nextDirection;
          break;
        }
      }
    }
  }

#ifdef LOG_MATCH_POINT
  data.close ();
#endif

  return true;
}

void PointMatch::matchSamplePoint(const QImage &imageProcessed,
  PointMatchSettings settings,
  const QPolygon &samplePointPixels, const QPolygon &pointsExisting,
  QList<PointMatchTriplet>* pointsCreated)
{
  QDialog dlg;
  dlg.setFixedSize(410, 70);
  dlg.setModal(true); // modeless would allow app to close with dialog still open!

  QLabel* labelComment = new QLabel(QObject::tr("Processing..."), &dlg);
  CHECK_PTR_ENGAUGE(labelComment);
  labelComment->setGeometry(10, 10, 390, 30);
 
  dlg.show();

  PointMatchThread thread(imageProcessed,
                          settings,
                          samplePointPixels,
                          pointsExisting);

  // start thread
  thread.start();

  const unsigned long SLEEP_IN_SECONDS = 1;
  while (!thread.isFinished())
  {
    qApp->processEvents();

#ifdef WIN32
//    _sleep(SLEEP_IN_SECONDS);
    Sleep(SLEEP_IN_SECONDS);
#else
    sleep(SLEEP_IN_SECONDS);
#endif
  }

  *pointsCreated = thread.pointsCreated();
}
