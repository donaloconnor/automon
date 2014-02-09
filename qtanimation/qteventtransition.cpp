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



#include "qteventtransition.h"
#include "qteventtransition_p.h"
#include "qtboundevent_p.h"
#include <qdebug.h>

QT_BEGIN_NAMESPACE

/*!
  \class QtEventTransition

  \brief The QtEventTransition class provides a QObject-specific transition for Qt events.

  

  A QtEventTransition object binds an event or transition to a particular
  QObject.  QtEventTransition is part of \l{The State Machine Framework}.

  Example:

  \code
  QPushButton *button = ...;
  QtState *s1 = ...;
  QtState *s2 = ...;
  // If in s1 and the button receives an Enter event, transition to s2
  s1->addTransition(new QtEventTransition(button, QEvent::Enter), s2);
  // If in s2 and the button receives an Exit event, transition back to s1
  s2->addTransition(new QtEventTransition(button, QEvent::Leave), s1);
  \endcode

  \sa QtState::addTransition()
*/

QtEventTransitionPrivate::QtEventTransitionPrivate()
{
    object = 0;
    transition = 0;
    registered = false;
}

QtEventTransitionPrivate *QtEventTransitionPrivate::get(QtEventTransition *q)
{
    return q->d_func();
}

/*!
  Constructs a new QtEventTransition object that binds the given \a transition to
  the given \a object.
  The new QtEventTransition object takes ownership of \a transition.
*/
QtEventTransition::QtEventTransition(QObject *object, QtAbstractTransition *transition)
#ifdef QT_STATEMACHINE_SOLUTION
    : QtTransition(*new QtEventTransitionPrivate, QEvent::Type(QEvent::User-3))
#else
    : QtTransition(*new QtEventTransitionPrivate, QEvent::Bound)
#endif
{
    Q_D(QtEventTransition);
    d->object = object;
    d->transition = transition;
    if (transition)
        transition->setParent(this);
}

namespace {

class EventTransition : public QtAbstractTransition
{
public:
    EventTransition(QEvent::Type type)
        : QtAbstractTransition(type) {}
protected:
    void onTransition() {}
    bool testCondition(QEvent *) const { return true; }
};

} // namespace

/*!
  Constructs a new QtEventTransition object associated with events of the given
  \a type for the given \a object.
*/
QtEventTransition::QtEventTransition(QObject *object, QEvent::Type type)
#ifdef QT_STATEMACHINE_SOLUTION
    : QtTransition(*new QtEventTransitionPrivate, QEvent::Type(QEvent::User-3))
#else
    : QtTransition(*new QtEventTransitionPrivate, QEvent::Bound)
#endif
{
    Q_D(QtEventTransition);
    d->registered = false;
    d->object = object;
    d->transition = new EventTransition(type);
    d->transition->setParent(this);
}

/*!
  \internal
*/
QtEventTransition::QtEventTransition(QtEventTransitionPrivate &dd, QObject *object,
                                   QtAbstractTransition *transition)
#ifdef QT_STATEMACHINE_SOLUTION
    : QtTransition(dd, QEvent::Type(QEvent::User-3))
#else
    : QtTransition(dd, QEvent::Bound)
#endif
{
    Q_D(QtEventTransition);
    d->object = object;
    Q_ASSERT(transition != 0);
    transition->setParent(this);
    d->transition = transition;
}

/*!
  Destroys this QObject event transition.
*/
QtEventTransition::~QtEventTransition()
{
}

/*!
  \reimp
*/
bool QtEventTransition::testCondition(QEvent *event) const
{
    Q_D(const QtEventTransition);
    QtBoundEvent *oe = static_cast<QtBoundEvent*>(event);
    return (oe->object() == d->object)
        && QtAbstractTransitionPrivate::get(d->transition)->eventTypes.contains(oe->event()->type())
        && (QtAbstractTransitionPrivate::get(d->transition)->callTestCondition(oe->event()));
}

/*!
  \reimp
*/
void QtEventTransition::onTransition()
{
}

/*!
  \reimp
*/
bool QtEventTransition::event(QEvent *e)
{
    return QtTransition::event(e);
}

QT_END_NAMESPACE
