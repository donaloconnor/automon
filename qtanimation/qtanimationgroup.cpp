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
    \class QtAnimationGroup
    \brief The QtAnimationGroup is an abstract base class for group of animations.
    
    
    \preliminary

    QtAnimationGroup represents a group of animations, such as parallel or sequential,
    and lets you combine different animations into one. The group manages any animation
    that inherits QtAbstractAnimation. By combining groups, you can easily construct
    complex animation graphs.

    The QtAnimationGroup base class provides methods for adding and retrieving animations.
    Besides that, you can remove animations by calling remove(), and clear the animation
    group by calling clearAnimations(). You may keep track of changes in the group's animations by
    listening to QEvent::ChildAdded and QEvent::ChildRemoved events.

    QtAnimationGroup takes ownership of the animations it manages, and ensures that they are
    deleted when the animation group is deleted.

    \sa QtAbstractAnimation, QtVariantAnimation, {The Animation Framework}
*/

#ifndef QT_NO_ANIMATION

#include "qtanimationgroup.h"
#include <QtCore/qdebug.h>
#include <QtCore/qcoreevent.h>
#include "qtanimationgroup_p.h"

QT_BEGIN_NAMESPACE


/*!
    Constructs a QtAnimationGroup.
    \a parent is passed to QObject's constructor.
*/
QtAnimationGroup::QtAnimationGroup(QObject *parent)
    : QtAbstractAnimation(*new QtAnimationGroupPrivate, parent)
{
}

/*!
    \internal
*/
QtAnimationGroup::QtAnimationGroup(QtAnimationGroupPrivate &dd, QObject *parent)
    : QtAbstractAnimation(dd, parent)
{
}

/*!
    Destroys the animation group. It will also destroy all its animations.
*/
QtAnimationGroup::~QtAnimationGroup()
{
}

/*!
    Returns a pointer to the animation at \a index in this group. This
    function is useful when you need access to a particular animation.  \a
    index is between 0 and animationCount() - 1.

    \sa animationCount(), indexOfAnimation()
*/
QtAbstractAnimation *QtAnimationGroup::animationAt(int index) const
{
    Q_D(const QtAnimationGroup);

    if (index < 0 || index >= d->animations.size()) {
        qWarning("QtAnimationGroup::animationAt: index is out of bounds");
        return 0;
    }

    return d->animations.at(index);
}


/*!
    Returns the number of animations managed by this group.

    \sa indexOfAnimation(), addAnimation(), animationAt()
*/
int QtAnimationGroup::animationCount() const
{
    Q_D(const QtAnimationGroup);
    return d->animations.size();
}

/*!
    Returns the index of \a animation. The returned index can be passed
    to the other functions that take an index as an argument.

    \sa insertAnimationAt() animationAt(), takeAnimationAt()
*/
int QtAnimationGroup::indexOfAnimation(QtAbstractAnimation *animation) const
{
    Q_D(const QtAnimationGroup);
    return d->animations.indexOf(animation);
}

/*!
    Adds \a animation to this group. This will call insertAnimationAt with
    index equals to animationCount()
*/
void QtAnimationGroup::addAnimation(QtAbstractAnimation *animation)
{
    Q_D(QtAnimationGroup);
    insertAnimationAt(d->animations.count(), animation);
}

/*!
    Inserts \a animation into this animation group at \a index.
    If \a index is 0 the animation is inserted at the beginning.
    If \a index is animationCount(), the animation is inserted at the end.
    \sa takeAnimationAt(), addAnimation(), indexOfAnimation()
*/
void QtAnimationGroup::insertAnimationAt(int index, QtAbstractAnimation *animation)
{
    Q_D(QtAnimationGroup);

    if (index < 0 || index > d->animations.size()) {
        qWarning("QtAnimationGroup::insertAnimationAt: index is out of bounds");
        return;
    }

    d->animations.insert(index, animation);
    if (QtAnimationGroup *oldGroup = animation->group())
        oldGroup->removeAnimation(animation);
    QtAbstractAnimationPrivate::get(animation)->group = this;
    // this will make sure that ChildAdded event is sent to 'this'
    animation->setParent(this);
    d->animationInsertedAt(index);
}

/*!
    Removes \a animation from this group. The ownership of \a animation is
    transferred to the caller.

    \sa takeAnimationAt(), insertAnimationAt(), addAnimation()
*/
void QtAnimationGroup::removeAnimation(QtAbstractAnimation *animation)
{
    Q_D(QtAnimationGroup);

    if (!animation) {
        qWarning("QtAnimationGroup::remove: cannot remove null animation");
        return;
    }
    int index = d->animations.indexOf(animation);
    if (index == -1) {
        qWarning("QtAnimationGroup::remove: animation is not part of this group");
        return;
    }

    takeAnimationAt(index);
}

/*!
    Removes the animation at \a index from this animation group. The ownership
    of the animation is transferred to the caller, and a pointer to the removed
    animation is returned.

    \sa addAnimation()
*/
QtAbstractAnimation *QtAnimationGroup::takeAnimationAt(int index)
{
    Q_D(QtAnimationGroup);
    if (index < 0 || index >= d->animations.size()) {
        qWarning("QtAnimationGroup::takeAnimationAt: no animation at index %d", index);
        return 0;
    }
    QtAbstractAnimation *animation = d->animations.at(index);
    QtAbstractAnimationPrivate::get(animation)->group = 0;
    // ### removing from list before doing setParent to avoid inifinite recursion
    // in ChildRemoved event
    d->animations.removeAt(index);
    animation->setParent(0);
    d->animationRemovedAt(index);
    return animation;
}

/*!
    Removes and deletes all animations in this animation group, and resets the current
    time to 0.

    \sa addAnimation(), removeAnimation()
*/
void QtAnimationGroup::clearAnimations()
{
    Q_D(QtAnimationGroup);
    qDeleteAll(d->animations);
}

/*!
    \reimp
*/
bool QtAnimationGroup::event(QEvent *event)
{
    Q_D(QtAnimationGroup);
    if (event->type() == QEvent::ChildAdded) {
        QChildEvent *childEvent = static_cast<QChildEvent *>(event);
        if (QtAbstractAnimation *a = qobject_cast<QtAbstractAnimation *>(childEvent->child())) {
            if (a->group() != this)
                addAnimation(a);
        }
    } else if (event->type() == QEvent::ChildRemoved) {
        QChildEvent *childEvent = static_cast<QChildEvent *>(event);
        QtAbstractAnimation *a = static_cast<QtAbstractAnimation *>(childEvent->child());
        // You can only rely on the child being a QObject because in the QEvent::ChildRemoved
        // case it might be called from the destructor.
        int index = d->animations.indexOf(a);
        if (index != -1)
            takeAnimationAt(index);
    }
    return QtAbstractAnimation::event(event);
}


void QtAnimationGroupPrivate::animationRemovedAt(int index)
{
    Q_Q(QtAnimationGroup);
    Q_UNUSED(index);
    if (animations.isEmpty()) {
        currentTime = 0;
        q->stop();
    }
}

QT_END_NAMESPACE

#include "moc_qtanimationgroup.cpp"

#endif //QT_NO_ANIMATION
