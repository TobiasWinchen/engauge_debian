/***************************************************************************
                          exportdlg.cpp  -  description
                             -------------------
    begin                : Fri Nov 15 2002
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


#include <qtooltip.h>
//Added by qt3to4:
#include <Q3TextStream>
#include <QPixmap>
#include <QLabel>
#include <QWhatsThis>
#include "exportdlg.h"
#include "img/whatsthis.xpm"
#include "pointsets.h"
#include "digitdebug.h"
#include "main.h"

ExportDlg::ExportDlg(QString title, PointSets* pointSets, const CoordSettings* coord,
  const GridMeshSettings* grid, ExportSettings* xport) :
  QDialog(0, "exportdlg", true),
  pointSets(pointSets),
  coord(coord),
  grid(grid),
  xport(xport)
{
  DigitDebug::ctor(QString("exportdlg ") + QString::number((ulong) this, 16));
  
  setCaption(tr("Export Settings - [%1]%2")
    .arg(title)
    .arg(freewareCaption));

  setFixedSize(380, 650);

  // curve selection
  groupCurveSelection = new Q3GroupBox(QString(tr("Curve Selection")), this);
  CHECK_PTR_ENGAUGE(groupCurveSelection);
  groupCurveSelection->setGeometry(10, 10, 360, 150);
  
  labelIncluded = new QLabel(QString(tr("Included")), groupCurveSelection);
  CHECK_PTR_ENGAUGE(labelIncluded);
  labelIncluded->setGeometry(10, 20, 110, 20);

  labelExcluded = new QLabel(QString(tr("Not included")), groupCurveSelection);
  CHECK_PTR_ENGAUGE(labelExcluded);
  labelExcluded->setGeometry(240, 20, 110, 20);
  
  listIncluded = new Q3ListBox(groupCurveSelection);
  CHECK_PTR_ENGAUGE(listIncluded);
  listIncluded->setGeometry(10, 40, 110, 100);
  QWhatsThis::add(listIncluded, QString(tr("List of curves to be included in the exported file")));
  connect(listIncluded, SIGNAL(selectionChanged(Q3ListBoxItem*)), this, SLOT(slotIncluded(Q3ListBoxItem*)));
  
  listExcluded = new Q3ListBox(groupCurveSelection);
  CHECK_PTR_ENGAUGE(listExcluded);
  listExcluded->setGeometry(240, 40, 110, 100);
  QWhatsThis::add(listExcluded, QString(tr("List of curves to be excluded from the exported file")));
  connect(listExcluded, SIGNAL(selectionChanged(Q3ListBoxItem*)), this, SLOT(slotExcluded(Q3ListBoxItem*)));
  
  buttonInclude = new QPushButton(QString(tr("<<Include")), groupCurveSelection);
  CHECK_PTR_ENGAUGE(buttonInclude);
  buttonInclude->setGeometry(130, 40, 100, 30);
  QWhatsThis::add(buttonInclude, QString(tr("Move the currently selected curve from the excluded list "
    "to the included list")));
  connect(buttonInclude, SIGNAL(clicked()), this, SLOT(slotInclude()));
  
  buttonExclude = new QPushButton(QString(tr("Exclude>>")), groupCurveSelection);
  CHECK_PTR_ENGAUGE(buttonExclude);
  buttonExclude->setGeometry(130, 80, 100, 30);
  QWhatsThis::add(buttonExclude, QString(tr("Move the currently selected curve from the included list "
    "to the excluded list")));
  connect(buttonExclude, SIGNAL(clicked()), this, SLOT(slotExclude()));
  
  // points selection
  groupPointsSelection = new Q3ButtonGroup(QString(tr("Points Selection")), this);
  CHECK_PTR_ENGAUGE(groupPointsSelection);
  groupPointsSelection->setGeometry(10, 170, 360, 110);

  buttonXAllCurves = new QRadioButton(QString(tr("Interpolate Y's at X's from all curves")),
    groupPointsSelection);
  CHECK_PTR_ENGAUGE(buttonXAllCurves);
  buttonXAllCurves->setGeometry(10, 20, 300, 20);
  QWhatsThis::add(buttonXAllCurves, QString(tr("Exported file will have values at every unique X "
    "value from every curve. Y values will be linearly interpolated if necessary")));
  connect(buttonXAllCurves, SIGNAL(toggled(bool)), this, SLOT(slotXAllCurves(bool)));

  buttonXFirstCurve = new QRadioButton(QString(tr("Interpolate Y's at X's from first curve")),
    groupPointsSelection);
  CHECK_PTR_ENGAUGE(buttonXFirstCurve);
  buttonXFirstCurve->setGeometry(10, 40, 300, 20);
  QWhatsThis::add(buttonXFirstCurve, QString(tr("Exported file will have values at every unique X "
    "value from the first curve. Y values will be linearly interpolated if necessary")));
  connect(buttonXFirstCurve, SIGNAL(toggled(bool)), this, SLOT(slotXFirstCurve(bool)));

  buttonXGridLines = new QRadioButton(QString(tr("Interpolate Y's at grid line X's")), groupPointsSelection);
  CHECK_PTR_ENGAUGE(buttonXGridLines);
  buttonXGridLines->setGeometry(10, 60, 300, 20);
  QWhatsThis::add(buttonXGridLines, QString(tr("Exported file will have values at every unique X "
    "grid line. Y values will be linearly interpolated if necessary")));
  connect(buttonXGridLines, SIGNAL(toggled(bool)), this, SLOT(slotXGridLines(bool)));

  buttonXYAllCurves = new QRadioButton(QString(tr("Raw X's and Y's")), groupPointsSelection);
  CHECK_PTR_ENGAUGE(buttonXYAllCurves);
  buttonXYAllCurves->setGeometry(10, 80, 300, 20);
  QWhatsThis::add(buttonXYAllCurves, QString(tr("Exported file will have only original X and Y values "
    "without any interpolation")));
  connect(buttonXYAllCurves, SIGNAL(toggled(bool)), this, SLOT(slotXYAllCurves(bool)));

  // layout
  groupLayout = new Q3ButtonGroup(QString(tr("Layout")), this);
  CHECK_PTR_ENGAUGE(groupLayout);
  groupLayout->setGeometry(10, 290, 360, 70);

  buttonAllCurvesOnEachLine = new QRadioButton(QString(tr("All curves on each line")), groupLayout);
  CHECK_PTR_ENGAUGE(buttonAllCurvesOnEachLine);
  buttonAllCurvesOnEachLine->setGeometry(10, 20, 210, 20);
  QWhatsThis::add(buttonAllCurvesOnEachLine, QString(tr("Exported file will have, on each line, "
    "an X value, the Y value for the first curve, the Y value for the second curve,...")));
  connect(buttonAllCurvesOnEachLine, SIGNAL(toggled(bool)), this, SLOT(slotAllCurvesOnEachLine(bool)));

  buttonOneCurveOnEachLine = new QRadioButton(QString(tr("One curve on each line")), groupLayout);
  CHECK_PTR_ENGAUGE(buttonOneCurveOnEachLine);
  buttonOneCurveOnEachLine->setGeometry(10, 40, 210, 20);
  QWhatsThis::add(buttonOneCurveOnEachLine, QString(tr("Exported file will have all the points for "
    "the first curve, with one X-Y pair on each line, then the points for the second curve,...")));
  connect(buttonOneCurveOnEachLine, SIGNAL(toggled(bool)), this, SLOT(slotOneCurveOnEachLine(bool)));

  // delimiters
  groupDelimiters = new Q3ButtonGroup(QString(tr("Delimiters")), this);
  CHECK_PTR_ENGAUGE(groupDelimiters);
  groupDelimiters->setGeometry(10, 370, 175, 90);

  buttonCommas = new QRadioButton(QString(tr("Commas")), groupDelimiters);
  CHECK_PTR_ENGAUGE(buttonCommas);
  buttonCommas->setGeometry(10, 20, 110, 20);
  QWhatsThis::add(buttonCommas, QString(tr("Exported file will have commas between adjacent values")));
  connect(buttonCommas, SIGNAL(toggled(bool)), this, SLOT(slotCommas(bool)));

  buttonSpaces = new QRadioButton(QString(tr("Spaces")), groupDelimiters);
  CHECK_PTR_ENGAUGE(buttonSpaces);
  buttonSpaces->setGeometry(10, 40, 110, 20);
  QWhatsThis::add(buttonSpaces, QString(tr("Exported file will have spaces between adjacent values")));
  connect(buttonSpaces, SIGNAL(toggled(bool)), this, SLOT(slotSpaces(bool)));

  buttonTabs = new QRadioButton(QString(tr("Tabs")), groupDelimiters);
  CHECK_PTR_ENGAUGE(buttonTabs);
  buttonTabs->setGeometry(10, 60, 110, 20);
  QWhatsThis::add(buttonTabs, QString(tr("Exported file will have tabs between adjacent values")));
  connect(buttonTabs, SIGNAL(toggled(bool)), this, SLOT(slotTabs(bool)));

  // header
  groupHeader = new Q3ButtonGroup(QString(tr("Header")), this);
  CHECK_PTR_ENGAUGE(groupHeader);
  groupHeader->setGeometry(195, 370, 175, 90);

  buttonNone = new QRadioButton(QString(tr("None")), groupHeader);
  CHECK_PTR_ENGAUGE(buttonNone);
  buttonNone->setGeometry(10, 20, 110, 20);
  QWhatsThis::add(buttonNone, QString(tr("Exported file will have no header line")));
  connect(buttonNone, SIGNAL(toggled(bool)), this, SLOT(slotNone(bool)));

  buttonSimple = new QRadioButton(QString(tr("Simple")), groupHeader);
  CHECK_PTR_ENGAUGE(buttonSimple);
  buttonSimple->setGeometry(10, 40, 110, 20);
  QWhatsThis::add(buttonSimple, QString(tr("Exported file will have simple header line")));
  connect(buttonSimple, SIGNAL(toggled(bool)), this, SLOT(slotSimple(bool)));

  buttonGnuplot = new QRadioButton(QString(tr("Gnuplot")), groupHeader);
  CHECK_PTR_ENGAUGE(buttonGnuplot);
  buttonGnuplot->setGeometry(10, 60, 110, 20);
  QWhatsThis::add(buttonGnuplot, QString(tr("Exported file will have gnuplot header line")));
  connect(buttonGnuplot, SIGNAL(toggled(bool)), this, SLOT(slotGnuplot(bool)));

  // preview
  labelPreview = new QLabel(QString(tr("Preview")), this);
  CHECK_PTR_ENGAUGE(labelPreview);
  labelPreview->setGeometry(10, 470, 100, 20);

  listPreview = new Q3ListBox(this);
  CHECK_PTR_ENGAUGE(listPreview);
  listPreview->setGeometry(10, 490, 360, 110);
  QWhatsThis::add(listPreview, QString(tr("Preview window shows how current settings affect the exported file")));

  // navigation buttons
  buttonOk = new QPushButton(tr("OK"), this);
  CHECK_PTR_ENGAUGE(buttonOk);
  buttonOk->setGeometry(45, 610, 100, 30);
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(accept()));

  buttonCancel = new QPushButton(tr("Cancel"), this);
  CHECK_PTR_ENGAUGE(buttonCancel);
  buttonCancel->setGeometry(175, 610, 100, 30);
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));

  buttonWhat = new QPushButton(this);
  CHECK_PTR_ENGAUGE(buttonWhat);
  buttonWhat->setPixmap(QPixmap(whatsthis_xpm));
  buttonWhat->setGeometry(305, 610, 60, 30);
  QToolTip::add(buttonWhat, QString(tr("What's this?")));
  connect(buttonWhat, SIGNAL(clicked()), this, SLOT(slotWhat()));

  resetLists();    
  toggleActions();
  updatePreview();
}

ExportDlg::~ExportDlg()
{
  DigitDebug::dtor(QString("exportdlg ") + QString::number((ulong) this, 16));
}

void ExportDlg::resetLists()
{
  ASSERT_ENGAUGE(listIncluded != 0);
  listIncluded->clear();
  ASSERT_ENGAUGE(listExcluded != 0);
  listExcluded->clear();

  ASSERT_ENGAUGE(pointSets != 0);
  listIncluded->insertStringList(pointSets->exportIncluded());
  listExcluded->insertStringList(pointSets->exportExcluded());
}

void ExportDlg::toggleActions()
{
  ASSERT_ENGAUGE(buttonInclude != 0);
  buttonInclude->setEnabled(!listExcluded->currentText().isNull());
  ASSERT_ENGAUGE(buttonExclude != 0);
  buttonExclude->setEnabled(!listIncluded->currentText().isNull());

  ASSERT_ENGAUGE(xport != 0);
  ASSERT_ENGAUGE(buttonXAllCurves != 0);
  ASSERT_ENGAUGE(buttonXFirstCurve != 0);
  ASSERT_ENGAUGE(buttonXGridLines != 0);
  ASSERT_ENGAUGE(buttonXYAllCurves != 0);
  switch (xport->pointsSelection)
  {
    case XFromAllCurves:
      buttonXAllCurves->setChecked(true);
      break;
    case XFromFirstCurve:
      buttonXFirstCurve->setChecked(true);
      break;
    case XFromGridLines:
      buttonXGridLines->setChecked(true);
      break;
    case XYFromAllCurves:
      buttonXYAllCurves->setChecked(true);
      break;
  }

  ASSERT_ENGAUGE(buttonAllCurvesOnEachLine != 0);
  ASSERT_ENGAUGE(buttonOneCurveOnEachLine != 0);
  switch (xport->layout)
  {
    case AllCurvesOnEachLine:
      buttonAllCurvesOnEachLine->setChecked(true);
      break;
    case OneCurveOnEachLine:
      buttonOneCurveOnEachLine->setChecked(true);
      break;
  }

  ASSERT_ENGAUGE(buttonCommas != 0);
  ASSERT_ENGAUGE(buttonSpaces != 0);
  ASSERT_ENGAUGE(buttonTabs != 0);
  switch (xport->delimiters)
  {
    case Commas:
      buttonCommas->setChecked(true);
      break;
    case Spaces:
      buttonSpaces->setChecked(true);
      break;
    case Tabs:
      buttonTabs->setChecked(true);
      break;
  }

  ASSERT_ENGAUGE(buttonNone != 0);
  ASSERT_ENGAUGE(buttonSimple != 0);
  ASSERT_ENGAUGE(buttonGnuplot != 0);
  switch (xport->header)
  {
    case HeaderNone:
      buttonNone->setChecked(true);
      break;
    case HeaderSimple:
      buttonSimple->setChecked(true);
      break;
    case HeaderGnuplot:
      buttonGnuplot->setChecked(true);
      break;
  }
}

void ExportDlg::updatePreview()
{
  ASSERT_ENGAUGE(listPreview != 0);
  listPreview->clear();

  // export into a string rather than a file
  QString str;

  Q3TextStream stream(&str, QIODevice::WriteOnly);

  ASSERT_ENGAUGE(pointSets != 0);
  pointSets->exportToStream(stream, *coord, *grid, *xport);

  // split the string into a string list for the preview listbox
  QStringList list = QStringList::split('\n', str, true);
  listPreview->insertStringList(list);
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void ExportDlg::slotIncluded(Q3ListBoxItem*)
{
  toggleActions();
}

void ExportDlg::slotExcluded(Q3ListBoxItem*)
{
  toggleActions();
}

void ExportDlg::slotInclude()
{
  ASSERT_ENGAUGE(listExcluded != 0);
  QString curve = listExcluded->currentText();
  ASSERT_ENGAUGE(pointSets != 0);
  pointSets->exportInclude(curve);

  resetLists();

  ASSERT_ENGAUGE(listIncluded != 0);
  listIncluded->setCurrentItem(listIncluded->findItem(curve));

  toggleActions();
  updatePreview();
}

void ExportDlg::slotExclude()
{
  ASSERT_ENGAUGE(listIncluded != 0);
  QString curve = listIncluded->currentText();
  ASSERT_ENGAUGE(pointSets != 0);
  pointSets->exportExclude(curve);

  resetLists();

  ASSERT_ENGAUGE(listExcluded != 0);
  listExcluded->setCurrentItem(listExcluded->findItem(curve));

  toggleActions();
  updatePreview();
}

void ExportDlg::slotXAllCurves(bool toggle)
{
  ASSERT_ENGAUGE(xport != 0);
  if (toggle && (xport->pointsSelection != XFromAllCurves))
  {
    xport->pointsSelection = XFromAllCurves;
    toggleActions();
    updatePreview();
  }
}

void ExportDlg::slotXFirstCurve(bool toggle)
{
  ASSERT_ENGAUGE(xport != 0);
  if (toggle && (xport->pointsSelection != XFromFirstCurve))
  {
    xport->pointsSelection = XFromFirstCurve;
    toggleActions();
    updatePreview();
  }
}

void ExportDlg::slotXGridLines(bool toggle)
{
  ASSERT_ENGAUGE(xport != 0);
  if (toggle && (xport->pointsSelection != XFromGridLines))
  {
    xport->pointsSelection = XFromGridLines;
    toggleActions();
    updatePreview();
  }
}

void ExportDlg::slotXYAllCurves(bool toggle)
{
  ASSERT_ENGAUGE(xport != 0);
  if (toggle && (xport->pointsSelection != XYFromAllCurves))
  {
    xport->pointsSelection = XYFromAllCurves;
    toggleActions();
    updatePreview();
  }
}

void ExportDlg::slotAllCurvesOnEachLine(bool toggle)
{
  ASSERT_ENGAUGE(xport != 0);
  if (toggle && (xport->layout != AllCurvesOnEachLine))
  {
    xport->layout = AllCurvesOnEachLine;
    toggleActions();
    updatePreview();
  }
}

void ExportDlg::slotOneCurveOnEachLine(bool toggle)
{
  ASSERT_ENGAUGE(xport != 0);
  if (toggle && (xport->layout != OneCurveOnEachLine))
  {
    xport->layout = OneCurveOnEachLine;
    toggleActions();
    updatePreview();
  }
}

void ExportDlg::slotCommas(bool toggle)
{
  ASSERT_ENGAUGE(xport != 0);
  if (toggle && (xport->delimiters != Commas))
  {
    xport->delimiters = Commas;
    toggleActions();
    updatePreview();
  }
}

void ExportDlg::slotSpaces(bool toggle)
{
  ASSERT_ENGAUGE(xport != 0);
  if (toggle && (xport->delimiters != Spaces))
  {
    xport->delimiters = Spaces;
    toggleActions();
    updatePreview();
  }
}

void ExportDlg::slotTabs(bool toggle)
{
  ASSERT_ENGAUGE(xport != 0);
  if (toggle && (xport->delimiters != Tabs))
  {
    xport->delimiters = Tabs;
    toggleActions();
    updatePreview();
  }
}

void ExportDlg::slotNone(bool toggle)
{
  ASSERT_ENGAUGE(xport != 0);
  if (toggle && (xport->header != HeaderNone))
  {
    xport->header = HeaderNone;
    toggleActions();
    updatePreview();
  }
}

void ExportDlg::slotSimple(bool toggle)
{
  ASSERT_ENGAUGE(xport != 0);
  if (toggle && (xport->header != HeaderSimple))
  {
    xport->header = HeaderSimple;
    toggleActions();
    updatePreview();
  }
}

void ExportDlg::slotGnuplot(bool toggle)
{
  ASSERT_ENGAUGE(xport != 0);
  if (toggle && (xport->header != HeaderGnuplot))
  {
    xport->header = HeaderGnuplot;
    toggleActions();
    updatePreview();
  }
}

void ExportDlg::slotWhat()
{
  QWhatsThis::enterWhatsThisMode();
}
