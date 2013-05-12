/***************************************************************************
                          zoomcmb.cpp  -  description
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

#include <QWhatsThis>
//Added by qt3to4:
#include <QKeyEvent>

#include "zoomcmb.h"
#include "digitdebug.h"
#include "digitdef.h"
#include "main.h"

// default zoom settings as percentages
static int zoomDefaults [] = {500, 400, 300, 200, 100, 75, 50, 33, 25, 10};

ZoomCmb::ZoomCmb(QWidget* parent) :
  QComboBox(true, parent, "zoom factor")
{
  DigitDebug::ctor(QString("zoomcmb ") + QString::number((ulong) this, 16));
  
  setEnabled(false);
  
  QWhatsThis::add(this, QString(tr("Zoom factor as a percentage.\n\n"
    "A value from the list may be selected, "
    "or another value may be typed in (followed by Enter)")));

  QRegExp rx("[1-9]\\d{0,2}%{0,1}$"); // 1 through 999 with optional trailing percentage
  validator = new QRegExpValidator(rx, parent, "zoom validator");
  CHECK_PTR_ENGAUGE(validator);
  setValidator(validator);

  setZoom(100);
    
  // override the default Qt focus policy with the Windows and Motif policy
  setFocusPolicy(Qt::StrongFocus);
}

ZoomCmb::~ZoomCmb()
{
  DigitDebug::dtor(QString("zoomcmb ") + QString::number((ulong) this, 16));
}

void ZoomCmb::setZoom(int zoom)
{
  clear();

  QString strSelect;
  strSelect.sprintf("%d%%", zoom);
  
  bool inserted = false;
  for (unsigned int i = 0; i < sizeof(zoomDefaults) / sizeof(int); i++)
  {
    QString str;

    if (!inserted && (zoom > zoomDefaults [i]))
    {
      insertItem(strSelect);

      inserted = true;
    }
    else if (zoom == zoomDefaults [i])
      inserted = true; // value is already in default list
    
    str.sprintf("%d%%", zoomDefaults [i]);
    insertItem(str);
  }

  setCurrentText(strSelect);
}

void ZoomCmb::keyPressEvent(QKeyEvent* e)
{
  if ((e->key() == Qt::Key_Return) || 
    (e->key() == Qt::Key_Enter))
    emit(activated(currentText()));

  e->ignore();
}
