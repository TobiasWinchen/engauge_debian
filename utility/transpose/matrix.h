/***************************************************************************
                          matrix.h  -  description
                             -------------------
    begin                : Jan 5 2005
    copyright            : (C) 2005 by Mark Mitchell
    email                : mmdigitizer@earthlink.net
    $Log: matrix.h,v $
    Revision 1.2  2006/10/25 03:35:15  markmitch
    Documented license info


 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MATRIX_H
#define MATRIX_H

#include <stdexcept>

#include <qvaluevector.h>

// matrix class that has a fixed number of rows and columns (set during
// construction)
template <class T>
class Matrix
{

 public:

  // only constructor
  Matrix(unsigned int rows,
         unsigned int columns) :
    m_array(rows * columns),
    m_rows(rows),
    m_columns(columns) {}

  // row class enables two-dimensional syntax like A[i][j]. this must
  // be public so the code compiles, but this class can be treated as
  // private since it works behind the scenes
  class Row
  {

   public:

    // only constructor
    Row(unsigned int row,
        Matrix& rMatrix) :
      m_rMatrix(rMatrix),
      m_row(row) {}


    // array operator
    T& operator [] (unsigned int column) const
    {
      return m_rMatrix.at (m_row, 
                           column);
    }

   private:

    Matrix& m_rMatrix;
    unsigned int m_row;
  };

  // get methods for size
  unsigned int rows() { return m_rows; }
  unsigned int columns() { return m_columns; }

  // return element at specified row and column, throwing an exception
  // if the column number is invalid or if the element has not yet been
  // defined. this class handles the row index, and the Row class handles
  // the column index
  Row operator [] (unsigned int row)
  {
    // construct row object and return a copy
    return Row(row, *this);
  };

  // unpublicized accessor method used by publicized array operator
  T& at (unsigned int row,
         unsigned int column)
  {
    if (row >= m_rows)
      throw std::out_of_range("row overflow");
    
    if (column >= m_columns)
      throw std::out_of_range("column overflow");
    
    int element = m_columns * row + column;
    
    return m_array [element];
  }

 private:

  // store two-dimensional matrix as vector
  QValueVector<T> m_array;

  // matrix limits
  unsigned int m_rows; // tracked during execution
  unsigned int m_columns; // known during initialization
};

#endif // MATRIX_H
