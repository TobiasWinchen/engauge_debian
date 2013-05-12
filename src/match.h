/***************************************************************************
                          match.h  -  description
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

#ifndef MATCH_H
#define MATCH_H

#include <q3canvas.h>
#include <qrect.h>
//Added by qt3to4:
#include <Q3PointArray>

#include "digitdef.h"
#include "pointsetstyles.h"

// class used to store match points. This class essentially duplicates the
// basic idea of the more important Point class, with irrelevant functionality
// removed and point match functionality added
class Match : public Q3CanvasPolygonalItem
{
  public:

    Match(int xScreen, int yScreen,
      Color colorAccepted, Color colorRejected,
      PointSetStyle style, Q3Canvas* canvas);
    ~Match();

    enum MatchState
    {
      StateAccepted,
      StateRejected,
      StateUnmarked
    };

    // change state
    MatchState state(void) { return m_state; }
    void setState(MatchState state);

    // change point style
    void setPointSetStyle(PointSetStyle style) { m_pointSetStyle = style; }

    // location
    int x(void) { return m_xScreen; }
    int y(void) { return m_yScreen; }
    
    // debug
    void dumpMatch(void);

  protected:
  
    // QCanvasPolygonalItem function that draws the polygonal item using the pen and brush
    void drawShape(QPainter& p);
    
  private:

    // QCanvasPolygonalItem function that returns the bounding points
    virtual Q3PointArray areaPoints(void) const;
     
    // screen coordinates are in pixels. They are only modified by a click-and-drag operation
    int m_xScreen;
    int m_yScreen;
    
    // colors in accepted and rejected states. these override the style
    Color m_colorAccepted;
    Color m_colorRejected;

    // state of this match point
    MatchState m_state;

    // point style
    PointSetStyle m_pointSetStyle;
};

#endif // MATCH_H
