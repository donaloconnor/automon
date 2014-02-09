/*
    ==================================================================================================
    |  monitoringwidget.cpp                                                                          |
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

    This is the implementation file of the MonitoringWidget. It is a very complex widget and more
    details are explained below.
*/


#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QList>
#include <QPushButton>
#include <QComboBox>
#include <QColor>
#include <QRgb>

#include "monitoringwidget.h"
#include "rule.h"
#include "automonapp.h"

MonitoringWidget::MonitoringWidget(Automon * kernel, QWidget * parent)
        : QWidget(parent), m_kernel(kernel)
{
    /* Create all our layouts managers */
    m_mainLayout = new QHBoxLayout(this);
    m_verticalLayout = new QVBoxLayout();
    m_mainSensorLayout = new QHBoxLayout();
    m_sensorLeftLayout = new QVBoxLayout();
    m_sensorRightLayout = new QVBoxLayout();
    m_sensorAddRemoveButtonsLayout = new QVBoxLayout();
    m_mainRuleLayout = new QHBoxLayout();
    m_ruleLeftLayout = new QVBoxLayout();
    m_ruleRightLayout = new QVBoxLayout();
    m_ruleAddRemoveButtonsLayout = new QVBoxLayout();
    m_sensorComboboxes = new QHBoxLayout();
    m_ruleComboboxes = new QHBoxLayout();

    m_sensorComboboxes->setAlignment(Qt::AlignTop);

    /* This variable is used by this widget to determine if we've started monitoring */
    m_isMonitoring = false;

    /* Create our tables. The sensor table list and the rules list */

    m_sensorsList = new QTableWidget();
    m_addedRulesList = new QTableWidget();

    /* Set the stylesheets for our tables */
    m_sensorsList->setStyleSheet("color:beige");
    m_addedRulesList->setStyleSheet("color:beige");

    /* Set some behaviours, to avoid multi selection and make it row selection */
    m_sensorsList->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_addedRulesList->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_sensorsList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_addedRulesList->setSelectionMode(QAbstractItemView::SingleSelection);


    /* Create our push buttons viewable on screen */
    m_addRuleButton = new QPushButton(tr("Add Rule"));
    m_removeRuleButton = new QPushButton(tr("Remove Rule"));
    m_addSensorButton = new QPushButton(tr("Add Sensor"));
    m_removeSensorButton = new QPushButton(tr("Remove Sensor"));
    m_ruleEditorButton = new QPushButton(tr("Rule Editor"));
    m_startStopMonitoring = new QPushButton(tr("Start Monitoring"));

    /* Connect the clicked signal of push buttons to their appropiate slots */
    connect(m_addSensorButton, SIGNAL(clicked()), this, SLOT(addSensor()));
    connect(m_removeSensorButton, SIGNAL(clicked()), this, SLOT(removeSensor()));
    connect(m_startStopMonitoring, SIGNAL(clicked()), this, SLOT(startMonitoring()));
    connect(m_ruleEditorButton, SIGNAL(clicked()), this, SLOT(displayRuleEditor()));
    connect(m_addRuleButton, SIGNAL(clicked()), this, SLOT(addRule()));
    connect(m_removeRuleButton, SIGNAL(clicked()), this, SLOT(removeRule()));

    /* Create a combo for the available rules list */
    m_availableRulesList = new QComboBox();

    /* Populate the available rules in the combo box */
    populateRulesAvailableList();

    /* Create our sensor combo that will hold available sensors */
    m_sensorComboList = new QComboBox();

    /* Populate the sensor's combo with all SUPPORTED sensors for the current vehicle */
    populateSensorCombo();

    /* Create a combo to select the frequency update. This specifies how often a sensor gets access to the ELM */
    m_frequencyUpdateList = new QComboBox();

    /* Populate the 1 - 30 Hz frequencies in the combo box */
    populateFrequencyUpdateList();

    /* Create our header and make font bigger with color green */
    m_header = new QLabel(tr("Monitoring"));
    m_header->setStyleSheet("color: #ace413; font-size:25px");

    /* Add a layout to our main layout */
    m_mainLayout->addLayout(m_verticalLayout);

    /* Add the header text to the layout */
    m_verticalLayout->addWidget(m_header);

    /* Create a welcome/introduction label and add this to the layout next */
    QLabel * welcome = new QLabel(tr("Welcome to Monitoring. Simply add which sensors, their frequency of update and what rules you want to apply."));
    m_verticalLayout->addWidget(welcome);

    /* Create new sensor label, and frequency labels setting their fixed widths */
    QLabel * addSensorLabel = new QLabel(tr("Add Sensor: "));
    addSensorLabel->setFixedWidth(80);
    QLabel * updateFrequencyLabel = new QLabel(tr("Update Frequency: "));
    updateFrequencyLabel->setFixedWidth(120);

    /* Create the add Rule label and set it's fixed width */
    QLabel * addRuleLabel = new QLabel(tr("Add Rule: "));
    addRuleLabel->setFixedWidth(80);

    /* Set the table width using the size hints of the other widgets we we make table full width */
    m_tableWidth = QSize(addSensorLabel->sizeHint()+m_sensorComboList->sizeHint()+updateFrequencyLabel->sizeHint()+m_frequencyUpdateList->sizeHint()).width();

    /* Set alignment of layouts */
    m_verticalLayout->setAlignment(Qt::AlignTop);
    m_mainLayout->setAlignment(Qt::AlignTop);

    /* Set up more Layouts */
    m_verticalLayout->addLayout(m_mainSensorLayout);
    m_verticalLayout->addLayout(m_mainRuleLayout);

    m_mainSensorLayout->addLayout(m_sensorLeftLayout);
    m_mainSensorLayout->addLayout(m_sensorRightLayout);

    m_mainRuleLayout->addLayout(m_ruleLeftLayout);
    m_mainRuleLayout->addLayout(m_ruleRightLayout);

    m_sensorLeftLayout->addLayout(m_sensorComboboxes);
    m_sensorRightLayout->addLayout(m_sensorAddRemoveButtonsLayout);

    m_ruleLeftLayout->addLayout(m_ruleComboboxes);
    m_ruleRightLayout->addLayout(m_ruleAddRemoveButtonsLayout);

    /* Add widgets to their associated layouts */
    m_sensorComboboxes->addWidget(addSensorLabel);
    m_sensorComboboxes->addWidget(m_sensorComboList);
    m_sensorComboboxes->addWidget(updateFrequencyLabel);
    m_sensorComboboxes->addWidget(m_frequencyUpdateList);

    m_sensorAddRemoveButtonsLayout->addWidget(m_addSensorButton);
    m_sensorAddRemoveButtonsLayout->addWidget(m_removeSensorButton);
    m_sensorAddRemoveButtonsLayout->addWidget(m_startStopMonitoring);

    m_ruleComboboxes->addWidget(addRuleLabel);
    m_ruleComboboxes->addWidget(m_availableRulesList);

    m_ruleAddRemoveButtonsLayout->addWidget(m_addRuleButton);
    m_ruleAddRemoveButtonsLayout->addWidget(m_removeRuleButton);
    m_ruleAddRemoveButtonsLayout->addWidget(m_ruleEditorButton);

    m_ruleLeftLayout->addWidget(m_addedRulesList);

    m_sensorLeftLayout->addWidget(m_sensorsList);

    /* Set the main layout */
    setLayout(m_mainLayout);
}

void MonitoringWidget::populateRulesAvailableList()
{
    /*
        This method is responsible for adding a list of available rules to the combo box for
        available rules
    */

    /* We may be calling this more than once, if we use rule editor so clear to prevent appending too many */
    m_availableRulesList->clear();

    /* Get available rules from the kernel */
    QStringList rulesAvailable = m_kernel->getRuleList();

    if (rulesAvailable.size() == 0)
    {
        /* If no rules available, add this to the rules List */
        m_availableRulesList->addItem(tr("No Rules Available!"), 0);
        return;
    }

    /*
        Otherwise, for each rule, add it to the combo box, making the human readable version for the text, and the normal
        representation for the value
    */

    for (int i = 0; i < rulesAvailable.size(); i++)
        m_availableRulesList->addItem(m_kernel->convertRuleToEnglish(rulesAvailable.at(i)), rulesAvailable.at(i));
}

void MonitoringWidget::addRule()
{
    /* This is the slot that is called when the user clicks to add a rule */

    /* The rule's table will only ever have a single column, the rule in human readable format */
    m_addedRulesList->setColumnCount(1);

    /* Get the rule to add */
    QString ruleToAdd = m_availableRulesList->itemData(m_availableRulesList->currentIndex()).toString();

    /* Convert to human readable text */
    QString ruleEnglishMeaning = m_availableRulesList->itemText(m_availableRulesList->currentIndex());

    for (int i = 0; i < m_addedRulesList->rowCount(); i++)
        if (m_addedRulesList->item(i,0)->data(Qt::DisplayRole).toString().compare(ruleEnglishMeaning) == 0)
        {
            /* Rule is already added so exit */
            emit changeStatus("Rule already added!");
            return;
        }

    /* Update row count of rule table to add in another rule */
    m_addedRulesList->setRowCount(m_addedRulesList->rowCount()+1);

    /* Create the cell item */
    QTableWidgetItem * columnItem = new QTableWidgetItem(ruleEnglishMeaning);

    /* Disable it from being editable by setting these flags */
    columnItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

    /* Set the item in the correct position in the rules table */
    m_addedRulesList->setItem(m_addedRulesList->rowCount()-1, 0, columnItem);


    /* If the row count is > 2, scroll bars appear so make adjustments to the width of table to accomodate this */
    if (m_addedRulesList->rowCount() > 2)
        m_addedRulesList->setColumnWidth(0, m_tableWidth-15);
    else
        m_addedRulesList->setColumnWidth(0, m_tableWidth);

    /* Set the table header labels */
    QStringList headerItemsRules;
    headerItemsRules.append(tr("Added Rules"));

    m_addedRulesList->setHorizontalHeaderLabels(headerItemsRules);
}

void MonitoringWidget::removeRule()
{
    /*
      This method is a slot that is called by the remove rule button. It removes a rule from the table, the selected one
    */

    /* Get current row in rules table that is selected */
    int currentRowSelected = m_addedRulesList->currentRow();

    if (currentRowSelected == -1)
    {
        /* No row is selected so let user know and exit */
        emit changeStatus(tr("No Rule Selected!"));
        return;
    }

    /* Simply remove the current row */
    m_addedRulesList->removeRow(currentRowSelected);

    emit changeStatus(tr("Rule removed"));
}

void MonitoringWidget::addSensor()
{
    /*
        This slot is what is called when the user clicks the add sensor button.
    */

    /* The sensor list will have 4 columns always: English Meaning, Code, Frequency and value */
    m_sensorsList->setColumnCount(4);

    /* Current Sensor */
    QString englishMeaning = m_sensorComboList->currentText();

    QString code = m_sensorComboList->itemData(m_sensorComboList->currentIndex()).toString();

    /* Check is used for conversion of QString to int */
    bool check;

    /* Convert the QVariant data to integer */
    int frequency = m_frequencyUpdateList->itemData(m_frequencyUpdateList->currentIndex()).toInt(&check);

    /* Get the current number of rows */
    int currentNumRows = m_sensorsList->rowCount();


    /* First ensure sensor not already added */
    
    for (int i = 0; i < currentNumRows; i++)
        if (m_sensorsList->item(i,1)->data(Qt::DisplayRole).toString().compare(code) == 0)
        {
            /* Sensor already present. Exit */
            emit changeStatus(tr("Sensor already present!"));
            return;
        }

       
    /* Update the row count to + 1 to accomodate a new sensor */
    m_sensorsList->setRowCount(currentNumRows+1);

    /* Create the cell items, 4 of them in total and set them to not editable by setting flags */
    QTableWidgetItem * column1Item = new QTableWidgetItem(englishMeaning);
    column1Item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    QTableWidgetItem * column2Item = new QTableWidgetItem(code);
    column2Item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    QTableWidgetItem * column3Item = new QTableWidgetItem(QString(QString::number(frequency)+"Hz"));
    column3Item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    QTableWidgetItem * column4Item = new QTableWidgetItem("0");
    column4Item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

    /* Set the items in their correct column/row positions */
    m_sensorsList->setItem(currentNumRows, 0, column1Item);
    m_sensorsList->setItem(currentNumRows, 1, column2Item);
    m_sensorsList->setItem(currentNumRows, 2, column3Item);
    m_sensorsList->setItem(currentNumRows, 3, column4Item);

    /* Set the widths of each column */
    int col1Width = m_tableWidth * .5;
    int col2Width = m_tableWidth * .15;
    int col3Width = m_tableWidth * .15;
    int col4Width = m_tableWidth * .20;

    /* If the row count is > 2, scroll bars present so reduce first column to accomodate the scroll bars */
    if (m_sensorsList->rowCount() > 2)
        col1Width -= m_tableWidth*.05;

    /* Set the column widths */
    m_sensorsList->setColumnWidth(0,col1Width);
    m_sensorsList->setColumnWidth(1,col2Width);
    m_sensorsList->setColumnWidth(2,col3Width);
    m_sensorsList->setColumnWidth(3,col4Width);

    /* Set the column header labels */
    QStringList headerItemsSensors;
    headerItemsSensors.append(tr("Sensor Name"));
    headerItemsSensors.append(tr("Code"));
    headerItemsSensors.append(tr("Frequency"));
    headerItemsSensors.append(tr("Value"));

    m_sensorsList->setHorizontalHeaderLabels(headerItemsSensors);

    /* Reset frequency combo box to first element (1Hz) for convienence */
    m_frequencyUpdateList->setCurrentIndex(0);

    emit changeStatus(tr("Sensor added to monitoring list!"));
}

void MonitoringWidget::removeSensor()
{
    /*
        This slot is called when the user clicks remove sensor.
    */

    /* We just have to find the selected Item. It's row will give us what we need */

    int currentRowSelected = m_sensorsList->currentRow();

    if (currentRowSelected == -1)
    {
        /* User has selected no sensor in the table so return */
        emit changeStatus(tr("No Sensor Selected!"));
        return;
    }

    /* Otherwise, they have so remove the selected row */
    m_sensorsList->removeRow(currentRowSelected);

    emit changeStatus(tr("Sensor removed"));
}

void MonitoringWidget::populateSensorCombo()
{
    /*
        This method is responsible for inserting all the available sensors into the sensor combo box
    */

    /* Clear first to avoid appending to old ones */
    m_sensorComboList->clear();

    /* Get list of sensor pointers */
    QList<Sensor*> availableSensors = m_kernel->getAllSensors();

    for (int i = 0; i < availableSensors.size(); i++)
    {
        if (availableSensors.at(i)->isSupported())
        {
            /* Only add a sensor to the combo box if it is supported by the current vehicle */
            m_sensorComboList->addItem(availableSensors.at(i)->getName(), availableSensors.at(i)->getPid());
        }
    }
}


void MonitoringWidget::populateFrequencyUpdateList()
{
    /*
        This method simply populates the Frequency combo with frequency ranges, 1Hz to 30Hz
    */

    for (int i = 1; i <= 30; i++)
        m_frequencyUpdateList->addItem(QString(QString::number(i)+"Hz (Every "+QString::number(i)+" cycles)"),i);
}

void MonitoringWidget::display(double sensorVal)
{
    /*
        This is the slot that is called by all sensors added to monitor when their values change
        It is responsible for locating the correct cell in the table and updating the new value
    */

    /* Get calling sensor pointer */
    Sensor * caller = dynamic_cast<Sensor*>(QObject::sender());

    /* Get the command of the sensor, ie its pid */
    QString sensorCode = caller->getPid();

    /* Go through each row and find the row that matches this sensor */
    for (int i = 0; i < m_sensorsList->rowCount(); i++)
        if (m_sensorsList->item(i,1)->data(Qt::DisplayRole).toString().compare(sensorCode) == 0) /* We have the pid/code match */
            m_sensorsList->item(i,3)->setData(Qt::DisplayRole, QString::number(sensorVal)); /* Update the value cell with new value */
}

void MonitoringWidget::startMonitoring()
{
    /*
        This is the method that is called when the user clicks the start/stop monitoring button.
        This is a complex method in that it looks after setting up the rules as well and ensuring
        that the rules can work, ie: all sensors in the rules are currently added to the monitoring etc
    */


    if (!m_isMonitoring && m_kernel->isMonitoring())
    {
        /* Automon is currently working on some other task so let user know and return */
        emit changeStatus(tr("Automon is currently busy with another task. Stop this first"));
        return;
    }

    /* Clear all active sensors from Serial I/O thread */
    m_kernel->removeAllActiveSensors();

    if (m_sensorsList->rowCount() == 0)
    {
        /* No sensors are added to the sensor table. Let user know and return */
        emit changeStatus(tr("No Sensors to Monitor!"));
        return;
    }

    /* Disable all buttons to prevent user interrupting */
    m_startStopMonitoring->setEnabled(false);
    m_addSensorButton->setEnabled(false);
    m_removeSensorButton->setEnabled(false);
    m_addRuleButton->setEnabled(false);
    m_removeRuleButton->setEnabled(false);
    m_ruleEditorButton->setEnabled(false);

    if (m_isMonitoring)
    {
        /* We are already monitoring so this button press is to stop monitoring */
        m_startStopMonitoring->setEnabled(false);

        for (int i = 0; i < m_sensorsList->rowCount(); i++)
        {
            /* For each sensor in the table, get its sensor code */
            QString sensorCode = m_sensorsList->item(i,1)->data(Qt::DisplayRole).toString();

            /* Disconnect the sensor's changeValue signal from our custom slot in this widget */
            disconnect(m_kernel->getActiveSensorByCommand(sensorCode), SIGNAL(changeOccurred(double)), this, SLOT(display(double)));

            /* Get a pointer to the current sensor */
            Sensor * thisSensor = m_kernel->getActiveSensorByCommand(sensorCode);

            /* Remove the sensor from the active list in the serial thread */
            if (!m_kernel->removeActiveSensorByCommand(sensorCode))
                emit changeStatus(tr("Sensor : ")+sensorCode+tr(" could not be removed!"));

            /* Reset the sensor, such as sensor change times etc. State has to be reversed */
            thisSensor->resetSensor();
        }

        /* Stop the serial thread */
        m_kernel->stopMonitoring();

        /* Change text on push button to more appropiate text */
        m_startStopMonitoring->setText(tr("Start Monitoring"));
        emit changeStatus(tr("Monitoring Stopped!"));

        /* Update this variable so next time we know that monitoring has stopped */
        m_isMonitoring = false;

        /* Re enable all push buttons */
        m_startStopMonitoring->setEnabled(true);
        m_addSensorButton->setEnabled(true);
        m_removeSensorButton->setEnabled(true);
        m_addRuleButton->setEnabled(true);
        m_removeRuleButton->setEnabled(true);
        m_ruleEditorButton->setEnabled(true);

        return;
    }

    /* If we get this far, monitoring is starting */

    /* Reset the background of all rules */
    for (int i = 0; i < m_addedRulesList->rowCount(); i++)
    {
        m_addedRulesList->item(i,0)->setBackground(QBrush(QColor(0,0,0,0)));
        m_addedRulesList->item(i,0)->setForeground(QBrush(Qt::white));
    }

    for (int i = 0; i < m_sensorsList->rowCount(); i++)
    {
        /* For each sensor in the list of sensors to monitor... */

        /* Get sensor code */
        QString sensorCode = m_sensorsList->item(i,1)->data(Qt::DisplayRole).toString();

        /* Bool is used for QString->Interger conversion */
        bool check;

        /* Get the frequency from the table */
        QString frequencyStr = m_sensorsList->item(i,2)->data(Qt::DisplayRole).toString();

        /* Convert to integer frequency */
        int frequency = frequencyStr.section("", 0,frequencyStr.indexOf("Hz")).toInt(&check);

        /* Add the sensor to the serial thread */
        if (m_kernel->addActiveSensor(m_kernel->getSensorByCommand(sensorCode)))
        {
            /* Sensor added successfully, now update it's refresh frequency to the user defined one in table */
            m_kernel->setSensorFrequency(m_kernel->getSensorByCommand(sensorCode), frequency);

            /* Connect the sensor's signal to our slot */
            connect(m_kernel->getActiveSensorByCommand(sensorCode), SIGNAL(changeOccurred(double)), this, SLOT(display(double)));
        }
        else
            emit changeStatus(tr("Sensor : ")+sensorCode+tr(" could not be added!"));
    }

    /* Now it is time to create the rules */

    /* Clear old rules and delete them */
    for (int i = 0; i < m_rules.size(); i++)
    {
        disconnect(m_rules.at(i), SIGNAL(sendAlert(QString)), this, SLOT(ruleHandler(QString)));
        delete(m_rules.at(i));
    }

    /* Clear rules list */
    m_rules.clear();


    for (int i = 0; i < m_addedRulesList->rowCount(); i++)
    {
        /* For each rule in the table list */

        /* Get the rule from the table, it is in human readable format */
        QString currentRuleEnglishName = m_addedRulesList->item(i,0)->data(Qt::DisplayRole).toString();

        for (int j = 0; j < m_availableRulesList->count(); j++)
        {
            /* For each rule in the available rules list ... */

            /* Convert it to human readable format and compare to the one in the table */
            if (m_kernel->convertRuleToEnglish(m_availableRulesList->itemData(j).toString()).compare(currentRuleEnglishName) == 0)
            {

                /* Found the rule we working on. Get the real rule string, not human readable one */
                QString nonEnglishRule = m_availableRulesList->itemData(j).toString();

                /* Now we also must verify that all sensors present in the kernel active sensoring*/
                QStringList sensorsInRule = m_kernel->extractSensorsFromRule(nonEnglishRule);


                bool ruleCanBeAdded = true;

                for (int k = 0; k < sensorsInRule.size(); k++)
                    /* Now for each sensor in the rule, check if it is in the active sensors */
                    if (m_kernel->getActiveSensorByCommand(sensorsInRule.at(k)) == NULL)
                        ruleCanBeAdded = false;

                if (ruleCanBeAdded)
                {
                    /* The rulecanbeadded variable didn't change to false so all sensors are in the serial thread */

                    /* Safe to create rule now */
                    Rule * thisRule = new Rule();

                    /* Set the rule to the real rule string */
                    thisRule->setRule(nonEnglishRule);

                    /* Add the rule to the list of rules */
                    m_rules.append(thisRule);


                    /* Connect rule to our slot */
                    connect(thisRule, SIGNAL(sendAlert(QString)), this, SLOT(ruleHandler(QString)));

                    for (int v=0; v<sensorsInRule.size(); v++)
                    {
                        /* For each sensor in the rule string, check to ensure it not empty */
                        if (sensorsInRule.at(v).compare("") != 0)
                            /* If not empty, then add a pointer to the sensor to the rule */
                            thisRule->addSensor(m_kernel->getActiveSensorByCommand(sensorsInRule.at(v)));
                    }

                    /* Set the rule name to the english name */
                    thisRule->setRuleName(currentRuleEnglishName);

                    /* Activate the rule */
                    if (!thisRule->activate())
                    {
                        /* Rule could not be actived. Highlight the rule in red and reverse actions from before */
                        m_addedRulesList->item(i,0)->setForeground(QBrush(Qt::red));
                        emit changeStatus(tr("The selected rule had an error of some kind"));

                        /* Remove all the sensors */
                        m_kernel->removeAllActiveSensors();

                        /* Re enable all buttons */
                        m_startStopMonitoring->setEnabled(true);
                        m_addSensorButton->setEnabled(true);
                        m_removeSensorButton->setEnabled(true);
                        m_addRuleButton->setEnabled(true);
                        m_removeRuleButton->setEnabled(true);
                        m_ruleEditorButton->setEnabled(true);

                        return;
                    }

                    /* If we get here, the rule successfully added */
                    emit changeStatus(tr("Rule Added!"));
                }
                else
                {
                    /* If in here, the current rule has sensors that are not present in the serial thread.
                       IE: We forgot to add a sensor needed by this rule
                    */

                    /* Set the rule color to red to highlight which rule caused the error */
                    m_addedRulesList->item(i,0)->setForeground(QBrush(Qt::red));
                    emit changeStatus(tr("Rule could not be added. Sensor not in list!"));

                    /* Reverse all actions before, by removing the few sensors added to the serial thread */
                    m_kernel->removeAllActiveSensors();

                    /* Re enable all buttons */
                    m_startStopMonitoring->setEnabled(true);
                    m_addSensorButton->setEnabled(true);
                    m_removeSensorButton->setEnabled(true);
                    m_addRuleButton->setEnabled(true);
                    m_removeRuleButton->setEnabled(true);
                    m_ruleEditorButton->setEnabled(true);

                    return;
                }
            }

        }
    }

    /* Now that sensors added, time to start monitoring! */

    /* Attempt to start monitoring, ie: start the serial I/O thread */
    if (!m_kernel->startMonitoring())
    {
        /* We should not get in here. */
        emit changeStatus(tr("Failed to start serial thread. This is an exception! Restart the device."));
        m_startStopMonitoring->setEnabled(true);
        return;
    }

    /* Update the text on the start/stop monitoring button to stop now */
    m_startStopMonitoring->setText(tr("Stop Monitoring"));
    emit changeStatus(tr("Monitoring Started!"));

    /* Only re enable the start/stop monitoring button so user can stop the monitoring */
    m_startStopMonitoring->setEnabled(true);

    /* Update this variable so we know next time we click, we are stopping the monitoring */
    m_isMonitoring = true;

}

void MonitoringWidget::ruleHandler(QString ruleString)
{
    /*
        This slot is called when a rule is satisfied. We must notify the user by highlighting which rule
        in the rules table
    */

    /* Rule string is the rule that is satisfied. Set the background color of cell to blue */


    for (int i = 0; i < m_addedRulesList->rowCount(); i++)
        if (m_addedRulesList->item(i,0)->data(Qt::DisplayRole).toString().compare(ruleString) == 0)
        {
            m_addedRulesList->item(i,0)->setBackground(QBrush(QColor(136,167,47)));
            break;
        }

    emit changeStatus(tr("Rule Satisfied. Highlighted in Green"));
}

void MonitoringWidget::displayRuleEditor()
{
    /*
        This slot is called by the rule editor button. It changes the stacked widget index to
        the rule editor so we see that view
    */

    emit changePanel(5);
}

void MonitoringWidget::refreshRules()
{
    /*
        This is a slot that is called by the rule editor widget. It ensures that if changes made
        in the rule editor, ie: New rule added, or rule removed, that these changes are reflected
        in the monitor widget so we see the new rules in the available rules combo box or they removed
        if we removed
    */


    /* Update the combo box */
    populateRulesAvailableList();

    /* Now in the table, clear any rules not in the combo */
    for (int i = 0; i < m_addedRulesList->rowCount(); i++)
    {
        bool found = false;

        for (int j =0; j < m_availableRulesList->count(); j++)
        {
            if (m_addedRulesList->item(i,0)->data(Qt::DisplayRole).toString().compare(m_availableRulesList->itemText(j)) == 0)
            {
                /* We found a rule in the table that is no longer in the available rules list */
                found = true;
                break;
            }
        }

        if (!found)
        {
            /* So the rule we had before, that is now gone has being removed. So remove from list */
            m_addedRulesList->removeRow(i);
        }
    }
}
