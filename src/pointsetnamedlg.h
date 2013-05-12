/***************************************************************************
                          pointsetnamedlg.h  -  description
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

#ifndef POINTSETNAMEDLG_H
#define POINTSETNAMEDLG_H

#include <qobject.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qstringlist.h>

// edit the name of a curve or measure. Note that the axis pointset has a fixed name so
// only curve and measure pointset names can be modified
class PointSetNameDlg : public QDialog
{
  Q_OBJECT

  public:

    PointSetNameDlg(bool curve, QString title, QString* name, QStringList disallowedNames);
    ~PointSetNameDlg();

  private slots:

    // called after each edit change in the edit box
    void slotTextChanged(const QString &);
    
  private:

    // true for curve pointsets, false for measure pointsets
    bool curve;

    // enable or disable buttons as appropriate  
    void toggleActions();

    // returns translated string with "curve" replaced by "measure" if curve is false
    QString curveReplace(const char* text);
        
    // document title
    QString title;

    // scratch copy of curve or measure name that will be edited by this dialog
    QString* name;
    
    // names are disallowed since pointset names must be unique
    QStringList disallowedNames;
    
    QLabel* labelName;
    QLineEdit* editName;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
};

#endif // POINTSETNAMEDLG_H
