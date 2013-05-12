/***************************************************************************
                          discretize.h  -  description
                             -------------------
    begin                : Sun Dec 22 2003
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

#ifndef DISCRETIZE_H
#define DISCRETIZE_H

#include <qimage.h>
#include <qcolor.h>
#include <qpoint.h>
#include <q3intdict.h>
//Added by qt3to4:
#include <QList>

#include "digitdef.h"
#include "digitdebug.h"

// this class discretizes an image by setting every pixel either to
// off or on
class Discretize
{
  public:
  
    Discretize();
    ~Discretize();

    // attribute maximum depends on which color attribute is chosen
    int colorAttributeMax(DiscretizeMethod method) const;

    // isolate background color, which is assumed to be the most popular color
    QRgb backgroundColor(const QImage* image);
    
    // return true if two colors are very close
    bool colorCompare(QRgb color1, QRgb color2);

    // return value of specified discretize color attribute at specified pixel. the
    // background color components are only used in foreground/background mode
    int discretizeValueNotForeground(const QImage* image, int x, int y, DiscretizeMethod method) const;
    int discretizeValueForeground(const QImage* image, int x, int y, DiscretizeMethod method,
      int rBg, int gBg, int bBg) const;

    // convert image to on-off using current settings. the background color is only
    // used in foreground/background mode. this function takes a fifth of a second to complete,
    // so it is reentrant with new calls aborting previous calls (since discretizedlg calls
    // this once per mouse move event so the program bogs down)
    void discretize(QImage* image, DiscretizeSettings settings, QRgb backgroundColor = 0);

    // return true if pixel satisfies the current attribute constraints
    bool pixelIsOn(int value, GridRemovalSettings settings) const;
    bool pixelIsOn(int value, DiscretizeSettings settings) const;
    bool processedPixelIsOn(const QImage &image, int x, int y) const;

  private:

    // store histogram of selected pixel colors as a QValueList
    struct ColorEntry
    {
      QColor color;
      int count;
    };
    typedef QList<ColorEntry> ColorList;
    
    bool pixelIsOn(int value, int low, int high) const;
};

#endif // DISCRETIZE_H
