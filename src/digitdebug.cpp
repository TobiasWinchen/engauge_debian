/***************************************************************************
                          digitdebug.cpp  -  description
                             -------------------
    begin                : Tue Dec 3 2002
    copyright            : (C) 2002 by Mark Mitchell
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

#include <qdatetime.h>
#include <qobject.h>

#include "digitdebug.h"
#include "main.h"

unsigned long DigitDebug::debugMask = 0;

DigitDebug::DigitDebug()
{
}

DigitDebug::~DigitDebug()
{
}

bool DigitDebug::setDebugFlag(QString str)
{
  for (int flag = 0; flag < (int) MaxFlags; flag++)
    if (str == QString("-") + flagName((Flags) flag))
    {
      debugMask |= 1 << flag;
      return true;
    }
      
  return false;
}

QString DigitDebug::flagName(Flags flag)
{
  return flagStringsLookup(flag, LookupFlagName);
}

QString DigitDebug::usageHelp(Flags flag)
{
  return flagStringsLookup(flag, LookupUsageHelp);
}

QString DigitDebug::flagStringsLookup(Flags flag, Lookup lookup)
{
  switch (flag)
  {
    case Ctor:
      if (lookup == LookupFlagName)
        return QString("ctor");
      else
        return QString("trace constructor calls");
    case Dtor:
      if (lookup == LookupFlagName)
        return QString("dtor");
      else
        return QString("trace destructor calls");
    case CurveCmb:
      if (lookup == LookupFlagName)
        return QString("curvecmb");
      else
        return QString("trace curve combobox");
    case MeasureCmb:
      if (lookup == LookupFlagName)
        return QString("measurecmb");
      else
        return QString("trace measure combobox");
    case Refresh:
      if (lookup == LookupFlagName)
        return QString("refresh");
      else
        return QString("trace screen refreshes");
    case Scanning:
      if (lookup == LookupFlagName)
        return QString("scanning");
      else
        return QString("trace image scanning");
    case MaxFlags:
      break;
  }
  
  FATAL_ENGAUGE("unable to find debug lookup flag");
  return QString("");
}

void DigitDebug::genericDebug(Flags flag, QString str)
{
  unsigned long bit = 1 << flag;
  
  if ((bit & debugMask) != 0)
    qDebug("%s  %-8.8s  %s", QTime::currentTime().toString("hh:mm:ss:zzz").latin1(),
      flagName (flag).latin1(),
      str.latin1());
}

QString DigitDebug::usageShort()
{
  QString str;

  for (int flag = 0; flag < (int) MaxFlags; flag++)
  {
    str.append(QString(" [-") + flagName((Flags) flag) + QString("]"));
  }

  return str;
}

QString DigitDebug::usageLong()
{
  QString str;

  for (int flag = 0; flag < (int) MaxFlags; flag++)
  {
    QString name = QString("[-") + flagName((Flags) flag) + QString("] ");
    QString comment = usageHelp((Flags) flag).latin1();
    str.append(name + comment + QString("\n"));
  }

  return str;
}
