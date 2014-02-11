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



#ifndef QANIMATION_H
#define QANIMATION_H

#if defined(QT_EXPERIMENTAL_SOLUTION)
# include "qtabstractanimation.h"
# include "qteasingcurve.h"
#else
# include <QtCore/qeasingcurve.h>
# include <QtCore/qabstractanimation.h>
#endif
#include <QtCore/qlist.h>
#include <QtCore/qpoint.h>
#include <QtCore/qvariant.h>
#include <QtCore/qpair.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

#ifndef QT_NO_ANIMATION

class QtVariantAnimationPrivate;
class Q_ANIMATION_EXPORT QtVariantAnimation : public QtAbstractAnimation
{
    Q_OBJECT
    Q_PROPERTY(QVariant startValue READ startValue WRITE setStartValue)
    Q_PROPERTY(QVariant endValue READ endValue WRITE setEndValue)
    Q_PROPERTY(QVariant currentValue READ currentValue NOTIFY currentValueChanged)
    Q_PROPERTY(int duration READ duration WRITE setDuration)
    Q_PROPERTY(QtEasingCurve easingCurve READ easingCurve WRITE setEasingCurve)

public:
    typedef QPair<qreal, QVariant> KeyValue;
    typedef QVector<KeyValue> KeyValues;

    QtVariantAnimation(QObject *parent = 0);
    ~QtVariantAnimation();

    QVariant startValue() const;
    void setStartValue(const QVariant &value);

    QVariant endValue() const;
    void setEndValue(const QVariant &value);

    QVariant keyValueAt(qreal step) const;
    void setKeyValueAt(qreal step, const QVariant &value);

    KeyValues keyValues() const;
    void setKeyValues(const KeyValues &values);

    QVariant currentValue() const;

    int duration() const;
    void setDuration(int msecs);

    QtEasingCurve easingCurve() const;
    void setEasingCurve(const QtEasingCurve &easing);

    typedef QVariant (*Interpolator)(const void *from, const void *to, qreal progress);

Q_SIGNALS:
    void valueChanged(const QVariant &value);

protected:
    QtVariantAnimation(QtVariantAnimationPrivate &dd, QObject *parent = 0);
    bool event(QEvent *event);

    void updateCurrentTime(int msecs);
    void updateState(QtAbstractAnimation::State oldState, QtAbstractAnimation::State newState);

    virtual void updateCurrentValue(const QVariant &value) = 0;
    virtual QVariant interpolated(const QVariant &from, const QVariant &to, qreal progress) const;

private:
    template <typename T> friend void qRegisterAnimationInterpolator(QVariant (*func)(const T &, const T &, qreal));
    static void registerInterpolator(Interpolator func, int interpolationType);

    Q_DISABLE_COPY(QtVariantAnimation)
    Q_DECLARE_PRIVATE(QtVariantAnimation)
};

template <typename T>
static void qRegisterAnimationInterpolator(QVariant (*func)(const T &from, const T &to, qreal progress)) {
    QtVariantAnimation::registerInterpolator(reinterpret_cast<QtVariantAnimation::Interpolator>(func), qMetaTypeId<T>());
}

#endif //QT_NO_ANIMATION

QT_END_NAMESPACE

QT_END_HEADER

#endif //QANIMATION_H
