/***************************************************************************
                          defaultsettings.cpp  -  description
                             -------------------
    begin                : Thu Sep 9 2004
    copyright            : (C) 2004 by Mark Mitchell
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

#include <qdir.h>
#include <qsettings.h>

#include "defaultsettings.h"
#include "digitdebug.h"

extern bool cmdSettingsReset;
extern bool cmdSettingsText;

DefaultSettings* DefaultSettings::m_settings = (DefaultSettings*) 0;

const QString ENGAUGE_GROUP_PREFIX("/Engauge"); // all settings below belong to this group

const QString SESSIONS_GROUP(ENGAUGE_GROUP_PREFIX + "/sessions"); // group prefix for sessions
const QString SESSIONS_SESSIONS(SESSIONS_GROUP + "/sessions");
const QString SESSIONS_COORDINATES(SESSIONS_GROUP + "/coordinates");
const QString SESSIONS_AXES(SESSIONS_GROUP + "/axes");
const QString SESSIONS_SCALEBAR(SESSIONS_GROUP + "/scalebar");
const QString SESSIONS_CURVES(SESSIONS_GROUP + "/curves");
const QString SESSIONS_SEGMENTS(SESSIONS_GROUP + "/segments");
const QString SESSIONS_POINTMATCH(SESSIONS_GROUP + "/pointmatch");
const QString SESSIONS_MEASURES(SESSIONS_GROUP + "/measures");
const QString SESSIONS_DISCRETIZE(SESSIONS_GROUP + "/discretize");
const QString SESSIONS_GRIDREMOVAL(SESSIONS_GROUP + "/gridremoval");
const QString SESSIONS_GRIDDISPLAY(SESSIONS_GROUP + "/griddisplay");
const QString SESSIONS_EXPORTING(SESSIONS_GROUP + "/export");
const QString SESSIONS_VIEWSELECTIONS(SESSIONS_GROUP + "/viewselections");
const QString SESSIONS_INITIALDIGITIZESTATE(SESSIONS_GROUP + "/initialdigitizestate");

const QString COORD_GROUP(ENGAUGE_GROUP_PREFIX + "/coord"); // group prefix for coord
const QString COORD_FRAME(COORD_GROUP + "/frame");
const QString COORD_ORIGINRADIUS(COORD_GROUP + "/originradius");
const QString COORD_XTHETASCALE(COORD_GROUP + "/xthetascale");
const QString COORD_YRSCALE(COORD_GROUP + "/yrscale");
const QString COORD_THETAUNITS(COORD_GROUP + "/thetaunits");

const QString EXPORT_GROUP(ENGAUGE_GROUP_PREFIX + "/export"); // group prefix for export
const QString EXPORT_POINTSSELECTION(EXPORT_GROUP + "/pointsselection");
const QString EXPORT_LAYOUT(EXPORT_GROUP + "/layout");
const QString EXPORT_DELIMITERS(EXPORT_GROUP + "/delimiters");
const QString EXPORT_HEADER(EXPORT_GROUP + "/header");

const QString VIEW_GROUP(ENGAUGE_GROUP_PREFIX + "/view"); // group prefix for view selections
const QString VIEW_TOOLBARS(VIEW_GROUP + "/toolbars"); // subgroup prefix for view toolbars selections
const QString VIEW_FILETOOLBAR(VIEW_TOOLBARS + "/file");
const QString VIEW_SELECTTOOLBAR(VIEW_TOOLBARS + "/select");
const QString VIEW_IMAGESCALETOOLBAR(VIEW_TOOLBARS + "/imagescale");
const QString VIEW_DIGITIZECURVEPOINTSTOOLBAR(VIEW_TOOLBARS + "/digitizecurvepoints");
const QString VIEW_DIGITIZEMEASUREPOINTSTOOLBAR(VIEW_TOOLBARS + "/digitizemeasurepoints");
const QString VIEW_ZOOMTOOLBAR(VIEW_TOOLBARS + "/zoom");
const QString VIEW_STATUSBAR(VIEW_GROUP + "/statusbar");
const QString VIEW_POINTS(VIEW_GROUP + "/points");
const QString VIEW_BACKGROUND(VIEW_GROUP + "/background");
const QString VIEW_CURVEGEOMETRY(VIEW_GROUP + "/curvegeometry");
const QString VIEW_MEASUREGEOMETRY(VIEW_GROUP + "/measuregeometry");

const QString MATH_GROUP(ENGAUGE_GROUP_PREFIX + "/math"); // group prefix for math
const QString MATH_POWERMOSTSIGMAX(MATH_GROUP + "/powermostsigmax");
const QString MATH_POWERMOSTSIGMIN(MATH_GROUP + "/powermostsigmin");
const QString MATH_DOUBLEMIN(MATH_GROUP + "/doublemin");
const QString MATH_MAXCOMMANDS(MATH_GROUP + "/maxcommands");

const QString WINDOW_CURVE_GROUP(ENGAUGE_GROUP_PREFIX + "/window/curve"); // group prefix for curve geometry window
const QString WINDOW_CURVE_WIDTH(WINDOW_CURVE_GROUP + "/width");
const QString WINDOW_CURVE_HEIGHT(WINDOW_CURVE_GROUP + "/height");
const QString WINDOW_CURVE_X(WINDOW_CURVE_GROUP + "/x");
const QString WINDOW_CURVE_Y(WINDOW_CURVE_GROUP + "/y");

const QString WINDOW_HELP_GROUP(ENGAUGE_GROUP_PREFIX + "/window/help"); // group prefix for help window
const QString WINDOW_HELP_WIDTH(WINDOW_HELP_GROUP + "/width");
const QString WINDOW_HELP_HEIGHT(WINDOW_HELP_GROUP + "/height");
const QString WINDOW_HELP_X(WINDOW_HELP_GROUP + "/x");
const QString WINDOW_HELP_Y(WINDOW_HELP_GROUP + "/y");

const QString WINDOW_MAIN_GROUP(ENGAUGE_GROUP_PREFIX + "/window/main"); // group prefix for main window
const QString WINDOW_MAIN_WIDTH(WINDOW_MAIN_GROUP + "/width");
const QString WINDOW_MAIN_HEIGHT(WINDOW_MAIN_GROUP + "/height");
const QString WINDOW_MAIN_X(WINDOW_MAIN_GROUP + "/x");
const QString WINDOW_MAIN_Y(WINDOW_MAIN_GROUP + "/y");
const QString WINDOW_MAIN_FONT_OVERRIDE(WINDOW_MAIN_GROUP + "/font/override");
const QString WINDOW_MAIN_FONT_NAME(WINDOW_MAIN_GROUP + "/font/name");
const QString WINDOW_MAIN_FONT_SIZE(WINDOW_MAIN_GROUP + "/font/size");
const QString WINDOW_MAIN_COMBOBOXWIDTHMIN(WINDOW_MAIN_GROUP + "/comboboxwidthmin");

const QString WINDOW_MEASURE_GROUP(ENGAUGE_GROUP_PREFIX + "/window/measure"); // group prefix for measure geometry window
const QString WINDOW_MEASURE_WIDTH(WINDOW_MEASURE_GROUP + "/width");
const QString WINDOW_MEASURE_HEIGHT(WINDOW_MEASURE_GROUP + "/height");
const QString WINDOW_MEASURE_X(WINDOW_MEASURE_GROUP + "/x");
const QString WINDOW_MEASURE_Y(WINDOW_MEASURE_GROUP + "/y");

const QString KEY_GROUP(ENGAUGE_GROUP_PREFIX + "/key"); // group prefix for key
const QString KEY_CLIPBOARD(KEY_GROUP + "/clipboard");
const QString KEY_TREE(KEY_GROUP + "/tree");
const QString KEY_VARIABLES(KEY_GROUP + "/variables");

const QString HANDLE_GROUP(ENGAUGE_GROUP_PREFIX + "/handle"); // group prefix for handle
const QString HANDLE_WIDTH(HANDLE_GROUP + "/width");
const QString HANDLE_HEIGHT(HANDLE_GROUP + "/height");

const QString SEGMENT_GROUP(ENGAUGE_GROUP_PREFIX + "/segment"); // group prefix for segment
const QString SEGMENT_MINPOINTS(SEGMENT_GROUP + "/minpoints");
const QString SEGMENT_POINTMINSEPARATION(SEGMENT_GROUP + "/pointminseparation");
const QString SEGMENT_POINTDEFAULTSEPARATION(SEGMENT_GROUP + "/pointdefaultseparation");
const QString SEGMENT_FILLCORNERS(SEGMENT_GROUP + "/fillcorners");
const QString SEGMENT_LINESIZE(SEGMENT_GROUP + "/linesize");
const QString SEGMENT_LINECOLOR(SEGMENT_GROUP + "/linecolor");

const QString GRID_REMOVAL_GROUP(ENGAUGE_GROUP_PREFIX + "/grid/removal"); // group prefix for grid removal
const QString GRID_REMOVAL_REMOVETHINLINES(GRID_REMOVAL_GROUP + "/removethinlines");
const QString GRID_REMOVAL_THINTHICKNESS(GRID_REMOVAL_GROUP + "/thinthickness");
const QString GRID_REMOVAL_REMOVEGRIDLINES(GRID_REMOVAL_GROUP + "/removegridlines");
const QString GRID_REMOVAL_GRIDSETX(GRID_REMOVAL_GROUP + "/gridsetx");
const QString GRID_REMOVAL_GRIDSETY(GRID_REMOVAL_GROUP + "/gridsety");
const QString GRID_REMOVAL_GRIDDISTANCE(GRID_REMOVAL_GROUP + "/griddistance");
const QString GRID_REMOVAL_REMOVECOLOR(GRID_REMOVAL_GROUP + "/removecolor");
const QString GRID_REMOVAL_COLOR_RED(GRID_REMOVAL_GROUP + "/color/red");
const QString GRID_REMOVAL_COLOR_GREEN(GRID_REMOVAL_GROUP + "/color/green");
const QString GRID_REMOVAL_COLOR_BLUE(GRID_REMOVAL_GROUP + "/color/blue");
const QString GRID_REMOVAL_GAPSEPARATION(GRID_REMOVAL_GROUP + "/gapseparation");
const QString GRID_REMOVAL_FOREGROUNDTHRESHOLDLOW(GRID_REMOVAL_GROUP + "/foregroundthresholdlow");
const QString GRID_REMOVAL_FOREGROUNDTHRESHOLDHIGH(GRID_REMOVAL_GROUP + "/foregroundthresholdhigh");

const QString GRID_DISPLAY_GROUP(ENGAUGE_GROUP_PREFIX + "/grid/display"); // group prefix for grid display
const QString GRID_DISPLAY_GRIDSETX(GRID_DISPLAY_GROUP + "/gridsetx");
const QString GRID_DISPLAY_GRIDSETY(GRID_DISPLAY_GROUP + "/gridsety");

const QString POINTMATCH_GROUP(ENGAUGE_GROUP_PREFIX + "/pointmatch"); // group prefix for pointmatch
const QString POINTMATCH_HIGHLIGHTDIAMETER(POINTMATCH_GROUP + "/highlightdiameter");
const QString POINTMATCH_HIGHLIGHTLINESIZE(POINTMATCH_GROUP + "/highlightlinesize");
const QString POINTMATCH_SEPARATIONDEFAULT(POINTMATCH_GROUP + "/separationdefault");
const QString POINTMATCH_SIZEDEFAULT(POINTMATCH_GROUP + "/sizedefault");
const QString POINTMATCH_ACCEPTEDCOLOR(POINTMATCH_GROUP + "/acceptedcolor");
const QString POINTMATCH_REJECTEDCOLOR(POINTMATCH_GROUP + "/rejectedcolor");
const QString POINTMATCH_CANDIDATECOLOR(POINTMATCH_GROUP + "/candidatecolor");

const QString DISCRETIZE_GROUP(ENGAUGE_GROUP_PREFIX + "/discretize"); // group prefix for discretize
const QString DISCRETIZE_METHODDEFAULT(DISCRETIZE_GROUP + "/methoddefault");
const QString DISCRETIZE_INTENSITY_THRESHOLDLOW(DISCRETIZE_GROUP + "/intensity/thresholdlow");
const QString DISCRETIZE_INTENSITY_THRESHOLDHIGH(DISCRETIZE_GROUP + "/intensity/thresholdhigh");
const QString DISCRETIZE_FOREGROUND_THRESHOLDLOW(DISCRETIZE_GROUP + "/foreground/thresholdlow");
const QString DISCRETIZE_FOREGROUND_THRESHOLDHIGH(DISCRETIZE_GROUP + "/foreground/thresholdhigh");
const QString DISCRETIZE_HUE_THRESHOLDLOW(DISCRETIZE_GROUP + "/hue/thresholdlow");
const QString DISCRETIZE_HUE_THRESHOLDHIGH(DISCRETIZE_GROUP + "/hue/thresholdhigh");
const QString DISCRETIZE_SATURATION_THRESHOLDLOW(DISCRETIZE_GROUP + "/saturation/thresholdlow");
const QString DISCRETIZE_SATURATION_THRESHOLDHIGH(DISCRETIZE_GROUP + "/saturation/thresholdhigh");
const QString DISCRETIZE_VALUE_THRESHOLDLOW(DISCRETIZE_GROUP + "/value/thresholdlow");
const QString DISCRETIZE_VALUE_THRESHOLDHIGH(DISCRETIZE_GROUP + "/value/thresholdhigh");

const QString POINTSET_AXES_GROUP(ENGAUGE_GROUP_PREFIX + "/pointset/axes"); // group prefix for axes
const QString POINTSET_AXES_POINTSHAPE(POINTSET_AXES_GROUP + "/pointshape");
const QString POINTSET_AXES_POINTSIZE(POINTSET_AXES_GROUP + "/pointsize");
const QString POINTSET_AXES_POINTLINESIZE(POINTSET_AXES_GROUP + "/pointlinesize");
const QString POINTSET_AXES_POINTLINECOLOR(POINTSET_AXES_GROUP + "/pointlinecolor");
const QString POINTSET_AXES_POINTINCOLOR(POINTSET_AXES_GROUP + "/pointincolor");
const QString POINTSET_AXES_LINESIZE(POINTSET_AXES_GROUP + "/linesize");
const QString POINTSET_AXES_LINECOLOR(POINTSET_AXES_GROUP + "/linecolor");
const QString POINTSET_AXES_LINECONNECTAS(POINTSET_AXES_GROUP + "/lineconnectas");

const QString POINTSET_SCALE_GROUP(ENGAUGE_GROUP_PREFIX + "/pointset/scale"); // group prefix for scale
const QString POINTSET_SCALE_POINTSHAPE(POINTSET_SCALE_GROUP + "/pointshape");
const QString POINTSET_SCALE_POINTSIZE(POINTSET_SCALE_GROUP + "/pointsize");
const QString POINTSET_SCALE_POINTLINESIZE(POINTSET_SCALE_GROUP + "/pointlinesize");
const QString POINTSET_SCALE_POINTLINECOLOR(POINTSET_SCALE_GROUP + "/pointlinecolor");
const QString POINTSET_SCALE_POINTINCOLOR(POINTSET_SCALE_GROUP + "/pointincolor");
const QString POINTSET_SCALE_LINESIZE(POINTSET_SCALE_GROUP + "/linesize");
const QString POINTSET_SCALE_LINECOLOR(POINTSET_SCALE_GROUP + "/linecolor");
const QString POINTSET_SCALE_LINECONNECTAS(POINTSET_SCALE_GROUP + "/lineconnectas");

const QString POINTSET_CURVE_GROUP(ENGAUGE_GROUP_PREFIX + "/pointset/curve"); // group prefix for 0 or more curves
const QString POINTSET_CURVE_POINTSHAPE(POINTSET_CURVE_GROUP + "/%1/pointshape");
const QString POINTSET_CURVE_POINTSIZE(POINTSET_CURVE_GROUP + "/%1/pointsize");
const QString POINTSET_CURVE_POINTLINESIZE(POINTSET_CURVE_GROUP + "/%1/pointlinesize");
const QString POINTSET_CURVE_POINTLINECOLOR(POINTSET_CURVE_GROUP + "/%1/pointlinecolor");
const QString POINTSET_CURVE_POINTINCOLOR(POINTSET_CURVE_GROUP + "/%1/pointincolor");
const QString POINTSET_CURVE_LINESIZE(POINTSET_CURVE_GROUP + "/%1/linesize");
const QString POINTSET_CURVE_LINECOLOR(POINTSET_CURVE_GROUP + "/%1/linecolor");
const QString POINTSET_CURVE_LINECONNECTAS(POINTSET_CURVE_GROUP + "/%1/lineconnectas");

const QString POINTSET_MEASURE_GROUP(ENGAUGE_GROUP_PREFIX + "/pointset/measure"); // group prefix for 0 or more measures
const QString POINTSET_MEASURE_POINTSHAPE(POINTSET_MEASURE_GROUP + "/%1/pointshape");
const QString POINTSET_MEASURE_POINTSIZE(POINTSET_MEASURE_GROUP + "/%1/pointsize");
const QString POINTSET_MEASURE_POINTLINESIZE(POINTSET_MEASURE_GROUP + "/%1/pointlinesize");
const QString POINTSET_MEASURE_POINTLINECOLOR(POINTSET_MEASURE_GROUP + "/%1/pointlinecolor");
const QString POINTSET_MEASURE_POINTINCOLOR(POINTSET_MEASURE_GROUP + "/%1/pointincolor");
const QString POINTSET_MEASURE_LINESIZE(POINTSET_MEASURE_GROUP + "/%1/linesize");
const QString POINTSET_MEASURE_LINECOLOR(POINTSET_MEASURE_GROUP + "/%1/linecolor");
const QString POINTSET_MEASURE_LINECONNECTAS(POINTSET_MEASURE_GROUP + "/%1/lineconnectas");

const QString MRU_LIST_GROUP(ENGAUGE_GROUP_PREFIX + "/mru"); // group prefix for mru list
const QString MRU_LIST_MAX(MRU_LIST_GROUP + "/max");
const QString MRU_LIST_DOCUMENTS_GROUP(MRU_LIST_GROUP + "/documents"); // subgroup prefix for 0 or more mru documents
const QString MRU_LIST_DOCUMENTS(MRU_LIST_DOCUMENTS_GROUP + "/%1/document");

const QString SCANFORAXES_GROUP(ENGAUGE_GROUP_PREFIX + "/scanforaxes"); // group prefix for scan for axes
const QString SCANFORAXES_LINECOLOR(ENGAUGE_GROUP_PREFIX + "/linecolor");
const QString SCANFORAXES_LINEWIDTH(ENGAUGE_GROUP_PREFIX + "/linewidth");
const QString SCANFORAXES_ENDPOINTKERNELWIDTH(ENGAUGE_GROUP_PREFIX + "/endpointkernelwidth");
const QString SCANFORAXES_SCANEFFECT(ENGAUGE_GROUP_PREFIX + "/scaneffect");

DefaultSettings::DefaultSettings()
{
  DigitDebug::ctor(QString("settings ") + QString::number((ulong) this, 16));

  initializeSettingsToFactoryDefaults();
  if (!cmdSettingsReset)
    loadArchivedSettings();
}

DefaultSettings::~DefaultSettings()
{
  DigitDebug::dtor(QString("settings ") + QString::number((ulong) this, 16));
}

void DefaultSettings::archiveSettings()
{
  // in linux this archiver must be a local variable or else file is not saved. in
  // windows any calls to insertSearchPath or setPath either cancel cmdSettingsText,
  // or they cause multiple files to be written out (one per group)
  //QSettings archiver(cmdSettingsText ? QSettings::Ini : QSettings::Native);
  QSettings archiver;

  archiver.writeEntry(SESSIONS_SESSIONS, m_sessionsSettings.sessions);
  archiver.writeEntry(SESSIONS_COORDINATES, m_sessionsSettings.coordinates);
  archiver.writeEntry(SESSIONS_AXES, m_sessionsSettings.axes);
  archiver.writeEntry(SESSIONS_SCALEBAR, m_sessionsSettings.scaleBar);
  archiver.writeEntry(SESSIONS_CURVES, m_sessionsSettings.curves);
  archiver.writeEntry(SESSIONS_SEGMENTS, m_sessionsSettings.segments);
  archiver.writeEntry(SESSIONS_POINTMATCH, m_sessionsSettings.pointMatch);
  archiver.writeEntry(SESSIONS_MEASURES, m_sessionsSettings.measures);
  archiver.writeEntry(SESSIONS_DISCRETIZE, m_sessionsSettings.discretize);
  archiver.writeEntry(SESSIONS_GRIDREMOVAL, m_sessionsSettings.gridRemoval);
  archiver.writeEntry(SESSIONS_GRIDDISPLAY, m_sessionsSettings.gridDisplay);
  archiver.writeEntry(SESSIONS_EXPORTING, m_sessionsSettings.exporting);
  archiver.writeEntry(SESSIONS_VIEWSELECTIONS, m_sessionsSettings.viewSelections);
  archiver.writeEntry(SESSIONS_INITIALDIGITIZESTATE, m_sessionsSettings.initialDigitizeState);
  
  archiver.writeEntry(COORD_FRAME, m_coordSettings.frame);
  archiver.writeEntry(COORD_ORIGINRADIUS, m_coordSettings.originRadius);
  archiver.writeEntry(COORD_XTHETASCALE, m_coordSettings.xThetaScale);
  archiver.writeEntry(COORD_YRSCALE, m_coordSettings.yRScale);
  archiver.writeEntry(COORD_THETAUNITS, m_coordSettings.thetaUnits);

  archiver.writeEntry(EXPORT_POINTSSELECTION, m_exportSettings.pointsSelection);
  archiver.writeEntry(EXPORT_LAYOUT, m_exportSettings.layout);
  archiver.writeEntry(EXPORT_DELIMITERS, m_exportSettings.delimiters);
  archiver.writeEntry(EXPORT_HEADER, m_exportSettings.header);

  archiver.writeEntry(VIEW_FILETOOLBAR, m_viewFileToolbar);
  archiver.writeEntry(VIEW_SELECTTOOLBAR, m_viewSelectToolbar);
  archiver.writeEntry(VIEW_IMAGESCALETOOLBAR, m_viewImageScaleToolbar);
  archiver.writeEntry(VIEW_DIGITIZECURVEPOINTSTOOLBAR, m_viewDigitizeCurvePointsToolbar);
  archiver.writeEntry(VIEW_DIGITIZEMEASUREPOINTSTOOLBAR, m_viewDigitizeMeasurePointsToolbar);
  archiver.writeEntry(VIEW_ZOOMTOOLBAR, m_viewZoomToolbar);
  archiver.writeEntry(VIEW_STATUSBAR, m_viewStatusBar);
  archiver.writeEntry(VIEW_POINTS, m_viewPoints);
  archiver.writeEntry(VIEW_BACKGROUND, m_viewBackground);
  archiver.writeEntry(VIEW_CURVEGEOMETRY, m_viewCurveGeometry);
  archiver.writeEntry(VIEW_MEASUREGEOMETRY, m_viewMeasureGeometry);

  archiver.writeEntry(MATH_POWERMOSTSIGMAX, m_powerMostSigMax);
  archiver.writeEntry(MATH_POWERMOSTSIGMIN, m_powerMostSigMin);
  archiver.writeEntry(MATH_DOUBLEMIN, m_doubleMin);
  archiver.writeEntry(MATH_MAXCOMMANDS, (int) m_maxCommands);

  archiver.writeEntry(WINDOW_CURVE_WIDTH, m_windowCurveSize.width());
  archiver.writeEntry(WINDOW_CURVE_HEIGHT, m_windowCurveSize.height());
  archiver.writeEntry(WINDOW_CURVE_X, m_windowCurvePosition.x());
  archiver.writeEntry(WINDOW_CURVE_Y, m_windowCurvePosition.y());

  archiver.writeEntry(WINDOW_HELP_WIDTH, m_windowHelpSize.width());
  archiver.writeEntry(WINDOW_HELP_HEIGHT, m_windowHelpSize.height());
  archiver.writeEntry(WINDOW_HELP_X, m_windowHelpPosition.x());
  archiver.writeEntry(WINDOW_HELP_Y, m_windowHelpPosition.y());

  archiver.writeEntry(WINDOW_MAIN_WIDTH, m_windowMainSize.width());
  archiver.writeEntry(WINDOW_MAIN_HEIGHT, m_windowMainSize.height());
  archiver.writeEntry(WINDOW_MAIN_X, m_windowMainPosition.x());
  archiver.writeEntry(WINDOW_MAIN_Y, m_windowMainPosition.y());
  archiver.writeEntry(WINDOW_MAIN_FONT_OVERRIDE, m_windowMainFontOverride);
  archiver.writeEntry(WINDOW_MAIN_FONT_NAME, m_windowMainFontName);
  archiver.writeEntry(WINDOW_MAIN_FONT_SIZE, m_windowMainFontSize);
  archiver.writeEntry(WINDOW_MAIN_COMBOBOXWIDTHMIN, m_windowMainComboBoxWidthMin);

  archiver.writeEntry(WINDOW_MEASURE_WIDTH, m_windowMeasureSize.width());
  archiver.writeEntry(WINDOW_MEASURE_HEIGHT, m_windowMeasureSize.height());
  archiver.writeEntry(WINDOW_MEASURE_X, m_windowMeasurePosition.x());
  archiver.writeEntry(WINDOW_MEASURE_Y, m_windowMeasurePosition.y());

  archiver.writeEntry(KEY_CLIPBOARD, m_keyClipboard);
  archiver.writeEntry(KEY_TREE, m_keyTree);
  archiver.writeEntry(KEY_VARIABLES, m_keyVariables);

  archiver.writeEntry(HANDLE_WIDTH, m_handleSize.width());
  archiver.writeEntry(HANDLE_HEIGHT, m_handleSize.height());

  archiver.writeEntry(SEGMENT_POINTMINSEPARATION, m_segmentPointMinSeparation);
  archiver.writeEntry(SEGMENT_MINPOINTS, m_segmentSettings.minPoints);
  archiver.writeEntry(SEGMENT_POINTDEFAULTSEPARATION, m_segmentSettings.pointSeparation);
  archiver.writeEntry(SEGMENT_FILLCORNERS, m_segmentSettings.fillCorners);
  archiver.writeEntry(SEGMENT_LINESIZE, m_segmentSettings.lineSize);
  archiver.writeEntry(SEGMENT_LINECOLOR, m_segmentSettings.lineColor);

  archiver.writeEntry(GRID_REMOVAL_GRIDSETX, m_gridRemovalSettings.gridMesh.gridSetX);
  archiver.writeEntry(GRID_REMOVAL_GRIDSETY, m_gridRemovalSettings.gridMesh.gridSetY);
  archiver.writeEntry(GRID_REMOVAL_THINTHICKNESS, m_gridRemovalSettings.thinThickness);
  archiver.writeEntry(GRID_REMOVAL_GRIDDISTANCE, m_gridRemovalSettings.gridDistance);
  archiver.writeEntry(GRID_REMOVAL_COLOR_RED, m_gridRemovalSettings.color.red());
  archiver.writeEntry(GRID_REMOVAL_COLOR_GREEN, m_gridRemovalSettings.color.green());
  archiver.writeEntry(GRID_REMOVAL_COLOR_BLUE, m_gridRemovalSettings.color.blue());
  archiver.writeEntry(GRID_REMOVAL_GAPSEPARATION, m_gridRemovalSettings.gapSeparation);
  archiver.writeEntry(GRID_REMOVAL_FOREGROUNDTHRESHOLDLOW, m_gridRemovalSettings.foregroundThresholdLow);
  archiver.writeEntry(GRID_REMOVAL_FOREGROUNDTHRESHOLDHIGH, m_gridRemovalSettings.foregroundThresholdHigh);

  archiver.writeEntry(GRID_DISPLAY_GRIDSETX, m_gridDisplayGridSetX);
  archiver.writeEntry(GRID_DISPLAY_GRIDSETY, m_gridDisplayGridSetY);

  archiver.writeEntry(POINTMATCH_HIGHLIGHTDIAMETER, m_pointMatchHighlightDiameter);
  archiver.writeEntry(POINTMATCH_HIGHLIGHTLINESIZE, m_pointMatchHighlightLineSize);
  archiver.writeEntry(POINTMATCH_SEPARATIONDEFAULT, m_pointMatchSettings.pointSeparation);
  archiver.writeEntry(POINTMATCH_SIZEDEFAULT, m_pointMatchSettings.pointSize);
  archiver.writeEntry(POINTMATCH_ACCEPTEDCOLOR, m_pointMatchSettings.acceptedColor);
  archiver.writeEntry(POINTMATCH_REJECTEDCOLOR, m_pointMatchSettings.rejectedColor);
  archiver.writeEntry(POINTMATCH_CANDIDATECOLOR, m_pointMatchSettings.candidateColor);

  archiver.writeEntry(DISCRETIZE_METHODDEFAULT, m_discretizeSettings.discretizeMethod);
  archiver.writeEntry(DISCRETIZE_INTENSITY_THRESHOLDLOW, m_discretizeSettings.intensityThresholdLow);
  archiver.writeEntry(DISCRETIZE_INTENSITY_THRESHOLDHIGH, m_discretizeSettings.intensityThresholdHigh);
  archiver.writeEntry(DISCRETIZE_FOREGROUND_THRESHOLDLOW, m_discretizeSettings.foregroundThresholdLow);
  archiver.writeEntry(DISCRETIZE_FOREGROUND_THRESHOLDHIGH, m_discretizeSettings.foregroundThresholdHigh);
  archiver.writeEntry(DISCRETIZE_HUE_THRESHOLDLOW, m_discretizeSettings.hueThresholdLow);
  archiver.writeEntry(DISCRETIZE_HUE_THRESHOLDHIGH, m_discretizeSettings.hueThresholdHigh);
  archiver.writeEntry(DISCRETIZE_SATURATION_THRESHOLDLOW, m_discretizeSettings.saturationThresholdLow);
  archiver.writeEntry(DISCRETIZE_SATURATION_THRESHOLDHIGH, m_discretizeSettings.saturationThresholdHigh);
  archiver.writeEntry(DISCRETIZE_VALUE_THRESHOLDLOW, m_discretizeSettings.valueThresholdLow);
  archiver.writeEntry(DISCRETIZE_VALUE_THRESHOLDHIGH, m_discretizeSettings.valueThresholdHigh);

  archiver.writeEntry(POINTSET_AXES_POINTSHAPE, m_axesStyle.pointShape);
  archiver.writeEntry(POINTSET_AXES_POINTSIZE, m_axesStyle.pointSize);
  archiver.writeEntry(POINTSET_AXES_POINTLINESIZE, m_axesStyle.pointLineSize);
  archiver.writeEntry(POINTSET_AXES_POINTLINECOLOR, m_axesStyle.pointLineColor);
  archiver.writeEntry(POINTSET_AXES_POINTINCOLOR, m_axesStyle.pointInColor);
  archiver.writeEntry(POINTSET_AXES_LINESIZE, m_axesStyle.lineSize);
  archiver.writeEntry(POINTSET_AXES_LINECOLOR, m_axesStyle.lineColor);
  archiver.writeEntry(POINTSET_AXES_LINECONNECTAS, m_axesStyle.lineConnectAs);

  archiver.writeEntry(POINTSET_SCALE_POINTSHAPE, m_scaleStyle.pointShape);
  archiver.writeEntry(POINTSET_SCALE_POINTSIZE, m_scaleStyle.pointSize);
  archiver.writeEntry(POINTSET_SCALE_POINTLINESIZE, m_scaleStyle.pointLineSize);
  archiver.writeEntry(POINTSET_SCALE_POINTLINECOLOR, m_scaleStyle.pointLineColor);
  archiver.writeEntry(POINTSET_SCALE_POINTINCOLOR, m_scaleStyle.pointInColor);
  archiver.writeEntry(POINTSET_SCALE_LINESIZE, m_scaleStyle.lineSize);
  archiver.writeEntry(POINTSET_SCALE_LINECOLOR, m_scaleStyle.lineColor);
  archiver.writeEntry(POINTSET_SCALE_LINECONNECTAS, m_scaleStyle.lineConnectAs);

  int index;
  PointSetStyleList::iterator itr;

  for (index = 0, itr = m_curveStyles.begin(); itr != m_curveStyles.end(); ++itr, ++index)
  {
    archiver.writeEntry(POINTSET_CURVE_POINTSHAPE.arg(index), (*itr).pointShape);
    archiver.writeEntry(POINTSET_CURVE_POINTSIZE.arg(index), (*itr).pointSize);
    archiver.writeEntry(POINTSET_CURVE_POINTLINESIZE.arg(index), (*itr).pointLineSize);
    archiver.writeEntry(POINTSET_CURVE_POINTLINECOLOR.arg(index), (*itr).pointLineColor);
    archiver.writeEntry(POINTSET_CURVE_POINTINCOLOR.arg(index), (*itr).pointInColor);
    archiver.writeEntry(POINTSET_CURVE_LINESIZE.arg(index), (*itr).lineSize);
    archiver.writeEntry(POINTSET_CURVE_LINECOLOR.arg(index), (*itr).lineColor);
    archiver.writeEntry(POINTSET_CURVE_LINECONNECTAS.arg(index), (*itr).lineConnectAs);
  }

  for (index = 0, itr = m_measureStyles.begin(); itr != m_measureStyles.end(); ++itr, ++index)
  {
    archiver.writeEntry(POINTSET_MEASURE_POINTSHAPE.arg(index), (*itr).pointShape);
    archiver.writeEntry(POINTSET_MEASURE_POINTSIZE.arg(index), (*itr).pointSize);
    archiver.writeEntry(POINTSET_MEASURE_POINTLINESIZE.arg(index), (*itr).pointLineSize);
    archiver.writeEntry(POINTSET_MEASURE_POINTLINECOLOR.arg(index), (*itr).pointLineColor);
    archiver.writeEntry(POINTSET_MEASURE_POINTINCOLOR.arg(index), (*itr).pointInColor);
    archiver.writeEntry(POINTSET_MEASURE_LINESIZE.arg(index), (*itr).lineSize);
    archiver.writeEntry(POINTSET_MEASURE_LINECOLOR.arg(index), (*itr).lineColor);
    archiver.writeEntry(POINTSET_MEASURE_LINECONNECTAS.arg(index), (*itr).lineConnectAs);
  }

  archiver.writeEntry(MRU_LIST_MAX, m_mruMax);

  QStringList::iterator its;
  for (index = 0, its = m_mruDocuments.begin(); its != m_mruDocuments.end(); ++index, ++its)
    archiver.writeEntry(MRU_LIST_DOCUMENTS.arg(index), (*its));

  archiver.writeEntry(SCANFORAXES_LINECOLOR, m_scanForAxesLineColor);
  archiver.writeEntry(SCANFORAXES_LINEWIDTH, m_scanForAxesLineWidth);
  archiver.writeEntry(SCANFORAXES_ENDPOINTKERNELWIDTH, m_scanForAxesEndpointKernelWidth);
  archiver.writeEntry(SCANFORAXES_SCANEFFECT, m_scanForAxesScanEffect);
}

PointSetStyle DefaultSettings::getAxesStyle()
{
  return m_axesStyle;
}

int DefaultSettings::getComboBoxWidthMin()
{
  return m_windowMainComboBoxWidthMin;
}

CoordSettings DefaultSettings::getCoordSettings()
{
  return m_coordSettings;
}

PointSetStyle DefaultSettings::getCurveStyle(int pointSetIndex)
{
  if (pointSetIndex >= (int) m_curveStyles.count())
  {
    // invent defaults since entry does not exist for this index in the list
    PointSetStyle style;

    style.pointShape = (PointShape) (pointSetIndex % MaxPointShape);
    style.pointSize = PointSize4;
    style.pointLineSize = PointLineSize2;
    style.pointLineColor = DigitBlue;
    style.pointInColor = DigitBlue;
    style.lineSize = LineSize1;
    style.lineColor = DigitBlue;
    style.lineConnectAs = SingleValuedFunction;

    return style;
  }
  else
    return m_curveStyles [pointSetIndex];
}

DiscretizeSettings DefaultSettings::getDiscretizeSettings()
{
  return m_discretizeSettings;
}

double DefaultSettings::getDoubleMin()
{
  return m_doubleMin;
}

ExportSettings DefaultSettings::getExportSettings()
{
  return m_exportSettings;
}

GridSet DefaultSettings::getGridDisplayGridSetX()
{
  return m_gridDisplayGridSetX;
}

GridSet DefaultSettings::getGridDisplayGridSetY()
{
  return m_gridDisplayGridSetY;
}

GridRemovalSettings DefaultSettings::getGridRemovalSettings()
{
  return m_gridRemovalSettings;
}

QSize DefaultSettings::getHandleSize()
{
  return m_handleSize;
}

int DefaultSettings::getKeyClipboard()
{
  return m_keyClipboard;
}

int DefaultSettings::getKeyTree()
{
  return m_keyTree;
}

int DefaultSettings::getKeyVariables()
{
  return m_keyVariables;
}

int DefaultSettings::getMaxCommands()
{
  return m_maxCommands;
}

PointSetStyle DefaultSettings::getMeasureStyle(int pointSetIndex)
{
  if (pointSetIndex >= (int) m_measureStyles.count())
  {
    // invent defaults since entry does not exist for this index in the list
    PointSetStyle style;

    const int OFFSET = 1; // offset to prevent first curve and first measure from looking alike
    style.pointShape = (PointShape) ((pointSetIndex + OFFSET) % MaxPointShape);
    style.pointSize = PointSize4;
    style.pointLineSize = PointLineSize2;
    style.pointLineColor = DigitBlue;
    style.pointInColor = DigitBlue;
    style.lineSize = LineSize1;
    style.lineColor = DigitBlue;
    style.lineConnectAs = SingleValuedFunction;

    return style;
  }
  else
    return m_measureStyles [pointSetIndex];
}

int DefaultSettings::getMruMax()
{
  return m_mruMax;
}

QStringList DefaultSettings::getMruDocuments()
{
  return m_mruDocuments;
}

int DefaultSettings::getPointMatchHighlightDiameter()
{
  return m_pointMatchHighlightDiameter;
}

int DefaultSettings::getPointMatchHighlightLineSize()
{
  return m_pointMatchHighlightLineSize;
}

PointMatchSettings DefaultSettings::getPointMatchSettings()
{
  return m_pointMatchSettings;
}

int DefaultSettings::getPowerMostSigMax()
{
  return m_powerMostSigMax;
}

int DefaultSettings::getPowerMostSigMin()
{
  return m_powerMostSigMin;
}

PointSetStyle DefaultSettings::getScaleStyle()
{
  return m_scaleStyle;
}

int DefaultSettings::getScanForAxesEndpointKernelWidth()
{
  return m_scanForAxesEndpointKernelWidth;
}

Color DefaultSettings::getScanForAxesLineColor()
{
  return m_scanForAxesLineColor;
}

int DefaultSettings::getScanForAxesLineWidth()
{
  return m_scanForAxesLineWidth;
}

bool DefaultSettings::getScanForAxesScanEffect()
{
  return m_scanForAxesScanEffect;
}

int DefaultSettings::getSegmentPointMinSeparation()
{
  return m_segmentPointMinSeparation;
}

SegmentSettings DefaultSettings::getSegmentSettings()
{
  return m_segmentSettings;
}

SessionsSettings DefaultSettings::getSessionsSettings()
{
  return m_sessionsSettings;
}

BackgroundSelection DefaultSettings::getViewBackground()
{
  return m_viewBackground;
}

bool DefaultSettings::getViewCurveGeometry()
{
  return m_viewCurveGeometry;
}

bool DefaultSettings::getViewDigitizeCurvePointsToolbar()
{
  return m_viewDigitizeCurvePointsToolbar;
}

bool DefaultSettings::getViewDigitizeMeasurePointsToolbar()
{
  return m_viewDigitizeMeasurePointsToolbar;
}

bool DefaultSettings::getViewFileToolbar()
{
  return m_viewFileToolbar;
}

bool DefaultSettings::getViewImageScaleToolbar()
{
  return m_viewImageScaleToolbar;
}

bool DefaultSettings::getViewMeasureGeometry()
{
  return m_viewMeasureGeometry;
}

ViewPointSelection DefaultSettings::getViewPoints()
{
  return m_viewPoints;
}

bool DefaultSettings::getViewSelectToolbar()
{
  return m_viewSelectToolbar;
}

bool DefaultSettings::getViewStatusBar()
{
  return m_viewStatusBar;
}

bool DefaultSettings::getViewZoomToolbar()
{
  return m_viewZoomToolbar;
}
   
QPoint DefaultSettings::getWindowCurvePosition()
{
  return m_windowCurvePosition;
}

QSize DefaultSettings::getWindowCurveSize()
{
  return m_windowCurveSize;
}

QPoint DefaultSettings::getWindowHelpPosition()
{
  return m_windowHelpPosition;
}

QSize DefaultSettings::getWindowHelpSize()
{
  return m_windowHelpSize;
}

QString DefaultSettings::getWindowMainFontName()
{
  return m_windowMainFontName;
}

bool DefaultSettings::getWindowMainFontOverride()
{
  return m_windowMainFontOverride;
}

int DefaultSettings::getWindowMainFontSize()
{
  return m_windowMainFontSize;
}

QPoint DefaultSettings::getWindowMainPosition()
{
  return m_windowMainPosition;
}

QSize DefaultSettings::getWindowMainSize()
{
  return m_windowMainSize;
}
QPoint DefaultSettings::getWindowMeasurePosition()
{
  return m_windowMeasurePosition;
}

QSize DefaultSettings::getWindowMeasureSize()
{
  return m_windowMeasureSize;
}

void DefaultSettings::initializeSettingsToFactoryDefaults()
{
  // default for session settings is to not save them since that is easier for
  // beginners to understand. advanced users, who will most benefit from saved
  // settings, must turn on this feature
  m_sessionsSettings.sessions = true;
  m_sessionsSettings.coordinates = false;
  m_sessionsSettings.axes = false;
  m_sessionsSettings.scaleBar = false;
  m_sessionsSettings.curves = false;
  m_sessionsSettings.segments = false;
  m_sessionsSettings.pointMatch = false;
  m_sessionsSettings.measures = false;
  m_sessionsSettings.discretize = false;
  m_sessionsSettings.gridRemoval = false;
  m_sessionsSettings.gridDisplay = false;
  m_sessionsSettings.exporting = false;
  m_sessionsSettings.viewSelections = false;
  // first document will start up in segment state. this will gently push the user towards
  // using the most efficient method of digitizing, which is what the user really cares about.
  // the user will be reminded elsewhere that axes points must be generated
  m_sessionsSettings.initialDigitizeState = SegmentState;
  
  m_coordSettings.frame = Cartesian;
  m_coordSettings.originRadius = 0.0;
  m_coordSettings.xThetaScale = Linear;
  m_coordSettings.yRScale = Linear;
  m_coordSettings.thetaUnits = ThetaDegrees;

  m_exportSettings.pointsSelection = XFromAllCurves;
  m_exportSettings.layout = AllCurvesOnEachLine;
  m_exportSettings.delimiters = Commas;
  m_exportSettings.header = HeaderSimple;

  m_viewFileToolbar = true;
  m_viewSelectToolbar = true;
  m_viewImageScaleToolbar = true;
  m_viewDigitizeCurvePointsToolbar = true;
  m_viewDigitizeMeasurePointsToolbar = true;
  m_viewZoomToolbar = true;
  m_viewStatusBar = true;
  m_viewPoints = ViewAllPoints;
  m_viewBackground = ProcessedImage;
  m_viewCurveGeometry = false;
  m_viewMeasureGeometry = false;
  
  m_powerMostSigMax = 6;
  m_powerMostSigMin = -4;

  m_doubleMin = 1.0e-12;

  m_maxCommands = 100;

  // initial width+x should be less than 800 pixels to fit low resolution displays, and
  // initial height+y should be less than 640 pixels to fit low resolution displays
  m_windowCurveSize = QSize(180, 280);
  m_windowCurvePosition = QPoint(40, 40);

  // initial width+x should be less than 800 pixels to fit low resolution displays, and
  // initial height+y should be less than 640 pixels to fit low resolution displays
  m_windowHelpSize = QSize(640, 700);
  m_windowHelpPosition = QPoint(40, 40);

  // initial width+x should be less than 800 pixels to fit low resolution displays, and
  // initial height+y should be less than 640 pixels to fit low resolution displays
  m_windowMainSize = QSize(700, 500);
  m_windowMainPosition = QPoint(40, 40);
  m_windowMainFontOverride = false;
  m_windowMainFontName = "Helvetica";
  m_windowMainFontSize = 12;

  // initial width+x should be less than 800 pixels to fit low resolution displays, and
  // initial height+y should be less than 640 pixels to fit low resolution displays
  m_windowMeasureSize = QSize(180, 280);
  m_windowMeasurePosition = QPoint(40, 40);

  m_keyClipboard = Qt::Key_C;
  m_keyTree = Qt::Key_T;
  m_keyVariables = Qt::Key_V;

  m_handleSize = QSize(6, 6);

  m_segmentPointMinSeparation = 4;
  m_segmentSettings.minPoints = 2;
  m_segmentSettings.pointSeparation = 10;
  m_segmentSettings.fillCorners = false; // false for smoothly curving graphs, true for piecewise linear graphs
  m_segmentSettings.lineSize = LineSize3;
  m_segmentSettings.lineColor = DigitGreen;

  m_gridRemovalSettings.removeThinLines = false;
  m_gridRemovalSettings.thinThickness = 2.0; // two pixels thick
  m_gridRemovalSettings.removeGridlines = false;
  m_gridRemovalSettings.gridMesh.gridSetX = AllButStep;
  m_gridRemovalSettings.gridMesh.gridSetY = AllButStep;
  m_gridRemovalSettings.gridDistance = 4.0; // two pixels on either side of the infinitely thin gridline
  m_gridRemovalSettings.removeColor = false;
  m_gridRemovalSettings.color = Qt::green; // do NOT use black, or black-on-white images will turn completely white
  m_gridRemovalSettings.foregroundThresholdLow = 0; // same as corresponding discretize value below
  m_gridRemovalSettings.foregroundThresholdHigh = 10; // same as corresponding discretize value below
  m_gridRemovalSettings.gapSeparation = 6.0; // greater than RemovalThinThickness times two

  m_gridDisplayGridSetX = AllButStep;
  m_gridDisplayGridSetY = AllButStep;

  m_pointMatchHighlightDiameter = 26;
  m_pointMatchHighlightLineSize = 3;
  m_pointMatchSettings.pointSeparation = 15;
  m_pointMatchSettings.pointSize = 60;
  m_pointMatchSettings.acceptedColor = DigitGreen;
  m_pointMatchSettings.rejectedColor = DigitRed;
  m_pointMatchSettings.candidateColor = DigitBlue; // synchronize with DigitDoc::openDocument

  m_discretizeSettings.discretizeMethod = DiscretizeIntensity;
  m_discretizeSettings.intensityThresholdLow = 0;
  m_discretizeSettings.intensityThresholdHigh = 50;
  m_discretizeSettings.foregroundThresholdLow = 0; // same as corresponding grid removal value above
  m_discretizeSettings.foregroundThresholdHigh = 10; // same as corresponding grid removal value above
  m_discretizeSettings.hueThresholdLow = 180;
  m_discretizeSettings.hueThresholdHigh = 360;
  m_discretizeSettings.saturationThresholdLow = 50;
  m_discretizeSettings.saturationThresholdHigh = 100;
  m_discretizeSettings.valueThresholdLow = 0;
  m_discretizeSettings.valueThresholdHigh = 50;

  m_axesStyle.pointShape = Cross;
  m_axesStyle.pointSize = PointSize4;
  m_axesStyle.pointLineSize = PointLineSize2;
  m_axesStyle.pointLineColor = DigitRed;
  m_axesStyle.pointInColor = DigitRed;
  m_axesStyle.lineSize = LineSize1;
  m_axesStyle.lineColor = DigitTransparent;
  m_axesStyle.lineConnectAs = SingleValuedFunction;

  m_scaleStyle.pointShape = Cross;
  m_scaleStyle.pointSize = PointSize4;
  m_scaleStyle.pointLineSize = PointLineSize2;
  m_scaleStyle.pointLineColor = DigitMagenta;
  m_scaleStyle.pointInColor = DigitMagenta;
  m_scaleStyle.lineSize = LineSize1;
  m_scaleStyle.lineColor = DigitMagenta;
  m_scaleStyle.lineConnectAs = SingleValuedFunction;

  // m_curveStyles defaults are not created here since the number that will be needed in the
  // future is unknown at startup. all available curve styles are loaded from archival
  // during startup, and then more are generated by getCurveStyle if necessary

  // m_measureStyles defaults are not created here since the number that will be needed in the
  // future is unknown at startup. all available measure styles are loaded from archival
  // during startup, and then more are generated by getMeasureStyle if necessary

  // most recently used document list starts out empty
  m_mruMax = 4;

  m_windowMainComboBoxWidthMin = 120;

  m_scanForAxesLineColor = DigitCyan;
  m_scanForAxesLineWidth = 2; // same width as convolution performed to recognize axes
  m_scanForAxesEndpointKernelWidth = 40; // substantially bigger than 2, which would pick up the first
                                         // on-pixel adjacent to an off-pixel, but smaller than the
                                         // width of a realistic axis line. an endpoint is expected to
                                         // have width/2 successive white or black pixels, followed by width/2
                                         // successive black or white pixels
  m_scanForAxesScanEffect = true; // enable scan effect so user can see that program is working, how
                                  // it is working, and when it is done working. penalty is that
                                  // redraws needed for scan effect make algorithm slow on some computers
}

void DefaultSettings::loadArchivedSettings()
{
  // in linux this archiver must be a local variable or else file is not saved. in
  // windows any calls to insertSearchPath or setPath either cancel cmdSettingsText,
  // or they cause multiple files to be written out (one per group)
  QSettings archiver;

  m_sessionsSettings.sessions = archiver.readBoolEntry(SESSIONS_SESSIONS,
    m_sessionsSettings.sessions);

  if (m_sessionsSettings.sessions)
  {
    m_sessionsSettings.coordinates = archiver.readBoolEntry(SESSIONS_COORDINATES,
      m_sessionsSettings.coordinates);
    m_sessionsSettings.axes = archiver.readBoolEntry(SESSIONS_AXES,
      m_sessionsSettings.axes);
    m_sessionsSettings.scaleBar = archiver.readBoolEntry(SESSIONS_SCALEBAR,
      m_sessionsSettings.scaleBar);
    m_sessionsSettings.curves = archiver.readBoolEntry(SESSIONS_CURVES,
      m_sessionsSettings.curves);
    m_sessionsSettings.segments = archiver.readBoolEntry(SESSIONS_SEGMENTS,
      m_sessionsSettings.segments);
    m_sessionsSettings.pointMatch = archiver.readBoolEntry(SESSIONS_POINTMATCH,
      m_sessionsSettings.pointMatch);
    m_sessionsSettings.measures = archiver.readBoolEntry(SESSIONS_MEASURES,
      m_sessionsSettings.measures);
    m_sessionsSettings.discretize = archiver.readBoolEntry(SESSIONS_DISCRETIZE,
      m_sessionsSettings.discretize);
    m_sessionsSettings.gridRemoval = archiver.readBoolEntry(SESSIONS_GRIDREMOVAL,
      m_sessionsSettings.gridRemoval);
    m_sessionsSettings.gridDisplay = archiver.readBoolEntry(SESSIONS_GRIDDISPLAY,
      m_sessionsSettings.gridDisplay);
    m_sessionsSettings.exporting = archiver.readBoolEntry(SESSIONS_EXPORTING,
      m_sessionsSettings.exporting);
    m_sessionsSettings.viewSelections = archiver.readBoolEntry(SESSIONS_VIEWSELECTIONS,
      m_sessionsSettings.viewSelections);
    m_sessionsSettings.initialDigitizeState = (DigitizeState) archiver.readNumEntry(SESSIONS_INITIALDIGITIZESTATE,
      m_sessionsSettings.initialDigitizeState);
  }
  
  if (m_sessionsSettings.sessions && m_sessionsSettings.coordinates)
  {
    m_coordSettings.frame = (ReferenceFrame) archiver.readNumEntry(COORD_FRAME,
      m_coordSettings.frame);
    m_coordSettings.originRadius = archiver.readDoubleEntry(COORD_ORIGINRADIUS,
      m_coordSettings.originRadius);
    m_coordSettings.xThetaScale = (Scale) archiver.readNumEntry(COORD_XTHETASCALE,
      m_coordSettings.xThetaScale);
    m_coordSettings.yRScale = (Scale) archiver.readNumEntry(COORD_YRSCALE,
      m_coordSettings.yRScale);
    m_coordSettings.thetaUnits = (ThetaUnits) archiver.readNumEntry(COORD_THETAUNITS,
      m_coordSettings.thetaUnits);
  }

  if (m_sessionsSettings.sessions && m_sessionsSettings.exporting)
  {
    m_exportSettings.pointsSelection = (ExportPointsSelection) archiver.readNumEntry(EXPORT_POINTSSELECTION,
      m_exportSettings.pointsSelection);
    m_exportSettings.layout = (ExportLayout) archiver.readNumEntry(EXPORT_LAYOUT,
      m_exportSettings.layout);
    m_exportSettings.delimiters = (ExportDelimiters) archiver.readNumEntry(EXPORT_DELIMITERS,
      m_exportSettings.delimiters);
    m_exportSettings.header = (ExportHeader) archiver.readNumEntry(EXPORT_HEADER,
      m_exportSettings.header);
  }

  if (m_sessionsSettings.sessions && m_sessionsSettings.viewSelections)
  {
    m_viewFileToolbar = archiver.readBoolEntry(VIEW_FILETOOLBAR,
      m_viewFileToolbar);
    m_viewSelectToolbar = archiver.readBoolEntry(VIEW_SELECTTOOLBAR,
      m_viewSelectToolbar);
    m_viewImageScaleToolbar = archiver.readBoolEntry(VIEW_IMAGESCALETOOLBAR,
      m_viewImageScaleToolbar);
    m_viewDigitizeCurvePointsToolbar = archiver.readBoolEntry(VIEW_DIGITIZECURVEPOINTSTOOLBAR,
      m_viewDigitizeCurvePointsToolbar);
    m_viewDigitizeMeasurePointsToolbar = archiver.readBoolEntry(VIEW_DIGITIZEMEASUREPOINTSTOOLBAR,
      m_viewDigitizeMeasurePointsToolbar);
    m_viewZoomToolbar = archiver.readBoolEntry(VIEW_ZOOMTOOLBAR,
      m_viewZoomToolbar);
    m_viewStatusBar = archiver.readBoolEntry(VIEW_STATUSBAR,
      m_viewStatusBar);
    m_viewPoints = (ViewPointSelection) archiver.readNumEntry(VIEW_POINTS,
      m_viewPoints);
    m_viewBackground = (BackgroundSelection) archiver.readNumEntry(VIEW_BACKGROUND,
      m_viewBackground);
    m_viewCurveGeometry = archiver.readBoolEntry(VIEW_CURVEGEOMETRY,
      m_viewCurveGeometry);
    m_viewMeasureGeometry = archiver.readBoolEntry(VIEW_MEASUREGEOMETRY,
      m_viewMeasureGeometry);
  }
  
  m_powerMostSigMax = archiver.readNumEntry(MATH_POWERMOSTSIGMAX,
    m_powerMostSigMax);
  m_powerMostSigMin = archiver.readNumEntry(MATH_POWERMOSTSIGMIN,
    m_powerMostSigMin);
  m_doubleMin = archiver.readDoubleEntry(MATH_DOUBLEMIN,
    m_doubleMin);
  m_maxCommands = archiver.readNumEntry(MATH_MAXCOMMANDS,
    m_maxCommands);

  m_windowCurveSize.setWidth(archiver.readNumEntry(WINDOW_CURVE_WIDTH,
    m_windowCurveSize.width()));
  m_windowCurveSize.setHeight(archiver.readNumEntry(WINDOW_CURVE_HEIGHT,
    m_windowCurveSize.height()));
  m_windowCurvePosition.setX(archiver.readNumEntry(WINDOW_CURVE_X,
    m_windowCurvePosition.x()));
  m_windowCurvePosition.setY(archiver.readNumEntry(WINDOW_CURVE_Y,
    m_windowCurvePosition.y()));

  m_windowHelpSize.setWidth(archiver.readNumEntry(WINDOW_HELP_WIDTH,
    m_windowHelpSize.width()));
  m_windowHelpSize.setHeight(archiver.readNumEntry(WINDOW_HELP_HEIGHT,
    m_windowHelpSize.height()));
  m_windowHelpPosition.setX(archiver.readNumEntry(WINDOW_HELP_X,
    m_windowHelpPosition.x()));
  m_windowHelpPosition.setY(archiver.readNumEntry(WINDOW_HELP_Y,
    m_windowHelpPosition.y()));

  m_windowMainSize.setWidth(archiver.readNumEntry(WINDOW_MAIN_WIDTH,
    m_windowMainSize.width()));
  m_windowMainSize.setHeight(archiver.readNumEntry(WINDOW_MAIN_HEIGHT,
    m_windowMainSize.height()));
  m_windowMainPosition.setX(archiver.readNumEntry(WINDOW_MAIN_X,
    m_windowMainPosition.x()));
  m_windowMainPosition.setY(archiver.readNumEntry(WINDOW_MAIN_Y,
    m_windowMainPosition.y()));
  m_windowMainFontOverride = archiver.readBoolEntry(WINDOW_MAIN_FONT_OVERRIDE,
    m_windowMainFontOverride);
  m_windowMainFontName = archiver.readEntry(WINDOW_MAIN_FONT_NAME,
    m_windowMainFontName);
  m_windowMainFontSize = archiver.readNumEntry(WINDOW_MAIN_FONT_SIZE,
    m_windowMainFontSize);
  m_windowMainComboBoxWidthMin = archiver.readNumEntry(WINDOW_MAIN_COMBOBOXWIDTHMIN,
    m_windowMainComboBoxWidthMin);

  m_windowMeasureSize.setWidth(archiver.readNumEntry(WINDOW_MEASURE_WIDTH,
    m_windowMeasureSize.width()));
  m_windowMeasureSize.setHeight(archiver.readNumEntry(WINDOW_MEASURE_HEIGHT,
    m_windowMeasureSize.height()));
  m_windowMeasurePosition.setX(archiver.readNumEntry(WINDOW_MEASURE_X,
    m_windowMeasurePosition.x()));
  m_windowMeasurePosition.setY(archiver.readNumEntry(WINDOW_MEASURE_Y,
    m_windowMeasurePosition.y()));
  
  m_keyClipboard = archiver.readNumEntry(KEY_CLIPBOARD, m_keyClipboard);
  m_keyTree = archiver.readNumEntry(KEY_TREE, m_keyTree);
  m_keyVariables = archiver.readNumEntry(KEY_VARIABLES, m_keyVariables);

  m_handleSize.setWidth(archiver.readNumEntry(HANDLE_WIDTH,
    m_handleSize.width()));
  m_handleSize.setHeight(archiver.readNumEntry(HANDLE_HEIGHT,
    m_handleSize.height()));

  if (m_sessionsSettings.sessions && m_sessionsSettings.segments)
  {
    m_segmentPointMinSeparation = archiver.readNumEntry(SEGMENT_POINTMINSEPARATION,
      m_segmentPointMinSeparation);
    m_segmentSettings.minPoints = archiver.readNumEntry(SEGMENT_MINPOINTS,
      m_segmentSettings.minPoints);
    m_segmentSettings.pointSeparation = archiver.readNumEntry(SEGMENT_POINTDEFAULTSEPARATION,
      m_segmentSettings.pointSeparation);
    m_segmentSettings.fillCorners = archiver.readBoolEntry(SEGMENT_FILLCORNERS,
      m_segmentSettings.fillCorners);
    m_segmentSettings.lineSize = (LineSize) archiver.readNumEntry(SEGMENT_LINESIZE,
      m_segmentSettings.lineSize);
    m_segmentSettings.lineColor = (Color) archiver.readNumEntry(SEGMENT_LINECOLOR,
      m_segmentSettings.lineColor);
  }

  if (m_sessionsSettings.sessions && m_sessionsSettings.gridRemoval)
  {
    m_gridRemovalSettings.removeThinLines = archiver.readBoolEntry(GRID_REMOVAL_REMOVETHINLINES,
      m_gridRemovalSettings.removeThinLines);
    m_gridRemovalSettings.thinThickness = archiver.readDoubleEntry(GRID_REMOVAL_THINTHICKNESS,
      m_gridRemovalSettings.thinThickness);
    m_gridRemovalSettings.removeGridlines = archiver.readBoolEntry(GRID_REMOVAL_REMOVEGRIDLINES,
      m_gridRemovalSettings.removeGridlines);
    m_gridRemovalSettings.gridMesh.gridSetX = (GridSet) archiver.readNumEntry(GRID_REMOVAL_GRIDSETX,
      m_gridRemovalSettings.gridMesh.gridSetX);
    m_gridRemovalSettings.gridMesh.gridSetY = (GridSet) archiver.readNumEntry(GRID_REMOVAL_GRIDSETY,
      m_gridRemovalSettings.gridMesh.gridSetY);
    m_gridRemovalSettings.gridDistance = archiver.readDoubleEntry(GRID_REMOVAL_GRIDDISTANCE,
      m_gridRemovalSettings.gridDistance);
    m_gridRemovalSettings.removeColor = archiver.readBoolEntry(GRID_REMOVAL_REMOVECOLOR,
      m_gridRemovalSettings.removeColor);
    int r = archiver.readNumEntry(GRID_REMOVAL_COLOR_RED, m_gridRemovalSettings.color.red());
    int g = archiver.readNumEntry(GRID_REMOVAL_COLOR_GREEN, m_gridRemovalSettings.color.green());
    int b = archiver.readNumEntry(GRID_REMOVAL_COLOR_BLUE, m_gridRemovalSettings.color.blue());
    m_gridRemovalSettings.color = QColor(r, g, b);
    m_gridRemovalSettings.foregroundThresholdLow = archiver.readNumEntry(GRID_REMOVAL_FOREGROUNDTHRESHOLDLOW,
      m_gridRemovalSettings.foregroundThresholdLow);
    m_gridRemovalSettings.foregroundThresholdHigh = archiver.readNumEntry(GRID_REMOVAL_FOREGROUNDTHRESHOLDHIGH,
      m_gridRemovalSettings.foregroundThresholdHigh);
    m_gridRemovalSettings.gapSeparation = archiver.readDoubleEntry(GRID_REMOVAL_GAPSEPARATION,
      m_gridRemovalSettings.gapSeparation);
  }

  if (m_sessionsSettings.sessions && m_sessionsSettings.gridDisplay)
  {
    m_gridDisplayGridSetX = (GridSet) archiver.readNumEntry(GRID_DISPLAY_GRIDSETX,
      m_gridDisplayGridSetX);
    m_gridDisplayGridSetY = (GridSet) archiver.readNumEntry(GRID_DISPLAY_GRIDSETY,
      m_gridDisplayGridSetY);
  }

  if (m_sessionsSettings.sessions && m_sessionsSettings.pointMatch)
  {
    m_pointMatchHighlightDiameter = archiver.readNumEntry(POINTMATCH_HIGHLIGHTDIAMETER,
      m_pointMatchHighlightDiameter);
    m_pointMatchHighlightLineSize = archiver.readNumEntry(POINTMATCH_HIGHLIGHTLINESIZE,
      m_pointMatchHighlightLineSize);
    m_pointMatchSettings.pointSeparation = archiver.readNumEntry(POINTMATCH_SEPARATIONDEFAULT,
      m_pointMatchSettings.pointSeparation);
    m_pointMatchSettings.pointSize = archiver.readNumEntry(POINTMATCH_SIZEDEFAULT,
      m_pointMatchSettings.pointSize);
    m_pointMatchSettings.acceptedColor = (Color) archiver.readNumEntry(POINTMATCH_ACCEPTEDCOLOR,
      m_pointMatchSettings.acceptedColor);
    m_pointMatchSettings.rejectedColor = (Color) archiver.readNumEntry(POINTMATCH_REJECTEDCOLOR,
      m_pointMatchSettings.rejectedColor);
    m_pointMatchSettings.candidateColor = (Color) archiver.readNumEntry(POINTMATCH_CANDIDATECOLOR,
      m_pointMatchSettings.candidateColor);
  }

  if (m_sessionsSettings.sessions && m_sessionsSettings.discretize)
  {
    m_discretizeSettings.discretizeMethod = (DiscretizeMethod) archiver.readNumEntry(DISCRETIZE_METHODDEFAULT,
      m_discretizeSettings.discretizeMethod);
    m_discretizeSettings.intensityThresholdLow = archiver.readNumEntry(DISCRETIZE_INTENSITY_THRESHOLDLOW,
      m_discretizeSettings.intensityThresholdLow);
    m_discretizeSettings.intensityThresholdHigh = archiver.readNumEntry(DISCRETIZE_INTENSITY_THRESHOLDHIGH,
      m_discretizeSettings.intensityThresholdHigh);
    m_discretizeSettings.foregroundThresholdLow = archiver.readNumEntry(DISCRETIZE_FOREGROUND_THRESHOLDLOW,
      m_discretizeSettings.foregroundThresholdLow);
    m_discretizeSettings.foregroundThresholdHigh = archiver.readNumEntry(DISCRETIZE_FOREGROUND_THRESHOLDHIGH,
      m_discretizeSettings.foregroundThresholdHigh);
    m_discretizeSettings.hueThresholdLow = archiver.readNumEntry(DISCRETIZE_HUE_THRESHOLDLOW,
      m_discretizeSettings.hueThresholdLow);
    m_discretizeSettings.hueThresholdHigh = archiver.readNumEntry(DISCRETIZE_HUE_THRESHOLDHIGH,
      m_discretizeSettings.hueThresholdHigh);
    m_discretizeSettings.saturationThresholdLow = archiver.readNumEntry(DISCRETIZE_SATURATION_THRESHOLDLOW,
      m_discretizeSettings.saturationThresholdLow);
    m_discretizeSettings.saturationThresholdHigh = archiver.readNumEntry(DISCRETIZE_SATURATION_THRESHOLDHIGH,
      m_discretizeSettings.saturationThresholdHigh);
    m_discretizeSettings.valueThresholdLow = archiver.readNumEntry(DISCRETIZE_VALUE_THRESHOLDLOW,
      m_discretizeSettings.valueThresholdLow);
    m_discretizeSettings.valueThresholdHigh = archiver.readNumEntry(DISCRETIZE_VALUE_THRESHOLDHIGH,
      m_discretizeSettings.valueThresholdHigh);
  }

  if (m_sessionsSettings.sessions && m_sessionsSettings.axes)
  {
    loadPointSetStyle(m_axesStyle,
      archiver,
      POINTSET_AXES_POINTSHAPE,
      POINTSET_AXES_POINTSIZE,
      POINTSET_AXES_POINTLINESIZE,
      POINTSET_AXES_POINTLINECOLOR,
      POINTSET_AXES_POINTINCOLOR,
      POINTSET_AXES_LINESIZE,
      POINTSET_AXES_LINECOLOR,
      POINTSET_AXES_LINECONNECTAS);
  }

  if (m_sessionsSettings.sessions && m_sessionsSettings.scaleBar)
  {
    loadPointSetStyle(m_scaleStyle,
      archiver,
      POINTSET_SCALE_POINTSHAPE,
      POINTSET_SCALE_POINTSIZE,
      POINTSET_SCALE_POINTLINESIZE,
      POINTSET_SCALE_POINTLINECOLOR,
      POINTSET_SCALE_POINTINCOLOR,
      POINTSET_SCALE_LINESIZE,
      POINTSET_SCALE_LINECOLOR,
      POINTSET_SCALE_LINECONNECTAS);
  }

  if (m_sessionsSettings.sessions && m_sessionsSettings.curves)
  {
    loadPointSetStyles(true,
      m_curveStyles,
      archiver,
      POINTSET_CURVE_GROUP,
      POINTSET_CURVE_POINTSHAPE,
      POINTSET_CURVE_POINTSIZE,
      POINTSET_CURVE_POINTLINESIZE,
      POINTSET_CURVE_POINTLINECOLOR,
      POINTSET_CURVE_POINTINCOLOR,
      POINTSET_CURVE_LINESIZE,
      POINTSET_CURVE_LINECOLOR,
      POINTSET_CURVE_LINECONNECTAS);
  }

  if (m_sessionsSettings.sessions && m_sessionsSettings.measures)
  {
    loadPointSetStyles(false,
      m_measureStyles,
      archiver,
      POINTSET_MEASURE_GROUP,
      POINTSET_MEASURE_POINTSHAPE,
      POINTSET_MEASURE_POINTSIZE,
      POINTSET_MEASURE_POINTLINESIZE,
      POINTSET_MEASURE_POINTLINECOLOR,
      POINTSET_MEASURE_POINTINCOLOR,
      POINTSET_MEASURE_LINESIZE,
      POINTSET_MEASURE_LINECOLOR,
      POINTSET_MEASURE_LINECONNECTAS);
  }

  m_mruMax = archiver.readNumEntry(MRU_LIST_MAX,
    m_mruMax);
  loadMruDocuments(archiver);

  m_scanForAxesLineColor = (Color) archiver.readNumEntry(SCANFORAXES_LINECOLOR,
    m_scanForAxesLineColor);
  m_scanForAxesLineWidth = archiver.readNumEntry(SCANFORAXES_LINEWIDTH,
    m_scanForAxesLineWidth);
  m_scanForAxesEndpointKernelWidth = archiver.readNumEntry(SCANFORAXES_ENDPOINTKERNELWIDTH,
    m_scanForAxesEndpointKernelWidth);
  m_scanForAxesScanEffect = archiver.readBoolEntry(SCANFORAXES_SCANEFFECT,
    m_scanForAxesScanEffect);
}

void DefaultSettings::loadMruDocuments(QSettings& rArchiver)
{
  QStringList subkeyList = rArchiver.subkeyList(MRU_LIST_DOCUMENTS_GROUP);
  QStringList::Iterator itr;
  int indexExpected = 0;
  for (itr = subkeyList.begin(); itr != subkeyList.end(); ++itr)
  {
    // convert subkey into numeric index. subkey must equal the expected value or else
    // entry is considered corrupted
    bool ok;
    int indexFound = (*itr).toInt(&ok);
    if (ok && (indexExpected++ == indexFound))
    {
      QString doc = rArchiver.readEntry(MRU_LIST_DOCUMENTS.arg(indexFound));
      m_mruDocuments.append(doc);
    }
  }
}

void DefaultSettings::loadPointSetStyle(PointSetStyle& rStyle,
  QSettings& rArchiver,
  const QString keyPointShape,
  const QString keyPointSize,
  const QString keyPointLineSize,
  const QString keyPointLineColor,
  const QString keyPointInColor,
  const QString keyLineSize,
  const QString keyLineColor,
  const QString keyLineConnectAs)
{
  rStyle.pointShape = (PointShape) rArchiver.readNumEntry(keyPointShape, rStyle.pointShape);
  rStyle.pointSize = (PointSize) rArchiver.readNumEntry(keyPointSize, rStyle.pointSize);
  rStyle.pointLineSize = (PointLineSize) rArchiver.readNumEntry(keyPointLineSize, rStyle.pointLineSize);
  rStyle.pointLineColor = (Color) rArchiver.readNumEntry(keyPointLineColor, rStyle.pointLineColor);
  rStyle.pointInColor = (Color) rArchiver.readNumEntry(keyPointInColor, rStyle.pointInColor);
  rStyle.lineSize = (LineSize) rArchiver.readNumEntry(keyLineSize, rStyle.lineSize);
  rStyle.lineColor = (Color) rArchiver.readNumEntry(keyLineColor, rStyle.lineColor);
  rStyle.lineConnectAs = (LineConnectAs) rArchiver.readNumEntry(keyLineConnectAs, rStyle.lineConnectAs);
}

void DefaultSettings::loadPointSetStyles(bool isCurve,
  PointSetStyleList& rStyleList,
  QSettings& rArchiver,
  const QString keyGroupPrefix,
  const QString keyPointShape,
  const QString keyPointSize,
  const QString keyPointLineSize,
  const QString keyPointLineColor,
  const QString keyPointInColor,
  const QString keyLineSize,
  const QString keyLineColor,
  const QString keyLineConnectAs)
{
  QStringList subkeyList = rArchiver.subkeyList(keyGroupPrefix);
  QStringList::Iterator itr;
  int indexExpected = 0;
  for (itr = subkeyList.begin(); itr != subkeyList.end(); ++itr)
  {
    // convert subkey into numeric index. subkey must equal the expected value or else
    // entry is considered corrupted
    bool ok;
    int indexFound = (*itr).toInt(&ok);
    if (ok && (indexExpected++ == indexFound))
    {
      PointSetStyle style;

      if (isCurve)
        style = getCurveStyle(indexFound);
      else
        style = getMeasureStyle(indexFound);

      loadPointSetStyle(style,
        rArchiver,
        QString(keyPointShape).arg(indexFound),
        QString(keyPointSize).arg(indexFound),
        QString(keyPointLineSize).arg(indexFound),
        QString(keyPointLineColor).arg(indexFound),
        QString(keyPointInColor).arg(indexFound),
        QString(keyLineSize).arg(indexFound),
        QString(keyLineColor).arg(indexFound),
        QString(keyLineConnectAs).arg(indexFound));

      rStyleList.append(style);
    }
  }
}

void DefaultSettings::setComboBoxWidthMin(int comboBoxWidthMin)
{
  m_windowMainComboBoxWidthMin = comboBoxWidthMin;
}

void DefaultSettings::setCoordSettings(CoordSettings coord)
{
  m_coordSettings.frame = coord.frame;
  m_coordSettings.originRadius = coord.originRadius;
  m_coordSettings.xThetaScale = coord.xThetaScale;
  m_coordSettings.yRScale = coord.yRScale;
  m_coordSettings.thetaUnits = coord.thetaUnits;
}

void DefaultSettings::setCurveStyles(const PointSetStyleList& rStyles)
{
  m_curveStyles = rStyles;
}

void DefaultSettings::setDiscretizeSettings(DiscretizeSettings discretize)
{
  m_discretizeSettings = discretize; 
}

void DefaultSettings::setExportSettings(ExportSettings exportSettings)
{
  m_exportSettings.pointsSelection = exportSettings.pointsSelection;
  m_exportSettings.layout = exportSettings.layout;
  m_exportSettings.delimiters = exportSettings.delimiters;
  m_exportSettings.header = exportSettings.header;
}

void DefaultSettings::setGridRemovalSettings(GridRemovalSettings grid)
{
  m_gridRemovalSettings = grid;
}

void DefaultSettings::setGridDisplaySettings(GridMeshSettings grid)
{
  m_gridDisplayGridSetX = grid.gridSetX;
  m_gridDisplayGridSetY = grid.gridSetY;
}

void DefaultSettings::setMeasureStyles(const PointSetStyleList& rStyles)
{
  m_measureStyles = rStyles;
}

void DefaultSettings::setMruDocuments(const QStringList& rMruDocuments)
{
  m_mruDocuments = rMruDocuments;
}

void DefaultSettings::setViewBackground(BackgroundSelection view)
{
  m_viewBackground = view;
}

void DefaultSettings::setViewCurveGeometry(bool view)
{
  m_viewCurveGeometry = view;
}

void DefaultSettings::setViewDigitizeCurvePointsToolbar(bool view)
{
  m_viewDigitizeCurvePointsToolbar = view;
}

void DefaultSettings::setViewDigitizeMeasurePointsToolbar(bool view)
{
  m_viewDigitizeMeasurePointsToolbar = view;
}

void DefaultSettings::setViewFileToolbar(bool view)
{
  m_viewFileToolbar = view;
}

void DefaultSettings::setViewImageScaleToolbar(bool view)
{
  m_viewImageScaleToolbar = view;
}

void DefaultSettings::setViewMeasureGeometry(bool view)
{
  m_viewMeasureGeometry = view;
}

void DefaultSettings::setViewPoints(ViewPointSelection view)
{
  m_viewPoints = view;
}

void DefaultSettings::setViewSelectToolbar(bool view)
{
  m_viewSelectToolbar = view;
}

void DefaultSettings::setViewStatusBar(bool view)
{
  m_viewStatusBar = view;
}

void DefaultSettings::setViewZoomToolbar(bool view)
{
  m_viewZoomToolbar = view;
}

void DefaultSettings::setWindowCurvePosition(QPoint pos)
{
  m_windowCurvePosition = pos;
}

void DefaultSettings::setWindowCurveSize(QSize size)
{
  m_windowCurveSize = size;
}

void DefaultSettings::setWindowHelpPosition(QPoint pos)
{
  m_windowHelpPosition = pos;
}

void DefaultSettings::setWindowHelpSize(QSize size)
{
  m_windowHelpSize = size;
}

void DefaultSettings::setWindowMainPosition(QPoint pos)
{
  m_windowMainPosition = pos;
}

void DefaultSettings::setWindowMainSize(QSize size)
{
  m_windowMainSize = size;
}

void DefaultSettings::setPointMatchSettings(PointMatchSettings match)
{
  m_pointMatchSettings = match;
}

void DefaultSettings::setWindowMeasurePosition(QPoint pos)
{
  m_windowMeasurePosition = pos;
}

void DefaultSettings::setWindowMeasureSize(QSize size)
{
  m_windowMeasureSize = size;
}

void DefaultSettings::setSegmentSettings(SegmentSettings seg)
{
  m_segmentSettings = seg;
}

void DefaultSettings::setAxesStyle(PointSetStyle style)
{
  m_axesStyle = style;
}

void DefaultSettings::setScaleStyle(PointSetStyle style)
{
  m_scaleStyle = style;
}

void DefaultSettings::setSessionsSettings(SessionsSettings sessionsSettings)
{
  m_sessionsSettings = sessionsSettings;
}

     
