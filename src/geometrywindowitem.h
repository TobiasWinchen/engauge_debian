/***************************************************************************
                          geometrywindowitem.h  -  description
                             -------------------
    begin                : Sun Sep 12 2004
    copyright            : (C) 2004 by 
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

#ifndef GEOMETRYWINDOWITEM_H
#define GEOMETRYWINDOWITEM_H

#include <qstring.h>
#include <QList>

// entry corresponding to one cell in the table of the curve geometry or 
// measure geometry windows
class GeometryWindowItem
{
  
  public:

    GeometryWindowItem(); // default constructor should only be used by GeometryWindowItemList
    GeometryWindowItem(int row, int column, QString entry);

    // copy and assignment constructors are required by GeometryWindowItemList
    GeometryWindowItem(const GeometryWindowItem& old);
    GeometryWindowItem& operator=(const GeometryWindowItem& old);

    // comparison operator
    bool operator==(const GeometryWindowItem& other) const;
    
    int row() const;
    int column() const;
    QString entry() const;
    
  private:

    int m_row;
    int m_column;
    QString m_entry;

};

// use value list, rather than pointer list, because it offers easy deep copies (although
// this means adding default constructor, copy and assignment operators to the items). also, the
// original pointer list implementation was probably leaking memory so value list should be safer
typedef QList<GeometryWindowItem> GeometryWindowItemList;

#endif // GEOMETRYWINDOWITEM_H
