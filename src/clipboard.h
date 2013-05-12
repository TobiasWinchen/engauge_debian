/***************************************************************************
                          clipboard.h  -  description
                             -------------------
    begin                : Mon Dec 9 2002
    copyright            : (C) 2002 by Mark Mitchell
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

#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include "point.h"
//Added by qt3to4:
#include <QList>

// this singleton class performs digitizer-specific clipboard operations.
// other applications and mime types are not supported
class Clipboard
{

  public: 

    static void create(void)
    {
      if (!m_clipboard)
        m_clipboard = new Clipboard;
    }
    static void destroy(void)
    {
      if (m_clipboard)
      {
        delete m_clipboard;
        m_clipboard = 0;
      }
    }
    static Clipboard& instance(void)
    {
      return *m_clipboard;
    }

    // empty contents of clipboard
    void clear(void) { m_contents.clear(); }

    // debug
    void dump(void);

    // append single point
    void addPoint(int xScreen, int yScreen);

    // return contents for building new points
    QList<QPoint>* contents(void) { return &m_contents; }

    // return true if clipboard has any items
    bool clipboardItems(void) { return (m_contents.count() > 0); }
    
  protected:
  
    Clipboard();
    ~Clipboard();

  private:

    // clipboard contents. points are detached from the canvas and pointsets
    QList<QPoint> m_contents;

    // this singleton object
    static Clipboard* m_clipboard;

};

#endif // CLIPBOARD_H
