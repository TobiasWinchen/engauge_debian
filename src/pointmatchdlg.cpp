/***************************************************************************
                          discretizedlg.cpp  -  description
                             -------------------
    begin                : Thu Jan 1 2004
    copyright            : (C) 2004 by
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
#include <qtooltip.h>
#include <qbitmap.h>
//Added by qt3to4:
#include <QList>
#include <QPixmap>
#include <QLabel>

#include <math.h>

#include "pointmatchdlg.h"
#include "digitdebug.h"
#include "main.h"
#include "highlight.h"
#include "img/whatsthis.xpm"
#include "img/matchdiamond.xpm"
#include "img/matchcircle.xpm"
#include "img/matchsquare.xpm"
                                                
PointMatchDlg::PointMatchDlg(const QString title, PointSetStyle style,
  PointMatchSettings* settings) :
  QDialog(0, "pointmatchdlg", true),
  title(title),
  style(style),
  settings(settings),
  highlight(0)
{
  DigitDebug::ctor(QString("pointmatchdlg ") + QString::number((ulong) this, 16));
  
  setCaption(tr("Point Match - [%1]%2")
    .arg(title)
    .arg(freewareCaption));

  setFixedSize(360, 390);

  labelPointSeparation = new QLabel(tr("Point Separation:"), this);
  CHECK_PTR_ENGAUGE(labelPointSeparation);
  labelPointSeparation->setGeometry(10, 10, 200, 30);

  editPointSeparation = new QLineEdit(this);
  CHECK_PTR_ENGAUGE(editPointSeparation);
  editPointSeparation->setGeometry(220, 10, 130, 30);
  editPointSeparation->setText(QString("%1").arg(settings->pointSeparation));
  QWhatsThis::add(editPointSeparation, QString(tr("Select a point separation in pixels.\n\n"
    "Matched points must be separated from existing points by at least this number of pixels.\n\n"
    "This value has a lower limit")));
  validatorPointSeparation = new QIntValidator(editPointSeparation);
  CHECK_PTR_ENGAUGE(validatorPointSeparation);
  editPointSeparation->setValidator(validatorPointSeparation);
  connect(editPointSeparation, SIGNAL(textChanged(const QString &)), this, SLOT(slotPointSeparation(const QString &)));

  labelPointSize = new QLabel(tr("Maximum Point Size:"), this);
  CHECK_PTR_ENGAUGE(labelPointSize);
  labelPointSize->setGeometry(10, 50, 200, 30);

  editPointSize = new QLineEdit(this);
  CHECK_PTR_ENGAUGE(editPointSize);
  editPointSize->setGeometry(220, 50, 130, 30);
  editPointSize->setText(QString("%1").arg(settings->pointSize));
  QWhatsThis::add(editPointSize, QString(tr("Select a maximum point size in pixels.\n\n"
    "Sample match points must fit within a square box, around the cursor, having width and height "
    "equal to this maximum.\n\n"
    "This size is also used to determine if a region of pixels that are on, in the processed image, "
    "should be ignored since that region is wider or taller than this limit.\n\n"
    "This value has a lower limit")));
  validatorPointSize = new QIntValidator(editPointSize);
  CHECK_PTR_ENGAUGE(validatorPointSize);
  editPointSize->setValidator(validatorPointSize);
  connect(editPointSize, SIGNAL(textChanged(const QString &)), this, SLOT(slotPointSize(const QString &)));

  labelAcceptedColor = new QLabel(QString(tr("Accepted Point Color:")), this);
  CHECK_PTR_ENGAUGE(labelAcceptedColor);
  labelAcceptedColor->setGeometry(10, 90, 200, 30);

  comboAcceptedColor = new QComboBox(false, this, tr("Accepted Point Color"));
  CHECK_PTR_ENGAUGE(comboAcceptedColor);
  comboAcceptedColor->setGeometry(220, 90, 130, 30);
  comboAcceptedColor->setEditable(false);
  comboAcceptedColor->insertStringList(PointSetStyles::instance().colorNames());
  QWhatsThis::add(comboAcceptedColor, QString(tr("Select a color for matched points that are accepted")));
  connect(comboAcceptedColor, SIGNAL(activated(const QString &)), this, SLOT(slotAcceptedColor(const QString &)));

  labelRejectedColor = new QLabel(QString(tr("Rejected Point Color:")), this);
  CHECK_PTR_ENGAUGE(labelRejectedColor);
  labelRejectedColor->setGeometry(10, 130, 200, 30);

  comboRejectedColor = new QComboBox(false, this, tr("Rejected Point Color"));
  CHECK_PTR_ENGAUGE(comboRejectedColor);
  comboRejectedColor->setGeometry(220, 130, 130, 30);
  comboRejectedColor->setEditable(false);
  comboRejectedColor->insertStringList(PointSetStyles::instance().colorNames());
  QWhatsThis::add(comboRejectedColor, QString(tr("Select a color for matched points that are rejected")));
  connect(comboRejectedColor, SIGNAL(activated(const QString &)), this, SLOT(slotRejectedColor(const QString &)));

  labelCandidateColor = new QLabel(QString(tr("Candidate Color:")), this);
  CHECK_PTR_ENGAUGE(labelCandidateColor);
  labelCandidateColor->setGeometry(10, 170, 200, 30);

  comboCandidateColor = new QComboBox(false, this, tr("Candidate Point Color"));
  CHECK_PTR_ENGAUGE(comboCandidateColor);
  comboCandidateColor->setGeometry(220, 170, 130, 30);
  comboCandidateColor->setEditable(false);
  comboCandidateColor->insertStringList(PointSetStyles::instance().colorNames());
  QWhatsThis::add(comboCandidateColor, QString(tr("Select a color for the point being decided upon")));
  connect(comboCandidateColor, SIGNAL(activated(const QString &)), this, SLOT(slotCandidateColor(const QString &)));

  // preview area
  labelPreview = new QLabel(QString(tr("Preview")), this);
  CHECK_PTR_ENGAUGE(labelPreview);
  labelPreview->setGeometry(20, 210, 100, 20);

  previewCanvas = new Q3Canvas(this);
  CHECK_PTR_ENGAUGE(previewCanvas);
  previewCanvas->resize(PreviewWidth, PreviewHeight);

  previewCanvasView = new Q3CanvasView(previewCanvas, this, 0, Qt::WDestructiveClose);
  CHECK_PTR_ENGAUGE(previewCanvasView);
  previewCanvasView->setGeometry(20, 230, PreviewWidth, PreviewHeight);
  previewCanvasView->setHScrollBarMode(Q3ScrollView::AlwaysOff);
  previewCanvasView->setVScrollBarMode(Q3ScrollView::AlwaysOff);
  QWhatsThis::add(previewCanvasView, QString(tr("Preview window shows how current settings affect "
    "point matching, and how the marked and candidate points are displayed.\n\nThe points are separated "
    "by the point separation value, and the maximum point size is shown as a box in the center")));

  // navigation buttons
  buttonOk = new QPushButton(tr("OK"), this);
  CHECK_PTR_ENGAUGE(buttonOk);
  buttonOk->setGeometry(60, 350, 100, 30);
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(accept()));

  buttonCancel = new QPushButton(tr("Cancel"), this);
  CHECK_PTR_ENGAUGE(buttonCancel);
  buttonCancel->setGeometry(170, 350, 100, 30);
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));

  buttonWhat = new QPushButton(this);
  CHECK_PTR_ENGAUGE(buttonWhat);
  buttonWhat->setPixmap(QPixmap(whatsthis_xpm));
  buttonWhat->setGeometry(290, 350, 60, 30);
  QToolTip::add(buttonWhat, QString(tr("What's this?")));
  connect(buttonWhat, SIGNAL(clicked()), this, SLOT(slotWhat()));

  setPointSetStyles();
  loadPointImages();
  toggleActions();
  updatePreview();
}

PointMatchDlg::~PointMatchDlg()
{
  DigitDebug::dtor(QString("pointmatchdlg ") + QString::number((ulong) this, 16));

  if (highlight != 0)
    delete highlight;

  pointsetAccepted.removePoints();
  pointsetRejected.removePoints();
}

void PointMatchDlg::drawPoint(QImage* imagePreview, const QImage &imagePoint,
  int xCenter, int yCenter, const Discretize &discretize, DiscretizeSettings disc)
{
  int wPoint = imagePoint.width();
  int hPoint = imagePoint.height();

  for (int xPoint = 0; xPoint < wPoint; xPoint++)
    for (int yPoint = 0; yPoint < hPoint; yPoint++)
    {
      ASSERT_ENGAUGE(0 <= xPoint);
      ASSERT_ENGAUGE(xPoint < wPoint);
      ASSERT_ENGAUGE(0 <= yPoint);
      ASSERT_ENGAUGE(yPoint < hPoint);
      int value = discretize.discretizeValueNotForeground(&imagePoint, xPoint, yPoint, DiscretizeIntensity);
      if (discretize.pixelIsOn(value, disc))
      {
        int xPreview = xCenter - (wPoint / 2 - xPoint);
        int yPreview = yCenter - (hPoint / 2 - yPoint);

        ASSERT_ENGAUGE(0 <= xPreview);
        ASSERT_ENGAUGE(xPreview < PreviewWidth);
        ASSERT_ENGAUGE(0 <= yPreview);
        ASSERT_ENGAUGE(yPreview < PreviewHeight);
        setPixelRGB(imagePreview, xPreview, yPreview, pixelRGB(&imagePoint, xPoint, yPoint));
      }
    }
}

void PointMatchDlg::loadPointImages()
{
  QPixmap pixmapDiamond(matchdiamond_xpm);
  QPixmap pixmapCircle(matchcircle_xpm);
  QPixmap pixmapSquare(matchsquare_xpm);

  imageDiamond = pixmapDiamond.convertToImage();
  imageCircle = pixmapCircle.convertToImage();
  imageSquare = pixmapSquare.convertToImage();
}

void PointMatchDlg::makePoints()
{
  if (highlight != 0)
  {
    delete highlight;
    highlight = 0;
  }

  pointsetAccepted.removePoints();
  pointsetRejected.removePoints();

  QImage imagePreview(PreviewWidth, PreviewHeight, 32);
  imagePreview.fill(Qt::white);
  
  if (validateSettings())
  {
    // show box around central point that represents the maximum sample point size
    int left = PreviewWidth / 2 - settings->pointSize / 2;
    int top = PreviewHeight / 2 - settings->pointSize / 2;

    int x;
    for (x = left; x <= left + settings->pointSize; x++)
      for (int y = top; y <= top + settings->pointSize; y++)
        setPixelRGB(&imagePreview, x, y, Qt::lightGray);

    setPointSetStyles();

    // implicit assumption that all point images have same size
    int wPoint = imageDiamond.width();
    ASSERT_ENGAUGE(wPoint == imageCircle.width());
    ASSERT_ENGAUGE(wPoint == imageSquare.width());

    // manually set the local discretize settings, which are only used for working with the point images
    Discretize discretize;
    DiscretizeSettings disc;
    disc.discretizeMethod = DiscretizeIntensity;
    disc.intensityThresholdLow = 0;
    disc.intensityThresholdHigh = 90;

    // start at center and move to left until we reach the left side
    int xCenter = PreviewWidth / 2;
    x = xCenter;
    while (x - settings->pointSeparation - (wPoint - 1) / 2 > 0)
      x -= settings->pointSeparation;

    // draw columns of points across the canvas
    int y [] = {PreviewHeight / 4, PreviewHeight / 2, (3 * PreviewHeight) / 4};
    QImage* images [] = {&imageDiamond, &imageCircle, &imageSquare};
    Point* p;
    QList<QRect> updateRectList;
    do
    {
      // rows are top (diamonds), middle (circles) and bottom (squares)
      for (int row = 0; row < 3; row++)
      {
        // original image points
        drawPoint(&imagePreview, *(images [row]), x, y [row], discretize, disc);

        // engauge digitizer points
        ASSERT_ENGAUGE(previewCanvas != 0);
        p = new Point(x, y [row], previewCanvas);
        CHECK_PTR_ENGAUGE(p);

        if (row == 0)
        {
          pointsetAccepted.addPoint(previewCanvas, p, &updateRectList);
          p->setPointSet(&pointsetAccepted);
        }
        else if (row == 1)
        {
          pointsetRejected.addPoint(previewCanvas, p, &updateRectList);
          p->setPointSet(&pointsetRejected);
        }
        else
        {
          // no points for candidate row
        }
      }

      x += settings->pointSeparation;
    } while (x + (wPoint - 1) / 2 < PreviewWidth);

    // show single highlight in middlemost ponit
    highlight = new Highlight(settings->candidateColor, previewCanvas);
    CHECK_PTR_ENGAUGE(highlight);
    highlight->moveCenter(xCenter, y [2]);
  }

  // save original image points as background
  QPixmap pixmapPreview;
  pixmapPreview.convertFromImage(imagePreview);
  ASSERT_ENGAUGE(previewCanvas != 0);
  previewCanvas->setBackgroundPixmap(pixmapPreview);
}

void PointMatchDlg::setPointSetStyles()
{
  // set style of engauge points
  style.lineColor = DigitTransparent;
  style.pointInColor = settings->acceptedColor;
  style.pointLineColor = settings->acceptedColor;
  pointsetAccepted.setStyle(style);

  style.pointInColor = settings->rejectedColor;
  style.pointLineColor = settings->rejectedColor;
  pointsetRejected.setStyle(style);
}

void PointMatchDlg::toggleActions()
{
  ASSERT_ENGAUGE(comboAcceptedColor != 0);
  comboAcceptedColor->setCurrentText(PointSetStyles::instance().colorName(settings->acceptedColor));
  ASSERT_ENGAUGE(comboRejectedColor != 0);
  comboRejectedColor->setCurrentText(PointSetStyles::instance().colorName(settings->rejectedColor));
  ASSERT_ENGAUGE(comboCandidateColor != 0);
  comboCandidateColor->setCurrentText(PointSetStyles::instance().colorName(settings->candidateColor));

  ASSERT_ENGAUGE(buttonOk != 0);
  buttonOk->setEnabled(validateSettings());
}

void PointMatchDlg::updatePreview()
{
  makePoints();

  ASSERT_ENGAUGE(previewCanvas != 0);
  previewCanvasView->updateContents();
}

bool PointMatchDlg::validateSettings()
{
  // enforce constraints
  ASSERT_ENGAUGE(settings != 0);
  if (settings->pointSeparation < PointMatchSeparationMin)
    return false;
  if (settings->pointSize < PointMatchSizeMin)
    return false;
    
  return true;
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void PointMatchDlg::slotAcceptedColor(const QString &text)
{
  ASSERT_ENGAUGE(settings != 0);
  settings->acceptedColor = PointSetStyles::instance().color(text);
  updatePreview();
}

void PointMatchDlg::slotCandidateColor(const QString &text)
{
  ASSERT_ENGAUGE(settings != 0);
  settings->candidateColor = PointSetStyles::instance().color(text);
  updatePreview();
}

void PointMatchDlg::slotPointSeparation(const QString &text)
{
  ASSERT_ENGAUGE(settings != 0);
  if (text == QString(""))
    settings->pointSeparation = 0;
  else
    settings->pointSeparation = text.toInt();
  toggleActions();
  updatePreview();
}

void PointMatchDlg::slotPointSize(const QString &text)
{
  ASSERT_ENGAUGE(settings != 0);
  if (text == QString(""))
    settings->pointSize = 0;
  else
    settings->pointSize = text.toInt();
  toggleActions();
  updatePreview();
}

void PointMatchDlg::slotRejectedColor(const QString &text)
{
  ASSERT_ENGAUGE(settings != 0);
  settings->rejectedColor = PointSetStyles::instance().color(text);
  updatePreview();
}

void PointMatchDlg::slotWhat()
{
  QWhatsThis::enterWhatsThisMode();
}
