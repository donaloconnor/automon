/****************************************************************************
**
** This file is part of a Qt Solutions component.
** 
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
** 
** Contact:  Qt Software Information (qt-info@nokia.com)
** 
** Commercial Usage  
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
** 
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
** 
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
** 
** GNU General Public License Usage 
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
** 
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
** 
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** 
****************************************************************************/



#include "qtfinalstate.h"
#include "qtactionstate_p.h"

QT_BEGIN_NAMESPACE

/*!
  \class QtFinalState

  \brief The QtFinalState class provides a final state.

  

  A final state is used to communicate that (part of) a QtStateMachine has
  finished its work. When a final top-level state is entered, the state
  machine's \l{QtStateMachine::finished()}{finished}() signal is emitted. In
  general, when a final substate (a child of a QtState) is entered, a
  QtStateFinishedEvent is generated for the final state's parent
  state. QtFinalState is part of \l{The State Machine Framework}.

  To use a final state, you create a QtFinalState object and add a transition
  to it from another state. Example:

  \code
  QPushButton button;

  QtStateMachine machine;
  QtState *s1 = new QtState();
  QtFinalState *s2 = new QtFinalState();
  s1->addTransition(&button, SIGNAL(clicked()), s2);
  machine.addState(s1);
  machine.addState(s2);

  QObject::connect(&machine, SIGNAL(finished()), QApplication::instance(), SLOT(quit()));
  machine.setInitialState(s1);
  machine.start();
  \endcode

  \sa QtStateFinishedTransition
*/

class QtFinalStatePrivate : public QtActionStatePrivate
{
    Q_DECLARE_PUBLIC(QtFinalState)

public:
    QtFinalStatePrivate();
};

QtFinalStatePrivate::QtFinalStatePrivate()
{
}

/*!
  Constructs a new QtFinalState object with the given \a parent state.
*/
QtFinalState::QtFinalState(QtState *parent)
    : QtActionState(*new QtFinalStatePrivate, parent)
{
}

/*!
  Destroys this final state.
*/
QtFinalState::~QtFinalState()
{
}

/*!
  \reimp
*/
void QtFinalState::onEntry()
{
    QtActionState::onEntry();
}

/*!
  \reimp
*/
void QtFinalState::onExit()
{
    QtActionState::onExit();
}

/*!
  \reimp
*/
bool QtFinalState::event(QEvent *e)
{
    return QtActionState::event(e);
}

QT_END_NAMESPACE
