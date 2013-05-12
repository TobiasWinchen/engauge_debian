/***************************************************************************
                          scanforaxes.h  -  description
                             -------------------
    begin                : Tue Oct 26 2004
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

#ifndef SCANFORAXES_H
#define SCANFORAXES_H

#include <QVector>

#include "digitdoc.h"
#include "digitdebug.h"

typedef QVector<unsigned int> AxisSlice;

// scan a newly-imported document for the x and y axes, then create
// three axis points
class ScanForAxes
{
  public:

    // ranges for x and y axes in graph coordinates 
    ScanForAxes(double xMinG, double xMaxG, 
      double yMinG, double yMaxG,
      DigitDoc* doc);
    ~ScanForAxes();

    // perform the scanning
    void execute();

  private:

    // scan in either x or y axis
    void scanXAxis(const QImage& img, Q3CanvasLine& scanLine, Q3CanvasLine& bestXLine,
      int& xAxisRow, int& xAxisColMin, int& xAxisColMax);
    void scanYAxis(const QImage& img, Q3CanvasLine& scanLine, Q3CanvasLine& bestYLine,
      int& yAxisCol, int& yAxisRowMin, int& yAxisRowMax);

    // convolutions along axis slice pinpoint start and end of axis line
    void scanAxisForLowEndpoint(AxisSlice axisSlice, int& axisMin);
    void scanAxisForHighEndpoint(AxisSlice axisSlice, int& axisMax);

    // update best lines in active view (there is only one view during import). this
    // is SLOW so only call this when necessary
    void updateView(const QImage& img);

    double m_xMinG;
    double m_xMaxG;
    double m_yMinG;
    double m_yMaxG;
    DigitDoc* m_doc;

};

#endif // SCANFORAXES_H
