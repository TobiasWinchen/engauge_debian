/***************************************************************************
                          digitdoc.h  -  description
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

#ifndef DIGITDOC_H
#define DIGITDOC_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qobject.h>
#include <qstring.h>
#include <q3canvas.h>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <QList>
#include <QPixmap>
#include <QPolygon>

#include "pointset.h"
#include "digitdef.h"
#include "digitview.h"
#include "mmsubs.h"

class PointSets;
class Segment;
class Segments;
class MatchSet;
class Transform;
class Grid;

const QString extensionCsv(".csv");
const QString extensionDig(".dig");

// this class provides a document object for in the document-view model.
//
// there are a lot of helper classes since the document has so many important duties
// that a single monolithic class would be unwieldy
class DigitDoc : public QObject
{
  Q_OBJECT

  friend class DigitView;

  public:

    DigitDoc(DigitizeState state);
    ~DigitDoc();

    // imported documents need to be setup with defaults
    void initDefaults();

    // adds a view to the document which represents the document contents
    void addView(DigitView* view);
    
    // removes a view from the list of currently connected views
    void removeView(DigitView* view);

    // gets called if a view is removed or added
    void changedViewList();

    // returns the first view instance
    DigitView* firstView(){ return m_viewList->first(); };

    // returns true, if the requested view is the last view of the document
    bool isLastView();

    // return true if view pointer points to valid view
    bool findView(DigitView* view);

    // modified flag for the document is true after document has been modified
    void setModified(bool modified = true) { m_modified = modified; };
    bool modified() { return m_modified; };

    // return true if there is an action to undo or redo
    bool isActionToUndo();
    bool isActionToRedo();
    
    // edit operations
    void undo();
    void redo();
    void cut(DigitView* view);
    void copy(DigitView* view);
    void paste(DigitView* view);

    // initializes a new document by importing an image file
    bool importFile(const QString &filename, const char* format = 0);

    // initializes a new document by importing a dragged image
    bool importImage(const QImage &image);    

    // initializes a new document by importing a dragged pixmap
    bool importPixmap(const QPixmap &pixmap);
    
    // loads the document and emits the updateViews() signal
    bool openDocument(const QString &filename);

    // saves the document
    bool saveDocument(const QString &filename);

    // exports the document
    bool exportDocument(const QString &filename);

    // sets the file name for the saved and exported files
    void setSavePath(const QString &filename);
    void setExportPath(const QString &filename);

    // returns the file names of the current document file and the last exported file
    const QString &savePath() const { return m_savePath; }
    const QString &exportPath() const { return m_exportPath; }

    // sets the filename of the document
    void setTitle(const QString &title) { m_title = title; }

    // returns the title of the document
    const QString &title() const { return m_title; }

    // returns the saved flag
    bool isExistingSaveFile(void) const { return m_saveFileExists; }

    // returns the exported flag
    bool isExistingExportFile(void) const { return m_exportFileExists; }

    // marking a file as exported is useful when automatically exporting, so
    // that the file will be saved without a save file dialog box appearing
    void markFileAsExported() { m_exportFileExists = true; }

    // canvasviews can access the document canvas
    Q3Canvas* canvas() { return (Q3Canvas*) &m_canvas; }

    // returns pointsets container belonging to this document
    const PointSets* pointSets(void) { return m_pointSets; }

    // replace current pointset list with the specified pointset list. both lists are optional
    void replacePointSets(PointSets* pointSets,
      QStringList* curvesWithOrphanedPoints,
      QStringList* measuresWithOrphanedPoints);
      
    // show cursor location in the statusbar, after converting from screen to graph coordinates
    void trackCursor(const QPoint &);

    // in point match mode, look for candidate match point under cursor
    void highlightCandidateMatchPoint(const QPoint &);
    
    // last curve choice in combobox. this persists across sessions
    QString curveCmbText();
    void setCurveCmbText(QString curveCmbText);

    // last measure choice in combobox. this persists across sessions
    QString measureCmbText();
    void setMeasureCmbText(QString measureCmbText);
    
    // get curve and measure names from pointset list. these will be shown in combobox
    void curveNames(QStringList* list);
    void measureNames(QStringList* list);

    // returns the number of axis and scale bar points
    int axisPointCount(void);
    int scalePointCount(void);

    // return whether or not the axis (need 3 points) or scale bar (need 2 points) points form
    // a valid transformation
    bool validAxes(void);
    bool validScale(void);

    // return whether or not the axis point is consistent with log (sometimes) or linear (always) coordinates
    bool validAxisPoint(double xTheta, double yR);

    // convert screen into graph coordinates, using screen/graph, cartesian/polar, linear/log transformations
    void screenToXThetaYR(int xScreen, int yScreen, double* xTheta, double* yR);

    // forward normal message to statusbar
    void forwardNormalMsg(QString text);

    // forward permanent message to statusbar
    void forwardPermanentMsg(QString text);

    // all documents are kept in the same digitize state
    DigitizeState digitizeState(void) { return m_digitizeState; }
    void setDigitizeState(DigitizeState state);
        
    // add axis or curve point, depending on state. axis point graph coordinates are set later by
    // setAxisPoint so user can see where the new axis point lies while editing the graph coordinatees
    Point* addPoint(int xScreen, int yScreen);

    // add axis point without user intervention
    void addAxisPoint(int xScreen, int yScreen, double xGraph, double yGraph);

    // change graph coordinates of an axis or scale bar point
    void setAxisPoint(Point* p, double xThetaGraph, double yRGraph);
    void setScalePoint(Point* p, double x, double y);

    // remove axis or scale bar point during its creation since user canceled editing of its graph coordinates
    void removeGestatingPoint(Point* p);

    // get and set methods for coordinates settings
    CoordSettings coordSettings(void) { return m_coordSettings; }
    void setCoordSettings(CoordSettings settings);
    
    // get and set methods for pointset styles of axes, scale bar, curve and matchset pointsets
    PointSetStyle axesStyle(void);
    PointSetStyle curveStyle(QString name);
    PointSetStyle scaleStyle(void);
    void setAxesStyle(PointSetStyle pointSetStyle);
    void setMatchSetStyle(PointSetStyle pointSetStyle);
    void setScaleStyle(PointSetStyle pointSetStyle);

    // get and set methods for export settings
    ExportSettings exportSettings(void) { return m_exportSettings; }
    void setExportSettings(ExportSettings settings) { m_modified = true; m_exportSettings = settings; }

    // get and set methods for grid removal line settings
    void setGridRemovalSettings(GridRemovalSettings settings);
    GridRemovalSettings gridRemovalSettings(void) { return m_gridRemovalSettings; }

    // get and set methods for discretization settings
    void setDiscretizeSettings(DiscretizeSettings settings);
    DiscretizeSettings discretizeSettings(void) { return m_discretizeSettings; }
    
    // get and set methods for grid and grid line settings
    bool gridDisplay(void);
    void showGridDisplay(bool toggle);
    void setGridDisplaySettings(GridMeshSettings settings, bool show);
    GridMeshSettings gridDisplaySettings(void) { return m_gridDisplaySettings; }

    // get and set methods for segment settings
    void setSegmentSettings(SegmentSettings settings);
    SegmentSettings segmentSettings(void) { return m_segmentSettings; }

    // get and set methods for point match settings
    void setPointMatchSettings(PointMatchSettings settings);
    PointMatchSettings pointMatchSettings(void) { return m_pointMatchSettings; }

    // transform. it is set internally using the axes points
    const Transform* transform(void) { return m_transform; }

    // update transformation, grid display, and graph coordinates after a move
    void finishMove(void);

    // print document
    void print(void);

    // forward notice of selection list change to main window
    void selectionListChanged(void);

    // match the sample point under the specified cursor position, returning false if there
    // is no point there
    bool matchSamplePoint(QPoint p);

    // add accepted, add rejected or remove one match point
    void matchPointAccept(void);
    void matchPointReject(void);
    void matchPointRemove(void);

    // selected points for viewing set and get methods
    void setViewPointSelection(ViewPointSelection viewPointSelection);
    ViewPointSelection viewPointSelection(void);

    // background set and get methods
    void setBackground(BackgroundSelection selection);
    BackgroundSelection background(void);

    // canvas bounds rectangle
    QRect bounds(void) { return QRect(QPoint(0, 0), m_canvas.size()); }

    // geometry info for the active curve or measure pointset. return true if info has changed
    bool geometryInfoCurve(GeometryWindowItemList& rInfo);
    bool geometryInfoMeasure(GeometryWindowItemList& rInfo);

    // send updateGeometry signal
    void sendUpdateGeometry(void);

    // is specified file version supported?
    bool supportedVersionNumber(QString versionString);

    // return original and processed images for copying and modifying externally
    const QImage &originalImage(void) { return m_originalImage; }
    const QPixmap &originalPixmap(void) { return m_originalPixmap; }
    const QImage &processedImage(void) { return m_processedImage; }
    const QPixmap &processedPixmap(void) { return m_processedPixmap; }

    // filters for opening and closing files. these are not document specific
    static QString filterImport();
    static QString filterOpenSave();

    // thumbnails for digitize toolbar comboboxes
    QPixmap curveThumbnail(QString name);
    QPixmap measureThumbnail(QString name);

    // scan for x and y axes in newly imported document. this is only invoked from
    // the command line because external scripts can, in some cases, fill in the
    // axes geometry information. the x axis is assumed to be near the bottom of the
    // image and the y axis is assumed to be near the left side of the image
    void scanForAxes(double xMinG, double xMaxG, double yMinG, double yMaxG);

    // for current view only, override the document's view state to show points in the
    // selected curve. every call to startShowOnlySelectedCurve must be followed by a 
    // call to stopShowOnlySelectedCurve
    void startShowOnlySelectedCurve();
    void stopShowOnlySelectedCurve();

    // debug
    void dump(void);

  signals:

    // send normal message to statusbar
    void normalMsg(const QString &text);

    // send permanent message to statusbar
    void permanentMsg(const QString &text);

    // send temporary message to statusbar
    void temporaryMsg(const QString &text);

    // send tracking message to statusbar
    void trackingMsg(const QString &textCoords, const QString &textRes);

    // selection just changed
    void selectionChange(void);

    // transformation transitioned from valid to invalid, or vice versa
    void transformationChange(void);

    // update geometry info of curve and measure pointsets if they have been modified. forwarded to DigitMain
    void updateGeometry(void);

  public slots:

    // calls repaint() on all views connected to the document object and is called by
    // the view that changed the document. as this view normally repaints itself, it
    // is excluded from the paintEvent
    void slotUpdateViews(DigitView* sender, QRect boundingRect);

  private:

    enum
    {
      MagicNumber = 0xcafe
    };

    // helper class that wraps around QCanvas to enable hiding background pixmap. initially the
    // pixmap was left in place and the QCanvas::drawBackground calls were intercepted, but this
    // left garbage after (1) foreground objects disappeared, and (2) scrolls
    class DigitCanvas : public Q3Canvas
    {
      public:

        DigitCanvas(DigitDoc* doc) : m_doc(doc) {}
        ~DigitCanvas() {}

        // set and get methods for state
        void setBackground(BackgroundSelection selection);

        // background update
        void updateBackground(void);
        
      private:

        DigitDoc* m_doc;
    };

    // display context-sensitive cursors
    void setViewCursor(DigitView* view);

    // load original image into canvas during import or open
    void loadOriginalPixmap(void);
    
    // remove gridlines from original image and discretize, then break processed image into segments
    void processOriginalPixmap(void);

    // compute pleasing grid display line settings, returning true if successful
    void initGridMesh(GridMeshSettings* gridSettings);
    void initGridRemovalMesh(void);
    void initGridDisplayMesh(void);

    // show segments if flag is set
    void showSegments(bool show);
    
    // update the axes and scale bar transformations, and use whichever works. if there is incomplete
    // (insufficient points) or inconsistent information (collocated or collinear points), this exits
    // gracefully. if successful, all curve and measure point coordinates are updated and the grid lines are moved
    void computeTransformation(void);

    // rebuild the grid display lines, usually just after updating the transformation, but also after
    // modifying the grid display settings. if there are not enough axis points, or the axis points are
    // badly defined, this exits gracefully
    void makeGridDisplayLines(void);

    // update screen regions described by rectangles
    void updateFromList(QList<QRect>* updateRectList);

    // tell all views to add or remove a sample match point
    void addSampleMatchPointToViews(void);
    void removeSampleMatchPointFromViews(void);

    // cleanup for state transition from point match to another state
    void cleanUpAcceptedPoints(DigitizeState state);
    void collectAcceptedPoints(QList<QPoint>* acceptedPoints);
    void addAcceptedPoints(QList<QPoint>* acceptedPoints);
    void clearAcceptedPoints(void);

    // pointsets owned by this document. each pointset has any number of points
    PointSets* m_pointSets;

    // segments that have been scanned in from the original image
    Segments* m_segments;

    // match points produced and used in point match mode. this is cleared when leaving point match mode
    MatchSet* m_matchSet;

    // gridlines for display. gridlines for removal are implicit in m_processedPixmap
    Grid* m_gridDisplay;

    // the dirty flag of the current document
    bool m_modified;
    QString m_title;
    QString m_savePath; // this is set to import path with appropriate file extension as default during import
    QString m_exportPath;

    // the first save after import is confirmed so user can select a new directory and edit the filename
    bool m_saveFileExists;

    // the first export after import is confirmed so user can select a new directory and edit the filename
    bool m_exportFileExists;

    // the list of the views currently connected to the document
    Q3PtrList<DigitView>* m_viewList;

    // points selected for viewing
    ViewPointSelection m_viewPointSelection;

    // used to stash m_viewPointSelection during temporary overrides by startShowOnlySelectedCurve
    bool m_inViewPointSelectionOverride; // state machine with two states allows only safe transitions
    ViewPointSelection m_viewPointSelectionSaved;
    
    // original scanned image is processed by gridline removal code, and available for m_canvas display
    // (in either original or processed form).
    //
    // m_originalImage and m_processedImage are QImage versions of m_originalPixmap and
    // m_processedPixmap, and should not be considered a waste of memory since
    // QImages are created anyway during the processing that creates the pixmaps
    BackgroundSelection m_backgroundSelection;
    QPixmap m_originalPixmap;
    QImage m_originalImage;
    QPixmap m_processedPixmap;
    QImage m_processedImage;
    
    // canvas is composed of scanned image (either original or discretized) as the
    // background, and drawn points
    DigitCanvas m_canvas;
    
    // digitize state is determined by the digitize toolbar
    DigitizeState m_digitizeState;

    // for the active document, these mirror the contents of the curve and measure combobox
    QString m_curveCmbText;
    QString m_measureCmbText;

    // coordinates settings
    CoordSettings m_coordSettings;

    // export settings determine the format of exported files
    ExportSettings m_exportSettings;

    // grid removal line settings
    GridRemovalSettings m_gridRemovalSettings;

    // grid display line settings
    GridMeshSettings m_gridDisplaySettings;

    // segment settings
    SegmentSettings m_segmentSettings;

    // point match settings
    PointMatchSettings m_pointMatchSettings;

    // transform owned by this document
    Transform* m_transform;
   
    // discretize settings
    DiscretizeSettings m_discretizeSettings;
    
    // true if axes or scale transformation is out of date. without
    // these flags, the transformations would be recomputed at least
    // once every event (including focusIn). since any popup warning
    // made by the transformation code causes a focusIn event while
    // returning to the main window, an infinite loop would otherwise
    // occur
    bool m_dirtyAxesTransformation;
    bool m_dirtyScaleTransformation;

    // candidate sample point for point match, represented as a set of pixels in original image
    QPolygon m_samplePointPixels;
};

#endif // DIGITDOC_H
