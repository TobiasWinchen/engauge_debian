/***************************************************************************
                          sessionsdlg.h  -  description
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

#ifndef SESSIONSDLG_H
#define SESSIONSDLG_H

#include <qdialog.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qcombobox.h>

#include "digitdef.h"

// this dialog box lets user control the saving of settings between sessions
class SessionsDlg : public QDialog
{
  Q_OBJECT
    
  public:

    SessionsDlg(SessionsSettings* sessionSettings);
    ~SessionsDlg();

  public slots:

    void slotSessions(bool on);
    void slotCoordinates(bool on);
    void slotAxes(bool on);
    void slotScaleBar(bool on);
    void slotCurves(bool on);
    void slotSegments(bool on);
    void slotPointMatch(bool on);
    void slotMeasures(bool on);
    void slotDiscretize(bool on);
    void slotGridRemoval(bool on);
    void slotGridDisplay(bool on);
    void slotExporting(bool on);
    void slotViewSelections(bool on);
    void slotInitialDigitizeState(const QString& state);
    void slotWhat(void);

  private:

    // enable or disable buttons as appropriate
    void toggleActions(void);

    // displayed name for particular state
    const QString stateName(DigitizeState s);

    // checkboxes control which settings, if any, are saved
    QCheckBox* checkSessions;
    QCheckBox* checkCoordinates;
    QCheckBox* checkAxes;
    QCheckBox* checkScaleBar;
    QCheckBox* checkCurves;
    QCheckBox* checkSegments;
    QCheckBox* checkPointMatch;
    QCheckBox* checkMeasures;
    QCheckBox* checkDiscretize;
    QCheckBox* checkGridRemoval;
    QCheckBox* checkGridDisplay;
    QCheckBox* checkExporting;
    QCheckBox* checkViewSelections;

    // combobox is used to choose initial state
    QLabel* lblInitialDigitizeState;
    QComboBox* cmbInitialDigitizeState;

    // user selections
    SessionsSettings* m_sessionsSettings;
    
    // navigation buttons
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QPushButton* buttonWhat;
};

#endif // SESSIONSDLG_H
