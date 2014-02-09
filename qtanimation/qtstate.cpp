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



#include "qtstate.h"
#include "qtstate_p.h"
#include "qthistorystate.h"
#include "qthistorystate_p.h"
#include "qtabstracttransition.h"
#include "qtabstracttransition_p.h"
#include "qtsignaltransition.h"
#include "qtstatefinishedtransition.h"
#ifndef QT_NO_ANIMATION
#include "qtanimationstate.h"
#endif

QT_BEGIN_NAMESPACE

/*!
  \class QtState

  \brief The QtState class provides a general-purpose state for QtStateMachine.

  

  QtState objects can have child states, and can have transitions to other
  states. QtState is part of \l{The State Machine Framework}.

  The addTransition() function adds a transition. The removeTransition()
  function removes a transition.

  \section1 States with Child States

  For non-parallel state groups, the setInitialState() function must be called
  to set the initial state. The child states are mutually exclusive states,
  and the state machine needs to know which child state to enter when the
  parent state is the target of a transition.

  The addHistoryState() function adds a history state.

  The setErrorState() sets the state's error state. The error state is the
  state that the state machine will transition to if an error is detected when
  attempting to enter the state (e.g. because no initial state has been set).
*/

/*!
  \enum QtState::Type

  This enum specifies the type of a state.

  \value Normal A normal state. If the state has no child states, it is an
  atomic state; otherwise, the child states are mutually exclusive and an
  initial state must be set by calling QtState::setInitialState().

  \value ParallelGroup The state is a parallel group state. When a parallel
  group state is entered, all its child states are entered in parallel.
*/

/*!
  \enum QtState::HistoryType

  This enum specifies the type of history that a QtHistoryState records.

  \value ShallowHistory Only the immediate child states of the parent state
  are recorded. In this case a transition with the history state as its
  target will end up in the immediate child state that the parent was in the
  last time it was exited. This is the default.

  \value DeepHistory Nested states are recorded. In this case a transition
  with the history state as its target will end up in the most deeply nested
  descendant state the parent was in the last time it was exited.
*/

QtStatePrivate::QtStatePrivate()
    : hasUnregisteredTransitions(false), errorState(0),
      isParallelGroup(false), initialState(0)
{
}

QtStatePrivate::~QtStatePrivate()
{
}

QtStatePrivate *QtStatePrivate::get(QtState *q)
{
    return q->d_func();
}

const QtStatePrivate *QtStatePrivate::get(const QtState *q)
{
    return q->d_func();
}

/*!
  Constructs a new state with the given \a parent state.
*/
QtState::QtState(QtState *parent)
    : QtActionState(*new QtStatePrivate, parent)
{
}

/*!
  Constructs a new state of the given \a type with the given \a parent state.
*/
QtState::QtState(Type type, QtState *parent)
    : QtActionState(*new QtStatePrivate, parent)
{
    Q_D(QtState);
    d->isParallelGroup = (type == ParallelGroup);
}

/*!
  \internal
*/
QtState::QtState(QtStatePrivate &dd, QtState *parent)
    : QtActionState(dd, parent)
{
}

/*!
  Destroys this state.
*/
QtState::~QtState()
{
}

QList<QtAbstractState*> QtStatePrivate::childStates() const
{
    QList<QtAbstractState*> result;
    QList<QObject*>::const_iterator it;
#ifdef QT_STATEMACHINE_SOLUTION
    const QObjectList &children = q_func()->children();
#endif
    for (it = children.constBegin(); it != children.constEnd(); ++it) {
        QtAbstractState *s = qobject_cast<QtAbstractState*>(*it);
        if (!s || qobject_cast<QtHistoryState*>(s))
            continue;
        result.append(s);
    }
    return result;
}

QList<QtHistoryState*> QtStatePrivate::historyStates() const
{
    QList<QtHistoryState*> result;
    QList<QObject*>::const_iterator it;
#ifdef QT_STATEMACHINE_SOLUTION
    const QObjectList &children = q_func()->children();
#endif
    for (it = children.constBegin(); it != children.constEnd(); ++it) {
        QtHistoryState *h = qobject_cast<QtHistoryState*>(*it);
        if (h)
            result.append(h);
    }
    return result;
}

QList<QtAbstractTransition*> QtStatePrivate::transitions() const
{
    QList<QtAbstractTransition*> result;
    QList<QObject*>::const_iterator it;
#ifdef QT_STATEMACHINE_SOLUTION
    const QObjectList &children = q_func()->children();
#endif
    for (it = children.constBegin(); it != children.constEnd(); ++it) {
        QtAbstractTransition *t = qobject_cast<QtAbstractTransition*>(*it);
        if (t)
            result.append(t);
    }
    return result;
}

/*!
  Returns this state group's error state. 

  \sa QtStateMachine::errorState(), QtStateMachine::setErrorState()
*/
QtAbstractState *QtState::errorState() const
{
    Q_D(const QtState);
    return d->errorState;
}

/*!
  Sets this state's error state to be the given \a state. If the error state
  is not set, or if it is set to 0, the state will inherit its parent's error
  state recursively.

  \sa QtStateMachine::setErrorState(), QtStateMachine::errorState()
*/
void QtState::setErrorState(QtAbstractState *state)
{
    Q_D(QtState);
    if (state != 0 && QtAbstractStatePrivate::get(state)->machine() != d->machine()) {
        qWarning("QtState::setErrorState: error state cannot belong "
                 "to a different state machine");
        return;
    }

    d->errorState = state;
}

/*!
  Adds the given \a transition. The transition has this state as the source,
  and the given \a target as the target state.

  \sa removeTransition()
*/
void QtState::addTransition(QtAbstractTransition *transition,
                           QtAbstractState *target)
{
    addTransition(transition, QList<QtAbstractState*>() << target);
}

/*!
  Adds the given \a transition. The transition has this state as the source,
  and the given \a targets as the target states.
*/
void QtState::addTransition(QtAbstractTransition *transition,
                           const QList<QtAbstractState*> &targets)
{
    Q_D(QtState);
    if (!transition) {
        qWarning("QtState::addTransition: cannot add null transition");
        return;
    }
    for (int i = 0; i < targets.size(); ++i) {
        if (!targets.at(i)) {
            qWarning("QtAbstractState::addTransition: cannot add transition to null state");
            return;
        }
        if (QtAbstractStatePrivate::get(targets.at(i))->machine() != d->machine()) {
            qWarning("QtAbstractState::addTransition: cannot add transition "
                     "to a state in a different state machine");
            return;
        }
    }
    Q_ASSERT(transition->parent() == 0);
    Q_ASSERT(QtAbstractTransitionPrivate::get(transition)->targetStates.isEmpty());
    transition->setParent(this);
    QtAbstractTransitionPrivate::get(transition)->targetStates = targets;
    d->hasUnregisteredTransitions = true;
}

/*!
  Adds a transition associated with the given \a signal of the given \a sender
  object. The transition has this state as the source, and the given \a target
  as the target state.
*/
void QtState::addTransition(const QObject *sender, const char *signal,
                           QtAbstractState *target)
{
    if (!sender) {
        qWarning("QtState::addTransition: sender cannot be null");
        return;
    }
    if (!signal) {
        qWarning("QtState::addTransition: signal cannot be null");
        return;
    }
    addTransition(new QtSignalTransition(sender, signal), target);
}

/*!
  Adds the given \a transition. The transition has this state as the source,
  and has no target.
*/
void QtState::addTransition(QtAbstractTransition *transition)
{
    addTransition(transition, QList<QtAbstractState*>());
}

/*!
  Adds a transition that's triggered by the finished event of this state, and
  that has the given \a target state.

  \sa QtStateFinishedEvent
*/
void QtState::addFinishedTransition(QtAbstractState *target)
{
    addTransition(new QtStateFinishedTransition(this), target);
}

namespace {

class UnconditionalTransition : public QtAbstractTransition
{
protected:
    void onTransition() {}
    bool testCondition(QEvent *) const { return true; }
};

} // namespace

/*!
  Adds an unconditional transition from this state to the given \a target
  state.
*/
void QtState::addTransition(QtAbstractState *target)
{
    addTransition(new UnconditionalTransition(), target);
}

/*!
  Removes the given \a transition from this state.  The state releases
  ownership of the transition.

  \sa addTransition()
*/
void QtState::removeTransition(QtAbstractTransition *transition)
{
    if (!transition) {
        qWarning("QtAbstractState::removeTransition: cannot remove null transition");
        return;
    }
    if (transition->sourceState() != this) {
        qWarning("QtAbstractState::removeTransition: transition %p's source state (%p)"
                 " is different from this state (%p)",
                 transition, transition->sourceState(), this);
        return;
    }
    transition->setParent(0);
    QtAbstractTransitionPrivate::get(transition)->targetStates.clear();
}

/*!
  Creates a history state of the given \a type for this state and returns the
  new state.  The history state becomes a child of this state.
*/
QtHistoryState *QtState::addHistoryState(HistoryType type)
{
    return QtHistoryStatePrivate::create(type, this);
}

/*!
  \reimp
*/
void QtState::onEntry()
{
    QtActionState::onEntry();
}

/*!
  \reimp
*/
void QtState::onExit()
{
    QtActionState::onExit();
}

#ifndef QT_NO_ANIMATION

/*!
  \overload addAnimatedTransition()

  Adds an animated transition from the current state to \a targetState for \a animation. 

  This function creates a QtSignalTransition for the \a sender and \a signal, and calls 
  addAnimatedTransition() with this transition object.
*/
QtAnimationState *QtState::addAnimatedTransition(const QObject *sender, const char *signal,
                                               QtAbstractState *targetState,
                                               QtAbstractAnimation *animation)
{
    return addAnimatedTransition(new QtSignalTransition(sender, signal), targetState, animation);
}

/*!
  Adds an animated transition from the current state to \a targetState. 
  
  The animated transition has an intermediate QtAnimationState which plays \a animation before 
  entering \a targetState. This QtAnimationState will be entered when \a transition is taken by the 
  state machine. When the animation has finished playing, the \a targetState will be entered.

  The new QtAnimationState object will become a child of \a targetState's parent state.

  \code
    QPushButton button;
    QtPropertyAnimation animation(&button, "geometry");
    animation.setEndValue(QRect(100, 100, 400, 400));

    QtStateMachine machine;

    QtState *s1 = new QtState();
    QtState *s2 = new QtState();

    QtTransition *transition = new QtTransition(MyEventType);
    s1->addAnimatedTransition(transition, s2, &animation);
  \endcode

  The function returns the new QtAnimationState. This state can be used if you want to add additional 
  transitions into or out from the animation state, and if you want to add additional animations.

  \sa QtAnimationState
*/
QtAnimationState *QtState::addAnimatedTransition(QtAbstractTransition *transition,
                                               QtAbstractState *targetState, 
                                               QtAbstractAnimation *animation)
{
    if (!targetState) {
        qWarning("QtAbstractState::addAnimatedTransition: cannot add transition to null state");
        return 0;
    }
    QtAnimationState *animState = new QtAnimationState(animation, targetState->parentState());
    animState->addFinishedTransition(targetState);
    addTransition(transition, animState);
    return animState;
}

#endif

/*!
  Returns this state's initial state, or 0 if the state has no initial state.
*/
QtAbstractState *QtState::initialState() const
{
    Q_D(const QtState);
    return d->initialState;
}

/*!
  Sets this state's initial state to be the given \a state.
  \a state has to be a child of this state.
*/
void QtState::setInitialState(QtAbstractState *state)
{
    Q_D(QtState);
    if (d->isParallelGroup) {
        qWarning("QtState::setInitialState: ignoring attempt to set initial state "
                 "of parallel state group %p", this);
        return;
    }
    if (state && (state->parentState() != this)) {
        qWarning("QtState::setInitialState: state %p is not a child of this state (%p)",
                 state, this);
        return;
    }
    d->initialState = state;
}

/*!
  \reimp
*/
bool QtState::event(QEvent *e)
{
    return QtActionState::event(e);
}

QT_END_NAMESPACE
