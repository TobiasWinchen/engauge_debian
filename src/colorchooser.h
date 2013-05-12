/***************************************************************************
                          colorchooser.h  -  description
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

#ifndef COLORCHOOSER_H
#define COLORCHOOSER_H

#include <qlineedit.h>
#include <qlabel.h>
#include <qstring.h>
#include <qvalidator.h>
#include <q3canvas.h>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <QPixmap>
#include <QMouseEvent>
#include <QProgressDialog>

#include "digitdef.h"

// this set of widgets presents a histogram of color attribute values,
// with one or two draggable limit dividers overlaying the histogram.
// a scale below the histogram pictorially shows the color attribute spectrum.
// one or two edit boxes below allow keyboard editing of the limits
class ColorChooser : public QObject
{
  Q_OBJECT
  
  public:
  
    ColorChooser(QWidget* parent, const QImage* imageOriginal,
      int xTopLeft, int yTopLeft, QRgb rgbBg, bool showLarge);
    ~ColorChooser();

    enum
    {
      ChooserWidth = 202
    };

    enum
    {
      ChooserPadding = 4 // padding on each side of histogram (needed so first and last points are visible)
    };

    enum
    {
      ChooserFrame = 3 // pixels used up by frame
    };

    enum
    {
      ChooserFullHeight = 110
    };

    enum
    {
      EditWidth = 80
    };

    enum
    {
      ScaleFullHeight = 20
    };

    enum
    {
      Separation = 3
    };

    enum
    {
      TabWidth = 32
    };

    enum
    {
      TabHeight = 20
    };

    // change background color, and update foreground histogram
    void changeBackgroundColor(const QImage* imageOriginal, QRgb rgbBg);

    // put limit into edit box
    void changeTextLimit(bool lower, int limit);

    // chooser height depends on the format
    int chooserHeight(void) { return m_chooserHeight; }

    // attribute maximum depends on which color attribute is chosen
    int colorAttributeMax(void);
  
    // set discretize method, loading appropriate background histogram image. the limits
    // are not relevant if the method is DiscretizeNone since that state is equivalent
    // to setEnabled(false). all other states are equivalent to setEnabled(true)
    void setDiscretizeMethod(DiscretizeMethod method, int limitLower = 0, int limitUpper = 0);

    // update grayout areas to correspond to current limits
    void updateGrayouts();
    
    // histogram divider class. one divider is lower bound, other divider is upper bound.
    // this class should be private but Microsoft compiler will choke if it is
    class Divider
    {
      public:
        Divider(ColorChooser* chooser, Q3Canvas* canvas, bool lower);
        ~Divider() { }

        // set horizontal position of vertical line, returning true if position changed
        void setLimit(int limit);
        void moveToScreenPosition(double xScreen);
        int screenPosition(void);
            
        // process a click-and-drag by dragging lower or upper limit a little in x direction
        void dragStart(int xCursor);
        void dragContinue(int xCursor);

        // show or hide this divider
        void setVisible(bool show);
        bool visible(void) { return m_line->visible(); }
        
        // tab is derived from QCanvasRectangle since QCanvasPixmap is NOT derived from QCanvasItem
        class Tab : public Q3CanvasRectangle
        {
          public:
            Tab(ColorChooser* chooser, Divider* divider, Q3Canvas* canvas, bool lower);
            ~Tab() {}

            void moveToScreenPosition(double xScreen);

            Divider* divider() { return m_divider; }

            // show or hide this tab
            void setVisible(bool show);
            bool visible(void) { return Q3CanvasRectangle::visible(); }

            enum
            {
              RttiTab = 123456 // large random value over 1000
            };
            
            virtual int rtti() const { return RttiTab; }

          protected:
            virtual void drawShape(QPainter &p);

          private:
            ColorChooser* m_chooser;
            Divider* m_divider;
            Q3Canvas* m_canvas;
            QPixmap* m_pixmap;
            bool m_lower;
        };
                        
      private:

        ColorChooser* m_chooser;
        Q3Canvas* m_canvas;
        bool m_lower; // true for lower bound, false for upper bound
        Q3CanvasLine* m_line;
        Tab* m_tab;

        // for any value of the limit, there are multiple possible screen positions. when
        // the limit is changed in an edit box, the screen position is updated accordingly.
        // when the divider is dragged, the screen position changes in one-pixel increments,
        // and the limit is updated accordingly
        int m_limit;
        double m_xScreen;

        // cursor distance from center of tab when mouse was pressed
        double m_xScreenOffset;
    };

  signals:
    void limitChange(bool lower, int limit);

  public slots:

    void slotHigh(const QString &);
    void slotLow(const QString &);

  private:
    
    // refresh a single histogram
    bool refreshHistogram(Q3PtrList<Q3CanvasLine>* histogram, DiscretizeMethod method);

    // build foreground pixmap
    void loadForegroundPixmap(QRgb rgbBg);
    
    // creates histograms from original image
    void loadHistogram(const QImage* imageOriginal, DiscretizeMethod method,
      int colorAttributeMax, Q3PtrList<Q3CanvasLine>* histogram, QRgb rgbBg,
      QProgressDialog* dlg);
    void loadHistograms(const QImage* imageOriginal);
    void showCurrentHistogram(void);

    // send limit to parent class using signal
    void passLimitToParent(bool lower, int limit);
  
    class ClickableView : public Q3CanvasView
    {
      public:
        ClickableView(Q3Canvas* canvas, QWidget* parent);
        ~ClickableView() {}
      
      private:
        void contentsMouseMoveEvent(QMouseEvent* e);
        void contentsMousePressEvent(QMouseEvent* e);
        void contentsMouseReleaseEvent(QMouseEvent* e);

        bool dragging;

        // screen offset from center of tab
        int draggingOffset;
        Divider::Tab* draggedTab;
    };

    QWidget* m_parent;
    
    // widgets for displaying the attribute histogram, and the adjustable boundary lines
    Q3Canvas* chooserCanvas;
    ClickableView* chooserCanvasView;
    Divider* dividerLower;
    Divider* dividerUpper;
    Q3PtrList<Q3CanvasLine> intensityHistogram;
    Q3PtrList<Q3CanvasLine> foregroundHistogram;
    Q3PtrList<Q3CanvasLine> hueHistogram;
    Q3PtrList<Q3CanvasLine> saturationHistogram;
    Q3PtrList<Q3CanvasLine> valueHistogram;
    Q3Canvas* scaleCanvas;
    Q3CanvasView* scaleCanvasView;
    Q3CanvasPolygon* grayoutLower;
    Q3CanvasPolygon* grayoutUpper;

    // histogram backgrounds
    QPixmap pixmapIntensity;
    QPixmap pixmapForeground;
    QPixmap pixmapHue;
    QPixmap pixmapSaturation;
    QPixmap pixmapValue;

    // column labels
    QLabel* labelLower;
    QLabel* labelUpper;

    // keyboard editing
    QLineEdit* editLow;
    QIntValidator* validatorLow;
    QLineEdit* editHigh;
    QIntValidator* validatorHigh;
    
    // histogram option
    DiscretizeMethod m_discretizeMethod;

    // background color
    QRgb m_rgbBg;

    // large format has two edit boxes for manual editing, and larger widget sizes
    bool m_showLarge;
    int m_chooserHeight;
    int m_scaleHeight;
};

#endif // COLORCHOOSER_H
