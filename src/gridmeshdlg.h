/***************************************************************************
                          gridmeshdlg.h  -  description
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

#ifndef GRIDMESHDLG_H
#define GRIDMESHDLG_H

#include <qdialog.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <q3groupbox.h>
#include <qlineedit.h>
#include <qstring.h>
#include <qvalidator.h>
#include <qcombobox.h>

#include "digitdef.h"

class DigitDoc;

// this dialog box class edits the document's grid information 
class GridMeshDlg : public QDialog
{
  Q_OBJECT
  
  public:
  
    GridMeshDlg(QString gridType, QString title,
      CoordSettings* coordSettings, GridMeshSettings* gridSettings);
    ~GridMeshDlg();

  public slots:

    void slotDisableX(int index);
    void slotDisableY(int index);
    void slotCountX(const QString &text);
    void slotCountY(const QString &text);
    void slotStartX(const QString &text);
    void slotStartY(const QString &text);
    void slotStepX(const QString &text);
    void slotStepY(const QString &text);
    void slotStopX(const QString &text);
    void slotStopY(const QString &text);
    void slotWhat(void);

  private:

    // enable or disable buttons as appropriate
    void toggleActions(void);

    // return true if all settings constraints are currently satisfied. setBottom and setTop
    // in the validators were not used since the validators do not distinguish Intermediate
    // and Acceptable externally
    bool validateSettings(Scale scale, GridSet gridSet,
      QString textCount, QString textStart, QString textStep, QString textStop,
      int count, double start, double step, double stop);
    bool validateSettingsX(void);
    bool validateSettingsY(void);

    // given valid new values for three settings, compute the fourth
    void computeDisabledValue(bool linear, GridSet gridSet, int* count, double* start,
      double* step, double* stop, QLineEdit* editCount, QLineEdit* editStart, QLineEdit* editStep,
      QLineEdit* editStop);

    CoordSettings* coordSettings;
    GridMeshSettings* gridSettings;

    Q3GroupBox* groupX;
    Q3GroupBox* groupY;
    
    QLabel* labelDisableX;
    QComboBox* comboDisableX;
    
    QLabel* labelDisableY;
    QComboBox* comboDisableY;

    QLabel* labelCountX;
    QLineEdit* editCountX;
    QIntValidator* validatorCountX;

    QLabel* labelCountY;
    QLineEdit* editCountY;
    QIntValidator* validatorCountY;

    QLabel* labelStartX;
    QLineEdit* editStartX;
    QDoubleValidator* validatorStartX;

    QLabel* labelStartY;
    QLineEdit* editStartY;
    QDoubleValidator* validatorStartY;
    
    QLabel* labelStopX;
    QLineEdit* editStopX;
    QDoubleValidator* validatorStopX;

    QLabel* labelStopY;
    QLineEdit* editStopY;
    QDoubleValidator* validatorStopY;
    
    QLabel* labelStepX;
    QLineEdit* editStepX;
    QDoubleValidator* validatorStepX;

    QLabel* labelStepY;
    QLineEdit* editStepY;
    QDoubleValidator* validatorStepY;

    // navigation buttons
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QPushButton* buttonWhat;
};

#endif // GRIDMESHDLG_H
