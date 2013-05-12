/***************************************************************************
                          transpose.cpp  -  description
                             -------------------
    begin                : Jan 5 2005
    copyright            : (C) 2005 by Mark Mitchell
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

#include <stdio.h>
#include <stdlib.h>

#include <qstring.h>
#include <qstringlist.h>
#include <qvaluevector.h>

#include "matrix.h"

const QString TOKEN_COMMA("-comma");
const QString TOKEN_SPACE("-space");
const QString TOKEN_TAB("-tab");
const QString VALUE_COMMA(",");
const QString VALUE_SPACE(" ");
const QString VALUE_TAB("\t");

// prototypes
void closeFile(FILE *file);
void inputPass1(FILE* fileIn,
                const QString& rDelimiter,
                unsigned int *pColumns,
                QPtrList<QStringList>& parsedFields);
Matrix<QString> inputPass2(unsigned int rows,
                           unsigned int columns,
                           QPtrList<QStringList>& parsedFields);
void outputPass(FILE* fileOut,
                const QString& rDelimiter,
                Matrix<QString>& rMatrix);
void parseCmdLine(int argc,
                  char** argv,
                  FILE*& fileIn,
                  FILE*& fileOut,
                  QString& rDelimiter);

// functions
void closeFile(FILE *file)
{
  if ((file != stdin) &&
      (file != stdout)) {
    fclose(file);
  }
}

void inputPass1(FILE* fileIn,
                const QString& rDelimiter,
                unsigned int *pColumns,
                QPtrList<QStringList>& parsedFields)
{
  // in first input pass we find the maximum number of columns
  *pColumns = 0;

  const bool AllowEmptyEntries = true;
  QTextStream str(fileIn, IO_ReadOnly);
  QString line = str.readLine();
  while (!line.isNull()) {
    QStringList* pRowFields = new QStringList;

    *pRowFields = QStringList::split(rDelimiter,
                                     line,
                                     AllowEmptyEntries);

    parsedFields.append(pRowFields);

    if (pRowFields->count() > *pColumns)
      *pColumns = pRowFields->count();

    line = str.readLine();
  }
}

Matrix<QString> inputPass2(unsigned int columns,
                           QPtrList<QStringList>& parsedFields)
{
  // in second input pass we copy, transpose, and then output
  Matrix<QString> matrix(parsedFields.count(),
                         columns);

  unsigned int row, col;
  QStringList* pRowFields;
  for (row = 0, pRowFields = parsedFields.first(); 
       pRowFields;
       row++, pRowFields = parsedFields.next()) {

    QStringList::Iterator itr;
    for (col = 0, itr = pRowFields->begin();
         itr != pRowFields->end();
         ++itr, col++) {

      // save field after stripping away carriage returns, and 
      // leading and trailing blanks
      if ((*itr).isNull())
        matrix [row] [col] = "";
      else
        matrix [row] [col] = (*itr).stripWhiteSpace();      
    }

    // pad if necessary
    for (; col < columns; col++)
      matrix [row] [col] = "";
  }

  // sure there is a bit of copying here, but this utility is not time critical
  return matrix;
}

int main(int argc, char** argv)
{
  FILE *fileIn = 0, *fileOut = 0;
  QString delimiter;

  parseCmdLine(argc, 
               argv, 
               fileIn,
               fileOut,
               delimiter);

  unsigned int columns;
  QPtrList<QStringList> parsedFields;
  inputPass1(fileIn,
             delimiter,
             &columns,
             parsedFields);
  Matrix<QString> matrix = inputPass2(columns,
                                      parsedFields);

  outputPass(fileOut,
             delimiter,
             matrix);

  closeFile(fileIn);
  closeFile(fileOut);

  return 0;
}

void outputPass(FILE* fileOut,
                const QString& rDelimiter,
                Matrix<QString>& rMatrix)
{
  // rows and columns in input correspond to columns and rows in 
  // output, respectively
  for (unsigned int column = 0; column < rMatrix.columns(); column++) {
    for (unsigned int row = 0; row < rMatrix.rows(); row++) {
      if (row > 0)
        fprintf(fileOut, "%s", rDelimiter.ascii());

      fprintf(fileOut, "%s", rMatrix [row] [column].ascii());
    }

    fprintf(fileOut, "\n");
  }
}

void parseCmdLine(int argc,
                  char** argv,
                  FILE*& fileIn,
                  FILE*& fileOut,
                  QString& rDelimiter)
{
  // default delimiter
  rDelimiter = VALUE_COMMA;

  fileIn = stdin;
  fileOut = stdout;

  // all error messages to stdout and stderr will be silently dropped 
  // in microsoft windows unless the application is compiled as a console app!

  // parse command line fields, skipping the first since it is the program name
  QString filename;
  bool usageError = false;
  for (int i = 1; i < argc; i++) {
    QString arg(argv [i]);

    if (arg == TOKEN_COMMA)
      rDelimiter = VALUE_COMMA;
    else if (arg == TOKEN_SPACE)
      rDelimiter = VALUE_SPACE;
    else if (arg == TOKEN_TAB)
      rDelimiter = VALUE_TAB;
    else {
      if (!filename.isEmpty())
        usageError = true; // file already specified
      else
        filename = arg;
    }
  }

  // parsing was successful. open input file if not using standard input
  if (!filename.isEmpty()) {
    fileIn = fopen(filename.ascii(), "r");
    if (!fileIn) {
      usageError = true;
    }
  }

  if (usageError) {
    QString delimiters = QString("%1 | %2 | %3")
      .arg(TOKEN_COMMA)
      .arg(TOKEN_SPACE)
      .arg(TOKEN_TAB);

    QString usageMsg = QString(
            "Usage: transpose [%1] <infile\n"
            "       transpose [%2] infile\n\n"
            "(default delimiter is comma)\n")
      .arg(delimiters)
      .arg(delimiters);

    fprintf(stderr, "%s", usageMsg.ascii());
    exit(-1);
  }
}
