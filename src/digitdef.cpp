/***************************************************************************
                          digitdef.cpp  -  description
                             -------------------
    begin                : Tue Oct 29 14:43:44 PST 2002
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

#include "digitdef.h"
#include "main.h"

QString VersionNumber;
QStringList VersionNumbersSupported;

// globals that really should not belong to any class. the goal is to
// localize the simple user datatype code, with more complicated user
// datatypes getting their own classes
QStringList GridSetStrings;

// kludge for freeware Qt 2.3.1 so window captions work
#if defined(WIN32)
const char* freewareCaption = " [Qt]";
#else
const char* freewareCaption = "";
#endif

QString delimiter(ExportDelimiters del)
{
  switch(del)
  {
    case Commas:
    default:
      return QString(",");
    case Spaces:
      return QString(" ");
    case Tabs:
      return QString("\t");
  }
}

void initGlobals()
{
  GridSetStrings.append("Count");
  GridSetStrings.append("Start");
  GridSetStrings.append("Step");
  GridSetStrings.append("Stop");

  // following asserts guarantee the QStringList will effectively work as an array. there
  // is no QStringArray, and QStringList is supported by many widgets
  ASSERT_ENGAUGE(GridSetStrings.findIndex("Count") == AllButCount);
  ASSERT_ENGAUGE(GridSetStrings.findIndex("Start") == AllButStart);
  ASSERT_ENGAUGE(GridSetStrings.findIndex("Step") == AllButStep);
  ASSERT_ENGAUGE(GridSetStrings.findIndex("Stop") == AllButStop);

  VersionNumber = QString("5.0");
  VersionNumbersSupported <<
    "2.0" << "2.1" << "2.2" << "2.3" << "2.4" << "2.5" << "2.6" << "2.7" <<  "2.8" << 
    "2.9" << "2.10" << "2.11" << "2.12" << "2.13" << "2.14" << "2.15" << "3.0" << 
    "4.0" << "4.1" <<
    "5.0";
}

mmUnits mmUnitize(CoordSettings coord)
{
  if (coord.frame == Cartesian)
    return mmCartesian;
  else
  {
    // polar coordinates
    if (coord.thetaUnits == ThetaDegrees)
     return mmDegrees;
    else if (coord.thetaUnits == ThetaGradians)
      return mmGradians;
    else
      return mmRadians;
  }
}
