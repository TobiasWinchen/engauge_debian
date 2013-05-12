/***************************************************************************
                          pointsetsdlg.cpp  -  description
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

#include <qmessagebox.h>
#include <QWhatsThis>
#include <qtooltip.h>
#include <qregexp.h>
//Added by qt3to4:
#include <QList>
#include <QPixmap>

#include "pointsetdlg.h"
#include "pointsetsdlg.h"
#include "pointsetnamedlg.h"
#include "pointsets.h"
#include "img/whatsthis.xpm"
#include "digitdebug.h"
#include "main.h"

PointSetsDlg::PointSetsDlg(bool curve, QString title, PointSets* pointSets, QStringList* pointSetsWithOrphanedPoints) :
  QDialog(0, "pointsetsdlg", true),
  curve(curve),
  title(title),
  pointSets(pointSets),
  pointSetsWithOrphanedPoints(pointSetsWithOrphanedPoints),
  activePointSet(NoActivePointSet)
{
  DigitDebug::ctor(QString("pointsetsdlg ") + QString::number((ulong) this, 16));
  
  setCaption(curveReplace("Curves - [%1]%2")
    .arg(title)
    .arg(freewareCaption));

  setFixedSize(380, 380);

  buttonNew = new QPushButton(tr("New..."), this);
  CHECK_PTR_ENGAUGE(buttonNew);
  buttonNew->setGeometry(10, 10, 100, 30);
  QWhatsThis::add(buttonNew, curveReplace("Adds a new curve to the curve list.\n\n"
    "Every curve name must be unique"));
  connect(buttonNew, SIGNAL(clicked()), this, SLOT(slotNew()));

  buttonRemove = new QPushButton(tr("Remove"), this);
  CHECK_PTR_ENGAUGE(buttonRemove);
  buttonRemove->setGeometry(140, 10, 100, 30);
  QWhatsThis::add(buttonRemove, curveReplace("Removes the currently selected curve from "
     "the curve list.\n\nThere must always be at least one curve"));
  connect(buttonRemove, SIGNAL(clicked()), this, SLOT(slotRemove()));

  pointSetNames = new Q3ListBox(this);
  CHECK_PTR_ENGAUGE(pointSetNames);
  pointSetNames->setGeometry(10, 50, 230, 280);
  QWhatsThis::add(pointSetNames, curveReplace("List of the curves belonging to this document"));
  connect(pointSetNames, SIGNAL(selectionChanged(Q3ListBoxItem*)), this, SLOT(slotSelectionChanged(Q3ListBoxItem*)));

  buttonUp = new QPushButton(tr("Move Up"), this);
  CHECK_PTR_ENGAUGE(buttonUp);
  buttonUp->setGeometry(270, 50, 100, 30);
  QWhatsThis::add(buttonUp, curveReplace("Move the currently selected curve up in the "
    "curve list.\n\nExported curves will be in the same order as displayed here.\n\n"
    "Basically, the order of the curves is cosmetic"));
  connect(buttonUp, SIGNAL(clicked()), this, SLOT(slotUp()));

  buttonDown = new QPushButton(tr("Move Down"), this);
  CHECK_PTR_ENGAUGE(buttonDown);
  buttonDown->setGeometry(270, 90, 100, 30);
  QWhatsThis::add(buttonDown, curveReplace("Move the currently selected curve down in "
    "the curve list.\n\nExported curves will be in the same order as displayed here.\n\n"
    "Basically, the order of the curves is cosmetic"));
  connect(buttonDown, SIGNAL(clicked()), this, SLOT(slotDown()));

  buttonRename = new QPushButton(tr("Rename..."), this);
  CHECK_PTR_ENGAUGE(buttonRename);
  buttonRename->setGeometry(270, 260, 100, 30);
  QWhatsThis::add(buttonRename, curveReplace("Rename the currently selected curve"));
  connect(buttonRename, SIGNAL(clicked()), this, SLOT(slotRename()));

  buttonProperties = new QPushButton(tr("Properties..."), this);
  CHECK_PTR_ENGAUGE(buttonProperties);
  buttonProperties->setGeometry(270, 300, 100, 30);
  QWhatsThis::add(buttonProperties, curveReplace("Edit the properties of the currently "
    "selected curve"));
  connect(buttonProperties, SIGNAL(clicked()), this, SLOT(slotProperties()));

  // navigation buttons
  buttonOk = new QPushButton(tr("OK"), this);
  CHECK_PTR_ENGAUGE(buttonOk);
  buttonOk->setGeometry(10, 340, 100, 30);
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(accept()));

  buttonCancel = new QPushButton(tr("Cancel"), this);
  CHECK_PTR_ENGAUGE(buttonCancel);
  buttonCancel->setGeometry(140, 340, 100, 30);
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));

  buttonWhat = new QPushButton(this);
  CHECK_PTR_ENGAUGE(buttonWhat);
  buttonWhat->setPixmap(QPixmap(whatsthis_xpm));
  buttonWhat->setGeometry(310, 340, 60, 30);
  QToolTip::add(buttonWhat, QString(tr("What's this?")));
  connect(buttonWhat, SIGNAL(clicked()), this, SLOT(slotWhat()));
  
  refreshPointSetList();
  QStringList list;
  if (curve)
    pointSets->curveNames(&list);
  else
    pointSets->measureNames(&list);
  selectPointSetName(list.first());
  toggleActions();

  pointSetsWithOrphanedPoints->clear();
}

PointSetsDlg::~PointSetsDlg()
{
  DigitDebug::dtor(QString("pointsetsdlg ") + QString::number((ulong) this, 16));
  
  delete buttonOk;
  delete buttonCancel;
}

void PointSetsDlg::refreshPointSetList()
{
  // after refreshing, one entry must be selected so try to make it the same as before
  ASSERT_ENGAUGE(pointSetNames != 0);
  QString selected = pointSetNames->currentText();

  pointSetNames->clear();

  QStringList list;
  ASSERT_ENGAUGE(pointSets != 0);
  if (curve)
    pointSets->curveNames(&list);
  else
    pointSets->measureNames(&list);

  QStringList::iterator itr;
  for (itr = list.begin(); itr != list.end(); ++itr)
  {
    // create a pixmap showing the style of this pointset
    const int width = 50, height = 15;

    QPixmap pix(width, height);
    pix.fill(Qt::red);

    Q3Canvas* canvas = new Q3Canvas(this);
    CHECK_PTR_ENGAUGE(canvas);
    canvas->resize(width, height);

    PointSet* pointSet = new PointSet;
    CHECK_PTR_ENGAUGE(pointSet);
    if (curve)
      pointSet->setStyle(pointSets->curveStyle(*itr));
    else
      pointSet->setStyle(pointSets->measureStyle(*itr));

    Point* previewLeftPoint = new Point(width / 5, height / 2, canvas);
    CHECK_PTR_ENGAUGE(previewLeftPoint);
    Point* previewRightPoint = new Point(width * 4 / 5, height / 2, canvas);
    CHECK_PTR_ENGAUGE(previewRightPoint);
    QList<QRect> updateRectList;
    pointSet->addPoint(canvas, previewLeftPoint, &updateRectList);
    pointSet->addPoint(canvas, previewRightPoint, &updateRectList);
    previewLeftPoint->setPointSet(pointSet);
    previewRightPoint->setPointSet(pointSet);

    QPainter p(&pix);
    canvas->drawArea(canvas->rect(), &p);

    delete previewLeftPoint;
    delete previewRightPoint;
    delete pointSet;
    delete canvas;

    // save pixmap and pointset name
    pointSetNames->insertItem(pix, (*itr));
  }

  Q3ListBoxItem* item = pointSetNames->findItem(selected);
  if (item)
    pointSetNames->setCurrentItem(item);
  else
    pointSetNames->setCurrentItem(0);

  ASSERT_ENGAUGE(pointSetNames->count() > 0);
}

void PointSetsDlg::selectPointSetName(QString name)
{
  ASSERT_ENGAUGE(pointSetNames != 0);
  for (uint index = 0; index < pointSetNames->count(); index++)
    if (name == pointSetNames->text(index))
    {
      pointSetNames->setSelected(index, true);
      return;
    }

  pointSetNames->setSelected(0, true);
}

void PointSetsDlg::toggleActions()
{
  // disallow having fewer than one pointset
  ASSERT_ENGAUGE(buttonRemove != 0);
  buttonRemove->setEnabled(pointSetNames->count() > 1);

  // cannot move highest pointset up, or lowest pointset down
  ASSERT_ENGAUGE(buttonUp != 0);
  buttonUp->setEnabled(pointSetNames->currentItem() > 0);
  ASSERT_ENGAUGE(buttonDown != 0);
  buttonDown->setEnabled(pointSetNames->currentItem() < (int) (pointSetNames->count() - 1));
}

QString PointSetsDlg::curveReplace(const char* text)
{
  if (curve)
    return QString(tr(text));
  else
  {
    QString rtn(text);
    rtn.replace(QRegExp("curve"), QString("measure"));
    rtn.replace(QRegExp("Curve"), QString("Measure"));
    return rtn;
  }
}

QString PointSetsDlg::newNameFromPreviousPointSet(QString previousName)
{
  // work backward to find last nonnumeric character
  int lastNonnumeric = previousName.length() - 1; // this may go as low as -1!
  int digitsInNumber = 0;
  while ((lastNonnumeric >= 0) && previousName.at(lastNonnumeric).isDigit())
  {
    --lastNonnumeric;
    ++digitsInNumber;
  }

  // parse numeric suffix value
  unsigned int suffix = 0;
  if (digitsInNumber > 0)
    suffix = previousName.mid(lastNonnumeric + 1).toUInt();

  // combine old prefix and new suffix
  QString prefix;
  if (lastNonnumeric >= 0)
    prefix = previousName.left(lastNonnumeric + 1);

  return prefix + QString::number(suffix + 1);
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void PointSetsDlg::slotDown()
{
  ASSERT_ENGAUGE(pointSets != 0);
  if (curve)
    pointSets->moveCurveDown(activePointSet);
  else
    pointSets->moveMeasureDown(activePointSet);

  refreshPointSetList();
  selectPointSetName(activePointSet);
}

void PointSetsDlg::slotNew()
{
  QString name;
  if (curve)
    name = DefaultCurveName;
  else
    name = DefaultMeasureName;

  QStringList disallowedNames;
  ASSERT_ENGAUGE(pointSetNames != 0);
  for (uint index = 0; index < pointSetNames->count(); index++)
    if (pointSetNames->text(index) != AxesPointSetName)
    {
      disallowedNames.append(pointSetNames->text(index));

      // this pointset is the most recent so far
      name = newNameFromPreviousPointSet(pointSetNames->text(index));
    }

  PointSetNameDlg* dlg = new PointSetNameDlg(curve, title, &name, disallowedNames);
  CHECK_PTR_ENGAUGE(dlg);

  if (dlg->exec() == QDialog::Accepted)
  {
    ASSERT_ENGAUGE(pointSets != 0);
    if (curve)
      pointSets->addCurve(name);
    else
      pointSets->addMeasure(name);

    refreshPointSetList();
    selectPointSetName(name);

    toggleActions();
  }
}

void PointSetsDlg::slotProperties()
{
  PointSetStyle pointSetStyle;
  ASSERT_ENGAUGE(pointSets != 0);
  QString curveName = pointSetNames->currentText();
  if (curve)
    pointSetStyle = pointSets->curveStyle(curveName);
  else
    pointSetStyle = pointSets->measureStyle(curveName);

  PointSetDlg* dlg = new PointSetDlg(title, curveName,
    (curve ? CurveState : MeasureState), &pointSetStyle);
  CHECK_PTR_ENGAUGE(dlg);

  if (dlg->exec() == QDialog::Accepted)
  {
    if (curve)
      pointSets->setCurveStyle(pointSetNames->currentText(), pointSetStyle);
    else
      pointSets->setMeasureStyle(pointSetNames->currentText(), pointSetStyle);

    refreshPointSetList();
  }
}

void PointSetsDlg::slotRemove()
{
  // prevent the dangerous action of deleting a pointset that has points
  int pointCount;

  ASSERT_ENGAUGE(pointSets != 0);
  if (curve)
    pointCount = pointSets->pointCountCurve(pointSetNames->currentText());
  else
    pointCount = pointSets->pointCountMeasure(pointSetNames->currentText());

  if (pointCount > 0)
  {
    QString msg;

    msg.sprintf(curveReplace("This curve has %d points. Do you still want to delete it and its points?"),
      pointCount);
    if (QMessageBox::warning(this, title, msg, QMessageBox::Ok, QMessageBox::Cancel) ==
      QMessageBox::Cancel)
      return;

    pointSetsWithOrphanedPoints->append(pointSetNames->currentText());
  }

  if (curve)
    pointSets->removeCurve(pointSetNames->currentText());
  else
    pointSets->removeMeasure(pointSetNames->currentText());

  ASSERT_ENGAUGE(pointSetNames != 0);
  int index = pointSetNames->currentItem();
  refreshPointSetList();
  pointSetNames->setCurrentItem(index + 1);
}

void PointSetsDlg::slotRename()
{
  ASSERT_ENGAUGE(pointSetNames != 0);
  QString copyName = pointSetNames->currentText();

  QStringList disallowedNames;
  for (uint index = 0; index < pointSetNames->count(); index++)
    if ((pointSetNames->text(index) != AxesPointSetName) &&
      (pointSetNames->text(index) != copyName))
      disallowedNames.append(pointSetNames->text(index));

  PointSetNameDlg* dlg = new PointSetNameDlg(curve, title, &copyName, disallowedNames);
  CHECK_PTR_ENGAUGE(dlg);

  if (dlg->exec() == QDialog::Accepted)
  {
    ASSERT_ENGAUGE(pointSets != 0);
    pointSets->setName(pointSetNames->currentText(), copyName);

    refreshPointSetList();
    selectPointSetName(copyName);
  }
}

void PointSetsDlg::slotSelectionChanged(Q3ListBoxItem* item)
{
  ASSERT_ENGAUGE(item != 0);
  activePointSet = item->text();
  toggleActions();
}

void PointSetsDlg::slotUp()
{
  ASSERT_ENGAUGE(pointSets != 0);
  if (curve)
    pointSets->moveCurveUp(activePointSet);
  else
    pointSets->moveMeasureUp(activePointSet);
    
  refreshPointSetList();
  selectPointSetName(activePointSet);
}

void PointSetsDlg::slotWhat()
{
  QWhatsThis::enterWhatsThisMode();
}
