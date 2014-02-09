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



#include "qtabstractstate.h"
#include "qtabstractstate_p.h"
#include "qtstatemachine.h"
#include "qtstatemachine_p.h"

QT_BEGIN_NAMESPACE

/*!
  \class QtAbstractState

  \brief The QtAbstractState class is the base class of states of a QtStateMachine.

  

  The QtAbstractState class is the abstract base class of states that are part
  of a QtStateMachine. It defines the interface that all state objects have in
  common. QtAbstractState is part of \l{The State Machine Framework}.

  The parentState() function returns the state's parent state.

  \section1 Subclassing

  The onEntry() function is called when the state is entered; reimplement this
  function to perform custom processing when the state is entered.

  The onExit() function is called when the state is exited; reimplement this
  function to perform custom processing when the state is exited.
*/

QtAbstractStatePrivate::QtAbstractStatePrivate()
{
}

QtAbstractStatePrivate *QtAbstractStatePrivate::get(QtAbstractState *q)
{
    return q->d_func();
}

const QtAbstractStatePrivate *QtAbstractStatePrivate::get(const QtAbstractState *q)
{
    return q->d_func();
}

QtStateMachine *QtAbstractStatePrivate::machine() const
{
    Q_Q(const QtAbstractState);
    QObject *par = q->parent();
    while (par != 0) {
        if (QtStateMachine *mach = qobject_cast<QtStateMachine*>(par))
            return mach;
        par = par->parent();
    }
    return 0;
}

void QtAbstractStatePrivate::callOnEntry()
{
    Q_Q(QtAbstractState);
    q->onEntry();
}

void QtAbstractStatePrivate::callOnExit()
{
    Q_Q(QtAbstractState);
    q->onExit();
}

/*!
  Constructs a new state with the given \a parent state.
*/
QtAbstractState::QtAbstractState(QtState *parent)
    : QObject(
#ifndef QT_STATEMACHINE_SOLUTION
        *new QtAbstractStatePrivate,
#endif
        parent)
#ifdef QT_STATEMACHINE_SOLUTION
    , d_ptr(new QtAbstractStatePrivate)
#endif
{
#ifdef QT_STATEMACHINE_SOLUTION
    d_ptr->q_ptr = this;
#endif
}

/*!
  \internal
*/
QtAbstractState::QtAbstractState(QtAbstractStatePrivate &dd, QtState *parent)
    : QObject(
#ifndef QT_STATEMACHINE_SOLUTION
        dd,
#endif
        parent)
#ifdef QT_STATEMACHINE_SOLUTION
    , d_ptr(&dd)
#endif
{
#ifdef QT_STATEMACHINE_SOLUTION
    d_ptr->q_ptr = this;
#endif
}

/*!
  Destroys this state.
*/
QtAbstractState::~QtAbstractState()
{
#ifdef QT_STATEMACHINE_SOLUTION
    delete d_ptr;
#endif
}

/*!
  Returns this state's parent state, or 0 if the state has no parent state.
*/
QtState *QtAbstractState::parentState() const
{
    return qobject_cast<QtState*>(parent());
}

/*!
  \fn QtAbstractState::onExit()

  This function is called when the state is exited.  Reimplement this function
  to perform custom processing when the state is exited.
*/

/*!
  \fn QtAbstractState::onEntry()

  This function is called when the state is entered. Reimplement this function
  to perform custom processing when the state is entered.
*/

/*!
  \reimp
*/
bool QtAbstractState::event(QEvent *e)
{
    return QObject::event(e);
}

QT_END_NAMESPACE
