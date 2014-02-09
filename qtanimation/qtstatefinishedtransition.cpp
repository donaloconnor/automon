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



#include "qtstatefinishedtransition.h"
#include "qtstatefinishedevent.h"
#include "qttransition_p.h"

QT_BEGIN_NAMESPACE

/*!
  \class QtStateFinishedTransition

  \brief The QtStateFinishedTransition class provides a transition that triggers when a state is finished.

  

  A state is finished when one of its final child states (a QtFinalState) is
  entered; this will cause a QtStateFinishedEvent to be generated. The
  QtStateFinishedTransition class provides a way of associating a transition
  with such an event. QtStateFinishedTransition is part of \l{The State Machine
  Framework}.

  \code
    QtStateMachine machine;
    QtState *s1 = new QtState(machine.rootState());
    QtState *s11 = new QtState(s1);
    QtFinalState *s12 = new QtFinalState(s1);
    s11->addTransition(s12);

    QtState *s2 = new QtState(machine.rootState());
    s1->addTransition(new QtStateFinishedTransition(s1), s2);
  \endcode

  \sa QtState::addFinishedTransition(), QtStateFinishedEvent
*/

class QtStateFinishedTransitionPrivate : public QtTransitionPrivate
{
    Q_DECLARE_PUBLIC(QtStateFinishedTransition)
public:
    QtStateFinishedTransitionPrivate();

    static QtStateFinishedTransitionPrivate *get(QtStateFinishedTransition *q);

    QtState *state;
};

QtStateFinishedTransitionPrivate::QtStateFinishedTransitionPrivate()
{
}

QtStateFinishedTransitionPrivate *QtStateFinishedTransitionPrivate::get(QtStateFinishedTransition *q)
{
    return q->d_func();
}

/*!
  Constructs a new QtStateFinishedTransition object associated with the given
  \a state.

  \sa QtState::addTransition(), QtState::addFinishedTransition()
*/
QtStateFinishedTransition::QtStateFinishedTransition(QtState *state)
    : QtTransition(*new QtStateFinishedTransitionPrivate,
#ifndef QT_STATEMACHINE_SOLUTION
                          QEvent::StateFinished
#else
                          QEvent::Type(QEvent::User-2)
#endif
        )
{
    Q_D(QtStateFinishedTransition);
    d->state = state;
    if (!state)
        qWarning("QtStateFinishedTransition(): null state will never finish");
}

/*!
  Destroys this QtStateFinishedTransition.
*/
QtStateFinishedTransition::~QtStateFinishedTransition()
{
}

/*!
  \reimp
*/
bool QtStateFinishedTransition::testCondition(QEvent *event) const
{
    Q_D(const QtStateFinishedTransition);
    QtStateFinishedEvent *sfe = static_cast<QtStateFinishedEvent*>(event);
    return (sfe->state() == d->state);
}

/*!
  \reimp
*/
void QtStateFinishedTransition::onTransition()
{
}

/*!
  \reimp
*/
bool QtStateFinishedTransition::event(QEvent *e)
{
    return QtTransition::event(e);
}

QT_END_NAMESPACE
