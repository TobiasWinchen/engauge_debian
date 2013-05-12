/***************************************************************************
                          sessionsdlg.cpp  -  description
                             -------------------
    begin                : Thu Oct 7 2004
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

#include <QWhatsThis>
#include <qpushbutton.h>
#include <qtooltip.h>
//Added by qt3to4:
#include <QPixmap>
#include <QLabel>

#include "sessionsdlg.h"
#include "img/whatsthis.xpm"
#include "digitdebug.h"
#include "defaultsettings.h"
#include "main.h"

SessionsDlg::SessionsDlg(SessionsSettings* sessionsSettings) :
  QDialog(0, "SessionsDlg", false),
  m_sessionsSettings(sessionsSettings)
{
  DigitDebug::ctor(QString("sessionsdlg " + QString::number((ulong) this, 16)));

  setCaption(tr("Sessions %1")
    .arg(freewareCaption));

  setFixedSize(380, 320);

  // sessions settings
  checkSessions = new QCheckBox(QString(tr("Save the following settings between sessions:")), this);
  CHECK_PTR_ENGAUGE(checkSessions);
  checkSessions->setGeometry(20, 20, 325, 20);
  QWhatsThis::add(checkSessions, QString(tr("Select this to enable saving of settings between sessions")));
  connect(checkSessions, SIGNAL(toggled(bool)), this, SLOT(slotSessions(bool)));

  checkCoordinates = new QCheckBox(QString(tr("Coordinates")), this);
  CHECK_PTR_ENGAUGE(checkCoordinates);
  checkCoordinates->setGeometry(50, 50, 130, 20);
  QWhatsThis::add(checkCoordinates, QString(tr("Select this to save coordinates settings between sessions")));
  connect(checkCoordinates, SIGNAL(toggled(bool)), this, SLOT(slotCoordinates(bool)));

  checkAxes = new QCheckBox(QString(tr("Axes")), this);
  CHECK_PTR_ENGAUGE(checkAxes);
  checkAxes->setGeometry(50, 80, 130, 20);
  QWhatsThis::add(checkAxes, QString(tr("Select this to save axes settings between sessions")));
  connect(checkAxes, SIGNAL(toggled(bool)), this, SLOT(slotAxes(bool)));

  checkScaleBar = new QCheckBox(QString(tr("Scale bar")), this);
  CHECK_PTR_ENGAUGE(checkScaleBar);
  checkScaleBar->setGeometry(50, 110, 130, 20);
  QWhatsThis::add(checkScaleBar, QString(tr("Select this to save scale bar settings between sessions")));
  connect(checkScaleBar, SIGNAL(toggled(bool)), this, SLOT(slotScaleBar(bool)));

  checkCurves = new QCheckBox(QString(tr("Curves")), this);
  CHECK_PTR_ENGAUGE(checkCurves);
  checkCurves->setGeometry(50, 140, 130, 20);
  QWhatsThis::add(checkCurves, QString(tr("Select this to save curves settings between sessions")));
  connect(checkCurves, SIGNAL(toggled(bool)), this, SLOT(slotCurves(bool)));

  checkSegments = new QCheckBox(QString(tr("Segments")), this);
  CHECK_PTR_ENGAUGE(checkSegments);
  checkSegments->setGeometry(50, 170, 130, 20);
  QWhatsThis::add(checkSegments, QString(tr("Select this to save segments settings between sessions")));
  connect(checkSegments, SIGNAL(toggled(bool)), this, SLOT(slotSegments(bool)));

  checkPointMatch = new QCheckBox(QString(tr("Point match")), this);
  CHECK_PTR_ENGAUGE(checkPointMatch);
  checkPointMatch->setGeometry(50, 200, 130, 20);
  QWhatsThis::add(checkPointMatch, QString(tr("Select this to save point match settings between sessions")));
  connect(checkPointMatch, SIGNAL(toggled(bool)), this, SLOT(slotPointMatch(bool)));

  checkMeasures = new QCheckBox(QString(tr("Measures")), this);
  CHECK_PTR_ENGAUGE(checkMeasures);
  checkMeasures->setGeometry(210, 50, 130, 20);
  QWhatsThis::add(checkMeasures, QString(tr("Select this to save measures settings between sessions")));
  connect(checkMeasures, SIGNAL(toggled(bool)), this, SLOT(slotMeasures(bool)));

  checkDiscretize = new QCheckBox(QString(tr("Discretize")), this);
  CHECK_PTR_ENGAUGE(checkDiscretize);
  checkDiscretize->setGeometry(210, 80, 130, 20);
  QWhatsThis::add(checkDiscretize, QString(tr("Select this to save discretize settings between sessions")));
  connect(checkDiscretize, SIGNAL(toggled(bool)), this, SLOT(slotDiscretize(bool)));

  checkGridRemoval = new QCheckBox(QString(tr("Grid removal")), this);
  CHECK_PTR_ENGAUGE(checkGridRemoval);
  checkGridRemoval->setGeometry(210, 110, 130, 20);
  QWhatsThis::add(checkGridRemoval, QString(tr("Select this to save grid removal settings between sessions")));
  connect(checkGridRemoval, SIGNAL(toggled(bool)), this, SLOT(slotGridRemoval(bool)));

  checkGridDisplay = new QCheckBox(QString(tr("Grid display")), this);
  CHECK_PTR_ENGAUGE(checkGridDisplay);
  checkGridDisplay->setGeometry(210, 140, 130, 20);
  QWhatsThis::add(checkGridDisplay, QString(tr("Select this to save grid display settings between sessions")));
  connect(checkGridDisplay, SIGNAL(toggled(bool)), this, SLOT(slotGridDisplay(bool)));

  checkExporting = new QCheckBox(QString(tr("Export")), this);
  CHECK_PTR_ENGAUGE(checkExporting);
  checkExporting->setGeometry(210, 170, 130, 20);
  QWhatsThis::add(checkExporting, QString(tr("Select this to save export settings between sessions")));
  connect(checkExporting, SIGNAL(toggled(bool)), this, SLOT(slotExporting(bool)));

  checkViewSelections = new QCheckBox(QString(tr("View selections")), this);
  CHECK_PTR_ENGAUGE(checkViewSelections);
  checkViewSelections->setGeometry(210, 200, 130, 20);
  QWhatsThis::add(checkViewSelections, QString(tr("Select this to save view selections between sessions")));
  connect(checkViewSelections, SIGNAL(toggled(bool)), this, SLOT(slotViewSelections(bool)));

  lblInitialDigitizeState = new QLabel(QString(tr("Initial digitizing state:")), this);
  CHECK_PTR_ENGAUGE(lblInitialDigitizeState);
  lblInitialDigitizeState->setGeometry(30, 240, 160, 20);
  
  cmbInitialDigitizeState = new QComboBox(this);
  CHECK_PTR_ENGAUGE(cmbInitialDigitizeState);
  cmbInitialDigitizeState->setGeometry(210, 240, 130, 20);
  cmbInitialDigitizeState->insertItem(stateName(SelectState)); // states in order of appearance in toolbar
  cmbInitialDigitizeState->insertItem(stateName(AxisState));
  cmbInitialDigitizeState->insertItem(stateName(ScaleState));
  cmbInitialDigitizeState->insertItem(stateName(CurveState));
  cmbInitialDigitizeState->insertItem(stateName(PointMatchState));
  cmbInitialDigitizeState->insertItem(stateName(SegmentState));
  cmbInitialDigitizeState->insertItem(stateName(MeasureState));
  QWhatsThis::add(cmbInitialDigitizeState, QString(tr("Select initial digitizing state after startup")));
  connect(cmbInitialDigitizeState, SIGNAL(activated(const QString&)), this, SLOT(slotInitialDigitizeState(const QString&)));
  
  // navigation buttons
  buttonOk = new QPushButton(tr("OK"), this);
  CHECK_PTR_ENGAUGE(buttonOk);
  buttonOk->setGeometry(30, 280, 100, 30);
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(accept()));

  buttonCancel = new QPushButton(tr("Cancel"), this);
  CHECK_PTR_ENGAUGE(buttonCancel);
  buttonCancel->setGeometry(160, 280, 100, 30);
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));

  buttonWhat = new QPushButton(this);
  CHECK_PTR_ENGAUGE(buttonWhat);
  buttonWhat->setPixmap(QPixmap(whatsthis_xpm));
  buttonWhat->setGeometry(290, 280, 60, 30);
  QToolTip::add(buttonWhat, QString(tr("What's this?")));
  connect(buttonWhat, SIGNAL(clicked()), this, SLOT(slotWhat()));

  toggleActions();
}

SessionsDlg::~SessionsDlg()
{
  DigitDebug::dtor(QString("sessionsdlg ") + QString::number((ulong) this, 16));
}

void SessionsDlg::toggleActions()
{
  ASSERT_ENGAUGE(checkSessions != 0);
  ASSERT_ENGAUGE(checkCoordinates != 0);
  ASSERT_ENGAUGE(checkAxes != 0);
  ASSERT_ENGAUGE(checkScaleBar != 0);
  ASSERT_ENGAUGE(checkCurves != 0);
  ASSERT_ENGAUGE(checkSegments != 0);
  ASSERT_ENGAUGE(checkPointMatch != 0);
  ASSERT_ENGAUGE(checkMeasures != 0);
  ASSERT_ENGAUGE(checkDiscretize != 0);
  ASSERT_ENGAUGE(checkGridRemoval != 0);
  ASSERT_ENGAUGE(checkGridDisplay != 0);
  ASSERT_ENGAUGE(checkExporting != 0);
  ASSERT_ENGAUGE(checkViewSelections != 0);
  ASSERT_ENGAUGE(cmbInitialDigitizeState != 0);
  
  checkSessions->setChecked(m_sessionsSettings->sessions);
  checkCoordinates->setChecked(m_sessionsSettings->coordinates);
  checkAxes->setChecked(m_sessionsSettings->axes);
  checkScaleBar->setChecked(m_sessionsSettings->scaleBar);
  checkCurves->setChecked(m_sessionsSettings->curves);
  checkSegments->setChecked(m_sessionsSettings->segments);
  checkPointMatch->setChecked(m_sessionsSettings->pointMatch);
  checkMeasures->setChecked(m_sessionsSettings->measures);
  checkDiscretize->setChecked(m_sessionsSettings->discretize);
  checkGridRemoval->setChecked(m_sessionsSettings->gridRemoval);
  checkGridDisplay->setChecked(m_sessionsSettings->gridDisplay);
  checkExporting->setChecked(m_sessionsSettings->exporting);
  checkViewSelections->setChecked(m_sessionsSettings->viewSelections);
  cmbInitialDigitizeState->setCurrentText(stateName(m_sessionsSettings->initialDigitizeState));

  checkCoordinates->setEnabled(m_sessionsSettings->sessions);
  checkAxes->setEnabled(m_sessionsSettings->sessions);
  checkScaleBar->setEnabled(m_sessionsSettings->sessions);
  checkCurves->setEnabled(m_sessionsSettings->sessions);
  checkSegments->setEnabled(m_sessionsSettings->sessions);
  checkPointMatch->setEnabled(m_sessionsSettings->sessions);
  checkMeasures->setEnabled(m_sessionsSettings->sessions);
  checkDiscretize->setEnabled(m_sessionsSettings->sessions);
  checkGridRemoval->setEnabled(m_sessionsSettings->sessions);
  checkGridDisplay->setEnabled(m_sessionsSettings->sessions);
  checkExporting->setEnabled(m_sessionsSettings->sessions);
  checkViewSelections->setEnabled(m_sessionsSettings->sessions);
  cmbInitialDigitizeState->setEnabled(m_sessionsSettings->sessions);
}

const QString SessionsDlg::stateName(DigitizeState s)
{
  // compiler will warn if any new states are overlooked here
  switch (s)
  {
  case AxisState:
    return "Axis Points";
  case CurveState:
    return "Curve Points";
  case MeasureState:
    return "Measure Points";
  case PointMatchState:
    return "Point Match";
  case ScaleState:
    return "Scale Bar";
  case SegmentState:
    return "Segment Fill";
  case SelectState:
    return "Select";
  }

  return "";
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void SessionsDlg::slotSessions(bool on)
{
  m_sessionsSettings->sessions = on;
  toggleActions();
}

void SessionsDlg::slotCoordinates(bool on)
{
  m_sessionsSettings->coordinates = on;
}

void SessionsDlg::slotAxes(bool on)
{
  m_sessionsSettings->axes = on;
}

void SessionsDlg::slotScaleBar(bool on)
{
  m_sessionsSettings->scaleBar = on;
}

void SessionsDlg::slotCurves(bool on)
{
  m_sessionsSettings->curves = on;
}

void SessionsDlg::slotSegments(bool on)
{
  m_sessionsSettings->segments = on;
}

void SessionsDlg::slotPointMatch(bool on)
{
  m_sessionsSettings->pointMatch = on;
}

void SessionsDlg::slotMeasures(bool on)
{
  m_sessionsSettings->measures = on;
}

void SessionsDlg::slotDiscretize(bool on)
{
  m_sessionsSettings->discretize = on;
}

void SessionsDlg::slotGridRemoval(bool on)
{
  m_sessionsSettings->gridRemoval = on;
}

void SessionsDlg::slotGridDisplay(bool on)
{
  m_sessionsSettings->gridDisplay = on;
}

void SessionsDlg::slotExporting(bool on)
{
  m_sessionsSettings->exporting = on;
}

void SessionsDlg::slotViewSelections(bool on)
{
  m_sessionsSettings->viewSelections = on;
}

void SessionsDlg::slotInitialDigitizeState(const QString& state)
{
  for (int index = 0;; index++)
  {
    DigitizeState s = (DigitizeState) index;

    if (stateName(s).isEmpty()) // failed to find match
      return;

    if (stateName(s) == state)
    {
      m_sessionsSettings->initialDigitizeState = s;
      return;
    }
  }
}

void SessionsDlg::slotWhat()
{
  QWhatsThis::enterWhatsThisMode();
}
