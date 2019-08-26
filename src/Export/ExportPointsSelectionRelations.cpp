/******************************************************************************************************
 * (C) 2014 markummitchell@github.com. This file is part of Engauge Digitizer, which is released      *
 * under GNU General Public License version 2 (GPLv2) or (at your option) any later version. See file *
 * LICENSE or go to gnu.org/licenses for details. Distribution requires prior written permission.     *
 ******************************************************************************************************/

#include "ExportPointsSelectionRelations.h"
#include <QObject>

extern QString exportPointsSelectionRelationsToString (ExportPointsSelectionRelations exportPointsSelectionRelations)
{
  QString rtn;

  switch (exportPointsSelectionRelations) {
    case EXPORT_POINTS_SELECTION_RELATIONS_INTERPOLATE:
      rtn = QObject::tr ("Interpolate");
      break;

    case EXPORT_POINTS_SELECTION_RELATIONS_RAW:
      rtn = QObject::tr ("Raw");
      break;
  }

  return rtn;
}
