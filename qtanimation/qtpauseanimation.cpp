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
    \class QtPauseAnimation
    \brief The QtPauseAnimation class provides a pause for QtSequentialAnimationGroup.
    
    
    \preliminary
*/

#ifndef QT_NO_ANIMATION

#include "qtpauseanimation.h"
#include "qtabstractanimation_p.h"


QT_BEGIN_NAMESPACE

class QtPauseAnimationPrivate : public QtAbstractAnimationPrivate
{
public:
    QtPauseAnimationPrivate() : QtAbstractAnimationPrivate(), duration(0)
    {
    }

    int duration;
};

/*!
    Constructs a QtPauseAnimation.
    \a parent is passed to QObject's constructor.
    The default duration is 0.
*/

QtPauseAnimation::QtPauseAnimation(QObject *parent) : QtAbstractAnimation(*new QtPauseAnimationPrivate, parent)
{
}

/*!
    Constructs a QtPauseAnimation.
    \a msecs is the duration of the pause.
    \a parent is passed to QObject's constructor.
*/

QtPauseAnimation::QtPauseAnimation(int msecs, QObject *parent) : QtAbstractAnimation(*new QtPauseAnimationPrivate, parent)
{
    setDuration(msecs);
}

/*!
    Destroys the pause animation.
*/
QtPauseAnimation::~QtPauseAnimation()
{
}

/*!
    \property QtPauseAnimation::duration
    \brief the duration of the pause.

    The duration of the pause. The duration should not be negative.
*/
int QtPauseAnimation::duration() const
{
    Q_D(const QtPauseAnimation);
    return d->duration;
}

void QtPauseAnimation::setDuration(int msecs)
{
    if (msecs < 0) {
        qWarning("QtPauseAnimation::setDuration: cannot set a negative duration");
        return;
    }
    Q_D(QtPauseAnimation);
    d->duration = msecs;
}

/*!
    \reimp
 */
bool QtPauseAnimation::event(QEvent *e)
{
    return QtAbstractAnimation::event(e);
}

/*!
    \reimp
 */
void QtPauseAnimation::updateCurrentTime(int msecs)
{
    Q_UNUSED(msecs);
}


QT_END_NAMESPACE

#include "moc_qtpauseanimation.cpp"

#endif //QT_NO_ANIMATION
