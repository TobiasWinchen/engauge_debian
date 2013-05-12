/***************************************************************************
                          exportdlg.h  -  description
                             -------------------
    begin                : Fri Nov 15 2002
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

#ifndef EXPORTDLG_H
#define EXPORTDLG_H

#include <qdialog.h>
#include <qstring.h>
#include <q3groupbox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlabel.h>
#include <q3listbox.h>
#include <q3buttongroup.h>

#include "digitdef.h"

class PointSets;

// this dialog box edits the export settings
class ExportDlg : public QDialog
{
  Q_OBJECT
  
  public: 

    ExportDlg(QString title, PointSets* pointSets, const CoordSettings* coord,
      const GridMeshSettings* grid, ExportSettings* xport);
    ~ExportDlg();

  public slots:

    void slotIncluded(Q3ListBoxItem* item);
    void slotExcluded(Q3ListBoxItem* item);
    void slotInclude(void);
    void slotExclude(void);
    void slotXAllCurves(bool toggle);
    void slotXFirstCurve(bool toggle);
    void slotXGridLines(bool toggle);
    void slotXYAllCurves(bool toggle);
    void slotAllCurvesOnEachLine(bool toggle);
    void slotOneCurveOnEachLine(bool toggle);
    void slotCommas(bool toggle);
    void slotSpaces(bool toggle);
    void slotTabs(bool toggle);
    void slotNone(bool toggle);
    void slotSimple(bool toggle);
    void slotGnuplot(bool toggle);
    void slotWhat(void);

  private:

    PointSets* pointSets;

    // reset included and excluded lists
    void resetLists(void);

    // enable or disable buttons as appropriate
    void toggleActions(void);

    // update preview after modifying a property
    void updatePreview(void);

    Q3GroupBox* groupCurveSelection;
    QLabel* labelIncluded;
    QLabel* labelExcluded;
    Q3ListBox* listIncluded;
    Q3ListBox* listExcluded;
    QPushButton* buttonInclude;
    QPushButton* buttonExclude;

    Q3ButtonGroup* groupPointsSelection;
    QRadioButton* buttonXAllCurves;
    QRadioButton* buttonXFirstCurve;
    QRadioButton* buttonXGridLines;
    QRadioButton* buttonXYAllCurves;

    Q3ButtonGroup* groupLayout;
    QRadioButton* buttonAllCurvesOnEachLine;
    QRadioButton* buttonOneCurveOnEachLine;

    Q3ButtonGroup* groupDelimiters;
    QRadioButton* buttonCommas;
    QRadioButton* buttonSpaces;
    QRadioButton* buttonTabs;

    Q3ButtonGroup* groupHeader;
    QRadioButton* buttonNone;
    QRadioButton* buttonSimple;
    QRadioButton* buttonGnuplot;

    // immediately update the preview screen after each setting change, so user can
    // see the effects. if it turns out the user is blocked for an unacceptably long
    // time, then the update calculations should have processEvents calls inserted. if
    // another setting change occurs before the preview update finishes, a new
    // preview update can be started
    QLabel* labelPreview;
    Q3ListBox* listPreview;

    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QPushButton* buttonWhat;

    const CoordSettings* coord;
    const GridMeshSettings* grid;
    ExportSettings* xport;
};

#endif
