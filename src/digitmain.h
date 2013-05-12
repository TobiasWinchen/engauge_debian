/***************************************************************************
                          digitmain.h  -  description
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

#ifndef DIGITMAIN_H
#define DIGITMAIN_H

#include <qapplication.h>
#include <qaction.h>
#include <qworkspace.h>
#include <qstatusbar.h>
#include <qtoolbutton.h>
#include <qmessagebox.h>
#include <qprinter.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qcombobox.h>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <QMainWindow>
#include <QLabel>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QResizeEvent>
#include <QEvent>
#include <QMoveEvent>
#include <QToolBar>
#include <QMdiArea>
#include <QFileDialog>

#include "digitdef.h"
#include "geometrywindowitem.h"
#include "mrudocuments.h"

class DigitDoc;
class DigitView;
class CurveCmb;
class MeasureCmb;
class PointSets;
class ZoomCmb;
class GeometryDlg;

// this class owns the main window, including the menus, toolbars, status bar
class DigitMain : public QMainWindow
{
  Q_OBJECT
  
  public:
  
    DigitMain(QString exportFileAtShutdown);
    ~DigitMain();

    // imports an image file specified by commandline option or dialog box
    void importImageFile(const QString file); // import
    void importImageFile(const QString file,
      double xMinG, double xMaxG,
      double yMinG, double yMaxG); // import and isolate axes
    
    // opens a document file specified by commandline option or dialog box
    void openDocumentFile(const QString file);

  protected:

    // query before exiting since user has unsaved changes
    bool queryExit();
    
    // event filter to catch close events for MDI child windows and is installed in
    // createClient() on every child window. Closing a window calls the eventFilter
    // first which removes the view from the connected documents' view list. If the
    // last view is going to be closed, the eventFilter() tests if the document is
    // modified; if yes, it asks the user to save the document. If the document has
    // never been saved, slotFileSaveAs() gets called to get a save name and path.
    virtual bool eventFilter(QObject* object, QEvent* event);

    // main window move and resize events are caught and the geometry is saved
    virtual void moveEvent(QMoveEvent* pEvent);
    virtual void resizeEvent(QResizeEvent* pEvent);

    // support import by dragging an image
    virtual void dragEnterEvent(QDragEnterEvent* pEvent);
    virtual void dropEvent(QDropEvent* pEvent);

    // creates a new child window. The document that will be connected to it
    // has to be created before and the instances filled, with e.g. openDocument(),
    // followed by a call createClient() to get a new MDI child window.
    void createClient(DigitDoc* doc);

  public slots:

    // update geometry info of curve and measure curves if they have been modified
    void slotUpdateGeometry(void);

    // update paste-as-new menu option if clipboard contents change
    void slotClipboardChanged();
    
    // recent file name was chosen in mru popup menu. this is not invoked by fileOpenRecent menu option
    void slotFileOpenRecent(QString filename);
        
  private slots:

    // generate a new document in the actual view
    void slotFileImport();
    // open a document
    void slotFileOpen();
    // save a document
    void slotFileSave();
    // save a document under a different filename
    void slotFileSaveAs();
    // close the actual file
    void slotFileClose();
    // export to text file
    void slotFileExport();
    // export to text file under a different filename
    void slotFileExportAs();
    // print the actual file
    void slotFilePrint();
    // exits the application
    void slotFileExit();

    // undo last action
    void slotEditUndo();
    // redo next action
    void slotEditRedo();
    // put the marked text/object into the clipboard and remove it from the document
    void slotEditCut();
    // put the marked text/object into the clipboard
    void slotEditCopy();
    // paste the clipboard into the document
    void slotEditPaste();
    // import image in the clipboard as new document
    void slotEditPasteAsNew();
        
    // toggle the file toolbar
    void slotViewFileToolBar(bool toggle);
    // toggle the select toolbar
    void slotViewSelectToolBar(bool toggle);
    // toggle the image scale toolbar
    void slotViewImageScaleToolBar(bool toggle);
    // toggle the digitize curve points toolbar
    void slotViewDigitizeCurvePointsToolBar(bool toggle);
    // toggle the digitize measure points toolbar
    void slotViewDigitizeMeasurePointsToolBar(bool toggle);
    // toggle the zoom toolbar
    void slotViewZoomToolBar(bool toggle);
    // toggle the statusbar
    void slotViewStatusBar(bool toggle);
    // toggle showing only axes points
    void slotViewAxesPoints(bool toggle);
    // toggle showing only scale bar points
    void slotViewScalePoints(bool toggle);
    // toggle showing only active curve points
    void slotViewCurvePoints(bool toggle);
    // toggle showing only measure points
    void slotViewMeasurePoints(bool toggle);
    // toggle showing all points
    void slotViewAllPoints(bool toggle);
    // no image in background
    void slotViewNoBackground(bool toggle);
    // original image in background
    void slotViewOriginalImage(bool toggle);
    // processed image in background
    void slotViewProcessedImage(bool toggle);
    // toggle the gridlines display
    void slotViewGridDisplay(bool toggle);
    // toggle the curve geometry dialog box
    void slotViewCurveGeometry(bool toggle);
    // toggle the measure geometry dialog box
    void slotViewMeasureGeometry(bool toggle);

    // coordinate system settings
    void slotSettingsCoordSys();
    // axes settings
    void slotSettingsAxes();
    // scale bar settings
    void slotSettingsScale();
    // curves settings
    void slotSettingsCurves();
    // segments settings
    void slotSettingsSegments();
    // point match settings
    void slotSettingsPointMatch();
    // measure settings
    void slotSettingsMeasures();
    // grid removal settings
    void slotSettingsGridRemoval();
    // grid display settings
    void slotSettingsGridDisplay();
    // discretize settings
    void slotSettingsDiscretize();
    // setup export
    void slotSettingsExportSetup();
    // sessions settings
    void slotSettingsSessions();
    
    // change to select mode
    void slotDigitizeSelect(bool toggle);
    // change to axis point mode
    void slotDigitizeAxis(bool toggle);
    // change to scale bar mode
    void slotDigitizeScale(bool toggle);
    // change to curve point mode
    void slotDigitizeCurve(bool toggle);
    // change to segment fill mode
    void slotDigitizeSegment(bool toggle);
    // change to point match mode
    void slotDigitizePointMatch(bool toggle);
    // change to measure mode
    void slotDigitizeMeasure(bool toggle);

    // creates a new view for the current document
    void slotWindowNewWindow();
    // shows an about dialog
    void slotHelpAbout();
    // shows user manual in browser
    void slotHelpUserManual();
    // shows glossary in browser
    void slotHelpGlossary();
    // shows date/time converter in browser
    void slotConverter();
    // shows manual line graph tutorial in browser
    void slotHelpLineGraphTutorial();
    // shows manual point graph tutorial in browser
    void slotHelpPointGraphTutorial();
    // shows map tutorial in browser
    void slotHelpMapTutorial();
    // display normal message in statusbar
    void slotStatusNormalMsg(const QString &text);
    // display permanent message in statusbar
    void slotStatusPermanentMsg(const QString &text);
    // display temporary message in statusbar
    void slotStatusTemporaryMsg(const QString &text);
    // display tracking information in statusbar
    void slotStatusTrackingMsg(const QString &textCoords, const QString &textRes);
    // gets called when the window menu is activated; recreates the window menu with all opened window titles
    void slotMenuPreactivated();
    // activates the MDI child widget when it gets selected from the window menu
    void slotMenuActivated(int id);

    // receive selection from combobox containing list of pointsets
    void slotCurveComboActivated(int);
    void slotMeasureComboActivated(int);

    // selection just changed
    void slotSelectionChange(void);

    // transformation just transitioned from invalid to valid, or vice versa
    void slotTransformationChange(void);

    // hide curve or measure pointset geometry info windows
    void slotHideCurveGeometry(void);
    void slotHideMeasureGeometry(void);
    
  private:

    // imports an image received by drag and drop
    void importImageDragged(const QImage& image);

    // imports a pixmap received by drag and drop
    void importPixmapDragged(const QPixmap& pixmap);
    
    // initialize all QActions of the application
    void initActions();
    // initialize the geometry info windows
    void initGeometryInfo();
    // initMenuBar creates the menu_bar and inserts the menuitems
    void initMenuBar();
    // setup the statusbar
    void initStatusBar();
    // setup the file and digitize toolbars
    void initToolBars();
    // setup the mainview
    void initView();

    // perform the first initialization steps of a new document
    DigitDoc* newDocument(void);

    // turn off the digitize buttons. when toggleAction turns on a button that is still
    // on from a previous document, the signal gets dropped which skips necessary initialization
    void resetActions(void);

    // attach curveCmb combobox to document. use zero to detach current document
    void setCurveCmb(DigitDoc* doc);

    // attach measureCmb combobox to document. use zero to detach current document
    void setMeasureCmb(DigitDoc* doc);

    // replace either curves or measures pointsets in document, using list of
    // points that have orphaned points
    void replacePointSets(DigitDoc* doc, PointSets* copyPointSets,
      QStringList &pointSetsWithOrphanedPoints, bool replacingCurves);

    // the document that owns the currently active DigitView
    DigitDoc* activeDocument(void);

    // remove and delete specified document
    void removeDocument(DigitDoc* doc);

    // the currently active DigitView. setting the active view also sets the active document
    DigitView* activeView(void);
    void setActiveView(DigitView* view);

    // return true if view pointer points to valid view
    bool findView(DigitView* view);

    // save current state to all documents, and current curveCmb and measureCmb selections to active document
    void setDigitizeState(DigitizeState digitizeState);

    // event handling for specific events
    bool eventClose(QObject* object, QEvent* event);
    bool eventKeyPress(QObject* object, QEvent* event);

    // toggle menu_bar and toolbar actions depending on active document state
    void toggleActions(void);

    // toggle menu_bar edit actions
    void toggleActionsEdit(void);

    // paste-as-new availability depends on external clipboard so it is updated separately
    void toggleActionPasteAsNew(void);

    // display geometry info of currently active curve or measure
    void displayCurveGeometry(GeometryWindowItemList& rInfo);
    void displayMeasureGeometry(GeometryWindowItemList& rInfo);
    void noGeometryInfo(void);

    // spawn help browser
    void spawnHelp(QString helpHome);

    // move focus to specified view if defined, otherwise first view of specified doc
    void focusOnViewOrDoc(DigitView* view, DigitDoc* doc);

    // only quit if there are no unsaved changes, or user does not care
    bool safeToQuit();
    bool canCloseFrame(DigitDoc* doc, DigitView* view);

    // save document and return true if operation was canceled
    bool documentSave(DigitDoc* doc);
    bool documentSaveAs(DigitDoc* doc);

    // help directory
    QDir helpPath();

    // one or more file names may be specified as file:/home/dir/file with linefeed delimiters
    bool clipboardContainsUris();
    
    // file menu contains all items of the menubar entry "File"
    QMenu* fileMenu;
    // edit menu contains all items of the menubar entry "Edit"
    QMenu* editMenu;
    // view menu contains all items of the menubar entry "View"
    QMenu* viewMenu;
    // digitize menu contains all items of the menubar entry "Digitize"
    QMenu* digitizeMenu;
    // settings menu contains all items of the menubar entry "Settings"
    QMenu* settingsMenu;
    // view menu contains all items of the menubar entry "Help"
    QMenu* helpMenu;
    // window menu
    QMenu* windowMenu;

    QToolBar* fileToolbar;
    QToolBar* selectToolbar;
    QToolBar* imageScaleToolbar;
    QToolBar* digitizeCurvePointsToolbar;
    QToolBar* digitizeMeasurePointsToolbar;    
    QToolBar* zoomToolbar;

    // comboboxes owned by digitizeCurvePointsToolbar and digitizeMeasurePointsToolbar respectively
    CurveCmb* curveCmb;
    MeasureCmb* measureCmb;

    // combobox owned by zoomToolbar
    ZoomCmb* zoomCmb;

    // workspace is the MDI frame widget that handles MDI child widgets
    QMdiArea* workspace;
    // the printer instance
    QPrinter* printer;
    // a list of all open documents. If the last window of a document gets closed, the installed
    // eventFilter removes this document from the list. The document list is checked for modified
    // documents when the user is about to close the application
    Q3PtrList<DigitDoc>* docList;

    QAction* fileImport;
    QAction* fileOpen;
    QMenu* fileOpenRecent;
    QAction* fileSave;
    QAction* fileSaveAs;
    QAction* fileClose;
    QAction* fileExport;
    QAction* fileExportAs;
    QAction* filePrint;
    QAction* fileExit;

    QAction* editUndo;
    QAction* editRedo;
    QAction* editCut;
    QAction* editCopy;
    QAction* editPaste;
    QAction* editPasteAsNew;

    QAction* viewFileToolBar;
    QAction* viewSelectToolBar;
    QAction* viewImageScaleToolBar;
    QAction* viewDigitizeCurvePointsToolBar;
    QAction* viewDigitizeMeasurePointsToolBar;
    QAction* viewZoomToolBar;
    QAction* viewStatusBar;
    QActionGroup* viewPointsAction;
    QAction* viewAxesPoints;
    QAction* viewScalePoints;
    QAction* viewCurvePoints;
    QAction* viewMeasurePoints;
    QAction* viewAllPoints;
    QActionGroup* viewBackgroundAction;
    QAction* viewNoBackground;
    QAction* viewOriginalImage;
    QAction* viewProcessedImage;
    QAction* viewGridDisplay;
    QAction* viewCurveGeometry;
    QAction* viewMeasureGeometry;

    QActionGroup* digitizeAction;
    QAction* digitizeSelect;
    QAction* digitizeAxis;
    QAction* digitizeScale;
    QAction* digitizeCurve;
    QAction* digitizeSegment;
    QAction* digitizePointMatch;
    QAction* digitizeMeasure;

    QAction* settingsCoordSys;
    QAction* settingsAxes;
    QAction* settingsScale;
    QAction* settingsCurves;
    QAction* settingsSegments;
    QAction* settingsPointMatch;
    QAction* settingsMeasures;
    QAction* settingsDiscretize;
    QAction* settingsGridRemoval;
    QAction* settingsGridDisplay;
    QAction* settingsExportSetup;
    QAction* settingsSessions;
         
    QActionGroup* windowAction;
    QAction* windowNewWindow;
    QAction* windowTile;
    QAction* windowCascade;

    QAction* helpAboutApp;
    QAction* helpUserManual;
    QAction* helpGlossary;
    QAction* helpConverter;
    QAction* helpLineGraphTutorial;
    QAction* helpPointGraphTutorial;
    QAction* helpMapTutorial;

    // status bar perment message, coordinates and resolution
    QLabel* statusNormal;
    QLabel* statusPermanent;
    QLabel* statusCoords;
    QLabel* statusRes;

    // active view currently owns keyboard focus
    DigitView* m_activeView;
    
    // geometry information windows are always alive and accepting information, but only sometimes visible
    GeometryDlg* curveGeometryDlg;
    GeometryDlg* measureGeometryDlg;

    // maintain the list of most recently used documents
    MruDocuments m_mruDocuments;

    // optionally export file at shutdown if string is not empty
    QString m_exportFileAtShutdown;
};
#endif // DIGITMAIN_H

