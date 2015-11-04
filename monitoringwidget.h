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

#ifndef MONITORINGWIDGET_H
#define MONITORINGWIDGET_H

#include <QWidget>
#include <QList>

#include "automon.h"

class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QTableWidget;
class QPushButton;
class QComboBox;

using namespace AutomonKernel;

class MonitoringWidget : public QWidget
{
    Q_OBJECT

public:
    MonitoringWidget(Automon * kernel, QWidget * parent = 0);

signals:
    void changeStatus(const QString & status);
    void changePanel(int panelID);


public slots:
    void addSensor();
    void removeSensor();
    void addRule();
    void removeRule();
    void display(double sensorVal);
    void startMonitoring();
    void ruleHandler(QString ruleString);
    void displayRuleEditor();
    void refreshRules();

private:
    void populateRulesAvailableList();
    void populateSensorCombo();
    void populateFrequencyUpdateList();

    int m_tableWidth;

    QLabel * m_header;
    QHBoxLayout * m_mainLayout;
    QVBoxLayout * m_verticalLayout;
    QHBoxLayout * m_mainSensorLayout;
    QVBoxLayout * m_sensorLeftLayout;
    QVBoxLayout * m_sensorRightLayout;
    QVBoxLayout * m_sensorAddRemoveButtonsLayout;
    QHBoxLayout * m_sensorComboboxes;

    QHBoxLayout * m_mainRuleLayout;
    QVBoxLayout * m_ruleLeftLayout;
    QVBoxLayout * m_ruleRightLayout;
    QVBoxLayout * m_ruleAddRemoveButtonsLayout;
    QHBoxLayout * m_ruleComboboxes;

    QTableWidget * m_sensorsList;
    QComboBox * m_availableRulesList;
    QTableWidget * m_addedRulesList;
    QPushButton * m_addRuleButton;
    QPushButton * m_removeRuleButton;
    QPushButton * m_addSensorButton;
    QPushButton * m_removeSensorButton;
    QPushButton * m_ruleEditorButton;
    QPushButton * m_startStopMonitoring;
    QComboBox * m_sensorComboList;
    QComboBox * m_frequencyUpdateList;
    QList<Rule*> m_rules;
    Automon * m_kernel;
    bool m_isMonitoring;
};

#endif // MONITORINGWIDGET_H

