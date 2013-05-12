/***************************************************************************
                          pointsetstyles.h  -  description
                             -------------------
    begin                : Wed Nov 13 2002
    copyright            : (C) 2002 by 
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

#ifndef POINTSETSTYLES_H
#define POINTSETSTYLES_H

#include <qstringlist.h>
#include <qpainter.h>
#include <qpen.h>
#include <qbrush.h>
//Added by qt3to4:
#include <Q3PointArray>

#include "digitdef.h"

// this singleton class draws points and lines for both axes and curve pointsets
class PointSetStyles
{
  public: 

    static void create(void)
    {
      if (!pointSetStyles)
        pointSetStyles = new PointSetStyles;
    }
    static void destroy(void)
    {
      if (pointSetStyles)
      {
        delete pointSetStyles;
        pointSetStyles = 0;
      }
    }
    static PointSetStyles& instance(void)
    {
      return *pointSetStyles;
    }

    QString pointShapeName(PointShape shape);
    QString pointSizeName(PointSize size);
    QString pointLineSizeName(PointLineSize size);
    QString lineSizeName(LineSize size);
    QString colorName(Color color);
    QString lineConnectAsName(LineConnectAs method);

    PointShape pointShape(QString text);
    PointSize pointSize(QString text);
    PointLineSize pointLineSize(QString text);
    LineSize lineSize(QString text);
    Color color(QString text);
    LineConnectAs lineConnectAs(QString text);
        
    QStringList pointShapeNames(void);
    QStringList pointSizeNames(void);
    QStringList pointLineSizeNames(void);
    QStringList lineSizeNames(void);
    QStringList colorNames(void);
    QStringList lineConnectAsNames(void);

    // drawing methods for canvas polygon objects
    Q3PointArray areaPoints(int xScreen, int yScreen, PointSetStyle pointSetStyle) const;
    void drawShape(QPainter &p, int xScreen, int yScreen, PointSetStyle pointSetStyle);

    // set pen and brush. this works on both the point line and the line
    QPen pointSetPen(Color color, int size, Qt::PenStyle style = Qt::SolidLine);
    QBrush pointSetBrush(Color color);
    QColor pointSetColor(Color color);

  protected:
  
    PointSetStyles();
    ~PointSetStyles();

  private:

    // conversion of pointSize from displayed index (1-8) to halfwidth in pixels
    int pointSizeToHalfwidth(PointSize size) const { return 2 * ((int) size + 1); }

    // this singleton object
    static PointSetStyles* pointSetStyles;
    
};

#endif
