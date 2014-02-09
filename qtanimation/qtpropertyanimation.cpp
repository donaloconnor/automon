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



/*!
    \class QtPropertyAnimation
    \brief The QtPropertyAnimation class animates properties for QObject(and QWidget)
    
    \preliminary

    This class is part of {The Animation Framework}.  You can use QtPropertyAnimation
    by itself as a simple animation class, or as part of more complex
    animations through QtAnimationGroup.

    The most common way to use QtPropertyAnimation is to construct an instance 
    of it by passing a pointer to a QObject or a QWidget, and the name of the
    property you would like to animate to QtPropertyAnimation's constructor.

    The start value of the animation is optional. If you do not set any start
    value, the animation will operate on the target's current property value
    at the point when the animation was started. You can call setStartValue()
    to set the start value, and setEndValue() to set the target value for
    the animated property.

    Animations can operate on QObjects and QWidgets. You can choose to assign a
    target object by either calling setTargetObject() or by passing a QObject
    pointer to QtPropertyAnimation's constructor.

    \sa QtVariantAnimation, QtAnimationGroup, {The Animation Framework}
*/

#ifndef QT_NO_ANIMATION

#include "qtpropertyanimation.h"
#include "qtanimationgroup.h"
#include <QtCore/qdebug.h>

#include "qtpropertyanimation_p.h"

#include <QtCore/qmath.h>
#include <QtCore/qmutex.h>

QT_BEGIN_NAMESPACE

typedef QPair<QObject *, QByteArray> QtPropertyAnimationPair;
typedef QHash<QtPropertyAnimationPair, QtPropertyAnimation*> QtPropertyAnimationHash;
Q_GLOBAL_STATIC(QtPropertyAnimationHash, _q_runningAnimations);
Q_GLOBAL_STATIC_WITH_ARGS(QMutex, guardHashLock, (QMutex::Recursive) )

void QtPropertyAnimationPrivate::updateMetaProperty()
{
    if (!target || propertyName.isEmpty())
        return;

    if (hasMetaProperty == 0 && !property.isValid()) {
        const QMetaObject *mo = target->metaObject();
        propertyIndex = mo->indexOfProperty(propertyName);
        if (propertyIndex != -1) {
            hasMetaProperty = 1;
            property = mo->property(propertyIndex);
            propertyType = property.userType();
        } else {
            hasMetaProperty = 2;
        }
    }

    if (property.isValid())
        convertValues(propertyType);
}

void QtPropertyAnimationPrivate::updateProperty(const QVariant &newValue)
{
    if (!target || state == QtAbstractAnimation::Stopped)
        return;

    if (hasMetaProperty == 1) {
        if (newValue.userType() == propertyType) {
          //no conversion is needed, we directly call the QObject::qt_metacall
          void *data = const_cast<void*>(newValue.constData());
          target->qt_metacall(QMetaObject::WriteProperty, propertyIndex, &data);
        } else {
          property.write(target, newValue);
        }
    } else {
        target->setProperty(propertyName.constData(), newValue);
    }
}

/*!
    Construct a QtPropertyAnimation object. \a parent is passed to QObject's
    constructor.
*/
QtPropertyAnimation::QtPropertyAnimation(QObject *parent)
    : QtVariantAnimation(*new QtPropertyAnimationPrivate, parent)
{
}

/*!
    Construct a QtPropertyAnimation object. \a parent is passed to QObject's
    constructor. The animation changes the property \a propertyName on \a
    target. The default duration is 250ms.

    \sa targetObject, propertyName
*/
QtPropertyAnimation::QtPropertyAnimation(QObject *target, const QByteArray &propertyName, QObject *parent)
    : QtVariantAnimation(*new QtPropertyAnimationPrivate, parent)
{
    setTargetObject(target);
    setPropertyName(propertyName);
}

/*!
    Destroys the QtPropertyAnimation instance.
 */
QtPropertyAnimation::~QtPropertyAnimation()
{
    stop();
}

/*!
    \property QtPropertyAnimation::targetObject
    \brief the target QObject for this animation.

    This property defines the target QObject for this animation.
 */
QObject *QtPropertyAnimation::targetObject() const
{
    Q_D(const QtPropertyAnimation);
    return d->target;
}
void QtPropertyAnimation::setTargetObject(QObject *target)
{
    Q_D(QtPropertyAnimation);
    if (d->target == target)
        return;

    d->target = target;
    d->hasMetaProperty = 0;
    d->updateMetaProperty();
}

/*!
    \property QtPropertyAnimation::propertyName
    \brief the target property name for this animation

    This property defines the target property name for this animation. The
    property name is required for the animation to operate.
 */
QByteArray QtPropertyAnimation::propertyName() const
{
    Q_D(const QtPropertyAnimation);
    return d->propertyName;
}
void QtPropertyAnimation::setPropertyName(const QByteArray &propertyName)
{
    Q_D(QtPropertyAnimation);
    d->propertyName = propertyName;
    d->hasMetaProperty = 0;
    d->updateMetaProperty();
}


/*!
    \reimp
 */
bool QtPropertyAnimation::event(QEvent *event)
{
    return QtVariantAnimation::event(event);
}

/*!
    This virtual function is called by QtVariantAnimation whenever the current value
    changes. \a value is the new, updated value. It updates the current value
    of the property on the target object.

    \sa currentValue, currentTime
 */
void QtPropertyAnimation::updateCurrentValue(const QVariant &value)
{
    Q_D(QtPropertyAnimation);
    d->updateProperty(value);
}

/*!
    \reimp
*/
void QtPropertyAnimation::updateState(QtAbstractAnimation::State oldState,
                                     QtAbstractAnimation::State newState)
{
    Q_D(QtPropertyAnimation);
    QtVariantAnimation::updateState(oldState, newState);
    QMutexLocker locker(guardHashLock());
    QtPropertyAnimationHash * hash = _q_runningAnimations();
    QtPropertyAnimationPair key(d->target, d->propertyName);
    if (newState == Running) {
        d->updateMetaProperty();
        QtPropertyAnimation *oldAnim = hash->value(key, 0);
        if (oldAnim) {
            //we try to stop the top level group
            QtAbstractAnimation *current = oldAnim;
            while(current->group() && current->state() != Stopped) current = current->group();
            current->stop();
        }
        hash->insert(key, this);
        // Initialize start value
        if (d->target && !d->defaultStartValue.isValid() && (d->atBeginning() || d->atEnd())) {
            d->setDefaultStartValue(d->target->property(d->propertyName.constData()));
        }
    } else if (hash->value(key) == this) {
        hash->remove(key);
    }

}

#include "moc_qtpropertyanimation.cpp"

QT_END_NAMESPACE

#endif //QT_NO_ANIMATION
