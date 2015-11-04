/*

    This file is part of the Automon Project (OBD Diagnostics) - http://www.automon.io/
    Source Repository: https://github.com/donaloconnor/automon/
    
    Copyright (c) 2015, Donal O'Connor <donaloconnor@gmail.com>

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef ACCELERATIONTESTWIDGET_H
#define ACCELERATIONTESTWIDGET_H

#include <QWidget>

#include "automon.h"

class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QLCDNumber;
class QPushButton;
class QSpinBox;
class QTime;

using namespace AutomonKernel;

class AccelerationTestWidget : public QWidget
{
    Q_OBJECT

public:
    AccelerationTestWidget(Automon * kernel, QWidget * parent = 0);
    ~AccelerationTestWidget();

public slots:
    void startTest();
    void display(double speed);

signals:
    void changeStatus(const QString & status);

private:
    void setUpSensor();
    void removeSensor();
    QLabel * m_header;
    QHBoxLayout * m_mainLayout;
    QVBoxLayout * m_verticalLayout;
    QLCDNumber * m_speed;
    QLCDNumber * m_accelerationTimeDisplay;
    Automon * m_kernel;
    QPushButton * m_startTest;
    QSpinBox * m_destinationSpeed;
    QTime * m_time;
    bool m_carMoving;
    int m_accelerationTime;
    bool m_testFinished;
    int m_parkThreshold;
    bool m_testStarted;
    bool m_startButtonClicked;
};

#endif // ACCELERATIONTESTWIDGET_H
