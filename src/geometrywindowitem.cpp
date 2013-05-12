/***************************************************************************
                          geometrywindowitem.cpp  -  description
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

#include "geometrywindowitem.h"

GeometryWindowItem::GeometryWindowItem() :
  m_row(0),
  m_column(0)
{
}

GeometryWindowItem::GeometryWindowItem(int row, int column, QString entry) :
  m_row(row),
  m_column(column),
  m_entry(entry)
{
}

GeometryWindowItem::GeometryWindowItem(const GeometryWindowItem& old)
{
  m_row = old.row();
  m_column = old.column();
  m_entry = old.entry();
}

GeometryWindowItem& GeometryWindowItem::operator=(const GeometryWindowItem& old)
{
  m_row = old.row();
  m_column = old.column();
  m_entry = old.entry();  

  return *this;
}

bool GeometryWindowItem::operator==(const GeometryWindowItem& other) const
{
  return ((m_row == other.row()) &&
    (m_column == other.column()) &&
    (m_entry == other.entry()));
}
    
int GeometryWindowItem::row() const
{
  return m_row;
}

int GeometryWindowItem::column() const
{
  return m_column;
}

QString GeometryWindowItem::entry() const
{
  return m_entry;
}
