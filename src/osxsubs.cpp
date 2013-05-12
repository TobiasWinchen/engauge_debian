/***************************************************************************
                          osxsubs.cpp  -  description
                             -------------------
    begin                : Sun Nov 7 2004
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

#include <qmessagebox.h>

#include "osxsubs.h"

#ifdef Q_OS_MACX

#include <CoreFoundation/CoreFoundation.h>

QDir userManualPath()
{
  CFURLRef pluginRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
  CFStringRef macPath = CFURLCopyFileSystemPath(pluginRef,
                                                kCFURLPOSIXPathStyle);
  const char *pathPtr = CFStringGetCStringPtr(macPath,
                                              CFStringGetSystemEncoding());

  QString p(pathPtr);
  p += "/Contents/Resources/usermanual";

  return QDir(p);
}

#endif
