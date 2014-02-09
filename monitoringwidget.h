#ifndef MONITORINGWIDGET_H
#define MONITORINGWIDGET_H

/*
    ==================================================================================================
    |  monitoringwudget.h                                                                            |
    |  Part of the Automon application.                                                              |
    |                                                                                                |
    |  Final Year Project - "An Embedded Automotive Monitoring Device"                               |
    |                                                                                                |
    |  By Donal O' Connor for completion of B.Sc (Hons) Software Development and Computer Networking |
    |  Email: donaloconnor@gmail.com                                                                 |
    |  Website/Blog: http://automon.killarneyonline.eu                                               |
    |                                                                                                |
    | Cork Institute of Technology, Cork, Ireland - http://www.cit.ie/                               |
    |                                                                                                |
    | Copyright © 2009 Donal O'Connor <donaloconnor@gmail.com>                                       |
    ==================================================================================================

    This is one of the stacked widgets/ views that the user of Automon sees.
    It is the Monitoring widget. It is a very complex widget that allows the user to set up
    multiple sensors to set up and what rules they want to apply.
*/

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

