/***************************************************************************
                          mrudocuments.cpp  -  description
                             -------------------
    begin                : Sat Sep 25 2004
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

#include "mrudocuments.h"
#include "digitdebug.h"
#include "defaultsettings.h"
#include "main.h"
//Added by qt3to4:
#include <QMenu>

MruDocuments::MruDocuments() :
  m_pParentMenu(0),
  m_parentActionId(-1),
  m_frameOffset(0, 0),
  m_pPopup(0),
  m_mruMax(0)
{
  DigitDebug::ctor(QString("mrudocuments ") + QString::number((ulong) this, 16));

  m_mruMax = DefaultSettings::instance().getMruMax();
}

MruDocuments::~MruDocuments()
{
  DigitDebug::dtor(QString("mrudocuments ") + QString::number((ulong) this, 16));

  m_mruDocuments.setAutoDelete(true);
  while (m_mruDocuments.count() > 0)
    m_mruDocuments.removeLast(); // autoDelete is on
  
  if (m_pPopup != 0)
    delete m_pPopup;
}

void MruDocuments::attachPopupMenu(QMenu* pParentMenu, int parentActionId, QSize frameOffset)
{
  ASSERT_ENGAUGE(m_pParentMenu == 0);

  m_pParentMenu = pParentMenu;
  m_parentActionId = parentActionId;

  // mru popup menu is offset from the parent popup menu
  m_frameOffset = QPoint(frameOffset.width(), frameOffset.height());

  // create mru popup menu
  m_pPopup = new QMenu("mru list", pParentMenu);
  CHECK_PTR_ENGAUGE(m_pPopup);
  
  m_pPopup->hide();

  // insert initial entries from settings
  QStringList list = DefaultSettings::instance().getMruDocuments();
  QStringList::iterator itr;
  for (itr = list.begin(); itr != list.end(); ++itr)
    appendDocumentToList(*itr);

  reconnectActionsToPopupMenu();

  // receive activation of an mru entry
  connect(m_pPopup, SIGNAL(activated(int)), this, SLOT(slotMruActivated(int)));
}

void MruDocuments::mergeDocument(QString doc)
{
  bool success = true;
  
  // if this document is already the first in the list, then nothing needs to be done
  if (!m_mruDocuments.isEmpty())
    if (doc == m_mruDocuments.first()->text())
      return;

  // disconnect all of the entries from the popup menu since list must be built with the newest entry 
  // first and there is no way to move entries around in the popup menu
  QAction* pAction;
  for (pAction = m_mruDocuments.first(); pAction; pAction = m_mruDocuments.next())
    success = pAction->removeFrom(m_pPopup);

  // remove duplicate entry from list if it exists
  for (pAction = m_mruDocuments.first(); pAction; pAction = m_mruDocuments.next())
    if (doc == pAction->text())
    {
      success = m_mruDocuments.remove(pAction); // autodelete is off
      delete pAction;
      break;
    }

  prependDocumentToList(doc);

  reconnectActionsToPopupMenu();
}

bool MruDocuments::isEmpty()
{
  return m_mruDocuments.isEmpty();
}

void MruDocuments::appendDocumentToList(QString doc)
{
  if ((int) m_mruDocuments.count() < m_mruMax)
  {
    QAction* pAction = new QAction(doc, 0);
    CHECK_PTR_ENGAUGE(pAction);
    m_mruDocuments.append(pAction); 
  }
}

void MruDocuments::prependDocumentToList(QString doc)
{
  // insert new entry into list
  QAction* pAction = new QAction(doc, 0);
  CHECK_PTR_ENGAUGE(pAction);
  m_mruDocuments.prepend(pAction);

  // remove entries if the limit is exceeded. since only one entry is added at a time, only
  // one entry might have to be removed
  if ((int) m_mruDocuments.count() > m_mruMax)
  {
    pAction = m_mruDocuments.last();
    m_mruDocuments.removeLast();

    ASSERT_ENGAUGE(pAction != 0);
    delete pAction;
  }
}

void MruDocuments::reconnectActionsToPopupMenu()
{
  // reconnect actions, now that they have been sorted, to popup menu
  QStringList list;
  for (QAction* pAction = m_mruDocuments.first(); pAction; pAction = m_mruDocuments.next())
  {
    pAction->addTo(m_pPopup);

    list.append(pAction->text());
  }

  // update settings
  DefaultSettings::instance().setMruDocuments(list);
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void MruDocuments::slotActionHighlighted(int id)
{
  if (!isEmpty() && (m_pPopup != 0))
  {
    if (id == m_parentActionId)
    {
      // since parent popup menu may have moved, reposition this frame
      m_pPopup->move(m_pParentMenu->pos() + m_frameOffset);

      m_pPopup->show();
    }
    else
    {
      m_pPopup->hide();
    }
  }
}

void MruDocuments::slotActionUnhighlighted()
{
  if (m_pPopup != 0)
  {
    m_pPopup->hide();
  }
}

void MruDocuments::slotMruActivated(int id)
{
  // search for the action having the specified id, since the id and index are different quantities
  for (int index = 0; index < (int) m_mruDocuments.count(); index++)
    if (id == m_pPopup->idAt(index))
    {     
      QAction* pAction = m_mruDocuments.at(index);
      if (pAction != 0)
        emit(mruChosen(pAction->text()));

      break;
    }
}
