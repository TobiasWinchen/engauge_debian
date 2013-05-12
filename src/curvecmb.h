/***************************************************************************
                          curvecmb.h  -  description
                             -------------------
    begin                : Thu Oct 31 2002
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

#ifndef CURVECMB_H
#define CURVECMB_H

#include <qwidget.h>
#include <qcombobox.h>

#include "digitdoc.h"

// this combobox in draw toolbar displays the currently active curve, if in
// curve mode. otherwise this is grayed out
class CurveCmb : public QComboBox
{
  Q_OBJECT

  public:
  
    CurveCmb(QWidget* parent);
    ~CurveCmb();

    // load curve names from a document
    void attachToDocument(DigitDoc* doc);
    
  private slots:

    // change cursor to curve point mode
    void slotComboCurve(bool toggle);
    // change cursor to noncurve point mode
    void slotComboNotCurve(bool toggle);
};

#endif // CURVECMB_H
