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



#ifndef QT_NO_ANIMATION

#include "qtanimationstate.h"
#include "qtparallelanimationgroup.h"
#include "qtabstracttransition.h"
#include "qtabstracttransition_p.h"
#include "qtstatefinishedtransition.h"
#include "qtsignaltransition.h"
#include "qtpropertyanimation.h"
#include "qtstatemachine.h"
#include "qtstatemachine_p.h"
#include "qtstateaction.h"
#include "qtstateaction_p.h"
#include "qtstate.h"
#include "qtstate_p.h"
#include "qtfinalstate.h"
#include "qtsignaltransition_p.h"
#ifdef QT_STATEMACHINE_SOLUTION
#include "qtvariantanimation_p.h"
#else
#include "private/qvariantanimation_p.h"
#endif
#include "qtpauseanimation.h"

#include <QtCore/qhash.h>
#include <QtCore/qstack.h>
#include <QtCore/qlist.h>
#include <QtCore/qsize.h>
#include <QtCore/qrect.h>
#include <QtCore/qpoint.h>
#include <QtCore/qbitarray.h>

QT_BEGIN_NAMESPACE

/*!
  \class QtAnimationState
  \brief The QtAnimationState class provides a state that plays one or more animations.
  
  \preliminary

  QtAnimationState is part of \l{The State Machine Framework}.

  The addAnimation() function adds an animation to be played by the state.

  When the state is entered, it will call each of the animations' start() functions.
  When the animation is finished, a QtStateFinishedEvent is posted; you can
  use the QtStateFinishedTransition class to associate a transition with this
  event.

  \code
    QPushButton button;
    QtPropertyAnimation animation(&button, "geometry");
    animation.setEndValue(QRect(100, 100, 400, 400));

    QtStateMachine machine;
    QtAnimationState *s1 = new QtAnimationState(&animation, machine.rootState());
    QtState *s2 = new QtState(machine.rootState());
    s1->addFinishedTransition(s2);
  \endcode

  If the state is exited before the animation has finished, the animations will
  be stopped, and no event is generated.

  For convenience, the QtState::addAnimatedTransition() functions can be used to set up the 
  animated transition between two states. 

  \section1 Initializing animations automatically
  QtAnimationState will try to automatically initialize any QtPropertyAnimation for which no 
  specific end value has been set. It will set the animation's end value based on actions in the 
  target state of the animation state's QtStateFinishedTransition. 
  
  The only actions evaluated are the entry actions of the very first states entered after the 
  animation state has finished.
  
  QtAnimationState will match its QtPropertyAnimation objects with QtStateSetPropertyAction objects 
  in the target states that manipulate the same property on the same object. The end values of the
  animations will be initialized to the values set by the actions.

  \code
    QPushButton button;
    QtPropertyAnimation animation(&button, "geometry");

    QtStateMachine machine;
    QtAnimationState *s1 = new QtAnimationState(&animation, machine.rootState());
    QtState *s2 = new QtState(machine.rootState());
    s2->setPropertyOnEntry(&button, "geometry", QRect(100, 100, 400, 400));

    s1->addFinishedTransition(s2);
  \endcode

  Specifically, QtAnimationState will evaluate the actions set for the target state itself, and 
  also look in following locations:
  objects:
  \list
    \i If the state has children and is not parallel, actions set for its initial 
       state will be evaluated.
    \i If the state has children and is parallel, actions set for any of its children will be
       evaluated.
  \endlist

  Children of the target state will be evaluated recursively. 

  \section1 Animated restoring of properties 

  When a state has restore policy QtActionState::RestoreProperties, any
  property that is set in the state using the QtStateSetPropertyAction will
  potentially be restored to its original value later on. When regular,
  unanimated transitions are used, the properties will be restored when the
  state machine enters one or more states that do not explicitly set the
  property.

  When QtAnimationState is used, it will restore the property with an
  animation. Rather than have the state machine restore the properties as it
  enters the target state, they will be restored by the QtAnimationState in
  parallel to the regular animations that have been added to the state.

  If no animation has been added to the state, only the restore animations
  will be played.

  The animations used to restore the properties are QtPropertyAnimations with
  with the default easing curve and duration.

  \sa QtActionState::RestorePolicy, QtPropertyAnimation, QtStateSetPropertyAction, 
      QtState::addAnimatedTransition()      

*/

namespace {

class AnimatingState : public QtState
{
public:
    AnimatingState(QtState *parent)
        : QtState(parent) {}
protected:
    void onEntry() {}
    void onExit() {}
};

class AnimationFinishedState : public QtFinalState
{
public:
    AnimationFinishedState(QtState *parent)
        : QtFinalState(parent)
    {
    }   

protected:
    void onEntry() {}
    void onExit() {}
};

class AnimationFinishedTransition: public QtSignalTransition
{
public:
    AnimationFinishedTransition(const QtAbstractAnimation *animation,
                                QtAnimationStatePrivate *animationState_d)
        : QtSignalTransition(animation, SIGNAL(finished())), m_animationState_d(animationState_d)
    {
    }

    virtual bool testCondition(QEvent *) const;

private:
    QtAnimationStatePrivate *m_animationState_d;
};

} // namespace

class QtAnimationStatePrivate : public QtStatePrivate
{
    Q_DECLARE_PUBLIC(QtAnimationState)

public:
    typedef QtStateMachinePrivate::RestorableId RestorableId;

    QtAnimationStatePrivate();
    void init();
    QtAbstractTransition *finishedTransition() const;

    void initializeAnimation(const QList<QtStateAction*> &actions, QtActionState::RestorePolicy restorePolicy);
    void initializeAnimation(const QList<QtAbstractState*> &targets);
    void initializeAnimationFromAction(QtAbstractAnimation *anim, 
                                       QtStateAction *action,
                                       QtActionState::RestorePolicy restorePolicy);

    void restoreAnimations();

    void addAnimation(QtAbstractAnimation *animation, QList<QtAbstractAnimation*> &list);
    void removeAnimation(QtAbstractAnimation *animation, QList<QtAbstractAnimation*> &list);

    QList<QtAbstractAnimation *> animations;
    QList<QtAbstractAnimation *> restorationAnimations;
    QList<QtPropertyAnimation *> resetEndValues;
    QtState *animatingState;
    QtFinalState *finishedState;
    QTimer *timer;

    uint initializeAnimationFromTargetStates        :  1;
    uint initializeAnimationFromRestorableVariables :  1;
    uint reserved                                   : 30;       
    
    QHash<RestorableId, QVariant> pendingRestorables;
};

// implement here because it requires the definition of QtAnimationStatePrivate.
namespace {
    bool AnimationFinishedTransition::testCondition(QEvent *e) const
    {    
        if (!QtSignalTransition::testCondition(e))
            return false;

        QList<QtAbstractAnimation *> animations = m_animationState_d->animations;
        QList<QtAbstractAnimation *> restorationAnimations = m_animationState_d->restorationAnimations;

        for (int i=0; i<animations.size(); ++i) {
            if (animations.at(i)->state() != QtAbstractAnimation::Stopped)
                return false;
        }

        for (int i=0; i<restorationAnimations.size(); ++i) {
            if (restorationAnimations.at(i)->state() != QtAbstractAnimation::Stopped)
                return false;
        }

        return true;
    }
}

QtAnimationStatePrivate::QtAnimationStatePrivate()
{
}

void QtAnimationStatePrivate::init()
{
    Q_Q(QtAnimationState);

    // ### make it a configurable property, as it is highly magical
    initializeAnimationFromTargetStates = true;
    initializeAnimationFromRestorableVariables = true;

    animatingState = new AnimatingState(q);
    q->setInitialState(animatingState);
    finishedState = new AnimationFinishedState(q);    

    timer = 0;
}

void QtAnimationStatePrivate::addAnimation(QtAbstractAnimation *animation, 
                                          QList<QtAbstractAnimation*> &list)
{
    if (animation != 0 && !list.contains(animation)) {
        list.append(animation);
        AnimationFinishedTransition *transition = new AnimationFinishedTransition(animation, this);
        animatingState->addTransition(transition, finishedState);
    }
}

void QtAnimationStatePrivate::removeAnimation(QtAbstractAnimation *animation,
                                             QList<QtAbstractAnimation*> &list)
{
    if (animation != 0 && list.contains(animation)) {
        QtStatePrivate *state_d = QtStatePrivate::get(animatingState); 
        QList<QtAbstractTransition *> transitions = state_d->transitions();
        Q_ASSERT(transitions.size() > 0);
        for (int i=0; i<transitions.size(); ++i) {
            QtSignalTransition *transition = qobject_cast<QtSignalTransition *>(transitions.at(i));

            if (transition != 0) {
                QtSignalTransitionPrivate *transition_p = QtSignalTransitionPrivate::get(transition);
                if (transition_p->sender == animation) {
                    delete transition;
                    break;
                }
            }            
        }

        list.removeAll(animation);
    }
}

/*!
  \internal

  Returns a transition from this state that is triggered when this state is
  finished, or 0 if there is no such transition.
*/
QtAbstractTransition *QtAnimationStatePrivate::finishedTransition() const
{
    QList<QtAbstractTransition*> trans = transitions();
    for (int i = 0; i < trans.size(); ++i) {
        QtAbstractTransition *t = trans.at(i);
        if (QtStateFinishedTransition *sft = qobject_cast<QtStateFinishedTransition*>(t))
            return sft;
    }
    return 0;
}

void QtAnimationStatePrivate::initializeAnimationFromAction(QtAbstractAnimation *abstractAnimation, 
                                                           QtStateAction *action,
                                                           QtActionState::RestorePolicy restorePolicy)
{
    QtAnimationGroup *group = qobject_cast<QtAnimationGroup*>(abstractAnimation);
    if (group) {
        for (int i = 0; i < group->animationCount(); ++i) {
            QtAbstractAnimation *animationChild = group->animationAt(i);
            initializeAnimationFromAction(animationChild, action, restorePolicy);
        }
    } else { 
        QtPropertyAnimation *animation = qobject_cast<QtPropertyAnimation *>(abstractAnimation);
        QtStateSetPropertyAction *propertyAction = qobject_cast<QtStateSetPropertyAction*>(action);
        if (propertyAction != 0 
            && animation != 0 
            && propertyAction->targetObject() == animation->targetObject()
            && propertyAction->propertyName() == animation->propertyName()) {

            if (!animation->startValue().isValid()) {                    
                QByteArray propertyName = animation->propertyName();
                QVariant currentValue = animation->targetObject()->property(propertyName);

                QtVariantAnimationPrivate::get(animation)->setDefaultStartValue(currentValue);
            }

            // Only change end value if it is undefined
            if (!animation->endValue().isValid()) {
                QtStateMachinePrivate *machine_d = QtStateMachinePrivate::get(machine());
                if (restorePolicy == QtActionState::RestoreProperties)
                    machine_d->registerRestorable(animation);          

                RestorableId id(animation->targetObject(), animation->propertyName());
                pendingRestorables.remove(id);

                animation->setEndValue(propertyAction->value());
                resetEndValues.append(animation);
            }
        }
    }
}

void QtAnimationStatePrivate::initializeAnimation(const QList<QtStateAction*> &actions,
                                                 QtActionState::RestorePolicy restorePolicy)
{

    for (int i = 0; i < actions.size(); ++i) {
        QtStateAction *act = actions.at(i);
        
        for (int j=0; j<animations.size(); ++j)
            initializeAnimationFromAction(animations.at(j), act, restorePolicy);
    }


}

void QtAnimationStatePrivate::initializeAnimation(const QList<QtAbstractState*> &targets)
{
    // ### consider resulting action order, and how to resolve conflicts (two actions that set the same property)
    for (int i = 0; i < targets.size(); ++i) {
        QtActionState *s = qobject_cast<QtActionState*>(targets.at(i));
        if (s != 0) {
            QtActionState::RestorePolicy restorePolicy = s->restorePolicy();
            if (restorePolicy == QtActionState::GlobalRestorePolicy)
                restorePolicy = machine()->globalRestorePolicy();
            initializeAnimation(QtActionStatePrivate::get(s)->entryActions(), restorePolicy);
        }

        if (QtStateMachinePrivate::isParallel(s)) {
            initializeAnimation(QtStatePrivate::get(qobject_cast<QtState*>(s))->childStates());
        } else if (QtStateMachinePrivate::isCompound(s)) {
            initializeAnimation(QList<QtAbstractState*>() << qobject_cast<QtState*>(s)->initialState());
        }
    }
}

void QtAnimationStatePrivate::restoreAnimations()
{
    QtStateMachinePrivate *machine_d = QtStateMachinePrivate::get(machine());

    QHash<RestorableId, QVariant>::const_iterator it;
    for (it=pendingRestorables.constBegin(); it != pendingRestorables.constEnd(); ++it) {
        QtPropertyAnimation *animation = machine_d->registeredRestorableAnimations.value(it.key());
        if (animation == 0)
            continue;

        // ### Check if this works
        // animation->setDirection(QtAbstractAnimation::Backward);

        QtPropertyAnimation *clonedAnimation = new QtPropertyAnimation(animation->targetObject(),
                                                                     animation->propertyName());
        clonedAnimation->setEasingCurve(animation->easingCurve());
        clonedAnimation->setEndValue(it.value());
        
        addAnimation(clonedAnimation, restorationAnimations);
    }

    pendingRestorables.clear();
}


/*!
  Constructs a new QtAnimationState object with the given \a animation and \a
  parent state 
*/
QtAnimationState::QtAnimationState(QtAbstractAnimation *animation, QtState *parent)
    : QtState(*new QtAnimationStatePrivate, parent)
{
    Q_D(QtAnimationState);
    d->init();

    if (animation != 0)
        addAnimation(animation);
}

/*!
  Constructs a new QtAnimationState object with the given \a parent state.
*/
QtAnimationState::QtAnimationState(QtState *parent)
    : QtState(*new QtAnimationStatePrivate, parent)
{
    Q_D(QtAnimationState);
    d->init();
}

/*!
  Destroys this QtAnimationState.
*/
QtAnimationState::~QtAnimationState()
{
}

/*!
  Returns the number of animations added to this QtAnimationState.
*/
int QtAnimationState::animationCount() const
{
    Q_D(const QtAnimationState);
    return d->animations.size();
}

/*!
  Returns the animation associated with this QtAnimationState at index \a i.
*/
QtAbstractAnimation *QtAnimationState::animationAt(int i) const
{
    Q_D(const QtAnimationState);
    return d->animations.at(i);
}

/*!
  Adds \a animation to this QtAnimationState.
*/
void QtAnimationState::addAnimation(QtAbstractAnimation *animation)
{    
    Q_D(QtAnimationState);
    if (animation == 0) {
        qWarning("QtAnimationState::addAnimation: Cannot add null animation");
        return;
    }

    d->addAnimation(animation, d->animations);
}

/*!
    Removes \a animation from this QtAnimationState.
*/
void QtAnimationState::removeAnimation(QtAbstractAnimation *animation)
{
    Q_D(QtAnimationState);
    d->removeAnimation(animation, d->animations);
}

/*!
  \reimp
*/
void QtAnimationState::onEntry()
{
    Q_D(QtAnimationState);

    {
        QtStateMachinePrivate *machine_d = QtStateMachinePrivate::get(d->machine());
        d->pendingRestorables = machine_d->registeredRestorables;
    }

    if (d->initializeAnimationFromTargetStates) {            
        if (QtAbstractTransition *t = d->finishedTransition())
            d->initializeAnimation(t->targetStates());
    }

    if (d->initializeAnimationFromRestorableVariables)
        d->restoreAnimations();    

    for (int i=0; i<d->animations.size(); ++i) 
        d->animations.at(i)->start();

    for (int i=0; i<d->restorationAnimations.size(); ++i)
        d->restorationAnimations.at(i)->start();

    // If there are no animations playing, we use a 0 timer to trigger the transition
    // to the final state
    if (d->animations.size()+d->restorationAnimations.size() == 0) {
        if (d->timer == 0) {
            d->timer = new QTimer(this);
            d->timer->setInterval(0);
            d->timer->setSingleShot(true);

            d->animatingState->addTransition(d->timer, SIGNAL(timeout()), d->finishedState);
        }

        d->timer->start();
    }

}

/*!
  \reimp
*/
void QtAnimationState::onExit()
{
    Q_D(QtAnimationState);

    for (int i=0; i<d->animations.size(); ++i) {
        if (d->animations.at(i)->state() != QtAbstractAnimation::Stopped)
            d->animations.at(i)->stop();    
    }

    QList<QtAbstractAnimation *> restorationAnimations = d->restorationAnimations;
    for (int i=0; i<restorationAnimations.size(); ++i) {
        QtAbstractAnimation *restorationAnimation = restorationAnimations.at(i);
        if (restorationAnimation->state() != QtAbstractAnimation::Stopped) {
            restorationAnimation->stop();
            d->removeAnimation(restorationAnimation, d->restorationAnimations);

            // ###
            delete restorationAnimation; 
        } else {
            QtPropertyAnimation *propertyAnimation = qobject_cast<QtPropertyAnimation*>(restorationAnimation);
            if (propertyAnimation != 0) {
                QtStateMachinePrivate *machine_d = QtStateMachinePrivate::get(d->machine());
                machine_d->unregisterRestorable(propertyAnimation->targetObject(), 
                                                propertyAnimation->propertyName());
            }
        }
    }

    for (int i=0; i<d->resetEndValues.size(); ++i) 
        d->resetEndValues.at(i)->setEndValue(QVariant());

    if (d->timer != 0)
        d->timer->stop();
}

/*!
  \reimp
*/
bool QtAnimationState::event(QEvent *e)
{
    return QtState::event(e);
}

QT_END_NAMESPACE

#endif //QT_NO_ANIMATION
