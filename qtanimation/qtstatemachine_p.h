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



#ifndef QSTATEMACHINE_P_H
#define QSTATEMACHINE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QT_STATEMACHINE_SOLUTION
#include <private/qobject_p.h>
#endif
#include <QtCore/qcoreevent.h>
#include <QtCore/qhash.h>
#include <QtCore/qlist.h>
#include <QtCore/qset.h>

#include "qtstate.h"

QT_BEGIN_NAMESPACE

class QEvent;
#ifndef QT_NO_STATEMACHINE_EVENTFILTER
class QtEventTransition;
#endif
class QtSignalEventGenerator;
class QtSignalTransition;
class QtAbstractState;
class QtAbstractTransition;
class QtState;

#ifndef QT_NO_ANIMATION
class QtPropertyAnimation;
#endif

class QtStateMachine;
class Q_ANIMATION_EXPORT QtStateMachinePrivate
#ifndef QT_STATEMACHINE_SOLUTION
    : public QObjectPrivate
#endif
{
    Q_DECLARE_PUBLIC(QtStateMachine)
public:
    enum State {
        NotRunning,
        Starting,
        Running
    };
    enum StopProcessingReason {
        EventQueueEmpty,
        Finished,
        Stopped
    };

    QtStateMachinePrivate();
    ~QtStateMachinePrivate();

    static QtStateMachinePrivate *get(QtStateMachine *q);

    static QtState *findLCA(const QList<QtAbstractState*> &states);

    static bool stateEntryLessThan(QtAbstractState *s1, QtAbstractState *s2);
    static bool stateExitLessThan(QtAbstractState *s1, QtAbstractState *s2);

    QtAbstractState *findErrorState(QtAbstractState *context);
    void setError(QtStateMachine::Error error, QtAbstractState *currentContext);

    // private slots
    void _q_start();
    void _q_process();

    void microstep(const QList<QtAbstractTransition*> &transitionList);
    bool isPreempted(const QtAbstractState *s, const QSet<QtAbstractTransition*> &transitions) const;
    QSet<QtAbstractTransition*> selectTransitions(QEvent *event) const;
    void exitStates(const QList<QtAbstractTransition*> &transitionList);
    void executeTransitionContent(const QList<QtAbstractTransition*> &transitionList);
    void enterStates(const QList<QtAbstractTransition*> &enabledTransitions);
    void addStatesToEnter(QtAbstractState *s, QtState *root,
                          QSet<QtAbstractState*> &statesToEnter,
                          QSet<QtAbstractState*> &statesForDefaultEntry);

    bool isInFinalState(QtAbstractState *s) const;
    static bool isFinal(const QtAbstractState *s);
    static bool isParallel(const QtAbstractState *s);
    static bool isCompound(const QtAbstractState *s);
    static bool isAtomic(const QtAbstractState *s);
    static bool isDescendantOf(const QtAbstractState *s, const QtAbstractState *other);
    static QList<QtState*> properAncestors(const QtAbstractState *s, const QtState *upperBound);

    void registerTransitions(QtAbstractState *state);
    void registerSignalTransition(QtSignalTransition *transition);
#ifndef QT_NO_STATEMACHINE_EVENTFILTER
    void registerEventTransition(QtEventTransition *transition);
#endif
    void handleTransitionSignal(const QObject *sender, int signalIndex,
                                void **args);    
    void scheduleProcess();
    
    typedef QPair<QObject *, QByteArray> RestorableId;
    QHash<RestorableId, QVariant> registeredRestorables;
    void registerRestorable(QObject *object, const QByteArray &propertyName);
    void unregisterRestorable(QObject *object, const QByteArray &propertyName);
    bool hasRestorable(QObject *object, const QByteArray &propertyName) const;
    QVariant restorableValue(QObject *object, const QByteArray &propertyName) const;

#ifndef QT_NO_ANIMATION
    void registerRestorable(QtPropertyAnimation *animation);
        
    QtPropertyAnimation *restorableAnimation(QObject *object, const QByteArray &propertyName);
    QList<QtPropertyAnimation *> restorableAnimations() const;
    
    QHash<RestorableId, QtPropertyAnimation*> registeredRestorableAnimations;    
#endif // QT_NO_ANIMATION

    State state;
    bool processing;
    bool processingScheduled;
    bool stop;
    StopProcessingReason stopProcessingReason;
    QtState *rootState;
    QSet<QtAbstractState*> configuration;
    QList<QEvent*> internalEventQueue;
    QList<QEvent*> externalEventQueue;
    
    QtStateMachine::Error error;
    QtActionState::RestorePolicy globalRestorePolicy;

    QString errorString;
    QSet<QtAbstractState *> pendingErrorStates;
    QSet<QtAbstractState *> pendingErrorStatesForDefaultEntry;
    QtAbstractState *initialErrorStateForRoot;

#ifndef QT_STATEMACHINE_SOLUTION
    QtSignalEventGenerator *signalEventGenerator;
#endif
    QHash<const QObject*, QList<int> > connections;
#ifndef QT_NO_STATEMACHINE_EVENTFILTER
    QHash<QObject*, QSet<QEvent::Type> > qobjectEvents;
#endif
    QHash<int, QEvent*> delayedEvents;
  
    typedef QEvent* (*f_cloneEvent)(QEvent*);
    struct Handler {
        f_cloneEvent cloneEvent;
    };

    static const Handler *handler;

#ifdef QT_STATEMACHINE_SOLUTION
    QtStateMachine *q_ptr;
#endif
};

QT_END_NAMESPACE

#endif
