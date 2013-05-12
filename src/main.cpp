/***************************************************************************
                          main.cpp  -  description
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

#include <qapplication.h>
#include <qfont.h>
#include <qmessagebox.h>
#include <qstring.h>
#include <qtextcodec.h>
#include <qtranslator.h>

#include <stdio.h>
#include <stdlib.h>

#include "digitmain.h"
#include "clipboard.h"
#include "pointsetstyles.h"
#include "digitdef.h"
#include "digitdebug.h"
#include "main.h"
#include "defaultsettings.h"

// command line options. static variables are local and others are global
static QString cmdImport;
static QString cmdExport;
static QString cmdOpen;
QDir cmdManualDirectory;
bool cmdLazySegments;
bool cmdOnlyBmp;
bool cmdPixels;
bool cmdSettingsReset;
bool cmdSettingsText;
static bool cmdXYAxes;
static double cmdXMinG; // x range min in graph units
static double cmdXMaxG; // x range max in graph units
static double cmdYMinG; // y range min in graph units
static double cmdYMaxG; // y range max in graph units

void parseCmdLine(int argc, char *argv[]);
bool parseRangePair(QString arg1, QString arg2,
  double &rangeMin, double &rangeMax);
void showAllUsageAndQuit(bool showAsError);
void showAndQuit(QString msgOutput, bool showAsError);
void showAxisUsageAndQuit();

void assert_engauge(bool condition, ENGAUGE_CONST_MODIFIER char *file, const int line)
{
  if (!condition)
  {
    QMessageBox::critical(0, "Engauge Digitizer",
      QObject::tr("Unexpected problem in %1 at line %2\n")
        .arg(file)
        .arg(line) +
        "Please contact the author at mmdigitizer@gmail.com with this information");
    exit(-1);
  }
}

void check_ptr_engauge(const char *ptr, ENGAUGE_CONST_MODIFIER char *file, const int line)
{
  if (ptr == 0)
  {
    QMessageBox::critical(0, "Engauge Digitizer",
      QObject::tr("Out of memory in %1 at line %2\n")
        .arg(file)
        .arg(line) +
        "Please contact the author at mmdigitizer@gmail.com with this information");
    exit(-1);
  }
}

void fatal_engauge(const char *comment, ENGAUGE_CONST_MODIFIER char *file, const int line)
{
  QMessageBox::critical(0, "Engauge Digitizer",
    QObject::tr("Fatal error in %1 at line %2\n")
    .arg(file)
    .arg(line) +
    "\nThe description is '" + comment + "'\n" +
    "Please contact the author at mmdigitizer@gmail.com with this information");
  exit(-1);
}

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

#ifdef Q_OS_MACX
  // connect to plugins directory
  app.setLibraryPaths(app.applicationDirPath() + "/../PlugIns");
#endif

  parseCmdLine(argc, argv);

  initGlobals();
  Clipboard::create();
  PointSetStyles::create();
  DefaultSettings::create();

  DefaultSettings& rSettings = DefaultSettings::instance();
  if (rSettings.getWindowMainFontOverride())
  {
    app.setFont(QFont(rSettings.getWindowMainFontName(),
      rSettings.getWindowMainFontSize()));
  }

  QTranslator tor (0);
  // set the location where your .qm files are in load() below as the last parameter instead of "."
  // for development, use "/" to use the english original as
  // .qm files are stored in the base project directory.
  tor.load (QString("digitizer.") + QTextCodec::locale(), ".");
  app.installTranslator (&tor);

  DigitMain digitizer(cmdExport);
  qApp->setMainWidget(&digitizer);

  digitizer.show();

  if (!cmdImport.isEmpty())
	{
    if (cmdXYAxes)
		{
      digitizer.importImageFile(cmdImport, cmdXMinG, cmdXMaxG, cmdYMinG, cmdYMaxG);
		}
    else
		{
      digitizer.importImageFile(cmdImport);
		}
	}

  if (!cmdOpen.isEmpty())
    digitizer.openDocumentFile (cmdOpen);

  return app.exec();
}

void parseCmdLine(int argc, char *argv[])
{
  cmdImport = "";
  cmdExport = "";
  cmdOpen = "";
  cmdManualDirectory = "";
  cmdLazySegments = false;
  cmdOnlyBmp = false;
  cmdPixels = false;
  cmdSettingsReset = false;
  cmdSettingsText = false;
  cmdXYAxes = false;

  bool success = true;
  bool usage = false;

  // QApplication constructor documentation describes the qt built-in command line
  // arguments. we will parse our own debug flags
  for (int i = 1; i < argc; i++)
  {
    QString arg(argv [i]);

    if ((arg == QString("-import")) && (i + 1 < argc))
    {
      cmdImport = argv [i + 1];
      ++i;
    }
    else if ((arg == QString("-export")) && (i + 1 < argc))
    {
      cmdExport = argv [i + 1];
      ++i;
    }
    else if ((arg == QString("-open")) && (i + 1 < argc))
    {
      cmdOpen = argv [i + 1];
      ++i;
    }
    else if (QFile::exists(arg))
    {
      cmdOpen = arg;
    }
    else if (arg == QString("-help"))
      usage = true;
    else if ((arg == QString("-manual")) && (i + 1 < argc))
    {
      cmdManualDirectory = argv [i + 1];
      if (!cmdManualDirectory.exists())
      {
        QMessageBox::critical(0, "Engauge Digitizer",
          QObject::tr("User manual directory %1 does not exist")
            .arg(cmdManualDirectory.path()));
        success = false;
      }
      ++i;
    }
    else if (arg == QString("-onlybmp"))
      cmdOnlyBmp = true;
    else if (arg == QString("-lazysegments"))
      cmdLazySegments = true;
    else if (arg == QString("-pixels"))
      cmdPixels = true;
    else if (arg == QString("-reset"))
      cmdSettingsReset = true;
    else if (arg == QString("-text"))
      cmdSettingsText = true;
    else if ((arg == QString("-axes")) && (i + 4 < argc))
    {
      if (parseRangePair(QString(argv [i + 1]), QString(argv [i + 2]), cmdXMinG, cmdXMaxG) &&
        parseRangePair(QString(argv [i + 3]), QString(argv [i + 4]), cmdYMinG, cmdYMaxG))
        cmdXYAxes = true;
      else
        success = false;

      i += 4;
    }
    else if ((arg == QString("-nograb")) ||
             (arg == QString("-dograb")) ||
             (arg == QString("-sync")))
    {
       // ignore these Qt command line debug options. They will be 
       // processed by QApplication if it was compiled with debugging
    }
    else if (!DigitDebug::setDebugFlag (QString (argv [i])))
      success = false;
  }

  if (!success)
    showAllUsageAndQuit(true); // show as error
  if (usage)
    showAllUsageAndQuit(false); // show as information

  if (cmdXYAxes && cmdImport.isEmpty())
    showAxisUsageAndQuit();
}

bool parseRangePair(QString arg1, QString arg2,
  double &rangeMin, double &rangeMax)
{
  bool okArg1, okArg2;

  rangeMin = arg1.toDouble(&okArg1);
  rangeMax = arg2.toDouble(&okArg2);

  return okArg1 && okArg2;
}

void showAllUsageAndQuit(bool showAsError)
{
  QString msgOutput;

  // display one line with all options unadorned
  msgOutput += QObject::tr(
    "\nUsage:\nengauge [-import FILE] [-export FILE] [-open FILE] [FILE] [-help] [-manual DIRECTORY]\n"
    "[-axes XMIN XMAX YMIN YMAX] [-reset] [-text] [-lazysegments] [-onlybmp] [-pixels]\n%1\n")
    .arg(DigitDebug::usageShort().latin1());

  // display multiple lines with each line having the details of one option
  msgOutput += QObject::tr("\nStartup:\n");
  msgOutput += QObject::tr("[-import FILE] import image file at startup\n");
  msgOutput += QObject::tr("[-open FILE] open document file at startup\n");
  msgOutput += QObject::tr("[FILE] open document file at startup\n");
  msgOutput += QObject::tr("[-help] display this help and exit\n");
  msgOutput += QObject::tr("[-manual DIRECTORY] look for the user manual in the specified directory\n");
  msgOutput += QObject::tr("[-axes XMIN XMAX YMIN YMAX] digitize axis points while importing file\n");
  msgOutput += QObject::tr("[-lazysegments] postpone scanning for segments during startup until needed later\n");
  msgOutput += QObject::tr("\nShutdown:\n");
  msgOutput += QObject::tr("[-export FILE] export active document at shutdown\n");
  msgOutput += QObject::tr("\nSettings:\n");
  msgOutput += QObject::tr("[-reset] reset settings to factory defaults\n");
  msgOutput += QObject::tr("[-text] load and save settings in a text file rather than the registry (Windows only)\n");
  msgOutput += QObject::tr("\nInternationalization:\n");
  msgOutput += QObject::tr("[-onlybmp] import all images as bmp files (fix for Chinese Windows)\n");
  msgOutput += QObject::tr("\nDebugging:\n");
  msgOutput += QObject::tr("[-pixels] track cursor in pixels\n");
  msgOutput += QObject::tr(DigitDebug::usageLong().latin1());

  // environment variables
  msgOutput += QObject::tr("\nEnvironment variables:\n");
  msgOutput += BrowserEnvironmentVariable + QObject::tr("=command to run browser for date/time converter\n");
  msgOutput += UserManualEnvironmentVariable + QObject::tr("=look for the user manual in the specified directory\n");

  showAndQuit(msgOutput, showAsError);
}

void showAndQuit(QString msgOutput, bool showAsError)
{
  msgOutput += QObject::tr("\n");
  
  // show usage using a messagebox rather than writing to stdout or stderr since console
  // output is suppressed in microsoft windows
  if (showAsError)
    QMessageBox::critical(0, QObject::tr("Engauge Digitizer"), msgOutput);
  else
    QMessageBox::information(0, QObject::tr("Engauge Digitizer"), msgOutput);

  exit(-1);
}

void showAxisUsageAndQuit()
{
  QString msgOutput;

  msgOutput += QObject::tr("Command line option '-axes' requires option '-import'\n");

  showAndQuit(msgOutput, true); // show as error
}
