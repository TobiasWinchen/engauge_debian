/***************************************************************************
                          discretizedlg.h  -  description
                             -------------------
    begin                : Wed Dec 17 2003
    copyright            : (C) 2003 by 
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

#ifndef DISCRETIZEDLG_H
#define DISCRETIZEDLG_H

#include <qdialog.h>
#include <q3buttongroup.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <q3groupbox.h>
#include <qlineedit.h>
#include <qstring.h>
#include <qvalidator.h>
#include <q3canvas.h>
#include <qcheckbox.h>

#include "digitdef.h"
#include "colorchooser.h"
#include "discretize.h"

// this dialog box class edits the discretizing settings, which determine
// how the original image is discretized
class DiscretizeDlg : public QDialog
{
  Q_OBJECT
  
  public:
  
    DiscretizeDlg(QString title, const QImage* imageOriginal,
      DiscretizeSettings* settings);
    ~DiscretizeDlg();

  public slots:

    void slotIntensity();
    void slotForeground();
    void slotHue();
    void slotLimitChange(bool, int);
    void slotSaturation();
    void slotValue();
    void slotWhat(void);

  private:

    // set discretize method, loading appropriate background histogram image
    void setDiscretizeMethod(DiscretizeMethod method);

    // set individual color attributes
    void setHueHigh(int limit);
    void setHueLow(int limit);
    void setSaturationHigh(int limit);
    void setSaturationLow(int limit);
    void setValueHigh(int limit);
    void setValueLow(int limit);

    // enable or disable buttons as appropriate
    void toggleActions(void);

    QString title;
    const QImage* imageOriginal;
    DiscretizeSettings* settings;
    
    // convert original image into pixel-addressable QImage, using current settings.
    // the first time through this also loads histograms
    void loadWorkingImage(void);
   
    // discretize method selection
    Q3ButtonGroup* groupSelection;

    // discretize methods
    QRadioButton* buttonIntensity;
    QRadioButton* buttonForeground;
    QRadioButton* buttonHue;
    QRadioButton* buttonSaturation;
    QRadioButton* buttonValue;

    // widgets for controlling the color attribute limits
    ColorChooser* colorChooser;
    
    // widgets for previewing the discretize results
    QLabel* labelPreview;
    Q3Canvas* previewCanvas;
    Q3CanvasView* previewCanvasView;
    
    // navigation buttons
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QPushButton* buttonWhat;

    // store pixel-addressable QImage
    QImage imageWorking;

    // discretize function of Discretize object takes a fifth of a second to complete,
    // so that function is reentrant with new calls aborting previous calls
    Discretize discretize;
};

#endif // DISCRETIZEDLG_H
