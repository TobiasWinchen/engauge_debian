/***************************************************************************
                          scalebardlg.h  -  description
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

#ifndef SCALEBARDLG_H
#define SCALEBARDLG_H

#include <qdialog.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qvalidator.h>

class DigitDoc;

// when a scale bar is created, this dialog appears so the user can immediately
// enter in the length of the scale bar
class ScaleBarDlg : public QDialog
{
  Q_OBJECT
  
  public: 

    ScaleBarDlg(DigitDoc* doc, double length);
    ~ScaleBarDlg();

    const QString length(void) { return editLength->text(); }
    
  private:

    QLabel* labelLength;
    QLineEdit* editLength;
    QDoubleValidator* doubleValidator;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

    DigitDoc* doc;
};

#endif // SCALEBARDLG_H
