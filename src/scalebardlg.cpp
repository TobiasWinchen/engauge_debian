/***************************************************************************
                          scalebardlg.cpp  -  description
                             -------------------
    begin                : Tue Dec 31 2002
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

#include "scalebardlg.h"
#include "digitdoc.h"
#include "digitview.h"
#include "digitdebug.h"
#include "main.h"
//Added by qt3to4:
#include <QLabel>

ScaleBarDlg::ScaleBarDlg(DigitDoc* doc, double length) :
  QDialog(0, "scalebardlg", true),
  doc(doc)
{
  DigitDebug::ctor(QString("scalebardlg " + QString::number((ulong) this, 16)));
  
  ASSERT_ENGAUGE(doc != 0);
  setCaption(tr("Scale Bar - [%1]%2")
    .arg(doc->title())
    .arg(freewareCaption));

  setFixedSize(250, 90);

  labelLength = new QLabel(tr("Length:"), this);
  CHECK_PTR_ENGAUGE(labelLength);
  labelLength->setGeometry(10, 10, 110, 30);

  editLength = new QLineEdit(QString::number(length), this);
  CHECK_PTR_ENGAUGE(editLength);
  editLength->setGeometry(140, 10, 100, 30);
  doubleValidator = new QDoubleValidator(editLength);
  CHECK_PTR_ENGAUGE(doubleValidator);
  editLength->setValidator(doubleValidator);

  buttonOk = new QPushButton(tr("OK"), this);
  CHECK_PTR_ENGAUGE(buttonOk);
  buttonOk->setGeometry(10, 50, 100, 30);
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(accept()));

  buttonCancel = new QPushButton(tr("Cancel"), this);
  CHECK_PTR_ENGAUGE(buttonCancel);
  buttonCancel->setGeometry(140, 50, 100, 30);
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
}

ScaleBarDlg::~ScaleBarDlg()
{
  DigitDebug::dtor(QString("scalebardlg " + QString::number((ulong) this, 16)));
}
