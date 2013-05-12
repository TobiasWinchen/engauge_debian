/***************************************************************************
                          segmentdlg.h  -  description
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

#ifndef SEGMENTDLG_H
#define SEGMENTDLG_H

#include <qdialog.h>
#include <qpushbutton.h>
#include <q3buttongroup.h>
#include <qradiobutton.h>
#include <qlabel.h>
#include <qvalidator.h>
#include <qlineedit.h>
#include <q3ptrlist.h>
#include <qcombobox.h>
#include <qcheckbox.h>

#include "digitdef.h"
#include "point.h"
#include "pointset.h"

class Segments;

// this dialog box class edits a document's segment information
class SegmentDlg : public QDialog
{
  Q_OBJECT

  public:
  
    SegmentDlg(QString title, SegmentSettings* settings);
    ~SegmentDlg();

  public slots:

    void slotMinPoints(const QString &text);
    void slotPointSeparation(const QString &text);
    void slotFillCorners(bool check);
    void slotLineSize(const QString &text);
    void slotLineColor(const QString &text);
    void slotWhat(void);

  private slots:

    // update preview after modifying a property. this is implemented as a slot so that
    // the long delay involved in creating segments will delay responses to the user (especially
    // typing in point separation window)
    void slotUpdatePreview(void);

  signals:

    // send off signal to update preview after current processing has a chance to finish
    void updatePreview(void);
    
  private:

    // enable or disable buttons as appropriate
    void toggleActions(void);

    // return true if all settings contraints are currently satisfied
    bool validateSettings(void);
    
    SegmentSettings* settings;

    // segment properties
    QLabel* labelMinPoints;
    QLineEdit* editMinPoints;
    QIntValidator* validatorMinPoints;

    QLabel* labelPointSeparation;
    QLineEdit* editPointSeparation;
    QIntValidator* validatorPointSeparation;

    QLabel* labelFillCorners;
    QCheckBox* checkFillCorners;

    QLabel* labelLineSize;
    QComboBox* comboLineSize;
    
    QLabel* labelLineColor;
    QComboBox* comboLineColor;

    // widgets for previewing the point and line styles of the currently selected segment
    QLabel* labelPreview;
    Q3Canvas* previewCanvas;
    Q3CanvasView* previewCanvasView;
    QImage previewImageOriginal;
    Segments* segments;
    PointSet pointSet; // every point, even preview points, must belong to a pointset

    // navigation buttons    
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QPushButton* buttonWhat;
};

#endif // SEGMENTDLG_H
