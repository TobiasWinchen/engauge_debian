/***************************************************************************
                          gridremovaldlg.cpp  -  description
                             -------------------
    begin                : Fri Dec 5 2003
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
#include <QWhatsThis>
#include <qbitmap.h>
//Added by qt3to4:
#include <QPixmap>
#include <QLabel>
#include <QMouseEvent>

#include <math.h>

#include "digitdoc.h"
#include "digitview.h"
#include "gridmesh.h"
#include "gridmeshdlg.h"
#include "gridremoval.h"
#include "gridremovaldlg.h"
#include "digitdebug.h"
#include "main.h"
#include "img/whatsthis.xpm"
#include "img/eyedropperonmap.xpm"
#include "img/eyedropperonmask.xpm"
#include "transform.h"
#include "discretize.h"

GridRemovalDlg::GridRemovalDlg(QString title,
  const QImage* imageOriginal,
  const Transform* transform,
  CoordSettings* coordSettings,
  GridRemovalSettings* gridSettings) :
  QDialog(0, "gridremovaldlg", true),
  title(title),
  imageOriginal(imageOriginal),
  transform(transform),
  coordSettings(coordSettings),
  gridSettings(gridSettings)
{
  DigitDebug::ctor(QString("gridremovaldlg ") + QString::number((ulong) this, 16));
  
  setCaption(tr("Grid Removal - [%1]%2")
    .arg(title)
    .arg(freewareCaption));

  setFixedSize(430, 620);

  // color
  checkColor = new QCheckBox(this);
  CHECK_PTR_ENGAUGE(checkColor);
  checkColor->setGeometry(10, 17, 15, 15);
  checkColor->setChecked(gridSettings->removeColor);
  QWhatsThis::add(checkColor, QString(tr("Check this box to remove all pixels whose color "
    "is close to the selected color.\n\n"
    "When this option is enabled, the selected color is displayed on the left end of the "
    "histogram scale. To change the selected color, click on a pixel in the preview widow below.\n\n"
    "This option is only useful if the gridlines in the original image are a different color "
    "than the curve lines")));
  connect(checkColor, SIGNAL(toggled(bool)), this, SLOT(slotColorToggled(bool)));

  labelColorMain = new QLabel(QString(tr("Remove pixels of a particular color")), this);
  CHECK_PTR_ENGAUGE(labelColorMain);
  labelColorMain->setGeometry(30, 10, 360, 30);

  labelColor = new QLabel(QString(tr("Color:")), this);
  CHECK_PTR_ENGAUGE(labelColor);
  labelColor->setGeometry(30, 50, 270, 30);

  // color chooser area
  colorChooser = new ColorChooser(this, imageOriginal, 205, 50, gridSettings->color.rgb(), false);
  CHECK_PTR_ENGAUGE(colorChooser);
  connect(colorChooser, SIGNAL(limitChange(bool, int)), this, SLOT(slotLimitChange(bool, int)));

  // gridline mesh
  checkGrid = new QCheckBox(this);
  CHECK_PTR_ENGAUGE(checkGrid);
  checkGrid->setGeometry(10, 137, 15, 15);
  checkGrid->setChecked(gridSettings->removeGridlines);
  if (!transform->validAxes())
    checkGrid->setEnabled(false); // do not get users hopes up - gridline approach requires transform
  QWhatsThis::add(checkGrid, QString(tr("Check this box to have pixels close to regularly spaced gridlines removed.\n\n"
    "This option is only available when the axis points have all been defined.\n\n"
    "The gridline values are defined here, by clicking on the Gridlines... button")));
  connect(checkGrid, SIGNAL(toggled(bool)), this, SLOT(slotGridToggled(bool)));

  labelGridMain = new QLabel(QString(tr("Remove pixels close to regularly spaced gridlines")), this);
  CHECK_PTR_ENGAUGE(labelGridMain);
  labelGridMain->setGeometry(30, 130, 360, 30);

  labelGridMesh = new QLabel(QString(tr("Removal gridlines:")), this);
  CHECK_PTR_ENGAUGE(labelGridMesh);
  labelGridMesh->setGeometry(30, 170, 230, 30);

  buttonGridMesh = new QPushButton(tr("Gridlines..."), this);
  CHECK_PTR_ENGAUGE(buttonGridMesh);
  buttonGridMesh->setGeometry(270, 170, 150, 30);
  connect(buttonGridMesh, SIGNAL(clicked()), this, SLOT(slotGridlines()));

  labelGridDistance = new QLabel(QString(tr("Close distance (pixels):")), this);
  CHECK_PTR_ENGAUGE(labelGridDistance);
  labelGridDistance->setGeometry(30, 210, 230, 30);

  editGridDistance = new QLineEdit(this);
  CHECK_PTR_ENGAUGE(editGridDistance);
  editGridDistance->setGeometry(270, 210, 150, 30);
  editGridDistance->setText(QString("%1").arg(gridSettings->gridDistance, 0, 'f', 1)); // make sure users see decimal point
  QWhatsThis::add(editGridDistance, QString(tr("Set closeness distance in pixels.\n\n"
    "Pixels that are closer to the regularly spaced gridlines, than this distance, "
    "will be removed.\n\n"
    "This value cannot be negative. A zero value disables this feature. Decimal values are allowed")));
  validatorGridDistance = new QDoubleValidator(editGridDistance);
  CHECK_PTR_ENGAUGE(validatorGridDistance);
  validatorGridDistance->setBottom(0.0);
  editGridDistance->setValidator(validatorGridDistance);
  connect(editGridDistance, SIGNAL(textChanged(const QString &)), this, SLOT(slotGridDistance(const QString &)));

  // thin lines
  checkThin = new QCheckBox(this);
  CHECK_PTR_ENGAUGE(checkThin);
  checkThin->setGeometry(10, 257, 15, 15);
  ASSERT_ENGAUGE(gridSettings != 0);
  checkThin->setChecked(gridSettings->removeThinLines);
  if (!transform->validAxes())
    checkThin->setEnabled(false); // do not get users hopes up - thin line approach requires transform
  QWhatsThis::add(checkThin, QString(tr("Check this box to remove thin lines that are parallel to the axes.\n\n"
    "This option is only available when the axis points have all been defined.\n\n"
    "This option works especially well if the gridlines in the original image are thinner "
    "than the curve lines")));
  connect(checkThin, SIGNAL(toggled(bool)), this, SLOT(slotThinToggled(bool)));

  labelThinMain = new QLabel(QString(tr("Remove thin lines parallel to the axes")), this);
  CHECK_PTR_ENGAUGE(labelThinMain);
  labelThinMain->setGeometry(30, 250, 360, 30);

  labelThinThickness = new QLabel(QString(tr("Minimum line thickness (pixels):")), this);
  CHECK_PTR_ENGAUGE(labelThinThickness);
  labelThinThickness->setGeometry(30, 290, 230, 30);

  editThinThickness = new QLineEdit(this);
  CHECK_PTR_ENGAUGE(editThinThickness);
  editThinThickness->setGeometry(270, 290, 150, 30);
  editThinThickness->setText(QString("%1").arg(gridSettings->thinThickness, 0, 'f', 1)); // make sure users see decimal point
  QWhatsThis::add(editThinThickness, QString(tr("Select minimum thickness for lines, in pixels.\n\n"
    "Pixels will be removed wherever lines are thinner than this value.\n\n"
    "This value cannot be negative. A zero value disables this feature. Decimal values are allowed")));
  validatorThinThickness = new QDoubleValidator(editThinThickness);
  CHECK_PTR_ENGAUGE(validatorThinThickness);
  validatorThinThickness->setBottom(0.0);
  editThinThickness->setValidator(validatorThinThickness);
  connect(editThinThickness, SIGNAL(textChanged(const QString &)), this, SLOT(slotThinThickness(const QString &)));

  // gap
  labelGap = new QLabel(QString(tr("Maximum gap to connect (pixels):")), this);
  CHECK_PTR_ENGAUGE(labelGap);
  labelGap->setGeometry(10, 340, 270, 30);

  editGap = new QLineEdit(this);
  CHECK_PTR_ENGAUGE(editGap);
  editGap->setGeometry(270, 340, 150, 30);
  editGap->setText(QString("%1").arg(gridSettings->gapSeparation, 0, 'f', 1)); // make sure users see decimal point
  QWhatsThis::add(editGap, QString(tr("Set maximum gap separation in pixels.\n\n"
    "Lines whose endpoints are separated by less than this distance, will be connected.\n\n"
    "This value cannot be negative. A zero value disables this feature. Decimal values are allowed")));
  validatorGap = new QDoubleValidator(editGap);
  CHECK_PTR_ENGAUGE(validatorGap);
  validatorGap->setBottom(0.0);
  editGap->setValidator(validatorGap);
  connect(editGap, SIGNAL(textChanged(const QString &)), this, SLOT(slotGap(const QString &)));

  // preview area
  labelPreview = new QLabel(QString(tr("Preview")), this);
  CHECK_PTR_ENGAUGE(labelPreview);
  labelPreview->setGeometry(30, 390, 100, 30);

  previewCanvas = new Q3Canvas(this);
  CHECK_PTR_ENGAUGE(previewCanvas);
  previewCanvas->resize(imageOriginal->width(), imageOriginal->height());
  previewCanvasView = new ClickablePreview(previewCanvas, this);
  CHECK_PTR_ENGAUGE(previewCanvasView);
  previewCanvasView->setGeometry(30, 420, 370, 150);
  QWhatsThis::add(previewCanvasView, QString(tr(
    "Preview window shows how current settings affect the gridlines.\n\n"
    "To change the selected color for the Remove By Color option, select that option and then "
    "click on a pixel in the preview window with the eyedropper")));

  // navigation buttons
  buttonOk = new QPushButton(tr("OK"), this);
  CHECK_PTR_ENGAUGE(buttonOk);
  buttonOk->setGeometry(100, 580, 100, 30);
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(accept()));

  buttonCancel = new QPushButton(tr("Cancel"), this);
  CHECK_PTR_ENGAUGE(buttonCancel);
  buttonCancel->setGeometry(230, 580, 100, 30);
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));

  buttonWhat = new QPushButton(this);
  CHECK_PTR_ENGAUGE(buttonWhat);
  buttonWhat->setPixmap(QPixmap(whatsthis_xpm));
  buttonWhat->setGeometry(360, 580, 60, 30);
  QToolTip::add(buttonWhat, QString(tr("What's this?")));
  connect(buttonWhat, SIGNAL(clicked()), this, SLOT(slotWhat()));

  loadEyedropper();
  loadWorkingImageAndIsolateBackground();
  enableColorChooser(gridSettings->removeColor);
  toggleActions();
}

GridRemovalDlg::~GridRemovalDlg()
{
  DigitDebug::dtor(QString("gridremovaldlg ") + QString::number((ulong) this, 16));
}

void GridRemovalDlg::enableColorChooser(bool enable)
{
  ASSERT_ENGAUGE(colorChooser != 0);
  colorChooser->setDiscretizeMethod(enable ? DiscretizeForeground : DiscretizeNone,
    gridSettings->foregroundThresholdLow,
    gridSettings->foregroundThresholdHigh);
}

void GridRemovalDlg::loadEyedropper()
{
  // load cursor images
  QPixmap pixmap(eyedropperonmap_xpm);
  QPixmap pixmask(eyedropperonmask_xpm);
  QBitmap bitmap;
  QBitmap bitmask;
  bitmap = pixmap.convertToImage().convertDepth(1, Qt::ThresholdDither);
  bitmask = pixmask.convertToImage().convertDepth(1, Qt::ThresholdDither);
  m_cursorEyedropper = new QCursor(bitmap, bitmask, EyedropperHotX, EyedropperHotY);
  CHECK_PTR_ENGAUGE(m_cursorEyedropper);
}

void GridRemovalDlg::loadWorkingImage()
{
  ASSERT_ENGAUGE(previewCanvas != 0);
  m_imageWorking = imageOriginal->copy(); // deep copy needed

  m_gridRemoval.removeAndConnect(&m_imageWorking, transform, *coordSettings, *gridSettings,
    m_backgroundColor.rgb());

  QPixmap pixmapWorking;
  pixmapWorking.convertFromImage(m_imageWorking, Qt::ThresholdDither);
  previewCanvas->setBackgroundPixmap(pixmapWorking);
}

void GridRemovalDlg::loadWorkingImageAndIsolateBackground()
{
  loadWorkingImage();

  Discretize discretize;
  m_backgroundColor = discretize.backgroundColor(&m_imageWorking);
}

void GridRemovalDlg::setColor(QPoint p)
{
  if (checkColor->isChecked())
  {
    if ((p.x() < m_imageWorking.width()) ||
      (p.y() < m_imageWorking.height()))
    {
      // save color under cursor
      ASSERT_ENGAUGE(gridSettings != 0);
      gridSettings->color.setRgb(pixelRGB(&m_imageWorking, p.x(), p.y()));

      colorChooser->changeBackgroundColor(imageOriginal, gridSettings->color.rgb());
          
      toggleActions();
    }
  }
}

void GridRemovalDlg::toggleActions()
{
  // a checkbox may be disabled but checked because there is no valid axis
  // transformation currently, but before there was and the user
  // selected that checkbox at that point
  ASSERT_ENGAUGE(checkThin != 0);
  ASSERT_ENGAUGE(checkGrid != 0);
  ASSERT_ENGAUGE(checkColor != 0);
  bool enableThin = checkThin->isChecked() && checkThin->isEnabled();
  bool enableGrid = checkGrid->isChecked() && checkGrid->isEnabled();
  bool enableColor = checkColor->isChecked() && checkColor->isEnabled();

  // controls controlled by checkColor
  if (enableColor)
    previewCanvasView->setCursor(*m_cursorEyedropper);
  else
    previewCanvasView->unsetCursor();
    
  // controls controlled by checkThin
  ASSERT_ENGAUGE(editThinThickness != 0);
  editThinThickness->setEnabled(enableThin);

  // controls controlled by checkGrid
  ASSERT_ENGAUGE(editGridDistance != 0);
  editGridDistance->setEnabled(enableGrid);
  ASSERT_ENGAUGE(buttonGridMesh != 0);
  buttonGridMesh->setEnabled(enableGrid);

  // update working image with current settings
  loadWorkingImage();
}

GridRemovalDlg::ClickablePreview::ClickablePreview(Q3Canvas* canvas, GridRemovalDlg* dlg) :
  Q3CanvasView(canvas, dlg),
  dlg(dlg)
{
}

void GridRemovalDlg::ClickablePreview::contentsMousePressEvent(QMouseEvent* e)
{
  ASSERT_ENGAUGE(dlg != 0);
  dlg->setColor(e->pos());
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void GridRemovalDlg::slotLimitChange(bool lower, int limit)
{
  if (lower)
    gridSettings->foregroundThresholdLow = limit;
  else
    gridSettings->foregroundThresholdHigh = limit;
  
  toggleActions();
}

void GridRemovalDlg::slotColorToggled(bool on)
{
  gridSettings->removeColor = on;

  enableColorChooser(on);
      
  toggleActions();
}

void GridRemovalDlg::slotGap(const QString &text)
{
  ASSERT_ENGAUGE(gridSettings != 0);
  if (text == QString(""))
    gridSettings->gapSeparation = 0.0;
  else
    gridSettings->gapSeparation = text.toDouble();

  toggleActions();
}

void GridRemovalDlg::slotGridlines()
{
  GridMeshSettings grid = gridSettings->gridMesh; 
  GridMeshDlg* dlg = new GridMeshDlg(QString(tr("Removal")), title, coordSettings, &grid);
  CHECK_PTR_ENGAUGE(dlg);

  if (dlg->exec() == QDialog::Accepted)
    gridSettings->gridMesh = grid;

  delete dlg;

  toggleActions();
}

void GridRemovalDlg::slotGridToggled(bool on)
{
  gridSettings->removeGridlines = on;
  
  toggleActions();
}

void GridRemovalDlg::slotGridDistance(const QString &text)
{
  ASSERT_ENGAUGE(gridSettings != 0);
  if (text == QString(""))
    gridSettings->gridDistance = 0.0;
  else
    gridSettings->gridDistance = text.toDouble();

  toggleActions();
}

void GridRemovalDlg::slotThinThickness(const QString &text)
{
  ASSERT_ENGAUGE(gridSettings != 0);
  if (text == QString(""))
    gridSettings->thinThickness = 0.0;
  else
    gridSettings->thinThickness = text.toDouble();

  toggleActions();
}

void GridRemovalDlg::slotThinToggled(bool on)
{
  gridSettings->removeThinLines = on;
  
  toggleActions();
}

void GridRemovalDlg::slotWhat()
{
  QWhatsThis::enterWhatsThisMode();
}
