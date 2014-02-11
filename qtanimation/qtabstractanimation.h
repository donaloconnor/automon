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



#ifndef QABSTRACTANIMATION_H
#define QABSTRACTANIMATION_H

#include <QtCore/qobject.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

#ifndef QT_NO_ANIMATION

class QtAnimationGroup;
class QtSequentialAnimationGroup;

class QtAbstractAnimationPrivate;
class Q_ANIMATION_EXPORT QtAbstractAnimation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(int iterationCount READ iterationCount WRITE setIterationCount)
    Q_PROPERTY(int currentTime READ currentTime WRITE setCurrentTime)
    Q_PROPERTY(int currentIteration READ currentIteration NOTIFY currentIterationChanged)
    Q_PROPERTY(Direction direction READ direction WRITE setDirection NOTIFY directionChanged)
    Q_PROPERTY(int duration READ duration)

public:
    enum Direction {
        Forward,
        Backward
    };

    enum State {
        Stopped,
        Paused,
        Running
    };

    enum DeletionPolicy {
        KeepWhenStopped = 0,
        DeleteWhenStopped
    };

    QtAbstractAnimation(QObject *parent = 0);
    virtual ~QtAbstractAnimation();

    State state() const;

    QtAnimationGroup *group() const;

    Direction direction() const;
    void setDirection(Direction direction);

    int iterationCount() const;
    void setIterationCount(int iterationCount);
    int currentIteration() const;

    virtual int duration() const = 0;
    int totalDuration() const;

    int currentTime() const;

Q_SIGNALS:
    void finished();
    void stateChanged(QtAbstractAnimation::State oldState, QtAbstractAnimation::State newState);
    void currentIterationChanged(int currentIteration);
    void directionChanged(QtAbstractAnimation::Direction);

public Q_SLOTS:
    void start(QtAbstractAnimation::DeletionPolicy policy = KeepWhenStopped);
    void pause();
    void resume();
    void stop();
    void setCurrentTime(int msecs);

protected:
    QtAbstractAnimation(QtAbstractAnimationPrivate &dd, QObject *parent = 0);
    bool event(QEvent *event);

    virtual void updateCurrentTime(int msecs) = 0;
    virtual void updateState(QtAbstractAnimation::State oldState, QtAbstractAnimation::State newState);
    virtual void updateDirection(QtAbstractAnimation::Direction direction);

#ifdef QT_EXPERIMENTAL_SOLUTION
     QtAbstractAnimationPrivate *d_ptr;
#endif

private:
    Q_DISABLE_COPY(QtAbstractAnimation)
    Q_DECLARE_PRIVATE(QtAbstractAnimation)
};

#endif //QT_NO_ANIMATION

QT_END_NAMESPACE

QT_END_HEADER

#endif // QABSTRACTANIMATION_H
