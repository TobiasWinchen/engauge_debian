/******************************************************************************************************
 * (C) 2014 markummitchell@github.com. This file is part of Engauge Digitizer, which is released      *
 * under GNU General Public License version 2 (GPLv2) or (at your option) any later version. See file *
 * LICENSE or go to gnu.org/licenses for details. Distribution requires prior written permission.     *
 ******************************************************************************************************/

#include "FileCmdAbstract.h"
#include "Logger.h"
#include "MainWindow.h"
#include <QMessageBox>

FileCmdAbstract::FileCmdAbstract(const QString &cmdDescription) :
  m_cmdDescription (cmdDescription)
{
}

FileCmdAbstract::~FileCmdAbstract()
{
}

QString FileCmdAbstract::cmdDescription() const
{
  return m_cmdDescription;
}
