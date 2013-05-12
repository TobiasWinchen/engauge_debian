/***************************************************************************
                          colorchooser.cpp  -  description
                             -------------------
    begin                : Fri Dec 26 2003
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
#include <qpainter.h>
#include <qapplication.h>
//Added by qt3to4:
#include <QLabel>
#include <Q3PointArray>
#include <Q3PtrList>
#include <QPixmap>
#include <QMouseEvent>
#include <QWhatsThis>
#include <QProgressDialog>

#include <math.h>

#include "colorchooser.h"
#include "digitdebug.h"
#include "main.h"
#include "discretize.h"
#include "img/colorchooser_hue.xpm"
#include "img/colorchooser_saturation.xpm"
#include "img/colorchooser_value.xpm"
#include "img/colorchooser_lower.xpm"
#include "img/colorchooser_upper.xpm"
                                                
ColorChooser::ColorChooser(QWidget* parent,
  const QImage* imageOriginal,
  int xTopLeft, int yTopLeft, QRgb rgbBg, bool showLarge) :
  m_parent(parent),
  pixmapIntensity(colorchooser_value_xpm),
  pixmapForeground(colorchooser_value_xpm),
  pixmapHue(colorchooser_hue_xpm),
  pixmapSaturation(colorchooser_saturation_xpm),
  pixmapValue(colorchooser_value_xpm),
  editLow(0),
  editHigh(0),
  m_discretizeMethod(DiscretizeNone),
  m_rgbBg(rgbBg),
  m_showLarge(showLarge)
{
  DigitDebug::ctor(QString("colorchooser ") + QString::number((ulong) this, 16));
  
  ASSERT_ENGAUGE(m_parent != 0);
  
  int x = xTopLeft, y = yTopLeft;
  QColor grayoutColor(232, 232, 232);

  if (showLarge)
  {
    m_chooserHeight = ChooserFullHeight;
    m_scaleHeight = ScaleFullHeight;
  }
  else
  {
    m_chooserHeight = ChooserFullHeight / 2;
    m_scaleHeight = ScaleFullHeight / 2;
  }

  
  chooserCanvas = new Q3Canvas(parent);
  CHECK_PTR_ENGAUGE(chooserCanvas);
  chooserCanvas->resize(ChooserWidth + 2 * ChooserPadding, m_chooserHeight);
  grayoutLower = new Q3CanvasPolygon(chooserCanvas);
  CHECK_PTR_ENGAUGE(grayoutLower);
  grayoutLower->setBrush(QBrush(grayoutColor));
  grayoutLower->setZ(-1);
  grayoutLower->setVisible(true);
  grayoutUpper = new Q3CanvasPolygon(chooserCanvas);
  CHECK_PTR_ENGAUGE(grayoutUpper);
  grayoutUpper->setBrush(QBrush(grayoutColor));
  grayoutUpper->setZ(-1);
  grayoutUpper->setVisible(true);

  chooserCanvasView = new ClickableView(chooserCanvas, m_parent);
  CHECK_PTR_ENGAUGE(chooserCanvasView);
  chooserCanvasView->setGeometry(x, y, ChooserWidth + 2 * ChooserPadding + ChooserFrame,
    m_chooserHeight);
  y += m_chooserHeight + Separation;
  chooserCanvasView->setHScrollBarMode(Q3ScrollView::AlwaysOff);
  chooserCanvasView->setVScrollBarMode(Q3ScrollView::AlwaysOff);
  QWhatsThis::add(chooserCanvasView, QString(tr(
    "Histogram of color attribute values. The lower and upper limits are shown, "
    "and may be dragged using the handles")));

  scaleCanvas = new Q3Canvas(this);
  CHECK_PTR_ENGAUGE(scaleCanvas);
  scaleCanvas->resize(ChooserWidth + 2 * ChooserPadding, m_scaleHeight);

  scaleCanvasView = new Q3CanvasView(scaleCanvas, m_parent);
  CHECK_PTR_ENGAUGE(scaleCanvasView);
  scaleCanvasView->setGeometry(x, y, ChooserWidth + 2 * ChooserPadding + ChooserFrame, m_scaleHeight);
  y += m_scaleHeight + 5;
  scaleCanvasView->setHScrollBarMode(Q3ScrollView::AlwaysOff);
  scaleCanvasView->setVScrollBarMode(Q3ScrollView::AlwaysOff);
  QWhatsThis::add(scaleCanvasView, QString(tr("Scale for histogram")));

  // histogram widgets
  dividerLower = new Divider(this, chooserCanvas, true);
  CHECK_PTR_ENGAUGE(dividerLower);
  dividerUpper = new Divider(this, chooserCanvas, false);
  CHECK_PTR_ENGAUGE(dividerUpper);

  if (m_showLarge)
  {
    // labels over min and max value columns
    labelLower = new QLabel(QString(tr("Lower limit:")), m_parent);
    CHECK_PTR_ENGAUGE(labelLower);
    labelLower->setGeometry(x, y, EditWidth, 30);

    labelUpper = new QLabel(QString(tr("Upper limit:")), m_parent);
    CHECK_PTR_ENGAUGE(labelUpper);
    labelUpper->setGeometry(x + ChooserWidth + 2 * ChooserPadding + ChooserFrame - EditWidth, y, EditWidth, 30);
    y += 25;

    // edit controls for keyboard editing of limits
    editLow = new QLineEdit(m_parent);
    CHECK_PTR_ENGAUGE(editLow);
    editLow->setGeometry(x, y, EditWidth, 30);
    QWhatsThis::add(editLow, QString(tr("Lower limit.\n\nIf the lower limit is less than the upper limit, "
      "only values between the limits are considered on. If the lower limit is greater than the upper limit, "
      "only values outside the limits are considered on")));
    validatorLow = new QIntValidator(editLow);
    CHECK_PTR_ENGAUGE(validatorLow);
    editLow->setValidator(validatorLow);
    connect(editLow, SIGNAL(textChanged(const QString &)), this, SLOT(slotLow(const QString &)));

    editHigh = new QLineEdit(m_parent);
    CHECK_PTR_ENGAUGE(editHigh);
    editHigh->setGeometry(x + ChooserWidth + 2 * ChooserPadding + ChooserFrame - EditWidth, y, EditWidth, 30);
    QWhatsThis::add(editHigh, QString(tr("Upper limit.\n\nIf the lower limit is less than the upper limit, "
      "only values between the limits are considered on. If the lower limit is greater than the upper limit, "
      "only values outside the limits are considered on")));
    validatorHigh = new QIntValidator(editHigh);
    CHECK_PTR_ENGAUGE(validatorHigh);
    editHigh->setValidator(validatorHigh);
    connect(editHigh, SIGNAL(textChanged(const QString &)), this, SLOT(slotHigh(const QString &)));
  }

  loadHistograms(imageOriginal);
}

ColorChooser::~ColorChooser()
{
  DigitDebug::dtor(QString("colorchooser ") + QString::number((ulong) this, 16));
}

void ColorChooser::changeBackgroundColor(const QImage* imageOriginal, QRgb rgbBg)
{
  if (rgbBg != m_rgbBg)
  {
    m_rgbBg = rgbBg;
    loadForegroundPixmap(m_rgbBg);
    
    loadHistograms(imageOriginal);
  }
}

void ColorChooser::changeTextLimit(bool lower, int limit)
{
  if (m_showLarge)
  {
    if (lower)
    {
      ASSERT_ENGAUGE(editLow != 0);
      if (editLow->text().isEmpty() || (limit != editLow->text().toInt()))
       editLow->setText(QString("%1").arg(limit));
    }
    else
    {
      ASSERT_ENGAUGE(editHigh != 0);
      if (editHigh->text().isEmpty() || (limit != editHigh->text().toInt()))
        editHigh->setText(QString("%1").arg(limit));
    }
  }
  else
  {
    // skip changing the edit boxes, since they do not exist. create the signal
    // the edit box change would have made
    if (lower)
      slotLow(QString("%1").arg(limit));
    else
      slotHigh(QString("%1").arg(limit));
  }
}

int ColorChooser::colorAttributeMax(void)
{
  Discretize discretize;
  return discretize.colorAttributeMax(m_discretizeMethod);
}

void ColorChooser::loadForegroundPixmap(QRgb rgbBg)
{
  // build foreground/background pixmap. for the foreground color we use black or white,
  // whichever is furthest in rgb space from the background color
  int width = ChooserWidth + 2 * ChooserPadding;
  QImage imageScale(width, m_scaleHeight, 32);

  int rBg, gBg, bBg;
  QColor colorBg(rgbBg);
  colorBg.rgb(&rBg, &gBg, &bBg);
  
  QRgb rgbFg;
  int distanceBlack = (rBg - 0) * (rBg - 0) + (gBg - 0) * (gBg - 0) + (bBg - 0) * (bBg - 0);
  int distanceWhite = (rBg - 255) * (rBg - 255) + (gBg - 255) * (gBg - 255) + (bBg - 255) * (bBg - 255);
  if (distanceWhite > distanceBlack)
    rgbFg = QColor(Qt::white).rgb();
  else
    rgbFg = QColor(Qt::black).rgb();

  for (int x = 0; x < width; x++)
    for (int y = 0; y < m_scaleHeight; y++)
    {
      // show an triangle with bottom side on the left, and point on the right
      if (x < (y * width) / (m_scaleHeight - 2 * ChooserFrame))
        setPixelRGB(&imageScale, x, y, rgbBg);
      else
        setPixelRGB(&imageScale, x, y, rgbFg);
    }

  pixmapForeground.convertFromImage(imageScale, Qt::ThresholdDither);

  if (m_discretizeMethod == DiscretizeForeground)
  {
    scaleCanvas->setBackgroundPixmap(pixmapForeground);
    scaleCanvas->update();
  }
}

void ColorChooser::loadHistogram(const QImage* imageOriginal,
  DiscretizeMethod method, int colorAttributeMax, Q3PtrList<Q3CanvasLine>* histogram,
  QRgb rgbBg, QProgressDialog* dlg)
{
  ASSERT_ENGAUGE(
    (method == DiscretizeIntensity) ||
    (method == DiscretizeForeground) ||
    (method == DiscretizeHue) ||
    (method == DiscretizeSaturation) ||
    (method == DiscretizeValue));

  // remove any stale points
  ASSERT_ENGAUGE(histogram != 0);
  histogram->clear();
    
  int *bins = new int [colorAttributeMax + 1];
  CHECK_PTR_ENGAUGE(bins);

  int i;
  for (i = 0; i <= colorAttributeMax; i++)
    bins [i] = 0;

  Discretize discretize;
  int x, y, value, rBg, gBg, bBg;
  QColor color(rgbBg);
  color.rgb(&rBg, &gBg, &bBg);
  for (x = 0; x < imageOriginal->width(); x++)
  {
    for (y = 0; y < imageOriginal->height(); y++)
    {
      if (method == DiscretizeForeground)
        value = discretize.discretizeValueForeground(imageOriginal, x, y, method, rBg, gBg, bBg);
      else
        value = discretize.discretizeValueNotForeground(imageOriginal, x, y, method);

      bins [value] += 1;
    }

    // update progress bar
    ASSERT_ENGAUGE(dlg != 0);
    dlg->setValue(imageOriginal->width() * method + x);
  }
  
  // represent histograms as lines on canvas
  int xOld = ChooserPadding, yOld = m_chooserHeight - 1, xNew, yNew;
  double pixelCount = (double) (imageOriginal->width() * imageOriginal->height());
  for (i = 0; i <= colorAttributeMax + 1; i++)
  {
    if (i < colorAttributeMax + 1)
    {
      // convert bin count to log scale so small bin counts are still visible
      xNew = ChooserPadding + (int) (((ChooserWidth - 1) * i) / (double) (colorAttributeMax) + 0.5);
      int binCount = bins [i];
      if (binCount < 1)
        binCount = 1;
      yNew = (int) ((m_chooserHeight - 1.0) * (1.0 - log(binCount) / log(pixelCount)) + 0.5);

      if (yNew < 0)
        yNew = 0;
      if (yNew > m_chooserHeight - 1)
        yNew = m_chooserHeight - 1;
    }
    else
    {
      xNew = ChooserPadding + ChooserWidth;
      yNew = m_chooserHeight - 1;
    }

    Q3CanvasLine* line = new Q3CanvasLine(chooserCanvas);
    CHECK_PTR_ENGAUGE(line);

    line->setPoints(xOld, yOld, xNew, yNew);

    histogram->append(line);

    xOld = xNew;
    yOld = yNew;
  }

  ASSERT_ENGAUGE(bins != 0);
  delete[] bins;
}

void ColorChooser::loadHistograms(const QImage* imageOriginal)
{
  qApp->processEvents(); // process events just once to fill gray box left over from dropped down menu

  int discretizeMethods = 5;
  int runTime = imageOriginal->width() * discretizeMethods;
  QProgressDialog* dlg = new QProgressDialog(QString(tr("Loading histograms")), "Cancel", 0,
    runTime, m_parent);
  CHECK_PTR_ENGAUGE(dlg);
  dlg->setCaption(tr("Progress"));
  dlg->show();

  loadForegroundPixmap(m_rgbBg);

  intensityHistogram.setAutoDelete(true);
  foregroundHistogram.setAutoDelete(true);
  hueHistogram.setAutoDelete(true);
  saturationHistogram.setAutoDelete(true);
  valueHistogram.setAutoDelete(true);

  loadHistogram(imageOriginal, DiscretizeIntensity, DiscretizeIntensityMax, &intensityHistogram, m_rgbBg, dlg);
  loadHistogram(imageOriginal, DiscretizeForeground, DiscretizeForegroundMax, &foregroundHistogram, m_rgbBg, dlg);
  loadHistogram(imageOriginal, DiscretizeHue, DiscretizeHueMax, &hueHistogram, m_rgbBg, dlg);
  loadHistogram(imageOriginal, DiscretizeSaturation, DiscretizeSaturationMax, &saturationHistogram, m_rgbBg, dlg);
  loadHistogram(imageOriginal, DiscretizeValue, DiscretizeValueMax, &valueHistogram, m_rgbBg, dlg);

  showCurrentHistogram();
  
  chooserCanvas->update();

  ASSERT_ENGAUGE(dlg != 0);
  delete dlg;

  qApp->processEvents(); // process events just once to fill gray box left over from previous window
}

void ColorChooser::passLimitToParent(bool lower, int limit)
{
  emit(limitChange(lower, limit));
}

bool ColorChooser::refreshHistogram(Q3PtrList<Q3CanvasLine>* histogram, DiscretizeMethod method)
{
  bool changed = false;
  Q3CanvasLine* line;
  ASSERT_ENGAUGE(histogram != 0);
  for (line = histogram->first(); line != 0; line = histogram->next())
    if (line->visible() != (m_discretizeMethod == method))
    {
      changed = true;

      line->setVisible(m_discretizeMethod == method);

      chooserCanvas->setChanged(line->boundingRect());
    }

  return changed;
}

void ColorChooser::setDiscretizeMethod(DiscretizeMethod method, int limitLower, int limitUpper)
{
  m_discretizeMethod = method;

  if (m_discretizeMethod != DiscretizeNone)
  {
    ASSERT_ENGAUGE(dividerLower);
    dividerLower->setLimit(limitLower);
    ASSERT_ENGAUGE(dividerUpper);
    dividerUpper->setLimit(limitUpper);
  }
  
  bool showDividers = true;
  switch (method)
  {
  case DiscretizeNone:
    {
      // the histogram and scale borrow the palette of the nearby edit boxes, so
      // the enabled and disabled colors are consistent, and the user sees consistent
      // visual clues about state
      QPixmap disabledBackground(ChooserWidth + 2 * ChooserPadding, 1);
      if (editLow == 0)
        disabledBackground.fill(Qt::lightGray);
      else
        disabledBackground.fill(editLow->palette().disabled().background());
      scaleCanvas->setBackgroundPixmap(disabledBackground);

      showDividers = false;
    }
    break;
  case DiscretizeIntensity:
    scaleCanvas->setBackgroundPixmap(pixmapIntensity);
    break;
  case DiscretizeForeground:
    scaleCanvas->setBackgroundPixmap(pixmapForeground);
    break;
  case DiscretizeHue:
    scaleCanvas->setBackgroundPixmap(pixmapHue);
    break;
  case DiscretizeSaturation:
    scaleCanvas->setBackgroundPixmap(pixmapSaturation);
    break;
  case DiscretizeValue:
    scaleCanvas->setBackgroundPixmap(pixmapValue);
    break;
  }

  grayoutLower->setVisible(showDividers);
  grayoutUpper->setVisible(showDividers);
  dividerLower->setVisible(showDividers);
  dividerUpper->setVisible(showDividers);
      
  scaleCanvas->update();

  if (m_showLarge)
  {
    validatorLow->setRange(0, colorAttributeMax());
    validatorHigh->setRange(0, colorAttributeMax());

    changeTextLimit(true, limitLower);
    changeTextLimit(false, limitUpper);
  }
  
  showCurrentHistogram();
  updateGrayouts();

  chooserCanvas->update();
}

void ColorChooser::showCurrentHistogram()
{
  dividerLower->setVisible(m_discretizeMethod != DiscretizeNone);
  dividerUpper->setVisible(m_discretizeMethod != DiscretizeNone);

  refreshHistogram(&intensityHistogram, DiscretizeIntensity);
  refreshHistogram(&foregroundHistogram, DiscretizeForeground);
  refreshHistogram(&hueHistogram, DiscretizeHue);
  refreshHistogram(&saturationHistogram, DiscretizeSaturation);
  refreshHistogram(&valueHistogram, DiscretizeValue);
}

void ColorChooser::updateGrayouts()
{
  if (m_discretizeMethod != DiscretizeNone)
  {
    // update grayout areas
    int lowerLeft = 0;
    int upperRight = ChooserWidth;
    int lower = dividerLower->screenPosition();
    int upper = dividerUpper->screenPosition();
    if (lower > upper)
    {
      lowerLeft = upper;
      upperRight = lower;
    }

    Q3PointArray boundsLowerNew(QRect(lowerLeft,
      0,
      lower - lowerLeft,
      m_chooserHeight - 1)); // left, top, width, height
    if (boundsLowerNew != grayoutLower->areaPoints())
    {
      chooserCanvas->setChanged(grayoutLower->boundingRect());

      grayoutLower->setPoints(boundsLowerNew);

      chooserCanvas->setChanged(grayoutLower->boundingRect());
    }

    Q3PointArray boundsUpperNew(QRect(upper,
      0,
      upperRight - upper + 2 * ChooserPadding,
      m_chooserHeight - 1)); // left, top, width, height
    if (boundsUpperNew != grayoutUpper->areaPoints())
    {
      chooserCanvas->setChanged(grayoutUpper->boundingRect());
    
      grayoutUpper->setPoints(boundsUpperNew);

      chooserCanvas->setChanged(grayoutUpper->boundingRect());
    }
  }
}

ColorChooser::ClickableView::ClickableView(Q3Canvas* canvas,
  QWidget* parent) :
  Q3CanvasView(canvas, parent),
  dragging(false)
{
}

void ColorChooser::ClickableView::contentsMouseMoveEvent(QMouseEvent* e)
{
  if (dragging)
  {
    // drag to new position
    ASSERT_ENGAUGE(draggedTab != 0);
    draggedTab->divider()->dragContinue(e->pos().x());
  }
}

void ColorChooser::ClickableView::contentsMousePressEvent(QMouseEvent* e)
{
  Q3CanvasItemList list = canvas()->collisions(e->pos());
  if (!list.isEmpty())
  {
    // sift through list looking for a Tab
   Q3CanvasItemList::iterator itr;
    for (itr = list.begin(); itr != list.end(); ++itr)
    {
      Q3CanvasItem* item = *itr;
      if (item->rtti() == Divider::Tab::RttiTab)
      {
        dragging = true;
        draggedTab = (Divider::Tab*) item;
        ASSERT_ENGAUGE(e != 0);
        draggedTab->divider()->dragStart(e->pos().x());
        break;
      }
    }
  }
}

void ColorChooser::ClickableView::contentsMouseReleaseEvent(QMouseEvent*)
{
  dragging = false;
  draggedTab = 0;
}

ColorChooser::Divider::Divider(ColorChooser* chooser, Q3Canvas* canvas, bool lower) :
  m_chooser(chooser),
  m_canvas(canvas),
  m_lower(lower),
  m_limit(0),
  m_xScreenOffset(0)
{
  m_line = new Q3CanvasLine(canvas);
  CHECK_PTR_ENGAUGE(m_line);
  m_line->setPen(QPen(QColor(0, 0, 255)));
  m_line->show();

  m_tab = new Tab(chooser, this, canvas, lower);
  CHECK_PTR_ENGAUGE(m_tab);
}

void ColorChooser::Divider::dragContinue(int xCursor)
{
  // clip to histogram extent, which is less than screen extent because of padding
  double xScreen = xCursor - m_xScreenOffset;
  if (xScreen < ChooserPadding)
    xScreen = ChooserPadding;
  if (ChooserWidth + ChooserPadding - 1 < xScreen)
    xScreen = ChooserWidth + ChooserPadding - 1;

  // move divider, including its tab
  moveToScreenPosition(xScreen);

  // show new position
  ASSERT_ENGAUGE(m_canvas);
  m_canvas->update();
}

void ColorChooser::Divider::dragStart(int xCursor)
{
  m_xScreenOffset = xCursor - m_xScreen;
}

void ColorChooser::Divider::moveToScreenPosition(double xScreen)
{
  m_xScreen = xScreen;

  ASSERT_ENGAUGE(m_chooser != 0);
  m_line->setPoints((int) (xScreen + 0.5), 0, (int) (xScreen + 0.5), m_chooser->chooserHeight() - 1);

  ASSERT_ENGAUGE(m_tab != 0);
  m_tab->moveToScreenPosition(xScreen);

  ASSERT_ENGAUGE(m_chooser != 0);
  int limit = (int) ((xScreen - ChooserPadding) * m_chooser->colorAttributeMax() /
    (ChooserWidth - 1) + 0.5);

  if (limit != m_limit)
  {
    m_limit = limit;

    ASSERT_ENGAUGE(m_chooser != 0);
    m_chooser->changeTextLimit(m_lower, m_limit);
  }
}

int ColorChooser::Divider::screenPosition(void)
{
  ASSERT_ENGAUGE(m_line != 0);
  return m_line->startPoint().x();
}
         
void ColorChooser::Divider::setLimit(int limit)
{
  ASSERT_ENGAUGE(m_chooser != 0);
  m_xScreen = ChooserPadding +
    (int) ((limit * (ChooserWidth - 1)) / (double) m_chooser->colorAttributeMax() + 0.5);

  ASSERT_ENGAUGE(m_line != 0);
  QRect oldBounds = m_line->boundingRect();
  ASSERT_ENGAUGE(m_chooser != 0);
  m_line->setPoints((int) (m_xScreen + 0.5), 0, (int) (m_xScreen + 0.5), m_chooser->chooserHeight() - 1);
  QRect newBounds = m_line->boundingRect();

  if (oldBounds != newBounds)
  {
    ASSERT_ENGAUGE(m_canvas != 0);
    m_canvas->setChanged(oldBounds);
    m_canvas->setChanged(newBounds);
  }

  ASSERT_ENGAUGE(m_tab != 0);
  m_tab->moveToScreenPosition(m_xScreen);
  
  m_limit = limit;

  ASSERT_ENGAUGE(m_chooser != 0);
  m_chooser->updateGrayouts();
  
  m_canvas->update();
}

void ColorChooser::Divider::setVisible(bool show)
{
  ASSERT_ENGAUGE(m_line != 0);
  m_line->setVisible(show);

  ASSERT_ENGAUGE(m_tab != 0);
  m_tab->setVisible(show);
}

ColorChooser::Divider::Tab::Tab(ColorChooser* chooser, Divider* divider, Q3Canvas* canvas, bool lower) :
  Q3CanvasRectangle(0, 0, TabWidth, TabHeight, canvas),
  m_chooser(chooser),
  m_divider(divider),
  m_canvas(canvas),
  m_lower(lower)
{
  if (lower)
    m_pixmap = new QPixmap(histogram_lower_xpm);
  else
    m_pixmap = new QPixmap(histogram_upper_xpm);
  CHECK_PTR_ENGAUGE(m_pixmap);

  Q3CanvasRectangle::show();
  Q3CanvasRectangle::setZ(10.0); // put above everything else

  if (m_lower)
    // lower limit is at a lower height than upper limit
    Q3CanvasRectangle::setY((double) (3 * chooser->chooserHeight() / 4 - TabHeight / 2));
  else
    // upper limit is at a higher height than lower limit
    Q3CanvasRectangle::setY((double) (chooser->chooserHeight() / 4 - TabHeight / 2));
}

void ColorChooser::Divider::Tab::drawShape(QPainter &p)
{
  if (Q3CanvasRectangle::visible())
    p.drawPixmap((int) x(), (int) y(), *m_pixmap);
}

void ColorChooser::Divider::Tab::moveToScreenPosition(double xScreen)
{
  QRect oldBounds = Q3CanvasRectangle::boundingRect();
  Q3CanvasRectangle::setX(xScreen - TabWidth / 2);
  QRect newBounds = Q3CanvasRectangle::boundingRect();

  if (oldBounds != newBounds)
  {
    ASSERT_ENGAUGE(m_canvas != 0);
    m_canvas->setChanged(oldBounds);
    m_canvas->setChanged(newBounds);
  }
}

void ColorChooser::Divider::Tab::setVisible(bool show)
{
  Q3CanvasRectangle::setVisible(show);
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void ColorChooser::slotHigh(const QString &text)
{
  int limit = text.toInt();

  // keep divider on screen. lower bound is handled by validator
  if (limit > colorAttributeMax())
  {
    limit = colorAttributeMax();

    if (m_showLarge)
    {
      ASSERT_ENGAUGE(editHigh != 0);
      editHigh->setText(QString("%1").arg(limit));
    }
  }
  
  ASSERT_ENGAUGE(dividerUpper);
  dividerUpper->setLimit(limit);
  
  // send limit if valid, else invalid value if blank so user is forced to enter 0 for zero
  if (text.isEmpty())  
    passLimitToParent(false, colorAttributeMax() + 1);
  else
    passLimitToParent(false, limit);
}

void ColorChooser::slotLow(const QString &text)
{ 
  int limit = text.toInt();

  // keep divider on screen. lower bound is handled by validator
  if (limit > colorAttributeMax())
  {
    limit = colorAttributeMax();

    if (m_showLarge)
    {
      ASSERT_ENGAUGE(editLow != 0);
      editLow->setText(QString("%1").arg(limit));
    }
  }
  
  ASSERT_ENGAUGE(dividerLower);
  dividerLower->setLimit(limit);
  
  // send limit if valid, else invalid value if blank so user is forced to enter 0 for zero
  if (text.isEmpty())
    passLimitToParent(true, colorAttributeMax() + 1);
  else
    passLimitToParent(true, limit);
}
