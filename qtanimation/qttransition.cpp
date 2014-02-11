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



#include "qttransition.h"
#include "qttransition_p.h"
#include "qtstateaction.h"
#include "qtstateaction_p.h"

QT_BEGIN_NAMESPACE

/*!
  \class QtTransition

  \brief The QtTransition class provides an action-based transition.

  

  QtTransition provides an action-based transition; you add actions with the
  addAction() function. The transition executes the actions when the
  transition is triggered. QtTransition is part of \l{The State Machine
  Framework}.

  Built-in actions are provided for setting properties and invoking methods of
  QObjects. The setPropertyOnTransition() function is used for defining
  property assignments that should be performed when a transition is taken.
  The invokeMethodOnTransition() function is used for defining method
  invocations that should be performed when a transition is taken.

  \code
  QtStateMachine machine;
  QtState *s1 = new QtState();
  machine.addState(s1);
  QtTransition *t1 = new QtTransition();
  QLabel label;
  t1->setPropertyOnTransition(&label, "text", "Transition t1 was triggered");
  QtState *s2 = new QtState();
  machine.addState(s2);
  s1->addTransition(t1, s2);
  \endcode

  Actions are executed in the order in which they were added.

  \sa QtState::addTransition(), QtStateAction
*/

QtTransitionPrivate::QtTransitionPrivate()
{
}

QtTransitionPrivate::~QtTransitionPrivate()
{
}

QtTransitionPrivate *QtTransitionPrivate::get(QtTransition *q)
{
    return q->d_func();
}

const QtTransitionPrivate *QtTransitionPrivate::get(const QtTransition *q)
{
    return q->d_func();
}

QList<QtStateAction*> QtTransitionPrivate::actions() const
{
    QList<QtStateAction*> result;
    QList<QObject*>::const_iterator it;
#ifdef QT_STATEMACHINE_SOLUTION
    const QObjectList &children = q_func()->children();
#endif
    for (it = children.constBegin(); it != children.constEnd(); ++it) {
        QtStateAction *s = qobject_cast<QtStateAction*>(*it);
        if (s)
            result.append(s);
    }
    return result;
}

/*!
  Constructs a new transition associated with the given \a eventTypes.
*/
QtTransition::QtTransition(const QList<QEvent::Type> &eventTypes)
    : QtAbstractTransition(*new QtTransitionPrivate, eventTypes)
{
}

/*!
  Constructs a new transition associated with the given \a eventType.
*/
QtTransition::QtTransition(QEvent::Type eventType)
    : QtAbstractTransition(*new QtTransitionPrivate, eventType)
{
}

/*!
  \internal
*/
QtTransition::QtTransition(QtTransitionPrivate &dd, QEvent::Type eventType)
    : QtAbstractTransition(dd, eventType)
{
}

/*!
  \internal
*/
QtTransition::QtTransition(QtTransitionPrivate &dd, const QList<QEvent::Type> &eventTypes)
    : QtAbstractTransition(dd, eventTypes)
{
}

/*!
  Destroys this transition.
*/
QtTransition::~QtTransition()
{
}

/*!
  Instructs this QtTransition to set the property with the given \a name of the
  given \a object to the given \a value when the transition is taken. This
  function will create a QtStateSetPropertyAction object and add it to the
  actions of the transition. If there is already an existing action associated
  with the property, the value of that action is updated.

  \sa invokeMethodOnTransition()
*/
void QtTransition::setPropertyOnTransition(QObject *object, const char *name,
                                          const QVariant &value)
{
    Q_D(QtTransition);
    QList<QtStateAction*> actions = d->actions();
    for (int i = 0; i < actions.size(); ++i) {
        QtStateAction *action = actions.at(i);           
        if (QtStateSetPropertyAction *spa = qobject_cast<QtStateSetPropertyAction*>(action)) {
            if (spa->targetObject() == object && spa->propertyName() == name) {
                QtStateSetPropertyActionPrivate::get(spa)->value = value;
                return;
            }
        }
    }
    addAction(new QtStateSetPropertyAction(object, name, value));
}

/*!
  Instructs this QtTransition to invoke the given \a method of the given \a
  object with the given \a arguments when the transition is taken. This
  function will create a QtStateInvokeMethodAction object and add it to the
  actions of the transition.

  \sa setPropertyOnTransition()
*/
void QtTransition::invokeMethodOnTransition(QObject *object, const char *method,
                                           const QList<QVariant> &arguments)
{
    addAction(new QtStateInvokeMethodAction(object, method, arguments));
}

/*!
  Adds the given \a action to this transition.
  The action will be executed when the transition is triggered.
  The transition takes ownership of the action.

  \sa removeAction()
*/
void QtTransition::addAction(QtStateAction *action)
{
    if (!action) {
        qWarning("QtTransition::addAction: cannot add null action");
        return;
    }
    action->setParent(this);
}

/*!
  Removes the given \a action from this transition.
  The transition releases ownership of the action.

  \sa addAction()
*/
void QtTransition::removeAction(QtStateAction *action)
{
    if (!action) {
        qWarning("QtTransition::removeAction: cannot remove null action");
        return;
    }
    action->setParent(0);
}

/*!
  Returns this transitions's actions, or an empty list if the transition has
  no actions.

  \sa addAction()
*/
QList<QtStateAction*> QtTransition::actions() const
{
    Q_D(const QtTransition);
    return d->actions();
}

/*!
  \reimp
*/
void QtTransition::onTransition()
{
    Q_D(QtTransition);
    QList<QtStateAction*> actions = d->actions();
    for (int i = 0; i < actions.size(); ++i)
        QtStateActionPrivate::get(actions.at(i))->callExecute();
}

/*!
  \reimp
*/
bool QtTransition::event(QEvent *e)
{
    return QtAbstractTransition::event(e);
}

QT_END_NAMESPACE
