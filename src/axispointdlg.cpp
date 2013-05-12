/***************************************************************************
                          axispointdlg.cpp  -  description
                             -------------------
    begin                : Fri Nov 1 2002
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

#include <stdio.h>

#include <qmessagebox.h>
//Added by qt3to4:
#include <QLabel>

#include "axispointdlg.h"
#include "digitdoc.h"
#include "digitview.h"
#include "digitdebug.h"
#include "main.h"
#include "defaultsettings.h"

AxisPointDlg::AxisPointDlg(DigitDoc* doc, double x, double y) :
  QDialog(0, "axispointdlg", true),
  doc(doc)
{
  DigitDebug::ctor(QString("axispointdlg " + QString::number((ulong) this, 16)));
  
  ASSERT_ENGAUGE(doc != 0);
  setCaption(tr("Axis Point - [%1]%2")
    .arg(doc->title())
    .arg(freewareCaption));
  
  setFixedSize(250, 130);

  labelX = new QLabel(tr("X or Theta:"), this);
  CHECK_PTR_ENGAUGE(labelX);
  labelX->setGeometry(10, 10, 110, 30);

  labelY = new QLabel(tr("Y or R:"), this);
  CHECK_PTR_ENGAUGE(labelY);
  labelY->setGeometry(10, 50, 110, 30);

  editX = new QLineEdit(QString::number(x), this);
  CHECK_PTR_ENGAUGE(editX);
  editX->setGeometry(140, 10, 100, 30);
  doubleValidatorX = new QDoubleValidator(editX);
  CHECK_PTR_ENGAUGE(doubleValidatorX);
  editX->setValidator(doubleValidatorX);
  
  editY = new QLineEdit(QString::number(y), this);
  CHECK_PTR_ENGAUGE(editY);
  editY->setGeometry(140, 50, 100, 30);
  doubleValidatorY = new QDoubleValidator(editY);
  CHECK_PTR_ENGAUGE(doubleValidatorY);
  editY->setValidator(doubleValidatorY);

  buttonOk = new QPushButton(tr("OK"), this);
  CHECK_PTR_ENGAUGE(buttonOk);
  buttonOk->setGeometry(10, 90, 100, 30);
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(slotAccept()));

  buttonCancel = new QPushButton(tr("Cancel"), this);
  CHECK_PTR_ENGAUGE(buttonCancel);
  buttonCancel->setGeometry(140, 90, 100, 30);
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
}

AxisPointDlg::~AxisPointDlg()
{
  DigitDebug::dtor(QString("axispointdlg " + QString::number((ulong) this, 16)));
}

double AxisPointDlg::x(void)
{
  double value;
  sscanf(editX->text(), "%lg", &value);
  return value;
}

double AxisPointDlg::y(void)
{
  double value;
  sscanf(editY->text(), "%lg", &value);
  return value;
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void AxisPointDlg::slotAccept()
{
  double doubleMin = DefaultSettings::instance().getDoubleMin();
  if (((x() != 0.0) && (dabs(x()) < doubleMin)) ||
    ((y() != 0.0) && (dabs(y()) < doubleMin)))
  {
    if (QMessageBox::No ==
      QMessageBox::warning(this, tr("Coordinate warning!"),
      tr("A coordinate's magnitude is smaller than the critical value of %1!\n"
      "This may cause problems later. You may continue, or return to modify \n"
      "the value. Hint - you might consider scaling the coordinates up by some \n"
      "power of ten.\n\n"
      "Do you want to continue?").arg(doubleMin), QMessageBox::Yes, QMessageBox::No))
    {
        // do not finish yet. let user edit points
        return;
    }
  }

  // done editing points
  accept();
}
