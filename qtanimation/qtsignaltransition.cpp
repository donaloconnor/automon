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



#include "qtsignaltransition.h"
#include "qtsignaltransition_p.h"
#include "qtsignalevent.h"
#include <qdebug.h>

QT_BEGIN_NAMESPACE

/*!
  \class QtSignalTransition

  \brief The QtSignalTransition class provides a transition based on a Qt signal.

  

  Typically you would use the overload of QtState::addTransition() that takes a
  sender and signal as arguments, rather than creating QtSignalTransition
  objects directly. QtSignalTransition is part of \l{The State Machine
  Framework}.

  You can subclass QtSignalTransition and reimplement testCondition() to make a
  signal transition conditional; the event object passed to testCondition()
  will be a QtSignalEvent object. Example:

  \code
  class CheckedTransition : public QtSignalTransition
  {
  public:
      CheckedTransition(QCheckBox *check)
          : QtSignalTransition(check, SIGNAL(stateChanged(int))) {}
  protected:
      bool testCondition(QEvent *e) const {
          QtSignalEvent *se = static_cast<QtSignalEvent*>(e);
          return (se->arguments().at(0).toInt() == Qt::Checked)
              && QtSignalTransition::testCondition(e);
      }
  };

  ...

  QCheckBox *check = new QCheckBox();
  check->setTristate(true);

  QtState *s1 = new QtState();
  QtState *s2 = new QtState();
  s1->addTransition(new CheckedTransition(check), s2);
  \endcode
*/

QtSignalTransitionPrivate::QtSignalTransitionPrivate()
{
}

QtSignalTransitionPrivate *QtSignalTransitionPrivate::get(QtSignalTransition *q)
{
    return q->d_func();
}

/*!
  Constructs a new signal transition associated with the given \a signal of
  the given \a sender.

  \sa QtState::addTransition()
*/
QtSignalTransition::QtSignalTransition(const QObject *sender, const char *signal)
    : QtTransition(*new QtSignalTransitionPrivate,
#ifndef QT_STATEMACHINE_SOLUTION
                  QEvent::Signal
#else
                  QEvent::Type(QEvent::User-1)
#endif
        )
{
    Q_D(QtSignalTransition);
    d->sender = sender;
    d->signal = signal;
    d->signalIndex = -1;
}

/*!
  Destroys this signal transition.
*/
QtSignalTransition::~QtSignalTransition()
{
}

/*!
  \reimp

  The \a event is a QtSignalEvent object.  The default implementation returns
  true if the event's sender and signal index match this transition, and
  returns false otherwise.
*/
bool QtSignalTransition::testCondition(QEvent *event) const
{
    Q_D(const QtSignalTransition);
    QtSignalEvent *se = static_cast<QtSignalEvent*>(event);
    Q_ASSERT(d->signalIndex != -1);
    return (se->sender() == d->sender)
        && (se->signalIndex() == d->signalIndex);
}

/*!
  \reimp
*/
void QtSignalTransition::onTransition()
{
}

/*!
  \reimp
*/
bool QtSignalTransition::event(QEvent *e)
{
    return QtTransition::event(e);
}

QT_END_NAMESPACE
