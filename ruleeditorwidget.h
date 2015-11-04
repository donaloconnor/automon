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

#ifndef RULEEDITORWIDGET_H
#define RULEEDITORWIDGET_H

#include <QWidget>
#include "automon.h"

class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QComboBox;
class QSlider;
class QLCDNumber;
class QPushButton;
class QTableWidget;

using namespace AutomonKernel;

class RuleEditorWidget : public QWidget
{
    Q_OBJECT

public:
    RuleEditorWidget(Automon * kernel, QWidget * parent = 0);

signals:
    void changeStatus(const QString & status);
    void refreshRules();

public slots:
    void changeSliderValuesSensor1(int index);
    void changeSliderValuesSensor2(int index);
    void createRule();
    void deleteRule();

private:
    void updateRulesTable();
    QLabel * m_header;
    QHBoxLayout * m_mainLayout;
    QVBoxLayout * m_verticalLayout;
    QVBoxLayout * m_newRuleLayout;
    QHBoxLayout * m_sensor1Layout;
    QHBoxLayout * m_sensor2Layout;
    QHBoxLayout * m_ruleListLayout;
    QHBoxLayout * m_buttonLayout;
    QHBoxLayout * m_tableLayout;    
    QComboBox * m_sensor1;
    QComboBox * m_sensor2;
    QComboBox * m_operator1;
    QComboBox * m_operator2;
    QComboBox * m_booleanChoice;
    QSlider * m_sensor1Value;
    QSlider * m_sensor2Value;
    QLCDNumber * m_sensor1ValueDisplay;
    QLCDNumber * m_sensor2ValueDisplay;
    QPushButton * m_createRuleButton;
    QPushButton * m_deleteRuleButton;
    QTableWidget * m_ruleListTable;

    Automon * m_kernel;

};

#endif // RULEEDITORWIDGET_H
