/*
    ==================================================================================================
    |  ruleeditorwidget.cpp                                                                          |
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

    This is the implementation file of the RuleEditorWidget. It is a complex widget and more details
    are included below
*/

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QSlider>
#include <QPushButton>
#include <QTableWidget>

#include "ruleeditorwidget.h"
#include "sensor.h"

RuleEditorWidget::RuleEditorWidget(Automon * kernel, QWidget * parent)
        : QWidget(parent), m_kernel(kernel)
{

    /* Create layout managers */
    m_mainLayout = new QHBoxLayout(this);
    m_verticalLayout = new QVBoxLayout();
    m_newRuleLayout = new QVBoxLayout();
    m_sensor1Layout = new QHBoxLayout();
    m_sensor2Layout = new QHBoxLayout();
    m_buttonLayout = new QHBoxLayout();
    m_tableLayout = new QHBoxLayout();
    m_ruleListLayout = new QHBoxLayout();

    /* Assign alignment to layouts */
    m_verticalLayout->setAlignment(Qt::AlignTop);
    m_mainLayout->setAlignment(Qt::AlignTop);
    m_mainLayout->addLayout(m_verticalLayout);

    /* Create a table list for displaying the rules in the system */
    m_ruleListTable = new QTableWidget();

    /* Set a style sheet for the table */
    m_ruleListTable->setStyleSheet("color:beige;");

    /* Add the table list to the table layout manager */
    m_tableLayout->addWidget(m_ruleListTable);

    m_tableLayout->setAlignment(Qt::AlignCenter);


    /* Create the create rule and delete rule push buttons */
    m_createRuleButton = new QPushButton(tr("Create Rule"));
    m_deleteRuleButton = new QPushButton(tr("Delete Rule"));

    /* Populate the rules table with rules loaded from the rules file */
    updateRulesTable();

    /* Connect the push buttons to their appropiate slots */
    connect(m_createRuleButton, SIGNAL(clicked()), this, SLOT(createRule()));
    connect(m_deleteRuleButton, SIGNAL(clicked()), this, SLOT(deleteRule()));

    /* Set the width of the buttons */
    m_createRuleButton->setFixedWidth(200);
    m_deleteRuleButton->setFixedWidth(200);

    /* Create header label and set it's stylesheet and add to a layout manager */
    m_header = new QLabel(tr("Rule Editor"));
    m_header->setStyleSheet("color: #ace413; font-size:25px");
    m_verticalLayout->addWidget(m_header);

    QLabel * welcome = new QLabel(tr("Welcome to the rule editor. Here you can create or delete rules"));
    m_verticalLayout->addWidget(welcome);

    /* Create sensor 1 combo box list and sensor 2 combo box list */
    m_sensor1 = new QComboBox();
    m_sensor2 = new QComboBox();

    /* Add an Ignore This option if you don't want to include the sensor combo in the adding to the rule */
    m_sensor1->addItem(tr("Ignore This"), 0);
    m_sensor2->addItem(tr("Ignore This"), 0);

    /* Get a list of all sensors */
    QList<Sensor*> availableSensors = m_kernel->getAllSensors();

    for (int i = 0; i < availableSensors.size(); i++)
    {
        if (availableSensors.at(i)->isSupported())
        {
            /* Only if the sensor is supported by the current vehicle, add it to the sensor combos */
            m_sensor1->addItem(availableSensors.at(i)->getName(), availableSensors.at(i)->getPid());
            m_sensor2->addItem(availableSensors.at(i)->getName(), availableSensors.at(i)->getPid());
        }
    }

    /* Connect the combos to their respective slots */
    connect(m_sensor1, SIGNAL(currentIndexChanged(int)), this, SLOT(changeSliderValuesSensor1(int)));
    connect(m_sensor2, SIGNAL(currentIndexChanged(int)), this, SLOT(changeSliderValuesSensor2(int)));


    /* Create a combo box for the comparison operator for both sensors*/
    m_operator1 = new QComboBox();
    m_operator1->addItem(tr("Less Than"), QString("<"));
    m_operator1->addItem(tr("Greater Than"), QString(">"));
    m_operator1->addItem(tr("Less Than Equal To"), QString("<="));
    m_operator1->addItem(tr("Greater Than Equal To"), QString(">="));
    m_operator1->addItem(tr("Not Equal To"), QString("!="));
    m_operator1->addItem(tr("Equal To"), QString("=="));

    m_operator2 = new QComboBox();
    m_operator2->addItem(tr("Less Than"), QString("<"));
    m_operator2->addItem(tr("Greater Than"), QString(">"));
    m_operator2->addItem(tr("Less Than Equal To"), QString("<="));
    m_operator2->addItem(tr("Greater Than Equal To"), QString(">="));
    m_operator2->addItem(tr("Not Equal To"), QString("!="));
    m_operator2->addItem(tr("Equal To"), QString("=="));

    /* This is the bool between both, AND or OR */
    m_booleanChoice = new QComboBox();

    /* Set it's width to the width of the sensor combo */
    m_booleanChoice->setFixedWidth(m_sensor1->sizeHint().width());

    /* Add the AND and OR options, with human readable versions and under lining computer style ones */
    m_booleanChoice->addItem(tr("AND"), QString("&&"));
    m_booleanChoice->addItem(tr("OR"), QString("||"));


    /* Create a slider for the first sensor */
    m_sensor1Value = new QSlider(Qt::Horizontal);
    m_sensor1Value->setTickInterval(1);
    m_sensor1Value->setValue(0);

    /* Create a slider for the second sensor */
    m_sensor2Value = new QSlider(Qt::Horizontal);
    m_sensor2Value->setTickInterval(1);
    m_sensor2Value->setValue(0);

    /* Create a QLCDNumber for both sensors to show the value the slider is at */
    m_sensor1ValueDisplay = new QLCDNumber();
    m_sensor2ValueDisplay = new QLCDNumber();

    /* Connect the slider valueChanged signal to the display slot in the QLCDNumber objects so we see the value */
    connect (m_sensor1Value, SIGNAL(valueChanged(int)), m_sensor1ValueDisplay, SLOT(display(int)));
    connect (m_sensor2Value, SIGNAL(valueChanged(int)), m_sensor2ValueDisplay, SLOT(display(int)));

    /* Initialise both slides to first sensor */
    changeSliderValuesSensor1(0);
    changeSliderValuesSensor2(0);

    /* Set up sensor 1 layout manager */
    m_sensor1Layout->addWidget(m_sensor1);
    m_sensor1Layout->addWidget(m_operator1);
    m_sensor1Layout->addWidget(m_sensor1ValueDisplay);
    m_sensor1Layout->addWidget(m_sensor1Value);

    /* Set up sensor 2 layout manager */
    m_sensor2Layout->addWidget(m_sensor2);
    m_sensor2Layout->addWidget(m_operator2);
    m_sensor2Layout->addWidget(m_sensor2ValueDisplay);
    m_sensor2Layout->addWidget(m_sensor2Value);

    /* Set up the rule layout manager */
    m_newRuleLayout->addLayout(m_sensor1Layout);
    m_newRuleLayout->addWidget(m_booleanChoice);
    m_newRuleLayout->addLayout(m_sensor2Layout);

    /* Organise layouts */
    m_verticalLayout->addLayout(m_newRuleLayout);
    m_verticalLayout->addSpacing(10);
    m_verticalLayout->addLayout(m_tableLayout);
    m_verticalLayout->addLayout(m_buttonLayout);
    m_buttonLayout->addWidget(m_createRuleButton);
    m_buttonLayout->addWidget(m_deleteRuleButton);
    m_buttonLayout->addStretch();

    m_verticalLayout->addLayout(m_ruleListLayout);

    /* Set the main layout of this widget to the main one */
    setLayout(m_mainLayout);
}

void RuleEditorWidget::changeSliderValuesSensor1(int index)
{
    /*
        This method is responsible for setting the range in the slider to that of the range of the current
        sensor selected. Eg: speed is between 0 and 255, while coolant is from -40 to 150
    */

    if (index != 0)
    {
        /* If the index is not 0, IE: Sensor 1 combo box is not set to "Ignore Sensor" */

        /* Get the sensor's command */
        QString selectedSensorCommand = m_sensor1->itemData(index).toString();

        /* Get pointer to sensor */
        Sensor * sensorPtr = m_kernel->getSensorByCommand(selectedSensorCommand);

        /* Get the high and low value */
        int highVal = sensorPtr->getMax();
        int lowVal = sensorPtr->getMin();

        /* Set the slider high and low values to the sensor's high and low values */
        m_sensor1Value->setMaximum(highVal);
        m_sensor1Value->setMinimum(lowVal);

        /* Set slide value to 0 */
        m_sensor1Value->setValue(0);

        /* Enable the slider */
        m_sensor1Value->setEnabled(true);
    }
    else
    {
        /* Otherwise if set to "Ignore Sensor", set slide to disabled */
        m_sensor1Value->setEnabled(false);
    }

}

void RuleEditorWidget::changeSliderValuesSensor2(int index)
{
     /*
        This method is responsible for setting the range in the slider to that of the range of the current
        sensor selected. Eg: speed is between 0 and 255, while coolant is from -40 to 150
    */

    if (index != 0)
    {
        /* If the index is not 0, IE: Sensor 2 combo box is not set to "Ignore Sensor" */

        /* Get the sensor's command */
        QString selectedSensorCommand = m_sensor2->itemData(index).toString();

        /* Get pointer to sensor */
        Sensor * sensorPtr = m_kernel->getSensorByCommand(selectedSensorCommand);

        /* Get the high and low value */
        int highVal = sensorPtr->getMax();
        int lowVal = sensorPtr->getMin();

        /* Set the slider high and low values to the sensor's high and low values */
        m_sensor2Value->setMaximum(highVal);
        m_sensor2Value->setMinimum(lowVal);

        /* Set slide value to 0 */
        m_sensor2Value->setValue(0);

        /* Enable the slider */
        m_sensor2Value->setEnabled(true);
    }
    else
    {
        /* Otherwise if set to "Ignore Sensor", set slide to disabled */
        m_sensor2Value->setEnabled(false);
    }

}

void RuleEditorWidget::createRule()
{
    /*
      This is the slot that is called when we click the create Rule button.
      It creates the rule string and adds it to the rule list and calls the kernel to update the file
    */


    /* First must validate that the rule is ok */

    if (m_sensor1->currentIndex() == 0 && m_sensor2->currentIndex() == 0)
    {
        /* Both sensors were ignored */
        emit changeStatus(tr("No Sensor Added!"));
        return;
    }

    if (m_sensor1->itemData(m_sensor1->currentIndex()) == m_sensor2->itemData(m_sensor2->currentIndex()))
    {
        /* Same sensor used in both combos */
        emit changeStatus(tr("Cannot use a rule where the same sensor is used twice!"));
        return;
    }


    QString sensor1Command;
    QString sensor1Operator;
    QString sensor1Value;

    QString booleanExpression = m_booleanChoice->itemData(m_booleanChoice->currentIndex()).toString();

    QString sensor2Command;
    QString sensor2Operator;
    QString sensor2Value;

    if (m_sensor1->currentIndex() > 0)
    {
        /* Sensor 1 combo has a sensor selected. Get the command, operator and value */
        sensor1Command = m_sensor1->itemData(m_sensor1->currentIndex()).toString();
        sensor1Operator = m_operator1->itemData(m_operator1->currentIndex()).toString();
        sensor1Value = QString::number(m_sensor1Value->value());
    }

    if (m_sensor2->currentIndex() > 0)
    {
        /* Sensor 2 combo has a sensor selected. Get the command, operator and value */
        sensor2Command = m_sensor2->itemData(m_sensor2->currentIndex()).toString();
        sensor2Operator = m_operator2->itemData(m_operator2->currentIndex()).toString();
        sensor2Value = QString::number(m_sensor2Value->value());
    }

    /* This QString will hold our rule */
    QString rule;

    /* Depending on which combos selected, create the appropiate rule string */
    if (m_sensor1->currentIndex() > 0 && m_sensor2->currentIndex() > 0)
        rule = QString("s") + sensor1Command + " " + sensor1Operator + " " + sensor1Value + " " + booleanExpression + " " + QString("s") + sensor2Command + " " + sensor2Operator + " " + sensor2Value;
    else if (m_sensor1->currentIndex() > 0 && m_sensor2->currentIndex() == 0)
        rule = QString("s") + sensor1Command + " " + sensor1Operator + " " + sensor1Value;
    else if (m_sensor2->currentIndex() > 0 && m_sensor1->currentIndex() == 0)
        rule = QString("s") + sensor2Command + " " + sensor2Operator + " " + sensor2Value;

    emit changeStatus(tr("Rule Successfully Added!"));

    /* Add the rule to the kernel */
    m_kernel->addRuleString(rule);

    /* Save the rule list so it's saved to file */
    m_kernel->saveRuleList();

    /* Update the rule table to reflect the new rule added */
    updateRulesTable();

    /* This signal is used so that the Monitoring widget can get updated list of values in it's combos of available rules */
    emit refreshRules();

}

void RuleEditorWidget::deleteRule()
{
    /*
        This slot is called when the user clicks the delete rule button.
        It removes a rule from the list and deletes it from the kernel
    */

    if (m_ruleListTable->currentItem() == NULL)
    {
        /* No rule was selected for deletion in the rule table */
        emit changeStatus("No Rule Selected!");
        return;
    }

    /* Get the current rule selected. It will be in human readable format */
    QString selectedRule = m_ruleListTable->currentItem()->data(Qt::DisplayRole).toString();

    /* Remove the rule using the kernel's method for removing by human readable format */
    m_kernel->removeRuleEnglishMeaningString(selectedRule);

    /* Save the rule list back to file */
    m_kernel->saveRuleList();

    /* Update the rule table list to show rule gone */
    updateRulesTable();

    /* The following signal is connected to the monitoring widget slot that refreshes combos in the monitoring widget */
    emit refreshRules();

    /* Update status */
    emit changeStatus("Rule Successfully Removed!");
}

void RuleEditorWidget::updateRulesTable()
{
    /*
        This method is responsible for re populating the rules table list
    */

    /* Reload the rules list from file so we have up to date rules list */
    m_kernel->loadRuleList();

    /* Get the list of rules in a string list */
    QStringList ruleList = m_kernel->getRuleList();

    /* Set the table properties */
    m_ruleListTable->setRowCount(ruleList.size());
    m_ruleListTable->setColumnCount(1);
    m_ruleListTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ruleListTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    /* Set table column header labels */
    QStringList headerItems;
    headerItems.append("Current Rules");

    m_ruleListTable->setHorizontalHeaderLabels(headerItems);

    m_ruleListTable->setColumnWidth(0,730);

    /* Insert each rule into the table, starting with the most up to date rule on top */
    for (int i = ruleList.size()-1; i >= 0; i--)
    {
        QTableWidgetItem *ruleItem = new QTableWidgetItem(m_kernel->convertRuleToEnglish((ruleList.at(i))));
        ruleItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        m_ruleListTable->setItem(ruleList.size()-1-i, 0, ruleItem);
    }

}
