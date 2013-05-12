/***************************************************************************
                          zoomcmb.h  -  description
                             -------------------
    begin                : Sat Dec 7 2002
    copyright            : (C) 2002 by Mark Mitchell
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

#ifndef ZOOMCMB_H
#define ZOOMCMB_H

#include <QComboBox>
#include <qvalidator.h>
//Added by qt3to4:
#include <QKeyEvent>

// zoom combobox in zoom toolbar shows and modifies zoom setting of
// currently active window
class ZoomCmb : public QComboBox
{
  Q_OBJECT
  
  public: 

    ZoomCmb(QWidget* parent);
    ~ZoomCmb();

    // called when changing focus to another window. show union of default list and
    // this value
    void setZoom(int z);
    
  private:

    // send the zoom value to the view when carriage return is pressed
    virtual void keyPressEvent(QKeyEvent* e);

    QRegExpValidator* validator;
    
};

#endif // ZOOMCMB_H
