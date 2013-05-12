/***************************************************************************
                          scanforaxes.cpp  -  description
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

#include <math.h>
#include <stdlib.h>

#include "scanforaxes.h"
#include "defaultsettings.h"
#include "main.h"
#include "discretize.h"

const int PIXEL_DARK = 1; // need value larger than PIXEL_LIGHT
const int PIXEL_LIGHT = 0; // need value less than PIXEL_DARK

ScanForAxes::ScanForAxes(double xMinG, double xMaxG,
  double yMinG, double yMaxG,
  DigitDoc* doc) :
  m_xMinG(xMinG),
  m_xMaxG(xMaxG),
  m_yMinG(yMinG),
  m_yMaxG(yMaxG),
  m_doc(doc)
{
  DigitDebug::ctor(QString("scanforaxes ") + QString::number((ulong) this, 16));
}

ScanForAxes::~ScanForAxes()
{
  DigitDebug::dtor(QString("scanforaxes ") + QString::number((ulong) this, 16));
}

void ScanForAxes::execute()
{
  const QImage img = m_doc->processedImage();

  if ((img.width() > 2) &&
    (img.height() > 2))
  {
    QColor scanLineColor = PointSetStyles::instance().pointSetColor(
      DefaultSettings::instance().getScanForAxesLineColor());
    int scanLineWidth = DefaultSettings::instance().getScanForAxesLineWidth();
  
    Q3CanvasLine scanLine(m_doc->canvas());
    Q3CanvasLine bestXLine(m_doc->canvas()); // best x scan line so far
    Q3CanvasLine bestYLine(m_doc->canvas()); // best y scan line so far

    scanLine.setBrush(QBrush(scanLineColor));
    bestXLine.setBrush(QBrush(scanLineColor));
    bestYLine.setBrush(QBrush(scanLineColor));
  
    scanLine.setPen(QPen(scanLineColor, scanLineWidth));
    bestXLine.setPen(QPen(scanLineColor, scanLineWidth));
    bestYLine.setPen(QPen(scanLineColor, scanLineWidth));

    scanLine.setZ(ZScanForAxesLines);
    bestXLine.setZ(ZScanForAxesLines);
    bestYLine.setZ(ZScanForAxesLines);
  
    scanLine.show();

    int xAxisRow, xAxisColMin, xAxisColMax;
    int yAxisCol, yAxisRowMin, yAxisRowMax;
    scanXAxis(img, scanLine, bestXLine, xAxisRow, xAxisColMin, xAxisColMax);
    scanYAxis(img, scanLine, bestYLine, yAxisCol, yAxisRowMin, yAxisRowMax);

    double yForXAxis = 0.0;
    double xForYAxis = 0.0;

    m_doc->addAxisPoint(xAxisColMin, xAxisRow, m_xMinG, yForXAxis);
    m_doc->addAxisPoint(xAxisColMax, xAxisRow, m_xMaxG, yForXAxis);
    m_doc->addAxisPoint(yAxisCol, yAxisRowMin, xForYAxis, m_yMaxG); // remember, lowest row is at top so use yAxisRowMin!
  }
}

void ScanForAxes::scanXAxis(const QImage& img, Q3CanvasLine& scanLine, Q3CanvasLine& bestXLine,
  int& xAxisRow, int& xAxisColMin, int& xAxisColMax)
{
  ASSERT_ENGAUGE(img.height() > 2);
  
  // x axis scan starts at bottom and goes up
  bestXLine.setPoints(0, img.height() - 1, img.width() - 1, img.height() - 1);
  bestXLine.show();

  // weight starts off at max value of 1 at bottom, and tapers off to 0 at top
  double weight = 1.0;
  double dWeight = 1.0 / img.height();

  xAxisRow = 0;
  double highestCorrelation = 0.0;
  int row, col;
  bool scanEffect = DefaultSettings::instance().getScanForAxesScanEffect();
  Discretize discretize;
  for (row = img.height() - 1; row >= 0; row--)
  {
    scanLine.setPoints(0, row, img.width() - 1, row);
     
    double correlation = 0.0;
    for (col = 0; col < img.width(); col++)
      if (row + 1 < img.height())
      {
        // perform one-dimensional convolution with finite step function. only two-pixel
        // wide step function convolution kernel is needed since it will pick up axes of any width,
        // plus its small value reduces number of computations
        int pixelHigh = (discretize.processedPixelIsOn(img, col, row + 1) ?
          PIXEL_DARK :
          PIXEL_LIGHT); // should be dark at edge of axis
        int pixelLow = (discretize.processedPixelIsOn(img, col, row) ?
          PIXEL_DARK :
          PIXEL_LIGHT); // should be light at edge of axis

        correlation += pixelHigh - pixelLow;
      }

    if (weight * correlation > highestCorrelation)
    {
      xAxisRow = row + 1; // we want the row with the black values
      highestCorrelation = correlation;
      bestXLine.setPoints(0, xAxisRow, img.width() - 1, xAxisRow);

      updateView(img); // update so user can see newest axis candidate
    }

    if (scanEffect)
      updateView(img); // update so user sees nice scanning effect as scanLine moves

    weight -= dWeight;
  }

  // get endpoints
  AxisSlice rowSlice(img.width());
  for (col = 0; col < img.width(); col++)
    rowSlice [col] = (img.pixel(col, xAxisRow) == QColor(Qt::black).rgb());
  scanAxisForLowEndpoint(rowSlice, xAxisColMin);
  scanAxisForHighEndpoint(rowSlice, xAxisColMax);

  // shrink line down to the actual axis
  bestXLine.setPoints(xAxisColMin, xAxisRow, xAxisColMax, xAxisRow);
  updateView(img);
}

void ScanForAxes::scanYAxis(const QImage& img, Q3CanvasLine& scanLine, Q3CanvasLine& bestYLine,
  int& yAxisCol, int& yAxisRowMin, int& yAxisRowMax)
{
  ASSERT_ENGAUGE(img.width() > 2);

  // y axis scan starts at left and goes right
  bestYLine.setPoints(0, 0, 0, img.height() - 1);
  bestYLine.show();

  // weight starts off at max value of 1 at left, and tapers off to 0 at right
  double weight = 1.0;
  double dWeight = 1.0 / img.width();

  yAxisCol = 0;
  double highestCorrelation = 0.0;
  int col, row;
  bool scanEffect = DefaultSettings::instance().getScanForAxesScanEffect();
  Discretize discretize;
  for (col = 0; col < img.width(); col++)
  {
    scanLine.setPoints(col, 0, col, img.height() - 1);

    double correlation = 0.0;
    for (row = 0; row < img.height(); row++)
      if (col + 1 < img.width())
      {
        // perform one-dimensional convolution with finite step function. only two-pixel
        // wide step function convolution kernel is needed since it will pick up axes of any width,
        // plus its small value reduces number of computations
        int pixelHigh = (discretize.processedPixelIsOn(img, col + 1, row) ?
          PIXEL_DARK :
          PIXEL_LIGHT); // should be dark at edge of axis
        int pixelLow = (discretize.processedPixelIsOn(img, col, row) ?
          PIXEL_DARK :
          PIXEL_LIGHT); // should be light at edge of axis

        correlation += pixelHigh - pixelLow;
      }

    if (weight * correlation > highestCorrelation)
    {
      yAxisCol = col + 1; // we want the column with the black values
      highestCorrelation = correlation;
      bestYLine.setPoints(yAxisCol, 0, yAxisCol, img.height() - 1);

      updateView(img); // update so user can see newest axis candidate
    }

    if (scanEffect)
      updateView(img); // update so user sees nice scanning effect as scanLine moves

    weight -= dWeight;
  }

  // get endpoints
  AxisSlice colSlice(img.height());
  for (row = 0; row < img.height(); row++)
    colSlice [row] = (img.pixel(yAxisCol, row) == QColor(Qt::black).rgb());
  scanAxisForLowEndpoint(colSlice, yAxisRowMin);
  scanAxisForHighEndpoint(colSlice, yAxisRowMax);

  // shrink line down to the actual axis
  bestYLine.setPoints(yAxisCol, yAxisRowMin, yAxisCol, yAxisRowMax);
  updateView(img);
}

void ScanForAxes::scanAxisForLowEndpoint(AxisSlice axisSlice, int& axisMin)
{
  // convolution kernel width
  int kernelWidth = DefaultSettings::instance().getScanForAxesEndpointKernelWidth();
  int halfWidth = kernelWidth / 2;

  // scan from low to high by moving center of convolution kernel
  double bestCorrelation = 0.0;
  axisMin = 0;
  for (int center = 0; center < (int) axisSlice.size(); center++)
  {
    // weight starts off at max value of 1 at left, and tapers off slowly and then more
    // quickly to 0 at right, as w=sqrt(1-col/colmax)
    double radical = 1.0 - (double) center / (double) axisSlice.size();
    double weight;
    if (radical < 0.0)
      weight = 0.0;
    else
      weight = sqrt(radical);
    
    double correlation = 0.0;
    for (int offsetSigned = center - halfWidth; offsetSigned < center + halfWidth; offsetSigned++)
    {    
      if (offsetSigned < center)
      {
        if (offsetSigned >= 0)
          correlation -= axisSlice [offsetSigned]; // microsoft compiler bug disallows unsigned int index
        else
          correlation -= PIXEL_LIGHT;
      }
      else
      {
        if (offsetSigned < (int) axisSlice.size())
          correlation += axisSlice [offsetSigned]; // microsoft compiler bug disallows unsigned int index
        else
          correlation += PIXEL_LIGHT;
      }
    }

    if (weight * correlation > bestCorrelation)
    {
      bestCorrelation = correlation;
      axisMin = center;
    }
  }
}
void ScanForAxes::scanAxisForHighEndpoint(AxisSlice axisSlice, int& axisMax)
{
  // convolution kernel width
  int kernelWidth = DefaultSettings::instance().getScanForAxesEndpointKernelWidth();
  int halfWidth = kernelWidth / 2;
  
  // scan from high to low by moving center of convolution kernel
  double bestCorrelation = 0.0;
  axisMax = 0;
  for (int center = axisSlice.size() - 1; center >= 0; center--)
  {
    // weight starts off at max value of 1 at right, and tapers off slowly and then more
    // quickly to 0 at left, as w=sqrt(col/colmax)
    double radical = (double) center / (double) axisSlice.size();
    double weight;
    if (radical < 0.0)
      weight = 0.0;
    else
      weight = sqrt(radical);
      
    double correlation = 0.0;
    for (int offsetSigned = center - halfWidth; offsetSigned < center + halfWidth; offsetSigned++)
    {
      if (offsetSigned > center)
      {
        if (offsetSigned < (int) axisSlice.size())
          correlation -= axisSlice [offsetSigned]; // microsoft compiler bug disallows unsigned int index
        else
          correlation -= PIXEL_LIGHT;
      }
      else
      {
        if (offsetSigned >= 0)
          correlation += axisSlice [offsetSigned]; // microsoft compiler bug disallows unsigned int index
        else
          correlation += PIXEL_LIGHT;
      }
    }

    if (weight * correlation > bestCorrelation)
    {
      bestCorrelation = correlation;
      axisMax = center;
    }
  }
}

void ScanForAxes::updateView(const QImage& img)
{
  // this is somewhat slow. since this may be an issue on slower computers, it can be disabled
  // in the default settings
  m_doc->slotUpdateViews(0, img.rect());
  m_doc->sendUpdateGeometry();
}
