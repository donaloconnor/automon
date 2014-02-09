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



#ifndef QSTATEACTION_H
#define QSTATEACTION_H

#include <QtCore/qobject.h>

#include <QtCore/qvariant.h>
#include <QtCore/qlist.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QtStateActionPrivate;
class Q_ANIMATION_EXPORT QtStateAction : public QObject
{
    Q_OBJECT
public:
    ~QtStateAction();

protected:
    QtStateAction(QObject *parent = 0);

    virtual void execute() = 0;

    bool event(QEvent *e);

protected:
#ifdef QT_STATEMACHINE_SOLUTION
    QtStateActionPrivate *d_ptr;
#endif
    QtStateAction(QtStateActionPrivate &dd, QObject *parent);

private:
    Q_DISABLE_COPY(QtStateAction)
    Q_DECLARE_PRIVATE(QtStateAction)
};

class QtStateSetPropertyActionPrivate;
class Q_ANIMATION_EXPORT QtStateSetPropertyAction : public QtStateAction
{
    Q_OBJECT
    Q_PROPERTY(QObject* target READ targetObject WRITE setTargetObject)
    Q_PROPERTY(QByteArray propertyName READ propertyName WRITE setPropertyName)
    Q_PROPERTY(QVariant value READ value WRITE setValue)
public:
    QtStateSetPropertyAction(QObject *target, const QByteArray &propertyName,
                            const QVariant &value, QObject *parent = 0);
    QtStateSetPropertyAction(QObject *parent = 0);
    ~QtStateSetPropertyAction();

    QObject *targetObject() const;
    void setTargetObject(QObject *target);

    QByteArray propertyName() const;
    void setPropertyName(const QByteArray &name);

    QVariant value() const;
    void setValue(const QVariant &value);

protected:
    void execute();

    bool event(QEvent *e);

private:
    Q_DISABLE_COPY(QtStateSetPropertyAction)
    Q_DECLARE_PRIVATE(QtStateSetPropertyAction)
};

class QtStateInvokeMethodActionPrivate;
class Q_ANIMATION_EXPORT QtStateInvokeMethodAction : public QtStateAction
{
    Q_OBJECT
    Q_PROPERTY(QObject* target READ targetObject WRITE setTargetObject)
    Q_PROPERTY(QByteArray methodName READ methodName WRITE setMethodName)
    Q_PROPERTY(QVariantList arguments READ arguments WRITE setArguments)
public:
    QtStateInvokeMethodAction(QObject *target, const QByteArray &methodName,
                             QObject *parent = 0);
    QtStateInvokeMethodAction(QObject *target, const QByteArray &methodName,
                             const QList<QVariant> &args, QObject *parent = 0);
    QtStateInvokeMethodAction(QObject *parent = 0);
    ~QtStateInvokeMethodAction();

    QObject *targetObject() const;
    void setTargetObject(QObject *target);

    QByteArray methodName() const;
    void setMethodName(const QByteArray &methodName);

    QVariantList arguments() const;
    void setArguments(const QVariantList &arguments);

protected:
    void execute();

    bool event(QEvent *e);

private:
    Q_DISABLE_COPY(QtStateInvokeMethodAction)
    Q_DECLARE_PRIVATE(QtStateInvokeMethodAction)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
