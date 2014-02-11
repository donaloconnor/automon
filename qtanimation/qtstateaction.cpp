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



#include "qtstateaction.h"
#include "qtstateaction_p.h"
#include <QtCore/qvarlengtharray.h>
#include <QtCore/qmetaobject.h>

QT_BEGIN_NAMESPACE

QtStateActionPrivate::QtStateActionPrivate()
{
    when = ExecuteOnEntry;
}

QtStateActionPrivate::~QtStateActionPrivate()
{
}

QtStateActionPrivate *QtStateActionPrivate::get(QtStateAction *q)
{
    return q->d_func();
}

void QtStateActionPrivate::callExecute()
{
    Q_Q(QtStateAction);
    q->execute();
}

/*!
  \class QtStateAction

  \brief The QtStateAction class is the base class of QtState actions.

  

  A state action is added to a state by calling QtState::addEntryAction() or
  QtState::addExitAction(). QtStateAction is part of \l{The State Machine
  Framework}.

  \section1 Subclassing

  Subclasses must implement the execute() function.
*/

/*!
  Constructs a new QtStateAction object with the given \a parent.
*/
QtStateAction::QtStateAction(QObject *parent)
    : QObject(
#ifndef QT_STATEMACHINE_SOLUTION
        *new QtStateActionPrivate,
#endif
        parent)
#ifdef QT_STATEMACHINE_SOLUTION
    , d_ptr(new QtStateActionPrivate)
#endif
{
#ifdef QT_STATEMACHINE_SOLUTION
    d_ptr->q_ptr = this;
#endif
}

/*!
  \internal
*/
QtStateAction::QtStateAction(QtStateActionPrivate &dd, QObject *parent)
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
  Destroys this QtStateAction object.
*/
QtStateAction::~QtStateAction()
{
#ifdef QT_STATEMACHINE_SOLUTION
    delete d_ptr;
#endif
}

/*!
  \fn QtStateAction::execute()

  Executes this action.
*/

/*!
  \reimp
*/
bool QtStateAction::event(QEvent *e)
{
    return QObject::event(e);
}

QtStateSetPropertyActionPrivate *QtStateSetPropertyActionPrivate::get(QtStateSetPropertyAction *q)
{
    return q->d_func();
}

/*!
  \class QtStateSetPropertyAction

  \brief The QtStateSetPropertyAction class provides a set property action for QObjects.

  

  The QtStateSetPropertyAction class provides an action that sets a property of
  a QObject to a pre-defined value when a QtState is entered or exited.
  QtStateSetPropertyAction is part of \l{The State Machine Framework}.

  Typically you don't construct QtStateSetPropertyAction objects directly, but
  rather call the QtState::setPropertyOnEntry() function or the
  QtState::setPropertyOnExit() function.
*/

/*!
    \property QtStateSetPropertyAction::target

    \brief the object for which this action sets a property
*/

/*!
    \property QtStateSetPropertyAction::propertyName

    \brief the name of the property set by this action
*/

/*!
    \property QtStateSetPropertyAction::value

    \brief the value set by this action
*/

/*!
  Constructs a new QtStateSetPropertyAction object for the property named \a
  propertyName of the given \a target object, with the given \a value, and
  with the given \a parent.
*/
QtStateSetPropertyAction::QtStateSetPropertyAction(
    QObject *target, const QByteArray &propertyName,
    const QVariant &value, QObject *parent)
    : QtStateAction(*new QtStateSetPropertyActionPrivate, parent)
{
    Q_D(QtStateSetPropertyAction);
    d->target = target;
    d->propertyName = propertyName;
    d->value = value;
}

/*!
  Constructs a new QtStateSetPropertyAction object with the given \a parent.
*/
QtStateSetPropertyAction::QtStateSetPropertyAction(QObject *parent)
    : QtStateAction(*new QtStateSetPropertyActionPrivate, parent)
{
    Q_D(QtStateSetPropertyAction);
    d->target = 0;
}

/*!
  Destroys this QtStateAbstractSetPropertyAction object.
*/
QtStateSetPropertyAction::~QtStateSetPropertyAction()
{
}

/*!
  Returns the object for which this action sets a property.
*/
QObject *QtStateSetPropertyAction::targetObject() const
{
    Q_D(const QtStateSetPropertyAction);
    return d->target;
}

/*!
  Sets the object for which this action sets a property.
*/
void QtStateSetPropertyAction::setTargetObject(QObject *target)
{
    Q_D(QtStateSetPropertyAction);
    d->target = target;
}

/*!
  Returns the name of the property set by this action.
*/
QByteArray QtStateSetPropertyAction::propertyName() const
{
    Q_D(const QtStateSetPropertyAction);
    return d->propertyName;
}

/*!
  Sets the name of the property set by this action.
*/
void QtStateSetPropertyAction::setPropertyName(const QByteArray &propertyName)
{
    Q_D(QtStateSetPropertyAction);
    d->propertyName = propertyName;
}

/*!
  Returns the value set by this action.
*/
QVariant QtStateSetPropertyAction::value() const
{
    Q_D(const QtStateSetPropertyAction);
    return d->value;
}

/*!
  Sets the value set by this action.
*/
void QtStateSetPropertyAction::setValue(const QVariant &value)
{
    Q_D(QtStateSetPropertyAction);
    d->value = value;
}

/*!
  \reimp
*/
void QtStateSetPropertyAction::execute()
{
    Q_D(QtStateSetPropertyAction);
    if (!d->target)
        return;
    d->target->setProperty(d->propertyName, d->value);
}

/*!
  \reimp
*/
bool QtStateSetPropertyAction::event(QEvent *e)
{
    return QtStateAction::event(e);
}

QtStateInvokeMethodActionPrivate *QtStateInvokeMethodActionPrivate::get(QtStateInvokeMethodAction *q)
{
    return q->d_func();
}

/*!
  \class QtStateInvokeMethodAction

  \brief The QtStateInvokeMethodAction class provides an invoke method action for QObjects.

  

  The QtStateInvokeMethodAction class provides an action that calls a method of
  a QObject when a QtState is entered or exited. QtStateInvokeMethodAction is
  part of \l{The State Machine Framework}.

  Typically you don't construct QtStateInvokeMethodAction objects directly, but
  rather call the QtState::invokeMethodOnEntry() function or the
  QtState::invokeMethodOnExit() function.
*/

/*!
    \property QtStateInvokeMethodAction::arguments

    \brief the arguments to the method this action invokes
*/

/*!
    \property QtStateInvokeMethodAction::methodName

    \brief the name of the method this action invokes
*/

/*!
    \property QtStateInvokeMethodAction::target

    \brief the object on which this action invokes a method
*/

/*!
  Constructs a new QtStateInvokeMethodAction object for the method named \a
  methodName of the given \a target object, with the given \a parent.
*/
QtStateInvokeMethodAction::QtStateInvokeMethodAction(
    QObject *target, const QByteArray &methodName, QObject *parent)
    : QtStateAction(*new QtStateInvokeMethodActionPrivate, parent)
{
    Q_D(QtStateInvokeMethodAction);
    d->target = target;
    d->methodName = methodName;
    d->methodIndex = -1;
}

/*!
  Constructs a new QtStateInvokeMethodAction object for the method named \a
  methodName of the given \a target object, with the given arguments, \a args,
  and with the given \a parent.
*/
QtStateInvokeMethodAction::QtStateInvokeMethodAction(
    QObject *target, const QByteArray &methodName,
    const QList<QVariant> &args, QObject *parent)
    : QtStateAction(*new QtStateInvokeMethodActionPrivate, parent)
{
    Q_D(QtStateInvokeMethodAction);
    d->target = target;
    d->methodName = methodName;
    d->methodIndex = -1;
    d->args = args;
}

/*!
  Constructs a new QtStateInvokeMethodAction object with the given \a parent.
*/
QtStateInvokeMethodAction::QtStateInvokeMethodAction(QObject *parent)
    : QtStateAction(*new QtStateInvokeMethodActionPrivate, parent)
{
    Q_D(QtStateInvokeMethodAction);
    d->target = 0;
    d->methodIndex = -1;
}

/*!
  Destroys this QtStateInvokeMethodAction object.
*/
QtStateInvokeMethodAction::~QtStateInvokeMethodAction()
{
}

/*!
  \reimp
*/
void QtStateInvokeMethodAction::execute()
{
    Q_D(QtStateInvokeMethodAction);
    if (!d->target)
        return;

    if (d->methodIndex == -1) {
        QVarLengthArray<char, 512> sig;
        int len = d->methodName.length();
        if (len <= 0)
            return;
        sig.append(d->methodName, len);
        sig.append('(');

        int paramCount;
        for (paramCount = 0; paramCount < d->args.size() && paramCount < 10; ++paramCount) {
            const char *tn = d->args.at(paramCount).typeName();
            len = qstrlen(tn);
            if (len <= 0)
                break;
            sig.append(tn, len);
            sig.append(',');
        }
        if (paramCount == 0)
            sig.append(')'); // no parameters
        else
            sig[sig.size() - 1] = ')';
        sig.append('\0');

        const QMetaObject *meta = d->target->metaObject();
        int idx = meta->indexOfMethod(sig.constData());
        if (idx < 0) {
            QByteArray norm = QMetaObject::normalizedSignature(sig.constData());
            idx = meta->indexOfMethod(norm.constData());
            if ((idx < 0) || (idx >= meta->methodCount())) {
                qWarning("InvokeMethodAction: unable to find method '%s' of %s(%p)",
                         sig.constData(), meta->className(), d->target);
                return;
            }
        }
        d->methodIndex = idx;
    }

    void *param[11];
    param[0] = 0; // return value
    for (int i = 0; i < 10; ++i)
        param[i+1] = (i < d->args.size()) ? const_cast<void*>(d->args.at(i).constData()) : (void*)0;
    (void)d->target->qt_metacall(QMetaObject::InvokeMetaMethod, d->methodIndex, param);
}

/*!
  Returns the object on which this action invokes a method.
*/
QObject *QtStateInvokeMethodAction::targetObject() const
{
    Q_D(const QtStateInvokeMethodAction);
    return d->target;
}

/*!
  Sets the object on which this action invokes a method.
*/
void QtStateInvokeMethodAction::setTargetObject(QObject *target)
{
    Q_D(QtStateInvokeMethodAction);
    d->target = target;
}

/*!
  Returns the name of the method this action will invoke.
*/
QByteArray QtStateInvokeMethodAction::methodName() const
{
    Q_D(const QtStateInvokeMethodAction);
    return d->methodName;
}

/*!
  Sets the name of the method this action will invoke.
*/
void QtStateInvokeMethodAction::setMethodName(const QByteArray &methodName)
{
    Q_D(QtStateInvokeMethodAction);
    if (methodName != d->methodName) {
        d->methodName = methodName;
        d->methodIndex = -1;
    }
}

/*!
  Returns the arguments to the method this action will invoke.
*/
QVariantList QtStateInvokeMethodAction::arguments() const
{
    Q_D(const QtStateInvokeMethodAction);
    return d->args;
}

/*!
  Sets the arguments to the method this action will invoke.
*/
void QtStateInvokeMethodAction::setArguments(const QVariantList &arguments)
{
    Q_D(QtStateInvokeMethodAction);
    if (d->args != arguments) {
        d->args = arguments;
        d->methodIndex = -1;
    }
}

/*!
  \reimp
*/
bool QtStateInvokeMethodAction::event(QEvent *e)
{
    return QtStateAction::event(e);
}

QT_END_NAMESPACE
