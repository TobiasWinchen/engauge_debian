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
#include <QList>

#include <math.h>
#include <stdlib.h>

#include "main.h"
#include "mmsubs.h"
#include "digitdebug.h"
#include "discretize.h"
#include "pointmatch.h"
#include "pointmatchthread.h"

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
  int xStart, int yStart, int x, int y)
{
  if ((x < 0) || (y < 0) || (image.width() <= x) || (image.height() <= y))
    return false; // out of bounds

  Discretize discretize;
  if (!discretize.processedPixelIsOn(image, x, y))
    return false; // pixel is off

  if (dabs (x - xStart) > settings.pointSize / 2)
    return false; // point is too far from start
  if (dabs (y - yStart) > settings.pointSize / 2)
    return false; // point is too far from start

  ASSERT_ENGAUGE(samplePointPixels != 0);
  bool found = (samplePointPixels->size() > 0);
  if (found)
    found = samplePointPixels->containsPoint(QPoint(x, y), Qt::OddEvenFill); 
			//(samplePointPixels->find(QPoint(x, y)) >= 0); // n-squared search happening here
  if (found)
    return true; // already in list

  // add this point
  int newSize = samplePointPixels->size() + 1;
  //bool resized = samplePointPixels->resize(newSize);
  samplePointPixels->resize(newSize);
  //ASSERT_ENGAUGE(resized);
  samplePointPixels->setPoint(newSize - 1, x, y);

  // recurse. diagonal points are included so single-pixel wide polygonal outlines will be traversed,
  // but for a 2x speed increase we only go diagonal if the adjacent nondiagonal pixels are off
  bool right =
    isolateSampleMatchPoint(samplePointPixels, image, settings, xStart, yStart, x + 1, y);
  bool up =
    isolateSampleMatchPoint(samplePointPixels, image, settings, xStart, yStart, x, y + 1);
  bool left =
    isolateSampleMatchPoint(samplePointPixels, image, settings, xStart, yStart, x - 1, y);
  bool down =
    isolateSampleMatchPoint(samplePointPixels, image, settings, xStart, yStart, x, y - 1);
  if (!right && !up)
    isolateSampleMatchPoint(samplePointPixels, image, settings, xStart, yStart, x + 1, y + 1);
  if (!up && !left)
    isolateSampleMatchPoint(samplePointPixels, image, settings, xStart, yStart, x - 1, y + 1);
  if (!left && !down)
    isolateSampleMatchPoint(samplePointPixels, image, settings, xStart, yStart, x - 1, y - 1);
  if (!down && !right)
    isolateSampleMatchPoint(samplePointPixels, image, settings, xStart, yStart, x + 1, y - 1);

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
    _sleep(SLEEP_IN_SECONDS);
#else
    sleep(SLEEP_IN_SECONDS);
#endif
  }

  *pointsCreated = thread.pointsCreated();
}
