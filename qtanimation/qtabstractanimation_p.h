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



#ifndef QABSTRACTANIMATION_P_H
#define QABSTRACTANIMATION_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of QIODevice. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qbasictimer.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qtimer.h>
#ifndef QT_EXPERIMENTAL_SOLUTION
#include <private/qobject_p.h>
#endif

QT_BEGIN_NAMESPACE

class QtAnimationGroup;
class QtAbstractAnimation;
#ifdef QT_EXPERIMENTAL_SOLUTION
class QtAbstractAnimationPrivate
#else
class QtAbstractAnimationPrivate : public QObjectPrivate
#endif
{
public:
    QtAbstractAnimationPrivate()
        : state(QtAbstractAnimation::Stopped),
          direction(QtAbstractAnimation::Forward),
          deleteWhenStopped(false),
          totalCurrentTime(0),
          currentTime(0),
          iterationCount(1),
          currentIteration(0),
          group(0)
    {
    }

    virtual ~QtAbstractAnimationPrivate() {}

    static QtAbstractAnimationPrivate *get(QtAbstractAnimation *q)
    {
        return q->d_func();
    }

    QtAbstractAnimation::State state;
    QtAbstractAnimation::Direction direction;
    bool deleteWhenStopped;
    void setState(QtAbstractAnimation::State state);

    int totalCurrentTime;
    int currentTime;
    int iterationCount;
    int currentIteration;

    QtAnimationGroup *group;
#ifdef QT_EXPERIMENTAL_SOLUTION
    QtAbstractAnimation *q_ptr;
#endif

private:
    Q_DECLARE_PUBLIC(QtAbstractAnimation)
};


class QUnifiedTimer : public QObject
{
private:
    QUnifiedTimer();

public:
    static QUnifiedTimer *instance();

    void timerEvent(QTimerEvent *);
    void updateTimer();
    void registerAnimation(QtAbstractAnimation *animation);
    void unregisterAnimation(QtAbstractAnimation *animation);


private:
    void updateRecentlyStartedAnimations();

    QBasicTimer animationTimer, startAnimationTimer;
    QTime time;
    int lastTick;
    QList<QtAbstractAnimation*> animations, animationsToStart;
};

QT_END_NAMESPACE
#endif
