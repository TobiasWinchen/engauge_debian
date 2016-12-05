/******************************************************************************************************
 * (C) 2014 markummitchell@github.com. This file is part of Engauge Digitizer, which is released      *
 * under GNU General Public License version 2 (GPLv2) or (at your option) any later version. See file *
 * LICENSE or go to gnu.org/licenses for details. Distribution requires prior written permission.     *
 ******************************************************************************************************/

#ifndef EXPORT_ORDINALS_SMOOTH_H
#define EXPORT_ORDINALS_SMOOTH_H

#include "ExportValuesOrdinal.h"
#include "Point.h"
#include "Points.h"
#include "SplinePair.h"
#include <QList>
#include <vector>

class Transformation;

/// Utility class to interpolate points spaced evenly along a piecewise defined curve with fitted spline
class ExportOrdinalsSmooth 
{
 public:
  /// Single constructor
  ExportOrdinalsSmooth ();

  /// Load t (=ordinal) and xy (=screen position) spline pairs, without any conversion to graph coordinates
  void loadSplinePairsWithoutTransformation (const Points &points,
                                             std::vector<double> &t,
                                             std::vector<SplinePair> &xy) const;

  /// Load t (=ordinal) and xy (=screen position) spline pairs, converting screen coordinates to graph coordinates
  void loadSplinePairsWithTransformation (const Points &points,
                                          const Transformation &transformation,
                                          bool isLogXTheta,
                                          bool isLogYRadius,
                                          std::vector<double> &t,
                                          std::vector<SplinePair> &xy) const;

  /// Perform the interpolation on the arrays loaded by the other methods
  ExportValuesOrdinal ordinalsAtIntervalsGraph (const std::vector<double> &t,
                                                const std::vector<SplinePair> &xy,
                                                double pointsInterval) const;
};

#endif // EXPORT_ORDINALS_SMOOTH_H
