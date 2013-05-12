/***************************************************************************
                          pointsetdlg.cpp  -  description
                             -------------------
    begin                : Wed Oct 30 2002
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

#include <QWhatsThis>
#include <qtooltip.h>
#include <qregexp.h>
//Added by qt3to4:
#include <QList>
#include <QPixmap>
#include <QLabel>

#include "pointsetdlg.h"
#include "digitdef.h"
#include "pointsetstyles.h"
#include "point.h"
#include "pointset.h"
#include "img/whatsthis.xpm"
#include "digitdebug.h"
#include "main.h"

PointSetDlg::PointSetDlg(QString title, QString pointsetName,
  DigitizeState state, PointSetStyle* pointSetStyle) :
  QDialog(0, "pointsetdlg", true),
  state(state),
  pointSetStyle(pointSetStyle)
{
  DigitDebug::ctor(QString("pointsetdlg ") + QString::number((ulong) this, 16));
  
  setCaption(curveReplace("Curve Properties - [%1%2]%3")
    .arg(title)
    .arg(pointsetNameFilter(pointsetName))
    .arg(freewareCaption));
    
  setFixedSize(340, 520);

  // point area
  groupPoint = new Q3GroupBox(QString(tr("Point")), this);
  CHECK_PTR_ENGAUGE(groupPoint);
  groupPoint->setGeometry(10, 10, 320, 220);
  
  labelShape = new QLabel(QString(tr("Shape:")), this);
  CHECK_PTR_ENGAUGE(labelShape);
  labelShape->setGeometry(20, 30, 120, 30);

  comboShape = new QComboBox(false, this, tr("Shape"));
  CHECK_PTR_ENGAUGE(comboShape);
  comboShape->setGeometry(145, 30, 175, 30);
  comboShape->setEditable(false);
  comboShape->insertStringList(PointSetStyles::instance().pointShapeNames());
  QWhatsThis::add(comboShape, QString(tr("Select a shape for the points")));
  connect(comboShape, SIGNAL(activated(const QString &)), this, SLOT(slotPointShape(const QString &)));
  
  labelShapeSize = new QLabel(QString(tr("Size:")), this);
  CHECK_PTR_ENGAUGE(labelShapeSize);
  labelShapeSize->setGeometry(20, 70, 120, 30);

  comboShapeSize = new QComboBox(false, this, tr("Shape Size"));
  CHECK_PTR_ENGAUGE(comboShapeSize);
  comboShapeSize->setGeometry(145, 70, 175, 30);
  comboShapeSize->setEditable(false);
  comboShapeSize->insertStringList(PointSetStyles::instance().pointSizeNames());
  QWhatsThis::add(comboShapeSize, QString(tr("Select an overall size for the points")));
  connect(comboShapeSize, SIGNAL(activated(const QString &)), this, SLOT(slotPointSize(const QString &)));

  labelShapeLineSize = new QLabel(QString(tr("Line Size:")), this);
  CHECK_PTR_ENGAUGE(labelShapeLineSize);
  labelShapeLineSize->setGeometry(20, 110, 120, 30);

  comboShapeLineSize = new QComboBox(false, this, tr("Shape Line Size"));
  CHECK_PTR_ENGAUGE(comboShapeLineSize);
  comboShapeLineSize->setGeometry(145, 110, 175, 30);
  comboShapeLineSize->setEditable(false);
  comboShapeLineSize->insertStringList(PointSetStyles::instance().pointLineSizeNames());
  QWhatsThis::add(comboShapeLineSize, QString(tr("Select a size for the line used to draw the point shapes")));
  connect(comboShapeLineSize, SIGNAL(activated(const QString &)), this, SLOT(slotPointLineSize(const QString &)));

  labelShapeLineColor = new QLabel(QString(tr("Line Color:")), this);
  CHECK_PTR_ENGAUGE(labelShapeLineColor);
  labelShapeLineColor->setGeometry(20, 150, 120, 30);

  comboShapeLineColor = new QComboBox(false, this, tr("Shape Line Color"));
  CHECK_PTR_ENGAUGE(comboShapeLineColor);
  comboShapeLineColor->setGeometry(145, 150, 175, 30);
  comboShapeLineColor->setEditable(false);
  comboShapeLineColor->insertStringList(PointSetStyles::instance().colorNames());
  QWhatsThis::add(comboShapeLineColor, QString(tr("Select a color for the line used to draw the point shapes")));
  connect(comboShapeLineColor, SIGNAL(activated(const QString &)), this, SLOT(slotPointLineColor(const QString &)));
  
  labelShapeInColor = new QLabel(QString(tr("Interior Color:")), this);
  CHECK_PTR_ENGAUGE(labelShapeInColor);
  labelShapeInColor->setGeometry(20, 190, 120, 30);

  comboShapeInColor = new QComboBox(false, this, tr("Shape Interior Color"));
  CHECK_PTR_ENGAUGE(comboShapeInColor);
  comboShapeInColor->setGeometry(145, 190, 175, 30);
  comboShapeInColor->setEditable(false);
  comboShapeInColor->insertStringList(PointSetStyles::instance().colorNames());
  QWhatsThis::add(comboShapeInColor, QString(tr("Select a color for the interior of the point shapes.\n\n"
    "This has no effect on points without interiors")));
  connect(comboShapeInColor, SIGNAL(activated(const QString &)), this, SLOT(slotPointInColor(const QString &)));

  // line area
  groupLine = new Q3GroupBox(QString(tr("Line")), this);
  CHECK_PTR_ENGAUGE(groupLine);
  groupLine->setGeometry(10, 240, 320, 140);

  labelLineSize = new QLabel(QString(tr("Size:")), this);
  CHECK_PTR_ENGAUGE(labelLineSize);
  labelLineSize->setGeometry(20, 260, 120, 30);

  comboLineSize = new QComboBox(false, this, tr("Line Size"));
  CHECK_PTR_ENGAUGE(comboLineSize);
  comboLineSize->setGeometry(145, 260, 175, 30);
  comboLineSize->setEditable(false);
  comboLineSize->insertStringList(PointSetStyles::instance().lineSizeNames());
  QWhatsThis::add(comboLineSize, QString(tr("Select a size for the lines drawn between points")));
  connect(comboLineSize, SIGNAL(activated(const QString &)), this, SLOT(slotLineSize(const QString &)));

  labelLineColor = new QLabel(QString(tr("Color:")), this);
  CHECK_PTR_ENGAUGE(labelLineColor);
  labelLineColor->setGeometry(20, 300, 120, 30);

  comboLineColor = new QComboBox(false, this, tr("Line Color"));
  CHECK_PTR_ENGAUGE(comboLineColor);
  comboLineColor->setGeometry(145, 300, 175, 30);
  comboLineColor->setEditable(false);
  comboLineColor->insertStringList(PointSetStyles::instance().colorNames());
  QWhatsThis::add(comboLineColor, QString(tr("Select a color for the lines drawn between points")));
  connect(comboLineColor, SIGNAL(activated(const QString &)), this, SLOT(slotLineColor(const QString &)));

  labelLineConnectAs = new QLabel(QString(tr("Connect As:")), this);
  CHECK_PTR_ENGAUGE(labelLineConnectAs);
  labelLineConnectAs->setGeometry(20, 340, 120, 30);

  comboLineConnectAs = new QComboBox(false, this, tr("Connect As"));
  CHECK_PTR_ENGAUGE(comboLineConnectAs);
  comboLineConnectAs->setGeometry(145, 340, 175, 30);
  comboLineConnectAs->setEditable(false);
  if (state != CurveState)               
    comboLineConnectAs->setEnabled(false); // can only change Connect As method of curves
  comboLineConnectAs->insertStringList(PointSetStyles::instance().lineConnectAsNames());
  QWhatsThis::add(comboLineConnectAs, QString(tr("Select rule for connecting points with lines.\n\n"
    "If the curve is connected as a single-valued function then the points are ordered by "
    "increasing value of the independent variable.\n\n"
    "If the curve is connected as a closed contour, then the points are ordered by age, except for "
    "points placed along an existing line. Any point placed on top of any existing line is inserted "
    "between the two endpoints of that line - as if its age was between the ages of the two "
    "endpoints.\n\n"
    "Lines are drawn between successively ordered points")));
  connect(comboLineConnectAs, SIGNAL(activated(const QString &)), this, SLOT(slotLineConnectAs(const QString &)));

  // preview area
  labelPreview = new QLabel(QString(tr("Preview")), this);
  CHECK_PTR_ENGAUGE(labelPreview);
  labelPreview->setGeometry(10, 390, 115, 20);

  previewCanvas = new Q3Canvas(this);
  CHECK_PTR_ENGAUGE(previewCanvas);
  previewCanvas->resize(360, 50);

  previewCanvasView = new Q3CanvasView(previewCanvas, this);
  CHECK_PTR_ENGAUGE(previewCanvasView);
  previewCanvasView->setGeometry(10, 420, 320, 50);
  previewCanvasView->setHScrollBarMode(Q3ScrollView::AlwaysOff);
  previewCanvasView->setVScrollBarMode(Q3ScrollView::AlwaysOff);
  QWhatsThis::add(previewCanvasView, QString(tr(
    "Preview window shows how current settings affect the lines and points")));

  pointSet = new PointSet;
  CHECK_PTR_ENGAUGE(pointSet);
  pointSet->setStyle(*pointSetStyle);

  previewLeftPoint = new Point(40, 25, previewCanvas);
  CHECK_PTR_ENGAUGE(previewLeftPoint);
  previewRightPoint = new Point(250, 25, previewCanvas);
  CHECK_PTR_ENGAUGE(previewRightPoint);
  QList<QRect> updateRectList;
  pointSet->addPoint(previewCanvas, previewLeftPoint, &updateRectList);
  pointSet->addPoint(previewCanvas, previewRightPoint, &updateRectList);
  previewLeftPoint->setPointSet(pointSet);
  previewRightPoint->setPointSet(pointSet);

  // navigation buttons
  buttonOk = new QPushButton(tr("OK"), this);
  CHECK_PTR_ENGAUGE(buttonOk);
  buttonOk->setGeometry(10, 480, 100, 30);
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(accept()));

  buttonCancel = new QPushButton(tr("Cancel"), this);
  CHECK_PTR_ENGAUGE(buttonCancel);
  buttonCancel->setGeometry(140, 480, 100, 30);
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));

  buttonWhat = new QPushButton(this);
  CHECK_PTR_ENGAUGE(buttonWhat);
  buttonWhat->setPixmap(QPixmap(whatsthis_xpm));
  buttonWhat->setGeometry(270, 480, 60, 30);
  QToolTip::add(buttonWhat, QString("What's this?"));
  connect(buttonWhat, SIGNAL(clicked()), this, SLOT(slotWhat()));

  toggleActions();
  updatePreview();
}

PointSetDlg::~PointSetDlg()
{
  DigitDebug::dtor(QString("pointsetdlg ") + QString::number((ulong) this, 16));
  
  delete pointSet;

  delete buttonOk;
  delete buttonCancel;
}

void PointSetDlg::toggleActions()
{
  ASSERT_ENGAUGE(comboShape != 0);
  comboShape->setCurrentText(PointSetStyles::instance().pointShapeName(pointSetStyle->pointShape));
  ASSERT_ENGAUGE(comboShapeSize != 0);
  comboShapeSize->setCurrentText(PointSetStyles::instance().pointSizeName(pointSetStyle->pointSize));
  ASSERT_ENGAUGE(comboShapeLineSize != 0);
  comboShapeLineSize->setCurrentText(PointSetStyles::instance().pointLineSizeName(pointSetStyle->pointLineSize));
  ASSERT_ENGAUGE(comboShapeLineColor != 0);
  comboShapeLineColor->setCurrentText(PointSetStyles::instance().colorName(pointSetStyle->pointLineColor));
  ASSERT_ENGAUGE(comboShapeInColor != 0);
  comboShapeInColor->setCurrentText(PointSetStyles::instance().colorName(pointSetStyle->pointInColor));
  ASSERT_ENGAUGE(comboLineSize != 0);
  comboLineSize->setCurrentText(PointSetStyles::instance().lineSizeName(pointSetStyle->lineSize));
  ASSERT_ENGAUGE(comboLineColor != 0);
  comboLineColor->setCurrentText(PointSetStyles::instance().colorName(pointSetStyle->lineColor));
  ASSERT_ENGAUGE(comboLineConnectAs != 0);
  comboLineConnectAs->setCurrentText(PointSetStyles::instance().lineConnectAsName(pointSetStyle->lineConnectAs));
}

void PointSetDlg::updatePreview()
{
  ASSERT_ENGAUGE(pointSet != 0);
  pointSet->setStyle(*pointSetStyle);

  ASSERT_ENGAUGE(previewCanvasView != 0);
  previewCanvasView->updateContents();
}

QString PointSetDlg::curveReplace(const char* text)
{
  QString rtn(text);

  switch (state)
  {
  case AxisState:
    rtn.replace(QRegExp("curve"), QString("axes"));
    rtn.replace(QRegExp("Curve"), QString("Axes"));
    break;
  case CurveState:
    break;
  case MeasureState:
    rtn.replace(QRegExp("curve"), QString("measure"));
    rtn.replace(QRegExp("Curve"), QString("Measure"));
    break;
  case ScaleState:
    rtn.replace(QRegExp("curve"), QString("scale bar"));
    rtn.replace(QRegExp("Curve"), QString("Scale bar"));
    break;
  default:
    FATAL_ENGAUGE("bad value for state");
  }

  return rtn;
}

QString PointSetDlg::pointsetNameFilter(const QString pointsetName)
{
  if ((state == CurveState) ||
    (state == MeasureState))
    return QString(" - ") + pointsetName;
  else
    return QString("");
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void PointSetDlg::slotPointShape(const QString &text)
{
  ASSERT_ENGAUGE(pointSetStyle != 0);
  pointSetStyle->pointShape = PointSetStyles::instance().pointShape(text);
  updatePreview();
}

void PointSetDlg::slotPointSize(const QString &text)
{
  ASSERT_ENGAUGE(pointSetStyle != 0);
  pointSetStyle->pointSize = PointSetStyles::instance().pointSize(text);
  updatePreview();
}

void PointSetDlg::slotPointLineSize(const QString &text)
{
  ASSERT_ENGAUGE(pointSetStyle != 0);
  pointSetStyle->pointLineSize = PointSetStyles::instance().pointLineSize(text);
  updatePreview();
}

void PointSetDlg::slotPointLineColor(const QString &text)
{
  ASSERT_ENGAUGE(pointSetStyle != 0);
  pointSetStyle->pointLineColor = PointSetStyles::instance().color(text);
  updatePreview();
}

void PointSetDlg::slotPointInColor(const QString &text)
{
  ASSERT_ENGAUGE(pointSetStyle != 0);
  pointSetStyle->pointInColor = PointSetStyles::instance().color(text);
  updatePreview();
}

void PointSetDlg::slotLineSize(const QString &text)
{
  ASSERT_ENGAUGE(pointSetStyle != 0);
  pointSetStyle->lineSize = PointSetStyles::instance().lineSize(text);
  updatePreview();
}

void PointSetDlg::slotLineColor(const QString &text)
{
  ASSERT_ENGAUGE(pointSetStyle != 0);
  pointSetStyle->lineColor = PointSetStyles::instance().color(text);
  updatePreview();
}

void PointSetDlg::slotLineConnectAs(const QString &text)
{
  ASSERT_ENGAUGE(pointSetStyle != 0);
  pointSetStyle->lineConnectAs = PointSetStyles::instance().lineConnectAs(text);
  updatePreview();
}

void PointSetDlg::slotWhat()
{
  QWhatsThis::enterWhatsThisMode();
}
