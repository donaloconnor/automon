#ifndef RULEEDITORWIDGET_H
#define RULEEDITORWIDGET_H

/*
    ==================================================================================================
    |  ruleeditorwidget.h                                                                            |
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

    This class, RuleEditorWidget is a widget that is loaded from the Monitoring widget.
    It provides a GUI interface to add and delete rules. It provides a scroll bar to set ranges and
    the logic for what the rule stands for. It supports adding one or two sensors to the rule
*/

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
