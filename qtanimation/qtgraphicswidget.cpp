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

/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qtgraphicswidget.h"

#include "qdebug.h"

/*!
    \class QtGraphicsWidget
    \brief The QtGraphicsWidget class provides addition functionality to that of QGraphicsWidget.
    \preliminary

    This class is part of \l{The Animation Framework}. It serves as a base class
    for new Graphics View controls.

	QtGraphicsWidget adds support for some new properties to be used in conjunction with the
	animations classes. In particular it declares the new properties pos, geometry and zValue.
	In addition to that it adds new properties that allow you to animate its own trnsformation.
	So you have now x/y/zRotation, x/yScale and transformOrigin, to gether with their respective
	getters and setters.

    \sa QtPropertyAnimation, {The Animation Framework}
*/


class QtGraphicsWidgetPrivate
{
public:
    QtGraphicsWidgetPrivate() : xRot(0), yRot(0), zRot(0),
        xscale(1), yscale(1)
    {
    }

    void updateTransform(QGraphicsItem *q);

    qreal xRot, yRot, zRot;
    qreal xscale, yscale;
    QPointF transOrigin;
};

void QtGraphicsWidgetPrivate::updateTransform(QGraphicsItem *q)
{
    q->setTransform(QTransform().translate(transOrigin.x(), transOrigin.y())
        .rotate(zRot, Qt::ZAxis)
        .rotate(yRot, Qt::YAxis)
        .rotate(xRot, Qt::XAxis)
        .scale(xscale, yscale)
        .translate(-transOrigin.x(), -transOrigin.y()));
}


/*!
    \property QtGraphicsWidget::pos
    \brief the position of the widget

    This property holds the position of the graphicswidget

    \sa QGraphicsItem::pos(), geometry
*/

/*!
    \property QtGraphicsWidget::geometry
    \brief the geometry of the widget

    This property holds the geometry of the graphicswidget

    \sa QGraphicsWidget::geometry()
*/


/*!
    \property QtGraphicsWidget::zValue
    \brief the stacking order of the item

    This property holds the the stacking order of the item.

    \sa QGraphicsItem::zValue(), QGraphicsItem::setZValue()
*/


/*!
    constructs a QtGraphicsWidget with the parent \a parent and window flags
    to \a wflags.
*/
QtGraphicsWidget::QtGraphicsWidget(QGraphicsItem * parent , Qt::WindowFlags wflags) :
    QGraphicsWidget(parent, wflags), d_ptr(new QtGraphicsWidgetPrivate)
{
}

/*!
    destroys the QtGraphicsWidget
*/
QtGraphicsWidget::~QtGraphicsWidget()
{
}

/*!
    \property QtGraphicsWidget::xRotation
    \brief the rotation of the item around the x-axis.

    This property holds the rotation of the item around the x-axis.

    \sa yRotation, zRotation
*/
qreal QtGraphicsWidget::xRotation() const
{
    Q_D(const QtGraphicsWidget);
    return d->xRot;
}

void QtGraphicsWidget::setXRotation(qreal angle)
{
    Q_D(QtGraphicsWidget);
    d->xRot = angle;
    d->updateTransform(this);
}


/*!
    \property QtGraphicsWidget::yRotation
    \brief the rotation of the item around the y-axis.

    This property holds the rotation of the item around the y-axis.

    \sa xRotation, zRotation
*/
qreal QtGraphicsWidget::yRotation() const
{
    Q_D(const QtGraphicsWidget);
    return d->yRot;
}

void QtGraphicsWidget::setYRotation(qreal angle)
{
    Q_D(QtGraphicsWidget);
    d->yRot = angle;
    d->updateTransform(this);
}


/*!
    \property QtGraphicsWidget::zRotation
    \brief the rotation of the item around the z-axis.

    This property holds the rotation of the item around the z-axis.

    \sa xRotation, yRotation
*/
qreal QtGraphicsWidget::zRotation() const
{
    Q_D(const QtGraphicsWidget);
    return d->zRot;
}

void QtGraphicsWidget::setZRotation(qreal angle)
{
    Q_D(QtGraphicsWidget);
    d->zRot = angle;
    d->updateTransform(this);
}


/*!
    \property QtGraphicsWidget::xScale
    \brief the scaling of the item along the x-axis.

    This property holds the scaling of the item along the x-axis.

    \sa yScale
*/
qreal QtGraphicsWidget::xScale() const
{
    Q_D(const QtGraphicsWidget);
    return d->xscale;
}

void QtGraphicsWidget::setXScale(qreal factor)
{
    Q_D(QtGraphicsWidget);
    d->xscale = factor;
    d->updateTransform(this);
}


/*!
    \property QtGraphicsWidget::yScale
    \brief the scaling of the item along the y-axis.

    This property holds the scaling of the item along the y-axis.

    \sa xScale
*/
qreal QtGraphicsWidget::yScale() const
{
    Q_D(const QtGraphicsWidget);
    return d->yscale;
}

void QtGraphicsWidget::setYScale(qreal factor)
{
    Q_D(QtGraphicsWidget);
    d->yscale = factor;
    d->updateTransform(this);
}

/*!
    \property QtGraphicsWidget::transformOrigin
    \brief the origin of the scale and rotate transformations.

    With this property you can set the point at which the rotate and
    scale operations will use as a origin. This is useful if you for instance
    want to move an item in a circle around a center point.

    \sa zRotation, xScale, yScale
*/
QPointF QtGraphicsWidget::transformOrigin() const
{
    Q_D(const QtGraphicsWidget);
    return d->transOrigin;
}

void QtGraphicsWidget::setTransformOrigin(const QPointF &origin)
{
    Q_D(QtGraphicsWidget);
    d->transOrigin = origin;
    d->updateTransform(this);
}
