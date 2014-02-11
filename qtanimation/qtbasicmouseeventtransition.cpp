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



#include "qtbasicmouseeventtransition_p.h"
#include <QtGui/qevent.h>
#include <QtGui/qpainterpath.h>
#include <qdebug.h>

#if defined(QT_EXPERIMENTAL_SOLUTION)
# include "qtabstracttransition_p.h"
#else
# include <private/qabstracttransition_p.h>
#endif

QT_BEGIN_NAMESPACE

/*!
  \internal
  \class QBasicMouseEventTransition

  \brief The QBasicMouseEventTransition class provides a transition for Qt mouse events.
*/

class QBasicMouseEventTransitionPrivate : public QtAbstractTransitionPrivate
{
    Q_DECLARE_PUBLIC(QBasicMouseEventTransition)
public:
    QBasicMouseEventTransitionPrivate();

    static QBasicMouseEventTransitionPrivate *get(QBasicMouseEventTransition *q);

    Qt::MouseButton button;
    QPainterPath path;
};

QBasicMouseEventTransitionPrivate::QBasicMouseEventTransitionPrivate()
{
}

QBasicMouseEventTransitionPrivate *QBasicMouseEventTransitionPrivate::get(QBasicMouseEventTransition *q)
{
    return q->d_func();
}

/*!
  Constructs a new mouse event transition for events of the given \a type.
*/
QBasicMouseEventTransition::QBasicMouseEventTransition(QEvent::Type type, Qt::MouseButton button)
    : QtAbstractTransition(*new QBasicMouseEventTransitionPrivate, type)
{
    Q_D(QBasicMouseEventTransition);
    d->button = button;
}

/*!
  Destroys this mouse event transition.
*/
QBasicMouseEventTransition::~QBasicMouseEventTransition()
{
}

/*!
  Returns the path for this mouse event transition.
*/
QPainterPath QBasicMouseEventTransition::path() const
{
    Q_D(const QBasicMouseEventTransition);
    return d->path;
}

/*!
  Sets the path for this mouse event transition.
*/
void QBasicMouseEventTransition::setPath(const QPainterPath &path)
{
    Q_D(QBasicMouseEventTransition);
    d->path = path;
}

/*!
  \reimp
*/
bool QBasicMouseEventTransition::testCondition(QEvent *event) const
{
    Q_D(const QBasicMouseEventTransition);
    QMouseEvent *me = static_cast<QMouseEvent*>(event);
    return (me->button() == d->button)
        && (d->path.isEmpty() || d->path.contains(me->pos()));
}

/*!
  \reimp
*/
void QBasicMouseEventTransition::onTransition()
{
}

QT_END_NAMESPACE
