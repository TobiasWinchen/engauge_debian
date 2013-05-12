/***************************************************************************
                          pointsetdlg.h  -  description
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

#ifndef POINTSETDLG_H
#define POINTSETDLG_H

#include <qobject.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <q3listbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <q3groupbox.h>
#include <q3canvas.h>
#include <qcombobox.h>

#include "digitdef.h"
#include "pointsetstyles.h"

class PointSet;
class Point;

// this dialog box class edits the properties of one pointset. The pointset
// may be an axis, scale, curve or measure pointset (depending on current state)
// but the pointset type does not matter here since all are handled identically 
class PointSetDlg : public QDialog
{
  Q_OBJECT
  
  public:

    PointSetDlg(QString title, QString curveName, DigitizeState state,
      PointSetStyle* pointSetStyle);
    ~PointSetDlg();

  private slots:

    void slotPointShape(const QString &text);
    void slotPointSize(const QString &text);
    void slotPointLineSize(const QString &text);
    void slotPointLineColor(const QString &text);
    void slotPointInColor(const QString &text);
    void slotLineSize(const QString &text);
    void slotLineColor(const QString &text);
    void slotLineConnectAs(const QString &text);
    void slotWhat(void);
        
  private:

    // state determines which type of pointset is being edited
    DigitizeState state;
    
    // style to be edited
    PointSetStyle *pointSetStyle;
    
    // enable or disable buttons as appropriate
    void toggleActions();

    // update preview after modifying a property
    void updatePreview();
    
    // returns translated string with "curve" replaced by "axis", "scale bar" or "measure"
    QString curveReplace(const char* text);

    // returns the pointset name if in curve or measure state, else blank
    QString pointsetNameFilter(const QString pointsetName);
        
    // widgets for editing the point style of the currently selected pointset
    Q3GroupBox* groupPoint;
    QLabel* labelShape;
    QLabel* labelShapeSize;
    QLabel* labelShapeLineSize;
    QLabel* labelShapeLineColor;
    QLabel* labelShapeInColor;
    QComboBox* comboShape;
    QComboBox* comboShapeSize;
    QComboBox* comboShapeLineSize;
    QComboBox* comboShapeLineColor;
    QComboBox* comboShapeInColor;

    // widgets for editing the line style of the currently selected pointset
    Q3GroupBox* groupLine;
    QLabel* labelLineSize;
    QLabel* labelLineColor;
    QLabel* labelLineConnectAs;
    QComboBox* comboLineSize;
    QComboBox* comboLineColor;
    QComboBox* comboLineConnectAs;

    // widgets for previewing the point and line styles of the currently selected pointset
    QLabel* labelPreview;
    PointSet* pointSet;
    Q3Canvas* previewCanvas;
    Q3CanvasView* previewCanvasView;
    Point* previewLeftPoint;
    Point* previewRightPoint;

    // navigation buttons
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QPushButton* buttonWhat;
};

#endif // POINTSETDLG_H
