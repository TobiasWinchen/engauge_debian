/***************************************************************************
                          pointsetstyles.cpp  -  description
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

#include <qnamespace.h>
#include <qcolor.h>
//Added by qt3to4:
#include <Q3PointArray>

#include "pointsetstyles.h"
#include "digitdebug.h"
#include "main.h"

PointSetStyles* PointSetStyles::pointSetStyles = 0;

const QColor COLOR_GOLD(218, 165, 32);

PointSetStyles::PointSetStyles()
{
  DigitDebug::ctor(QString("pointsetstyles ") + QString::number((ulong) this, 16));
}

PointSetStyles::~PointSetStyles()
{
  DigitDebug::dtor(QString("pointsetstyles ") + QString::number((ulong) this, 16));
}

QString PointSetStyles::pointShapeName(PointShape shape)
{
  switch (shape)
  {
  case Cross:
    return QString("Cross");
  case Diamond:
    return QString("Diamond");
  case Square:
    return QString("Square");
  case Triangle:
    return QString("Triangle");
  case X:
    return QString("X");
  case MaxPointShape:
  default:
    return QString("");
  }
}

QString PointSetStyles::pointSizeName(PointSize size)
{
  switch (size)
  {
  case PointSize1:
    return QString("1");
  case PointSize2:
    return QString("2");
  case PointSize3:
    return QString("3");
  case PointSize4:
    return QString("4");
  case PointSize5:
    return QString("5");
  case PointSize6:
    return QString("6");
  case PointSize7:
    return QString("7");
  case PointSize8:
    return QString("8");
  case MaxPointSize:
  default:
    return QString("");
  }
}

QString PointSetStyles::pointLineSizeName(PointLineSize size)
{
  switch (size)
  {
  case PointLineSize1:
    return QString("1");
  case PointLineSize2:
    return QString("2");
  case PointLineSize3:
    return QString("3");
  case PointLineSize4:
    return QString("4");
  case PointLineSize5:
    return QString("5");
  case PointLineSize6:
    return QString("6");
  case PointLineSize7:
    return QString("7");
  case PointLineSize8:
    return QString("8");
  case MaxPointLineSize:
  default:
    return QString("");
  }
}

QString PointSetStyles::lineSizeName(LineSize size)
{
  switch (size)
  {
  case LineSize1:
    return QString("1");
  case LineSize2:
    return QString("2");
  case LineSize3:
    return QString("3");
  case LineSize4:
    return QString("4");
  case LineSize5:
    return QString("5");
  case LineSize6:
    return QString("6");
  case LineSize7:
    return QString("7");
  case LineSize8:
    return QString("8");
  case MaxLineSize:
  default:
    return QString("");
  }
}

QString PointSetStyles::colorName(Color color)
{
  switch (color)
  {
  case DigitBlack:
    return QString("Black");
  case DigitBlue:
    return QString("Blue");
  case DigitCyan:
    return QString("Cyan");
  case DigitGold:
    return QString("Gold");
  case DigitGreen:
    return QString("Green");
  case DigitMagenta:
    return QString("Magenta");
  case DigitRed:
    return QString("Red");
  case DigitTransparent:
    return QString("Transparent");
  case DigitYellow:
    return QString("Yellow");
  case DigitMaxColor:
  default:
    return QString("");
  }
}

QString PointSetStyles::lineConnectAsName(LineConnectAs method)
{
  switch (method)
  {
  case SingleValuedFunction:
    return QString("Single Valued Function");
  case Contour:
    return QString("Contour");
  case MaxConnectAsMethod:
  default:
    return QString("");
  }
}

PointShape PointSetStyles::pointShape(QString text)
{
  for (int shape = 0; shape < (int) MaxPointShape; shape++)
    if (text == pointShapeName((PointShape) shape))
      return (PointShape) shape;

  return (PointShape) 0;
}

PointSize PointSetStyles::pointSize(QString text)
{
  for (int size = 0; size < (int) MaxPointSize; size++)
    if (text == pointSizeName((PointSize) size))
      return (PointSize) size;

  return (PointSize) 0;
}

PointLineSize PointSetStyles::pointLineSize(QString text)
{
  for (int size = 0; size < (int) MaxPointLineSize; size++)
    if (text == pointLineSizeName((PointLineSize) size))
      return (PointLineSize) size;

  return (PointLineSize) 0;
}

LineSize PointSetStyles::lineSize(QString text)
{
  for (int size = 0; size < (int) MaxLineSize; size++)
    if (text == lineSizeName((LineSize) size))
      return (LineSize) size;

  return (LineSize) 0;
}

Color PointSetStyles::color(QString text)
{
  for (int color = 0; color < (int) DigitMaxColor; color++)
    if (text == colorName((Color) color))
      return (Color) color;

  return (Color) 0;
}

LineConnectAs PointSetStyles::lineConnectAs(QString text)
{
  for (int method = 0; method < (int) MaxConnectAsMethod; method++)
    if (text == lineConnectAsName((LineConnectAs) method))
      return (LineConnectAs) method;

  return (LineConnectAs) 0;
}

QStringList PointSetStyles::pointShapeNames(void)
{
  QStringList list;
  for (int shape = 0; shape < (int) MaxPointShape; shape++)
    list.append(pointShapeName((PointShape) shape));

  return list;
}

QStringList PointSetStyles::pointSizeNames(void)
{
  QStringList list;
  for (int size = 0; size < (int) MaxPointSize; size++)
    list.append(pointSizeName((PointSize) size));
  
  return list;
}

QStringList PointSetStyles::pointLineSizeNames(void)
{
  QStringList list;
  for (int size = 0; size < (int) MaxPointLineSize; size++)
    list.append(pointLineSizeName((PointLineSize) size));

  return list;
}

QStringList PointSetStyles::lineSizeNames(void)
{
  QStringList list;
  for (int size = 0; size < (int) MaxLineSize; size++)
    list.append(lineSizeName((LineSize) size));

  return list;
}

QStringList PointSetStyles::colorNames(void)
{
  QStringList list;
  for (int color = 0; color < (int) DigitMaxColor; color++)
    list.append(colorName((Color) color));
  
  return list;
}

QStringList PointSetStyles::lineConnectAsNames(void)
{
  QStringList list;
  for (int method = 0; method < (int) MaxConnectAsMethod; method++)
    list.append(lineConnectAsName((LineConnectAs) method));

  return list;
}

Q3PointArray PointSetStyles::areaPoints(int xScreen, int yScreen, PointSetStyle pointSetStyle) const
{
  Q3PointArray arr;
  int fudgeFactor = 1; // add an extra pixel around the perimeter to allow for roundoff error

  // half point size
  int hPS = pointSizeToHalfwidth(pointSetStyle.pointSize) + pointSetStyle.pointLineSize + fudgeFactor;
    
  switch (pointSetStyle.pointShape)
  {
  case Cross:
  case Diamond:
    arr.resize(5);
    arr.setPoint(0, QPoint(-hPS + xScreen,    0 + yScreen));
    arr.setPoint(1, QPoint(   0 + xScreen, -hPS + yScreen));
    arr.setPoint(2, QPoint( hPS + xScreen,    0 + yScreen));
    arr.setPoint(3, QPoint(   0 + xScreen,  hPS + yScreen));
    arr.setPoint(4, QPoint(-hPS + xScreen,    0 + yScreen));
    break;
  case Square:
  case X:
  default:
    arr.resize(5);
    arr.setPoint(0, QPoint(-hPS + xScreen, -hPS + yScreen));
    arr.setPoint(1, QPoint( hPS + xScreen, -hPS + yScreen));
    arr.setPoint(2, QPoint( hPS + xScreen,  hPS + yScreen));
    arr.setPoint(3, QPoint(-hPS + xScreen,  hPS + yScreen));
    arr.setPoint(4, QPoint(-hPS + xScreen, -hPS + yScreen));
    break;
  case Triangle:
    arr.resize(4);
    arr.setPoint(0, QPoint(-hPS + xScreen,  hPS + yScreen));
    arr.setPoint(1, QPoint( hPS + xScreen,  hPS + yScreen));
    arr.setPoint(2, QPoint(   0 + xScreen, -hPS + yScreen));
    arr.setPoint(3, QPoint(-hPS + xScreen,  hPS + yScreen));
    break;
  }

  return arr;
}

void PointSetStyles::drawShape(QPainter &p, int xScreen, int yScreen, PointSetStyle pointSetStyle)
{
  p.setPen(pointSetPen(pointSetStyle.pointLineColor, pointSetStyle.pointLineSize));
  p.setBrush(pointSetBrush(pointSetStyle.pointInColor));

  Q3PointArray arr;  
  int hPS = pointSizeToHalfwidth(pointSetStyle.pointSize); // half point size, excluding point line size
  
  switch (pointSetStyle.pointShape)
  {
  case Cross:
    arr.resize(9);
    arr.setPoint(0, QPoint(-hPS + xScreen,    0 + yScreen));
    arr.setPoint(1, QPoint(   0 + xScreen,    0 + yScreen));
    arr.setPoint(2, QPoint(   0 + xScreen, -hPS + yScreen));
    arr.setPoint(3, QPoint(   0 + xScreen,    0 + yScreen));
    arr.setPoint(4, QPoint( hPS + xScreen,    0 + yScreen));
    arr.setPoint(5, QPoint(   0 + xScreen,    0 + yScreen));
    arr.setPoint(6, QPoint(   0 + xScreen,  hPS + yScreen));
    arr.setPoint(7, QPoint(   0 + xScreen,    0 + yScreen));
    arr.setPoint(8, QPoint(-hPS + xScreen,    0 + yScreen));
    break;
  case Diamond:
    arr.resize(5);
    arr.setPoint(0, QPoint(-hPS + xScreen,    0 + yScreen));
    arr.setPoint(1, QPoint(   0 + xScreen, -hPS + yScreen));
    arr.setPoint(2, QPoint( hPS + xScreen,    0 + yScreen));
    arr.setPoint(3, QPoint(   0 + xScreen,  hPS + yScreen));
    arr.setPoint(4, QPoint(-hPS + xScreen,    0 + yScreen));
    break;
  case Square:
    arr.resize(5);
    arr.setPoint(0, QPoint(-hPS + xScreen, -hPS + yScreen));
    arr.setPoint(1, QPoint( hPS + xScreen, -hPS + yScreen));
    arr.setPoint(2, QPoint( hPS + xScreen,  hPS + yScreen));
    arr.setPoint(3, QPoint(-hPS + xScreen,  hPS + yScreen));
    arr.setPoint(4, QPoint(-hPS + xScreen, -hPS + yScreen));
    break;
  case Triangle:
    arr.resize(4);
    arr.setPoint(0, QPoint(-hPS + xScreen,  hPS + yScreen));
    arr.setPoint(1, QPoint( hPS + xScreen,  hPS + yScreen));
    arr.setPoint(2, QPoint(   0 + xScreen, -hPS + yScreen));
    arr.setPoint(3, QPoint(-hPS + xScreen,  hPS + yScreen));
    break;
  case X:
  default:
    arr.resize(9);
    arr.setPoint(0, QPoint(-hPS + xScreen, -hPS + yScreen));
    arr.setPoint(1, QPoint(   0 + xScreen,    0 + yScreen));
    arr.setPoint(2, QPoint(-hPS + xScreen,  hPS + yScreen));
    arr.setPoint(3, QPoint(   0 + xScreen,    0 + yScreen));
    arr.setPoint(4, QPoint( hPS + xScreen,  hPS + yScreen));
    arr.setPoint(5, QPoint(   0 + xScreen,    0 + yScreen));
    arr.setPoint(6, QPoint( hPS + xScreen, -hPS + yScreen));
    arr.setPoint(7, QPoint(   0 + xScreen,    0 + yScreen));
    arr.setPoint(8, QPoint(-hPS + xScreen, -hPS + yScreen));
    break;
  }

  p.drawPolygon(arr);
}

QPen PointSetStyles::pointSetPen(Color color, int size, Qt::PenStyle style)
{
  switch (color)
  {
  case DigitBlack:
  case DigitMaxColor:
  default:
    return QPen(Qt::black, size, style);
  case DigitBlue:
    return QPen(Qt::blue, size, style);
  case DigitCyan:
    return QPen(Qt::cyan, size, style);
  case DigitGold:
    return QPen(COLOR_GOLD, size, style);
  case DigitGreen:
    return QPen(Qt::green, size, style);
  case DigitMagenta:
    return QPen(Qt::magenta, size, style);
  case DigitRed:
    return QPen(Qt::red, size, style);
  case DigitTransparent:
    return QPen(Qt::color0, size, style);
  case DigitYellow:
    return QPen(Qt::yellow, size, style);
  }
}

QBrush PointSetStyles::pointSetBrush(Color color)
{
  switch (color)
  {
  case DigitBlack:
  case DigitMaxColor:
  default:
    return QBrush(Qt::black);
  case DigitBlue:
    return QBrush(Qt::blue);
  case DigitCyan:
    return QBrush(Qt::cyan);
  case DigitGold:
    return QBrush(COLOR_GOLD);
  case DigitGreen:
    return QBrush(Qt::green);
  case DigitMagenta:
    return QBrush(Qt::magenta);
  case DigitRed:
    return QBrush(Qt::red);
  case DigitTransparent:
    return QBrush(Qt::color0);
  case DigitYellow:
    return QBrush(Qt::yellow);
  }
}

QColor PointSetStyles::pointSetColor(Color color)
{
  switch (color)
  {
  case DigitBlack:
  case DigitMaxColor:
  default:
    return Qt::black;
  case DigitBlue:
    return Qt::blue;
  case DigitCyan:
    return Qt::cyan;
  case DigitGold:
    return COLOR_GOLD;
  case DigitGreen:
    return Qt::green;
  case DigitMagenta:
    return Qt::magenta;
  case DigitRed:
    return Qt::red;
  case DigitTransparent:
    return Qt::color0;
  case DigitYellow:
    return Qt::yellow;
  }
}
