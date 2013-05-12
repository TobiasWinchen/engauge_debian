/****************************************************************************
** $Id: helpwindow.h,v 1.3 2005/03/20 01:47:05 markmitch Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <q3mainwindow.h>
#include <q3textbrowser.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qdir.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <QMenu>
#include <QMoveEvent>

class QComboBox;

class HelpWindow : public Q3MainWindow
{
    Q_OBJECT
    
  public:
  
    HelpWindow( const QString& home_,  const QString& path, QWidget* parent = 0, const char *name=0 );
    ~HelpWindow();

  private slots:
  
    void setBackwardAvailable( bool );
    void setForwardAvailable( bool );

    void textChanged();
    void openFile();
    void print();

    void pathSelected( const QString &);
    void histChosen( int );
    void bookmChosen( int );
    void addBookmark();

  protected:

    virtual void moveEvent(QMoveEvent *pEvent);
    virtual void resizeEvent(QResizeEvent *pEvent);
    
  private:

    void readHistory();
    void readBookmarks();
    
    Q3TextBrowser* browser;
    QComboBox *pathCombo;
    int backwardId, forwardId;
    QString selectedURL;
    QStringList history, bookmarks;
    QMap<int, QString> mHistory, mBookmarks;
    QMenu *hist, *bookm;
    
};

#endif // HELPWINDOW_H
