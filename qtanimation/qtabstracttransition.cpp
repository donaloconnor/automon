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



#include "qtabstracttransition.h"
#include "qtabstracttransition_p.h"
#include "qtabstractstate.h"
#include "qtstate.h"
#include "qtstatemachine.h"

QT_BEGIN_NAMESPACE

/*!
  \class QtAbstractTransition

  \brief The QtAbstractTransition class is the base class of transitions between QtAbstractState objects.

  

  The QtAbstractTransition class is the abstract base class of transitions
  between states (QtAbstractState objects) of a
  QtStateMachine. QtAbstractTransition is part of \l{The State Machine
  Framework}.

  The QtTransition class provides a default (action-based) implementation of
  the QtAbstractTransition interface.

  The sourceState() function returns the source of the transition. The
  targetStates() function returns the targets of the transition.

  \section1 Subclassing

  The testCondition() function is called by the state machine to determine
  whether the condition(s) associated with the transition is satisfied. In
  your reimplementation you typically cast the event object to the proper
  type, and check that one or more properties of the event meet your criteria.

  The onTransition() function is called when the transition is triggered;
  reimplement this function to perform custom processing for the transition.
*/

QtAbstractTransitionPrivate::QtAbstractTransitionPrivate()
{
}

QtAbstractTransitionPrivate *QtAbstractTransitionPrivate::get(QtAbstractTransition *q)
{
    return q->d_func();
}

const QtAbstractTransitionPrivate *QtAbstractTransitionPrivate::get(const QtAbstractTransition *q)
{
    return q->d_func();
}

QtStateMachine *QtAbstractTransitionPrivate::machine() const
{
    Q_Q(const QtAbstractTransition);
    QObject *par = q->parent();
    while (par != 0) {
        if (QtStateMachine *mach = qobject_cast<QtStateMachine*>(par))
            return mach;
        par = par->parent();
    }
    return 0;
}

bool QtAbstractTransitionPrivate::callTestCondition(QEvent *e) const
{
    Q_Q(const QtAbstractTransition);
    return q->testCondition(e);
}

void QtAbstractTransitionPrivate::callOnTransition()
{
    Q_Q(QtAbstractTransition);
    q->onTransition();
}

QtState *QtAbstractTransitionPrivate::sourceState() const
{
    Q_Q(const QtAbstractTransition);
    return qobject_cast<QtState*>(q->parent());
}

/*!
  Constructs a new transition associated with the given \a eventTypes.

  The transition should be added to the appropriate state using
  QtAbstractState::addTransition().
*/
QtAbstractTransition::QtAbstractTransition(const QList<QEvent::Type> &eventTypes)
    : QObject(
#ifndef QT_STATEMACHINE_SOLUTION
        *new QtAbstractTransitionPrivate
#endif
        )
#ifdef QT_STATEMACHINE_SOLUTION
    , d_ptr(new QtAbstractTransitionPrivate)
#endif
{
#ifdef QT_STATEMACHINE_SOLUTION
    d_ptr->q_ptr = this;
#endif
    Q_D(QtAbstractTransition);
    d->eventTypes = eventTypes;
}

/*!
  Constructs a new transition associated with the given \a eventType.

  The transition should be added to the appropriate state using
  QtAbstractState::addTransition().
*/
QtAbstractTransition::QtAbstractTransition(QEvent::Type eventType)
    : QObject(
#ifndef QT_STATEMACHINE_SOLUTION
        *new QtAbstractTransitionPrivate
#endif
        )
#ifdef QT_STATEMACHINE_SOLUTION
    , d_ptr(new QtAbstractTransitionPrivate)
#endif
{
#ifdef QT_STATEMACHINE_SOLUTION
    d_ptr->q_ptr = this;
#endif
    Q_D(QtAbstractTransition);
    d->eventTypes = QList<QEvent::Type>() << eventType;
}

/*!
  \internal
*/
QtAbstractTransition::QtAbstractTransition(QtAbstractTransitionPrivate &dd,
                                         const QList<QEvent::Type> &eventTypes)
    : QObject(
#ifndef QT_STATEMACHINE_SOLUTION
        dd
#endif
        )
#ifdef QT_STATEMACHINE_SOLUTION
    , d_ptr(&dd)
#endif
{
#ifdef QT_STATEMACHINE_SOLUTION
    d_ptr->q_ptr = this;
#endif
    Q_D(QtAbstractTransition);
    d->eventTypes = eventTypes;
}

/*!
  \internal
*/
QtAbstractTransition::QtAbstractTransition(QtAbstractTransitionPrivate &dd,
                                         QEvent::Type eventType)
    : QObject(
#ifndef QT_STATEMACHINE_SOLUTION
        dd
#endif
        )
#ifdef QT_STATEMACHINE_SOLUTION
    , d_ptr(&dd)
#endif
{
#ifdef QT_STATEMACHINE_SOLUTION
    d_ptr->q_ptr = this;
#endif
    Q_D(QtAbstractTransition);
    d->eventTypes = QList<QEvent::Type>() << eventType;
}

/*!
  Destroys this transition.
*/
QtAbstractTransition::~QtAbstractTransition()
{
#ifdef QT_STATEMACHINE_SOLUTION
    delete d_ptr;
#endif
}

/*!
  Returns the source state of this transition, or 0 if this transition has no
  source state.
*/
QtState *QtAbstractTransition::sourceState() const
{
    Q_D(const QtAbstractTransition);
    return d->sourceState();
}

/*!
  Returns the target states of this transition, or an empty list if this
  transition has no target states.
*/
QList<QtAbstractState*> QtAbstractTransition::targetStates() const
{
    Q_D(const QtAbstractTransition);
    return d->targetStates;
}

/*!
  This function is called to determine whether the condition (if any)
  associated with this transition is satisfied.
  The default implementation returns true, i.e. there is no condition
  associated with the transition.

  You are guaranteed that the \a event is of the type that was passed to the
  QtAbstractTransition constructor (i.e. it is safe to static_cast it).
*/
bool QtAbstractTransition::testCondition(QEvent *event) const
{
    Q_UNUSED(event);
    return true;
}

/*!
  \fn QtAbstractTransition::onTransition()

  This function is called when the transition is triggered.  Reimplement this
  function to perform custom processing when the transition is triggered.
*/

/*!
  \reimp
*/
bool QtAbstractTransition::event(QEvent *e)
{
    return QObject::event(e);
}

QT_END_NAMESPACE
