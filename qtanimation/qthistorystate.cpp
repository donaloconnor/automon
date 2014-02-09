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



#include "qthistorystate.h"
#include "qthistorystate_p.h"

QT_BEGIN_NAMESPACE

/*!
  \class QtHistoryState

  \brief The QtHistoryState class provides a means of returning to a previously active substate.

  

  A history state is a pseudo-state that represents the child state that the
  parent state was in the last time the parent state was exited. A transition
  with a history state as its target is in fact a transition to one of the
  other child states of the parent state. QtHistoryState is part of \l{The
  State Machine Framework}.

  Use QtState::addHistoryState() to construct a history state. Use the
  setDefaultState() function to set the state that should be entered if the
  parent state has never been entered.  Example:

  \code
  QtStateMachine machine;

  QtState *s1 = new QtState();
  QtState *s11 = new QtState(s1);
  QtState *s12 = new QtState(s1);

  QtState *s1h = s1->addHistoryState();
  s1h->setDefaultState(s11);

  machine.addState(s1);

  QtState *s2 = new QtState();
  machine.addState(s2);

  QPushButton *button = new QPushButton();
  // Clicking the button will cause the state machine to enter the child state
  // that s1 was in the last time s1 was exited, or the history state's default
  // state if s1 has never been entered.
  s1->addTransition(button, SIGNAL(clicked()), s1h);
  \endcode
*/

QtHistoryStatePrivate::QtHistoryStatePrivate()
    : defaultState(0)
{
}

QtHistoryState *QtHistoryStatePrivate::create(QtState::HistoryType type,
                                            QtState *parent)
{
    return new QtHistoryState(type, parent);
}

QtHistoryStatePrivate *QtHistoryStatePrivate::get(QtHistoryState *q)
{
    return q->d_func();
}

const QtHistoryStatePrivate *QtHistoryStatePrivate::get(const QtHistoryState *q)
{
    return q->d_func();
}

/*!
  \internal

  Constructs a new history state of the given \a type, with the given \a
  parent state.
*/
QtHistoryState::QtHistoryState(QtState::HistoryType type, QtState *parent)
    : QtAbstractState(*new QtHistoryStatePrivate, parent)
{
    Q_D(QtHistoryState);
    d->historyType = type;
}

/*!
  Destroys this history state.
*/
QtHistoryState::~QtHistoryState()
{
}

/*!
  Returns this history state's default state.  The default state indicates the
  state to transition to if the parent state has never been entered before.
*/
QtAbstractState *QtHistoryState::defaultState() const
{
    Q_D(const QtHistoryState);
    return d->defaultState;
}

/*!
  Sets this history state's default state to be the given \a state.
  \a state must be a sibling of this history state.
*/
void QtHistoryState::setDefaultState(QtAbstractState *state)
{
    Q_D(QtHistoryState);
    if (state && state->parentState() != parentState()) {
        qWarning("QtHistoryState::setDefaultState: state %p does not belong "
                 "to this history state's group (%p)", state, parentState());
        return;
    }
    d->defaultState = state;
}

/*!
  \reimp
*/
void QtHistoryState::onEntry()
{
}

/*!
  \reimp
*/
void QtHistoryState::onExit()
{
}

/*!
  \reimp
*/
bool QtHistoryState::event(QEvent *e)
{
    return QtAbstractState::event(e);
}

QT_END_NAMESPACE
