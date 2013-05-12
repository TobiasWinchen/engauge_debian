/****************************************************************************
** $Id: helpwindow.cpp,v 1.4 2006/10/08 06:41:49 markmitch Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qstatusbar.h>
#include <qpixmap.h>
#include <qmenubar.h>
#include <q3toolbar.h>
#include <qtoolbutton.h>
#include <qicon.h>
#include <qfile.h>
#include <q3textstream.h>
#include <q3stylesheet.h>
#include <qmessagebox.h>
#include <q3filedialog.h>
#include <qapplication.h>
#include <qcombobox.h>
#include <qevent.h>
#include <qlineedit.h>
#include <qobject.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qdatastream.h>
#include <qprinter.h>
#include <q3simplerichtext.h>
#include <qpainter.h>
#include <q3paintdevicemetrics.h>
//Added by qt3to4:
#include <QMoveEvent>
#include <QResizeEvent>
#include <Q3Frame>

#include <ctype.h>

#include "img/helpback.xpm"
#include "img/helpforward.xpm"
#include "img/helphome.xpm"
#include "helpwindow.h"
#include "main.h"
#include "digitdebug.h"
#include "defaultsettings.h"

HelpWindow::HelpWindow( const QString& home_, const QString& _path,
  QWidget* parent, const char *name) :
  Q3MainWindow( parent, name, Qt::WDestructiveClose ),
  pathCombo( 0 ), selectedURL()
{
  DigitDebug::ctor(QString("helpwindow " + QString::number((ulong) this, 16)));
  
  readHistory();
  readBookmarks();

  browser = new Q3TextBrowser( this );
  CHECK_PTR_ENGAUGE(browser);

  browser->mimeSourceFactory()->setFilePath( _path );
  browser->setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
  connect( browser, SIGNAL( textChanged() ),
    this, SLOT( textChanged() ) );

  setCentralWidget( browser );

  if ( !home_.isEmpty() )
    browser->setSource( home_ );

  connect( browser, SIGNAL( highlighted( const QString&) ),
    statusBar(), SLOT( message( const QString&)) );

  // display help window in previous position with same size
  DefaultSettings& rSettings = DefaultSettings::instance();
  move(rSettings.getWindowHelpPosition());
  resize(rSettings.getWindowHelpSize());

  QMenu* file = new QMenu( this );
  CHECK_PTR_ENGAUGE(file);
  file->insertItem( tr("&Open File"), this, SLOT( openFile() ), Qt::CTRL+Qt::Key_O );
  file->insertItem( tr("&Print"), this, SLOT( print() ), Qt::CTRL+Qt::Key_P );
  file->insertSeparator();
  file->insertItem( tr("&Close"), this, SLOT( close() ), Qt::CTRL+Qt::Key_Q );
//file->insertItem( tr("E&xit"), qApp, SLOT( closeAllWindows() ), Qt::CTRL+Qt::Key_X );

  // The same three icons are used twice each.
  QPixmap helpback (helpback_xpm);
  QPixmap helpforward (helpforward_xpm);
  QPixmap helphome (helphome_xpm);
  
  QIcon icon_back (helpback);
  QIcon icon_forward (helpforward);
  QIcon icon_home (helphome);

  QMenu* go = new QMenu( this );
  CHECK_PTR_ENGAUGE(go);
  backwardId = go->insertItem( icon_back,
    tr("&Backward"), browser, SLOT( backward() ),
    Qt::CTRL+Qt::Key_Left );
  forwardId = go->insertItem( icon_forward,
    tr("&Forward"), browser, SLOT( forward() ),
    Qt::CTRL+Qt::Key_Right );
  go->insertItem( icon_home, tr("&Home"), browser, SLOT( home() ) );

  hist = new QMenu( this );
  CHECK_PTR_ENGAUGE(hist);
  QStringList::Iterator it = history.begin();
  for ( ; it != history.end(); ++it )
    mHistory[ hist->insertItem( *it ) ] = *it;
    connect( hist, SIGNAL( activated( int ) ),
    this, SLOT( histChosen( int ) ) );

  bookm = new QMenu( this );
  CHECK_PTR_ENGAUGE(bookm);
  bookm->insertItem( tr( "Add Bookmark" ), this, SLOT( addBookmark() ) );
  bookm->insertSeparator();

  QStringList::Iterator it2 = bookmarks.begin();
  for ( ; it2 != bookmarks.end(); ++it2 )
    mBookmarks[ bookm->insertItem( *it2 ) ] = *it2;
  connect( bookm, SIGNAL( activated( int ) ),
    this, SLOT( bookmChosen( int ) ) );

  menuBar()->insertItem( tr("&File"), file );
  menuBar()->insertItem( tr("&Go"), go );
  menuBar()->insertItem( tr( "History" ), hist );
  menuBar()->insertItem( tr( "Bookmarks" ), bookm );

  menuBar()->setItemEnabled( forwardId, FALSE);
  menuBar()->setItemEnabled( backwardId, FALSE);
  connect( browser, SIGNAL( backwardAvailable( bool ) ),
    this, SLOT( setBackwardAvailable( bool ) ) );
  connect( browser, SIGNAL( forwardAvailable( bool ) ),
    this, SLOT( setForwardAvailable( bool ) ) );

  Q3ToolBar* toolbar = new Q3ToolBar( this );
  CHECK_PTR_ENGAUGE(toolbar);
  addToolBar( toolbar, "Toolbar");
  QToolButton* button;

  button = new QToolButton( icon_back, tr("Backward"), "", browser, SLOT(backward()), toolbar );
  CHECK_PTR_ENGAUGE(button);
  connect( browser, SIGNAL( backwardAvailable(bool) ), button, SLOT( setEnabled(bool) ) );
  button->setEnabled( FALSE );
  button = new QToolButton( icon_forward, tr("Forward"), "", browser, SLOT(forward()), toolbar );
  CHECK_PTR_ENGAUGE(button);
  connect( browser, SIGNAL( forwardAvailable(bool) ), button, SLOT( setEnabled(bool) ) );
  button->setEnabled( FALSE );
  button = new QToolButton( icon_home, tr("Home"), "", browser, SLOT(home()), toolbar );
  CHECK_PTR_ENGAUGE(button);

  toolbar->addSeparator();

  pathCombo = new QComboBox( TRUE, toolbar );
  CHECK_PTR_ENGAUGE(pathCombo);
  connect( pathCombo, SIGNAL( activated( const QString & ) ),
    this, SLOT( pathSelected( const QString & ) ) );
  toolbar->setStretchableWidget( pathCombo );
  setRightJustification( TRUE );
  setDockEnabled( Qt::DockLeft, FALSE );
  setDockEnabled( Qt::DockRight, FALSE );

  pathCombo->insertItem( home_ );
  browser->setFocus();
}

HelpWindow::~HelpWindow()
{
  DigitDebug::dtor(QString("helpwindow " + QString::number((ulong) this, 16)));
  
  history.clear();
  QMap<int, QString>::Iterator it = mHistory.begin();
  for ( ; it != mHistory.end(); ++it )
    history.append( *it );

  QFile f( QDir::currentDirPath() + "/.history" );
  f.open( QIODevice::WriteOnly );
  QDataStream s( &f );
  s << history;
  f.close();

  bookmarks.clear();
  QMap<int, QString>::Iterator it2 = mBookmarks.begin();
  for ( ; it2 != mBookmarks.end(); ++it2 )
    bookmarks.append( *it2 );

  QFile f2( QDir::currentDirPath() + "/.bookmarks" );
  f2.open( QIODevice::WriteOnly );
  QDataStream s2( &f2 );
  s2 << bookmarks;
  f2.close();
}

void HelpWindow::setBackwardAvailable( bool b)
{
  menuBar()->setItemEnabled( backwardId, b);
}

void HelpWindow::setForwardAvailable( bool b)
{
  menuBar()->setItemEnabled( forwardId, b);
}

void HelpWindow::textChanged()
{
  if ( browser->documentTitle().isNull() )
    setCaption( "Engauge Digitizer Help - " + browser->context() );
  else
    setCaption( "Engauge Digitizer Help - " + browser->documentTitle() ) ;

  selectedURL = browser->context();

  if ( !selectedURL.isEmpty() && pathCombo ) {
    bool exists = FALSE;
    int i;
    for ( i = 0; i < pathCombo->count(); ++i ) {
      if ( pathCombo->text( i ) == selectedURL ) {
        exists = TRUE;
        break;
      }
    }
    if ( !exists ) {
      pathCombo->insertItem( selectedURL, 0 );
      pathCombo->setCurrentItem( 0 );
      mHistory[ hist->insertItem( selectedURL ) ] = selectedURL;
    }
    else
      pathCombo->setCurrentItem( i );
    selectedURL = QString::null;
  }
}

void HelpWindow::openFile()
{
#ifndef QT_NO_FILEDIALOG
  QString fn = Q3FileDialog::getOpenFileName( QString::null, QString::null, this );
  if ( !fn.isEmpty() )
    browser->setSource( fn );
#endif
}

void HelpWindow::print()
{
#ifndef QT_NO_PRINTER
  QPrinter printer;
  printer.setFullPage(TRUE);
  if ( printer.setup( this ) ) {
    QPainter p( &printer );
    Q3PaintDeviceMetrics metrics(p.device());
    int dpix = metrics.logicalDpiX();
    int dpiy = metrics.logicalDpiY();
    const int margin = 72; // pt
    QRect body(margin*dpix/72, margin*dpiy/72,
      metrics.width()-margin*dpix/72*2,
      metrics.height()-margin*dpiy/72*2 );
    QFont font( "times", 10 );
    Q3SimpleRichText richText( browser->text(), font,
      browser->context(), 
      browser->styleSheet(),
      browser->mimeSourceFactory(), 
      body.height() );
    richText.setWidth( &p, body.width() );
    QRect view( body );
    int page = 1;
    do {
      richText.draw( &p, body.left(), body.top(), view, colorGroup() );
      view.moveBy( 0, body.height() );
      p.translate( 0 , -body.height() );
      p.setFont( font );
      p.drawText( view.right() - p.fontMetrics().width( QString::number(page) ),
      view.bottom() + p.fontMetrics().ascent() + 5, QString::number(page) );
      if ( view.top()  >= body.top() + richText.height() )
        break;
      printer.newPage();
      page++;
    } while (TRUE);
  }
#endif
}

void HelpWindow::pathSelected( const QString &_path )
{
  ASSERT_ENGAUGE(browser != 0);
  browser->setSource( _path );
  QMap<int, QString>::Iterator it = mHistory.begin();
  bool exists = FALSE;
  for ( ; it != mHistory.end(); ++it ) {
    if ( *it == _path ) {
      exists = TRUE;
      break;
    }
  }
  if ( !exists )
    mHistory[ hist->insertItem( _path ) ] = _path;
}

void HelpWindow::readHistory()
{
  if ( QFile::exists( QDir::currentDirPath() + "/.history" ) ) {
    QFile f( QDir::currentDirPath() + "/.history" );
    f.open( QIODevice::ReadOnly );
    QDataStream s( &f );
    s >> history;
    f.close();
    while ( history.count() > 20 )
      history.remove( history.begin() );
  }
}

void HelpWindow::readBookmarks()
{
  if ( QFile::exists( QDir::currentDirPath() + "/.bookmarks" ) ) {
    QFile f( QDir::currentDirPath() + "/.bookmarks" );
    f.open( QIODevice::ReadOnly );
    QDataStream s( &f );
    s >> bookmarks;
    f.close();
  }
}

void HelpWindow::histChosen( int i )
{
  if ( mHistory.contains( i ) )
  {
    ASSERT_ENGAUGE(browser != 0);
    browser->setSource( mHistory[ i ] );
  }
}

void HelpWindow::bookmChosen( int i )
{
  if ( mBookmarks.contains( i ) )
  {
    ASSERT_ENGAUGE(browser != 0);
    browser->setSource( mBookmarks[ i ] );
  }
}

void HelpWindow::addBookmark()
{
  ASSERT_ENGAUGE(browser != 0);
  mBookmarks[ bookm->insertItem( caption() ) ] = browser->context();
}

void HelpWindow::moveEvent(QMoveEvent* pEvent)
{
  if (pEvent)
    DefaultSettings::instance().setWindowHelpPosition(pEvent->pos());
}

void HelpWindow::resizeEvent(QResizeEvent* pEvent)
{
  if (pEvent)
    DefaultSettings::instance().setWindowHelpSize(pEvent->size());
}
