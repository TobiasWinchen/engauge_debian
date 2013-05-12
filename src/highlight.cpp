/***************************************************************************
                          highlight.cpp  -  description
                             -------------------
    begin                : Tue Dec 19 2006
    copyright            : (C) 2006 by Mark Mitchell
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

#include "defaultsettings.h"
#include "digitdebug.h"
#include "highlight.h"
#include "pointsetstyles.h"

Highlight::Highlight(Color color, Q3Canvas* canvas) :
  Q3CanvasRectangle(0, 0, 
                   DefaultSettings::instance().getPointMatchHighlightDiameter(),
                   DefaultSettings::instance().getPointMatchHighlightDiameter(),
                   canvas)
{
  DigitDebug::ctor(QString("highlight ") + QString::number((ulong) this, 16));

  uint lineSize = DefaultSettings::instance().getPointMatchHighlightLineSize();
  setPen(PointSetStyles::instance().pointSetPen(color, lineSize));
  setVisible(true);  
}

Highlight::~Highlight()
{
  DigitDebug::dtor(QString("highlight ") + QString::number((ulong) this, 16));
}

void Highlight::moveCenter(int x, int y)
{
  int highlightDiameter = DefaultSettings::instance().getPointMatchHighlightDiameter();
  
  move(x - highlightDiameter / 2,
       y - highlightDiameter / 2);
}
