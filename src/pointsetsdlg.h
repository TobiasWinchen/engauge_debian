/***************************************************************************
                          pointsetsdlg.h  -  description
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

#ifndef POINTSETSDLG_H
#define POINTSETSDLG_H

#include <qobject.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <q3listbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <q3groupbox.h>

#include "pointset.h"

const QString NoActivePointSet = QString("");

class PointSet;
class PointSets;

// this dialog box class edits one of the documents pointset lists
class PointSetsDlg : public QDialog
{
  Q_OBJECT
  
  public:
  
    PointSetsDlg(bool curve, QString title, PointSets* pointSets, QStringList* pointSetsWithOrphanedPoints);
    ~PointSetsDlg();

  public slots:

    void slotDown(void);
    void slotUp(void);
    void slotNew(void);
    void slotProperties(void);
    void slotRemove(void);
    void slotRename(void);
    void slotSelectionChanged(Q3ListBoxItem* item);
    void slotWhat(void);
        
  private:

    // generate next curve or measure name from previous by adding one to any digits at the end
    QString newNameFromPreviousPointSet(QString previousName);

    // true for curve pointsets, false for measure pointsets
    bool curve;
    
    // document tile for captions
    QString title;

    // scratch copy of pointsets container that will be edited by this dialog
    PointSets* pointSets;

    // points belonging to removed pointsets are orphaned and must be deleted if user clicks on ok
    QStringList *pointSetsWithOrphanedPoints;

    // refresh pointset list after any change to pointset name list or properties. exits with first entry selected
    void refreshPointSetList(void);
    
    // select a particular pointset name in pointSetNames. goal is to have selection follow the moved item
    void selectPointSetName(QString selectionName);

    // returns translated string with "curve" replaced by "measure" if curve is false
    QString curveReplace(const char* text);
    
    // enable or disable buttons as appropriate
    void toggleActions(void);
        
    // widgets for maintaining the pointset list 
    QPushButton* buttonNew;
    QPushButton* buttonRemove;
    QPushButton* buttonRename;
    QPushButton* buttonProperties;
    QPushButton* buttonUp;
    QPushButton* buttonDown;
    Q3ListBox* pointSetNames;

    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QPushButton* buttonWhat;

    // name of curve or measure currently selected in the listbox. initially empty
    QString activePointSet;
};

#endif // POINTSETSDLG_H
