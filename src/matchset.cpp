/***************************************************************************
                          matchset.cpp  -  description
                             -------------------
    begin                : Mon Jan 12 2004
    copyright            : (C) 2004 by 
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

#include <q3canvas.h>
#include <qregexp.h>
//Added by qt3to4:
#include <QList>

#include "matchset.h"
#include "digitdef.h"
#include "digitdebug.h"
#include "highlight.h"
#include "main.h"
#include "match.h"
#include "defaultsettings.h"

MatchSet::MatchSet() :
  m_canvas(0),
  m_matches(0),
  m_highlightCandidate(0)
{
  DigitDebug::ctor(QString("matchset ") + QString::number((ulong) this, 16));
  
  m_matchList.setAutoDelete(true);
}

MatchSet::~MatchSet()
{
  DigitDebug::dtor(QString("matchset ") + QString::number((ulong) this, 16));
  
  QList<QRect> updateRectList;
  clear(&updateRectList);
  
  if (m_highlightCandidate)
    delete m_highlightCandidate;
}

void MatchSet::acceptedPoints(QList<QPoint>* acceptedPoints)
{
  Match* m;
  for (m = m_matchList.first(); m != 0; m = m_matchList.next())
  {
    if (m->state() == Match::StateAccepted)
    {
      QPoint p(m->x(), m->y());

      ASSERT_ENGAUGE(acceptedPoints != 0);
      acceptedPoints->append(p);
    }
  }
}

void MatchSet::addCreatedPointsToCanvas(Q3Canvas* canvas, PointSetStyle style,               
  PointMatchSettings settings, const QList<PointMatchTriplet>* pointsCreated,
  QList<QRect>* updateRectList)
{
  // make sure we have no stale points from previous invocations
  if (m_matchList.count() > 0)
    clear(updateRectList);

  m_canvas = canvas;
  m_settings = settings;
  
  QList<PointMatchTriplet>::const_iterator itr;
  for (itr = pointsCreated->begin(); itr != pointsCreated->end(); ++itr)
  {
    PointMatchTriplet t = (*itr);

    Match* m = new Match(t.x, t.y,
      settings.acceptedColor, settings.rejectedColor,
      style, canvas);
    CHECK_PTR_ENGAUGE(m);

    m_matchList.append(m);
  }

  // create the highlight
  m_highlightCandidate = new Highlight(m_settings.candidateColor, canvas);
  CHECK_PTR_ENGAUGE(m_highlightCandidate);

  moveHighlights(updateRectList);
}

void MatchSet::clear(QList<QRect>* updateRectList)
{
  if (m_highlightCandidate)
  {
    updateRectList->append(m_highlightCandidate->boundingRect());
    delete m_highlightCandidate;
    m_highlightCandidate = 0;
  }

  while (m_matchList.count() > 0)
  {
    ASSERT_ENGAUGE(updateRectList != 0);
    updateRectList->append(m_matchList.last()->boundingRect());
    
    m_matchList.removeLast(); // autoDelete is on
  }
}

Q3CanvasRectangle* MatchSet::makeHighlight(Color color)
{
  ASSERT_ENGAUGE(m_canvas != 0);
  Q3CanvasRectangle* r = new Q3CanvasRectangle(0, 0,
    DefaultSettings::instance().getPointMatchHighlightDiameter(),
    DefaultSettings::instance().getPointMatchHighlightDiameter(),
    m_canvas);
  CHECK_PTR_ENGAUGE(r);
  r->setPen(PointSetStyles::instance().pointSetPen(color,
    DefaultSettings::instance().getPointMatchHighlightLineSize()));
  r->setVisible(true);

  return r;
}

bool MatchSet::matchPointAccept(QList<QRect>* updateRectList)
{
  Match* m;
  for (m = m_matchList.first(); m != 0; m = m_matchList.next())
  {
    if (m->state() == Match::StateUnmarked)
    {
      m->setState(Match::StateAccepted);

      ASSERT_ENGAUGE(updateRectList != 0);
      updateRectList->append(m->boundingRect());

      moveHighlights(updateRectList);
      
      return true;
    }
  }

  return false;
}

bool MatchSet::matchPointReject(QList<QRect>* updateRectList)
{
  Match* m;
  for (m = m_matchList.first(); m != 0; m = m_matchList.next())
  {
    if (m->state() == Match::StateUnmarked)
    {
      m->setState(Match::StateRejected);

      ASSERT_ENGAUGE(updateRectList != 0);
      updateRectList->append(m->boundingRect());

      moveHighlights(updateRectList);
      
      return true;
    }
  }

  return false;
}

bool MatchSet::matchPointRemove(QList<QRect>* updateRectList)
{
  Match* mLastNotRejected = 0;
  Match* m;
  for (m = m_matchList.first(); m != 0; m = m_matchList.next())
  {
    if (m->state() == Match::StateUnmarked)
    {
      if (mLastNotRejected != 0)
      {
        mLastNotRejected->setState(Match::StateUnmarked);

        ASSERT_ENGAUGE(updateRectList != 0);
        updateRectList->append(m->boundingRect());

        moveHighlights(updateRectList);
        
        return true;
      }
      else
        return false;
    }

    mLastNotRejected = m;
  }

  // all points are accepted, so reject the last point
  m = m_matchList.last();
  if (m != 0)
  {
    m->setState(Match::StateUnmarked);
    ASSERT_ENGAUGE(updateRectList != 0);
    updateRectList->append(m->boundingRect());
  }

  moveHighlights(updateRectList);
  
  return true;
}

void MatchSet::moveHighlights(QList<QRect>* updateRectList)
{
  Match* m;
  for (m = m_matchList.first(); m != 0; m = m_matchList.next())
  {
    if (m->state() == Match::StateUnmarked)
    {
      if (m_highlightCandidate == 0)
      {
        ASSERT_ENGAUGE(m_canvas != 0);
        m_highlightCandidate = new Highlight(m_settings.candidateColor, m_canvas);
        CHECK_PTR_ENGAUGE(m_highlightCandidate);
      }
      else
        updateRectList->append(m_highlightCandidate->boundingRect());
      m_highlightCandidate->moveCenter(m->x(),
                                       m->y());

      if (m_highlightCandidate != 0)
        updateRectList->append(m_highlightCandidate->boundingRect());     
      return;
    }
  }

  if (m_highlightCandidate != 0)
  {
    updateRectList->append(m_highlightCandidate->boundingRect());
    delete m_highlightCandidate;
    m_highlightCandidate = 0;
  }
}

void MatchSet::setPointSetStyle(PointSetStyle style)
{
  for (Match* m = m_matchList.first(); m != 0; m = m_matchList.next())
    m->setPointSetStyle(style);
}
