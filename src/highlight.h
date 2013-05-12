/***************************************************************************
                          highlight.h  -  description
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

#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include <q3canvas.h>

#include "digitdef.h"

class Q3Canvas;

// this class implements the highlight box used during point matching. for the
// most part the standard QCanvasRectangle methods are used, with geometry
// configuration performed in this derived class
class Highlight : public Q3CanvasRectangle
{
  public: 

    Highlight(Color color, Q3Canvas* canvas);
    ~Highlight();                                                 

    // move center of box to specified location
    void moveCenter(int x, int y);
};

#endif // HIGHLIGHT_H
