/***************************************************************************
                          measurecmb.h  -  description
                             -------------------
    begin                : Fri Dec 27 2002
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

#ifndef MEASURECMB_H
#define MEASURECMB_H

#include <qwidget.h>
#include <qcombobox.h>

#include "digitdoc.h"

// this combobox in draw toolbar displays the currently active measure, if in
// measure mode. otherwise this is grayed out
class MeasureCmb : public QComboBox
{
  Q_OBJECT

  public:
  
    MeasureCmb(QWidget* parent);
    ~MeasureCmb();

    // load measure names from a document
    void attachToDocument(DigitDoc* doc);

  private slots:

    // change cursor to measure point mode
    void slotComboMeasure(bool toggle);
    // change cursor to nonmeasure point mode
    void slotComboNotMeasure(bool toggle);
};

#endif // MEASURECMB_H
