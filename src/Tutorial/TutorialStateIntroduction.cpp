/******************************************************************************************************
 * (C) 2014 markummitchell@github.com. This file is part of Engauge Digitizer, which is released      *
 * under GNU General Public License version 2 (GPLv2) or (at your option) any later version. See file *
 * LICENSE or go to gnu.org/licenses for details. Distribution requires prior written permission.     *
 ******************************************************************************************************/

#include "Logger.h"
#include <qdebug.h>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "TutorialButton.h"
#include "TutorialDlg.h"
#include "TutorialStateContext.h"
#include "TutorialStateIntroduction.h"

TutorialStateIntroduction::TutorialStateIntroduction (TutorialStateContext &context) : 
  TutorialStateAbstractBase (context),
  m_title (nullptr),
  m_background (nullptr),
  m_text0 (nullptr),
  m_text1 (nullptr),
  m_text2 (nullptr),
  m_next (nullptr)
{
  LOG4CPP_INFO_S ((*mainCat)) << "TutorialStateIntroduction::TutorialStateIntroduction";
}

void TutorialStateIntroduction::begin ()
{
  LOG4CPP_INFO_S ((*mainCat)) << "TutorialStateIntroduction::begin ()";

  m_title = createTitle (tr ("Introduction"));
  m_background = createPixmapItem (":/engauge/img/SpreadsheetsForDoc.png",
                                   QPoint (0, 0));
  m_text0 = createTextItem (tr ("Engauge Digitizer starts with\n"
                                "images of graphs and maps."),
                            QPoint (200, 40));
  m_text1 = createTextItem (tr ("You create (or digitize) points along\n"
                                "the graph and map curves."),
                            QPoint (240, 180));
  m_text2 = createTextItem (tr ("The digitized curve points can be\n"
                                "exported, as numbers, to other software tools."),
                            QPoint (210, 330));

  QSize backgroundSize = context().tutorialDlg().backgroundSize();

  m_next = new TutorialButton (tr ("Next"),
                               context().tutorialDlg().scene());
  m_next->setGeometry (QPoint (backgroundSize.width () - buttonMargin () - m_next->size ().width (),
                               backgroundSize.height () - buttonMargin () - m_next->size ().height ()));
  connect (m_next, SIGNAL (signalTriggered ()), this, SLOT (slotNext ()));
}

void TutorialStateIntroduction::end ()
{
  LOG4CPP_INFO_S ((*mainCat)) << "TutorialStateIntroduction::end ()";

  context().tutorialDlg().scene().removeItem (m_title);
  context().tutorialDlg().scene().removeItem (m_background);
  context().tutorialDlg().scene().removeItem (m_text0);
  context().tutorialDlg().scene().removeItem (m_text1);
  context().tutorialDlg().scene().removeItem (m_text2);
  // TutorialButtons removes themselves from the scene

  delete m_title;
  delete m_background;
  delete m_text0;
  delete m_text1;
  delete m_text2;
  delete m_next;

  m_title = nullptr;
  m_background = nullptr;
  m_text0 = nullptr;
  m_text1 = nullptr;
  m_text2 = nullptr;
  m_next = nullptr;
}

void TutorialStateIntroduction::slotNext ()
{
  LOG4CPP_INFO_S ((*mainCat)) << "TutorialStateIntroduction::slotNext";

  context().requestDelayedStateTransition (TUTORIAL_STATE_AXIS_POINTS);
}
