/***************************************************************************
                          match.cpp  -  description
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

#include <qpainter.h>
#include <q3pointarray.h>

#include "match.h"
#include "digitdebug.h"
#include "main.h"

Match::Match(int xScreen, int yScreen,
  Color colorAccepted, Color colorRejected, PointSetStyle style,
  Q3Canvas* canvas) :
  Q3CanvasPolygonalItem(canvas),
  m_xScreen(xScreen),
  m_yScreen(yScreen),
  m_colorAccepted(colorAccepted),
  m_colorRejected(colorRejected),
  m_state(StateUnmarked),
  m_pointSetStyle(style)
{
  DigitDebug::ctor(QString("match ") + QString::number((ulong) this, 16));
}

Match::~Match()
{
  DigitDebug::dtor(QString("match ") + QString::number((ulong) this, 16));
  
  // all subclasses of QCanvasPolygonalItem must call hide in destructor
  hide();
}

Q3PointArray Match::areaPoints() const
{
  return PointSetStyles::instance().areaPoints(m_xScreen, m_yScreen, m_pointSetStyle);
}

void Match::setState(MatchState state)
{
  m_state = state;

  switch (m_state)
  {
  case StateAccepted:
    show();
    m_pointSetStyle.pointLineColor = m_colorAccepted;
    m_pointSetStyle.pointInColor = m_colorAccepted;
    break;
  case StateRejected:
    show();
    m_pointSetStyle.pointLineColor = m_colorRejected;
    m_pointSetStyle.pointInColor = m_colorRejected;
    break;
  case StateUnmarked:
    hide();
    break;
  }
}

void Match::drawShape(QPainter &p)
{
  PointSetStyles::instance().drawShape(p, m_xScreen, m_yScreen, m_pointSetStyle);
}

void Match::dumpMatch()
{
  qDebug("match this %lx xScreen %d yScreen %d ",
    (unsigned long) this, m_xScreen, m_yScreen);
}
