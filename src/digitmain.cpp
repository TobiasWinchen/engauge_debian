/***************************************************************************
                          digitmain.cpp  -  description
                             -------------------
    begin                : Tue Oct 29 14:43:44 PST 2002
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

#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#endif

#include <q3dragobject.h>
#include <qclipboard.h>
#include <qmenubar.h>
#include <qtooltip.h>
//Added by qt3to4:
#include <QWhatsThis>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QLabel>
#include <Q3StrList>
#include <QPixmap>
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QKeyEvent>
#include <QEvent>
#include <QDropEvent>
#include <Q3PtrList>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QDesktopWidget>
#include <QFileDialog>
#include "digitview.h"
#include "digitdoc.h"
#include "digitmain.h"
#include "img/fileimport.xpm"
#include "img/fileopen.xpm"
#include "img/filesave.xpm"
#include "img/fileexport.xpm"
#include "img/digitselectlarge.xpm"
#include "img/digitselectsmall.xpm"
#include "img/digitaxislarge.xpm"
#include "img/digitaxissmall.xpm"
#include "img/digitscalelarge.xpm"
#include "img/digitscalesmall.xpm"
#include "img/digitcurvelarge.xpm"
#include "img/digitcurvesmall.xpm"
#include "img/digitsegmentlarge.xpm"
#include "img/digitsegmentsmall.xpm"
#include "img/digitmatchlarge.xpm"
#include "img/digitmatchsmall.xpm"
#include "img/digitmeasurelarge.xpm"
#include "img/digitmeasuresmall.xpm"
#include "img/bannerapp.xpm"
#include "coordsysdlg.h"
#include "pointsetdlg.h"
#include "pointsetsdlg.h"
#include "gridmeshdlg.h"
#include "gridremovaldlg.h"
#include "discretizedlg.h"
#include "curvecmb.h"
#include "measurecmb.h"
#include "zoomcmb.h"
#include "pointsets.h"
#include "exportdlg.h"
#include "segmentdlg.h"
#include "digitdebug.h"
#include "clipboard.h"
#include "geometrydlg.h"
#include "helpwindow.h"
#include "pointmatchdlg.h"
#include "main.h"
#include "defaultsettings.h"
#include "sessionsdlg.h"
#include "osxsubs.h"

/// Helper Functions to emulate Qt3 QAction constructors
QAction* createAction(QObject * parent, const char * name = 0)
{
	QAction *f = new QAction(name, parent);
	return f;
}
QAction* createAction(const QString & menuText, QKeySequence accel, QObject * parent, const char * name = 0 )
{
	QAction *f = createAction(parent, menuText);
	f->setShortcut(accel);
	f->setText(menuText);
	return f;
}
QAction* createAction( const QIconSet & icon, const QString & menuText, const QString & accel, QObject * parent, const char * name = 0 )
{
	QAction *f = createAction(menuText, accel, parent, name);
	f->setIcon(icon);
	return f;
}
QAction* createAction( const QString & text, const QIconSet & icon, const QString &, const QString& accel, QObject * parent, const char * name = 0, bool toggle = FALSE )
{
	QAction *f = createAction(icon, text, accel, parent, name);
	f->setCheckable(toggle);
	return f;
}

QAction* createAction( const QString & text, const QString &, QKeySequence accel, QObject * parent, const char * name = 0, bool toggle = FALSE )
{
	QAction *f = createAction(text, accel, parent, name);
	f->setCheckable(toggle);
	return f;
}


extern QDir cmdManualDirectory;
extern bool cmdOnlyBmp;

DigitMain::DigitMain(QString exportFileAtShutdown) :
  m_activeView(0),
  m_exportFileAtShutdown(exportFileAtShutdown)
{
  DigitDebug::ctor(QString("digitmain ") + QString::number((ulong) this, 16));
  
  setCaption(tr("Engauge Digitizer %1%2")
    .arg(VersionNumber)
    .arg(freewareCaption));
  printer = new QPrinter;
  CHECK_PTR_ENGAUGE(printer);
  docList = new Q3PtrList<DigitDoc>();
  CHECK_PTR_ENGAUGE(docList);
  docList->setAutoDelete(true);

  QPixmap bannerIcon(bannerapp_xpm);
  setIcon(bannerIcon);

  // enable import by dropping
  setAcceptDrops(true);
  
  // display main window in previous position with same size
  DefaultSettings& rSettings = DefaultSettings::instance();
  move(rSettings.getWindowMainPosition());
  resize(rSettings.getWindowMainSize());
    
  // call inits to invoke all other construction parts
  initView();
  initActions();
  initMenuBar();
  initToolBars();
  initStatusBar();
  initGeometryInfo();

  viewFileToolBar->setOn(rSettings.getViewFileToolbar());
  viewSelectToolBar->setOn(rSettings.getViewSelectToolbar());
  viewImageScaleToolBar->setOn(rSettings.getViewImageScaleToolbar());
  viewDigitizeCurvePointsToolBar->setOn(rSettings.getViewDigitizeCurvePointsToolbar());
  viewDigitizeMeasurePointsToolBar->setOn(rSettings.getViewDigitizeMeasurePointsToolbar());
  viewZoomToolBar->setOn(rSettings.getViewZoomToolbar());
  viewStatusBar->setOn(rSettings.getViewStatusBar());
  viewAxesPoints->setOn(ViewAxesPoints == rSettings.getViewPoints());
  viewScalePoints->setOn(ViewScalePoints == rSettings.getViewPoints());
  viewCurvePoints->setOn(ViewCurvePoints == rSettings.getViewPoints());
  viewMeasurePoints->setOn(ViewMeasurePoints == rSettings.getViewPoints());
  viewAllPoints->setOn(ViewAllPoints == rSettings.getViewPoints());
  viewNoBackground->setOn(BlankBackground == rSettings.getViewBackground());
  viewOriginalImage->setOn(OriginalImage == rSettings.getViewBackground());
  viewProcessedImage->setOn(ProcessedImage == rSettings.getViewBackground());
  viewGridDisplay->setOn(false); // always start with grid display off since it requires transformation
  viewCurveGeometry->setOn(rSettings.getViewCurveGeometry());
  viewMeasureGeometry->setOn(rSettings.getViewMeasureGeometry());

  toggleActions();
}

DigitMain::~DigitMain()
{
  DigitDebug::dtor(QString("digitmain ") + QString::number((ulong) this, 16));

  delete printer;

	delete docList;
  delete curveGeometryDlg;
  delete measureGeometryDlg;
}

void DigitMain::initActions()
{
  // xpm files created by gimp will give the compilation error
  // "warning: deprecated conversion from string constant to char*" during compilation.
  // these warnings can be prevented by inserting "const" in between "static" and "char*"
  QPixmap importIcon(fileimport_xpm);
  QPixmap openIcon(fileopen_xpm);
  QPixmap saveIcon(filesave_xpm);
  QPixmap exportIcon(fileexport_xpm);
  QPixmap pixmapSelectSmall(digitselectsmall_xpm), pixmapSelectLarge(digitselectlarge_xpm);
  QPixmap pixmapAxisSmall(digitaxissmall_xpm), pixmapAxisLarge(digitaxislarge_xpm);
  QPixmap pixmapScaleSmall(digitscalesmall_xpm), pixmapScaleLarge(digitscalelarge_xpm);
  QPixmap pixmapCurveSmall(digitcurvesmall_xpm), pixmapCurveLarge(digitcurvelarge_xpm);
  QPixmap pixmapSegmentSmall(digitsegmentsmall_xpm), pixmapSegmentLarge(digitsegmentlarge_xpm);
  QPixmap pixmapMatchSmall(digitmatchsmall_xpm), pixmapMatchLarge(digitmatchlarge_xpm);
  QPixmap pixmapMeasureSmall(digitmeasuresmall_xpm), pixmapMeasureLarge(digitmeasurelarge_xpm);
  QIcon selectIcons(pixmapSelectSmall);
  QIcon axisIcons(pixmapAxisSmall);
  QIcon scaleIcons(pixmapScaleSmall);
  QIcon curveIcons(pixmapCurveSmall);
  QIcon segmentIcons(pixmapSegmentSmall);
  QIcon matchIcons(pixmapMatchSmall);
  QIcon measureIcons(pixmapMeasureSmall);

  fileImport = createAction(tr("&Import File"), importIcon, tr("&Import"), tr("Ctrl+I"), this);
  CHECK_PTR_ENGAUGE(fileImport);
  fileImport->setStatusTip(tr("Creates a new document by importing an image"));
  fileImport->setWhatsThis(tr("New File\n\nCreates a new document by importing an image"));
  connect(fileImport, SIGNAL(activated()), this, SLOT(slotFileImport()));

  fileOpen = createAction(tr("&Open Document"), openIcon, tr("&Open..."), 0, this);
  CHECK_PTR_ENGAUGE(fileOpen);
  fileOpen->setStatusTip(tr("Opens an existing document"));
  fileOpen->setWhatsThis(tr("Open Document\n\nOpens an existing document"));
  connect(fileOpen, SIGNAL(activated()), this, SLOT(slotFileOpen()));

  fileOpenRecent = new QMenu(tr("Open Recent Document"), this);
	//createAction(tr("Open Recent Document"), tr("Open Recent..."), 0, this);
  //CHECK_PTR_ENGAUGE(fileOpenRecent);
  //fileOpenRecent->setStatusTip(tr("Opens a recent document"));
  //fileOpenRecent->setWhatsThis(tr("Open File\n\nOpens a recent document"));

  fileClose = createAction(tr("&Close Document"), tr("&Close"), tr("Ctrl+W"), this);
  CHECK_PTR_ENGAUGE(fileClose);
  fileClose->setStatusTip(tr("Closes the current document"));
  fileClose->setWhatsThis(tr("Close Document\n\nCloses the current document"));
  connect(fileClose, SIGNAL(activated()), this, SLOT(slotFileClose()));

  fileSave = createAction(tr("&Save Document"), saveIcon, tr("&Save"), tr("Ctrl+S"), this);
  CHECK_PTR_ENGAUGE(fileSave);
  fileSave->setStatusTip(tr("Saves the current document"));
  fileSave->setWhatsThis(tr("Save Document\n\nSaves the current document"));
  connect(fileSave, SIGNAL(activated()), this, SLOT(slotFileSave()));

  fileSaveAs = createAction(tr("Save Document &As"), tr("Save &as..."), tr("Ctrl+A"), this);
  CHECK_PTR_ENGAUGE(fileSaveAs);
  fileSaveAs->setStatusTip(tr("Saves the current document under a new filename"));
  fileSaveAs->setWhatsThis(tr("Save As\n\nSaves the current document under a new filename"));
  connect(fileSaveAs, SIGNAL(activated()), this, SLOT(slotFileSaveAs()));

  fileExport = createAction(tr("&Export File"), exportIcon, tr("&Export"), tr("Ctrl+E"), this);
  CHECK_PTR_ENGAUGE(fileExport);
  fileExport->setStatusTip(tr("Exports the current document into a text file"));
  fileExport->setWhatsThis(tr("Export File\n\nExports the current document into a text file"));
  connect(fileExport, SIGNAL(activated()), this, SLOT(slotFileExport()));

  fileExportAs = createAction(tr("Export File As"), tr("Export As..."), 0, this);
  CHECK_PTR_ENGAUGE(fileExportAs);
  fileExportAs->setStatusTip(tr("Exports the current document into a text file under a new filename"));
  fileExportAs->setWhatsThis(tr("Export As\n\nExports the current document into a text file under a new filename"));
  connect(fileExportAs, SIGNAL(activated()), this, SLOT(slotFileExportAs()));

  filePrint = createAction(tr("&Print File"), tr("&Print"), tr("Ctrl+P"), this);
  CHECK_PTR_ENGAUGE(filePrint);
  filePrint->setStatusTip(tr("Prints out the current document"));
  filePrint->setWhatsThis(tr("Print File\n\nPrints out the current document"));
  connect(filePrint, SIGNAL(activated()), this, SLOT(slotFilePrint()));

  fileExit = createAction(tr("E&xit"), tr("E&xit"), tr("Ctrl+Q"), this);
  CHECK_PTR_ENGAUGE(fileExit);
  fileExit->setStatusTip(tr("Quits the application"));
  fileExit->setWhatsThis(tr("Exit\n\nQuits the application"));
  connect(fileExit, SIGNAL(activated()), this, SLOT(slotFileExit()));

  editUndo = createAction(tr("&Undo"), tr("&Undo"), tr("Ctrl+U"), this);
  CHECK_PTR_ENGAUGE(editUndo);
  editUndo->setStatusTip(tr("Undoes the previous action"));
  editUndo->setWhatsThis(tr("Undo\n\nUndoes the previous action"));
  connect(editUndo, SIGNAL(activated()), this, SLOT(slotEditUndo()));

  editRedo = createAction(tr("&Redo"), tr("&Redo"), tr("Ctrl+R"), this);
  CHECK_PTR_ENGAUGE(editRedo);
  editRedo->setStatusTip(tr("Redoes the next action"));
  editRedo->setWhatsThis(tr("Redo\n\nRedoes the next action"));
  connect(editRedo, SIGNAL(activated()), this, SLOT(slotEditRedo()));

  editCut = createAction(tr("Cu&t"), tr("Cu&t"), globalKeyCut, this);
  CHECK_PTR_ENGAUGE(editCut);
  editCut->setStatusTip(tr("Cuts the selected section and puts it to the clipboard"));
  editCut->setWhatsThis(tr("Cut\n\nCuts the selected section and puts it to the clipboard"));
  connect(editCut, SIGNAL(activated()), this, SLOT(slotEditCut()));

  editCopy = createAction(tr("&Copy"), tr("&Copy"), globalKeyCopy, this);
  CHECK_PTR_ENGAUGE(editCopy);
  editCopy->setStatusTip(tr("Copies the selected section to the clipboard"));
  editCopy->setWhatsThis(tr("Copy\n\nCopies the selected section to the clipboard"));
  connect(editCopy, SIGNAL(activated()), this, SLOT(slotEditCopy()));

  editPaste = createAction(tr("&Paste"), tr("&Paste"), globalKeyPaste, this);
  CHECK_PTR_ENGAUGE(editPaste);
  editPaste->setStatusTip(tr("Pastes the clipboard contents to actual position"));
  editPaste->setWhatsThis(tr("Paste\n\nPastes the clipboard contents to actual position"));
  connect(editPaste, SIGNAL(activated()), this, SLOT(slotEditPaste()));

  editPasteAsNew = createAction(tr("Paste As &New"), tr("Paste As &New"), tr("Ctrl+N"), this);
  CHECK_PTR_ENGAUGE(editPasteAsNew);
  editPasteAsNew->setStatusTip(tr("Pastes the image in the clipboard as a new document"));
  editPasteAsNew->setWhatsThis(tr("Paste As New\n\nPastes the image in the clipboard as a new document"));
  connect(editPasteAsNew, SIGNAL(activated()), this, SLOT(slotEditPasteAsNew()));
  connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(slotClipboardChanged()));

  viewFileToolBar = createAction(tr("File Tool&bar"), tr("File Tool&bar"), 0, this, 0, true);
  CHECK_PTR_ENGAUGE(viewFileToolBar);
  viewFileToolBar->setStatusTip(tr("Enables/disables the file toolbar"));
  viewFileToolBar->setWhatsThis(tr("View File Toolbar\n\nEnables/disables the file toolbar"));
  connect(viewFileToolBar, SIGNAL(toggled(bool)), this, SLOT(slotViewFileToolBar(bool)));

  viewSelectToolBar = createAction(tr("Select Toolbar"), tr("Select Toolbar"), 0, this, 0, true);
  CHECK_PTR_ENGAUGE(viewSelectToolBar);
  viewSelectToolBar->setStatusTip(tr("Enables/disables the select toolbar"));
  viewSelectToolBar->setWhatsThis(tr("View Digitize Common toolbar\n\nEnables/disables the select toolbar"));
  connect(viewSelectToolBar, SIGNAL(toggled(bool)), this, SLOT(slotViewSelectToolBar(bool)));

  viewImageScaleToolBar = createAction(tr("Image Scale Toolbar"), tr("Image Scale Toolbar"), 0, this, 0, true);
  CHECK_PTR_ENGAUGE(viewImageScaleToolBar);
  viewImageScaleToolBar->setStatusTip(tr("Enables/disables the image scale toolbar"));
  viewImageScaleToolBar->setWhatsThis(tr("View Image Scale toolbar\n\nEnables/disables the image scale toolbar"));
  connect(viewImageScaleToolBar, SIGNAL(toggled(bool)), this, SLOT(slotViewImageScaleToolBar(bool)));

  viewDigitizeCurvePointsToolBar = createAction(tr("Digitize Curve Points Toolbar"), tr("Digitize Curve Points Toolbar"), 0, this, 0, true);
  CHECK_PTR_ENGAUGE(viewDigitizeCurvePointsToolBar);
  viewDigitizeCurvePointsToolBar->setStatusTip(tr("Enables/disables the toolbar for digitizing curve points"));
  viewDigitizeCurvePointsToolBar->setWhatsThis(tr("View Digitize Curve Points Scale toolbar\n\nEnables/disables the toolbar for digitizing curve points"));
  connect(viewDigitizeCurvePointsToolBar, SIGNAL(toggled(bool)), this, SLOT(slotViewDigitizeCurvePointsToolBar(bool)));

  viewDigitizeMeasurePointsToolBar = createAction(tr("Digitize Measure Points Toolbar"), tr("Digitize Measure Points Toolbar"), 0, this, 0, true);
  CHECK_PTR_ENGAUGE(viewDigitizeMeasurePointsToolBar);
  viewDigitizeMeasurePointsToolBar->setStatusTip(tr("Enables/disables the toolbar for digitizing measure points"));
  viewDigitizeMeasurePointsToolBar->setWhatsThis(tr("View Digitize Measure Points Scale toolbar\n\nEnables/disables the toolbar for digitizing measure points"));
  connect(viewDigitizeMeasurePointsToolBar, SIGNAL(toggled(bool)), this, SLOT(slotViewDigitizeMeasurePointsToolBar(bool)));

  viewZoomToolBar = createAction(tr("&Zoom Toolbar"), tr("&Zoom Toolbar"), 0, this, 0, true);
  CHECK_PTR_ENGAUGE(viewZoomToolBar);
  viewZoomToolBar->setStatusTip(tr("Enables/disables the zoom toolbar"));
  viewZoomToolBar->setWhatsThis(tr("View Zoom Toolbar\n\nEnables/disables the zoom toolbar"));
  connect(viewZoomToolBar, SIGNAL(toggled(bool)), this, SLOT(slotViewZoomToolBar(bool)));

  viewStatusBar = createAction(tr("&Statusbar"), tr("&Statusbar"), 0, this, 0, true);
  CHECK_PTR_ENGAUGE(viewStatusBar);
  viewStatusBar->setStatusTip(tr("Enables/disables the statusbar"));
  viewStatusBar->setWhatsThis(tr("View Statusbar\n\nEnables/disables the statusbar"));
  connect(viewStatusBar, SIGNAL(toggled(bool)), this, SLOT(slotViewStatusBar(bool)));

  viewAxesPoints = createAction(tr("Axes Points"), tr("Axes Points"), 0, this, 0, true);
  CHECK_PTR_ENGAUGE(viewAxesPoints);
  viewAxesPoints->setStatusTip(tr("Show only axes points"));
  viewAxesPoints->setWhatsThis(tr("View Axes Points\n\nShow only axes points, and hide all other points"));
  connect(viewAxesPoints, SIGNAL(toggled(bool)), this, SLOT(slotViewAxesPoints(bool)));

  viewScalePoints = createAction(tr("Scale Bar Points"), tr("Scale Bar Points"), 0, this, 0, true);
  CHECK_PTR_ENGAUGE(viewScalePoints);
  viewScalePoints->setStatusTip(tr("Show only the scale bar"));
  viewScalePoints->setWhatsThis(tr("View Scale Bar\n\nShow only the scale bar, and hide all other points"));
  connect(viewScalePoints, SIGNAL(toggled(bool)), this, SLOT(slotViewScalePoints(bool)));

  viewCurvePoints = createAction(tr("Curve Points"), tr("Curve Points"), 0, this, 0, true);
  CHECK_PTR_ENGAUGE(viewCurvePoints);
  viewCurvePoints->setStatusTip(tr("Show only points in the active curve"));
  viewCurvePoints->setWhatsThis(tr("View Curve Points\n\nShow only points in the active curve, and hide all others"));
  connect(viewCurvePoints, SIGNAL(toggled(bool)), this, SLOT(slotViewCurvePoints(bool)));

  viewMeasurePoints = createAction(tr("Measure Points"), tr("Measure Points"), 0, this, 0, true);
  CHECK_PTR_ENGAUGE(viewMeasurePoints);
  viewMeasurePoints->setStatusTip(tr("Show only points in the active measure"));
  viewMeasurePoints->setWhatsThis(tr("View Measure Points\n\nShow only points in the active measure, and hide all others"));
  connect(viewMeasurePoints, SIGNAL(toggled(bool)), this, SLOT(slotViewMeasurePoints(bool)));

  viewAllPoints = createAction(tr("All Points"), tr("All Points"), 0, this, 0, true);
  CHECK_PTR_ENGAUGE(viewAllPoints);
  viewAllPoints->setStatusTip(tr("Show all axes, scale bar, curve and measure points"));
  viewAllPoints->setWhatsThis(tr("View All Points\n\nShow all axes, scale bar, curve and measure points"));
  connect(viewAllPoints, SIGNAL(toggled(bool)), this, SLOT(slotViewAllPoints(bool)));

  viewPointsAction = new QActionGroup(this);
  CHECK_PTR_ENGAUGE(viewPointsAction);
  viewPointsAction->addAction(viewAxesPoints);
  viewPointsAction->addAction(viewScalePoints);
  viewPointsAction->addAction(viewCurvePoints);
  viewPointsAction->addAction(viewMeasurePoints);
  viewPointsAction->addAction(viewAllPoints);

  viewNoBackground = createAction(tr("No Background"), tr("No Background"), 0, this, 0, true);
  CHECK_PTR_ENGAUGE(viewNoBackground);
  viewNoBackground->setStatusTip(tr("Show no background"));
  viewNoBackground->setWhatsThis(tr("No Background\n\nShow no background image"));
  connect(viewNoBackground, SIGNAL(toggled(bool)), this, SLOT(slotViewNoBackground(bool)));

  viewOriginalImage = createAction(tr("Original Image"), tr("Original Image"), 0, this, 0, true);
  CHECK_PTR_ENGAUGE(viewOriginalImage);
  viewOriginalImage->setStatusTip(tr("Show original image in the background"));
  viewOriginalImage->setWhatsThis(tr("View Original Image\n\nShow original image in the background"));
  connect(viewOriginalImage, SIGNAL(toggled(bool)), this, SLOT(slotViewOriginalImage(bool)));

  viewProcessedImage = createAction(tr("Processed Image"), tr("Processed Image"), 0, this, 0, true);
  CHECK_PTR_ENGAUGE(viewProcessedImage);
  viewProcessedImage->setStatusTip(tr("Show processed image in the background"));
  viewProcessedImage->setWhatsThis(tr("View Processed Image\n\nShow processed image in the background. "
    "The processed image is the result of removing gridlines from the original image, and "
    "discretizing the image into black and white. The processed image is used for segment fill, "
    "and is often less messy than the original image."));
  connect(viewProcessedImage, SIGNAL(toggled(bool)), this, SLOT(slotViewProcessedImage(bool)));

  viewBackgroundAction = new QActionGroup(this);
  CHECK_PTR_ENGAUGE(viewBackgroundAction);
  viewBackgroundAction->addAction(viewNoBackground);
  viewBackgroundAction->addAction(viewOriginalImage);
  viewBackgroundAction->addAction(viewProcessedImage);

  viewGridDisplay = createAction(tr("&Gridlines Display"), tr("&Gridlines Display"), 0, this, 0, true);
  CHECK_PTR_ENGAUGE(viewGridDisplay);
  viewGridDisplay->setStatusTip(tr("Enables/disables the gridlines display"));
  viewGridDisplay->setWhatsThis(tr("View Gridlines\n\nEnables/disables the gridlines display"));
  connect(viewGridDisplay, SIGNAL(toggled(bool)), this, SLOT(slotViewGridDisplay(bool)));

  viewCurveGeometry = createAction(tr("Curve Geometry"), tr("Curve Geometry &Info..."), 0, this, 0, true);
  CHECK_PTR_ENGAUGE(viewCurveGeometry);
  viewCurveGeometry->setStatusTip(tr("Enables/disables the curve geometry information window"));
  viewCurveGeometry->setWhatsThis(tr("View Curve Geometry\n\nEnables/disables the curve geometry "
    "information window. Lengths and areas of the active curve are displayed"));
  connect(viewCurveGeometry, SIGNAL(toggled(bool)), this, SLOT(slotViewCurveGeometry(bool)));

  viewMeasureGeometry = createAction(tr("Measure Geometry"), tr("Measure Geometry &Info..."), 0, this, 0, true);
  CHECK_PTR_ENGAUGE(viewMeasureGeometry);
  viewMeasureGeometry->setStatusTip(tr("Enables/disables the measure geometry information window"));
  viewMeasureGeometry->setWhatsThis(tr("View Measure Geometry\n\nEnables/disables the measure geometry "
    "information window. Lengths and areas of the active measure are displayed"));
  connect(viewMeasureGeometry, SIGNAL(toggled(bool)), this, SLOT(slotViewMeasureGeometry(bool)));

  digitizeSelect = createAction(tr("&Select"), selectIcons, tr("&Select"), tr("Ctrl+S"), this, 0, true);
  CHECK_PTR_ENGAUGE(digitizeSelect);
  digitizeSelect->setStatusTip(tr("Select one or more points on screen"));
  digitizeSelect->setWhatsThis(tr("Select\n\nSelect one or more points on screen"));
  connect(digitizeSelect, SIGNAL(toggled(bool)), this, SLOT(slotDigitizeSelect(bool)));

  digitizeAxis = createAction(tr("&Axis Point"), axisIcons, tr("&Axis Point"), 0, this, 0, true);
  CHECK_PTR_ENGAUGE(digitizeAxis);
  digitizeAxis->setStatusTip(tr("Digitizes an axis point"));
  digitizeAxis->setWhatsThis(tr("Digitize Axis Point\n\nDigitizes an axis point by placing a new point "
    "at the cursor. The coordinates of the axis point are then entered. In a graph, three axis points are "
    "required to define the coordinates"));
  connect(digitizeAxis, SIGNAL(toggled(bool)), this, SLOT(slotDigitizeAxis(bool)));

  digitizeScale = createAction(tr("&Scale Bar"), scaleIcons, tr("&Scale Bar"), 0, this, 0, true);
  CHECK_PTR_ENGAUGE(digitizeScale);
  digitizeScale->setStatusTip(tr("Digitizes a scale bar"));
  digitizeScale->setWhatsThis(tr("Digitize Scale Bar\n\nDigitizes a scale bar by placing a new point "
    "at the cursor. The length of the scale bar is entered. In a map, a scale bar is required to "
    "define the coordinate scale\n\nThis can only be used with linear cartesian coordinates, so it "
    "will be unavailable if log and/or polar coordinates have been selected"));
  connect(digitizeScale, SIGNAL(toggled(bool)), this, SLOT(slotDigitizeScale(bool)));

  digitizeCurve = createAction(tr("&Curve Point"), curveIcons, tr("&Curve Point"), 0, this, 0, true);
  CHECK_PTR_ENGAUGE(digitizeCurve);
  digitizeCurve->setStatusTip(tr("Digitizes a curve point"));
  digitizeCurve->setWhatsThis(tr("Digitize Curve Point\n\nDigitizes a single curve point under the cursor\n\n"
    "New points will be assigned to the currently active curve"));
  connect(digitizeCurve, SIGNAL(toggled(bool)), this, SLOT(slotDigitizeCurve(bool)));

  digitizeSegment = createAction(tr("&Segment Fill"), segmentIcons, tr("&Segment Fill"), 0, this, 0, true);
  CHECK_PTR_ENGAUGE(digitizeSegment);
  digitizeSegment->setStatusTip(tr("Digitizes curve points along a curve segment"));
  digitizeSegment->setWhatsThis(tr("Digitize Segment Fill\n\nDigitizes a curve segment by placing curve points "
    "along the segment under the cursor\n\n"
    "New points will be assigned to the currently active curve"));
  connect(digitizeSegment, SIGNAL(toggled(bool)), this, SLOT(slotDigitizeSegment(bool)));

  digitizePointMatch = createAction(tr("&Point Match"), matchIcons, tr("&Point Match"), 0, this, 0, true);
  CHECK_PTR_ENGAUGE(digitizePointMatch);
  digitizePointMatch->setStatusTip(tr("Digitizes curve points in a point plot by matching a point"));
  digitizePointMatch->setWhatsThis(tr("Digitize Curve Points by Point Matching\n\nDigitizes curve points in a "
    "point plot by finding points that match a sample point\n\n"
    "New points will be assigned to the currently active curve"));
  connect(digitizePointMatch, SIGNAL(toggled(bool)), this, SLOT(slotDigitizePointMatch(bool)));

  digitizeMeasure = createAction(tr("&Measure Point"), measureIcons, tr("&Measure Point"), 0, this, 0, true);
  CHECK_PTR_ENGAUGE(digitizeMeasure);
  digitizeMeasure->setStatusTip(tr("Digitizes a measure point for measuring length and area"));
  digitizeMeasure->setWhatsThis(tr("Digitize Measure Point\n\nDigitizes a measure point by placing a new point "
    "at the cursor. Measure points give length and area values for the underlying regions\n\n"
    "New points will be assigned to the currently active measure"));
  connect(digitizeMeasure, SIGNAL(toggled(bool)), this, SLOT(slotDigitizeMeasure(bool)));

  digitizeAction = new QActionGroup(this);
  CHECK_PTR_ENGAUGE(digitizeAction);
  digitizeAction->addAction(digitizeSelect);
  digitizeAction->addAction(digitizeAxis);
  digitizeAction->addAction(digitizeScale);
  digitizeAction->addAction(digitizeCurve);
  digitizeAction->addAction(digitizeSegment);
  digitizeAction->addAction(digitizePointMatch);
  digitizeAction->addAction(digitizeMeasure);

  settingsCoordSys = createAction(tr("Coordinates"), tr("Coordinates..."), 0, this);
  CHECK_PTR_ENGAUGE(settingsCoordSys);
  settingsCoordSys->setStatusTip(tr("Edits the coordinate system"));
  settingsCoordSys->setWhatsThis(tr("Coordinate System Settings\n\nEdits the coordinate system properties"));
  connect(settingsCoordSys, SIGNAL(activated()), this, SLOT(slotSettingsCoordSys()));

  settingsAxes = createAction(tr("Axes"), tr("Axes..."), 0, this);
  CHECK_PTR_ENGAUGE(settingsAxes);
  settingsAxes->setStatusTip(tr("Edits the axes properties"));
  settingsAxes->setWhatsThis(tr("Axes Settings\n\nEdits the axes properties"));
  connect(settingsAxes, SIGNAL(activated()), this, SLOT(slotSettingsAxes()));

  settingsScale = createAction(tr("Scale Bar"), tr("Scale Bar..."), 0, this);
  CHECK_PTR_ENGAUGE(settingsScale);
  settingsScale->setStatusTip(tr("Edits the scale bar properties"));
  settingsScale->setWhatsThis(tr("Scale Bar Settings\n\nEdits the scale bar properties"));
  connect(settingsScale, SIGNAL(activated()), this, SLOT(slotSettingsScale()));

  settingsCurves = createAction(tr("Curves"), tr("Curves..."), 0, this);
  CHECK_PTR_ENGAUGE(settingsCurves);
  settingsCurves->setStatusTip(tr("Edits the curve properties"));
  settingsCurves->setWhatsThis(tr("Curve Settings\n\nEdits the properties of the curves"));
  connect(settingsCurves, SIGNAL(activated()), this, SLOT(slotSettingsCurves()));

  settingsSegments = createAction(tr("Segments"), tr("Segments..."), 0, this);
  CHECK_PTR_ENGAUGE(settingsSegments);
  settingsSegments->setStatusTip(tr("Edits the segment fill properties"));
  settingsSegments->setWhatsThis(tr("Segment Fill Settings\n\nEdits the segment fill properties"));
  connect(settingsSegments, SIGNAL(activated()), this, SLOT(slotSettingsSegments()));

  settingsPointMatch = createAction(tr("Point Match"), tr("Point Match..."), 0, this);
  CHECK_PTR_ENGAUGE(settingsPointMatch);
  settingsPointMatch->setStatusTip(tr("Edits the point match properties"));
  settingsPointMatch->setWhatsThis(tr("Point Match Settings\n\nEdits the point match properties"));
  connect(settingsPointMatch, SIGNAL(activated()), this, SLOT(slotSettingsPointMatch()));

  settingsMeasures = createAction(tr("Measures"), tr("Measures..."), 0, this);
  CHECK_PTR_ENGAUGE(settingsMeasures);
  settingsMeasures->setStatusTip(tr("Edits the measure properties"));
  settingsMeasures->setWhatsThis(tr("Measures Settings\n\nEdits the measure properties"));
  connect(settingsMeasures, SIGNAL(activated()), this, SLOT(slotSettingsMeasures()));

  settingsDiscretize = createAction(tr("Discretize"), tr("Discretize..."), 0, this);
  CHECK_PTR_ENGAUGE(settingsDiscretize);
  settingsDiscretize->setStatusTip(tr("Edits the discretization properties"));
  settingsDiscretize->setWhatsThis(tr("Discretize Settings\n\nEdits the discretize properties, which affect the processed image"));
  connect(settingsDiscretize, SIGNAL(activated()), this, SLOT(slotSettingsDiscretize()));

  settingsGridRemoval = createAction(tr("Grid Removal"), tr("Grid Removal..."), 0, this);
  CHECK_PTR_ENGAUGE(settingsGridRemoval);
  settingsGridRemoval->setStatusTip(tr("Edits the grid removal properties"));
  settingsGridRemoval->setWhatsThis(tr("Grid Removal Settings\n\nEdits the grid removal properties, which affect the processed image"));
  connect(settingsGridRemoval, SIGNAL(activated()), this, SLOT(slotSettingsGridRemoval()));

  settingsGridDisplay = createAction(tr("Grid Display"), tr("Grid Display..."), 0, this);
  CHECK_PTR_ENGAUGE(settingsGridDisplay);
  settingsGridDisplay->setStatusTip(tr("Edits the grid display properties"));
  settingsGridDisplay->setWhatsThis(tr("Grid Display Settings\n\nEdits the grid display properties"));
  connect(settingsGridDisplay, SIGNAL(activated()), this, SLOT(slotSettingsGridDisplay()));

  settingsExportSetup = createAction(tr("Export Setup"), tr("Export Setup..."), 0, this);
  CHECK_PTR_ENGAUGE(settingsExportSetup);
  settingsExportSetup->setStatusTip(tr("Configure the export settings"));
  settingsExportSetup->setWhatsThis(tr("Export Setup\n\nConfigure the export settings"));
  connect(settingsExportSetup, SIGNAL(activated()), this, SLOT(slotSettingsExportSetup()));

  settingsSessions = createAction(tr("Sessions"), tr("Sessions..."), 0, this);
  CHECK_PTR_ENGAUGE(settingsSessions);
  settingsSessions->setStatusTip(tr("Select settings to be saved between sessions"));
  settingsSessions->setWhatsThis(tr("Sessions Setup\n\nSelect settings to be saved between sessions"));
  connect(settingsSessions, SIGNAL(activated()), this, SLOT(slotSettingsSessions()));

  windowNewWindow = createAction(tr("&New Window"), tr("&New Window"), 0, this);
  CHECK_PTR_ENGAUGE(windowNewWindow);
  windowNewWindow->setStatusTip(tr("Opens a new view for the current document"));
  windowNewWindow->setWhatsThis(tr("New Window\n\nOpens a new view for the current document"));
  connect(windowNewWindow, SIGNAL(activated()), this, SLOT(slotWindowNewWindow()));

  windowCascade = createAction(tr("&Cascade"), tr("&Cascade"), 0, this);
  CHECK_PTR_ENGAUGE(windowCascade);
  windowCascade->setStatusTip(tr("Cascades all windows"));
  windowCascade->setWhatsThis(tr("Cascade\n\nCascades all windows"));
  connect(windowCascade, SIGNAL(activated()), workspace, SLOT(cascadeSubWindows()));

  windowTile = createAction(tr("&Tile"), tr("&Tile"), 0, this);
  CHECK_PTR_ENGAUGE(windowTile);
  windowTile->setStatusTip(tr("Tiles all windows"));
  windowTile->setWhatsThis(tr("Tile\n\nTiles all windows"));
  connect(windowTile, SIGNAL(activated()), workspace, SLOT(tileSubWindows()));

  windowAction = new QActionGroup(this);
  CHECK_PTR_ENGAUGE(windowAction);
  windowAction->addAction(windowNewWindow);
  windowAction->addAction(windowCascade);
  windowAction->addAction(windowTile);

  helpAboutApp = createAction(tr("&About Engauge"), tr("&About Engauge..."), 0, this);
  CHECK_PTR_ENGAUGE(helpAboutApp);
  helpAboutApp->setStatusTip(tr("About the application"));
  helpAboutApp->setWhatsThis(tr("About Engauge\n\nAbout the application"));
  connect(helpAboutApp, SIGNAL(activated()), this, SLOT(slotHelpAbout()));

  helpUserManual = createAction(tr("&User Manual"), tr("&User Manual..."), 0, this);
  CHECK_PTR_ENGAUGE(helpUserManual);
  helpUserManual->setStatusTip(tr("Browse user manual"));
  helpUserManual->setWhatsThis(tr("User Manual\n\nBrowse user manual"));
  connect(helpUserManual, SIGNAL(activated()), this, SLOT(slotHelpUserManual()));

  helpGlossary = createAction(tr("&Glossary"), tr("&Glossary..."), 0, this);
  CHECK_PTR_ENGAUGE(helpGlossary);
  helpGlossary->setStatusTip(tr("Browse glossary"));
  helpGlossary->setWhatsThis(tr("Glossary\n\nBrowse glossary"));
  connect(helpGlossary, SIGNAL(activated()), this, SLOT(slotHelpGlossary()));

  helpConverter = createAction(tr("&Date/Time Converter"), tr("&Date/Time Converter..."), 0, this);
  CHECK_PTR_ENGAUGE(helpConverter);
  helpConverter->setStatusTip(tr("Convert dates and times in browser"));
  helpConverter->setWhatsThis(tr("Converter\n\nConvert dates and times in browser"));
  connect(helpConverter, SIGNAL(activated()), this, SLOT(slotConverter()));
                              
  helpLineGraphTutorial = createAction(tr("&Line Graph Tutorial"), tr("&Line Graph Tutorial..."), 0, this);
  CHECK_PTR_ENGAUGE(helpLineGraphTutorial);
  helpLineGraphTutorial->setStatusTip(tr("Browse manual line graph tutorial"));
  helpLineGraphTutorial->setWhatsThis(tr("Line Graph Tutorial\n\nBrowse manual line graph tutorial"));
  connect(helpLineGraphTutorial, SIGNAL(activated()), this, SLOT(slotHelpLineGraphTutorial()));

  helpPointGraphTutorial = createAction(tr("&Point Graph Tutorial"), tr("&Point Graph Tutorial..."), 0, this);
  CHECK_PTR_ENGAUGE(helpPointGraphTutorial);
  helpPointGraphTutorial->setStatusTip(tr("Browse manual point graph tutorial"));
  helpPointGraphTutorial->setWhatsThis(tr("Point Graph Tutorial\n\nBrowse manual point graph tutorial"));
  connect(helpPointGraphTutorial, SIGNAL(activated()), this, SLOT(slotHelpPointGraphTutorial()));

  helpMapTutorial = createAction(tr("&Map Tutorial"), tr("&Map Tutorial..."), 0, this);
  CHECK_PTR_ENGAUGE(helpMapTutorial);
  helpMapTutorial->setStatusTip(tr("Browse map tutorial"));
  helpMapTutorial->setWhatsThis(tr("Map Tutorial\n\nBrowse map tutorial"));
  connect(helpMapTutorial, SIGNAL(activated()), this, SLOT(slotHelpMapTutorial()));
}

void DigitMain::initGeometryInfo()
{
  curveGeometryDlg = new GeometryDlg(this, true);
  CHECK_PTR_ENGAUGE(curveGeometryDlg);
  connect(curveGeometryDlg, SIGNAL(hideGeometry()), this, SLOT(slotHideCurveGeometry()));

  measureGeometryDlg = new GeometryDlg(this, false);
  CHECK_PTR_ENGAUGE(measureGeometryDlg);
  connect(measureGeometryDlg, SIGNAL(hideGeometry()), this, SLOT(slotHideMeasureGeometry()));

  noGeometryInfo();
}

void DigitMain::initMenuBar()
{
  // menuBar entry fileMenu
  fileMenu = new QMenu(this);
  CHECK_PTR_ENGAUGE(fileMenu);
  fileImport->addTo(fileMenu);
  fileOpen->addTo(fileMenu);
  QSize mruFrameOffset = fileMenu->sizeHint(); // size() is unreliable because of performance optimization
#ifndef Q_OS_MACX
  //fileOpenRecent->addMenu(fileMenu);
  fileMenu->addMenu(fileOpenRecent);
  const int INDEX_FILEOPENRECENT = 2; // import at index 0, open at index 1, open recent at index 2
  m_mruDocuments.attachPopupMenu(fileMenu, fileMenu->idAt(INDEX_FILEOPENRECENT), mruFrameOffset);
#endif
  fileClose->addTo(fileMenu);
  fileMenu->insertSeparator();
  fileSave->addTo(fileMenu);
  fileSaveAs->addTo(fileMenu);
  fileExport->addTo(fileMenu);
  fileExportAs->addTo(fileMenu);
  fileMenu->insertSeparator();
  filePrint->addTo(fileMenu);
  fileMenu->insertSeparator();
  fileExit->addTo(fileMenu);

  connect(fileMenu, SIGNAL(highlighted(int)), &m_mruDocuments, SLOT(slotActionHighlighted(int)));
  connect(fileMenu, SIGNAL(aboutToHide()), &m_mruDocuments, SLOT(slotActionUnhighlighted()));
  connect(&m_mruDocuments, SIGNAL(mruChosen(QString)), this, SLOT(slotFileOpenRecent(QString)));

  // menuBar entry editMenu
  editMenu = new QMenu(this);
  CHECK_PTR_ENGAUGE(editMenu);
  editCut->addTo(editMenu);
  editCopy->addTo(editMenu);
  editPaste->addTo(editMenu);
  editPasteAsNew->addTo(editMenu);

  // menuBar entry viewMenu
  viewMenu = new QMenu(this);
  CHECK_PTR_ENGAUGE(viewMenu);
  viewMenu->setCheckable(true);
  viewFileToolBar->addTo(viewMenu);
  viewSelectToolBar->addTo(viewMenu);
  viewImageScaleToolBar->addTo(viewMenu);
  viewDigitizeCurvePointsToolBar->addTo(viewMenu);
  viewDigitizeMeasurePointsToolBar->addTo(viewMenu);
  viewZoomToolBar->addTo(viewMenu);
  viewStatusBar->addTo(viewMenu);
  viewMenu->insertSeparator();
  viewAxesPoints->addTo(viewMenu);
  viewScalePoints->addTo(viewMenu);
  viewCurvePoints->addTo(viewMenu);
  viewMeasurePoints->addTo(viewMenu);
  viewAllPoints->addTo(viewMenu);
  viewMenu->insertSeparator();
  viewNoBackground->addTo(viewMenu);
  viewOriginalImage->addTo(viewMenu);
  viewProcessedImage->addTo(viewMenu);
  viewMenu->insertSeparator();
  viewGridDisplay->addTo(viewMenu);
  viewMenu->insertSeparator();
  viewCurveGeometry->addTo(viewMenu);
  viewMeasureGeometry->addTo(viewMenu);

  // menuBar entry digitizeMenu
  digitizeMenu = new QMenu(this);
  CHECK_PTR_ENGAUGE(digitizeMenu);
  digitizeSelect->addTo(digitizeMenu);
  digitizeMenu->insertSeparator();
  digitizeAxis->addTo(digitizeMenu);
  digitizeScale->addTo(digitizeMenu);
  digitizeMenu->insertSeparator();
  digitizeCurve->addTo(digitizeMenu);
  digitizeSegment->addTo(digitizeMenu);
  digitizePointMatch->addTo(digitizeMenu);
  digitizeMenu->insertSeparator();
  digitizeMeasure->addTo(digitizeMenu);

  // menuBar entry settingsMenu
  settingsMenu = new QMenu(this);
  CHECK_PTR_ENGAUGE(settingsMenu);
  settingsCoordSys->addTo(settingsMenu);
  settingsMenu->insertSeparator();
  settingsAxes->addTo(settingsMenu);
  settingsScale->addTo(settingsMenu);
  settingsCurves->addTo(settingsMenu);
  settingsSegments->addTo(settingsMenu);
  settingsPointMatch->addTo(settingsMenu);
  settingsMeasures->addTo(settingsMenu);
  settingsMenu->insertSeparator();
  settingsDiscretize->addTo(settingsMenu);
  settingsGridRemoval->addTo(settingsMenu);
  settingsMenu->insertSeparator();
  settingsGridDisplay->addTo(settingsMenu);
  settingsMenu->insertSeparator();
  settingsExportSetup->addTo(settingsMenu);
  settingsMenu->insertSeparator();
  settingsSessions->addTo(settingsMenu);

  // menuBar entry windowMenu
  windowMenu = new QMenu(this);
  CHECK_PTR_ENGAUGE(windowMenu);
  windowMenu->setCheckable(true);
  connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(slotMenuPreactivated()));

  // menuBar entry helpMenu
  helpMenu = new QMenu(this);
  CHECK_PTR_ENGAUGE(helpMenu);
  helpAboutApp->addTo(helpMenu);
  helpMenu->insertSeparator();
  helpUserManual->addTo(helpMenu);
  helpGlossary->addTo(helpMenu);
  helpConverter->addTo(helpMenu);  
  helpLineGraphTutorial->addTo(helpMenu);
  helpPointGraphTutorial->addTo(helpMenu);
  helpMapTutorial->addTo(helpMenu);
  helpMenu->insertSeparator();
  helpMenu->addAction(QWhatsThis::createAction(this));

  menuBar()->insertItem(tr("&File"), fileMenu);
  menuBar()->insertItem(tr("&Edit"), editMenu);
  menuBar()->insertItem(tr("&View"), viewMenu);
  menuBar()->insertItem(tr("&Digitize"), digitizeMenu);
  menuBar()->insertItem(tr("&Settings"), settingsMenu);
  menuBar()->insertItem(tr("&Window"), windowMenu);
  menuBar()->insertItem(tr("&Help"), helpMenu);
}

void DigitMain::initStatusBar()
{
  DefaultSettings& rSettings = DefaultSettings::instance();
  statusBar()->setShown(rSettings.getViewStatusBar());
  
  statusNormal = new QLabel(statusBar());
  CHECK_PTR_ENGAUGE(statusNormal);
  statusBar()->addWidget(statusNormal, 7, true);
  QWhatsThis::add(statusNormal, QString(tr("Messages that describe the current activity")));

  statusPermanent = new QLabel(statusBar());
  CHECK_PTR_ENGAUGE(statusPermanent);
  statusBar()->addWidget(statusPermanent, 7, true);
  QWhatsThis::add(statusPermanent, QString(tr("Messages that describe remaining steps for "
    "defining the graph coordinates.\n\nThe graph coordinates are defined by creating "
    "three axes points or the scale bar")));

  statusCoords = new QLabel(statusBar());
  CHECK_PTR_ENGAUGE(statusCoords);
  statusBar()->addWidget(statusCoords, 1, true);
  QWhatsThis::add(statusCoords, QString(tr("Graph coordinates of the cursor.\n\n"
    "These are not available until the graph coordinates have been defined by creating "
    "three axes points or the scale bar")));

  statusRes = new QLabel(statusBar());
  CHECK_PTR_ENGAUGE(statusRes);
  statusBar()->addWidget(statusRes, 1, true);
  QWhatsThis::add(statusRes, QString(tr("Resolutions of the graph coordinates.\n\n"
    "These are not available until the graph coordinates have been defined by creating "
    "three axes points or the scale bar.\n\nThe resolution of a coordinate is the root-mean-square average "
    "of the errors in the horizontal and vertical directions on the screen")));

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::initToolBars()
{
  DefaultSettings& rSettings = DefaultSettings::instance();
  
  fileToolbar = new QToolBar("File Toolbar",this);
  CHECK_PTR_ENGAUGE(fileToolbar);
  fileImport->addTo(fileToolbar);
  fileOpen->addTo(fileToolbar);
  fileSave->addTo(fileToolbar);
  fileExport->addTo(fileToolbar);
  fileToolbar->setShown(rSettings.getViewFileToolbar());
	addToolBar(fileToolbar);

  selectToolbar = new QToolBar("Select Toolbar", this);
  CHECK_PTR_ENGAUGE(selectToolbar);
  selectToolbar->addWidget(QWhatsThis::whatsThisButton(selectToolbar));
  digitizeSelect->addTo(selectToolbar);
  selectToolbar->setShown(rSettings.getViewSelectToolbar());
	addToolBar(selectToolbar);
    
  imageScaleToolbar = new QToolBar("Image Scale Toolbar", this);
  CHECK_PTR_ENGAUGE(imageScaleToolbar);  
  digitizeAxis->addTo(imageScaleToolbar);
  digitizeScale->addTo(imageScaleToolbar);
  imageScaleToolbar->setShown(rSettings.getViewImageScaleToolbar());
	addToolBar(imageScaleToolbar);
  
  digitizeCurvePointsToolbar = new QToolBar("Digitize Curve Toolbar", this);
  CHECK_PTR_ENGAUGE(digitizeCurvePointsToolbar);
  digitizeCurve->addTo(digitizeCurvePointsToolbar);
  digitizeSegment->addTo(digitizeCurvePointsToolbar);
  digitizePointMatch->addTo(digitizeCurvePointsToolbar);
  curveCmb = new CurveCmb(digitizeCurvePointsToolbar);
  digitizeCurvePointsToolbar->addWidget(curveCmb);
	CHECK_PTR_ENGAUGE(curveCmb);
  QToolTip::add(curveCmb, QString(tr("Curve List")));
  digitizeCurvePointsToolbar->setShown(rSettings.getViewDigitizeCurvePointsToolbar());
	addToolBar(digitizeCurvePointsToolbar);
  
  digitizeMeasurePointsToolbar = new QToolBar("Digitize Measure Points Toolbar", this);
  CHECK_PTR_ENGAUGE(digitizeMeasurePointsToolbar);
  digitizeMeasure->addTo(digitizeMeasurePointsToolbar);
  measureCmb = new MeasureCmb(digitizeMeasurePointsToolbar);
  digitizeMeasurePointsToolbar->addWidget(measureCmb);
	CHECK_PTR_ENGAUGE(measureCmb);
  QToolTip::add(measureCmb, QString(tr("Measure List")));
  digitizeMeasurePointsToolbar->setShown(rSettings.getViewDigitizeMeasurePointsToolbar());
	addToolBar(digitizeMeasurePointsToolbar);
  
  // clicking on toolbar pushbuttons causes state change in curve combobox
  connect(digitizeSelect, SIGNAL(toggled(bool)), curveCmb, SLOT(slotComboNotCurve(bool)));
  connect(digitizeAxis, SIGNAL(toggled(bool)), curveCmb, SLOT(slotComboNotCurve(bool)));
  connect(digitizeScale, SIGNAL(toggled(bool)), curveCmb, SLOT(slotComboNotCurve(bool)));
  connect(digitizeCurve, SIGNAL(toggled(bool)), curveCmb, SLOT(slotComboCurve(bool)));
  connect(digitizeSegment, SIGNAL(toggled(bool)), curveCmb, SLOT(slotComboCurve(bool)));
  connect(digitizePointMatch, SIGNAL(toggled(bool)), curveCmb, SLOT(slotComboCurve(bool)));
  connect(digitizeMeasure, SIGNAL(toggled(bool)), curveCmb, SLOT(slotComboNotCurve(bool)));

  // clicking on toolbar pushbuttons causes state change in measure combobox
  connect(digitizeSelect, SIGNAL(toggled(bool)), measureCmb, SLOT(slotComboNotMeasure(bool)));
  connect(digitizeAxis, SIGNAL(toggled(bool)), measureCmb, SLOT(slotComboNotMeasure(bool)));
  connect(digitizeScale, SIGNAL(toggled(bool)), measureCmb, SLOT(slotComboNotMeasure(bool)));
  connect(digitizeCurve, SIGNAL(toggled(bool)), measureCmb, SLOT(slotComboNotMeasure(bool)));
  connect(digitizeSegment, SIGNAL(toggled(bool)), measureCmb, SLOT(slotComboNotMeasure(bool)));
  connect(digitizePointMatch, SIGNAL(toggled(bool)), measureCmb, SLOT(slotComboNotMeasure(bool)));
  connect(digitizeMeasure, SIGNAL(toggled(bool)), measureCmb, SLOT(slotComboMeasure(bool)));

  // clicking on curvecmb combobox causes state change in toolbar pushbuttons
  connect(curveCmb, SIGNAL(activated(int)), this, SLOT(slotCurveComboActivated(int)));

  // clicking on measurecmb combobox causes state change in toolbar pushbuttons
  connect(measureCmb, SIGNAL(activated(int)), this, SLOT(slotMeasureComboActivated(int)));

  zoomToolbar = new QToolBar("Zoom Toolbar", this);
  CHECK_PTR_ENGAUGE(zoomToolbar);
  zoomCmb = new ZoomCmb(zoomToolbar);
	zoomToolbar->addWidget(zoomCmb);
  CHECK_PTR_ENGAUGE(zoomCmb);
  QToolTip::add(zoomCmb, QString(tr("Zoom")));
  zoomToolbar->setShown(rSettings.getViewZoomToolbar());
	addToolBar(zoomToolbar);
}

void DigitMain::initView()
{
	// set the main widget here
	workspace = new QMdiArea();
	workspace->setViewMode(QMdiArea::SubWindowView);
	setCentralWidget(workspace);
}

DigitDoc* DigitMain::newDocument()
{
  DigitDoc* doc;

  if (activeDocument()) {
    // this document is not the first so start up in same state as previous document for continuity
    doc = new DigitDoc(activeDocument()->digitizeState());
    CHECK_PTR_ENGAUGE(doc);
  }
  else
  {
    doc = new DigitDoc(DefaultSettings::instance().getSessionsSettings().initialDigitizeState);
    CHECK_PTR_ENGAUGE(doc);
  }

  ASSERT_ENGAUGE(docList != 0);
  docList->append(doc);

  connect(doc, SIGNAL(normalMsg(const QString&)), this, SLOT(slotStatusNormalMsg(const QString&)));
  connect(doc, SIGNAL(permanentMsg(const QString&)), this, SLOT(slotStatusPermanentMsg(const QString&)));
  connect(doc, SIGNAL(temporaryMsg(const QString&)), this, SLOT(slotStatusTemporaryMsg(const QString&)));
  connect(doc, SIGNAL(trackingMsg(const QString&, const QString&)), this,
    SLOT(slotStatusTrackingMsg(const QString&, const QString&)));
  connect(doc, SIGNAL(selectionChange()), this, SLOT(slotSelectionChange()));
  connect(doc, SIGNAL(transformationChange()), this, SLOT(slotTransformationChange()));
  connect(doc, SIGNAL(updateGeometry()), this, SLOT(slotUpdateGeometry()));

  QString text(tr("Three axis points or the scale bar must be defined."));
  doc->forwardPermanentMsg(text);

  return doc;
}

void DigitMain::createClient(DigitDoc* doc)
{
  DigitView* view = new DigitView (doc, workspace, 0, Qt::WDestructiveClose);
  workspace->addSubWindow(view);
	CHECK_PTR_ENGAUGE(view);

  view->installEventFilter (this);
  ASSERT_ENGAUGE(doc != 0);
  doc->addView(view);
  if (workspace->subWindowList().isEmpty()) // show the very first window in maximized mode
    view->showMaximized();
  else
    view->show();

  setActiveView(view);
}

void DigitMain::importImageFile(const QString file)
{
  slotStatusNormalMsg(QString(tr("Importing file...")));
  QApplication::setOverrideCursor(Qt::WaitCursor);

  DigitDoc* doc = newDocument();

  // open the file
  ASSERT_ENGAUGE(doc != 0);
  if (!doc->importFile(file))
  {
    QApplication::restoreOverrideCursor();

    if (cmdOnlyBmp)
      QMessageBox::critical(this, tr("Error!"), tr("Could not open BMP image!"));
    else
      QMessageBox::critical(this, tr("Error!"), tr("Could not open image!"));
    removeDocument(doc);

    return;
  }

  doc->initDefaults();
  resetActions();

  // create the window
  createClient(doc);

  toggleActions();

  slotStatusNormalMsg(QString(tr("Ready.")));
  QApplication::restoreOverrideCursor();
}

void DigitMain::importImageFile(const QString file,
  double xMinG, double xMaxG,
  double yMinG, double yMaxG)
{
  importImageFile(file);

  if (activeDocument())
  {
    slotStatusNormalMsg(QString(tr("Scanning for axes...")));
    QApplication::setOverrideCursor(Qt::WaitCursor);

    activeDocument()->scanForAxes(xMinG, xMaxG, yMinG, yMaxG);

    slotStatusNormalMsg(QString(tr("Ready.")));
    QApplication::restoreOverrideCursor();
  }
}

void DigitMain::importImageDragged(const QImage& image)
{
  slotStatusNormalMsg(QString(tr("Importing image...")));
  QApplication::setOverrideCursor(Qt::WaitCursor);

  DigitDoc* doc = newDocument();

  // save the image
  ASSERT_ENGAUGE(doc != 0);
  if (!doc->importImage(image))
  {
    QApplication::restoreOverrideCursor();

    QMessageBox::critical(this, tr("Error!"), tr("Could not load image!"));

    removeDocument(doc);

    return;
  }

  doc->initDefaults();
  resetActions();

  // create the window
  createClient(doc);

  toggleActions();

  slotStatusNormalMsg(QString(tr("Ready.")));
  QApplication::restoreOverrideCursor();
}

void DigitMain::importPixmapDragged(const QPixmap& pixmap)
{
  slotStatusNormalMsg(QString(tr("Importing image...")));
  QApplication::setOverrideCursor(Qt::WaitCursor);

  DigitDoc* doc = newDocument();

  // save the image
  ASSERT_ENGAUGE(doc != 0);
  if (!doc->importPixmap(pixmap))
  {
    QApplication::restoreOverrideCursor();

    QMessageBox::critical(this, tr("Error!"), tr("Could not load image!"));

    removeDocument(doc);

    return;
  }

  doc->initDefaults();
  resetActions();

  // create the window
  createClient(doc);

  toggleActions();

  slotStatusNormalMsg(QString(tr("Ready.")));
  QApplication::restoreOverrideCursor();
}

void DigitMain::openDocumentFile(const QString file)
{
  // do not set override cursor externally!

  slotStatusNormalMsg(QString(tr("Opening file...")));
  QApplication::setOverrideCursor(Qt::WaitCursor);

  // check, if document already open. If yes, set the focus to the first view
  DigitDoc* doc;
  ASSERT_ENGAUGE(docList != 0);
  for (doc = docList->first(); doc; doc = docList->next())
  {
    ASSERT_ENGAUGE(doc != 0);
    if (doc->savePath() == file)
    {
      focusOnViewOrDoc(0, doc);

      QApplication::restoreOverrideCursor();
      return;
    }
  }
  doc = newDocument();

  // open the file
  if (!doc->openDocument(file))
  {
    QApplication::restoreOverrideCursor();

    QMessageBox::critical(this, tr("Error!"),tr("Could not open document!"));
    removeDocument(doc);

    return;
  }

  resetActions();

  // create the window
  createClient(doc);

  toggleActions();

  slotStatusNormalMsg(QString(tr("Ready.")));
  QApplication::restoreOverrideCursor();
}

void DigitMain::setCurveCmb(DigitDoc* doc)
{
  // save new pointset set into combobox
  ASSERT_ENGAUGE(curveCmb != 0);
  curveCmb->attachToDocument(doc);

  if (doc)
  {
    // synchronize document and combobox
    curveCmb->setCurrentText(doc->curveCmbText());
  }
}

void DigitMain::setMeasureCmb(DigitDoc* doc)
{
  // save new pointset set into combobox
  ASSERT_ENGAUGE(measureCmb != 0);
  measureCmb->attachToDocument(doc);

  if (doc)
  {
    // synchronize document and combobox
    measureCmb->setCurrentText(doc->measureCmbText());
  }
}

void DigitMain::resetActions()
{
  ASSERT_ENGAUGE(digitizeAxis != 0);
  digitizeAxis->setOn(false);
  ASSERT_ENGAUGE(digitizeCurve != 0);
  digitizeCurve->setOn(false);
  ASSERT_ENGAUGE(digitizePointMatch != 0);
  digitizePointMatch->setOn(false);
  ASSERT_ENGAUGE(digitizeScale != 0);
  digitizeScale->setOn(false);
  ASSERT_ENGAUGE(digitizeSegment != 0);
  digitizeSegment->setOn(false);
  ASSERT_ENGAUGE(digitizeSelect != 0);
  digitizeSelect->setOn(false);
  ASSERT_ENGAUGE(digitizeMeasure != 0);
  digitizeMeasure->setOn(false);
}

bool DigitMain::queryExit()
{
  int exit = QMessageBox::information(this, tr("Quit..."),
    tr("You have unsaved changes. Do your really want to quit?"),
    QMessageBox::Ok, QMessageBox::Cancel);

  return (exit == QMessageBox::Ok);
}

void DigitMain::toggleActions()
{
  // perform updates that are independent of the document
  ASSERT_ENGAUGE(fileOpenRecent != 0);
  fileOpenRecent->setEnabled(!m_mruDocuments.isEmpty());

  // state change just occurred so update the menubar and toolbars correspondingly
  DigitDoc* doc = activeDocument();
  if (doc)
  {
    CoordSettings coord = doc->coordSettings();

    ASSERT_ENGAUGE(fileClose != 0);
    fileClose->setEnabled(true);
    ASSERT_ENGAUGE(fileSave != 0);
    fileSave->setEnabled(true);
    ASSERT_ENGAUGE(fileSaveAs != 0);
    fileSaveAs->setEnabled(true);
    ASSERT_ENGAUGE(fileExport != 0);
    fileExport->setEnabled(true);
    ASSERT_ENGAUGE(fileExportAs != 0);
    fileExportAs->setEnabled(true);
    ASSERT_ENGAUGE(filePrint != 0);
    filePrint->setEnabled(true);

    toggleActionsEdit();

    ASSERT_ENGAUGE(viewAxesPoints != 0);
    ASSERT_ENGAUGE(viewScalePoints != 0);
    ASSERT_ENGAUGE(viewCurvePoints != 0);
    ASSERT_ENGAUGE(viewMeasurePoints != 0);
    ASSERT_ENGAUGE(viewAllPoints != 0);
    switch (doc->viewPointSelection())
    {
      // setOn(false) causes unwanted toggling with action groups, so use switch statement!
      case ViewAxesPoints:
        viewAxesPoints->setOn(true);
        break;
      case ViewScalePoints:
        viewScalePoints->setOn(true);
        break;
      case ViewCurvePoints:
        viewCurvePoints->setOn(true);
        break;
      case ViewMeasurePoints:
        viewMeasurePoints->setOn(true);
        break;
      case ViewAllPoints:
        viewAllPoints->setOn(true);
        break;
    }
    viewAxesPoints->setEnabled(true);
    viewScalePoints->setEnabled(true);
    viewCurvePoints->setEnabled(true);
    viewMeasurePoints->setEnabled(true);
    viewAllPoints->setEnabled(true);
    ASSERT_ENGAUGE(viewNoBackground != 0);
    ASSERT_ENGAUGE(viewOriginalImage != 0);
    ASSERT_ENGAUGE(viewProcessedImage != 0);
    switch (doc->background())
    {
      // setOn(false) causes unwanted toggling with action groups, so use switch statement!
      case BlankBackground:
        viewNoBackground->setOn(true);
        break;
      case OriginalImage:
        viewOriginalImage->setOn(true);
        break;
      case ProcessedImage:
        viewProcessedImage->setOn(true);
        break;  
    }
    viewNoBackground->setEnabled(true);
    viewOriginalImage->setEnabled(true);
    viewProcessedImage->setEnabled(true);
    ASSERT_ENGAUGE(viewGridDisplay != 0);
    viewGridDisplay->setOn(doc->gridDisplay());
    viewGridDisplay->setEnabled(doc->validAxes());

    ASSERT_ENGAUGE(digitizeAction != 0);
    digitizeAction->setEnabled(true);
    ASSERT_ENGAUGE(digitizeAxis != 0);
    digitizeAxis->setEnabled((doc->axisPointCount() < 3) && // prevent too many axis points
      (doc->scalePointCount() == 0)); // prevent mixture of axis and scale points
    ASSERT_ENGAUGE(digitizeCurve != 0);
    digitizeCurve->setEnabled(true);
    ASSERT_ENGAUGE(digitizeScale != 0);
    digitizeScale->setEnabled(
      (coord.frame == Cartesian) &&
      (coord.xThetaScale == Linear) &&
      (coord.yRScale == Linear) && // incompatible with polar or log coordinates
      (doc->axisPointCount() == 0) && // prevent mixture of axis and scale points
      (doc->scalePointCount() < 2)); // prevent too many scale points
    ASSERT_ENGAUGE(digitizeSegment != 0);
    digitizeSegment->setEnabled(true);
    ASSERT_ENGAUGE(digitizePointMatch != 0);
    digitizePointMatch->setEnabled(true);
    ASSERT_ENGAUGE(digitizeSelect != 0);
    digitizeSelect->setEnabled(true);
    ASSERT_ENGAUGE(digitizeMeasure != 0);
    digitizeMeasure->setEnabled(true);

    ASSERT_ENGAUGE(settingsCoordSys != 0);
    settingsCoordSys->setEnabled(true);
    ASSERT_ENGAUGE(settingsAxes != 0);
    settingsAxes->setEnabled(true);
    ASSERT_ENGAUGE(settingsScale != 0);
    settingsScale->setEnabled(true);
    ASSERT_ENGAUGE(settingsCurves != 0);
    settingsCurves->setEnabled(true);
    ASSERT_ENGAUGE(settingsSegments != 0);
    settingsSegments->setEnabled(true);
    ASSERT_ENGAUGE(settingsPointMatch != 0);
    settingsPointMatch->setEnabled(true);
    ASSERT_ENGAUGE(settingsMeasures != 0);
    settingsMeasures->setEnabled(true);
    ASSERT_ENGAUGE(settingsDiscretize != 0);
    settingsDiscretize->setEnabled(true);
    ASSERT_ENGAUGE(settingsGridRemoval != 0);
    settingsGridRemoval->setEnabled(true);
    ASSERT_ENGAUGE(settingsGridDisplay != 0);
    settingsGridDisplay->setEnabled(doc->validAxes());
    ASSERT_ENGAUGE(settingsExportSetup != 0);
    settingsExportSetup->setEnabled(true);

    setCurveCmb(doc);
    setMeasureCmb(doc);
    ASSERT_ENGAUGE(zoomCmb != 0);
    // zooming and point match state are incompatible, so zoom is disabled in that state,
    // because changing zoom immediately sends focus to zoom control so that all succeeding
    // mouse button and key press events are processed by the zoom control rather than 
    // PointMatchTool
    zoomCmb->setEnabled(doc->digitizeState() != PointMatchState);
    ASSERT_ENGAUGE(activeView() != 0);
    zoomCmb->setZoom(activeView()->zoom());

    switch (doc->digitizeState())
    {
      // setOn(false) causes unwanted toggling with action groups, so use switch statement!
      case AxisState:
        ASSERT_ENGAUGE(digitizeAxis != 0);
        digitizeAxis->setOn(true);
        break;
      case CurveState:
        ASSERT_ENGAUGE(digitizeCurve != 0);
        digitizeCurve->setOn(true);
        break;
      case MeasureState:
        ASSERT_ENGAUGE(digitizeMeasure != 0);
        digitizeMeasure->setOn(true);
        break;
      case PointMatchState:
        ASSERT_ENGAUGE(digitizePointMatch != 0);
        digitizePointMatch->setOn(true);
        break;
      case ScaleState:
        ASSERT_ENGAUGE(digitizeScale != 0);
        digitizeScale->setOn(true);
        break;
      case SegmentState:
        ASSERT_ENGAUGE(digitizeSegment != 0);
        digitizeSegment->setOn(true);
        break;
      case SelectState:
        ASSERT_ENGAUGE(digitizeSelect != 0);
        digitizeSelect->setOn(true);
        break;
    }
  }
  else
  {
    ASSERT_ENGAUGE(fileClose != 0);
    fileClose->setEnabled(false);
    ASSERT_ENGAUGE(fileSave != 0);
    fileSave->setEnabled(false);
    ASSERT_ENGAUGE(fileSaveAs != 0);
    fileSaveAs->setEnabled(false);
    ASSERT_ENGAUGE(fileExport != 0);
    fileExport->setEnabled(false);
    ASSERT_ENGAUGE(fileExportAs != 0);
    fileExportAs->setEnabled(false);
    ASSERT_ENGAUGE(filePrint != 0);
    filePrint->setEnabled(false);

    toggleActionsEdit();

    ASSERT_ENGAUGE(viewAxesPoints != 0);
    viewAxesPoints->setEnabled(false);
    ASSERT_ENGAUGE(viewScalePoints != 0);
    viewScalePoints->setEnabled(false);
    ASSERT_ENGAUGE(viewCurvePoints != 0);
    viewCurvePoints->setEnabled(false);
    ASSERT_ENGAUGE(viewMeasurePoints != 0);
    viewMeasurePoints->setEnabled(false);
    ASSERT_ENGAUGE(viewAllPoints != 0);
    viewAllPoints->setEnabled(false);
    ASSERT_ENGAUGE(viewNoBackground != 0);
    viewNoBackground->setEnabled(false);
    ASSERT_ENGAUGE(viewOriginalImage != 0);
    viewOriginalImage->setEnabled(false);
    ASSERT_ENGAUGE(viewProcessedImage != 0);
    viewProcessedImage->setEnabled(false);
    ASSERT_ENGAUGE(viewGridDisplay != 0);
    viewGridDisplay->setEnabled(false);

    ASSERT_ENGAUGE(digitizeAction != 0);
    digitizeAction->setEnabled(false);

    ASSERT_ENGAUGE(settingsCoordSys != 0);
    settingsCoordSys->setEnabled(false);
    ASSERT_ENGAUGE(settingsAxes != 0);
    settingsAxes->setEnabled(false);
    ASSERT_ENGAUGE(settingsScale != 0);
    settingsScale->setEnabled(false);
    ASSERT_ENGAUGE(settingsCurves != 0);
    settingsCurves->setEnabled(false);
    ASSERT_ENGAUGE(settingsSegments != 0);
    settingsSegments->setEnabled(false);
    ASSERT_ENGAUGE(settingsPointMatch != 0);
    settingsPointMatch->setEnabled(false);
    ASSERT_ENGAUGE(settingsMeasures != 0);
    settingsMeasures->setEnabled(false);
    ASSERT_ENGAUGE(settingsDiscretize != 0);
    settingsDiscretize->setEnabled(false);
    ASSERT_ENGAUGE(settingsGridRemoval != 0);
    settingsGridRemoval->setEnabled(false);
    ASSERT_ENGAUGE(settingsGridDisplay != 0);
    settingsGridDisplay->setEnabled(false);
    ASSERT_ENGAUGE(settingsExportSetup != 0);
    settingsExportSetup->setEnabled(false);

    setCurveCmb(0);
    setMeasureCmb(0);
    ASSERT_ENGAUGE(zoomCmb != 0);
    zoomCmb->setEnabled(false);
  }
}

void DigitMain::toggleActionsEdit()
{
  DigitView* view = activeView();

  if (view)
  {
    ASSERT_ENGAUGE(editUndo != 0);
    editUndo->setEnabled(view->document()->isActionToUndo());
    ASSERT_ENGAUGE(editRedo != 0);
    editRedo->setEnabled(view->document()->isActionToRedo());
    ASSERT_ENGAUGE(editCut != 0);
    editCut->setEnabled(view->isSelection());
    ASSERT_ENGAUGE(editCopy != 0);
    editCopy->setEnabled(view->isSelection());
    ASSERT_ENGAUGE(editPaste != 0);
    ASSERT_ENGAUGE(view != 0);
    ASSERT_ENGAUGE(view->document() != 0);
    bool creationState =
      (view->document()->digitizeState() == CurveState) ||
      (view->document()->digitizeState() == MeasureState);
    editPaste->setEnabled(Clipboard::instance().clipboardItems() && creationState);
  }
  else
  {
    ASSERT_ENGAUGE(editUndo != 0);
    editUndo->setEnabled(false);
    ASSERT_ENGAUGE(editRedo != 0);
    editRedo->setEnabled(false);
    ASSERT_ENGAUGE(editCut != 0);
    editCut->setEnabled(false);
    ASSERT_ENGAUGE(editCopy != 0);
    editCopy->setEnabled(false);
    ASSERT_ENGAUGE(editPaste != 0);
    editPaste->setEnabled(false);
  }

  toggleActionPasteAsNew();
}

void DigitMain::toggleActionPasteAsNew()
{
  // accessing the clipboard below crashes in linux with qt3-gcc3.2, prompting the upgrade to qt-3.3.3
  editPasteAsNew->setEnabled(
    clipboardContainsUris() ||
    !QApplication::clipboard()->image().isNull() ||
    !QApplication::clipboard()->pixmap().isNull());
}

bool DigitMain::eventFilter(QObject* object, QEvent* event)
{
  ASSERT_ENGAUGE(event != 0);
  switch (event->type())
  {
    case QEvent::Close:
      if ((DigitMain*) object == this)
      {
        // close button in titlebar was clicked
        if (!safeToQuit())
          return true; // true filters the event out according to QObject documentation
      }
      else
      {
        // close button in view was clicked
        bool rtn = eventClose(object, event);
        toggleActions();
        return rtn;
      }
      break; // this break was missing for quite a while!
    case QEvent::FocusIn:
      if (((DigitMain*) object != this) &&
        ((QMenuBar*) object != menuBar()))
      {
        DigitView* view = (DigitView*) object;
        if (findView(view))
        {
          DigitDoc* doc = view->document();
          ASSERT_ENGAUGE(doc != 0);

          setActiveView(view);
          toggleActions();
          slotUpdateGeometry();

          QString text(tr("The three axis points are correctly defined."));
          ASSERT_ENGAUGE(doc != 0);
          if (!doc->validAxes())
          {
            if (doc->validScale())
              text = QString(tr("The scale bar is correctly defined."));
            else
              text = QString(tr("Three axis points, or the scale bar, must be defined."));
          }
          doc->forwardPermanentMsg(text);
        }
      }
      break;
    case QEvent::KeyPress:
      if (activeView())
      {
        QKeyEvent* keyEvent = (QKeyEvent*) event;
        if ((keyEvent->key() == Qt::Key_Delete) && (activeView()->isSelection()))
        {
          // convert pressing delete key into cut
          slotEditCut();
          return true;
        }
      }
      return eventKeyPress(object, event);
    default:
      break;
  }

  return QWidget::eventFilter (object, event); // standard event processing
}

bool DigitMain::eventClose(QObject* object, QEvent* event)
{
  DigitView* view = (DigitView*) object;
  if (findView(view))
  {
    DigitDoc* doc = view->document();
    ASSERT_ENGAUGE(doc != 0);

    QCloseEvent* e = (QCloseEvent*) event;
    if (canCloseFrame(doc, view))
    {
      doc->removeView(view);
      if (!doc->firstView())
      {
        setActiveView(0);
        removeDocument(doc);
      }
    }

    e->ignore();
    return true;
  }

  return false;
}

bool DigitMain::eventKeyPress(QObject* object, QEvent* event)
{
  // object usually, but not always, contains view at this point
  DigitView* view = (DigitView*) object;
  if (findView(view))
  {
    DigitDoc* doc = view->document();

    QKeyEvent* e = (QKeyEvent*) event;

    if (e->key() == DefaultSettings::instance().getKeyClipboard())
    {
      Clipboard::instance().dump();

      e->ignore();
      return true;
    }
    else if (e->key() == DefaultSettings::instance().getKeyTree())
    {
      ASSERT_ENGAUGE(qApp != 0);
      qApp->dumpObjectTree();

      e->ignore();
      return true;
    }
    else if (e->key() == DefaultSettings::instance().getKeyVariables())
    {
      ASSERT_ENGAUGE(doc != 0);
      doc->dump();

      e->ignore();
      return true;
    }
  }

  return false;
}

void DigitMain::moveEvent(QMoveEvent* pEvent)
{
  if (pEvent)
    DefaultSettings::instance().setWindowMainPosition(pEvent->pos());
}

void DigitMain::resizeEvent(QResizeEvent* pEvent)
{
  if (pEvent)
    DefaultSettings::instance().setWindowMainSize(pEvent->size());
}

void DigitMain::dragEnterEvent(QDragEnterEvent* pEvent)
{
  if (Q3UriDrag::canDecode(pEvent))
    pEvent->accept();
  else if (Q3ImageDrag::canDecode(pEvent))
    pEvent->accept();
}

void DigitMain::dropEvent(QDropEvent* pEvent)
{
  Q3StrList uriList;
  QImage image;
  if (Q3UriDrag::decode(pEvent, uriList))
  {
    char* uri;
    for (uri = uriList.first(); uri; uri = uriList.next())
      importImageFile(Q3UriDrag::uriToLocalFile(uri));
  }
  else if (Q3ImageDrag::decode(pEvent, image))
    importImageDragged(image);
}

DigitDoc* DigitMain::activeDocument()
{
  DigitView* view = activeView();
  if (view)
    return view->document();
  else
    return 0;
}

DigitView* DigitMain::activeView()
{
  // the active view is tracked by DigitMain, independent of the
  // active view that QWorkspace tracks, since DigitMain updates
  // its active view as soon as a focus in event occurs. QWorkspace
  // seems, at least under Linux, to post the focus in event into
  // a queue so the value returned by workspace->activeWindow()
  // is stale for quite a while after a new window focus in event
  return m_activeView;
}

bool DigitMain::findView(DigitView* view)
{
  DigitDoc* doc;
  for (doc = docList->first(); doc; doc = docList->next())
  {
    ASSERT_ENGAUGE(doc != 0);
    if (doc->findView(view))
      return true;
  }

  return false;
}

void DigitMain::focusOnViewOrDoc(DigitView* view, DigitDoc* doc)
{
  if (view != 0)
  {
    view->setFocus();
  }
  else
  {
    // at least one of the arguments must be nonzero
    ASSERT_ENGAUGE(doc != 0);
    ASSERT_ENGAUGE(doc->firstView() != 0);
    doc->firstView()->setFocus();
  }
}

void DigitMain::setActiveView(DigitView* view)
{
  m_activeView = view;

  disconnect(zoomCmb, 0, 0, 0);
  if (view)
    connect(zoomCmb, SIGNAL(activated(const QString &)), view, SLOT(slotZoomChange(const QString &)));
}

void DigitMain::setDigitizeState(DigitizeState state)
{
  // update all documents with next state
  for (DigitDoc* doc = docList->first(); doc; doc = docList->next())
  {
    ASSERT_ENGAUGE(doc != 0);
    doc->setDigitizeState(state);

    ASSERT_ENGAUGE(curveCmb != 0);
    if ((doc == activeDocument()) && !curveCmb->currentText().isEmpty())
      doc->setCurveCmbText(curveCmb->currentText());

    ASSERT_ENGAUGE(measureCmb != 0);
    if ((doc == activeDocument()) && !measureCmb->currentText().isEmpty())
      doc->setMeasureCmbText(measureCmb->currentText());
  }
}

void DigitMain::displayCurveGeometry(GeometryWindowItemList& rInfo)
{
  ASSERT_ENGAUGE(curveGeometryDlg != 0);
  curveGeometryDlg->displayGeometryInfo(rInfo);
}

void DigitMain::displayMeasureGeometry(GeometryWindowItemList& rInfo)
{
  ASSERT_ENGAUGE(measureGeometryDlg != 0);
  measureGeometryDlg->displayGeometryInfo(rInfo);
}

void DigitMain::noGeometryInfo()
{
  GeometryWindowItemList emptyInfo;
  emptyInfo.append(GeometryWindowItem(0, 0, tr("No geometry information yet")));

  displayCurveGeometry(emptyInfo);
  displayMeasureGeometry(emptyInfo);
}

void DigitMain::removeDocument(DigitDoc* doc)
{
  if (doc)
  {
    ASSERT_ENGAUGE(docList != 0);
    docList->remove(doc); // autoDelete is on

    if (docList->count() < 1)
      setActiveView(0);

    toggleActions();

    slotUpdateGeometry();
  }
}

QDir DigitMain::helpPath()
{
  // user-specified locations of user manual directory come from command line or environment variable
  QString envValue(getenv(UserManualEnvironmentVariable));
  QDir helpPath0(envValue);

  // possible locations of user manual directory
#ifndef DEBIAN_PACKAGE
  const QDir helpPath1("usermanual");
  const QDir helpPath2("../usermanual");
  const QDir helpPath3("../../usermanual");
#else /* DEBIAN_PACKAGE */
  const QDir helpPath1("/usr/share/doc/engauge-digitizer-doc/html");
#endif /* DEBIAN_PACKAGE */

  // command line value can be easily changed and is controllable, so it is used first. environment
  // variable is also user-specified but it can be overridden by the command line
  QDir helpPath;
  if (cmdManualDirectory.exists())
    helpPath = cmdManualDirectory;
  else if (!envValue.isEmpty() && helpPath0.exists())
    helpPath = helpPath0;
  else if (helpPath1.exists())
    helpPath = helpPath1;
#ifndef DEBIAN_PACKAGE
  else if (helpPath2.exists())
    helpPath = helpPath2;
  else if (helpPath3.exists())
    helpPath = helpPath3;
#endif /* ! DEBIAN_PACKAGE */
#ifdef Q_OS_MACX
  else if (userManualPath().exists())
    helpPath = userManualPath();
#endif

  else
  {
    QMessageBox::critical(this, tr("Error!"), tr("Could not open user manual!\n\n"
      "This problem may be fixed by making any one of the following changes and then restarting:\n"
#ifndef DEBIAN_PACKAGE
      "1. Run Engauge in its original directory, which contains the 'usermanual' subdirectory\n"
      "2. adding '-manual <path>' to the command line with <path> set to the 'usermanual' subdirectory location\n"
#else /* DEBIAN_PACKAGE */
      "1. installing engauge-digitizer-doc (apt-get install engauge-digitizer-doc)\n"
      "2. adding '-manual <path>' to the command line with <path> set a 'usermanual' directory location\n"
#endif /* DEBIAN_PACKAGE */
      "3. setting the %1 environment variable to the 'usermanual' subdirectory location")
        .arg(UserManualEnvironmentVariable));
  }

  return helpPath;
}

void DigitMain::spawnHelp(QString helpHome)
{
  QDir path = helpPath();
   
  HelpWindow *help = new HelpWindow(helpHome, path.path(), 0, "help viewer");
  CHECK_PTR_ENGAUGE(help);
  help->setCaption(QString("%1%2")
    .arg(tr("Engauge Digitizer Help"))
    .arg(freewareCaption));
  if ((QApplication::desktop()->width() > 400) &&
    (QApplication::desktop()->height() > 500))
    help->show();
  else
    help->showMaximized();
}

void DigitMain::replacePointSets(DigitDoc* doc, PointSets* copyPointSets,
  QStringList &pointSetsWithOrphanedPoints, bool replacingCurves)
{
  ASSERT_ENGAUGE(doc != 0);

  if (replacingCurves)
  {
    ASSERT_ENGAUGE(curveCmb != 0);
    QString oldSelection = curveCmb->currentText();

    // save new pointset set into document
    doc->replacePointSets(copyPointSets, &pointSetsWithOrphanedPoints, 0);

    // update curveCmb
    setCurveCmb(doc);

    // set style according to selected curve in curveCmb
    doc->setMatchSetStyle(doc->curveStyle(curveCmb->currentText()));

    if (oldSelection != doc->curveCmbText())
      QMessageBox::warning(this, doc->title(), tr("Selected curve was changed from " + oldSelection +
      " to " + doc->curveCmbText()), QMessageBox::Ok, 0);
  }
  else
  {
    ASSERT_ENGAUGE(measureCmb != 0);
    QString oldSelection = measureCmb->currentText();

    // save new pointset set into document
    doc->replacePointSets(copyPointSets, 0, &pointSetsWithOrphanedPoints);

    // update measureCmb
    setMeasureCmb(doc);

    if (oldSelection != doc->measureCmbText())
      QMessageBox::warning(this, doc->title(), tr("Selected measure was changed from " + oldSelection +
        " to " + doc->measureCmbText()), QMessageBox::Ok, 0);
  }
}

bool DigitMain::safeToQuit()
{
  // ask user for guidance for each document with unsaved changes, but skip
  // active document if user requested automatic file export at shutdown
  DigitDoc* doc;
  for (doc = docList->first(); doc; doc = docList->next())
  {
    if (m_exportFileAtShutdown.isEmpty() || (doc != activeDocument()))
    {
      ASSERT_ENGAUGE(doc != 0);
      if (doc->modified())
      {
        if (!queryExit()) // user is canceling quit
          return false;
        break;
      }
    }
  }

  if (!m_exportFileAtShutdown.isEmpty())
  {
    // user has requested automatic file export of active document at shutdown
    doc = activeDocument();
    ASSERT_ENGAUGE(doc != 0);
    doc->markFileAsExported(); // prevent save file dialog from appearing in slotFileExport
    doc->setExportPath(m_exportFileAtShutdown);
    slotFileExport(); // export active document to export path from previous step
  }

  // last chance to save settings before quitting
  DefaultSettings::instance().archiveSettings();

  return true;
}

bool DigitMain::canCloseFrame(DigitDoc* doc, DigitView* view)
{
  if (!doc->isLastView())
    return true;

  bool ret = false;
  ASSERT_ENGAUGE(doc != 0);
  if (doc->modified())
  {
    QString saveName;
    switch(QMessageBox::information(view, doc->title(), tr("The current file has been modified.\n\n"
      "Do you want to save it?"), QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel))
    {
      case QMessageBox::Yes:
        if (documentSave(doc))
        {
          // save was canceled
          switch(QMessageBox::critical(view, tr("I/O Error!"), tr("Could not save the current document!\n\n"
            "Close anyway?"), QMessageBox::Yes, QMessageBox::No))
          {
            case QMessageBox::Yes:
              ret = true;
              break;
            case QMessageBox::No:
              ret = false;
              break;
          }
        }
        else
          ret = true;
        break;
      case QMessageBox::No:
        ret = true;
        break;
      case QMessageBox::Cancel:
      default:
        ret = false;
        break;
    }
  }
  else
    ret = true;

  return ret;
}

bool DigitMain::clipboardContainsUris()
{
  // all entries must contain valid file names
  QString text = QApplication::clipboard()->text();
	//if (!text)
  //  return false;

  QStringList list = QStringList::split(QString("\n"), text);
  if (list.size() == 0)
    return false;

  for (QStringList::iterator itr = list.begin(); itr != list.end(); ++itr)
  {
    QFile file(Q3UriDrag::uriToLocalFile(*itr));
    if (!file.exists())
      return false;
  }

  return true;
}

bool DigitMain::documentSave(DigitDoc* doc)
{
  bool canceled = false;

  if (doc)
  {
    if (!doc->isExistingSaveFile())
      canceled = documentSaveAs(doc);
    else
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);
      if (!doc->saveDocument(doc->savePath()))
      {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical (this, tr("I/O Error!"), tr("Could not save the current document!"));
      }
      else
        QApplication::restoreOverrideCursor();
    }
  }

  return canceled;
}

bool DigitMain::documentSaveAs(DigitDoc* doc)
{
  bool canceled = false;

  if (doc)
  {
    QString filename = QFileDialog::getSaveFileName(this, tr("Save"), 
				doc->savePath(), DigitDoc::filterOpenSave());
    if (!filename.isEmpty())
    {
      bool save = true;
      if (QFile::exists(filename))
      {
        int rtn = QMessageBox::warning(0, tr("Warning"),
          tr("File already exists. Save anyway?"), tr("OK"), tr("Cancel"), 0, 0, 1);
        save = (rtn == 0);
      }

      if (save)
      {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        if (!doc->saveDocument(filename))
        {
          QApplication::restoreOverrideCursor();
          QMessageBox::critical (this, tr("I/O Error!"), tr("Could not save the current document!"));

          canceled = true;
        }
        else
        {
          doc->changedViewList();

          m_mruDocuments.mergeDocument(filename);

          QApplication::restoreOverrideCursor();
        }
      }
      else
        canceled = true;
    }
  }

  return canceled;
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void DigitMain::slotClipboardChanged()
{
  toggleActionPasteAsNew();
}

void DigitMain::slotFileImport()
{
  slotStatusNormalMsg(QString(tr("Opening image file...")));

  QString filename = QFileDialog::getOpenFileName(this, tr("Import"), QString(), DigitDoc::filterImport());

  if (!filename.isEmpty())
  {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    importImageFile(filename);
    QApplication::restoreOverrideCursor();
  }

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotFileOpen()
{
  slotStatusNormalMsg(QString(tr("Opening document file...")));

  QString filename = QFileDialog::getOpenFileName(this, tr("Open"), QString(), DigitDoc::filterOpenSave());

  if (!filename.isEmpty())
  {
    openDocumentFile(filename);

    m_mruDocuments.mergeDocument(filename);
  }

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotFileOpenRecent(QString filename)
{
  if (!filename.isEmpty())
  {
    slotStatusNormalMsg(QString(tr("Opening recent document file...")));

    openDocumentFile(filename);

    m_mruDocuments.mergeDocument(filename);

    slotStatusNormalMsg(QString(tr("Ready.")));
  }
}

void DigitMain::slotFileSave()
{
  slotStatusNormalMsg(QString(tr("Saving file...")));

  DigitDoc* doc = activeDocument();
  documentSave(doc);

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotFileSaveAs()
{
  slotStatusNormalMsg(QString(tr("Saving file under new filename...")));

  DigitDoc* doc = activeDocument();
  documentSaveAs(doc);

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotFileClose()
{
  slotStatusNormalMsg(QString(tr("Closing file...")));
  QApplication::setOverrideCursor(Qt::WaitCursor);

  DigitDoc* doc = activeDocument();
  removeDocument(doc);

  slotStatusNormalMsg(QString(tr("Ready.")));
  QApplication::restoreOverrideCursor();
}

void DigitMain::slotFileExport()
{
  slotStatusNormalMsg(QString(tr("Exporting file...")));

  DigitDoc* doc = activeDocument();
  if (doc)
  {
    if (!doc->isExistingExportFile())
      slotFileExportAs();
    else
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);
      if (!doc->exportDocument(doc->exportPath()))
      {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical (this, tr("I/O Error!"), tr("Could not export the current document!"));
      }
      else
        QApplication::restoreOverrideCursor();
    }
  }

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotFileExportAs()
{
  slotStatusNormalMsg(QString(tr("Exporting file under new filename...")));

  DigitDoc* doc = activeDocument();
  if (doc)
  {
    QString filename = QFileDialog::getSaveFileName(doc->exportPath(), 0, this, "export as",
      QString(tr("Export")));
    if (!filename.isEmpty())
    {
      bool save = true;
      if (QFile::exists(filename))
      {
        int rtn = QMessageBox::warning(0, tr("Warning"), tr("File already exists. Export anyway?"),
          tr("OK"), tr("Cancel"), 0, 0, 1);
        save = (rtn == 0);
      }

      if (save)
      {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        if (!doc->exportDocument(filename))
        {
          QApplication::restoreOverrideCursor();
          QMessageBox::critical (this, tr("I/O Error!"), tr("Could not export the current document!"));

          return;
        }
        else
          QApplication::restoreOverrideCursor();

        doc->changedViewList();
      }
    }
  }

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotFilePrint()
{
  slotStatusNormalMsg(QString(tr("Printing...")));

  ASSERT_ENGAUGE(activeDocument() != 0);
  activeDocument()->print();

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotFileExit()
{
  slotStatusNormalMsg(QString(tr("Exiting application...")));

  if (safeToQuit())
  {
    ASSERT_ENGAUGE(qApp != 0);
    qApp->quit();
  }

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotEditUndo()
{
  slotStatusNormalMsg(QString(tr("Undoing action...")));

  DigitDoc* doc = activeDocument();
  if (doc)
    doc->undo();

  toggleActions();

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotEditRedo()
{
  slotStatusNormalMsg(QString(tr("Redoing action...")));

  DigitDoc* doc = activeDocument();
  if (doc)
    doc->redo();

  toggleActions();

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotEditCut()
{
  slotStatusNormalMsg(QString(tr("Cutting selection...")));

  DigitDoc* doc = activeDocument();
  if (doc)
    doc->cut(activeView());

  toggleActions();

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotEditCopy()
{
  slotStatusNormalMsg(QString(tr("Copying selection to clipboard...")));

  DigitDoc* doc = activeDocument();
  if (doc)
    doc->copy(activeView());

  toggleActions();

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotEditPaste()
{
  slotStatusNormalMsg(QString(tr("Inserting clipboard contents...")));

  DigitDoc* doc = activeDocument();
  if (doc)
    doc->paste(activeView());

  toggleActions();

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotEditPasteAsNew()
{
  slotStatusNormalMsg(QString(tr("Inserting clipboard image...")));

  if (clipboardContainsUris())
  {
    QStringList list = QStringList::split("\n", QApplication::clipboard()->text());

    for (QStringList::iterator itr = list.begin(); itr != list.end(); ++itr)
      importImageFile(Q3UriDrag::uriToLocalFile(*itr));
  }
  else if (!QApplication::clipboard()->image().isNull())
    importImageDragged(QApplication::clipboard()->image());
  else if (!QApplication::clipboard()->pixmap().isNull())
    importPixmapDragged(QApplication::clipboard()->pixmap());

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotViewFileToolBar(bool toggle)
{
  slotStatusNormalMsg(QString(tr("Toggle file toolbar...")));

  ASSERT_ENGAUGE(fileToolbar != 0);
  fileToolbar->setShown(toggle);

  DefaultSettings::instance().setViewFileToolbar(toggle);
  
  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotViewSelectToolBar(bool toggle)
{
  slotStatusNormalMsg(QString(tr("Toggle select toolbar...")));

  ASSERT_ENGAUGE(selectToolbar != 0);
  selectToolbar->setShown(toggle);

  DefaultSettings::instance().setViewSelectToolbar(toggle);

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotViewImageScaleToolBar(bool toggle)
{
  slotStatusNormalMsg(QString(tr("Toggle image scale toolbar...")));

  ASSERT_ENGAUGE(imageScaleToolbar != 0);
  imageScaleToolbar->setShown(toggle);

  DefaultSettings::instance().setViewImageScaleToolbar(toggle);

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotViewDigitizeCurvePointsToolBar(bool toggle)
{
  slotStatusNormalMsg(QString(tr("Toggle digitize curve points toolbar...")));

  ASSERT_ENGAUGE(digitizeCurvePointsToolbar != 0);
  digitizeCurvePointsToolbar->setShown(toggle);

  DefaultSettings::instance().setViewDigitizeCurvePointsToolbar(toggle);
  
  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotViewDigitizeMeasurePointsToolBar(bool toggle)
{
  slotStatusNormalMsg(QString(tr("Toggle digitize measure points toolbar...")));

  ASSERT_ENGAUGE(digitizeMeasurePointsToolbar != 0);
  digitizeMeasurePointsToolbar->setShown(toggle);

  DefaultSettings::instance().setViewDigitizeMeasurePointsToolbar(toggle);
  
  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotViewZoomToolBar(bool toggle)
{
  slotStatusNormalMsg(QString(tr("Toggle zoom toolbar...")));

  ASSERT_ENGAUGE(zoomToolbar != 0);
  zoomToolbar->setShown(toggle);

  DefaultSettings::instance().setViewZoomToolbar(toggle);
  
  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotViewStatusBar(bool toggle)
{
  slotStatusNormalMsg(QString(tr("Toggle statusbar...")));

  ASSERT_ENGAUGE(statusBar() != 0);
  statusBar()->setShown(toggle);

  DefaultSettings::instance().setViewStatusBar(toggle);
  
  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotViewAxesPoints(bool toggle)
{
  if (activeDocument() && toggle)
  {
    slotStatusNormalMsg(QString(tr("Show axes points...")));

    activeDocument()->setViewPointSelection(ViewAxesPoints);

    DefaultSettings::instance().setViewPoints(ViewAxesPoints);
  
    slotStatusNormalMsg(QString(tr("Ready.")));
  }
}

void DigitMain::slotViewScalePoints(bool toggle)
{
  if (activeDocument() && toggle)
  {
    slotStatusNormalMsg(QString(tr("Show scale bar points...")));

    activeDocument()->setViewPointSelection(ViewScalePoints);

    DefaultSettings::instance().setViewPoints(ViewScalePoints);
    
    slotStatusNormalMsg(QString(tr("Ready.")));
  }
}

void DigitMain::slotViewCurvePoints(bool toggle)
{
  if (activeDocument() && toggle)
  {
    slotStatusNormalMsg(QString(tr("Show curve points...")));

    activeDocument()->setViewPointSelection(ViewCurvePoints);

    DefaultSettings::instance().setViewPoints(ViewCurvePoints);
    
    slotStatusNormalMsg(QString(tr("Ready.")));
  }
}

void DigitMain::slotViewMeasurePoints(bool toggle)
{
  if (activeDocument() && toggle)
  {
    slotStatusNormalMsg(QString(tr("Show measure points...")));

    activeDocument()->setViewPointSelection(ViewMeasurePoints);

    DefaultSettings::instance().setViewPoints(ViewMeasurePoints);
    
    slotStatusNormalMsg(QString(tr("Ready.")));
  }
}

void DigitMain::slotViewAllPoints(bool toggle)
{
  if (activeDocument() && toggle)
  {
    slotStatusNormalMsg(QString(tr("Show all points...")));

    activeDocument()->setViewPointSelection(ViewAllPoints);

    DefaultSettings::instance().setViewPoints(ViewAllPoints);
    
    slotStatusNormalMsg(QString(tr("Ready.")));
  }
}

void DigitMain::slotViewNoBackground(bool toggle)
{
  if (activeDocument() && toggle)
  {
    activeDocument()->setBackground(BlankBackground);

    DefaultSettings::instance().setViewBackground(BlankBackground);
  }
}

void DigitMain::slotViewOriginalImage(bool toggle)
{
  if (activeDocument() && toggle)
  {
    activeDocument()->setBackground(OriginalImage);

    DefaultSettings::instance().setViewBackground(OriginalImage);
  }
}

void DigitMain::slotViewProcessedImage(bool toggle)
{
  if (activeDocument() && toggle)
  {
    activeDocument()->setBackground(ProcessedImage);

    DefaultSettings::instance().setViewBackground(ProcessedImage);
  }
}

void DigitMain::slotViewGridDisplay(bool toggle)
{
  ASSERT_ENGAUGE(activeDocument() != 0);
  activeDocument()->showGridDisplay(toggle);
}

void DigitMain::slotViewCurveGeometry(bool toggle)
{
  ASSERT_ENGAUGE(curveGeometryDlg != 0);
  curveGeometryDlg->setShown(toggle);

  DefaultSettings::instance().setViewCurveGeometry(toggle);
}

void DigitMain::slotViewMeasureGeometry(bool toggle)
{
  ASSERT_ENGAUGE(measureGeometryDlg != 0);
  measureGeometryDlg->setShown(toggle);

  DefaultSettings::instance().setViewMeasureGeometry(toggle);
}

void DigitMain::slotDigitizeSelect(bool toggle)
{
  if (toggle)
  {
    slotStatusNormalMsg(QString(tr("Select points...")));

    setDigitizeState(SelectState);

    ASSERT_ENGAUGE(curveCmb != 0);
    ASSERT_ENGAUGE(measureCmb != 0);
    if (curveCmb->hasFocus() ||
        measureCmb->hasFocus())
    {
      // Focus is currently on a combobox. Move focus to a view so a Control 
      // keypress (which may occur prior to a click-and-drag) is handled by a
      // view rather than the combobox which just ignores it. Without this hack
      // the user has to click in the view to move focus, then press Control,
      // then click again to start the click-and-drag
      if (activeDocument())
      {
        focusOnViewOrDoc(activeView(), activeDocument());
      }
    }
  }
}

void DigitMain::slotDigitizeAxis(bool toggle)
{
  if (toggle)
  {
    slotStatusNormalMsg(QString(tr("Digitize axis point...")));

    setDigitizeState(AxisState);
  }
}

void DigitMain::slotDigitizeScale(bool toggle)
{
  if (toggle)
  {
    slotStatusNormalMsg(QString(tr("Digitize scale bar...")));

    setDigitizeState(ScaleState);
  }
}

void DigitMain::slotDigitizeCurve(bool toggle)
{
  if (toggle)
  {
    slotStatusNormalMsg(QString(tr("Digitize curve point...")));

    setDigitizeState(CurveState);
  }
}

void DigitMain::slotDigitizeMeasure(bool toggle)
{
  if (toggle)
  {
    slotStatusNormalMsg(QString(tr("Digitize measure point...")));

    setDigitizeState(MeasureState);
  }
}

void DigitMain::slotDigitizeSegment(bool toggle)
{
  if (toggle)
  {
    slotStatusNormalMsg(QString(tr("Digitize segment by filling it with points...")));

    setDigitizeState(SegmentState);
  }
}

void DigitMain::slotDigitizePointMatch(bool toggle)
{
  if (toggle)
  {
    slotStatusNormalMsg(QString(tr("Digitize point plot by first clicking on sample point...")));

    setDigitizeState(PointMatchState);
  }
}

void DigitMain::slotSettingsCoordSys()
{
  slotStatusNormalMsg(QString(tr("Configuring coordinate system...")));

  DigitDoc* doc = activeDocument();
  if (doc)
  {
    CoordSettings coord = doc->coordSettings();
    CoordSysDlg* dlg = new CoordSysDlg(doc->title(), &coord);
    CHECK_PTR_ENGAUGE(dlg);

    if (dlg->exec() == QDialog::Accepted)
    {
      DefaultSettings::instance().setCoordSettings(coord);
      doc->setCoordSettings(coord);
    }

    delete dlg;
  }

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotSettingsAxes()
{
  slotStatusNormalMsg(QString(tr("Configuring axes properties...")));

  DigitDoc* doc = activeDocument();
  if (doc)
  {
    PointSetStyle pointSetStyle = doc->axesStyle();
    PointSetDlg* dlg = new PointSetDlg(doc->title(), AxesPointSetName,
      AxisState, &pointSetStyle);
    CHECK_PTR_ENGAUGE(dlg);

    if (dlg->exec() == QDialog::Accepted)
    {
      DefaultSettings::instance().setAxesStyle(pointSetStyle);
      doc->setAxesStyle(pointSetStyle);
    }

    delete dlg;
  }

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotSettingsScale()
{
  slotStatusNormalMsg(QString(tr("Configuring scale bar properties...")));

  DigitDoc* doc = activeDocument();
  if (doc)
  {
    PointSetStyle pointSetStyle = doc->scaleStyle();
    PointSetDlg* dlg = new PointSetDlg(doc->title(), ScalePointSetName,
      ScaleState, &pointSetStyle);
    CHECK_PTR_ENGAUGE(dlg);

    if (dlg->exec() == QDialog::Accepted)
    {
      DefaultSettings::instance().setScaleStyle(pointSetStyle);
      doc->setScaleStyle(pointSetStyle);
    }

    delete dlg;
  }

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotSettingsCurves()
{
  slotStatusNormalMsg(QString(tr("Adding, removing and editing curves...")));

  DigitDoc* doc = activeDocument();
  if (doc)
  {
    PointSets* copyPointSets = new PointSets;
    CHECK_PTR_ENGAUGE(copyPointSets);
    QStringList curvesWithOrphanedPoints;

    *copyPointSets = *doc->pointSets();

    PointSetsDlg* dlg = new PointSetsDlg(true, doc->title(), copyPointSets, &curvesWithOrphanedPoints);
    CHECK_PTR_ENGAUGE(dlg);

    if (dlg->exec() == QDialog::Accepted)
    {
      // save new pointset set into document
      replacePointSets(doc, copyPointSets, curvesWithOrphanedPoints, true);

      DefaultSettings::instance().setCurveStyles(copyPointSets->curveStyles());
    }
    else
      delete copyPointSets;

    delete dlg;
  }

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotSettingsDiscretize()
{
  slotStatusNormalMsg(QString(tr("Configuring discretization...")));

  DigitDoc* doc = activeDocument();
  if (doc)
  {
    DiscretizeSettings discretize = doc->discretizeSettings();
    DiscretizeDlg* dlg = new DiscretizeDlg(doc->title(),
       &(doc->originalImage()), &discretize);
    CHECK_PTR_ENGAUGE(dlg);

    if (dlg->exec() == QDialog::Accepted)
    {
      DefaultSettings::instance().setDiscretizeSettings(discretize);
      doc->setDiscretizeSettings(discretize);
    }

    delete dlg;
  }

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotSettingsGridRemoval()
{
  slotStatusNormalMsg(QString(tr("Configuring grid removal...")));

  DigitDoc* doc = activeDocument();
  if (doc)
  {
    CoordSettings coord = doc->coordSettings();
    GridRemovalSettings grid = doc->gridRemovalSettings();
    GridRemovalDlg* dlg = new GridRemovalDlg(doc->title(),
      &(doc->originalImage()), doc->transform(), &coord, &grid);
    CHECK_PTR_ENGAUGE(dlg);

    if (dlg->exec() == QDialog::Accepted)
    {
      DefaultSettings::instance().setGridRemovalSettings(grid);
      doc->setGridRemovalSettings(grid);
    }

    delete dlg;
  }

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotSettingsGridDisplay()
{
  slotStatusNormalMsg(QString(tr("Configuring grid display...")));

  DigitDoc* doc = activeDocument();
  if (doc)
  {
    CoordSettings coord = doc->coordSettings();
    GridMeshSettings grid = doc->gridDisplaySettings();
    GridMeshDlg* dlg = new GridMeshDlg(QString(tr("Display")), doc->title(), &coord, &grid);
    CHECK_PTR_ENGAUGE(dlg);

    if (dlg->exec() == QDialog::Accepted)
    {
      DefaultSettings::instance().setGridDisplaySettings(grid);
      ASSERT_ENGAUGE(viewGridDisplay != 0);
      doc->setGridDisplaySettings(grid, viewGridDisplay->isOn());
    }

    delete dlg;
  }

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotSettingsMeasures()
{
  slotStatusNormalMsg(QString(tr("Adding, removing and editing measures...")));

  DigitDoc* doc = activeDocument();
  if (doc)
  {
    PointSets* copyPointSets = new PointSets;
    CHECK_PTR_ENGAUGE(copyPointSets);
    QStringList measuresWithOrphanedPoints;

    *copyPointSets = *doc->pointSets();

    PointSetsDlg* dlg = new PointSetsDlg(false, doc->title(), copyPointSets, &measuresWithOrphanedPoints);
    CHECK_PTR_ENGAUGE(dlg);

    if (dlg->exec() == QDialog::Accepted)
    {
      // save new pointset set into document
      replacePointSets(doc, copyPointSets, measuresWithOrphanedPoints, false);

      DefaultSettings::instance().setMeasureStyles(copyPointSets->measureStyles());
    }
    else
      delete copyPointSets;

    delete dlg;
  }

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotSettingsPointMatch()
{
  slotStatusNormalMsg(QString(tr("Configuring point match settings...")));

  DigitDoc* doc = activeDocument();
  if (doc)
  {
    PointMatchSettings match = doc->pointMatchSettings();
    PointMatchDlg* dlg = new PointMatchDlg(doc->title(),
      doc->curveStyle(curveCmb->currentText()), &match);
    CHECK_PTR_ENGAUGE(dlg);

    if (dlg->exec() == QDialog::Accepted)
    {
      DefaultSettings::instance().setPointMatchSettings(match);
      doc->setPointMatchSettings(match);
    }

    delete dlg;
  }

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotSettingsSegments()
{
  slotStatusNormalMsg(QString(tr("Configuring segment properties...")));

  DigitDoc* doc = activeDocument();
  if (doc)
  {
    SegmentSettings seg = doc->segmentSettings();
    SegmentDlg* dlg = new SegmentDlg(doc->title(), &seg);
    CHECK_PTR_ENGAUGE(dlg);

    if (dlg->exec() == QDialog::Accepted)
    {
      DefaultSettings::instance().setSegmentSettings(seg);
      doc->setSegmentSettings(seg);
    }

    delete dlg;
  }

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotSettingsExportSetup()
{
  slotStatusNormalMsg(QString(tr("Configuring export settings...")));

  DigitDoc* doc = activeDocument();
  if (doc)
  {
    PointSets* copyPointSets = new PointSets;
    CHECK_PTR_ENGAUGE(copyPointSets);

    *copyPointSets = *doc->pointSets();

    CoordSettings coordSettings = doc->coordSettings();
    GridMeshSettings gridSettings = doc->gridDisplaySettings();
    ExportSettings exportSettings = doc->exportSettings();
    ExportDlg* dlg = new ExportDlg(doc->title(), copyPointSets, &coordSettings,
      &gridSettings, &exportSettings);
    CHECK_PTR_ENGAUGE(dlg);

    if (dlg->exec() == QDialog::Accepted)
    {
      // save same set of pointsets into document, but with possibly different set of
      // pointsets included marked for export
      QStringList pointSetsWithOrphanedPoints;
      replacePointSets(doc, copyPointSets, pointSetsWithOrphanedPoints, true);

      DefaultSettings::instance().setExportSettings(exportSettings);
      doc->setExportSettings(exportSettings);
    }
    else
      delete copyPointSets;

    delete dlg;
  }

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotSettingsSessions()
{
  slotStatusNormalMsg(QString(tr("Configuring sessions settings...")));

  SessionsSettings sessionsSettings = DefaultSettings::instance().getSessionsSettings();
  SessionsDlg* dlg = new SessionsDlg(&sessionsSettings);
  CHECK_PTR_ENGAUGE(dlg);

  if (dlg->exec() == QDialog::Accepted)
  {
    DefaultSettings::instance().setSessionsSettings(sessionsSettings);
  }

  delete dlg;
  
  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotWindowNewWindow()
{
  slotStatusNormalMsg(QString(tr("Opening new document view...")));

  DigitDoc* doc = activeDocument();
  if (doc)
    createClient(doc);

  slotStatusNormalMsg(QString(tr("Ready.")));
}

void DigitMain::slotHelpAbout()
{
  QMessageBox::about(this, tr("About Engauge Digitizer"), QString("%1%2%3")
    .arg(tr("Engauge Digitizer\nVersion "))
    .arg(VersionNumber)
    .arg(tr("\n(c) 2002 by Tobias Winchen, Tobias Richter, Mark Mitchell\n"
    "Engauge Digitizer comes with ABSOLUTELY NO WARRANTY.\n\n"
    "This is free software, and you are welcome to redistribute it under\n"
    "certain conditions according to the GNU General Public License Version 2;\n"
    "read the included LICENSE file for details.\n\n"
    "Author\nContact: mmdigitizer@gmail.com\n"
    "(All messages are welcome, and your privacy will be respected)\n\n"
    "Engauge Digitizer\n"
    "Home Page: http://digitizer.sourceforge.net\n"
    "Help Forum: http://sourceforge.net/forum/forum.php?forum_id=229857\n"
    "Open Discussion Forum: http://sourceforge.net/forum/forum.php?forum_id=229856\n\n"
    "FFTW 3.X.X\n"
    "Home Page: http://www.fftw.org")));
}

void DigitMain::slotHelpUserManual()
{
  spawnHelp(QString("index.html"));
}

void DigitMain::slotHelpGlossary()
{
  spawnHelp(QString("glossary.html"));
}

void DigitMain::slotConverter()
{
  QDir path = helpPath();
  QString file(path.absPath() + "/dateconvert.html");

#ifdef WIN32
   ShellExecuteA(NULL, "open", file.latin1(), NULL, NULL, SW_SHOWNORMAL);   
#else
   QString envValue(getenv(BrowserEnvironmentVariable));
   if (envValue.isEmpty())
   {
#ifndef DEBIAN_PACKAGE
      QMessageBox::critical(this, tr("Error!"), tr("Environment variable ENGAUGE_BROWSER is not set! It must be set to the command used to start your browser"));
#else /* DEBIAN_PACKAGE */
      // QMessageBox::critical(this, tr("Error!"), tr("Environment variable ENGAUGE_BROWSER is not set! It must be set to the command used to start your browser"));
      QString cmd = QString("sh -c '/usr/bin/sensible-browser %2' &").arg(file);
      system(cmd);
#endif /* DEBIAN_PACKAGE */
   } else {
      QString cmd = QString("sh -c '%1 %2' &").arg(envValue).arg(file);
      system(cmd);
   }
#endif
}

void DigitMain::slotHelpLineGraphTutorial()
{
  spawnHelp(QString("tutormanlinegraph.html"));
}

void DigitMain::slotHelpPointGraphTutorial()
{
  spawnHelp(QString("tutormanpointgraph.html"));
}

void DigitMain::slotHelpMapTutorial()
{
  spawnHelp(QString("tutormanmap.html"));
}

void DigitMain::slotStatusNormalMsg(const QString &text)
{
  ASSERT_ENGAUGE(statusNormal != 0);
  statusNormal->setText(text);
}

void DigitMain::slotStatusPermanentMsg(const QString &text)
{
  ASSERT_ENGAUGE(statusPermanent != 0);
  statusPermanent->setText(text);
}

void DigitMain::slotStatusTemporaryMsg(const QString &text)
{
  ASSERT_ENGAUGE(statusNormal != 0);
  statusNormal->setText(text);
}

void DigitMain::slotStatusTrackingMsg(const QString &textCoords, const QString &textRes)
{
  ASSERT_ENGAUGE(statusCoords != 0);
  statusCoords->setText(textCoords);
  ASSERT_ENGAUGE(statusRes != 0);
  statusRes->setText(textRes);
}

void DigitMain::slotMenuPreactivated()
{
  ASSERT_ENGAUGE(windowMenu != 0);
  windowMenu->clear();

  ASSERT_ENGAUGE(windowNewWindow != 0);
  windowNewWindow->addTo(windowMenu);
  ASSERT_ENGAUGE(windowCascade != 0);
  windowCascade->addTo(windowMenu);
  ASSERT_ENGAUGE(windowTile != 0);
  windowTile->addTo(windowMenu);
  ASSERT_ENGAUGE(windowMenu != 0);
  windowMenu->insertSeparator();

  ASSERT_ENGAUGE(workspace != 0);
  ASSERT_ENGAUGE(windowAction != 0);
  if (workspace->subWindowList().isEmpty())
    windowAction->setEnabled(false);
  else
    windowAction->setEnabled(true);

  QList<QMdiSubWindow *> windows = workspace->subWindowList();
  for (int i = 0; i < int(windows.count()); ++i)
  {
    ASSERT_ENGAUGE(windows.at(i) != 0);
    int id = windowMenu->insertItem(QString("&%1 ").arg(i+1) + windows.at(i)->caption(), this,
      SLOT(slotMenuActivated(int)));
    windowMenu->setItemParameter(id, i);
    windowMenu->setItemChecked(id, workspace->activeSubWindow() == windows.at(i));
  }
}

void DigitMain::slotMenuActivated (int id)
{
  ASSERT_ENGAUGE(workspace != 0);
  QMdiSubWindow* view = workspace->subWindowList().at(id);
  if (view)
    view->setFocus();
}

void DigitMain::slotCurveComboActivated(int)
{
  if (activeDocument())
  {
    // keep current state, but save curveCmb selection to the active document
    setDigitizeState(activeDocument()->digitizeState());

    ASSERT_ENGAUGE(viewPointsAction != 0);
    if (viewCurvePoints->isOn())
    {
      // hide unwanted curves and show wanted curve
      slotViewCurvePoints(true);
    }
    
    activeDocument()->sendUpdateGeometry();
  }
}

void DigitMain::slotMeasureComboActivated(int)
{
  if (activeDocument())
  {
    // keep current state, but save measureCmb selection to the active document
    setDigitizeState(activeDocument()->digitizeState());

    ASSERT_ENGAUGE(viewPointsAction != 0);
    if (viewMeasurePoints->isOn())
    {
      // hide unwanted measures and show wanted measure
      slotViewMeasurePoints(true);
    }

    activeDocument()->sendUpdateGeometry();
  }
}

void DigitMain::slotSelectionChange()
{
  toggleActionsEdit();
}

void DigitMain::slotTransformationChange()
{
  toggleActions();
}

void DigitMain::slotHideCurveGeometry()
{
  ASSERT_ENGAUGE(viewCurveGeometry != 0);
  viewCurveGeometry->setOn(false);
}

void DigitMain::slotHideMeasureGeometry()
{
  ASSERT_ENGAUGE(viewMeasureGeometry != 0);
  viewMeasureGeometry->setOn(false);
}

void DigitMain::slotUpdateGeometry()
{
  if (activeDocument())
  {
    GeometryWindowItemList info;

    if (activeDocument()->geometryInfoCurve(info))
      displayCurveGeometry(info);
      
    if (activeDocument()->geometryInfoMeasure(info))
      displayMeasureGeometry(info);
  }
  else
    noGeometryInfo();
}
