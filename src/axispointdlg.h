/***************************************************************************
                          axispointdlg.h  -  description
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

#ifndef AXISPOINTDLG_H
#define AXISPOINTDLG_H

#include <qdialog.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qvalidator.h>

class DigitDoc;

// when an axis point is created, this dialog appears so the user can immediately
// enter in the world coordinates of that axis point
class AxisPointDlg : public QDialog
{
  Q_OBJECT

  public: 

    AxisPointDlg(DigitDoc* doc, double x, double y);
    ~AxisPointDlg();

    double x(void);
    double y(void);

  public slots:

    // user has accepted coordinates, so make sure they are are reasonable
    void slotAccept(void);
    
  private:

    QLabel* labelX;
    QLabel* labelY;
    QLineEdit* editX;
    QLineEdit* editY;
    QDoubleValidator* doubleValidatorX;
    QDoubleValidator* doubleValidatorY;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

    DigitDoc* doc;
};

#endif // AXISPOINTDLG_H
