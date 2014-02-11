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



#ifndef QSTATEMACHINE_H
#define QSTATEMACHINE_H

#ifndef QT_STATEMACHINE_SOLUTION
#  include <QtCore/qactionstate.h>
#else
#  include "qtactionstate.h"
#endif

#include <QtCore/qlist.h>
#include <QtCore/qobject.h>
#include <QtCore/qset.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QEvent;
class QtAbstractState;
class QtState;

class QtStateMachinePrivate;
class Q_ANIMATION_EXPORT QtStateMachine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QtState* rootState READ rootState)
    Q_PROPERTY(QtAbstractState* initialState READ initialState WRITE setInitialState)
    Q_PROPERTY(QtAbstractState* errorState READ errorState WRITE setErrorState)
    Q_PROPERTY(QString errorString READ errorString)
public:

    enum Error {
        NoError, 
        NoInitialStateError,
        NoDefaultStateInHistoryState,
    };

    QtStateMachine(QObject *parent = 0);
    ~QtStateMachine();

    void addState(QtAbstractState *state);
    void removeState(QtAbstractState *state);

    QtState *rootState() const;

    QtAbstractState *initialState() const;
    void setInitialState(QtAbstractState *state);

    QtAbstractState *errorState() const;
    void setErrorState(QtAbstractState *state);

    Error error() const;
    QString errorString() const;
    void clearError();

    QtActionState::RestorePolicy globalRestorePolicy() const;
    void setGlobalRestorePolicy(QtActionState::RestorePolicy restorePolicy);

    void postEvent(QEvent *event, int delay = 0);

    QList<QtAbstractState*> states() const;
    QSet<QtAbstractState*> configuration() const;

#ifndef QT_NO_STATEMACHINE_EVENTFILTER
    bool eventFilter(QObject *watched, QEvent *event);
#endif

public Q_SLOTS:
    void start();
    void stop();

Q_SIGNALS:
    void started();
    void stopped();
    void finished();

protected:
    void postInternalEvent(QEvent *event);

    virtual void beginSelectTransitions(QEvent *event);
    virtual void endSelectTransitions(QEvent *event);

    virtual void beginMicrostep(QEvent *event);
    virtual void endMicrostep(QEvent *event);

    bool event(QEvent *e);

#ifdef QT_STATEMACHINE_SOLUTION
     QtStateMachinePrivate *d_ptr;
#endif

private:
    Q_DISABLE_COPY(QtStateMachine)
    Q_DECLARE_PRIVATE(QtStateMachine)
    Q_PRIVATE_SLOT(d_func(), void _q_start())
    Q_PRIVATE_SLOT(d_func(), void _q_process())
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
