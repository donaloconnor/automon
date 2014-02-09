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



#include "qtactionstate.h"
#include "qtactionstate_p.h"
#include "qtstateaction.h"
#include "qtstateaction_p.h"

QT_BEGIN_NAMESPACE

/*!
  \class QtActionState

  \brief The QtActionState class provides an action-based state.

  

  QtActionState executes \l{QtStateAction}{state actions} when the state is
  entered and exited. QtActionState is part of \l{The State Machine Framework}.

  You can add actions to a state with the addEntryAction() and addExitAction()
  functions. The state executes the actions when the state is entered and
  exited, respectively.

  Built-in actions are provided for setting properties and invoking methods of
  QObjects. The setPropertyOnEntry() and setPropertyOnExit() functions are
  used for defining property assignments that should be performed when a state
  is entered and exited, respectively.

  \code
  QLabel label;
  QtStateMachine machine;
  QtState *s1 = new QtState();
  s1->setPropertyOnEntry(&label, "text", "Entered state s1");
  machine.addState(s1);
  \endcode

  The invokeMethodOnEntry() and invokeMethodOnExit() functions are used for
  defining method invocations that should be performed when a state is entered
  and exited, respectively.

  \code
  QtState *s2 = new QtState();
  s2->invokeMethodOnEntry(&label, "showMaximized");
  machine.addState(s2);
  \endcode

  \sa QtStateAction
*/

/*!
   \enum QtActionState::RestorePolicy

   This enum specifies the restore policy type for a state. The restore policy takes effect when
   the machine enters a state which has entry actions of the type QtStateSetPropertyAction. If the 
   restore policy of the state is set to RestoreProperties, the state machine will save the 
   value of the property before the QtStateSetPropertyAction is executed. 

   Later, when the machine either enters a state which has its restore policy set to 
   DoNotRestoreProperties or when it enters a state which does not set a value for the given 
   property, the property will automatically be restored to its initial value. The state machine
   will only detect which properties are being set if they are being set using a
   QtStateSetPropertyAction object set as entry action on a state.

   Special rules apply when using QtAnimationState. If a QtAnimationState registers that a property
   should be restored before entering the target state of its QtStateFinishedTransition, it will
   restore this property using a QtPropertyAnimation. 
   
   Only one initial value will be saved for any given property. If a value for a property has 
   already been saved by the state machine, it will not be overwritten until the property has been
   successfully restored. Once the property has been restored, the state machine will clear the 
   initial value until it enters a new state which sets the property and which has RestoreProperties
   as its restore policy.

   \value GlobalRestorePolicy The restore policy for the state should be retrieved using 
          QtStateMachine::globalRestorePolicy()
   \value DoNotRestoreProperties The state machine should not save the initial values of properties 
          set in the state and restore them later.
   \value RestoreProperties The state machine should save the initial values of properties 
          set in the state and restore them later.


   \sa setRestorePolicy(), restorePolicy(), addEntryAction(), setPropertyOnEntry()
*/

QtActionStatePrivate::QtActionStatePrivate()
    : restorePolicy(QtActionState::GlobalRestorePolicy)
{
}

QtActionStatePrivate::~QtActionStatePrivate()
{
}

QtActionStatePrivate *QtActionStatePrivate::get(QtActionState *q)
{
    return q->d_func();
}

const QtActionStatePrivate *QtActionStatePrivate::get(const QtActionState *q)
{
    return q->d_func();
}

QList<QtStateAction*> QtActionStatePrivate::entryActions() const
{
    QList<QtStateAction*> result;
    QList<QObject*>::const_iterator it;
#ifdef QT_STATEMACHINE_SOLUTION
    const QObjectList &children = q_func()->children();
#endif
    for (it = children.constBegin(); it != children.constEnd(); ++it) {
        QtStateAction *act = qobject_cast<QtStateAction*>(*it);
        if (act && (QtStateActionPrivate::get(act)->when == QtStateActionPrivate::ExecuteOnEntry))
            result.append(act);
    }
    return result;
}

QList<QtStateAction*> QtActionStatePrivate::exitActions() const
{
    QList<QtStateAction*> result;
    QList<QObject*>::const_iterator it;
#ifdef QT_STATEMACHINE_SOLUTION
    const QObjectList &children = q_func()->children();
#endif
    for (it = children.constBegin(); it != children.constEnd(); ++it) {
        QtStateAction *act = qobject_cast<QtStateAction*>(*it);
        if (act && (QtStateActionPrivate::get(act)->when == QtStateActionPrivate::ExecuteOnExit))
            result.append(act);
    }
    return result;
}

/*!
  Constructs a new action state with the given \a parent state.
*/
QtActionState::QtActionState(QtState *parent)
    : QtAbstractState(*new QtActionStatePrivate, parent)
{
}

/*!
  \internal
*/
QtActionState::QtActionState(QtActionStatePrivate &dd,
                           QtState *parent)
    : QtAbstractState(dd, parent)
{
}

/*!
  Destroys this action state.
*/
QtActionState::~QtActionState()
{
}

/*!
  Instructs this state to set the property with the given \a name of the given
  \a object to the given \a value when the state is entered. This function
  will create a QtStateSetPropertyAction object and add it to the entry actions
  of the state. If there is already an existing action associated with the
  property, the value of that action is updated.

  \sa setPropertyOnExit(), invokeMethodOnEntry(), addEntryAction()
*/
void QtActionState::setPropertyOnEntry(QObject *object, const char *name,
                                      const QVariant &value)
{
    Q_D(QtActionState);
    QList<QtStateAction*> actions = d->entryActions();
    for (int i=0; i<actions.size(); ++i) {
        QtStateAction *action = actions.at(i);           
        if (QtStateSetPropertyAction *spa = qobject_cast<QtStateSetPropertyAction*>(action)) {
            if (spa->targetObject() == object && spa->propertyName() == name) {
                QtStateSetPropertyActionPrivate::get(spa)->value = value;
                return;
            }
        }
    }

    addEntryAction(new QtStateSetPropertyAction(object, name, value));
}

/*!
  Instructs this state to set the property with the given \a name of the given
  \a object to the given \a value when the state is exited. This function will
  create a QtStateSetPropertyAction object and add it to the exit actions of
  the state. If there is already an existing action associated with the
  property, the value of that action is updated.

  \sa setPropertyOnEntry(), invokeMethodOnExit(), addExitAction()
*/
void QtActionState::setPropertyOnExit(QObject *object, const char *name,
                                     const QVariant &value)
{
    Q_D(QtActionState);
    QList<QtStateAction*> actions = d->exitActions();
    for (int i=0; i<actions.size(); ++i) {
        QtStateAction *action = actions.at(i);
        if (QtStateSetPropertyAction *spa = qobject_cast<QtStateSetPropertyAction*>(action)) {
            if (spa->targetObject() == object && spa->propertyName() == name) {
                QtStateSetPropertyActionPrivate::get(spa)->value = value;
                return;
            }
        }
    }

    addExitAction(new QtStateSetPropertyAction(object, name, value));
}

/*!
  Instructs this state to invoke the given \a method of the given \a object
  with the given \a arguments when the state is entered. This function will
  create a QtStateInvokeMethodAction object and add it to the entry actions of
  the state.

  \sa invokeMethodOnExit(), setPropertyOnEntry(), addEntryAction()
*/
void QtActionState::invokeMethodOnEntry(QObject *object, const char *method,
                                       const QList<QVariant> &arguments)
{
    addEntryAction(new QtStateInvokeMethodAction(object, method, arguments));
}

/*!
  Instructs this state to invoke the given \a method of the given \a object
  with the given \a arguments when the state is exited. This function will
  create a QtStateInvokeMethodAction object and add it to the exit actions of
  the state.

  \sa invokeMethodOnEntry(), setPropertyOnExit(), addExitAction()
*/
void QtActionState::invokeMethodOnExit(QObject *object, const char *method,
                                      const QList<QVariant> &arguments)
{
    addExitAction(new QtStateInvokeMethodAction(object, method, arguments));
}

/*!
  Adds the given \a action to this state. The action will be executed when
  this state is entered. The state takes ownership of the action.

  \sa addExitAction(), removeEntryAction()
*/
void QtActionState::addEntryAction(QtStateAction *action)
{
    if (!action) {
        qWarning("QtActionState::addEntryAction: cannot add null action");
        return;
    }
    action->setParent(this);
    QtStateActionPrivate::get(action)->when = QtStateActionPrivate::ExecuteOnEntry;
}

/*!
  Adds the given \a action to this state. The action will be executed when
  this state is exited. The state takes ownership of the action.

  \sa addEntryAction(), removeExitAction()
*/
void QtActionState::addExitAction(QtStateAction *action)
{
    if (!action) {
        qWarning("QtActionState::addExitAction: cannot add null action");
        return;
    }
    action->setParent(this);
    QtStateActionPrivate::get(action)->when = QtStateActionPrivate::ExecuteOnExit;
}

/*!
  Removes the given entry \a action from this state. The state releases
  ownership of the action.

  \sa addEntryAction()
*/
void QtActionState::removeEntryAction(QtStateAction *action)
{
    if (!action) {
        qWarning("QtActionState::removeEntryAction: cannot remove null action");
        return;
    }
    if (action->parent() == this)
        action->setParent(0);
}

/*!
  Removes the given exit \a action from this state. The state releases
  ownership of the action.

  \sa addExitAction()
*/
void QtActionState::removeExitAction(QtStateAction *action)
{
    if (!action) {
        qWarning("QtActionState::removeExitAction: cannot remove null action");
        return;
    }
    if (action->parent() == this)
        action->setParent(0);
}

/*!
  Returns this state's entry actions.

  \sa addEntryAction(), exitActions()
*/
QList<QtStateAction*> QtActionState::entryActions() const
{
    Q_D(const QtActionState);
    return d->entryActions();
}

/*!
  Returns this state's exit actions.

  \sa addExitAction(), entryActions()
*/
QList<QtStateAction*> QtActionState::exitActions() const
{
    Q_D(const QtActionState);
    return d->exitActions();
}

/*!
  Sets the restore policy of this state to \a restorePolicy. 
  
  The default restore policy is QtActionState::GlobalRestorePolicy.
*/
void QtActionState::setRestorePolicy(RestorePolicy restorePolicy)
{
    Q_D(QtActionState);
    d->restorePolicy = restorePolicy;
}

/*!
  Returns the restore policy for this state.
*/
QtActionState::RestorePolicy QtActionState::restorePolicy() const
{
    Q_D(const QtActionState);
    return d->restorePolicy;
}

/*!
  \reimp
*/
void QtActionState::onEntry()
{
    Q_D(QtActionState);
    QList<QtStateAction*> actions = d->entryActions();
    for (int i = 0; i < actions.size(); ++i)
        QtStateActionPrivate::get(actions.at(i))->callExecute();
}

/*!
  \reimp
*/
void QtActionState::onExit()
{
    Q_D(QtActionState);
    QList<QtStateAction*> actions = d->exitActions();
    for (int i = 0; i < actions.size(); ++i)
        QtStateActionPrivate::get(actions.at(i))->callExecute();
}

/*!
  \reimp
*/
bool QtActionState::event(QEvent *e)
{
    return QtAbstractState::event(e);
}

QT_END_NAMESPACE
