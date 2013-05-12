/***************************************************************************
                          gridmeshdlg.cpp  -  description
                             -------------------
    begin                : Wed Oct 30 2002
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


#include <qtooltip.h>
//Added by qt3to4:
#include <QPixmap>
#include <QLabel>
#include <QWhatsThis>

#include <math.h>

#include "digitdoc.h"
#include "digitview.h"
#include "gridmeshdlg.h"
#include "img/whatsthis.xpm"
#include "digitdebug.h"
#include "main.h"
#include "defaultsettings.h"

GridMeshDlg::GridMeshDlg(QString gridType, QString title,
  CoordSettings* coordSettings, GridMeshSettings* gridSettings) :
  QDialog(0, "gridmeshdlg", true),
  coordSettings(coordSettings),
  gridSettings(gridSettings)
{
  DigitDebug::ctor(QString("gridmeshdlg ") + QString::number((ulong) this, 16));
  
  setCaption(tr("Grid Mesh %1 - [%2]%3")
    .arg(gridType)
    .arg(title)
    .arg(freewareCaption));

  setFixedSize(470, 280);

  // x grid lines
  groupX = new Q3GroupBox(QString(tr("X Grid Lines")), this);
  CHECK_PTR_ENGAUGE(groupX);
  groupX->setGeometry(10, 10, 220, 220);

  labelDisableX = new QLabel(QString(tr("Disable")), groupX);
  CHECK_PTR_ENGAUGE(labelDisableX);
  labelDisableX->setGeometry(10, 20, 100, 30);

  comboDisableX = new QComboBox(this, groupX, tr("Disabled X Value"));
  CHECK_PTR_ENGAUGE(comboDisableX);
  comboDisableX->setGeometry(110, 20, 100, 30);
  comboDisableX->setEditable(false);
  comboDisableX->insertStringList(GridSetStrings);
  QWhatsThis::add(comboDisableX, QString(tr("Disabled value.\n\nThe X grid lines are specified "
    "using only three values at a time. For flexibility, four values are offered so you must chose "
    "which value is disabled. Once disabled, that value is simply updated as the other values change")));
  connect(comboDisableX, SIGNAL(activated(int)), this, SLOT(slotDisableX(int)));

  labelCountX = new QLabel(QString(tr("X Count")), groupX);
  CHECK_PTR_ENGAUGE(labelCountX);
  labelCountX->setGeometry(10, 60, 100, 30);

  editCountX = new QLineEdit(groupX);
  CHECK_PTR_ENGAUGE(editCountX);
  editCountX->setGeometry(110, 60, 100, 30);
  ASSERT_ENGAUGE(gridSettings != 0);
  if (gridSettings->initialized)
    editCountX->setText(QString("%1").arg(gridSettings->countX));
  QWhatsThis::add(editCountX, QString(tr("Number of X grid lines.\n\nThe number of X grid lines must "
    "be entered as an integer greater than zero")));
  validatorCountX = new QIntValidator(editCountX);
  CHECK_PTR_ENGAUGE(validatorCountX);
  editCountX->setValidator(validatorCountX);
  connect(editCountX, SIGNAL(textChanged(const QString &)), this, SLOT(slotCountX(const QString &)));

  labelStartX = new QLabel(QString(tr("X Start")), groupX);
  CHECK_PTR_ENGAUGE(labelStartX);
  labelStartX->setGeometry(10, 100, 100, 30);

  editStartX = new QLineEdit(groupX);
  CHECK_PTR_ENGAUGE(editStartX);
  editStartX->setGeometry(110, 100, 100, 30);
  ASSERT_ENGAUGE(gridSettings != 0);
  if (gridSettings->initialized)
    editStartX->setText(QString("%1").arg(gridSettings->startX));
  QWhatsThis::add(editStartX, QString(tr("Value of the first X grid line.\n\nThe start value cannot "
    "be greater than the stop value")));
  validatorStartX = new QDoubleValidator(editStartX);
  CHECK_PTR_ENGAUGE(validatorStartX);
  editStartX->setValidator(validatorStartX);
  connect(editStartX, SIGNAL(textChanged(const QString &)), this, SLOT(slotStartX(const QString &)));

  labelStepX = new QLabel(QString(tr("X Step")), groupX);
  CHECK_PTR_ENGAUGE(labelStepX);
  labelStepX->setGeometry(10, 140, 100, 30);

  editStepX = new QLineEdit(groupX);
  CHECK_PTR_ENGAUGE(editStepX);
  editStepX->setGeometry(110, 140, 100, 30);
  ASSERT_ENGAUGE(gridSettings != 0);
  if (gridSettings->initialized)
    editStepX->setText(QString("%1").arg(gridSettings->stepX));
  QWhatsThis::add(editStepX, QString(tr("Difference in value between two successive X grid lines.\n\n"
    "The step value must be greater than zero")));
  validatorStepX = new QDoubleValidator(editStepX);
  CHECK_PTR_ENGAUGE(validatorStepX);
  editStepX->setValidator(validatorStepX);
  connect(editStepX, SIGNAL(textChanged(const QString &)), this, SLOT(slotStepX(const QString &)));

  labelStopX = new QLabel(QString(tr("X Stop")), groupX);
  CHECK_PTR_ENGAUGE(labelStopX);
  labelStopX->setGeometry(10, 180, 100, 30);

  editStopX = new QLineEdit(groupX);
  CHECK_PTR_ENGAUGE(editStopX);
  editStopX->setGeometry(110, 180, 100, 30);
  ASSERT_ENGAUGE(gridSettings != 0);
  if (gridSettings->initialized)
    editStopX->setText(QString("%1").arg(gridSettings->stopX));
  QWhatsThis::add(editStopX, QString(tr("Value of the last X grid line.\n\nThe stop value cannot "
    "be less than the start value")));
  validatorStopX = new QDoubleValidator(editStopX);
  CHECK_PTR_ENGAUGE(validatorStopX);
  editStopX->setValidator(validatorStopX);
  connect(editStopX, SIGNAL(textChanged(const QString &)), this, SLOT(slotStopX(const QString &)));

  // y grid lines
  groupY = new Q3GroupBox(QString(tr("Y Grid Lines")), this);
  CHECK_PTR_ENGAUGE(groupY);
  groupY->setGeometry(240, 10, 220, 220);

  labelDisableY = new QLabel(QString(tr("Disable")), groupY);
  CHECK_PTR_ENGAUGE(labelDisableY);
  labelDisableY->setGeometry(10, 20, 100, 30);

  comboDisableY = new QComboBox(false, groupY, tr("Disabled Y Value"));
  CHECK_PTR_ENGAUGE(comboDisableY);
  comboDisableY->setGeometry(110, 20, 100, 30);
  comboDisableY->setEditable(false);
  comboDisableY->insertStringList(GridSetStrings);
  QWhatsThis::add(comboDisableY, QString(tr("Disabled value.\n\nThe Y grid lines are specified "
    "using only three values at a time. For flexibility, four values are offered so you must chose "
    "which value is disabled. Once disabled, that value is simply updated as the other values change")));
  connect(comboDisableY, SIGNAL(activated(int)), this, SLOT(slotDisableY(int)));

  labelCountY = new QLabel(QString(tr("Y Count")), groupY);
  CHECK_PTR_ENGAUGE(labelCountY);
  labelCountY->setGeometry(10, 60, 100, 30);

  editCountY = new QLineEdit(groupY);
  CHECK_PTR_ENGAUGE(editCountY);
  editCountY->setGeometry(110, 60, 100, 30);
  ASSERT_ENGAUGE(gridSettings != 0);
  if (gridSettings->initialized)
    editCountY->setText(QString("%1").arg(gridSettings->countY));
  QWhatsThis::add(editCountY, QString(tr("Number of Y grid lines.\n\nThe number of X grid lines must "
    "be entered as an integer greater than zero")));
  validatorCountY = new QIntValidator(editCountY);
  CHECK_PTR_ENGAUGE(validatorCountY);
  editCountY->setValidator(validatorCountY);
  connect(editCountY, SIGNAL(textChanged(const QString &)), this, SLOT(slotCountY(const QString &)));

  labelStartY = new QLabel(QString(tr("Y Start")), groupY);
  CHECK_PTR_ENGAUGE(labelStartY);
  labelStartY->setGeometry(10, 100, 100, 30);

  editStartY = new QLineEdit(groupY);
  CHECK_PTR_ENGAUGE(editStartY);
  editStartY->setGeometry(110, 100, 100, 30);
  ASSERT_ENGAUGE(gridSettings != 0);
  if (gridSettings->initialized)
    editStartY->setText(QString("%1").arg(gridSettings->startY));
  QWhatsThis::add(editStartY, QString(tr("Value of the first Y grid line.\n\nThe start value cannot "
    "be greater than the stop value")));
  validatorStartY = new QDoubleValidator(editStartY);
  CHECK_PTR_ENGAUGE(validatorStartY);
  editStartY->setValidator(validatorStartY);
  connect(editStartY, SIGNAL(textChanged(const QString &)), this, SLOT(slotStartY(const QString &)));

  labelStepY = new QLabel(QString(tr("Y Step")), groupY);
  CHECK_PTR_ENGAUGE(labelStepY);
  labelStepY->setGeometry(10, 140, 100, 30);

  editStepY = new QLineEdit(groupY);
  CHECK_PTR_ENGAUGE(editStepY);
  editStepY->setGeometry(110, 140, 100, 30);
  ASSERT_ENGAUGE(gridSettings != 0);
  if (gridSettings->initialized)
    editStepY->setText(QString("%1").arg(gridSettings->stepY));
  QWhatsThis::add(editStepY, QString(tr("Difference in value between two successive Y grid lines.\n\n"
    "The step value must be greater than zero")));
  validatorStepY = new QDoubleValidator(editStepY);
  CHECK_PTR_ENGAUGE(validatorStepY);
  editStepY->setValidator(validatorStepY);
  connect(editStepY, SIGNAL(textChanged(const QString &)), this, SLOT(slotStepY(const QString &)));

  labelStopY = new QLabel(QString(tr("Y Stop")), groupY);
  CHECK_PTR_ENGAUGE(labelStopY);
  labelStopY->setGeometry(10, 180, 100, 30);

  editStopY = new QLineEdit(groupY);
  CHECK_PTR_ENGAUGE(editStopY);
  editStopY->setGeometry(110, 180, 100, 30);
  ASSERT_ENGAUGE(gridSettings != 0);
  if (gridSettings->initialized)
    editStopY->setText(QString("%1").arg(gridSettings->stopY));
  QWhatsThis::add(editStopY, QString(tr("Value of the last Y grid line.\n\nThe stop value cannot "
    "be less than the start value")));
  validatorStopY = new QDoubleValidator(editStopY);
  CHECK_PTR_ENGAUGE(validatorStopY);
  editStopY->setValidator(validatorStopY);
  connect(editStopY, SIGNAL(textChanged(const QString &)), this, SLOT(slotStopY(const QString &)));

  // navigation buttons
  buttonOk = new QPushButton(tr("OK"), this);
  CHECK_PTR_ENGAUGE(buttonOk);
  buttonOk->setGeometry(120, 240, 100, 30);
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(accept()));

  buttonCancel = new QPushButton(tr("Cancel"), this);
  CHECK_PTR_ENGAUGE(buttonCancel);
  buttonCancel->setGeometry(250, 240, 100, 30);
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));

  buttonWhat = new QPushButton(this);
  CHECK_PTR_ENGAUGE(buttonWhat);
  buttonWhat->setPixmap(QPixmap(whatsthis_xpm));
  buttonWhat->setGeometry(400, 240, 60, 30);
  QToolTip::add(buttonWhat, QString(tr("What's this?")));
  connect(buttonWhat, SIGNAL(clicked()), this, SLOT(slotWhat()));

  toggleActions();
}

GridMeshDlg::~GridMeshDlg()
{
  DigitDebug::dtor(QString("gridmeshdlg ") + QString::number((ulong) this, 16));
}

void GridMeshDlg::computeDisabledValue(bool linear, GridSet gridSet, int* count, double* start,
  double* step, double* stop, QLineEdit* editCount, QLineEdit* editStart, QLineEdit* editStep,
  QLineEdit* editStop)
{
  // validators prevent divide by zero errors and negative log errors from occurring below
  switch (gridSet)
  {
    case AllButCount:
      if (linear)
        *count = 1 + (int) (0.5 + (*stop - *start) / *step);
      else
        *count = 1 + (int) ((log(*stop) - log(*start)) / log(*step));
      ASSERT_ENGAUGE(editCount != 0);
      editCount->setText(QString("%1").arg(*count));
      break;
    case AllButStart:
      if (linear)
        *start = *stop - (*count - 1.0) * *step;
      else
        *start = exp(log(*stop) - (*count - 1.0) * log(*step));
      ASSERT_ENGAUGE(editStart != 0);
      editStart->setText(QString("%1").arg(*start));
      break;
    case AllButStep:
      if (linear)
        *step = (*stop - *start) / (*count - 1.0);
      else
        *step = exp((log(*stop) - log(*start)) / (*count - 1.0));
      ASSERT_ENGAUGE(editStep != 0);
      editStep->setText(QString("%1").arg(*step));
      break;
    case AllButStop:
      if (linear)
        *stop = *start + (*count - 1.0) * *step;
      else
        *stop = exp(log(*start) + (*count - 1.0) * log(*step));
      ASSERT_ENGAUGE(editStop != 0);
      editStop->setText(QString("%1").arg(*stop));
      break;
  }
}

void GridMeshDlg::toggleActions()
{
  ASSERT_ENGAUGE(gridSettings != 0);
  
  ASSERT_ENGAUGE(comboDisableX != 0);
  comboDisableX->setCurrentText(GridSetStrings [gridSettings->gridSetX]);
  ASSERT_ENGAUGE(comboDisableY != 0);
  comboDisableY->setCurrentText(GridSetStrings [gridSettings->gridSetY]);

  if (validateSettingsX())
  {
    // automatically fill in the missing setting
    ASSERT_ENGAUGE(coordSettings != 0);
    ASSERT_ENGAUGE(gridSettings != 0);
    computeDisabledValue(coordSettings->xThetaScale == Linear, gridSettings->gridSetX,
      &gridSettings->countX, &gridSettings->startX, &gridSettings->stepX, &gridSettings->stopX,
      editCountX, editStartX, editStepX, editStopX);
  }

  if (validateSettingsY())
  {
    // automatically fill in the missing setting
    ASSERT_ENGAUGE(coordSettings != 0);
    ASSERT_ENGAUGE(gridSettings != 0);  
    computeDisabledValue(coordSettings->yRScale == Linear, gridSettings->gridSetY,
      &gridSettings->countY, &gridSettings->startY, &gridSettings->stepY, &gridSettings->stopY,
      editCountY, editStartY, editStepY, editStopY);
  }

  if (validateSettingsX() && validateSettingsY())
  {
    // settings are now initialized. we do not need to worry about the case where x settings
    // are set but y are not, or vice versa, since user cannot click on OK until both
    // x and y settings are all set
    gridSettings->initialized = true;
  }
  
  // enable appropriate edit boxes
  ASSERT_ENGAUGE(editCountX != 0);
  editCountX->setEnabled(gridSettings->gridSetX != AllButCount);
  ASSERT_ENGAUGE(editStartX != 0);
  editStartX->setEnabled(gridSettings->gridSetX != AllButStart);
  ASSERT_ENGAUGE(editStepX != 0);
  editStepX->setEnabled(gridSettings->gridSetX != AllButStep);
  ASSERT_ENGAUGE(editStopX != 0);
  editStopX->setEnabled(gridSettings->gridSetX != AllButStop);

  ASSERT_ENGAUGE(editCountY != 0);
  editCountY->setEnabled(gridSettings->gridSetY != AllButCount);
  ASSERT_ENGAUGE(editStartY != 0);
  editStartY->setEnabled(gridSettings->gridSetY != AllButStart);
  ASSERT_ENGAUGE(editStepX != 0);
  editStepY->setEnabled(gridSettings->gridSetY != AllButStep);
  ASSERT_ENGAUGE(editStopY != 0);
  editStopY->setEnabled(gridSettings->gridSetY != AllButStop);

  ASSERT_ENGAUGE(buttonOk != 0);
  buttonOk->setEnabled(validateSettingsX() && validateSettingsY());
}

bool GridMeshDlg::validateSettings(Scale scale, GridSet gridSet,
  QString textCount, QString textStart, QString textStep, QString textStop,
  int count, double start, double step, double stop)
{
  // enforce constraints, but only on enabled settings since the disabled setting
  // will immediately be fixed automatically

  // (1) the fields other than the gridSet field cannot be blank
  if ((textCount.length() == 0) && (gridSet != AllButCount))
    return false;
  if ((textStart.length() == 0) && (gridSet != AllButStart))
    return false;
  if ((textStep.length() == 0) && (gridSet != AllButStep))
    return false;
  if ((textStop.length() == 0) && (gridSet != AllButStop))
    return false;

  // (2) need count values greater than one to eliminate divide by zero
  // (3) need count values greater than zero since we are forcing start<stop and step>0
  if ((count < 2) && (gridSet != AllButCount))
    return false;
  // (4) need start values less than or equal to stop values
  if ((start > stop) && (gridSet != AllButStart) && (gridSet != AllButStop))
    return false;
  // (5) need start values greater than zero for log coordinates to eliminate log of negative value
  if (scale == Log)
    if ((start <= 0.0) && (gridSet != AllButStart))
      return false;
  // (6) need nonzero step values to eliminate divide by zero
  // (7) need step values greater than zero for log coordinates to eliminate log of negative value
  // (8) need step values greater than zero to eliminate infinite loops (loops in the wrong direction)
  if ((step < DefaultSettings::instance().getDoubleMin()) && (gridSet != AllButStep))
    return false;

  return true;
}

bool GridMeshDlg::validateSettingsX()
{
  return validateSettings(
    coordSettings->xThetaScale,
    gridSettings->gridSetX,
    editCountX->text(),
    editStartX->text(),
    editStepX->text(),
    editStopX->text(),
    gridSettings->countX,
    gridSettings->startX,
    gridSettings->stepX,
    gridSettings->stopX);
}

bool GridMeshDlg::validateSettingsY()
{
  return validateSettings(
    coordSettings->yRScale,
    gridSettings->gridSetY,
    editCountY->text(),
    editStartY->text(),
    editStepY->text(),
    editStopY->text(),
    gridSettings->countY,
    gridSettings->startY,
    gridSettings->stepY,
    gridSettings->stopY);
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void GridMeshDlg::slotDisableX(int index)
{
  ASSERT_ENGAUGE(gridSettings != 0);
  if ((GridSet) index != gridSettings->gridSetX)
  {
    gridSettings->gridSetX = (GridSet) index;
    toggleActions();
  }
}

void GridMeshDlg::slotDisableY(int index)
{
  ASSERT_ENGAUGE(gridSettings != 0);
  if ((GridSet) index != gridSettings->gridSetY)
  {
    gridSettings->gridSetY = (GridSet) index;
    toggleActions();
  }
}

void GridMeshDlg::slotCountX(const QString &text)
{
  ASSERT_ENGAUGE(gridSettings != 0);
  if (text == QString(""))
    gridSettings->countX = 0;
  else
    gridSettings->countX = text.toInt();
  toggleActions();
}

void GridMeshDlg::slotCountY(const QString &text)
{
  ASSERT_ENGAUGE(gridSettings != 0);
  if (text == QString(""))
    gridSettings->countY = 0;
  else
    gridSettings->countY = text.toInt();
  toggleActions();
}

void GridMeshDlg::slotStartX(const QString &text)
{
  ASSERT_ENGAUGE(gridSettings != 0);
  if (text == QString(""))
    gridSettings->startX = 0.0;
  else
    gridSettings->startX = text.toDouble();
  toggleActions();
}

void GridMeshDlg::slotStartY(const QString &text)
{
  ASSERT_ENGAUGE(gridSettings != 0);
  if (text == QString(""))
    gridSettings->startY = 0.0;
  else
    gridSettings->startY = text.toDouble();
  toggleActions();
}

void GridMeshDlg::slotStepX(const QString &text)
{
  ASSERT_ENGAUGE(gridSettings != 0);
  if (text == QString(""))
    gridSettings->stepX = 0.0;
  else
    gridSettings->stepX = text.toDouble();
  toggleActions();
}

void GridMeshDlg::slotStepY(const QString &text)
{
  ASSERT_ENGAUGE(gridSettings != 0);
  if (text == QString(""))
    gridSettings->stepY = 0.0;
  else
    gridSettings->stepY = text.toDouble();
  toggleActions();
}

void GridMeshDlg::slotStopX(const QString &text)
{
  ASSERT_ENGAUGE(gridSettings != 0);
  if (text == QString(""))
    gridSettings->stopX = 0.0;
  else
    gridSettings->stopX = text.toDouble();
  toggleActions();
}

void GridMeshDlg::slotStopY(const QString &text)
{
  ASSERT_ENGAUGE(gridSettings != 0);
  if (text == QString(""))
    gridSettings->stopY = 0.0;
  else
    gridSettings->stopY = text.toDouble();
  toggleActions();
}

void GridMeshDlg::slotWhat()
{
  QWhatsThis::enterWhatsThisMode();
}
