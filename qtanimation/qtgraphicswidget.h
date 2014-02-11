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
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QTGRAPHICSWIDGET_H
#define QTGRAPHICSWIDGET_H

#include <QtGui/qgraphicswidget.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QtGraphicsWidgetPrivate;
class Q_ANIMATION_EXPORT QtGraphicsWidget : public QGraphicsWidget
{
    Q_OBJECT

    //these properties are defined starting with 4.5
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    Q_PROPERTY(QRectF geometry READ geometry WRITE setGeometry)

    //these properties are needed for the animation solution
    //and should be considered for inclusion in Qt
    Q_PROPERTY(qreal xRotation READ xRotation WRITE setXRotation)
    Q_PROPERTY(qreal yRotation READ yRotation WRITE setYRotation)
    Q_PROPERTY(qreal zRotation READ zRotation WRITE setZRotation)
    Q_PROPERTY(qreal xScale READ xScale WRITE setXScale)
    Q_PROPERTY(qreal yScale READ yScale WRITE setYScale)
    Q_PROPERTY(QPointF transformOrigin READ transformOrigin WRITE setTransformOrigin)
    Q_PROPERTY(qreal zValue READ zValue WRITE setZValue)

public:
    QtGraphicsWidget( QGraphicsItem * parent = 0, Qt::WindowFlags wFlags = 0);
    ~QtGraphicsWidget();

    qreal xRotation() const;
    void setXRotation(qreal angle);

    qreal yRotation() const;
    void setYRotation(qreal angle);

    qreal zRotation() const;
    void setZRotation(qreal angle);

    qreal xScale() const;
    void setXScale(qreal factor);

    qreal yScale() const;
    void setYScale(qreal factor);

    QPointF transformOrigin() const;
    void setTransformOrigin(const QPointF &origin);

private:
    QtGraphicsWidgetPrivate *d_ptr;
    Q_DISABLE_COPY(QtGraphicsWidget)
    Q_DECLARE_PRIVATE(QtGraphicsWidget)
};
QT_END_NAMESPACE

QT_END_HEADER

#endif // QABSTRACTANIMATION_H

