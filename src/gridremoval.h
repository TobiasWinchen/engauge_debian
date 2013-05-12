/***************************************************************************
                          gridremoval.h  -  description
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

#ifndef GRIDREMOVAL_H
#define GRIDREMOVAL_H

#include <qimage.h>
#include <qcolor.h>
#include <qpoint.h>
#include <q3intdict.h>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <Q3PtrCollection>
#include <QList>

#include "digitdef.h"
#include "digitdebug.h"

class Transform;

// this class removes gridlines from an image using a combination of
// approaches. the following goals were kept in mind during development:
//   (1) algorithms had to actually remove gridlines
//   (2) algorithms had to be computationally efficient
//   (3) algorithms had to be simple enough to be quickly understood
// unfortunately, compiler quirks forced the Synapse, SynapseDict, Neuron and NeuronDict
// classes to be public rather than private as they should be for encapsulation
class GridRemoval
{
  public:
  
    GridRemoval();
    ~GridRemoval();

    // remove gridlines and then tidy up afterwards by reconnecting across gaps.
    // this function takes a fifth of a second to complete,
    // so it is reentrant with new calls aborting previous calls (since gridremovaldlg calls
    // this once per mouse move event so the program bogs down)
    void removeAndConnect(QImage *imageWorking, const Transform *transform,
      CoordSettings coordSettings, GridRemovalSettings gridSettings, QRgb backgroundColor);

    // each stage N considers pixels with state below PixelOnRemovedN to be off, otherwise on
    enum PixelState {
      PixelOff = 0,
      PixelOnRemovedStage1,
      PixelOnRemovedStage2,
      PixelOnRemovedStage3,
      PixelOnRemovedStage4,
      PixelOn
    };

    class Synapse;
    typedef Q3IntDict<Synapse> SynapseDict;

    // a synapse is a complete set of adjacent on-pixels where each is adjacent to at
    // least one removed-pixel. the pixels of a synapse are a subset of the pixels in
    // a neuron. a neuron can have zero, one or more synapses. each synapse has a
    // center-of-mass, which is the arithmetic average of the (x,y) coordinates of all
    // the component pixels.
    // this class should be private but Microsoft compiler will choke if it is
    class Synapse
    {
      public:
        Synapse(SynapseDict *synapseDict);
        ~Synapse();

        void addPixel(int x, int y);
        long index() { return m_index; }
        int xCenterOfMass();
        int yCenterOfMass();

      private:
        long m_index;
        int m_countPixels;
        int m_xSum;
        int m_ySum;
    };

    class Neuron;
    typedef Q3IntDict<Neuron> NeuronDict;

    // a neuron is a complete set of adjacent on-pixels. each neuron is identified by
    // its index in the neuron list. each neuron has zero or more synapses
    class Neuron
    {
      public:
        Neuron(NeuronDict *neuronDict);
        ~Neuron();

        Synapse *addSynapse();
        void connectNeuronAcrossGaps(char *pixels, int *pixel2Neuron,
          const NeuronDict neuronDict, int width, int height, double gapSeparation);
        long index() { return m_index; }

      private:
        long m_index;
        SynapseDict m_synapseDict;

        void connectSynapseToSynapsesDestination(char *pixels,
          int height, double gapSeparation, int xSource, int ySource);
        void connectSynapseToSynapsesSource(char *pixels, int *pixel2Neuron,
          const NeuronDict neuronDict, int width, int height,
          double gapSeparation, int x, int y,
          int xLook, int yLook, QList<int> *neuronsProcessed);
    };

    // a "search point" is a pixel along one of the axis directions
    // (+x,-x,+y,-y) that is to be checked. the search pattern is composed
    // of search points along those four directions. search points and
    // search patterns are used for thin gridline removal
    class SearchPoint
    {
      public:

        SearchPoint(int x, int y, double distance)
        {
          m_point.setX(x);
          m_point.setY(y);
          m_distance = distance;
        }
        ~SearchPoint() {};

        int x() { return m_point.x(); }
        int y() { return m_point.y(); }
        double distance() { return m_distance; }

      private:

        // (x,y) screen coordinates of this search pattern point, relative to
        // pixel under consideration
        QPoint m_point;

        // distance from pixel under consideration. positive if parallel to axis,
        // and negative if antiparallel
        double m_distance;
    };

  private:

    // pointer list that also sorts SearchPoint objects by distance
    template <class T>
    class SearchPointPtrList: public Q3PtrList<T>
    {
      protected:

        virtual int compareItems(Q3PtrCollection::Item item1, Q3PtrCollection::Item item2)
        {
          SearchPoint* p1 = (SearchPoint*) item1;
          SearchPoint* p2 = (SearchPoint*) item2;
          if (p1->distance() < p2->distance())
            return -1;
          else if (p1->distance() > p2->distance())
            return 1;
          else
            return 0;
        }
    };

    // the point lists are pointer lists (versus value lists) so they can be sorted.
    // a search pattern is a sorted list of search points, which is an efficient
    // way of ignoring unimportant points, and efficiently scanning the important points
    typedef SearchPointPtrList<SearchPoint> SearchPattern;
    typedef Q3PtrListIterator<SearchPoint> SearchPatternIterator;

    NeuronDict neuronDict;

    // true if any color settings changed
    bool colorSettingsChanged(GridRemovalSettings gridSettings,
      GridRemovalSettings* gridSettingsLatest);
  
    // start gridline removal and gap connection by copying image into pixel array
    char *initializePixels(QImage *imageWorking);

    // main gridline removal functions
    void removeColor(QImage *imageWorking, char *pixels, int width, int height,
      GridRemovalSettings gridSettings,
      GridRemovalSettings* gridSettingsLatest,
      PixelState pixelStateRemoved);
    void removeGridlines(char *pixels, int width, int height, const Transform *transform,
      CoordSettings coordSettings, GridRemovalSettings gridSettings,
      PixelState pixelStateRemoved);
    void removeThinLines(char *pixels, int width, int height, CoordSettings coord,
      const Transform *transform, double thinThickness,
      PixelState pixelStateRemovedPass1, PixelState pixelStateRemovedPass2);

    // finish gridline removal and gap connection by saving pixel choices into working image
    void savePixels(char *pixels, int width, int height,
      QImage *imageWorking, QRgb backgroundColor);
          
    // gridline removal functions
    void removeGridlineHorizontal(char *pixels, int width, int height,
      int xStart, int yStart, int xStop, int yStop, GridRemovalSettings gridSettings,
      PixelState pixelStateRemoved);
    void removeGridlineVertical(char *pixels, int width, int height,
      int xStart, int yStart, int xStop, int yStop, GridRemovalSettings gridSettings,
      PixelState pixelStateRemoved);

    // thin gridline removal functions
    double distanceToOffPixel(char *pixels, int width, int height, int x, int y,
      SearchPattern *searchPattern, PixelState pixelThreshold);
    void initializeThin(CoordSettings coord, const Transform *transform,
      double thinThickness);
    void initializeThinSearch(double thinThickness, double xBasis, double yBasis,
      SearchPattern *searchPattern);
    void eraseThinPixels(char *pixels, int width, int height,
      double thinThickness, PixelState pixelThreshold);

    // thin gridline removal variables
    double xBasisXS, xBasisYS, yBasisXS, yBasisYS; // basis vectors in screen coordinates
    SearchPattern searchPatternPlusX; // search pattern in +x direction
    SearchPattern searchPatternPlusY; // search pattern in +y direction
    SearchPattern searchPatternMinusX; // search pattern in -x direction
    SearchPattern searchPatternMinusY; // search pattern in -y direction

    // gaps smaller than epsilon are connected using a "neuron-synapse"
    // metaphor. a neuron is defined as a group of connected on-pixels. a
    // synapse is a group of connect on-pixels, within a neuron and also
    // touching pixels that were removed by gridline removal functions.
    // synapses within the epsilon distance are connected by a line from
    // the center-of-mass of the first synapse to the center-of-mass of
    // the second line
    void connectNeuronsAcrossGaps(char *pixels, int width, int height,
      double gapSeparation);
    int *initializePixel2Neuron(char *pixels, int width, int height);
    void recursivelySetPixel2Neuron(char *pixels, int *pixel2Neuron,
      int width, int height, int x, int y, Neuron *activeNeuron,
      Synapse *activeSynapse, int level);
};

#endif // GRIDREMOVAL_H
