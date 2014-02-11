/*
    ==================================================================================================
    |  accelerationtestwidget.cpp                                                                    |
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

    This is the implementation file of the class AccelerationTestWidget. Details are blow
*/

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLCDNumber>
#include <QPushButton>
#include <QSpinBox>
#include <QTime>
#include <QGridLayout>
#include "accelerationtestwidget.h"


AccelerationTestWidget::AccelerationTestWidget(Automon * kernel, QWidget * parent)
        : QWidget(parent), m_kernel(kernel)
{

    /*
        Create all grid layouts. The main grid is attached the the full widget hince it's parent is this.
        All the rest will form child/parent relationships with eachother so object distruction will be handled
        automatically by QT.
    */

    m_mainLayout = new QHBoxLayout(this);
    m_verticalLayout = new QVBoxLayout();
    QGridLayout * gridLayout = new QGridLayout();
    QHBoxLayout * kphLayout = new QHBoxLayout();
    QHBoxLayout * timeElapsedLayout = new QHBoxLayout();
    QHBoxLayout * destinationSpeedLayout = new QHBoxLayout();
    QHBoxLayout * startTestLayout = new QHBoxLayout();


    /* The following spin box allows user to specify speed for accelleration test to reach. Increments of 5kph */
    m_destinationSpeed = new QSpinBox();

    /* The following QLCDNumbers display a digital number for the current speed and elapsed time of acceleration test */
    m_speed = new QLCDNumber();
    m_accelerationTimeDisplay = new QLCDNumber();

    /* Simple push button to start AND stop the acceleration test if it is running already */
    m_startTest = new QPushButton(tr("Start Acceleration Test!"));

    /* Create all the labels that are visible to the user */
    m_header = new QLabel(tr("Acceleration Test"));
    QLabel * introduction = new QLabel(tr("The acceleration test allows you to test how quick your car can reach a speed! <br /><br /><strong>Instructions</strong><br />Simply select your destination speed, get ready on track, click start and when ready put the foot down!"));
    QLabel * kphDisplayLabel = new QLabel(tr("Speed (KPH): "));
    QLabel * timeElapsedLabel = new QLabel(tr("Time: (Seconds)"));
    QLabel * destinationSpeedLabel = new QLabel(tr("Speed to Reach: "));

    /*
        This QTime object is used to catch the elapsed time from the user moving car > 1kph to destinatino speed
        It has no parent object so will have to be deleted in the distructor
    */
    m_time = new QTime();

    /* Set the destination speed spinbox properties */
    m_destinationSpeed->setFixedWidth(250);
    m_destinationSpeed->setSingleStep(5);
    m_destinationSpeed->setMaximum(255);
    m_destinationSpeed->setValue(100);
    m_destinationSpeed->setSuffix(QString(tr(" KM")));

    /* Set the Current Speed and Elapsed Acceleration Time LCD number properties */
    m_speed->setSegmentStyle(QLCDNumber::Filled);
    m_speed->setStyleSheet("color: beige");
    m_speed->setFixedSize(QSize(200,100));

    m_accelerationTimeDisplay->setSegmentStyle(QLCDNumber::Filled);
    m_accelerationTimeDisplay->setFixedSize(QSize(200,100));
    m_accelerationTimeDisplay->setStyleSheet("color: beige");

    /* Set the header style to Green and larger font */
    m_header->setStyleSheet("color: #ace413; font-size:25px");

    /* Set the start/stop push button width */
    m_startTest->setFixedWidth(200);

    /* Connect the start/stop test pushbutton's clicked() signal to our defined slot in this widget */
    connect(m_startTest, SIGNAL(clicked()), this, SLOT(startTest()));

    /* Set some label styles */
    kphDisplayLabel->setStyleSheet("font-size: 20px;");
    kphDisplayLabel->setFixedWidth(160);

    timeElapsedLabel->setStyleSheet("font-size: 20px;");
    timeElapsedLabel->setFixedWidth(160);

    destinationSpeedLabel->setStyleSheet("font-size: 20px;");

    /* Set up the layouts */

    m_mainLayout->addLayout(m_verticalLayout);

    /* Add header widget first and introduction message along with a spacing */
    m_verticalLayout->addWidget(m_header);
    m_verticalLayout->addWidget(introduction);
    m_verticalLayout->addStretch();

    m_verticalLayout->addLayout(gridLayout);

    kphLayout->addWidget(kphDisplayLabel);
    kphLayout->addWidget(m_speed);
    kphLayout->addStretch();

    timeElapsedLayout->addWidget(timeElapsedLabel);
    timeElapsedLayout->addWidget(m_accelerationTimeDisplay);
    timeElapsedLayout->addStretch();

    startTestLayout->addWidget(m_startTest);
    startTestLayout->setAlignment(Qt::AlignCenter);

    destinationSpeedLayout->addWidget(destinationSpeedLabel);
    destinationSpeedLayout->addWidget(m_destinationSpeed);
    gridLayout->addLayout(kphLayout,0,0);
    gridLayout->addLayout(timeElapsedLayout,1,0);
    gridLayout->addLayout(destinationSpeedLayout,0,1);
    gridLayout->addLayout(startTestLayout,1,1);

    m_verticalLayout->setAlignment(Qt::AlignTop);
    m_mainLayout->setAlignment(Qt::AlignTop);

    /* Set the main overall layout to this widget */
    setLayout(m_mainLayout);

    /* Initialise some variables */
    m_carMoving = false;
    /* When car is stopped, misreadings could indicate car moving 1kph. This threshold specifies that */
    m_parkThreshold = 1;
}

AccelerationTestWidget::~AccelerationTestWidget()
{
    /* Delete any variables created on Heap that are not part of the parent/child hierarchy */
    delete(m_time);
}

void AccelerationTestWidget::display(double speed)
{
    /*
        This is a slot that we have defined that our sensor will connect it's signal to.
        The Automon kernel provides us a function to connect a object to a sensor. That object
        has to have defined, a slot of type void display(double).
        Each time the sensor value changes, this slot will receive that value.
        It is here we define the logic of what happens with this value
    */

    /* First check if the car is moving before test starts. IE: teststarted false, speed is > 0 + park threshold */
    if (!m_testStarted && !m_carMoving && speed > 0 + m_parkThreshold)
        emit changeStatus(tr("Car moving already. Waiting for you to slow to 0mph and start accelerating"));
    else
    {
        /*
            If we are in here, the test is running and we are waiting for the car to reach destination speed.
            We may still be under the stating speed criteria however.
        */

        m_testStarted = true;
        m_speed->display(speed); /* Update the current speed QLCDNumber to the current speed of the vehicle */

        /* Check if car is actually > than parking threshold and this is our first time, ie: m_carMoving = false */
        if (!m_carMoving && speed > 0 + m_parkThreshold && !m_testFinished)
        {
            /* If in here, car has started to move so start the test. Only in here once */
            m_carMoving = true;

            m_time->start(); /* Start our timer. This timer will keep counting from here now */

            emit changeStatus(tr("Timer started. Timing until you reach "+ QString::number(m_destinationSpeed->value()).toLatin1() + "KPH"));
        }
        else if (m_carMoving && speed >= m_destinationSpeed->value())
        {
            /* If in here, we have hit our destination speed */
            m_carMoving = false; /* Turn this off to prevent going into previous if block */

            m_accelerationTime = m_time->elapsed(); /* Capture the elapsed time */
            m_kernel->stopMonitoring();             /* Stop the serial thread and ELM327 reading the speed from ECU */
            removeSensor();                         /* Remove the speed sensor, 010D */

            /*
                Display the elapsed time. Remember speed in milliseconds, so convert to seconds.
                Also set the precision so we don't end up with too accurate speeds
            */

            m_accelerationTimeDisplay->display(static_cast<double>(m_accelerationTime)/1000);

            emit changeStatus(tr("Acceleration test finished"));

            /* Reset all check variables */
            m_destinationSpeed->setEnabled(true);
            m_startTest->setEnabled(true);
            m_testFinished = true;
            m_startButtonClicked = false;

            /* Change the start/stop button text and change the style of the time so it green */
            m_startTest->setText(tr("Start Another Test!"));
            m_accelerationTimeDisplay->setStyleSheet("color: #ace413");
        }
    }
}

void AccelerationTestWidget::startTest()
{
    /*
        This slot is called when the user clicks the Start/Stop Acceleration test button.
        It is responsible for starting the acceleration test or stopping it if already running.
    */

    if (m_startButtonClicked)
    {
        /* If in here, the test was running so stop the test and do some cleaning up */

        m_kernel->stopMonitoring(); /* Stop the serial I/O thread */
        removeSensor();             /* Remove sensor from serial thread */

        /* Do some status updates and change text of start/stop button */
        emit changeStatus(tr("Acceleration Test Cancelled!"));
        m_startTest->setText(tr("Start Another Test!"));

        /* Do some cleaning up */
        m_startButtonClicked = false;
        m_testStarted = false;
        m_carMoving = false;

        /* Re enable the destination speed spin box */
        m_destinationSpeed->setEnabled(true);
    }
    else
    {
        /*
           If in here, we are starting the test. First of all ensure that some other widget is not
           already using the serial thread. If so abort starting this test letting the user know in the status
        */

        if (m_kernel->isMonitoring())
        {
            emit changeStatus(tr("Automon is currently busy with another task. Stop this first"));
            return;
        }

        /* If we done test before, the time would be green. Reset to 0 */
        m_accelerationTimeDisplay->setStyleSheet("color: beige");

        /* Some variable initialisation/clean up */
        m_testFinished = false;
        m_testStarted = false;

        /* Add the sensor to the serial I/O thread */
        setUpSensor();

        /* If we done a test before, the m_time would be further on than 0. Reset */
        m_time->restart();

        /* Reset LCD and time */
        m_accelerationTimeDisplay->display(0.0);
        m_accelerationTime = 0.0;
        m_speed->display(0);

        /* Start the serial I/O thread */
        if (!m_kernel->startMonitoring())
        {
            /* We should not get in here since the check was done above. Just incase however */
            emit changeStatus(tr("Failed to start serial thread. This is an exception! Restart the device."));
            return;
        }

        /* Disable the spinbox */
        m_destinationSpeed->setEnabled(false);

        /* Update label on Pushbutton to stop test */
        m_startTest->setText(tr("Stop Test"));

        emit changeStatus(tr("Test started. Waiting for you to start accelerating before timer starts"));

        /* Setting this means, next time we click button, we go into other section of code above to stop test */
        m_startButtonClicked = true;
    }
}

void AccelerationTestWidget::removeSensor()
{
    /*
      This method is responsible for removing the Speed sensor from the serial I/O thread
    */
    if (m_kernel->disconnectSensorFromSlot(m_kernel->getActiveSensorByCommand("010D"),this))
    {
        m_kernel->removeActiveSensorByCommand("010D");
    }
    else
        qDebug() << "Sensor not present";

}

void AccelerationTestWidget::setUpSensor()
{
    /*
      This method is responsible for adding the speed sensor to the serial I/O thread
    */

    if(m_kernel->addActiveSensorByCommand("010D"))
    {
        /* Connect the sensor to the slot in this method, display(double) */
        m_kernel->connectSensorToSlot(m_kernel->getActiveSensorByCommand("010D"),this);
        /* Set frequency so gets updated ASAP */
        m_kernel->setSensorFrequency(m_kernel->getActiveSensorByCommand("010D"), 1);
    }
    else
        qDebug() << "Command not supported by ECU";
}
