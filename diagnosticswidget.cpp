/*
    ==================================================================================================
    |  diagnosticswidget.cpp                                                                         |
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

    This is the implementation file for the DiagnosticsWidget of the GUI. More details are below
*/

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QPixmap>
#include <QMessageBox>

#include "dtc.h"
#include "diagnosticswidget.h"

DiagnosticsWidget::DiagnosticsWidget(Automon * kernel, QWidget * parent)
        : QWidget(parent), m_kernel(kernel)
{
    /* Create the layout managers */
    m_mainLayout = new QHBoxLayout(this);
    m_verticalLayout = new QVBoxLayout();
    m_contentArea = new QHBoxLayout();
    m_leftSide = new QVBoxLayout();
    m_rightSide = new QVBoxLayout();
    m_buttonsTop = new QHBoxLayout();

    /* The m_tableList displays a list of DTCs if present in the cars ECU */
    m_tableList = new QTableWidget();
    m_tableList->setStyleSheet("color:beige;");
    m_tableList->setFixedHeight(300);

    /* Specify the picture (from resource file) displayed when MIL On and MIL off */
    engineMilOnPic = ":/files/enginemil.png";
    engineMilOffPic = ":/files/enginemiloff.png";

    /* The picture is displayed within a label */
    engineMilPic = new QLabel();

    /* Set to off by default */
    setMilPicOff();

    /* Create the prompt box that is used when user wants to clear DTCs and reset MIL */
    QString title(tr("Clear MIL"));
    QString message(tr("Are you sure you want to reset the MIL?<br /><br /><strong>This will:</strong><br />1. Turn off the MIL<br />2. Delete all current DTCs<br />3. Delete any Freeze Frame data<br /><br />It is also important to note that your car may not run perfect for a while afterwards as it needs to re-calibrate. <br />Are you sure you want to continue?"));
    m_resetMilPromptBox = new QMessageBox(QMessageBox::NoIcon, title, message, QMessageBox::Ok | QMessageBox::Cancel, 0, Qt::FramelessWindowHint);

    m_resetMilPromptBox->setDefaultButton(QMessageBox::Cancel);
    m_resetMilPromptBox->setStyleSheet("background-color: rgba(51,51,51,80%); color:beige");

    /* Add widgets on the right hand side layout */
    m_rightSide->addWidget(engineMilPic);
    m_rightSide->setAlignment(Qt::AlignRight);

    /* Create reset MIL push button and connect it's clicked signal to our custom slot */
    m_resetMilButton = new QPushButton(tr("Reset MIL"));
    m_resetMilButton->setFixedWidth(200);
    connect(m_resetMilButton, SIGNAL(clicked()), this, SLOT(resetMIL()));

    /* Create check ECU button and connnect ti's clicked signal to our custom slot */
    m_checkButton = new QPushButton(tr("Check ECU"));
    m_checkButton->setFixedWidth(200);
    connect(m_checkButton, SIGNAL(clicked()), this, SLOT(checkECU()));

    /* Add more widgets to the layouts */
    m_buttonsTop->addWidget(m_checkButton);
    m_buttonsTop->addStretch();
    m_buttonsTop->addWidget(m_resetMilButton);


    /* Create our header label and assign style */
    m_header = new QLabel(tr("Diagnostics"));
    m_header->setStyleSheet("color: #ace413; font-size:25px");

    /* Set up more layout details */
    m_verticalLayout->addWidget(m_header);
    m_verticalLayout->setAlignment(Qt::AlignTop);
    m_mainLayout->setAlignment(Qt::AlignTop);
    m_leftSide->setAlignment(Qt::AlignTop);
    m_contentArea->setAlignment(Qt::AlignTop);

    /* Set the No DTCs present message */
    m_noDTCMsg = new QLabel(tr("<strong>Good News!</strong> No DTC Codes were found.<br />If you would like to check again, tap the Check ECU button."));

    /* Create the label for the welcome/introduction message */
    QLabel * welcome = new QLabel(tr("In here you can view any Diagnostic Trouble Codes (DTCs) that are present on your ECU. <br />"));

    /* Add the welcome widget to the appropiate layout */
    m_verticalLayout->addWidget(welcome);

    /* Create the resetted MIL message label */
    m_milResetMsg = new QLabel(tr("<strong>MIL Successfully Reset</strong><br />All DTCs were removed including freeze frame data and the MIL reset."));

    /* More layout stuff */
    m_mainLayout->addLayout(m_verticalLayout);
    m_verticalLayout->addLayout(m_contentArea);
    m_contentArea->addLayout(m_leftSide);
    m_contentArea->addLayout(m_rightSide);
    m_verticalLayout->addStretch();
    m_verticalLayout->addLayout(m_buttonsTop);

if(m_kernel) // [LA]
{displayNoDTCs();

    if (m_kernel->getNumCodes() > 0)
    {
        if (m_kernel->checkMil())
        {
            /* If there are codes present, then set the MIL on and set the reset MIL button to active */
            setMilPicOn();
            m_resetMilButton->setEnabled(true);
        }

        setupDTCTable(); /* Set up the DTC table to display the codes found */
    }
    else
    {

        if (!m_kernel->checkMil())
        {
            /* If MIL is off disable the button */
            m_resetMilButton->setEnabled(false);            
        }

        /* If no codes present set up table to invisible and show no codes. */
        displayNoDTCs();
    }
}

    /* Set the main layout */
    setLayout(m_mainLayout);
}

void DiagnosticsWidget::setMilPicOn()
{
    /* Set the picture for MIL on */
    engineMilPic->setPixmap(QPixmap(engineMilOnPic));
}

void DiagnosticsWidget::setMilPicOff()
{
    /* Set the picture for MIL off */
    engineMilPic->setPixmap(QPixmap(engineMilOffPic));
}

void DiagnosticsWidget::resetMIL()
{
    /*
        This is the slot that is called when user clicks the reset MIL button if it's active.
        In here we send a command to reset the MIL and clear codes
    */

    if (m_kernel->isMonitoring())
    {
        /* Don't continue if Automon is currently doing something else with the serial thread */
        emit changeStatus(tr("Automon is currently busy with another task. Stop this first"));
        return;
    }

    /* Display the Reset MIL Prompt box */

    int ret = m_resetMilPromptBox->exec();

    if (ret == QMessageBox::Ok)
    {
        /* If in here, the user clicked OK to reset MIL and clear DTCs */

        m_resetMilButton->setEnabled(false); /* Disable reset MIL button for now */

        if (m_kernel->checkMil())
        {
            /* Only if MIL set. Double check */

            emit changeStatus(tr("Resetting MIL and clearing diagnostics codes"));

            /* Call the kernel function to clear the MIL and delete the DTCs */
            m_kernel->resetMilAndClearCodes();

            /* Refresh DTC info now */
            m_kernel->refreshDTCInformation();

            if (m_kernel->getNumCodes() == 0)
            {
                /* Should get in here as codes now gone. */
                emit changeStatus(tr("MIL successfully reset and all DTCs cleared"));
                displayMilResetMsg();
                setMilPicOff();
            }
            else
            {
                /* By right we should have 0 DTCs now since Mil off but could come on if car messed up */
                setupDTCTable();

                if (m_kernel->checkMil())
                {
                    setMilPicOn();
                    emit changeStatus(tr("Warning - The MIL is still on after a reset. Needs attention"));
                    m_resetMilButton->setEnabled(true);
                }
            }
        }
    }
    else
    {
        /* User clicked cancel on the prompt to confirm deletion of DTCs and resetting of the MIL */
        emit changeStatus(tr("Resetting of MIL canceled"));
    }
}

void DiagnosticsWidget::checkECU()
{
    /* This is the slot that is called when user clicks check ECU */

    if (m_kernel->isMonitoring())
    {
        /* Don't continue if Automon is busy with something else */
        emit changeStatus(tr("Automon is currently busy with another task. Stop this first"));
        return;
    }

    /* Disable the checkButton for now */
    m_checkButton->setEnabled(false);

    emit changeStatus(tr("Checking ECU for new DTCs"));

    /* Call the Kernel's refresh DTC information method that will recheck for DTC codes and update list */
    m_kernel->refreshDTCInformation();

    emit changeStatus(tr("Communication Received"));

    if (m_kernel->getNumCodes() > 0 && m_kernel->checkMil())
    {
        /* Codes were found since Automon was started. Set message and update DTC table */
        emit changeStatus(tr("MIL light is *ON* and DTCs found! - Displaying..."));
        setupDTCTable();
        setMilPicOn();
        m_resetMilButton->setEnabled(true);
    }
    else if (m_kernel->getNumCodes() > 0)
    {
        setupDTCTable();
        emit changeStatus(tr("DTCs found but MIL is off."));
    }
    else
    {
        displayNoDTCs();
        emit changeStatus(tr("No DTCs found and MIL off!"));
    }

    /* Set button active again so they can check again */
    m_checkButton->setEnabled(true);
}

void DiagnosticsWidget::setupDTCTable()
{
    /* This method is responsible for loading up a table of DTCs present */

    /* Get list of all DTCs currently in ECU */
    QList<DTC*> DTCs = m_kernel->getDTCs();

    /* Set the column count of table to 2, one for DTC code and another for the english meaning */
    m_tableList->setColumnCount(2);

    /* Set the table row count to that of how many DTCs present */
    m_tableList->setRowCount(DTCs.size());

    /* Set table header labels */
    QStringList headerItems;
    headerItems.append("DTC Code");
    headerItems.append("English Meaning");

    m_tableList->setHorizontalHeaderLabels(headerItems);

    /* Iterate through each DTC adding it to the DTC Table List */
    for(int i = 0; i < DTCs.size(); i++)
    {
        /* Create the code cell item */
        QTableWidgetItem *codeItem = new QTableWidgetItem(DTCs.at(i)->getCode());

        /* Set it to enabled and selectable only. No editing allowed */
        codeItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

        /* Set it's position in the table */
        m_tableList->setItem(i, 0, codeItem);

        /* Do the same as above for the english meaning */
        QTableWidgetItem *englishMeaningItem = new QTableWidgetItem(DTCs.at(i)->getEnglishMeaning());
        englishMeaningItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        m_tableList->setItem(i, 1, englishMeaningItem);
    }

    /* Update table widths and heights */

    m_tableList->setColumnWidth(0,100);
    m_tableList->setColumnWidth(1,400);

    /* Set to single selection only to prevent user selection multiple rows */
    m_tableList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableList->setSelectionBehavior(QAbstractItemView::SelectRows);

    /* Remove the no DTC message and reset Msg if pressent */
    m_leftSide->removeWidget(m_noDTCMsg);
    m_leftSide->removeWidget(m_milResetMsg);

    /* Add the table widget */
    m_leftSide->addWidget(m_tableList);

    /* Set it to visible */
    m_tableList->setVisible(true);

}

void DiagnosticsWidget::displayMilResetMsg()
{
    /* This simply displays the reset MIL successful message, taking away the DTC table if present */

    m_leftSide->removeWidget(m_tableList);

    if (m_tableList->isVisible())
        m_tableList->setVisible(false);

    m_leftSide->addWidget(m_milResetMsg);

}

void DiagnosticsWidget::displayNoDTCs()
{
    /* This simply displays the No DTCs present message and taking away the DTC table if present */
    m_leftSide->removeWidget(m_tableList);
    m_leftSide->removeWidget(m_milResetMsg);

    if (m_tableList->isVisible())
        m_tableList->setVisible(false);

    m_leftSide->addWidget(m_noDTCMsg);
}
