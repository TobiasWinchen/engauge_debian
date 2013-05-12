/***************************************************************************
                          pointsetnamedlg.cpp  -  description
                             -------------------
    begin                : Fri Nov 8 2002
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

#include <qmessagebox.h>
#include <qregexp.h>
//Added by qt3to4:
#include <QLabel>

#include "pointsetnamedlg.h"
#include "digitdebug.h"
#include "digitdef.h"
#include "main.h"

PointSetNameDlg::PointSetNameDlg(bool curve, QString title, QString* name, QStringList disallowedNames) :
  QDialog(0, "pointsetnamedlg", true),
  curve(curve),
  title(title),
  name(name),
  disallowedNames(disallowedNames)
{
  DigitDebug::ctor(QString("pointsetnamedlg ") + QString::number((ulong) this, 16));
  
  setCaption(curveReplace("Curve Name - [%1]%2")
    .arg(title)
    .arg(freewareCaption));
      
  setFixedSize(250, 100);

  labelName = new QLabel(QString(tr("Name:")), this);
  CHECK_PTR_ENGAUGE(labelName);
  labelName->setGeometry(10, 10, 100, 30);

  editName = new QLineEdit(*name, this);
  CHECK_PTR_ENGAUGE(editName);
  editName->setGeometry(140, 10, 100, 30);
  connect(editName, SIGNAL(textChanged(const QString &)), this, SLOT(slotTextChanged(const QString &)));

  buttonOk = new QPushButton(tr("OK"), this);
  CHECK_PTR_ENGAUGE(buttonOk);
  buttonOk->setGeometry(10, 60, 100, 30);
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(accept()));

  buttonCancel = new QPushButton(tr("Cancel"), this);
  CHECK_PTR_ENGAUGE(buttonCancel);
  buttonCancel->setGeometry(140, 60, 100, 30);
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));

  toggleActions();
}

PointSetNameDlg::~PointSetNameDlg()
{
  DigitDebug::dtor(QString("pointsetnamedlg ") + QString::number((ulong) this, 16));
}

void PointSetNameDlg::toggleActions()
{
  bool enable = true;
  ASSERT_ENGAUGE(editName != 0);
  if (editName->text() == QString(""))
    enable = false;
  else if (disallowedNames.find(editName->text()) != disallowedNames.end())
    enable = false;

  ASSERT_ENGAUGE(buttonOk != 0);
  buttonOk->setEnabled(enable);
}

QString PointSetNameDlg::curveReplace(const char* text)
{
  if (curve)
    return QString(tr(text));
  else
  {
    QString rtn(text);
    rtn.replace(QRegExp("curve"), QString("measure"));
    rtn.replace(QRegExp("Curve"), QString("Measure"));
    return rtn;
  }
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void PointSetNameDlg::slotTextChanged(const QString &text)
{
  *name = text;

  toggleActions();
}
