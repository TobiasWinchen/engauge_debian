/***************************************************************************
                          gridremoval.cpp  -  description
                             -------------------
    begin                : Fri Dec 5 2003
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

#include <math.h>
#include <qapplication.h>
//Added by qt3to4:
#include <QList>

#include "gridmesh.h"
#include "gridremoval.h"
#include "transform.h"
#include "main.h"
#include "discretize.h"

GridRemoval::GridRemoval()
{
  DigitDebug::ctor(QString("gridremoval ") + QString::number((ulong) this, 16));
  
  neuronDict.setAutoDelete(true);

  searchPatternPlusX.setAutoDelete(true);
  searchPatternPlusY.setAutoDelete(true);
  searchPatternMinusX.setAutoDelete(true);
  searchPatternMinusY.setAutoDelete(true);
}

GridRemoval::~GridRemoval()
{
  DigitDebug::dtor(QString("gridremoval ") + QString::number((ulong) this, 16));
}

bool GridRemoval::colorSettingsChanged(GridRemovalSettings gridSettings,
  GridRemovalSettings* gridSettingsLatest)
{
  ASSERT_ENGAUGE(gridSettingsLatest != 0);
  return ((gridSettings.removeColor != gridSettingsLatest->removeColor) ||
    (gridSettings.color != gridSettingsLatest->color) ||
    (gridSettings.foregroundThresholdLow != gridSettingsLatest->foregroundThresholdLow) ||
    (gridSettings.foregroundThresholdHigh != gridSettingsLatest->foregroundThresholdHigh));
}
  
void GridRemoval::connectNeuronsAcrossGaps(char *pixels, int width, int height,
  double gapSeparation)
{
  int *pixel2Neuron = initializePixel2Neuron(pixels, width, height);

  neuronDict.clear(); // autoDelete is on
  
  for (unsigned int n = 1; n <= neuronDict.count(); n++)
    neuronDict[n]->connectNeuronAcrossGaps(pixels, pixel2Neuron, neuronDict,
      width, height, gapSeparation);

  delete[] pixel2Neuron;
}

double GridRemoval::distanceToOffPixel(char *pixels, int width, int height, int x, int y,
  SearchPattern *searchPattern, PixelState pixelThreshold)
{
  // pixels having values of pixelThreshold and higher are considered "on"

  SearchPatternIterator itr(*searchPattern);
  SearchPoint* p;
  while ((p = itr.current()) != 0)
  {
    int i = x + p->x();
    if ((0 <= i) && (i < width))
    {
      int j = y + p->y();
      if ((0 <= j) && (j < height))
      {
        if (pixels [FOLD2DINDEX(i, j, height)] < (int) pixelThreshold)
        {
          // reduce by one half since the distance is from the center of the on pixel
          // to the boundary of the off pixel
          return p->distance() - 0.5;
        }
      }
    }

    ++itr;
  }

  // return really big number
  return (double) width;
}

void GridRemoval::eraseThinPixels(char *pixels, int width, int height,
  double thinThickness, PixelState pixelThreshold)
{
  // loop through pixels, removing those on thin lines
  for (int x = 0; x < width; x++)
    for (int y = 0; y < height; y++)
    {
      if (pixels [FOLD2DINDEX(x, y, height)] == (int) PixelOn)
      {
        double dPlusX = distanceToOffPixel(pixels, width, height, x, y, &searchPatternPlusX,
          pixelThreshold);
        double dMinusX = distanceToOffPixel(pixels, width, height, x, y, &searchPatternMinusX,
          pixelThreshold);
        if (dMinusX + dPlusX <= thinThickness)
        {
          pixels [FOLD2DINDEX(x, y, height)] = pixelThreshold;
          continue;
        }
        else
        {
          double dPlusY = distanceToOffPixel(pixels, width, height, x, y, &searchPatternPlusY,
            pixelThreshold);
          double dMinusY = distanceToOffPixel(pixels, width, height, x, y, &searchPatternMinusY,
            pixelThreshold);
          if (dMinusY + dPlusY <= thinThickness)
          {
            pixels [FOLD2DINDEX(x, y, height)] = pixelThreshold;
            continue;
          }
        }
      }
    }
}
      
int *GridRemoval::initializePixel2Neuron(char *pixels, int width, int height)
{
  // each pixel gets assigned to a neuron. each neuron has its own index
  int *pixel2Neuron = new int [width * height];
  CHECK_PTR_ENGAUGE(pixel2Neuron);

  int x, y;
  for (x = 0; x < width; x++)
    for (y = 0; y < height; y++)
      pixel2Neuron [FOLD2DINDEX(x, y, height)] = 0;

  for (x = 0; x < width; x++)
    for (y = 0; y < height; y++)
      recursivelySetPixel2Neuron(pixels, pixel2Neuron, width, height,
        x, y, 0, 0, 0);
      
  return pixel2Neuron;
}

char *GridRemoval::initializePixels(QImage *imageWorking)
{
  // create and initialize an image in memory, which will greatly speed
  // up pixel reads and writes. using a custom array also allows an
  // splitting the "on" state into "on and part of gridline" and
  // "on and not part of gridline" states
  ASSERT_ENGAUGE(imageWorking != 0);
  int size = imageWorking->width() * imageWorking->height();
  char* image = new char[size];
  CHECK_PTR_ENGAUGE(image);
  for (int x = 0; x < imageWorking->width(); x++)
    for (int y = 0; y < imageWorking->height(); y++)
    {
      QColor pixel(pixelRGB(imageWorking, x, y));

      int hue, saturation, value;
      pixel.hsv(&hue, &saturation, &value);

      if (value < 127)
      {
        image [FOLD2DINDEX(x, y, imageWorking->height())] = PixelOn;
      }
      else
      {
        image [FOLD2DINDEX(x, y, imageWorking->height())] = PixelOff;
      }
    }

  return image;
}

void GridRemoval::initializeThin(CoordSettings coord,
  const Transform *transform, double thinThickness)
{
  // get basic vectors in screen coordinates. each is relative to (0,0)
  ASSERT_ENGAUGE(transform != 0);
  transform->xBasisScreen(coord, &xBasisXS, &xBasisYS);
  transform->yBasisScreen(coord, &yBasisXS, &yBasisYS);

  // initialize search patterns
  initializeThinSearch(thinThickness, xBasisXS, yBasisXS, &searchPatternPlusX);
  initializeThinSearch(thinThickness, xBasisYS, yBasisYS, &searchPatternPlusY);
  initializeThinSearch(thinThickness, -xBasisXS, yBasisXS, &searchPatternMinusX);
  initializeThinSearch(thinThickness, xBasisYS, -yBasisYS, &searchPatternMinusY);
}

void GridRemoval::initializeThinSearch(double thinThickness,
      double xBasis, double yBasis, SearchPattern *searchPattern)
{
  // simply loop through all pixels that could possibly be closer than
  // a half pixel from a line through (0,0) and parallel to the basis vector
  int ijmax = (int) (thinThickness + 0.5) + 1;
  for (int j = -ijmax; j <= ijmax; j++)
    for (int i = -ijmax; i <= ijmax; i++)
    {
      double x = (double) i;
      double y = (double) j;

      // this point must be in same direction as the basis vector. use dot product
      if (x * xBasis + y * yBasis > 0)
      {
        // this point must be within a half pixel of the basis vector
        if (distancetoline(x, y, 0.0, 0.0, xBasis, yBasis) <= 0.5)
        {
          // add new search point
          SearchPoint *p = new SearchPoint(i, j, sqrt(i * i + j * j));
          CHECK_PTR_ENGAUGE(p);

          searchPattern->append(p);
        }
      }
    }

  // sort the search points by increasing distance, since we will want to
  // find the point with the smallest distance first
  searchPattern->sort(); 
}

GridRemoval::Neuron::Neuron(NeuronDict *neuronDict)
{
  m_index = 1 + neuronDict->count(); // keys are numbered 1,2,...
  neuronDict->insert(m_index, this);

  m_synapseDict.setAutoDelete(true);
}

GridRemoval::Neuron::~Neuron()
{
}

GridRemoval::Synapse *GridRemoval::Neuron::addSynapse()
{
  Synapse *s = new Synapse(&m_synapseDict);
  CHECK_PTR_ENGAUGE(s);

  return s;
}

void GridRemoval::Neuron::connectNeuronAcrossGaps(char *pixels, int *pixel2Neuron,
  const NeuronDict neuronDict, int width, int height, double gapSeparation)
{
  int searchHalfWidth = (int) (gapSeparation + 0.5);

  // loop through synapses
  for (unsigned int i = 1; i <= m_synapseDict.count(); i++)
  {
    Synapse *s = m_synapseDict [i];
    int x = s->xCenterOfMass();
    int y = s->yCenterOfMass();

    // look at all pixels within a square centered around this synapse center-of-mass,
    // but only halfway around, starting straight up and going clockwise) around the
    // current pixel for other neurons. do not look in a full circle since then
    // we would end up with two connections for each pair of synapses (one in
    // either direction)
    QList<int> neuronsProcessed; // list of neurons that have been processed

    int xDelta, yDelta;
    for (xDelta = 0; xDelta <= searchHalfWidth; xDelta++)
      for (yDelta = -searchHalfWidth; yDelta <= searchHalfWidth; yDelta++)
      {
        // look at quadrants 1 and 4, and along +x and -y axes
        if ((0 < xDelta) || (yDelta < 0))
          connectSynapseToSynapsesSource(pixels, pixel2Neuron, neuronDict,
            width, height, gapSeparation,
            x, y, x + xDelta, y + yDelta, &neuronsProcessed);
      }
  }
}

void GridRemoval::Neuron::connectSynapseToSynapsesDestination(char *pixels,
  int height, double gapSeparation, int xSource, int ySource)
{
  // loop through synapses
  for (unsigned int i = 1; i <= m_synapseDict.count(); i++)
  {
    Synapse *s = m_synapseDict [i];
    int xDestination = s->xCenterOfMass();
    int yDestination = s->yCenterOfMass();
    double separation = sqrt((xDestination - xSource) * (xDestination - xSource) +
      (yDestination - ySource) * (yDestination - ySource));
    if (separation <= gapSeparation)
    {
      // draw a line from the source to the destination. we sacrifice efficiency for
      // simplicity, and do not use the Bresenham line-drawing algorithm (this is
      // a one-time cost and at most there should be less than a few hundred
      // pixels drawn in total)
      int nSteps = 1 + (int) (separation / 0.5); // separation in pixels times 1 step per 0.5 pixels
      double xDelta = (double) (xDestination - xSource) / (double) nSteps;
      double yDelta = (double) (yDestination - ySource) / (double) nSteps;
      double x = xSource + xDelta, y = ySource + yDelta;
      for (int i = 0; i < nSteps; i++)
      {
        // any pixel within a half pixel of (x,y) will be turned on
        int xLine = (int) x;
        int yLine = (int) y;
        if (sqrt((x - xLine) * (x - xLine) + (y - yLine) * (y - yLine)) < 0.5)
          pixels [FOLD2DINDEX(xLine, yLine, height)] = PixelOn;
        xLine += 1;
        if (sqrt((x - xLine) * (x - xLine) + (y - yLine) * (y - yLine)) < 0.5)
          pixels [FOLD2DINDEX(xLine, yLine, height)] = PixelOn;
        yLine += 1;
        if (sqrt((x - xLine) * (x - xLine) + (y - yLine) * (y - yLine)) < 0.5)
          pixels [FOLD2DINDEX(xLine, yLine, height)] = PixelOn;
        xLine -= 1;
        if (sqrt((x - xLine) * (x - xLine) + (y - yLine) * (y - yLine)) < 0.5)
          pixels [FOLD2DINDEX(xLine, yLine, height)] = PixelOn;

        x += xDelta;
        y += yDelta;
      }
    }
  }
}

void GridRemoval::Neuron::connectSynapseToSynapsesSource(char *pixels, int *pixel2Neuron,
  const NeuronDict neuronDict, int width, int height, double gapSeparation, int x, int y,
  int xLook, int yLook, QList<int> *neuronsProcessed)
{
  // this function tries to connect this synapse with center-of-mass at (x,y),
  // to the synapses of another neuron. the other neuron actually does the
  // work of making the connections

  if ((0 <= xLook) && (xLook < width) && (0 <= yLook) && (yLook < height))
  {
    // see if pixel belongs to a neuron, that is not this neuron,
    // and this synapse has not already been connected to that neuron
    int n = pixel2Neuron [FOLD2DINDEX(xLook, yLook, height)];
    if ((n != 0) &&
      (n != m_index) &&
      (neuronsProcessed->find(n) == neuronsProcessed->end()))
    {
      // let the other neuron do the actual connections, since there may be
      // zero, one or more and it alone knows where its synapses are
      neuronDict [n]->connectSynapseToSynapsesDestination(pixels, height, gapSeparation,
        x, y);

      // update list of process neurons
      neuronsProcessed->append(n);
    }
  }
}

void GridRemoval::recursivelySetPixel2Neuron(char *pixels, int *pixel2Neuron,
  int width, int height, int x, int y, Neuron *activeNeuron,
  Synapse *activeSynapse, int level)
{
  // if this pixel should belong to a neuron, then assign it to the active neuron.
  // if there is no active neuron (activeNeuron is null), then create one
  int xDelta, yDelta;
  
  // do not set this pixel if it is not on, or if it has already been assigned
  // to a neuron
  int i = FOLD2DINDEX(x, y, height);
  if ((pixels [i] != (int) PixelOn) || (pixel2Neuron [i] != 0))
    return;

  // this pixel needs to be assigned to a neuron
  if (activeNeuron == 0)
  {
    activeNeuron = new Neuron(&neuronDict);
    CHECK_PTR_ENGAUGE(activeNeuron);
  }
  pixel2Neuron [i] = activeNeuron->index();

  // does this pixel need to be assigned to a synapse? look at eight
  // nearest neighbors
  bool needSynapse = false;
  for (xDelta = -1; !needSynapse && (xDelta <= 1); xDelta++)
    for (yDelta = -1; !needSynapse && (yDelta <= 1); yDelta++)
      if ((xDelta != 0) || (yDelta != 0))
      {
        int xNeighbor = x + xDelta;
        int yNeighbor = y + yDelta;
        if ((0 <= xNeighbor) && (xNeighbor < width) &&
          (0 <= yNeighbor) && (yNeighbor < height))
        {
          if ((pixels [FOLD2DINDEX(xNeighbor, yNeighbor, height)] != (int) PixelOff) &&
            (pixels [FOLD2DINDEX(xNeighbor, yNeighbor, height)] != (int) PixelOn))
          {
            needSynapse = true;
          }
        }
      }

  // assign to synapse, creating a new one if necessary
  if (needSynapse)
  {
    if (activeSynapse == 0)
    {
      ASSERT_ENGAUGE(activeNeuron != 0);
      activeSynapse = activeNeuron->addSynapse();
    }
    activeSynapse->addPixel(x, y);
  }
  else
    activeSynapse = 0;

  // limit the levels of recursion since Microsoft Windows will run out of stack
  // space. specifically, the default stack size of one megabyte only handles
  // 5700 levels of recursion here, and a stack size of two megabytes only
  // handles 11000 levels of recursion. extreme amounts of recursion happen in
  // images with extreme numbers of lines
  if (level < RemovalMaxRecursion)
  {
    // also set the eight nearest neighbors
    for (xDelta = -1; xDelta <= 1; xDelta++)
      for (yDelta = -1; yDelta <= 1; yDelta++)
        if ((xDelta != 0) || (yDelta != 0))
        {
          int xNeighbor = x + xDelta;
          int yNeighbor = y + yDelta;
          if ((0 <= xNeighbor) && (xNeighbor < width) &&
            (0 <= yNeighbor) && (yNeighbor < height))
          {
            recursivelySetPixel2Neuron(pixels, pixel2Neuron, width, height,
              xNeighbor, yNeighbor, activeNeuron, activeSynapse, level + 1);
          }
        }
  }
}

void GridRemoval::removeAndConnect(QImage *imageWorking, const Transform *transform,
  CoordSettings coordSettings, GridRemovalSettings gridSettings, QRgb backgroundColor)
{
  // save the most recent settings
  static GridRemovalSettings gridSettingsLatest;
  gridSettingsLatest = gridSettings;

  // abort all calls to this function except the most recent, which last defined gridSettingsLatest
  qApp->processEvents();
  if (colorSettingsChanged(gridSettings, &gridSettingsLatest))
    return;

  int width = imageWorking->width();
  int height = imageWorking->height();

  char *pixels = initializePixels(imageWorking);

  if (!colorSettingsChanged(gridSettings, &gridSettingsLatest))
    if (gridSettings.removeColor)
      removeColor(imageWorking, pixels, width, height,
        gridSettings, &gridSettingsLatest, PixelOnRemovedStage1);

  if (!colorSettingsChanged(gridSettings, &gridSettingsLatest))
    if ((gridSettings.removeGridlines) && (gridSettings.gridDistance > 0.0))
      removeGridlines(pixels, width, height, transform, coordSettings,
        gridSettings, PixelOnRemovedStage2);

  if (!colorSettingsChanged(gridSettings, &gridSettingsLatest))
    if ((gridSettings.removeThinLines) && (gridSettings.thinThickness > 0.0))
      removeThinLines(pixels, width, height, coordSettings, transform,
        gridSettings.thinThickness, PixelOnRemovedStage3, PixelOnRemovedStage4);

  if (!colorSettingsChanged(gridSettings, &gridSettingsLatest))
    if (gridSettings.gapSeparation > 0.0)
      connectNeuronsAcrossGaps(pixels, width, height, gridSettings.gapSeparation);

  if (!colorSettingsChanged(gridSettings, &gridSettingsLatest))
    savePixels(pixels, width, height, imageWorking, backgroundColor);

  delete[] pixels;
}
    
void GridRemoval::removeColor(QImage *imageWorking, char *pixels, int width, int height,
  GridRemovalSettings gridSettings,
  GridRemovalSettings* gridSettingsLatest,
  PixelState pixelStateRemoved)
{
  int rBg, gBg, bBg;
  gridSettings.color.rgb(&rBg, &gBg, &bBg);

  ASSERT_ENGAUGE(imageWorking != 0);
  Discretize discretize;
  for (int x = 0; x < width; x++)
  {
    for (int y = 0; y < height; y++)
    {
      int value = discretize.discretizeValueForeground(imageWorking, x, y, DiscretizeForeground, rBg, gBg, bBg);

      if (!discretize.pixelIsOn(value, gridSettings))
        pixels [FOLD2DINDEX(x, y, height)] = pixelStateRemoved;
    }

    // abort all calls to this function except the most recent, which last defined gridSettingsLatest
    qApp->processEvents();
    if (colorSettingsChanged(gridSettings, gridSettingsLatest))
      break;
  }
}

void GridRemoval::removeGridlines(char *pixels, int width, int height,
  const Transform *transform, CoordSettings coordSettings,
  GridRemovalSettings gridSettings, PixelState pixelStateRemoved)
{
  ASSERT_ENGAUGE(transform != 0);
  if (transform->validAxes())
  {
    // get removal gridlines
    GridMesh gridMesh;
    GridlinesScreen gridlines;
    gridlines = gridMesh.makeGridLines(transform, coordSettings, gridSettings.gridMesh);

    GridlinesScreen::iterator itr;
    for (itr = gridlines.begin(); itr != gridlines.end(); ++itr)
    {
      int xStart = (*itr).start.x();
      int yStart = (*itr).start.y();
      int xStop = (*itr).stop.x();
      int yStop = (*itr).stop.y();
      if (dabs(xStop - xStart) < dabs(yStop - yStart))
        removeGridlineVertical(pixels, width, height, xStart, yStart, xStop, yStop,
          gridSettings, pixelStateRemoved);
      else
        removeGridlineHorizontal(pixels, width, height, xStart, yStart, xStop, yStop,
          gridSettings, pixelStateRemoved);
    }
  }
}

void GridRemoval::removeGridlineHorizontal(char *pixels, int width, int height,
  int xStart, int yStart, int xStop, int yStop, GridRemovalSettings gridSettings,
  PixelState pixelStateRemoved)
{
  // theta is the angle between the vertical column and the gridline. since we
  // divide by sinTheta, this function should NOT be used for vertical gridlines
  // since divide-by-zero error would occur
  if (xStart > xStop)
  {
    int temp = xStart;
    xStart = xStop;
    xStop = temp;
    temp = yStart;
    yStart = yStop;
    yStop = temp;
  }
  double sinTheta = sin(datan2safe(xStop - xStart, yStop - yStart)); // NOT datan2safe(dy,dx)!

  for (int x = (int) (xStart - gridSettings.gridDistance + 0.5);
    x < (int) (xStop + gridSettings.gridDistance + 0.5); x++)
  {
    // interest this pixel column (x=xc) with the gridline (x-x0)/(x1-x0)=(y-y0)/(y1-y0)
    // to get (xp,yp)
    double sLine1, sLine2;
    ASSERT_ENGAUGE(pixels != 0);
    if (intersecttwolines(x, 0.0, x, height,
      xStart, yStart, xStop, yStop, &sLine1, &sLine2))
    {
      double yp = (1.0 - sLine2) * yStart + sLine2 * yStop;
      int yLow = (int) (-gridSettings.gridDistance / sinTheta + yp + 0.5);
      int yHigh = (int) (gridSettings.gridDistance / sinTheta + yp + 0.5);
      for (int y = yLow; y <= yHigh; y++)
      {
        bool include = true;
        if (sLine2 < 0.0)
        {
          // at start end, the pixels have to be on the same side of (xStart,yStart) as
          // (xStop,yStop), so use dot product to see if this pixel is on the same side
          double dotProduct = (x - xStart) * (xStop - xStart) + (y - yStart) * (yStop - yStart);
          include = (dotProduct >= 0.0);
        }
        else if (sLine2 > 1.0)
        {
          // at stop end, the pixels have to be on the same side of (xStop,yStop) as
          // (xStart,yStart), so use dot product to see if this pixel is on the same side
          double dotProduct = (x - xStop) * (xStart - xStop) + (y - yStop) * (yStart - yStop);
          include = (dotProduct >= 0.0);
        }

        if (include && (0 <= x) && (x < width) && (0 <= y) && (y < height))
        {
          // overwrite this pixel with background color
          pixels [FOLD2DINDEX(x, y, height)] = pixelStateRemoved;
        }
      }
    }
  }
}

void GridRemoval::removeGridlineVertical(char *pixels, int width, int height,
  int xStart, int yStart, int xStop, int yStop, GridRemovalSettings gridSettings,
  PixelState pixelStateRemoved)
{
  // theta is the angle between the horizontal row and the gridline. since we
  // divide by sinTheta, this function should NOT be used for horizontal gridlines
  // since divide-by-zero error would occur
  if (yStart > yStop)
  {
    int temp = yStart;
    yStart = yStop;
    yStop = temp;
    temp = xStart;
    xStart = xStop;
    xStop = temp;
  }
  double sinTheta = sin(datan2safe(yStop - yStart, xStop - xStart));

  for (int y = (int) (yStart - gridSettings.gridDistance + 0.5);
    y < (int) (yStop + gridSettings.gridDistance + 0.5); y++)
  {
    // interest this pixel row (y=yc) with the gridline (x-x0)/(x1-x0)=(y-y0)/(y1-y0)
    // to get (xp,yp)
    double sLine1, sLine2;
    ASSERT_ENGAUGE(pixels != 0);
    if (intersecttwolines(0.0, y, width, y,
      xStart, yStart, xStop, yStop, &sLine1, &sLine2))
    {
      double xp = (1.0 - sLine2) * xStart + sLine2 * xStop;
      int xLow = (int) (-gridSettings.gridDistance / sinTheta + xp + 0.5);
      int xHigh = (int) (gridSettings.gridDistance / sinTheta + xp + 0.5);
      for (int x = xLow; x <= xHigh; x++)
      {
        bool include = true;
        if (sLine2 < 0.0)
        {
          // at start end, the pixels have to be on the same side of (xStart,yStart) as
          // (xStop,yStop), so use dot product to see if this pixel is on the same side
          double dotProduct = (x - xStart) * (xStop - xStart) + (y - yStart) * (yStop - yStart);
          include = (dotProduct >= 0.0);
        }
        else if (sLine2 > 1.0)
        {
          // at stop end, the pixels have to be on the same side of (xStop,yStop) as
          // (xStart,yStart), so use dot product to see if this pixel is on the same side
          double dotProduct = (x - xStop) * (xStart - xStop) + (y - yStop) * (yStart - yStop);
          include = (dotProduct >= 0.0);
        }

        if (include && (0 <= x) && (x < width) && (0 <= y) && (y < height))
        {
          // overwrite this pixel with background color
          pixels [FOLD2DINDEX(x, y, height)] = pixelStateRemoved;
        }
      }
    }
  }
}

void GridRemoval::removeThinLines(char *pixels, int width, int height,
  CoordSettings coord, const Transform *transform,
  double thinThickness, PixelState pixelStateRemovedPass1, PixelState pixelStateRemovedPass2)
{
  ASSERT_ENGAUGE(transform != 0);
  if (transform->validAxes())
  {
    initializeThin(coord, transform, thinThickness);

    // first pass erases the grid lines, except for the junctions
    eraseThinPixels(pixels, width, height, thinThickness, pixelStateRemovedPass1);

    // second pass erases the grid line junctions, which are little islands of on-pixels
    eraseThinPixels(pixels, width, height, thinThickness, pixelStateRemovedPass2);
  }
}

void GridRemoval::savePixels(char *pixels, int width, int height,
  QImage *imageWorking, QRgb backgroundColor)
{
  for (int x = 0; x < width; x++)
    for (int y = 0; y < height; y++)
    {
      PixelState state = (PixelState) pixels [FOLD2DINDEX(x, y, height)];
      if ((state != PixelOff) && (state != PixelOn))
        setPixelRGB(imageWorking, x, y, backgroundColor);
    }
}

GridRemoval::Synapse::Synapse(SynapseDict *synapseDict) :
  m_countPixels(0),
  m_xSum(0),
  m_ySum(0)
{
  m_index = 1 + synapseDict->count(); // keys are numbered 1,2,...
  synapseDict->insert(m_index, this);
}

GridRemoval::Synapse::~Synapse()
{
}

void GridRemoval::Synapse::addPixel(int x, int y)
{
  ++m_countPixels;
  m_xSum += x;
  m_ySum += y;
}

int GridRemoval::Synapse::xCenterOfMass()
{
  if (m_countPixels < 1)
    return 0;
  else
    return (int) (m_xSum / (double) m_countPixels + 0.5);
}

int GridRemoval::Synapse::yCenterOfMass()
{
  if (m_countPixels < 1)
    return 0;
  else
    return (int) (m_ySum / (double) m_countPixels + 0.5);
}
