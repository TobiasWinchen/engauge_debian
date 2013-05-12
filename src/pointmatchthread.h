/***************************************************************************
                          pointmatchthread.h  -  description
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

#ifndef POINTMATCHTHREAD_H
#define POINTMATCHTHREAD_H

#include <qimage.h>
#include <qthread.h>
//Added by qt3to4:
#include <Q3PointArray>
#include <QList>

#include "digitdef.h"
#include "digitdebug.h"
#include "pointmatch.h"

#include "fftw3.h"

// this class, used by PointMatch, implements the threaded part of 
// the point matching calculations. since the fftw library performs its
// calculations in just 3 or 4 big steps and is responsive for long periods, 
// the lack of frequent returns precludes a progress bar, or a cancel button
class PointMatchThread : public QThread
{
  public:
  
    PointMatchThread(const QImage &imageProcessed,
                     PointMatchSettings settings, 
                     const Q3PointArray &samplePointPixels,
                     const Q3PointArray &pointsExisting);
    ~PointMatchThread();

    // perform thread calculations
    virtual void run();
        
    // output
    QList<PointMatchTriplet> pointsCreated();
  
  private:

    enum PixelStates
    {
      PixelOff, // pixel is off
      PixelOnUnscanned, // pixel is on and has not yet been scanned
      PixelOnScanned // pixel is on and has been scanned and assigned to a maxima, so ignore it
    };

    // allocate memory for an image array and phase array pair before calculations
    void allocateMemory(double** array, fftw_complex** arrayPrime,
                        int width, int height);

    // find each local maxima that is the largest value in a region that is:
    //   1. as big as the the sample
    //   2. centered about that local maxima
    void assembleLocalMaxima(double* image,
                             double* sample,
                             double* convolution, 
                             PointMatchList& listCreated, 
                             int width, int height,
                             int sampleXCenter, int sampleYCenter, 
                             int sampleXExtent, int sampleYExtent);

    // compute convolution in image space from phase space image and sample arrays
    void computeConvolution(fftw_complex* imagePrime,
                            fftw_complex* samplePrime,
                            int width, int height,
                            double** convolution);

    // in-place replacement of matrix by its complex conjugate
    void conjugateMatrix(int width, int height, fftw_complex* matrix);

    // correlate sample and image around specified screen location
    double correlation(double* image, double* sample,
                       int width, int height,
                       int x, int y, 
                       int sampleXCenter, int sampleYCenter, int sampleXExtent, int sampleYExtent);

    // load image and imagePrime arrays
    void loadImage(const QImage &imageProcessed, PointMatchSettings settings,
                   const Q3PointArray &pointsExisting,
                   int width, int height, 
                   double** image, fftw_complex** imagePrime);

    // load sample and samplePrime arrays, and compute center location and extent
    void loadSample(const Q3PointArray &samplePointPixels, int width, int height,
                    double** sample, fftw_complex** samplePrime,
                    int* sampleXCenter, int* sampleYCenter,
                    int* sampleXExtent, int* sampleYExtent);

    // multiply corresponding elements of two matrices into a third matrix
    void multiplyMatrices(int width, int height, fftw_complex* in1, fftw_complex* in2,
                          fftw_complex* out);
    
    // given an original array length, this method returns an array length that includes
    // enough padding so that the array length equals 2^a * 3^b * 5^c * 7^d, which
    // optimizes the fft performance. Typical memory penalties are less than 6% to get
    // a cpu performance increase of 0% to roughly 100% or 200%
    uint optimizeLengthForFft(uint originalLength);

    // populate sample array with sample image
    void populateSampleArray(const Q3PointArray &samplePointPixels, int width, int height,
                             double** sample,
                             int* sampleXCenter, int* sampleYCenter,
                             int* sampleXExtent, int* sampleYExtent);
                        
    // populate image array with processed image
    void populateImageArray(const QImage &imageProcessed, 
                            int width, int height,
                            double** image);

    // release memory for one array after finishing calculations
    void releaseImageArray(double* array);
    void releasePhaseArray(fftw_complex* array);

    // prevent duplication of existing points. this function returns
    // the number of pixels removed
    void removePixelsNearExistingPoints(double* image, int imageWidth, int imageHeight,
      const Q3PointArray &pointsExisting, int pointSeparation);
  
    // correlate the sample point with the image, returning points in list that is
    // sorted by correlation
    void scanImage(bool* sampleMaskArray,
      int sampleMaskWidth, int sampleMaskHeight,
      int sampleXCenter, int sampleYCenter, PointMatchSettings settings,
      int* imageArray, int imageWidth, int imageHeight,
      PointMatchList* pointsCreated);

    // bounds of region around sample center
    int xDeltaMax(int sampleXCenter, int sampleXExtent);
    int xDeltaMin(int sampleXCenter);
    int yDeltaMax(int sampleYCenter, int sampleYExtent);
    int yDeltaMin(int sampleYCenter);

    // inputs
    const QImage& m_imageProcessed;
    PointMatchSettings m_settings;
    const Q3PointArray& m_samplePointPixels;
    const Q3PointArray& m_pointsExisting;

    // output
    QList<PointMatchTriplet> m_pointsCreated;

};

#endif // POINTMATCHTHREAD_H
