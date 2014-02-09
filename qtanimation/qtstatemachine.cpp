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



#include "qtstatemachine.h"
#include "qtstatemachine_p.h"
#include "qtabstracttransition.h"
#include "qtabstracttransition_p.h"
#include "qtsignaltransition.h"
#include "qtsignaltransition_p.h"
#include "qtsignalevent.h"
#include "qtsignaleventgenerator_p.h"
#include "qtabstractstate.h"
#include "qtabstractstate_p.h"
#include "qtactionstate.h"
#include "qtactionstate_p.h"
#include "qtfinalstate.h"
#include "qthistorystate.h"
#include "qthistorystate_p.h"
#include "qtstatefinishedevent.h"
#include "qtstatefinishedtransition.h"
#include <QtCore/qmetaobject.h>
#include "qtstate.h"
#include "qtstate_p.h"
#include "qtstateaction.h"
#include "qtstateaction_p.h"
#ifndef QT_STATEMACHINE_SOLUTION
#include "private/qobject_p.h"
#include "private/qthread_p.h"
#endif

#ifndef QT_NO_STATEMACHINE_EVENTFILTER
#include "qteventtransition.h"
#include "qteventtransition_p.h"
#include "qtboundevent_p.h"
#endif

#ifndef QT_NO_ANIMATION
#include "qtpropertyanimation.h"
#include "qtanimationstate.h"
#endif

#include <qdebug.h>

QT_BEGIN_NAMESPACE

/*!
  \class QtStateMachine
  \reentrant

  \brief The QtStateMachine class provides a hierarchical finite state machine.

  

  The QtStateMachine class provides a hierarchical finite state machine based
  on \l{Statecharts: A visual formalism for complex systems}{Statecharts}
  concepts and notation. QtStateMachine is part of \l{The State Machine
  Framework}.

  A state machine manages a set of states (QtAbstractState objects) and
  transitions (QtAbstractTransition objects) between those states; the states
  and the transitions collectively define a state graph. Once a state graph
  has been defined, the state machine can execute it. QtStateMachine's
  execution algorithm is based on the \l{State Chart XML: State Machine
  Notation for Control Abstraction}{State Chart XML (SCXML)} algorithm.

  The QtState class provides a state that you can use to set properties and
  invoke methods on QObjects when the state is entered or exited. This is
  typically used in conjunction with \l{Signals and Slots}{signals}; the
  signals determine the flow of the state graph, whereas the states' property
  assigments and method invocations are the actions.

  Use the addState() function to add a state to the state machine;
  alternatively, pass the machine's rootState() to the state constructor.  Use
  the removeState() function to remove a state from the state machine.

  The following snippet shows a state machine that will finish when a button
  is clicked:

  \code
  QPushButton button;

  QtStateMachine machine;
  QtState *s1 = new QtState();
  s1->setPropertyOnEntry(&button, "text", "Click me");

  QtFinalState *s2 = new QtFinalState();
  s1->addTransition(&button, SIGNAL(clicked()), s2);

  machine.addState(s1);
  machine.addState(s2);
  machine.setInitialState(s1);
  machine.start();
  \endcode

  The setInitialState() function sets the state machine's initial state; this
  state is entered when the state machine is started.

  The start() function starts the state machine. The state machine executes
  asynchronously, i.e. you need to run an event loop in order for it to make
  progress. The started() signal is emitted when the state machine has entered
  the initial state.

  The state machine processes events and takes transitions until a top-level
  final state is entered; the state machine then emits the finished() signal.

  The stop() function stops the state machine. The stopped() signal is emitted
  when the state machine has stopped.

  The postEvent() function posts an event to the state machine. This is useful
  when you are using custom events to trigger transitions.

  The rootState() function returns the state machine's root state. All
  top-level states have the root state as their parent.

  \sa QtAbstractState, QtAbstractTransition
*/

/*!
    \property QtStateMachine::rootState

    \brief the root state of this state machine
*/

/*!
    \property QtStateMachine::initialState

    \brief the initial state of this state machine
*/

/*!
    \property QtStateMachine::errorState

    \brief the error state of this state machine
*/

/*!
    \property QtStateMachine::errorString

    \brief the error string of this state machine
*/

// #define QSTATEMACHINE_DEBUG
// #define QT_NO_STATEMACHINE_RESTOREPROPERTIES

QtStateMachinePrivate::QtStateMachinePrivate()
{
    state = NotRunning;
    processing = false;
    processingScheduled = false;
    stop = false;
    error = QtStateMachine::NoError;
    globalRestorePolicy = QtState::DoNotRestoreProperties;
    rootState = 0;
    initialErrorStateForRoot = 0;
#ifndef QT_STATEMACHINE_SOLUTION
    signalEventGenerator = 0;
#endif
}

QtStateMachinePrivate::~QtStateMachinePrivate()
{
    qDeleteAll(internalEventQueue);
    qDeleteAll(externalEventQueue);
}

QtStateMachinePrivate *QtStateMachinePrivate::get(QtStateMachine *q)
{
    if (q)
        return q->d_func();
    return 0;
}

static QEvent *cloneEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::None:
        return new QEvent(*e);
    case QEvent::Timer:
        return new QTimerEvent(*static_cast<QTimerEvent*>(e));
    default:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    }
    return 0;
}

const QtStateMachinePrivate::Handler qt_kernel_statemachine_handler = {
    cloneEvent
};

const QtStateMachinePrivate::Handler *QtStateMachinePrivate::handler = &qt_kernel_statemachine_handler;

Q_ANIMATION_EXPORT const QtStateMachinePrivate::Handler *qcoreStateMachineHandler()
{
    return &qt_kernel_statemachine_handler;
}

bool QtStateMachinePrivate::stateEntryLessThan(QtAbstractState *s1, QtAbstractState *s2)
{
    if (s1->parent() == s2->parent()) {
        return s1->children().indexOf(s1)
            < s2->children().indexOf(s2);
    } else if (isDescendantOf(s1, s2)) {
        return false;
    } else if (isDescendantOf(s2, s1)) {
        return true;
    } else {
        // ### fixme
        return s1 < s2;
    }
}

bool QtStateMachinePrivate::stateExitLessThan(QtAbstractState *s1, QtAbstractState *s2)
{
    if (s1->parent() == s2->parent()) {
        return s1->children().indexOf(s1)
            < s2->children().indexOf(s2);
    } else if (isDescendantOf(s1, s2)) {
        return true;
    } else if (isDescendantOf(s2, s1)) {
        return false;
    } else {
        // ### fixme
        return s2 < s1;
    }
}

QtState *QtStateMachinePrivate::findLCA(const QList<QtAbstractState*> &states)
{
    if (states.isEmpty())
        return 0;
    QList<QtState*> ancestors = properAncestors(states.at(0), 0);
    for (int i = 0; i < ancestors.size(); ++i) {
        QtState *anc = ancestors.at(i);
        bool ok = true;
        for (int j = states.size() - 1; (j > 0) && ok; --j) {
            const QtAbstractState *s = states.at(j);
            if (!isDescendantOf(s, anc))
                ok = false;
        }
        if (ok)
            return anc;
    }
    return 0;
}

bool QtStateMachinePrivate::isPreempted(const QtAbstractState *s, const QSet<QtAbstractTransition*> &transitions) const
{
    QSet<QtAbstractTransition*>::const_iterator it;
    for (it = transitions.constBegin(); it != transitions.constEnd(); ++it) {
        QtAbstractTransition *t = *it;
        QList<QtAbstractState*> lst = t->targetStates();
        if (!lst.isEmpty()) {
            lst.prepend(t->sourceState());
            QtAbstractState *lca = findLCA(lst);
            if (isDescendantOf(s, lca)) {
#ifdef QSTATEMACHINE_DEBUG
                qDebug() << q_func() << ":" << transitions << "preempts selection of a transition from"
                         << s << "because" << s << "is a descendant of" << lca;
#endif
                return true;
            }
        }
    }
    return false;
}

QSet<QtAbstractTransition*> QtStateMachinePrivate::selectTransitions(QEvent *event) const
{
    Q_Q(const QtStateMachine);
    QSet<QtAbstractTransition*> enabledTransitions;
    QSet<QtAbstractState*>::const_iterator it;
    const_cast<QtStateMachine*>(q)->beginSelectTransitions(event);
    for (it = configuration.constBegin(); it != configuration.constEnd(); ++it) {
        QtAbstractState *state = *it;
        if (!isAtomic(state))
            continue;
        if (isPreempted(state, enabledTransitions))
            continue;
        QList<QtState*> lst = properAncestors(state, 0);
        if (QtState *grp = qobject_cast<QtState*>(state))
            lst.prepend(grp);
        bool found = false;
        for (int j = 0; (j < lst.size()) && !found; ++j) {
            QtState *s = lst.at(j);
            QList<QtAbstractTransition*> transitions = QtStatePrivate::get(s)->transitions();
            for (int k = 0; k < transitions.size(); ++k) {
                QtAbstractTransition *t = transitions.at(k);
                if (!event && QtAbstractTransitionPrivate::get(t)->eventTypes.isEmpty()
                    && QtAbstractTransitionPrivate::get(t)->callTestCondition(0)) {
#ifdef QSTATEMACHINE_DEBUG
                    qDebug() << q << ": selecting event-less transition" << t;
#endif
                    enabledTransitions.insert(t);
                    found = true;
                    break;
                } else if (event && QtAbstractTransitionPrivate::get(t)->eventTypes.contains(event->type())
                           && QtAbstractTransitionPrivate::get(t)->callTestCondition(event)) {
#ifdef QSTATEMACHINE_DEBUG
                    qDebug() << q << ": selecting transition" << t;
#endif
                    enabledTransitions.insert(t);
                    found = true;
                    break;
                }
            }
        }
    }
    const_cast<QtStateMachine*>(q)->endSelectTransitions(event);
    return enabledTransitions;
}

void QtStateMachinePrivate::microstep(const QList<QtAbstractTransition*> &enabledTransitions)
{
#ifdef QSTATEMACHINE_DEBUG
    qDebug() << q_func() << ": begin microstep( enabledTransitions:" << enabledTransitions << ")";
    qDebug() << q_func() << ": configuration before exiting states:" << configuration;
#endif
    exitStates(enabledTransitions);
#ifdef QSTATEMACHINE_DEBUG
    qDebug() << q_func() << ": configuration after exiting states:" << configuration;
#endif
    executeTransitionContent(enabledTransitions);
    enterStates(enabledTransitions);
#ifdef QSTATEMACHINE_DEBUG
    qDebug() << q_func() << ": configuration after entering states:" << configuration;
    qDebug() << q_func() << ": end microstep";
#endif
}

void QtStateMachinePrivate::exitStates(const QList<QtAbstractTransition*> &enabledTransitions)
{
//    qDebug() << "exitStates(" << enabledTransitions << ")";
    QSet<QtAbstractState*> statesToExit;
//    QSet<QtAbstractState*> statesToSnapshot;
    for (int i = 0; i < enabledTransitions.size(); ++i) {
        QtAbstractTransition *t = enabledTransitions.at(i);
        QList<QtAbstractState*> lst = t->targetStates();
        if (lst.isEmpty())
            continue;
        lst.prepend(t->sourceState());
        QtAbstractState *lca = findLCA(lst);
        {
            QSet<QtAbstractState*>::const_iterator it;
            for (it = configuration.constBegin(); it != configuration.constEnd(); ++it) {
                QtAbstractState *s = *it;
                if (isDescendantOf(s, lca))
                    statesToExit.insert(s);
            }
        }
    }
    QList<QtAbstractState*> statesToExit_sorted = statesToExit.toList();
    qSort(statesToExit_sorted.begin(), statesToExit_sorted.end(), stateExitLessThan);
    for (int i = 0; i < statesToExit_sorted.size(); ++i) {
        QtAbstractState *s = statesToExit_sorted.at(i);
        if (QtState *grp = qobject_cast<QtState*>(s)) {
            QList<QtHistoryState*> hlst = QtStatePrivate::get(grp)->historyStates();
            for (int j = 0; j < hlst.size(); ++j) {
                QtHistoryState *h = hlst.at(j);
                QtHistoryStatePrivate::get(h)->configuration.clear();
                QSet<QtAbstractState*>::const_iterator it;
                for (it = configuration.constBegin(); it != configuration.constEnd(); ++it) {
                    QtAbstractState *s0 = *it;
                    if (QtHistoryStatePrivate::get(h)->historyType == QtState::DeepHistory) {
                        if (isAtomic(s0) && isDescendantOf(s0, s))
                            QtHistoryStatePrivate::get(h)->configuration.append(s0);
                    } else if (s0->parentState() == s) {
                        QtHistoryStatePrivate::get(h)->configuration.append(s0);
                    }
                }
#ifdef QSTATEMACHINE_DEBUG
                qDebug() << q_func() << ": recorded" << ((QtHistoryStatePrivate::get(h)->historyType == QtState::DeepHistory) ? "deep" : "shallow")
                         << "history for" << s << "in" << h << ":" << QtHistoryStatePrivate::get(h)->configuration;
#endif
            }
        }
    }
    for (int i = 0; i < statesToExit_sorted.size(); ++i) {
        QtAbstractState *s = statesToExit_sorted.at(i);
#ifdef QSTATEMACHINE_DEBUG
        qDebug() << q_func() << ": exiting" << s;
#endif
        QtAbstractStatePrivate::get(s)->callOnExit();
        configuration.remove(s);
    }
}

void QtStateMachinePrivate::executeTransitionContent(const QList<QtAbstractTransition*> &enabledTransitions)
{
    for (int i = 0; i < enabledTransitions.size(); ++i) {
        QtAbstractTransition *t = enabledTransitions.at(i);
#ifdef QSTATEMACHINE_DEBUG
        qDebug() << q_func() << ": triggering" << t;
#endif
        QtAbstractTransitionPrivate::get(t)->callOnTransition();
    }
}

void QtStateMachinePrivate::enterStates(const QList<QtAbstractTransition*> &enabledTransitions)
{
#ifdef QSTATEMACHINE_DEBUG
    Q_Q(QtStateMachine);
#endif
//    qDebug() << "enterStates(" << enabledTransitions << ")";
    QSet<QtAbstractState*> statesToEnter;
    QSet<QtAbstractState*> statesForDefaultEntry;

    for (int i = 0; i < enabledTransitions.size(); ++i) {
        QtAbstractTransition *t = enabledTransitions.at(i);
        QList<QtAbstractState*> lst = t->targetStates();
        if (lst.isEmpty())
            continue;
        lst.prepend(t->sourceState());
        QtState *lca = findLCA(lst);
        for (int j = 1; j < lst.size(); ++j) {
            QtAbstractState *s = lst.at(j);
            if (QtHistoryState *h = qobject_cast<QtHistoryState*>(s)) {
                QList<QtAbstractState*> hconf = QtHistoryStatePrivate::get(h)->configuration;
                if (!hconf.isEmpty()) {
                    for (int k = 0; k < hconf.size(); ++k) {
                        QtAbstractState *s0 = hconf.at(k);
                        addStatesToEnter(s0, lca, statesToEnter, statesForDefaultEntry);
                    }
#ifdef QSTATEMACHINE_DEBUG
                    qDebug() << q << ": restoring"
                            << ((QtHistoryStatePrivate::get(h)->historyType == QtState::DeepHistory) ? "deep" : "shallow")
                            << "history from" << s << ":" << hconf;
#endif
                } else {
                    QList<QtAbstractState*> hlst;
                    if (QtHistoryStatePrivate::get(h)->defaultState)
                        hlst.append(QtHistoryStatePrivate::get(h)->defaultState);
                    if (hlst.isEmpty()) {
                        setError(QtStateMachine::NoDefaultStateInHistoryState, h);
                    } else {
                        for (int k = 0; k < hlst.size(); ++k) {
                            QtAbstractState *s0 = hlst.at(k);
                            addStatesToEnter(s0, lca, statesToEnter, statesForDefaultEntry);
                        }
#ifdef QSTATEMACHINE_DEBUG
                        qDebug() << q << ": initial history targets for" << s << ":" << hlst;
#endif
                    }
                }
            } else {
                addStatesToEnter(s, lca, statesToEnter, statesForDefaultEntry);
            }
        }
    }

    // Did an error occur while selecting transitions? Then we enter the error state.
    if (!pendingErrorStates.isEmpty()) {
        statesToEnter.clear();
        statesToEnter = pendingErrorStates;
        statesForDefaultEntry = pendingErrorStatesForDefaultEntry;
        pendingErrorStates.clear();
        pendingErrorStatesForDefaultEntry.clear();
    }

    QList<QtAbstractState*> statesToEnter_sorted = statesToEnter.toList();
    qSort(statesToEnter_sorted.begin(), statesToEnter_sorted.end(), stateEntryLessThan);

#ifndef QT_NO_STATEMACHINE_RESTOREPROPERTIES
    bool hasAnimationState = false;

    QHash<RestorableId, QVariant> pendingRestorables = registeredRestorables;
    for (int i = 0; i < statesToEnter_sorted.size(); ++i) {
        QtAbstractState *as = statesToEnter_sorted.at(i);

#ifndef QT_NO_ANIMATION
        // If we are going to an animation state, it will restore properties for us
        hasAnimationState = hasAnimationState
                         || qobject_cast<QtAnimationState*>(as) != 0;
#endif

        QtActionState *s = qobject_cast<QtActionState*>(as);
        if (s == 0)
            continue;

        QtActionState::RestorePolicy restorePolicy = s->restorePolicy();
        if (restorePolicy == QtActionState::GlobalRestorePolicy)
            restorePolicy = globalRestorePolicy;

        if (restorePolicy == QtActionState::DoNotRestoreProperties)
            continue;

        QtActionStatePrivate *s_d = QtActionStatePrivate::get(s);
        QList<QtStateAction*> actions = s_d->entryActions();

        for (int j = 0; j < actions.size(); ++j) {
            QtStateSetPropertyAction *spa = qobject_cast<QtStateSetPropertyAction*>(actions.at(j));
            if (spa == 0 || spa->targetObject() == 0)
                continue;

            registerRestorable(spa->targetObject(), spa->propertyName());
            pendingRestorables.remove(RestorableId(spa->targetObject(), spa->propertyName()));
        }
    }

#ifndef QT_NO_ANIMATION
    // If the configuration has an animation state, we do not want to restore here, as we
    // might be transitioning inside the animation state (into the final state for instance.)
    // We let the animation state handle the restoration.
    QSet<QtAbstractState*>::const_iterator it;
    for (it=configuration.constBegin(); !hasAnimationState && it!=configuration.constEnd(); ++it)
        hasAnimationState = qobject_cast<QtAnimationState*>(*it) != 0;
#endif

    if (!hasAnimationState) {
        QHash<RestorableId, QVariant>::const_iterator it;
        for (it = pendingRestorables.constBegin(); it != pendingRestorables.constEnd(); ++it) {
            QObject *object = it.key().first;
            QByteArray propertyName = it.key().second;

            object->setProperty(propertyName, it.value());
            unregisterRestorable(object, propertyName);
        }
    }

#endif // QT_NO_STATEMACHINE_RESTOREPROPERTIES

    for (int i = 0; i < statesToEnter_sorted.size(); ++i) {
        QtAbstractState *s = statesToEnter_sorted.at(i);
#ifdef QSTATEMACHINE_DEBUG
        qDebug() << q << ": entering" << s;
#endif
        configuration.insert(s);
        registerTransitions(s);
        QtAbstractStatePrivate::get(s)->callOnEntry();
        if (statesForDefaultEntry.contains(s)) {
            // ### executeContent(s.initial.transition.children())
        }
        if (isFinal(s)) {
            QtState *parent = s->parentState();
            if (parent) {
                QtState *grandparent = parent->parentState();
#ifdef QSTATEMACHINE_DEBUG
                qDebug() << q << ": posting finished event for" << parent;
#endif
                internalEventQueue.append(new QtStateFinishedEvent(parent));
                if (grandparent && isParallel(grandparent)) {
                    bool allChildStatesFinal = true;
                    QList<QtAbstractState*> childStates = QtStatePrivate::get(grandparent)->childStates();
                    for (int j = 0; j < childStates.size(); ++j) {
                        QtAbstractState *cs = childStates.at(j);
                        if (!isInFinalState(cs)) {
                            allChildStatesFinal = false;
                            break;
                        }
                    }
                    if (allChildStatesFinal) {
#ifdef QSTATEMACHINE_DEBUG
                        qDebug() << q << ": posting finished event for" << grandparent;
#endif
                        internalEventQueue.append(new QtStateFinishedEvent(grandparent));
                    }
                }
            }
        }
    }
    {
        QSet<QtAbstractState*>::const_iterator it;
        for (it = configuration.constBegin(); it != configuration.constEnd(); ++it) {
            if (isFinal(*it) && (*it)->parentState() == rootState) {
                processing = false;
                stopProcessingReason = Finished;
                break;
            }
        }
    }
//    qDebug() << "configuration:" << configuration.toList();
}

void QtStateMachinePrivate::addStatesToEnter(QtAbstractState *s, QtState *root,
                                            QSet<QtAbstractState*> &statesToEnter,
                                            QSet<QtAbstractState*> &statesForDefaultEntry)
{
    statesToEnter.insert(s);
    if (isParallel(s)) {
        QtState *grp = qobject_cast<QtState*>(s);
        QList<QtAbstractState*> lst = QtStatePrivate::get(grp)->childStates();
        for (int i = 0; i < lst.size(); ++i) {
            QtAbstractState *child = lst.at(i);
            addStatesToEnter(child, grp, statesToEnter, statesForDefaultEntry);
        }
    } else if (isCompound(s)) {
        statesForDefaultEntry.insert(s);
        QtState *grp = qobject_cast<QtState*>(s);
        QtAbstractState *initial = grp->initialState();
        if (initial != 0) {
            addStatesToEnter(initial, grp, statesToEnter, statesForDefaultEntry);
        } else {
            setError(QtStateMachine::NoInitialStateError, grp);
            return;
        }
    }
    QList<QtState*> ancs = properAncestors(s, root);
    for (int i = 0; i < ancs.size(); ++i) {
        QtState *anc = ancs.at(i);
        if (!anc->parentState())
            continue;
        statesToEnter.insert(anc);
        if (isParallel(anc)) {
            QList<QtAbstractState*> lst = QtStatePrivate::get(anc)->childStates();
            for (int j = 0; j < lst.size(); ++j) {
                QtAbstractState *child = lst.at(j);
                bool hasDescendantInList = false;
                QSet<QtAbstractState*>::const_iterator it;
                for (it = statesToEnter.constBegin(); it != statesToEnter.constEnd(); ++it) {
                    if (isDescendantOf(*it, child)) {
                        hasDescendantInList = true;
                        break;
                    }
                }
                if (!hasDescendantInList)
                    addStatesToEnter(child, anc, statesToEnter, statesForDefaultEntry);
            }
        }
    }
}

bool QtStateMachinePrivate::isFinal(const QtAbstractState *s)
{
    return qobject_cast<const QtFinalState*>(s) != 0;
}

bool QtStateMachinePrivate::isParallel(const QtAbstractState *s)
{
    const QtState *ss = qobject_cast<const QtState*>(s);
    return ss && QtStatePrivate::get(ss)->isParallelGroup;
}

bool QtStateMachinePrivate::isCompound(const QtAbstractState *s)
{
    const QtState *group = qobject_cast<const QtState*>(s);
    if (!group)
        return false;
    return (!isParallel(group) && !QtStatePrivate::get(group)->childStates().isEmpty())
        || (qobject_cast<QtStateMachine*>(group->parent()) != 0);
}

bool QtStateMachinePrivate::isAtomic(const QtAbstractState *s)
{
    const QtState *ss = qobject_cast<const QtState*>(s);
    return (ss && !QtStatePrivate::get(ss)->isParallelGroup
            && QtStatePrivate::get(ss)->childStates().isEmpty())
        || isFinal(s);
}


bool QtStateMachinePrivate::isDescendantOf(const QtAbstractState *state, const QtAbstractState *other)
{
    for (QtAbstractState *s = state->parentState(); s != 0; s = s->parentState()) {
        if (s == other)
            return true;
    }
    return false;
}

QList<QtState*> QtStateMachinePrivate::properAncestors(const QtAbstractState *state, const QtState *upperBound)
{
    QList<QtState*> result;
    for (QtState *s = state->parentState(); s && s != upperBound; s = s->parentState()) {
        result.append(s);
    }
    return result;
}

bool QtStateMachinePrivate::isInFinalState(QtAbstractState* s) const
{
    if (isCompound(s)) {
        QtState *grp = qobject_cast<QtState*>(s);
        QList<QtAbstractState*> lst = QtStatePrivate::get(grp)->childStates();
        for (int i = 0; i < lst.size(); ++i) {
            QtAbstractState *cs = lst.at(i);
            if (isFinal(cs) && configuration.contains(cs))
                return true;
        }
        return false;
    } else if (isParallel(s)) {
        QtState *grp = qobject_cast<QtState*>(s);
        QList<QtAbstractState*> lst = QtStatePrivate::get(grp)->childStates();
        for (int i = 0; i < lst.size(); ++i) {
            QtAbstractState *cs = lst.at(i);
            if (!isInFinalState(cs))
                return false;
        }
        return true;
    }
    else
        return false;
}

void QtStateMachinePrivate::registerRestorable(QObject *object, const QByteArray &propertyName)
{
    RestorableId id(object, propertyName);
    if (!registeredRestorables.contains(id))
        registeredRestorables.insert(id, object->property(propertyName));
}

/*!
   \internal
   Returns true if the variable with the given \a id has been registered for restoration.
*/
bool QtStateMachinePrivate::hasRestorable(QObject *object, const QByteArray &propertyName) const
{
    return registeredRestorables.contains(RestorableId(object, propertyName));
}

QVariant QtStateMachinePrivate::restorableValue(QObject *object, const QByteArray &propertyName) const
{
    return registeredRestorables.value(RestorableId(object, propertyName), QVariant());
}


/*!
   \internal
    Unregisters the variable identified by \a id
*/
void QtStateMachinePrivate::unregisterRestorable(QObject *object, const QByteArray &propertyName)
{
    RestorableId id(object, propertyName);
    registeredRestorables.remove(id);

#ifndef QT_NO_ANIMATION
    registeredRestorableAnimations.remove(id);
#endif

}

#ifndef QT_NO_ANIMATION
void QtStateMachinePrivate::registerRestorable(QtPropertyAnimation *animation)
{
    // We always want to restore to the first registered value, so if one is already present, we
    // leave it be.
    RestorableId id(animation->targetObject(), animation->propertyName());
    if (!registeredRestorableAnimations.contains(id))
        registeredRestorableAnimations.insert(id, animation);
    registerRestorable(animation->targetObject(), animation->propertyName());
}


/*!
   \internal
   Returns all variables currently registered. The list returned is in no particular order.
*/
QList<QtPropertyAnimation*> QtStateMachinePrivate::restorableAnimations() const
{
    return registeredRestorableAnimations.values();
}

/*!
   \internal
   Returns a reference to the restorable identified by \a id.
*/
QtPropertyAnimation *QtStateMachinePrivate::restorableAnimation(QObject *object,
                                                              const QByteArray &propertyName)
{
    return registeredRestorableAnimations.value(RestorableId(object, propertyName), 0);
}

#endif // QT_NO_ANIMATION

QtAbstractState *QtStateMachinePrivate::findErrorState(QtAbstractState *context)
{
    // If the user sets the root state's error state to 0, we return the initial error state
    if (context == 0)
        return initialErrorStateForRoot;

    // Find error state recursively in parent hierarchy if not set explicitly for context state
    QtAbstractState *errorState = qobject_cast<QtState*>(context)->errorState();
    if (errorState == 0)
        errorState = findErrorState(context->parentState());

    return errorState;
}

void QtStateMachinePrivate::setError(QtStateMachine::Error errorCode, QtAbstractState *currentContext)
{
    error = errorCode;

    switch (errorCode) {
    case QtStateMachine::NoInitialStateError:
        Q_ASSERT(currentContext != 0);

        errorString = QtStateMachine::tr("Missing initial state in compound state '%1'")
                        .arg(currentContext->objectName());

        break;
    case QtStateMachine::NoDefaultStateInHistoryState:
        Q_ASSERT(currentContext != 0);

        errorString = QtStateMachine::tr("Missing default state in history state '%1'")
                        .arg(currentContext->objectName());
        break;
    default:
        errorString = QtStateMachine::tr("Unknown error");
    };

    pendingErrorStates.clear();
    pendingErrorStatesForDefaultEntry.clear();

    QtAbstractState *currentErrorState = findErrorState(currentContext);

    // Avoid infinite loop if the error state itself has an error
    if (currentContext == currentErrorState) {
        Q_ASSERT(currentContext != initialErrorStateForRoot); // RootErrorState is broken
        currentErrorState = initialErrorStateForRoot;
    }

    QtState *lca = findLCA(QList<QtAbstractState*>() << currentErrorState << currentContext);
    addStatesToEnter(currentErrorState, lca, pendingErrorStates, pendingErrorStatesForDefaultEntry);
}

namespace {

class StartState : public QtState
{
public:
    StartState(QtState *parent)
        : QtState(parent) {}
protected:
    void onEntry() {}
    void onExit() {}
};

class InitialTransition : public QtAbstractTransition
{
public:
    InitialTransition() {}
protected:
    virtual void onTransition() {}
};

} // namespace

void QtStateMachinePrivate::_q_start()
{
    Q_Q(QtStateMachine);
    Q_ASSERT(state == Starting);
    if (!rootState) {
        state = NotRunning;
        return;
    }
    QtAbstractState *initial = rootState->initialState();
    if (initial == 0)
        setError(QtStateMachine::NoInitialStateError, rootState);

    configuration.clear();
    qDeleteAll(internalEventQueue);
    internalEventQueue.clear();
    qDeleteAll(externalEventQueue);
    externalEventQueue.clear();

#ifdef QSTATEMACHINE_DEBUG
    qDebug() << q << ": starting";
#endif
    state = Running;
    processingScheduled = true; // we call _q_process() below
    emit q->started();

    StartState *start = new StartState(rootState);
    QtAbstractTransition *initialTransition = new InitialTransition();
    start->addTransition(initialTransition, initial);
    QList<QtAbstractTransition*> transitions;
    transitions.append(initialTransition);
    executeTransitionContent(transitions);
    enterStates(transitions);
    delete start;

#ifdef QSTATEMACHINE_DEBUG
    qDebug() << q << ": initial configuration:" << configuration;
#endif
    _q_process();
}

void QtStateMachinePrivate::_q_process()
{
    Q_Q(QtStateMachine);
    Q_ASSERT(state == Running);
    Q_ASSERT(!processing);
    processing = true;
    processingScheduled = false;
#ifdef QSTATEMACHINE_DEBUG
    qDebug() << q << ": starting the event processing loop";
#endif
    while (processing) {
        if (stop) {
            stop = false;
            processing = false;
            stopProcessingReason = Stopped;
            break;
        }
        QEvent *e = 0;
        QSet<QtAbstractTransition*> enabledTransitions;
        enabledTransitions = selectTransitions(0);
        if (enabledTransitions.isEmpty() && !internalEventQueue.isEmpty()) {
            e = internalEventQueue.takeFirst();
#ifdef QSTATEMACHINE_DEBUG
            qDebug() << q << ": dequeued internal event" << e;
#endif
            enabledTransitions = selectTransitions(e);
            if (enabledTransitions.isEmpty()) {
                delete e;
                e = 0;
            }
        }
        if (enabledTransitions.isEmpty()) {
            if (externalEventQueue.isEmpty()) {
                if (internalEventQueue.isEmpty()) {
                    processing = false;
                    stopProcessingReason = EventQueueEmpty;
                }
            } else {
                e = externalEventQueue.takeFirst();
#ifdef QSTATEMACHINE_DEBUG
                qDebug() << q << ": dequeued external event" << e;
#endif
                enabledTransitions = selectTransitions(e);
                if (enabledTransitions.isEmpty()) {
                    delete e;
                    e = 0;
                }
            }
        }
        if (!enabledTransitions.isEmpty()) {
            q->beginMicrostep(e);
            microstep(enabledTransitions.toList());
            q->endMicrostep(e);
        }
#ifdef QSTATEMACHINE_DEBUG
        else {
            qDebug() << q << ": no transitions enabled";
        }
#endif
        delete e;
    }
#ifdef QSTATEMACHINE_DEBUG
    qDebug() << q << ": finished the event processing loop";
#endif
    switch (stopProcessingReason) {
    case EventQueueEmpty:
        break;
    case Finished:
        state = NotRunning;
        emit q->finished();
        break;
    case Stopped:
        state = NotRunning;
        emit q->stopped();
        break;
    }
}

void QtStateMachinePrivate::scheduleProcess()
{
    if ((state != Running) || processing || processingScheduled)
        return;
    processingScheduled = true;
    QMetaObject::invokeMethod(q_func(), "_q_process", Qt::QueuedConnection);
}

void QtStateMachinePrivate::registerTransitions(QtAbstractState *state)
{
    QtState *group = qobject_cast<QtState*>(state);
    if (!group)
        return;
    if (!QtStatePrivate::get(group)->hasUnregisteredTransitions)
        return;
    QList<QtAbstractTransition*> transitions = QtStatePrivate::get(group)->transitions();
    for (int i = 0; i < transitions.size(); ++i) {
        QtAbstractTransition *t = transitions.at(i);
        if (QtSignalTransition *st = qobject_cast<QtSignalTransition*>(t)) {
            registerSignalTransition(st);
        }
#ifndef QT_NO_STATEMACHINE_EVENTFILTER
        else if (QtEventTransition *oet = qobject_cast<QtEventTransition*>(t)) {
            registerEventTransition(oet);
        }
#endif
    }
    QtStatePrivate::get(group)->hasUnregisteredTransitions = false;
}

#ifndef QT_STATEMACHINE_SOLUTION

static int senderSignalIndex(const QObject *sender)
{
    QObjectPrivate *d = QObjectPrivate::get(const_cast<QObject*>(sender));
    QMutexLocker(&d->threadData->mutex);
    if (!d->currentSender)
        return -1;

    // Return -1 if d->currentSender isn't in d->senders
    bool found = false;
    for (int i = 0; !found && i < d->senders.count(); ++i)
        found = (d->senders.at(i).sender == d->currentSender->sender);
    if (!found)
        return -1;
    return d->currentSender->signal;
}

#endif

void QtStateMachinePrivate::registerSignalTransition(QtSignalTransition *transition)
{
    Q_Q(QtStateMachine);
    if (QtSignalTransitionPrivate::get(transition)->signalIndex != -1)
        return; // already registered
    const QObject *sender = QtSignalTransitionPrivate::get(transition)->sender;
    QByteArray signal = QtSignalTransitionPrivate::get(transition)->signal;
    int signalIndex = sender->metaObject()->indexOfSignal(signal.mid(1));
    if (signalIndex == -1)
        return;
    QList<int> &connectedSignalIndexes = connections[sender];
    if (!connectedSignalIndexes.contains(signalIndex)) {
#ifndef QT_STATEMACHINE_SOLUTION
        if (!signalEventGenerator)
            signalEventGenerator = new QtSignalEventGenerator(q);
#else
        QtSignalEventGenerator *signalEventGenerator = new QtSignalEventGenerator(signalIndex, q);
#endif
        bool ok = QMetaObject::connect(sender, signalIndex, signalEventGenerator,
                                       signalEventGenerator->metaObject()->methodOffset());
        if (!ok) {
#ifdef QSTATEMACHINE_DEBUG
            qDebug() << q << ": FAILED to add signal transition from" << transition->sourceState()
                     << ": ( sender =" << sender << ", signal =" << (signal.mid(1))
                     << ", targets =" << transition->targetStates() << ")";
#endif
            return;
        }
        connectedSignalIndexes.append(signalIndex);
    }
    QtSignalTransitionPrivate::get(transition)->signalIndex = signalIndex;
#ifdef QSTATEMACHINE_DEBUG
    qDebug() << q << ": added signal transition from" << transition->sourceState()
             << ": ( sender =" << sender << ", signal =" << (signal.mid(1))
             << ", targets =" << transition->targetStates() << ")";
#endif
}

#ifndef QT_NO_STATEMACHINE_EVENTFILTER
void QtStateMachinePrivate::registerEventTransition(QtEventTransition *transition)
{
    Q_Q(QtStateMachine);
    if (QtEventTransitionPrivate::get(transition)->registered)
        return;
    QtAbstractTransition *tx = QtEventTransitionPrivate::get(transition)->transition;
    QList<QEvent::Type> eventTypes = QtAbstractTransitionPrivate::get(tx)->eventTypes;
    for (int i = 0; i < eventTypes.size(); ++i) {
        if (eventTypes.at(i) > QEvent::User) {
            qWarning("QObject event transitions are not supported for custom types");
            return;
        }
    }
    QObject *object = QtEventTransitionPrivate::get(transition)->object;
#ifndef QT_STATEMACHINE_SOLUTION
    QObjectPrivate *od = QObjectPrivate::get(object);
    if (!od->eventFilters.contains(q))
#endif
        object->installEventFilter(q);
    for (int i = 0; i < eventTypes.size(); ++i)
        qobjectEvents[object].insert(eventTypes.at(i));
    QtEventTransitionPrivate::get(transition)->registered = true;
}
#endif

void QtStateMachinePrivate::handleTransitionSignal(const QObject *sender, int signalIndex,
                                                  void **argv)
{
    const QList<int> &connectedSignalIndexes = connections[sender];
    Q_ASSERT(connectedSignalIndexes.contains(signalIndex));
    const QMetaObject *meta = sender->metaObject();
    QMetaMethod method = meta->method(signalIndex);
    QList<QByteArray> parameterTypes = method.parameterTypes();
    int argc = parameterTypes.count();
    QList<QVariant> vargs;
    for (int i = 0; i < argc; ++i) {
        int type = QMetaType::type(parameterTypes.at(i));
        vargs.append(QVariant(type, argv[i+1]));
    }

#ifdef QSTATEMACHINE_DEBUG
    qDebug() << q_func() << ": sending signal event ( sender =" << sender
             << ", signal =" << sender->metaObject()->method(signalIndex).signature() << ")";
#endif
    internalEventQueue.append(new QtSignalEvent(sender, signalIndex, vargs));
    scheduleProcess();
}

/*!
  Constructs a new state machine with the given \a parent.
*/
QtStateMachine::QtStateMachine(QObject *parent)
    : QObject(
#ifndef QT_STATEMACHINE_SOLUTION
        *new QtStateMachinePrivate,
#endif
        parent)
#ifdef QT_STATEMACHINE_SOLUTION
    , d_ptr(new QtStateMachinePrivate)
#endif
{
#ifdef QT_STATEMACHINE_SOLUTION
    d_ptr->q_ptr = this;
#endif
}

/*!
  Destroys this state machine.
*/
QtStateMachine::~QtStateMachine()
{
#ifdef QT_STATEMACHINE_SOLUTION
    delete d_ptr;
#endif
}

namespace {

class RootErrorState : public QtAbstractState
{
public:
    RootErrorState(QtState *parent)
        : QtAbstractState(parent)
    {
        setObjectName(QString::fromLatin1("DefaultErrorState"));
    }

    void onEntry()
    {
        QtAbstractStatePrivate *d = QtAbstractStatePrivate::get(this);
        QtStateMachine *machine = d->machine();

        qWarning("Unrecoverable error detected in running state machine: %s",
                 qPrintable(machine->errorString()));
    }

    void onExit() {}
};

class RootState : public QtState
{
public:
    RootState(QtState *parent)
        : QtState(parent)
    {
    }

    void onEntry() {}
    void onExit() {}
};

} // namespace

/*!
  Returns this state machine's root state.
*/
QtState *QtStateMachine::rootState() const
{
    Q_D(const QtStateMachine);
    if (!d->rootState) {
        const_cast<QtStateMachinePrivate*>(d)->rootState = new RootState(0);
        const_cast<QtStateMachinePrivate*>(d)->initialErrorStateForRoot = new RootErrorState(d->rootState);
        d->rootState->setParent(const_cast<QtStateMachine*>(this));
        d->rootState->setErrorState(d->initialErrorStateForRoot);
    }
    return d->rootState;
}

/*!
  Returns the error state of the state machine's root state.

  \sa QtState::errorState()
*/
QtAbstractState *QtStateMachine::errorState() const
{
    return rootState()->errorState();
}

/*!
  Sets the error state of this state machine's root state to be \a state. When a running state
  machine encounters an error which puts it in an undefined state, it will enter an error state
  based on the context of the error that occurred. It will enter this state regardless of what
  is currently in the event queue.

  If the erroneous state has an error state set, this will be entered by the machine. If no error
  state has been set, the state machine will search the parent hierarchy recursively for an
  error state. The error state of the root state can thus be seen as a global error state that
  applies for the states for which a more specific error state has not been set.

  Before entering the error state, the state machine will set the error code returned by error() and
  error  message returned by errorString().

  The default error state will print a warning to the console containing the information returned by
  errorString(). By setting a new error state on either the state machine itself, or on specific
  states, you can fine tune error handling in the state machine.

  If the root state's error state is set to 0, or if the error state selected by the machine itself
  contains an error, the default error state will be used.

  \sa QtState::setErrorState(), rootState()
*/
void QtStateMachine::setErrorState(QtAbstractState *state)
{
    rootState()->setErrorState(state);
}

/*! \enum QtStateMachine::Error

    This enum type defines errors that can occur in the state machine at run time. When the state
    machine encounters an unrecoverable error at run time, it will set the error code returned
    by error(), the error message returned by errorString(), and enter an error state based on
    the context of the error.

    \value NoError No error has occurred.
    \value NoInitialStateError The machine has entered a QtState with children which does not have an
           initial state set. The context of this error is the state which is missing an initial
           state.
    \value NoDefaultStateInHistoryState The machine has entered a QtHistoryState which does not have
           a default state set. The context of this error is the QtHistoryState which is missing a
           default state.

    \sa setErrorState()
*/

/*!
  Returns the error code of the last error that occurred in the state machine.
*/
QtStateMachine::Error QtStateMachine::error() const
{
    Q_D(const QtStateMachine);
    return d->error;
}

/*!
  Returns the error string of the last error that occurred in the state machine.
*/
QString QtStateMachine::errorString() const
{
    Q_D(const QtStateMachine);
    return d->errorString;
}

/*!
  Clears the error string and error code of the state machine.
*/
void QtStateMachine::clearError()
{
    Q_D(QtStateMachine);
    d->errorString.clear();
    d->error = NoError;
}

/*!
   Returns the global restore policy of the state machine.

   \sa QtActionState::restorePolicy()
*/
QtActionState::RestorePolicy QtStateMachine::globalRestorePolicy() const
{
    Q_D(const QtStateMachine);
    return d->globalRestorePolicy;
}

/*!
   Sets the global restore policy of the state machine to \a restorePolicy. The default global
   restore policy is QtActionState::DoNotRestoreProperties.

   The global restore policy cannot be set to QtActionState::GlobalRestorePolicy.

   \sa QtActionState::setRestorePolicy()
*/
void QtStateMachine::setGlobalRestorePolicy(QtActionState::RestorePolicy restorePolicy)
{
    Q_D(QtStateMachine);
    if (restorePolicy == QtState::GlobalRestorePolicy) {
        qWarning("QtStateMachine::setGlobalRestorePolicy: Cannot set global restore policy to "
                 "GlobalRestorePolicy");
        return;
    }

    d->globalRestorePolicy = restorePolicy;
}

/*!
  Returns this state machine's initial state, or 0 if no initial state has
  been set.
*/
QtAbstractState *QtStateMachine::initialState() const
{
    Q_D(const QtStateMachine);
    if (!d->rootState)
        return 0;
    return d->rootState->initialState();
}

/*!
  Sets this state machine's initial \a state.
*/
void QtStateMachine::setInitialState(QtAbstractState *state)
{
    Q_D(QtStateMachine);
    if (!d->rootState) {
        if (!state)
            return;
        rootState()->setInitialState(state);
    }
    d->rootState->setInitialState(state);
}

/*!
  Adds the given \a state to this state machine. The state becomes a top-level
  state (i.e. a child of the rootState()).

  If the state is already in a different machine, it will first be removed
  from its old machine, and then added to this machine.

  \sa removeState(), rootState()
*/
void QtStateMachine::addState(QtAbstractState *state)
{
    if (!state) {
        qWarning("QtStateMachine::addState: cannot add null state");
        return;
    }
    if (QtAbstractStatePrivate::get(state)->machine() == this) {
        qWarning("QtStateMachine::addState: state has already been added to this machine");
        return;
    }
    state->setParent(rootState());
}

/*!
  Removes the given \a state from this state machine.  The state machine
  releases ownership of the state.

  \sa addState()
*/
void QtStateMachine::removeState(QtAbstractState *state)
{
    if (!state) {
        qWarning("QtStateMachine::removeState: cannot remove null state");
        return;
    }
    if (QtAbstractStatePrivate::get(state)->machine() != this) {
        qWarning("QtStateMachine::removeState: state %p's machine (%p)"
                 " is different from this machine (%p)",
                 state, QtAbstractStatePrivate::get(state)->machine(), this);
        return;
    }
    state->setParent(0);
}

/*!
  Starts this state machine.
  The machine will reset its configuration and transition to the initial
  state.  When a final top-level state is entered, the machine will emit the
  finished() signal.

  \sa started(), finished(), stop(), initialState()
*/
void QtStateMachine::start()
{
    Q_D(QtStateMachine);

    if (rootState()->initialState() == 0) {
        qWarning("QtStateMachine::start: No initial state set for machine. Refusing to start.");
        return;
    }

    switch (d->state) {
    case QtStateMachinePrivate::NotRunning:
        d->state = QtStateMachinePrivate::Starting;
        QMetaObject::invokeMethod(this, "_q_start", Qt::QueuedConnection);
        break;
    case QtStateMachinePrivate::Starting:
        break;
    case QtStateMachinePrivate::Running:
        qWarning("QtStateMachine::start(): already running");
        break;
    }
}

/*!
  Stops this state machine.

  \sa stopped()
*/
void QtStateMachine::stop()
{
    Q_D(QtStateMachine);
    switch (d->state) {
    case QtStateMachinePrivate::NotRunning:
        break;
    case QtStateMachinePrivate::Starting:
        // the machine will exit as soon as it enters the event processing loop
        d->stop = true;
        break;
    case QtStateMachinePrivate::Running:
        d->stop = true;
        d->scheduleProcess();
        break;
    }
}

/*!
  Posts the given \a event for processing by this state machine, with a delay
  of \a delay milliseconds.

  This function returns immediately. The event is added to the state machine's
  event queue. Events are processed in the order posted. The state machine
  takes ownership of the event and deletes it once it has been processed.

  You can only post events when the state machine is running.
*/
void QtStateMachine::postEvent(QEvent *event, int delay)
{
    Q_D(QtStateMachine);
    if (d->state != QtStateMachinePrivate::Running) {
        qWarning("QtStateMachine::postEvent: cannot post event when the state machine is not running");
        return;
    }
#ifdef QSTATEMACHINE_DEBUG
    qDebug() << this << ": posting external event" << event << "with delay" << delay;
#endif
    if (delay) {
        int tid = startTimer(delay);
        d->delayedEvents[tid] = event;
    } else {
        d->externalEventQueue.append(event);
        d->scheduleProcess();
    }
}

/*!
  \internal

  Posts the given internal \a event for processing by this state machine.
*/
void QtStateMachine::postInternalEvent(QEvent *event)
{
    Q_D(QtStateMachine);
#ifdef QSTATEMACHINE_DEBUG
    qDebug() << this << ": posting internal event" << event;
#endif
    d->internalEventQueue.append(event);
    d->scheduleProcess();
}

/*!
  Returns the list of this state machine's states, or an empty list if the
  state machine has no states.
*/
QList<QtAbstractState*> QtStateMachine::states() const
{
    return QtStatePrivate::get(rootState())->childStates();
}

/*!
  \internal

   Returns the maximal consistent set of states (including parallel and final
   states) that this state machine is currently in. If a state \c s is in the
   configuration, it is always the case that the parent of \c s is also in
   c. Note, however, that the rootState() is not an explicit member of the
   configuration.
*/
QSet<QtAbstractState*> QtStateMachine::configuration() const
{
    Q_D(const QtStateMachine);
    return d->configuration;
}

/*!
  \fn QtStateMachine::started()

  This signal is emitted when the state machine has entered its initial state.

  \sa QtStateMachine::finished(), QtStateMachine::start()
*/

/*!
  \fn QtStateMachine::finished()

  This signal is emitted when the state machine has reached a top-level final
  state.

  \sa QtStateMachine::started()
*/

/*!
  \fn QtStateMachine::stopped()

  This signal is emitted when the state machine has stopped.

  \sa QtStateMachine::stop()
*/

/*!
  \reimp
*/
bool QtStateMachine::event(QEvent *e)
{
    Q_D(QtStateMachine);
    if (e->type() == QEvent::Timer) {
        QTimerEvent *te = static_cast<QTimerEvent*>(e);
        int tid = te->timerId();
        if (d->delayedEvents.contains(tid)) {
            killTimer(tid);
            QEvent *ee = d->delayedEvents.take(tid);
            d->externalEventQueue.append(ee);
            d->scheduleProcess();
            return true;
        }
    } else if (e->type() == QEvent::ChildAdded) {
        QChildEvent *ce = static_cast<QChildEvent*>(e);
        if (QtAbstractState *state = qobject_cast<QtAbstractState*>(ce->child())) {
            if (state != rootState()) {
                state->setParent(rootState());
                return true;
            }
        }
    }
    return QObject::event(e);
}

#ifndef QT_NO_STATEMACHINE_EVENTFILTER
/*!
  \reimp
*/
bool QtStateMachine::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(QtStateMachine);
    Q_ASSERT(d->qobjectEvents.contains(watched));
    if (d->qobjectEvents[watched].contains(event->type()))
        postEvent(new QtBoundEvent(watched, d->handler->cloneEvent(event)));
    return false;
}
#endif

/*!
  \internal

  This function is called when the state machine is about to select
  transitions based on the given \a event.

  The default implementation does nothing.
*/
void QtStateMachine::beginSelectTransitions(QEvent *event)
{
    Q_UNUSED(event);
}

/*!
  \internal

  This function is called when the state machine has finished selecting
  transitions based on the given \a event.

  The default implementation does nothing.
*/
void QtStateMachine::endSelectTransitions(QEvent *event)
{
    Q_UNUSED(event);
}

/*!
  \internal

  This function is called when the state machine is about to do a microstep.

  The default implementation does nothing.
*/
void QtStateMachine::beginMicrostep(QEvent *event)
{
    Q_UNUSED(event);
}

/*!
  \internal

  This function is called when the state machine has finished doing a
  microstep.

  The default implementation does nothing.
*/
void QtStateMachine::endMicrostep(QEvent *event)
{
    Q_UNUSED(event);
}

static const uint qt_meta_data_QtSignalEventGenerator[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QtSignalEventGenerator[] = {
    "QtSignalEventGenerator\0\0execute()\0"
};

const QMetaObject QtSignalEventGenerator::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QtSignalEventGenerator,
      qt_meta_data_QtSignalEventGenerator, 0 }
};

const QMetaObject *QtSignalEventGenerator::metaObject() const
{
    return &staticMetaObject;
}

void *QtSignalEventGenerator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtSignalEventGenerator))
        return static_cast<void*>(const_cast< QtSignalEventGenerator*>(this));
    return QObject::qt_metacast(_clname);
}

int QtSignalEventGenerator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: {
#ifndef QT_STATEMACHINE_SOLUTION
// ### in Qt 4.6 we can use QObject::senderSignalIndex()
            int signalIndex = senderSignalIndex(this);
            Q_ASSERT(signalIndex != -1);
#endif
            QtStateMachine *machine = qobject_cast<QtStateMachine*>(parent());
            QtStateMachinePrivate::get(machine)->handleTransitionSignal(sender(), signalIndex, _a);
            break;
        }
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

QtSignalEventGenerator::QtSignalEventGenerator(
#ifdef QT_STATEMACHINE_SOLUTION
    int sigIdx,
#endif
    QtStateMachine *parent)
    : QObject(parent)
#ifdef QT_STATEMACHINE_SOLUTION
    , signalIndex(sigIdx)
#endif
{
}

/*!
  \class QtSignalEvent

  \brief The QtSignalEvent class represents a Qt signal event.

  

  A signal event is generated by a QtStateMachine in response to a Qt
  signal. The QtSignalTransition class provides a transition associated with a
  signal event. QtSignalEvent is part of \l{The State Machine Framework}.

  The sender() function returns the object that generated the signal. The
  signalIndex() function returns the index of the signal. The arguments()
  function returns the arguments of the signal.

  \sa QtSignalTransition
*/

/*!
  Constructs a new QtSignalEvent object with the given \a sender, \a
  signalIndex and \a arguments.
*/
QtSignalEvent::QtSignalEvent(const QObject *sender, int signalIndex,
                           const QList<QVariant> &arguments)
    :
#ifndef QT_STATEMACHINE_SOLUTION
    QEvent(QEvent::Signal)
#else
    QEvent(QEvent::Type(QEvent::User-1))
#endif
    , m_sender(sender), m_signalIndex(signalIndex), m_arguments(arguments)
{
}

/*!
  Destroys this QtSignalEvent.
*/
QtSignalEvent::~QtSignalEvent()
{
}

/*!
  \fn QtSignalEvent::sender() const

  Returns the object that emitted the signal.

  \sa QObject::sender()
*/

/*!
  \fn QtSignalEvent::signalIndex() const

  Returns the index of the signal.

  \sa QMetaObject::indexOfSignal()
*/

/*!
  \fn QtSignalEvent::arguments() const

  Returns the arguments of the signal.
*/

/*!
  \class QtStateFinishedEvent

  \brief The QtStateFinishedEvent class contains parameters that describe a state that has finished.

  

  A state is finished when one of its final child states (a QtFinalState) is
  entered; this will cause a QtStateFinishedEvent to be generated by the state
  machine. QtStateFinishedEvent is part of \l{The State Machine Framework}.

  Typically you do not create QtStateFinishedEvent objects yourself, but rather
  use QtStateFinishedTransition to create a transition that's triggered by a
  state's finished event.

  \sa QtStateFinishedTransition
*/

/*!
  Constructs a new QtStateFinishedEvent object associated with the given \a state.
*/
QtStateFinishedEvent::QtStateFinishedEvent(QtState *state)
    :
#ifndef QT_STATEMACHINE_SOLUTION
    QEvent(StateFinished)
#else
    QEvent(QEvent::Type(QEvent::User-2))
#endif
    , m_state(state)
{
}

/*!
  Destroys this QtStateFinishedEvent.
*/
QtStateFinishedEvent::~QtStateFinishedEvent()
{
}

/*!
  Returns the state associated with this QtStateFinishedEvent.
*/
QtState *QtStateFinishedEvent::state() const
{
    return m_state;
}

QT_END_NAMESPACE

#include "moc_qtstatemachine.cpp"
