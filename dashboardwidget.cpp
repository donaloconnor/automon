/*
    ==================================================================================================
    |  dashboardwidget.cpp                                                                           |
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

    This is the implementation file of the DashboardWidget. More details are below.
*/

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

#include "dashboardwidget.h"
#include "S5WDial.h" /* This is the S5WDial class that is third party and modified by me */

DashboardWidget::DashboardWidget(Automon * kernel, QWidget * parent)
        : QWidget(parent), m_kernel(kernel)
{
    /* This is a variable that is used to signify if the dashboard is running or not. Used by the button */
    m_started = false;

    /* Create layouts */
    m_mainLayout = new QHBoxLayout(this);
    m_verticalLayout = new QVBoxLayout();

    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->setAlignment(Qt::AlignCenter);

    m_dialLayouts = new QHBoxLayout();
    m_dialLayouts->setAlignment(Qt::AlignCenter);
    m_verticalLayout->setAlignment(Qt::AlignCenter);

    /* Create the button that will be used to start and stop the dashboard dials */
    m_startStopButton = new QPushButton(tr("Start Dashboard"));

    /* Connect the clicked signal to our custom slot */
    connect(m_startStopButton, SIGNAL(clicked()), this, SLOT(startStopDashboard()));

    m_startStopButton->setFixedWidth(200);

    /*
        Create the rev dial with it's properties. We have a multiplier that is used to divide in side.
        In the rev dial example, we passed 1000, so for 8500 revs, we map to 8.5 on the dial. 8500/1000
    */

    m_revDial = new S5WDial(this, 1000);
    m_revDial->setLabel(tr("x1000r/min"));
    m_revDial->setMinimum(0);
    m_revDial->setMaximum(12);
    m_revDial->setTickInterval(1);
    m_revDial->setGreenBand(3,6.5);
    m_revDial->setRedBand(6.5,12);
    m_revDial->setFixedHeight(250);
    m_revDial->setFixedWidth(250);

    /* Set the speed dial and associated properties. */
    m_speedDial = new S5WDial(this, 1);
    m_speedDial->setLabel(tr("km/h"));
    m_speedDial->setMinimum(0);
    m_speedDial->setMaximum(255);
    m_speedDial->setTickInterval(10);
    m_speedDial->setGreenBand(70,200);
    m_speedDial->setRedBand(150,255);
    m_speedDial->setFixedHeight(250);
    m_speedDial->setFixedWidth(250);

    /* Add widgets to layouts */
    m_dialLayouts->addStretch();
    m_dialLayouts->addWidget(m_revDial);
    m_dialLayouts->addSpacing(30);
    m_dialLayouts->addWidget(m_speedDial);
    m_dialLayouts->addStretch();


    /* Create header label with styling */
    m_header = new QLabel(tr("Digital Dashboard"));
    m_header->setStyleSheet("color: #ace413; font-size:25px");

    /* Add layout to main layout */
    m_mainLayout->addLayout(m_verticalLayout);

    m_verticalLayout->addWidget(m_header);
    m_verticalLayout->addSpacing(10);
    m_verticalLayout->addLayout(m_dialLayouts);

    m_buttonLayout->addWidget(m_startStopButton);
    m_verticalLayout->addLayout(m_buttonLayout);
    m_verticalLayout->setAlignment(Qt::AlignTop);

    m_mainLayout->setAlignment(Qt::AlignTop);
    setLayout(m_mainLayout);    /* Set to main layout */
}

void DashboardWidget::startStopDashboard()
{
    /*
        This is the slot that is called when the user clicks the Start/Stop Dashboard button.
        It is responsible for adding the sensors to the serial thread via the Kernel interface
    */


    if (!m_started)
    {
        /* If in here, we are starting the dashboard */

        /* Check to ensure that the Serial I/O thread isn't running already */
        if (m_kernel->isMonitoring())
        {
            emit changeStatus(tr("Automon is currently busy with another task. Stop this first"));
            return;
        }

        /* Disable clicking of the start/stop button for now */
        m_startStopButton->setEnabled(false);

        bool success = true;

        /* Remove all sensors incase any in the serial I/O thread */
        m_kernel->removeAllActiveSensors();

        /* Get the two sensors we need */
        Sensor * rpmSensor = m_kernel->getSensorByCommand("010C");
        Sensor * kphSensor = m_kernel->getSensorByCommand("010D");

        /* Add the rpmSensor by passing the pointer */
        if (m_kernel->addActiveSensor(rpmSensor))
            m_kernel->connectSensorToSlot(rpmSensor, m_revDial);
        else
            success = false; /* Sensor couldn't be added. */

        /* Add the speedSensor by passing its pointer */
        if (m_kernel->addActiveSensor(kphSensor))
            m_kernel->connectSensorToSlot(kphSensor, m_speedDial);
        else
            success = false; /* Speed sensor couldn't be added */

        if (!success)
        {
            /* If in here, one or more of the sensors could not be added */

            /* Remove any sensors if any were added */
            m_kernel->removeAllActiveSensors();

            /* Disconnect any if already connected */
            m_kernel->disconnectSensorFromSlot(rpmSensor, m_revDial);
            m_kernel->disconnectSensorFromSlot(kphSensor, m_speedDial);
            emit changeStatus(tr("RPM or KPH Sensor could not be added to monitor. Exiting!"));
            return;
        }

        /* If at this pointer, sensors were added successfully */

        /* Set the start/stop button to Stop Dashboard since it will be running now */
        m_startStopButton->setText(tr("Stop Dashboard"));

        if (!m_kernel->startMonitoring())
        {
            /* We should not get in here */
            emit changeStatus(tr("Failed to start serial thread. This is an exception! Restart the device."));
            return;
        }

        /* Monitoring has started now, set the button to enabled again so they can stop the dashboard */
        m_startStopButton->setEnabled(true);
        emit changeStatus(tr("Dashboard Started!"));

        /* Set m_started to true so next time we know to go into stop dashboard section of code. (What follows) */
        m_started = true;

        return ;
    }
    else
    {
        /* If in here, dashboard was running and we now stopping it */
        m_startStopButton->setEnabled(false);

        /* Stop the serial I/O thread */
        m_kernel->stopMonitoring();

        /* Get sensor pointers */
        Sensor * rpmSensor = m_kernel->getSensorByCommand("010C");
        Sensor * kphSensor = m_kernel->getSensorByCommand("010D");

        /* Disconnect sensors from our slot */
        m_kernel->disconnectSensorFromSlot(rpmSensor, m_revDial);
        m_kernel->disconnectSensorFromSlot(kphSensor, m_speedDial);

        /* Remove all sensors from Serial Thread */
        m_kernel->removeAllActiveSensors();

        /* Update button text and re enable */
        m_startStopButton->setText(tr("Start Monitoring"));
        m_startStopButton->setEnabled(true);

        /* Set this variable to true so next time we know we want to start the dashboard */
        m_started = false;

        /* Reset dials to 0 */
        m_revDial->setValue(0);
        m_speedDial->setValue(0);

        emit changeStatus(tr("Dashboard Stopped!"));
        return;
    }
}
