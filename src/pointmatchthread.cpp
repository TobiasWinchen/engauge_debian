/***************************************************************************
                          pointmatchthread.cpp  -  description
                             -------------------
    begin                : Mon Jan 1 2007
    copyright            : (C) 2007 by
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

#include <qmessagebox.h>
#include <qobject.h>
#include <qapplication.h>
#include <qlabel.h>
//Added by qt3to4:
#include <Q3PointArray>
#include <QList>

#include <math.h>
#include <stdlib.h>

#include "main.h"
#include "mmsubs.h"
#include "digitdebug.h"
#include "discretize.h"
#include "pointmatchthread.h"

PointMatchThread::PointMatchThread(const QImage &imageProcessed,
                                   PointMatchSettings settings,
                                   const Q3PointArray &samplePointPixels, 
                                   const Q3PointArray &pointsExisting) :
  m_imageProcessed(imageProcessed),
  m_settings(settings),
  m_samplePointPixels(samplePointPixels),
  m_pointsExisting(pointsExisting)
{
  DigitDebug::ctor(QString("pointmatchthread ") + QString::number((ulong) this, 16));
}

PointMatchThread::~PointMatchThread()
{
  DigitDebug::dtor(QString("pointmatchthread ") + QString::number((ulong) this, 16));
}

void PointMatchThread::allocateMemory(double** array, fftw_complex** arrayPrime,
                                      int width, int height)
{
  *array = new double [width * height];
  CHECK_PTR_ENGAUGE(*array);

  *arrayPrime = new fftw_complex [width * height];
  CHECK_PTR_ENGAUGE(*arrayPrime);
}

void PointMatchThread::assembleLocalMaxima(double* image, 
                                           double* sample, 
                                           double* convolution, 
                                           PointMatchList& listCreated, 
                                           int width, int height,
                                           int sampleXCenter, int sampleYCenter, 
                                           int sampleXExtent, int sampleYExtent)
{
  // ignore tiny correlation values near zero by applying this threshold
  const double SINGLE_PIXEL_CORRELATION = 1.0;

  int iDeltaMin = xDeltaMin(sampleXCenter);
  int iDeltaMax = xDeltaMax(sampleXCenter, sampleXExtent);
  int jDeltaMin = yDeltaMin(sampleYCenter);
  int jDeltaMax = yDeltaMax(sampleYCenter, sampleYExtent);

  for (int i = 0; i < width; i++)
    for (int j = 0; j < height; j++)
    {
      double convIJ = convolution[FOLD2DINDEX(i, j, height)];

      // loop through neighboring points
      bool isLocalMax = true;
      for (int iDelta= iDeltaMin; iDelta < iDeltaMax; iDelta++)
      {
        int iNeighbor = i + iDelta;
        if ((0 <= iNeighbor) && (iNeighbor < width))
        {
          for (int jDelta = jDeltaMin; jDelta < jDeltaMax; jDelta++)
          {
            int jNeighbor = j + jDelta;
            if ((0 <= jNeighbor) && (jNeighbor < height))
            {
              if ((convolution [FOLD2DINDEX(iNeighbor, jNeighbor, height)] >= convIJ) &&
                  (iNeighbor != i || jNeighbor != j))
              {
                isLocalMax = false;
                break;
              }
            }
          }
        }
      }
          
      if (isLocalMax && (convIJ > SINGLE_PIXEL_CORRELATION))
      {
        // save new local maximum
        PointMatchTriplet* t = new PointMatchTriplet;
        CHECK_PTR_ENGAUGE(t);

        t->x = i + sampleXCenter;
        t->y = j + sampleYCenter;

        // correlation stored in convIJ is not used since fast convolution is meant to
        // be performed once (it is global and slow), and later on we may want to redo 
        // local correlations when a maximum is removed and it overlaps another maxima. 
        // so, the manually-computed correlation value is assigned to this maximum. 
        // computation of manual convolutions is not too slow because there are 
        // typically less than 200 of them
        t->correlation = correlation(image, sample, width, height, t->x, t->y,
                                     sampleXCenter, sampleYCenter, 
                                     sampleXExtent, sampleYExtent);

        listCreated.append(t);
      }
    }  
}

void PointMatchThread::computeConvolution(fftw_complex* imagePrime, 
                                          fftw_complex* samplePrime, 
                                          int width, int height,
                                          double** convolution)
{
  fftw_complex* convolutionPrime;

  allocateMemory(convolution, &convolutionPrime, width, height);

  // perform in-place conjugation of the sample since equation is F-1 {F(f) * F*(g)}
  conjugateMatrix(width, height, samplePrime);

  // perform the convolution in transform space
  multiplyMatrices(width, height, imagePrime, samplePrime, convolutionPrime);

  // backward transform the convolution
  fftw_plan pConvolution = fftw_plan_dft_c2r_2d(width, height, 
                                                convolutionPrime, *convolution, 
                                                FFTW_ESTIMATE);

  fftw_execute (pConvolution);

  releasePhaseArray(convolutionPrime);
}

void PointMatchThread::conjugateMatrix(int width, int height, fftw_complex* matrix)
{
  ASSERT_ENGAUGE(matrix != 0);

  for (int x = 0; x < width; x++)
    for (int y = 0; y < height; y++)
    {
      int index = FOLD2DINDEX(x, y, height);
      matrix [index] [1] = -1.0 * matrix [index] [1];
    }
}

double PointMatchThread::correlation(double* image, double* sample,
                                     int width, int height,
                                     int x, int y, 
                                     int sampleXCenter, int sampleYCenter,
                                     int sampleXExtent, int sampleYExtent)
{
  ASSERT_ENGAUGE(image != 0);
  ASSERT_ENGAUGE(sample != 0);

  int iDeltaMin = xDeltaMin(sampleXCenter);
  int iDeltaMax = xDeltaMax(sampleXCenter, sampleXExtent);
  int jDeltaMin = yDeltaMin(sampleYCenter);
  int jDeltaMax = yDeltaMax(sampleYCenter, sampleYExtent);

  // highest correlation value corresponds to image pixels centered around (x,y)
  // matching the sample pixels centered around (sampleXCenter,sampleYCenter)
  double sum = 0.0;
  for (int iDelta = iDeltaMin; iDelta < iDeltaMax; iDelta++)
  {
    int iImage = x + iDelta;
    int iSample = sampleXCenter + iDelta; // assumes sample starts at lowest row
    if ((0 <= iImage) && (iImage < width))
    {
      for (int jDelta = jDeltaMin; jDelta < jDeltaMax; jDelta++)
      {
        int jImage = y + jDelta;
        int jSample = sampleYCenter + jDelta; // assumes sample starts at lowest column
        if ((0 <= jImage) && (jImage < height))
        {
          // good correlation occurs when both pixels have the same PixelOnUnscanned value. once
          // an image pixel has been scanned then it should be ignored
          if ((image [FOLD2DINDEX(iImage, jImage, height)] == PixelOnUnscanned) &&
              (sample [FOLD2DINDEX(iSample, jSample, height)] == PixelOnUnscanned))
          {
            sum += PixelOnUnscanned;
          }
	}
      }
    }
  }

  return sum;
}

void PointMatchThread::loadImage(const QImage &imageProcessed, PointMatchSettings settings,
                                 const Q3PointArray &pointsExisting,
                                 int width, int height, 
                                 double** image, fftw_complex** imagePrime)
{
  allocateMemory(image, imagePrime, width, height);
  
  populateImageArray(imageProcessed, 
                     width, height, 
                     image);

  removePixelsNearExistingPoints(*image, width, height, pointsExisting, 
                                 settings.pointSeparation);

  // forward transform the image
  fftw_plan pImage = fftw_plan_dft_r2c_2d(width, height, 
                                          *image, *imagePrime, 
                                          FFTW_ESTIMATE);
  fftw_execute(pImage);
}

void PointMatchThread::loadSample(const Q3PointArray &samplePointPixels, 
                                  int width, int height,
                                  double** sample, fftw_complex** samplePrime,
                                  int* sampleXCenter, int* sampleYCenter,
                                  int* sampleXExtent, int* sampleYExtent)
{
  // populate 2d sample array with same size (width x height) as image so fft 
  // transforms will have same dimensions, which means their transforms can be
  // multiplied element-to-element
  allocateMemory(sample, samplePrime, width, height);

  populateSampleArray(samplePointPixels, width, height, sample, 
                      sampleXCenter, sampleYCenter,
                      sampleXExtent, sampleYExtent);

  // forward transform the sample
  fftw_plan pSample = fftw_plan_dft_r2c_2d(width, height, 
                                           *sample, *samplePrime, 
                                           FFTW_ESTIMATE);
  fftw_execute(pSample);
}
void PointMatchThread::multiplyMatrices(int width, int height, 
                                        fftw_complex* in1, 
                                        fftw_complex* in2,
                                        fftw_complex* out)
{
  for (int x = 0; x < width; x++)
    for (int y = 0; y < height; y++)
    {
      int index = FOLD2DINDEX(x, y, height);

      out [index] [0] = in1 [index] [0] * in2 [index] [0] - in1 [index] [1] * in2 [index] [1];
      out [index] [1] = in1 [index] [0] * in2 [index] [1] + in1 [index] [1] * in2 [index] [0];
    }
}

uint PointMatchThread::optimizeLengthForFft(uint originalLength)
{
  const uint INITIAL_CLOSEST_LENGTH = 0;

  // loop through powers, looking for the lowest multiple of 2^a * 3^b * 5^c * 7^d
  // that is at or above the original length. Since the original length is expected
  // to usually be less than 2000, we use only the smaller primes (2, 3, 5 and 7) 
  // and ignore 11 and 13 even though fftw can benefit from those as well
  uint closestLength = INITIAL_CLOSEST_LENGTH;
  for (uint power2 = 1; power2 < originalLength; power2 *= 2)
  {
    for (uint power3 = 1; power3 < originalLength; power3 *= 3)
    {
      for (uint power5 = 1; power5 < originalLength; power5 *= 5)
      {
        for (uint power7 = 1; power7 < originalLength; power7 *= 7)
        {
          uint newLength = power2 * power3 * power5 * power7;
          if (originalLength <= newLength)
          {
            if ((closestLength == INITIAL_CLOSEST_LENGTH) ||
                (newLength < closestLength))
            {
              // this is the best so far, so save it. No special weighting
              // is given to powers of 2, although those can be more
              // efficient than other 
              closestLength = newLength;
            }
          }
        }
      }
    }
  }

  if (closestLength == INITIAL_CLOSEST_LENGTH)
  {
    // no closest length was found, so just return the original length 
    // and expect slow fft performance
    closestLength = originalLength;
  }

  return closestLength;
}

QList<PointMatchTriplet> PointMatchThread::pointsCreated()
{
  return m_pointsCreated;
}

void PointMatchThread::populateImageArray(const QImage &imageProcessed, 
                                          int width, int height,
                                          double** image)
{
  // initialize memory with original image in real component, and imaginary component set to zero
  Discretize discretize;
  for (int x = 0; x < width; x++)
    for (int y = 0; y < height; y++)
    {
      (*image) [FOLD2DINDEX(x, y, height)]  =
      (discretize.processedPixelIsOn(imageProcessed, x, y) ? 
       PixelOnUnscanned : 
       PixelOff);
    }
}

void PointMatchThread::populateSampleArray(const Q3PointArray &samplePointPixels, 
                                           int width, int height,
                                           double** sample, 
                                           int* sampleXCenter, int* sampleYCenter,
                                           int* sampleXExtent, int* sampleYExtent) 
{
  // compute bounds
  bool first = true;
  unsigned int i;
  int xMin = width, yMin = height, xMax = 0, yMax = 0;
  for (i = 0; i < (unsigned int) samplePointPixels.size(); i++)
  {
    int x = (samplePointPixels.at(i)).x();
    int y = (samplePointPixels.at(i)).y();
    if (first || (x < xMin))
      xMin = x;
    if (first || (x > xMax))
      xMax = x;
    if (first || (y < yMin))
      yMin = y;
    if (first || (y > yMax))
      yMax = y;

    first = false;
  }

  const int border = 1; // #pixels in border on each side

  xMin -= border;
  yMin -= border;
  xMax += border;
  yMax += border;

  // initialize memory with original image in real component, and imaginary component set to zero
  int x, y;
  for (x = 0; x < width; x++)
    for (y = 0; y < height; y++)
    {
      (*sample) [FOLD2DINDEX(x, y, height)] = PixelOff;
    }

  int xSum = 0, ySum = 0;
  for (i = 0; i < (unsigned int) samplePointPixels.size(); i++)
  {
    // place, quite arbitrarily, the sample image up against the top left corner
    x = (samplePointPixels.at(i)).x() - xMin;
    y = (samplePointPixels.at(i)).y() - yMin;
    ASSERT_ENGAUGE((0 < x) && (x < width));
    ASSERT_ENGAUGE((0 < y) && (y < height));
    (*sample) [FOLD2DINDEX(x, y, height)] = PixelOnUnscanned;

    xSum += x;
    ySum += y;
  }

  // compute location of center of mass, which will represent the center of the point
  *sampleXCenter = (int) ((double) xSum / (double) samplePointPixels.size() + 0.5);
  *sampleYCenter = (int) ((double) ySum / (double) samplePointPixels.size() + 0.5);

  // dimensions of portion of array actually used by sample (rest is empty)
  *sampleXExtent = xMax - xMin + 1;
  *sampleYExtent = yMax - yMin + 1;
}

void PointMatchThread::releaseImageArray(double* array)
{
  ASSERT_ENGAUGE(array != 0);
  delete[] array;
}

void PointMatchThread::releasePhaseArray(fftw_complex* arrayPrime)
{
  ASSERT_ENGAUGE(arrayPrime != 0);
  delete[] arrayPrime;
}

void PointMatchThread::removePixelsNearExistingPoints(double* image, 
                                                      int imageWidth, int imageHeight,
                                                      const Q3PointArray &pointsExisting, 
                                                      int pointSeparation)
{
  for (int i = 0; i < pointsExisting.size(); i++)
  {
    int xPoint = (pointsExisting.at(i)).x();
    int yPoint = (pointsExisting.at(i)).y();

    // loop through rows of pixels
    int yMin = yPoint - pointSeparation;
    if (yMin < 0)
      yMin = 0;
    int yMax = yPoint + pointSeparation;
    if (imageHeight < yMax)
      yMax = imageHeight;

    for (int y = yMin; y < yMax; y++)
    {
      // pythagorean theorem gives range of x values
      int radical = pointSeparation * pointSeparation - (y - yPoint) * (y - yPoint);
      if (0 < radical)
      {
        int xMin = (int) (xPoint - sqrt(radical));
        if (xMin < 0)
          xMin = 0;
        int xMax = xPoint + (xPoint - xMin);
        if (imageWidth < xMax)
          xMax = imageWidth;

        // turn off pixels in this row of pixels
        for (int x = xMin; x < xMax; x++)
        {
          if (image [FOLD2DINDEX(x, y, imageHeight)] != PixelOff)
          {
            image [FOLD2DINDEX(x, y, imageHeight)] = PixelOff;
          }
        }
      }
    }
  }
}

void PointMatchThread::run()
{
  // use larger arrays for computations, if necessary, to improve fft performance
  int originalWidth = m_imageProcessed.width();
  int originalHeight = m_imageProcessed.height();
  int width = optimizeLengthForFft(originalWidth);
  int height = optimizeLengthForFft(originalHeight);

  // the untransformed (unprimed) and transformed (primed) storage arrays can be huge
  // for big pictures, so minimize the number of allocated arrays at every point in time
  double *image, *sample, *convolution;
  fftw_complex *imagePrime, *samplePrime;

  // compute convolution=F(-1){F(image)*F(*)(sample)}
  int sampleXCenter, sampleYCenter, sampleXExtent, sampleYExtent;
  loadImage(m_imageProcessed, m_settings, m_pointsExisting, 
            width, height, 
            &image, &imagePrime);
  loadSample(m_samplePointPixels, width, height, &sample, &samplePrime,
             &sampleXCenter, &sampleYCenter, &sampleXExtent, &sampleYExtent);
  computeConvolution(imagePrime, samplePrime, width, height, &convolution);

  // assemble local maxima, where each is the maxima centered in a region
  // having a width of sampleWidth and a height of sampleHeight
  PointMatchList listCreated;
  assembleLocalMaxima(image, sample, convolution, listCreated, width, height,
                      sampleXCenter, sampleYCenter, sampleXExtent, sampleYExtent);
  listCreated.sort();

  // copy sorted match points to output
  for (PointMatchTriplet* t = listCreated.first(); t != 0; t = listCreated.next())
  {
    m_pointsCreated.append(*t);
    
    // current order of maxima would be fine if they never overlapped. however,
    // they often overlap so as each point is pulled off the list, and its pixels
    // are removed from the image, we might consider updating all succeeding maxima
    // here if those maximax overlap the just-removed maxima. the maxima list is kept
    // in descending order according to correlation value
  }

  releaseImageArray(image);
  releasePhaseArray(imagePrime);
  releaseImageArray(sample);
  releasePhaseArray(samplePrime);
  releaseImageArray(convolution);
}

int PointMatchThread::xDeltaMax(int sampleXCenter, int sampleXExtent)
{
  return (int) sampleXExtent - (int) sampleXCenter;
}

int PointMatchThread::xDeltaMin(int sampleXCenter)
{
  return -1 * (int) sampleXCenter;
}

int PointMatchThread::yDeltaMax(int sampleYCenter, int sampleYExtent)
{
  return (int) sampleYExtent - (int) sampleYCenter;
}

int PointMatchThread::yDeltaMin(int sampleYCenter)
{
  return -1 * (int) sampleYCenter;
}
