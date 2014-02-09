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

#include "qtvariantanimation.h"
#include "qtvariantanimation_p.h"

#include <QtCore/QRect>
#include <QtCore/QLineF>
#include <QtCore/QLine>
#include <QtCore/QReadWriteLock>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    \class QtVariantAnimation
    
    \brief The QtVariantAnimation class provides an abstract base class for animations.
    
    \preliminary

    This class is part of {The Animation Framework}. It serves as a base class
    for property and item animations, with functions for shared functionality.

    If you want to create an animation, you should look at QtPropertyAnimation instead.

    You can then set start and end values for the property by calling
    setStartValue() and setEndValue(), and finally call start() to
    start the animation. When control goes back to the event loop, QtVariantAnimation
    will interpolate the property of the target object and emit the valueChanged
    signal. To react to a change in the current value you have to reimplement the
    updateCurrentValue virtual method.

    There are two ways to affect how QtVariantAnimation interpolates the values.
    You can set an easing curve by calling setEasingCurve(), and configure the
    duration by calling setDuration(). You can change how the QVariants are
    interpolated by creating a subclass of QtVariantAnimation, and reimplementing the
    virtual interpolated() function.


    \sa QtPropertyAnimation, {The Animation Framework}
*/

/*!
    \fn void QtVariantAnimation::valueChanged(const QVariant &value)

    QtVariantAnimation emits this signal whenever the current \a value changes.

    \sa currentValue, startValue, endValue
*/

static bool animationValueLessThan(const QtVariantAnimation::KeyValue &p1, const QtVariantAnimation::KeyValue &p2)
{
    return p1.first < p2.first;
}

template<> Q_INLINE_TEMPLATE QRect _q_interpolate(const QRect &f, const QRect &t, qreal progress)
{
    QRect ret;
    ret.setCoords(_q_interpolate(f.left(), t.left(), progress),
                  _q_interpolate(f.top(), t.top(), progress),
                  _q_interpolate(f.right(), t.right(), progress),
                  _q_interpolate(f.bottom(), t.bottom(), progress));
    return ret;
}

template<> Q_INLINE_TEMPLATE QRectF _q_interpolate(const QRectF &f, const QRectF &t, qreal progress)
{
    qreal x1, y1, w1, h1;
    f.getRect(&x1, &y1, &w1, &h1);
    qreal x2, y2, w2, h2;
    t.getRect(&x2, &y2, &w2, &h2);
    return QRectF( _q_interpolate(x1, x2, progress), _q_interpolate(y1, y2, progress),
                  _q_interpolate(w1, w2, progress), _q_interpolate(h1, h2, progress));
}

template<> Q_INLINE_TEMPLATE QLine _q_interpolate(const QLine &f, const QLine &t, qreal progress)
{
    return QLine( _q_interpolate(f.p1(), t.p1(), progress), _q_interpolate(f.p2(), t.p2(), progress));
}

template<> Q_INLINE_TEMPLATE QLineF _q_interpolate(const QLineF &f, const QLineF &t, qreal progress)
{
    return QLineF( _q_interpolate(f.p1(), t.p1(), progress), _q_interpolate(f.p2(), t.p2(), progress));
}

void QtVariantAnimationPrivate::convertValues(int t)
{
    //this ensures that all the keyValues are of type t
    for (int i = 0; i < keyValues.count(); ++i) {
        QtVariantAnimation::KeyValue &pair = keyValues[i];
        if (pair.second.userType() != t)
            pair.second.convert(static_cast<QVariant::Type>(t));
    }
    currentInterval.start.first = 2; // this will force the refresh
    interpolator = 0;               // if the type changed we need to update the interpolator
}

/*!
    \fn void QtVariantAnimation::updateCurrentValue(const QVariant &value) = 0;
    This pure virtual function is called when the animated value is changed.
    \a value is the new value.
*/

void QtVariantAnimationPrivate::updateCurrentValue()
{
    Q_Q(QtVariantAnimation);
    const qreal progress = easing.valueForProgress(((duration == 0) ? qreal(1) : qreal(currentTime) / qreal(duration)));

    if (progress < currentInterval.start.first || progress > currentInterval.end.first) {
        //let's update currentInterval
        QtVariantAnimation::KeyValues::const_iterator itStart = qLowerBound(keyValues.constBegin(), keyValues.constEnd(), qMakePair(progress, QVariant()), animationValueLessThan);
        QtVariantAnimation::KeyValues::const_iterator itEnd = itStart;

        // If we are at the end we should continue to use the last keyValues in case of extrapolation (progress > 1.0).
        // This is because the easing function can return a value slightly outside the range [0, 1]
        if (itStart != keyValues.constEnd()) {

            //this can't happen because we always prepend the default start value there
            if (itStart == keyValues.begin()) {
                ++itEnd;
                if (itEnd == keyValues.constEnd())
                    return; //there is no upper bound
            } else {
                --itStart;
            }

            //update all the values of the currentInterval
            currentInterval.start = *itStart;
            currentInterval.end = *itEnd;
        }
    }

    const qreal startProgress = currentInterval.start.first,
                endProgress = currentInterval.end.first;
    const qreal localProgress = (progress - startProgress) / (endProgress - startProgress);

    bool changed = false;
    {
      //we do that here in a limited scope so that ret is dereferenced and frees memory
      //and the call to updateCurrentValue can recreate QVariant of the same type (for ex. in
      //QGraphicsItem::setPos
      QVariant ret = q->interpolated(currentInterval.start.second,
                                    currentInterval.end.second,
                                    localProgress);
      if (currentValue != ret) {
          changed = true;
          qSwap(currentValue, ret);
      }
    }

    if (changed) {
        //the value has changed
        q->updateCurrentValue(currentValue);
#ifndef QT_EXPERIMENTAL_SOLUTION
        if (connectedSignals & changedSignalMask)
#endif
            emit q->valueChanged(currentValue);
    }
}


QVariant QtVariantAnimationPrivate::valueAt(qreal step) const
{
    QtVariantAnimation::KeyValues::const_iterator result =
        qBinaryFind(keyValues.begin(), keyValues.end(), qMakePair(step, QVariant()), animationValueLessThan);
    if (result != keyValues.constEnd())
        return result->second;

    return QVariant();
}


void QtVariantAnimationPrivate::setValueAt(qreal step, const QVariant &value)
{
    if (step < qreal(0.0) || step > qreal(1.0)) {
        qWarning("QtVariantAnimation::setValueAt: invalid step = %f", step);
        return;
    }

    QtVariantAnimation::KeyValue pair(step, value);

    QtVariantAnimation::KeyValues::iterator result = qLowerBound(keyValues.begin(), keyValues.end(), pair, animationValueLessThan);
    if (result == keyValues.end() || result->first != step) {
        keyValues.insert(result, pair);
    } else {
        if (value.isValid())
            result->second = value; //remove the previous value
        else if (step == 0 && !hasStartValue && defaultStartValue.isValid())
            result->second = defaultStartValue; //we reset to the default start value
        else
            keyValues.erase(result); //replace the previous value
    }

    currentInterval.start.first = 2; // this will force the refresh
    updateCurrentValue();
}

void QtVariantAnimationPrivate::setDefaultStartValue(const QVariant &value)
{
    defaultStartValue = value;
    if (!hasStartValue)
      setValueAt(0, value);
}

/*!
    Construct a QtVariantAnimation object. \a parent is passed to QtAbstractAnimation's
    constructor.
*/
QtVariantAnimation::QtVariantAnimation(QObject *parent) : QtAbstractAnimation(*new QtVariantAnimationPrivate, parent)
{
   d_func()->init();
}

/*!
    \internal
*/
QtVariantAnimation::QtVariantAnimation(QtVariantAnimationPrivate &dd, QObject *parent) : QtAbstractAnimation(dd, parent)
{
   d_func()->init();
}

/*!
    Destroys the animation.
*/
QtVariantAnimation::~QtVariantAnimation()
{
}

/*!
    \property QtVariantAnimation::easingCurve
    \brief the easing curve of the animation

    This property defines the easing curve of the animation. By default, a
    linear easing curve is used, resulting in linear interpolation of the
    end property. For many animations, it's useful to try different easing
    curves, including QtEasingCurve::InCirc, which provides a circular entry curve,
    and QtEasingCurve::InOutElastic, which provides an elastic effect on the values
    of the interpolated property.

    The easing curve is used with the interpolator, the interpolated() virtual
    function, the animation's duration, and iterationCount, to control how the
    current value changes as the animation progresses.
*/
QtEasingCurve QtVariantAnimation::easingCurve() const
{
    Q_D(const QtVariantAnimation);
    return d->easing;
}

void QtVariantAnimation::setEasingCurve(const QtEasingCurve &easing)
{
    Q_D(QtVariantAnimation);
    d->easing = easing;
    d->updateCurrentValue();
}

Q_GLOBAL_STATIC(QVector<QtVariantAnimation::Interpolator>, registeredInterpolators)
Q_GLOBAL_STATIC(QReadWriteLock, registeredInterpolatorsLock)

/*!
    \fn void qRegisterAnimationInterpolator(QVariant (*func)(const T &from, const T &to, qreal progress))
    \relates QtVariantAnimation
    \threadsafe

    Registers a custom interpolator \a func for the template type \c{T}.
    The interpolator has to be registered before the animation is constructed.
    To unregister (and use the default interpolator) set \a func to 0.
 */

/*!
    \internal
    \typedef QtVariantAnimation::Interpolator

    This is a typedef for a pointer to a function with the following
    signature:
    \code
    QVariant myInterpolator(const QVariant &from, const QVariant &to, qreal progress);
    \endcode

*/

/*! \internal
 * Registers a custom interpolator \a func for the specific \a interpolationType.
 * The interpolator has to be registered before the animation is constructed.
 * To unregister (and use the default interpolator) set \a func to 0.
 */
void QtVariantAnimation::registerInterpolator(QtVariantAnimation::Interpolator func, int interpolationType)
{
    // will override any existing interpolators
    QWriteLocker locker(registeredInterpolatorsLock());
    if (int(interpolationType) >= registeredInterpolators()->count())
        registeredInterpolators()->resize(int(interpolationType) + 1);
    registeredInterpolators()->replace(interpolationType, func);
}


template<typename T> static inline QtVariantAnimation::Interpolator castToInterpolator(QVariant (*func)(const T &from, const T &to, qreal progress))
{
     return reinterpret_cast<QtVariantAnimation::Interpolator>(func);
}

static QtVariantAnimation::Interpolator getInterpolator(int interpolationType)
{
    QReadLocker locker(registeredInterpolatorsLock());
    QtVariantAnimation::Interpolator ret = 0;
    if (interpolationType < registeredInterpolators()->count()) {
        ret = registeredInterpolators()->at(interpolationType);
        if (ret) return ret;
    }

    switch(interpolationType)
    {
    case QMetaType::Int:
        return castToInterpolator(_q_interpolateVariant<int>);
    case QMetaType::Double:
        return castToInterpolator(_q_interpolateVariant<double>);
    case QMetaType::Float:
        return castToInterpolator(_q_interpolateVariant<float>);
    case QMetaType::QLine:
        return castToInterpolator(_q_interpolateVariant<QLine>);
    case QMetaType::QLineF:
        return castToInterpolator(_q_interpolateVariant<QLineF>);
    case QMetaType::QPoint:
        return castToInterpolator(_q_interpolateVariant<QPoint>);
    case QMetaType::QPointF:
        return castToInterpolator(_q_interpolateVariant<QPointF>);
    case QMetaType::QSize:
        return castToInterpolator(_q_interpolateVariant<QSize>);
    case QMetaType::QSizeF:
        return castToInterpolator(_q_interpolateVariant<QSizeF>);
    case QMetaType::QRect:
        return castToInterpolator(_q_interpolateVariant<QRect>);
    case QMetaType::QRectF:
        return castToInterpolator(_q_interpolateVariant<QRectF>);
    default:
        return 0; //this type is not handled
    }
}

/*!
    \property QtVariantAnimation::duration
    \brief the duration of the animation

    This property describes the duration of the animation. The default
    duration is 250 milliseconds.

    \sa QtAbstractAnimation::duration()
 */
int QtVariantAnimation::duration() const
{
    Q_D(const QtVariantAnimation);
    return d->duration;
}

void QtVariantAnimation::setDuration(int msecs)
{
    Q_D(QtVariantAnimation);
    if (msecs < 0) {
        qWarning("QtVariantAnimation::setDuration: cannot set a negative duration");
        return;
    }
    if (d->duration == msecs)
        return;
    d->duration = msecs;
    d->updateCurrentValue();
}

/*!
    \property QtVariantAnimation::startValue
    \brief the optional start value of the animation

    This property describes the optional start value of the animation. If
    omitted, or if a null QVariant is assigned as the start value, the
    animation will use the current position of the end when the animation
    is started.

    \sa endValue
*/
QVariant QtVariantAnimation::startValue() const
{
    return keyValueAt(0);
}

void QtVariantAnimation::setStartValue(const QVariant &value)
{
    setKeyValueAt(0, value);
}

/*!
    \property QtVariantAnimation::endValue
    \brief the end value of the animation

    This property describes the end value of the animation.

    \sa startValue
 */
QVariant QtVariantAnimation::endValue() const
{
    return keyValueAt(1);
}

void QtVariantAnimation::setEndValue(const QVariant &value)
{
    setKeyValueAt(1, value);
}


/*!
    Returns the key frame value for the given \a step. The given \a step
    must be in the range 0 to 1. If there is no KeyValue for \a step,
    it returns an invalid QVariant.

    \sa keyValues(), setKeyValueAt()
*/
QVariant QtVariantAnimation::keyValueAt(qreal step) const
{
    Q_D(const QtVariantAnimation);
    if (step == 0 && !d->hasStartValue)
        return QVariant(); //special case where we don't have an explicit startValue

    return d->valueAt(step);
}

/*!
    \typedef QtVariantAnimation::KeyValue

    This is a typedef for QPair<qreal, QVariant>.
*/
/*!
    \typedef QtVariantAnimation::KeyValues

    This is a typedef for QVector<KeyValue>
*/

/*!
    Creates a key frame at the given \a step with the given \a value.
    The given \a step must be in the range 0 to 1.

    \sa setKeyValues(), keyValueAt()
*/
void QtVariantAnimation::setKeyValueAt(qreal step, const QVariant &value)
{
    Q_D(QtVariantAnimation);
    if (step == 0)
        d->hasStartValue = value.isValid();
    d->setValueAt(step, value);
}

/*!
    Returns the key frames of this animation.

    \sa keyValueAt(), setKeyValues()
*/
QtVariantAnimation::KeyValues QtVariantAnimation::keyValues() const
{
    Q_D(const QtVariantAnimation);
    QtVariantAnimation::KeyValues ret = d->keyValues;
    //in case we added the default start value, we remove it
    if (!d->hasStartValue && !ret.isEmpty() && ret.at(0).first == 0)
        ret.remove(0);
    return ret;
}

/*!
    Replaces the current set of key frames with the given \a keyValues.
    the step of the key frames must be in the range 0 to 1.

    \sa keyValues(), keyValueAt()
*/
void QtVariantAnimation::setKeyValues(const KeyValues &keyValues)
{
    Q_D(QtVariantAnimation);
    d->keyValues = keyValues;
    qSort(d->keyValues.begin(), d->keyValues.end(), animationValueLessThan);
    d->currentInterval.start.first = 2; // this will force the refresh
    d->hasStartValue = !d->keyValues.isEmpty() && d->keyValues.at(0).first == 0;
}

/*!
    \property QtVariantAnimation::currentValue
    \brief the current value of the animation

    This property describes the current value; an interpolation between the
    start value and the end value, using the current time for progress.

    QtVariantAnimation calls the virtual updateCurrentValue() function when the
    current value changes. This is particularily useful for subclasses that
    need to track updates.

    \sa startValue, endValue
 */
QVariant QtVariantAnimation::currentValue() const
{
    Q_D(const QtVariantAnimation);
    if (!d->currentValue.isValid())
        const_cast<QtVariantAnimationPrivate*>(d)->updateCurrentValue();
    return d->currentValue;
}

/*!
    \reimp
 */
bool QtVariantAnimation::event(QEvent *event)
{
    return QtAbstractAnimation::event(event);
}

/*!
    \reimp
*/
void QtVariantAnimation::updateState(QtAbstractAnimation::State oldState,
                             QtAbstractAnimation::State newState)
{
    Q_UNUSED(oldState);
    Q_UNUSED(newState);
    Q_D(QtVariantAnimation);
    d->currentValue = QVariant(); // this will force the refresh
}

/*!
    This virtual function returns the linear interpolation between variants \a
    from and \a to, at \a progress, usually a value between 0 and 1. You can reimplement
    this function in a subclass of QtVariantAnimation to provide your own interpolation
    algorithm. Note that in order for the interpolation to work with a QtEasingCurve
    that return a value smaller than 0 or larger than 1 (such as QtEasingCurve::InBack)
    you should make sure that it can extrapolate. If the semantic of the datatype
    does not allow extrapolation this function should handle that gracefully.

    \sa QtEasingCurve
 */
QVariant QtVariantAnimation::interpolated(const QVariant &from, const QVariant &to, qreal progress) const
{
    Q_D(const QtVariantAnimation);
    if (d->interpolator == 0) {
        if (from.userType() == to.userType())
            d->interpolator = getInterpolator(from.userType());
        if (d->interpolator == 0) //no interpolator found
            return QVariant();
    }

    return d->interpolator(from.constData(), to.constData(), progress);
}

/*!
    \reimp
 */
void QtVariantAnimation::updateCurrentTime(int msecs)
{
    Q_D(QtVariantAnimation);
    Q_UNUSED(msecs);
    d->updateCurrentValue();
}

QT_END_NAMESPACE

#include "moc_qtvariantanimation.cpp"

#endif //QT_NO_ANIMATION
