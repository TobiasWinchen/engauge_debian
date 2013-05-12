/***************************************************************************
                          discretizedlg.cpp  -  description
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


#include <qtooltip.h>
#include <qbitmap.h>
//Added by qt3to4:
#include <QPixmap>
#include <QLabel>
#include <QWhatsThis>

#include <math.h>

#include "digitdoc.h"
#include "digitview.h"
#include "discretize.h"
#include "discretizedlg.h"
#include "img/whatsthis.xpm"
#include "digitdebug.h"
#include "main.h"
                                                
DiscretizeDlg::DiscretizeDlg(QString title,
  const QImage* imageOriginal,
  DiscretizeSettings* settings) :
  QDialog(0, "discretizedlg", true),
  title(title),
  imageOriginal(imageOriginal),
  settings(settings)
{
  DigitDebug::ctor(QString("discretizedlg ") + QString::number((ulong) this, 16));
  
  setCaption(tr("Discretize - [%1]%2")
    .arg(title)
    .arg(freewareCaption));

  setFixedSize(400, 490);

  // discretize method selection
  groupSelection = new Q3ButtonGroup(QString(tr("Discretize method selection")), this);
  CHECK_PTR_ENGAUGE(groupSelection);
  groupSelection->setGeometry(10, 10, 380, 230);

  // intensity
  buttonIntensity = new QRadioButton(QString(tr("Intensity")), groupSelection);
  CHECK_PTR_ENGAUGE(buttonIntensity);
  buttonIntensity->setGeometry(10, 20, 130, 30);
  QWhatsThis::add(buttonIntensity, QString(tr("Discretize the original image using intensity")));
  connect(buttonIntensity, SIGNAL(clicked()), this, SLOT(slotIntensity()));

  // foreground or background
  buttonForeground = new QRadioButton(QString(tr("Foreground")), groupSelection);
  CHECK_PTR_ENGAUGE(buttonForeground);
  buttonForeground->setGeometry(10, 60, 130, 30);
  QWhatsThis::add(buttonForeground, QString(tr("Discretize the original image into foreground and background.\n\n"
    "The background color is shown on the left side of the scale bar. All pixels with approximately "
    "the background color are considered part of the background, and all other pixels are considered "
    "part of the foreground")));
  connect(buttonForeground, SIGNAL(clicked()), this, SLOT(slotForeground()));

  // hue
  buttonHue = new QRadioButton(QString(tr("Hue")), groupSelection);
  CHECK_PTR_ENGAUGE(buttonHue);
  buttonHue->setGeometry(10, 100, 130, 30);
  QWhatsThis::add(buttonHue, QString(tr("Discretize the original image using hue")));
  connect(buttonHue, SIGNAL(clicked()), this, SLOT(slotHue()));

  // saturation
  buttonSaturation = new QRadioButton(QString(tr("Saturation")), groupSelection);
  CHECK_PTR_ENGAUGE(buttonSaturation);
  buttonSaturation->setGeometry(10, 140, 130, 30);
  QWhatsThis::add(buttonSaturation, QString(tr("Discretize the original image using saturation")));
  connect(buttonSaturation, SIGNAL(clicked()), this, SLOT(slotSaturation()));

  // value
  buttonValue = new QRadioButton(QString(tr("Value")), groupSelection);
  CHECK_PTR_ENGAUGE(buttonValue);
  buttonValue->setGeometry(10, 180, 130, 30);
  QWhatsThis::add(buttonValue, QString(tr("Discretize the original image using value")));
  connect(buttonValue, SIGNAL(clicked()), this, SLOT(slotValue()));

  // color chooser area
  QRgb rgbBg = discretize.backgroundColor(imageOriginal);
  colorChooser = new ColorChooser(groupSelection, imageOriginal, 150, 20, rgbBg, true);
  CHECK_PTR_ENGAUGE(colorChooser);
  connect(colorChooser, SIGNAL(limitChange(bool, int)), this, SLOT(slotLimitChange(bool, int)));

  // preview area
  labelPreview = new QLabel(QString(tr("Preview")), this);
  CHECK_PTR_ENGAUGE(labelPreview);
  labelPreview->setGeometry(30, 240, 160, 30);

  previewCanvas = new Q3Canvas(this);
  CHECK_PTR_ENGAUGE(previewCanvas);
  previewCanvas->resize(imageOriginal->width(), imageOriginal->height());

  previewCanvasView = new Q3CanvasView(previewCanvas, this);
  CHECK_PTR_ENGAUGE(previewCanvasView);
  previewCanvasView->setGeometry(30, 270, 340, 150);
  QWhatsThis::add(previewCanvasView, QString(tr(
    "Preview window shows how current settings affect the discretizing of the original image")));

  // navigation buttons
  buttonOk = new QPushButton(tr("OK"), this);
  CHECK_PTR_ENGAUGE(buttonOk);
  buttonOk->setGeometry(80, 440, 100, 30);
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(accept()));

  buttonCancel = new QPushButton(tr("Cancel"), this);
  CHECK_PTR_ENGAUGE(buttonCancel);
  buttonCancel->setGeometry(210, 440, 100, 30);
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));

  buttonWhat = new QPushButton(this);
  CHECK_PTR_ENGAUGE(buttonWhat);
  buttonWhat->setPixmap(QPixmap(whatsthis_xpm));
  buttonWhat->setGeometry(330, 440, 60, 30);
  QToolTip::add(buttonWhat, QString(tr("What's this?")));
  connect(buttonWhat, SIGNAL(clicked()), this, SLOT(slotWhat()));

  setDiscretizeMethod(settings->discretizeMethod);
}

DiscretizeDlg::~DiscretizeDlg()
{
  DigitDebug::dtor(QString("discretizedlg ") + QString::number((ulong) this, 16));
}

void DiscretizeDlg::loadWorkingImage()
{
  imageWorking = imageOriginal->copy(); // deep copy needed
  
  // use settings to process image
  discretize.discretize(&imageWorking, *settings);
  
  // update preview window
  QPixmap pixmapWorking;
  pixmapWorking.convertFromImage(imageWorking, Qt::ThresholdDither);
  ASSERT_ENGAUGE(previewCanvas != 0);
  previewCanvas->setBackgroundPixmap(pixmapWorking);
}

void DiscretizeDlg::setDiscretizeMethod(DiscretizeMethod method)
{
  settings->discretizeMethod = method;

  switch (method)
  {
  case DiscretizeNone:
    colorChooser->setDiscretizeMethod(method);
    break;
  case DiscretizeIntensity:
    colorChooser->setDiscretizeMethod(method, settings->intensityThresholdLow, settings->intensityThresholdHigh);
    break;
  case DiscretizeForeground:
    colorChooser->setDiscretizeMethod(method, settings->foregroundThresholdLow, settings->foregroundThresholdHigh);
    break;
  case DiscretizeHue:
    colorChooser->setDiscretizeMethod(method, settings->hueThresholdLow, settings->hueThresholdHigh);
    break;
  case DiscretizeSaturation:
    colorChooser->setDiscretizeMethod(method, settings->saturationThresholdLow, settings->saturationThresholdHigh);
    break;
  case DiscretizeValue:
    colorChooser->setDiscretizeMethod(method, settings->valueThresholdLow, settings->valueThresholdHigh);
    break;
  }

  toggleActions();
}

void DiscretizeDlg::toggleActions()
{
  loadWorkingImage();

  if ((settings->discretizeMethod == DiscretizeIntensity) && (!buttonIntensity->isChecked()))
    buttonIntensity->setChecked(true);
  if ((settings->discretizeMethod == DiscretizeForeground) && (!buttonForeground->isChecked()))
    buttonForeground->setChecked(true);
  if ((settings->discretizeMethod == DiscretizeHue) && (!buttonHue->isChecked()))
    buttonHue->setChecked(true);
  if ((settings->discretizeMethod == DiscretizeSaturation) && (!buttonSaturation->isChecked()))
    buttonSaturation->setChecked(true);
  if ((settings->discretizeMethod == DiscretizeValue) && (!buttonValue->isChecked()))
    buttonValue->setChecked(true);

  buttonOk->setEnabled(
    (settings->intensityThresholdHigh <= DiscretizeIntensityMax) &&
    (settings->intensityThresholdLow <= DiscretizeIntensityMax) &&
    (settings->foregroundThresholdHigh <= DiscretizeForegroundMax) &&
    (settings->foregroundThresholdLow <= DiscretizeForegroundMax) &&
    (settings->hueThresholdHigh <= DiscretizeHueMax) &&
    (settings->hueThresholdLow <= DiscretizeHueMax) &&
    (settings->saturationThresholdHigh <= DiscretizeSaturationMax) &&
    (settings->saturationThresholdLow <= DiscretizeSaturationMax) &&
    (settings->valueThresholdHigh <= DiscretizeValueMax) &&
    (settings->valueThresholdLow <= DiscretizeValueMax));
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void DiscretizeDlg::slotIntensity()
{
  setDiscretizeMethod(DiscretizeIntensity);
}

void DiscretizeDlg::slotForeground()
{
  setDiscretizeMethod(DiscretizeForeground);
}

void DiscretizeDlg::slotHue()
{
  setDiscretizeMethod(DiscretizeHue);
}

void DiscretizeDlg::slotLimitChange(bool lower, int limit)
{
  switch (settings->discretizeMethod)
  {
  case DiscretizeNone:
    ASSERT_ENGAUGE(0);
    break;
  case DiscretizeIntensity:
    if (lower)
      settings->intensityThresholdLow = limit;
    else
      settings->intensityThresholdHigh = limit;
    break;
  case DiscretizeForeground:
    if (lower)
      settings->foregroundThresholdLow = limit;
    else
      settings->foregroundThresholdHigh = limit;
    break;
  case DiscretizeHue:
    if (lower)
      settings->hueThresholdLow = limit;
    else
      settings->hueThresholdHigh = limit;
    break;
  case DiscretizeSaturation:
    if (lower)
      settings->saturationThresholdLow = limit;
    else
      settings->saturationThresholdHigh = limit;
    break;
  case DiscretizeValue:
    if (lower)
      settings->valueThresholdLow = limit;
    else
      settings->valueThresholdHigh = limit;
    break;
  }

  toggleActions();
}

void DiscretizeDlg::slotSaturation()
{
  setDiscretizeMethod(DiscretizeSaturation);
}

void DiscretizeDlg::slotValue()
{
  setDiscretizeMethod(DiscretizeValue);
}

void DiscretizeDlg::slotWhat()
{
  QWhatsThis::enterWhatsThisMode();
}
