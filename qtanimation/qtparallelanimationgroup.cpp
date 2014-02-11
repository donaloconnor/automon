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
    \class QtParallelAnimationGroup
    \brief The QtParallelAnimationGroup class provides a parallel group of animations.
    
    
    \preliminary

    The animations are all started at the same time, and run in parallel. The animation group
    finishes when the longest lasting animation has finished.
*/

#ifndef QT_NO_ANIMATION

#include "qtparallelanimationgroup.h"
#include "qtparallelanimationgroup_p.h"
//#define QANIMATION_DEBUG
QT_BEGIN_NAMESPACE

/*!
    Constructs a QtParallelAnimationGroup.
    \a parent is passed to QObject's constructor.
*/
QtParallelAnimationGroup::QtParallelAnimationGroup(QObject *parent)
    : QtAnimationGroup(*new QtParallelAnimationGroupPrivate, parent)
{
}

/*!
    \internal
*/
QtParallelAnimationGroup::QtParallelAnimationGroup(QtParallelAnimationGroupPrivate &dd,
                                                 QObject *parent)
    : QtAnimationGroup(dd, parent)
{
}

/*!
    Destroys the animation group. It will also destroy all its animations.
*/
QtParallelAnimationGroup::~QtParallelAnimationGroup()
{
}

/*!
    \reimp
*/
int QtParallelAnimationGroup::duration() const
{
    Q_D(const QtParallelAnimationGroup);
    int ret = 0;

    for (int i = 0; i < d->animations.size(); ++i) {
        QtAbstractAnimation *animation = d->animations.at(i);
        const int currentDuration = animation->totalDuration();
        if (currentDuration == -1)
            return -1; // Undetermined length

        ret = qMax(ret, currentDuration);
    }

    return ret;
}

/*!
    \reimp
*/
void QtParallelAnimationGroup::updateCurrentTime(int)
{
    Q_D(QtParallelAnimationGroup);
    if (d->animations.isEmpty())
        return;

    if (d->currentIteration > d->lastIteration) {
        // simulate completion of the loop
        int dura = duration();
        if (dura > 0) {
            foreach (QtAbstractAnimation *animation, d->animations) {
                animation->setCurrentTime(dura);   // will stop
            }
        }
    } else if (d->currentIteration < d->lastIteration) {
        // simulate completion of the loop seeking backwards
        foreach (QtAbstractAnimation *animation, d->animations) {
            animation->setCurrentTime(0);
            animation->stop();
        }
    }

    bool timeFwd = ((d->currentIteration == d->lastIteration && d->currentTime >= d->lastCurrentTime)
                   || d->currentIteration > d->lastIteration);
#ifdef QANIMATION_DEBUG
    qDebug("QParallellAnimationGroup %5d: setCurrentTime(%d), loop:%d, last:%d, timeFwd:%d, lastcurrent:%d, %d",
        __LINE__, d->currentTime, d->currentIteration, d->lastIteration, timeFwd, d->lastCurrentTime, state());
#endif
    // finally move into the actual time of the current loop
    foreach (QtAbstractAnimation *animation, d->animations) {
        const int dura = animation->totalDuration();
        if (dura == -1 && d->isUncontrolledAnimationFinished(animation))
            continue;
        if (dura == -1 || (d->currentTime <= dura && dura != 0)
            || (dura == 0 && d->currentIteration != d->lastIteration)) {
            switch (state()) {
            case Running:
                animation->start();
                break;
            case Paused:
                animation->pause();
                break;
            case Stopped:
            default:
                break;
            }
        }

        if (dura <= 0) {
            if (dura == -1)
                animation->setCurrentTime(d->currentTime);
            continue;
        }

        if ((timeFwd && d->lastCurrentTime <= dura)
            || (!timeFwd && d->currentTime <= dura))
                animation->setCurrentTime(d->currentTime);
        if (d->currentTime > dura)
            animation->stop();
    }
    d->lastIteration = d->currentIteration;
    d->lastCurrentTime = d->currentTime;
}

/*!
    \reimp
*/
void QtParallelAnimationGroup::updateState(QtAbstractAnimation::State oldState,
                                          QtAbstractAnimation::State newState)
{
    Q_D(QtParallelAnimationGroup);
    QtAnimationGroup::updateState(oldState, newState);

    switch (newState) {
    case Stopped:
        foreach (QtAbstractAnimation *animation, d->animations)
            animation->stop();
        d->disconnectUncontrolledAnimations();
        break;
    case Paused:
        foreach (QtAbstractAnimation *animation, d->animations)
            animation->pause();
        break;
    case Running:
        d->connectUncontrolledAnimations();
        foreach (QtAbstractAnimation *animation, d->animations) {
            animation->stop();
            animation->setDirection(d->direction);
            animation->start();
        }
        break;
    }
}

void QtParallelAnimationGroupPrivate::_q_uncontrolledAnimationFinished()
{
    Q_Q(QtParallelAnimationGroup);

    QtAbstractAnimation *animation = qobject_cast<QtAbstractAnimation *>(q->sender());
    Q_ASSERT(animation);

    int uncontrolledRunningCount = 0;
    if (animation->duration() == -1 || animation->iterationCount() < 0) {
        QHash<QtAbstractAnimation *, int>::iterator it = uncontrolledFinishTime.begin();
        while (it != uncontrolledFinishTime.end()) {
            if (it.key() == animation) {
                *it = animation->currentTime();
            }
            if (it.value() == -1)
                ++uncontrolledRunningCount;
            ++it;
        }
    }

    if (uncontrolledRunningCount > 0)
        return;

    int maxDuration = 0;
    foreach (QtAbstractAnimation *a, animations)
        maxDuration = qMax(maxDuration, a->totalDuration());

    if (currentTime >= maxDuration)
        q->stop();
}

void QtParallelAnimationGroupPrivate::disconnectUncontrolledAnimations()
{
    Q_Q(QtParallelAnimationGroup);

    QHash<QtAbstractAnimation *, int>::iterator it = uncontrolledFinishTime.begin();
    while (it != uncontrolledFinishTime.end()) {
        QObject::disconnect(it.key(), SIGNAL(finished()), q, SLOT(_q_uncontrolledAnimationFinished()));
        ++it;
    }

    uncontrolledFinishTime.clear();
}

void QtParallelAnimationGroupPrivate::connectUncontrolledAnimations()
{
    Q_Q(QtParallelAnimationGroup);

    foreach (QtAbstractAnimation *animation, animations) {
        if (animation->duration() == -1 || animation->iterationCount() < 0) {
            uncontrolledFinishTime[animation] = -1;
            QObject::connect(animation, SIGNAL(finished()), q, SLOT(_q_uncontrolledAnimationFinished()));
        }
    }
}

bool QtParallelAnimationGroupPrivate::isUncontrolledAnimationFinished(QtAbstractAnimation *anim) const
{
    return uncontrolledFinishTime.value(anim, -1) >= 0;
}

/*!
    \reimp
*/
void QtParallelAnimationGroup::updateDirection(QtAbstractAnimation::Direction direction)
{
    Q_D(QtParallelAnimationGroup);
    //we need to update the direction of the current animation
    if (state() != Stopped) {
        foreach(QtAbstractAnimation *anim, d->animations) {
            anim->setDirection(direction);
        }
    } else {
        if (direction == Forward) {
            d->lastIteration = 0;
            d->lastCurrentTime = 0;
        } else {
            // Looping backwards with iterationCount == -1 does not really work well...
            d->lastIteration = (d->iterationCount == -1 ? 0 : d->iterationCount - 1);
            d->lastCurrentTime = duration();
        }
    }
}

/*!
    \reimp
*/
bool QtParallelAnimationGroup::event(QEvent *event)
{
    return QtAnimationGroup::event(event);
}

QT_END_NAMESPACE

#include "moc_qtparallelanimationgroup.cpp"

#endif //QT_NO_ANIMATION
