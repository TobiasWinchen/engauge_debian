/******************************************************************************************************
 * (C) 2019 markummitchell@github.com. This file is part of Engauge Digitizer, which is released      *
 * under GNU General Public License version 2 (GPLv2) or (at your option) any later version. See file *
 * LICENSE or go to gnu.org/licenses for details. Distribution requires prior written permission.     *
 ******************************************************************************************************/

#ifndef CURVES_INCLUDED_HASH_H
#define CURVES_INCLUDED_HASH_H

#include <QHash>
#include <QString>

// Container of curve names to include in export, using QHash for rapid access
typedef QHash<QString, bool> CurvesIncludedHash;

#endif // CURVES_INCLUDED_HASH_H
