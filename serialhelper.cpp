/*
    ==================================================================================================
    |  serialhelper.cpp                                                                              |
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

    This is the implementation file of the SerialHelper class. More details are below.

*/

#include <QMutex>
#include "automon.h"

using namespace AutomonKernel;

SerialHelper::SerialHelper(QString port)
{
    /* Create a QExtSerialPort object passing in the paramaters that the ELM327 communicates with */
    m_connection = new QextSerialPort(port);
    m_connection->setBaudRate(BAUD38400);
    m_connection->setDataBits(DATA_8);
    m_connection->setParity(PAR_NONE);
    m_connection->setStopBits(STOP_1);
    m_connection->setFlowControl(FLOW_OFF);
    m_connection->setTimeout(0,10);

    /* m_stop is used to stop the monitoring thread running */
    m_stop = true;
    m_isMonitoring = false;

    /* Open a connection to the ELM327 */
    int result = m_connection->open(QextSerialPort::ReadWrite);

    /* If could not connect, major problem! Throw exception */
    if (!result)
        throw serialio_exception();

#ifdef DEBUGAUTOMON

    if (result)
        qDebug("Successfully Connected to Serial Port");
    else
        qWarning("Serial Could not be opened");

#endif


}

void SerialHelper::setMonitoring(bool monitoring)
{
    /* Set the monitoring variable so we know the Serial thread running */
    m_isMonitoring = monitoring;
}

SerialHelper::~SerialHelper()
{
    /* SerialHelper destructor. We have to clean up the serial connection object */
    if(m_connection->isOpen())
        m_connection->close();

    /* Delete memory from heap */
    delete m_connection;

#ifdef DEBUGAUTOMON
    qDebug("Closed SerialHelper");
#endif

}

void SerialHelper::clearReadBuffer()
{
    /* Read any trash that's currently in the serial input buffer */
    QString rubbish = m_connection->readAll();  /* Read Trash */
}

void SerialHelper::run()
{
    /* This is the code that runs when the Serial thread started */

    QTime t; /* Used for timeout purposes */
    int i = 0;
    QString command;
    int bytes = 0;
    int size = 0;

    /* Temporary buffers */
    char buffer[1024];
    char tmpBuf[1024];

    m_stop = false;
    m_isMonitoring = true;

#ifdef DEBUGAUTOMON
    qDebug("Started serial thread");
#endif

    while (!m_stop && m_activeSensors.size() > 0)
    {
        /* Iterate through each sensors, while there are sensors to monitor and we are not stopped */

        t.start(); /* Start the clock */

        if(m_activeSensors[i]->isTurn())
        {
            /* Only process sensor if it is it's turn. (Frequency) */

            /*
                Each sensor can have an expected bytes interger assigned that gives the ELM327 a hint of how many bytes
                to receive. This speeds up the waiting time from the ELM327
            */
            int expectedBytes = m_activeSensors[i]->getExpectedBytes();

            /* Create the command to send to the ELM327 */
            command = m_activeSensors[i]->getCommand() + " " + (!expectedBytes ? "" : QString::number(expectedBytes)) + "\x0D";

            /* Send command to ELM327 */
            m_connection->write(command.toAscii(), command.length());

            /* Get the amount of bytes in the serial input buffer */
            bytes = m_connection->bytesAvailable();

            size = 0;
            buffer[0] = '\0';
            tmpBuf[0] = '\0';

            while(strchr(buffer, '>') == NULL)
            {
                /* Keep gathering bytes until we hit the prompt character at which point we know we're at end of response */
                bytes = m_connection->bytesAvailable();

                if (bytes > 0)
                {
                    /* Bytes available in input buffer */
                    m_connection->read(tmpBuf, bytes);

                    buffer[size] = '\0';
                    tmpBuf[bytes] = '\0';

                    /* Concatenate response to previous response to build up full final response */
                    strcat(buffer,tmpBuf);
                            size += bytes;

                }

                if (t.elapsed() > 2500)
                {
                    /*
                        If we still receiving responses after 2.5 secondsor waiting on the prompt character, then
                        something wrong. Exit out of the loop to prevent hang
                    */
#ifdef DEBUGAUTOMON
                    qDebug("Timeout!");
#endif
                    /* Clear out all rubbish in the input buffer */
                    QString rubbish = m_connection->readAll();  /* Read Trash */

                    /* Skip out of this loop */
                    goto timeout;
                }
                msleep(1); /* Insert a little sleep to prevent CPU utilization going up, (even tho Linux is pre emptive */
            }

        timeout:

            buffer[size] = '\0';

            /* Set the returned response from ELM into the sensor's buffer. The sensor will look after rest such as
               sending signal updates etc.
            */

            m_activeSensors[i]->setBuffer(QString(buffer));

#ifdef DEBUGAUTOMON
           qDebug() << "Received " << QString::number(m_activeSensors[i]->getBuffer().size()) << " Bytes";
#endif

            msleep(1);

#ifdef DEBUGAUTOMON
            qDebug("Time Elapsed :%d", t.elapsed());
#endif

        }

        /* Move on to next sensor */
        if (i == m_activeSensors.size() - 1)
            i = 0;
        else
            i++;
    }
}

void SerialHelper::removeAllActiveSensors()
{
    /* Clear all sensors from list */
    m_activeSensors.clear();
}

bool SerialHelper::addActiveSensor(Sensor * sensor)
{
    /*
        This method is reponsible for accepting a sensor pointer and adding it to the list of sensors
        to monitor
    */

#ifdef DEBUGAUTOMON
    qDebug("Adding Realtime Sensor to Active Sensors");
#endif

    m_activeSensors << sensor;
    return true;
}

bool SerialHelper::removeActiveSensorByCommand(QString command)
{
    /* This method allows one to remove a sensor from the monitoring list by specifying it's command/pid */

    for (int i = 0; i < m_activeSensors.size(); i++)
    {
        if (command.compare(m_activeSensors.at(i)->getCommand()) == 0)
        {
            /* Found the sensor we looking for so remove it from list and exit */
            m_activeSensors.removeAt(i);
            return true;
        }
    }
    
    return false;
}

bool SerialHelper::isMonitoring()
{
    /* Return a bool to suggest if we monitoring or not */
    return m_isMonitoring;
}

bool SerialHelper::sendCommand(Command & command, int timeout)
{
    /*
        This method allows one to send commands to the ELM327. It looks after updating the command object's buffer
        to the response as well. I've included mutex's so incase multiple threads use this, they get blocked as only
        one thread can send a command at a time
    */

    /* Create a lock for current thread */
    m_mutexLock.lock();

    if (m_isMonitoring)
    {
        /* Cannot send a command if monitoring is active! */
        monitoring_exception e;
        throw e;
    }

    QTime t;
    QByteArray rubbish = m_connection->readAll();

    /* Create command to send to ELM327 */
    QString message = command.getCommand()+"\x0D";

    /* Send command */
    m_connection->write(message.toAscii(),message.length());

    /* Sleeep for a few ms to give chance for buffer to fill */
    msleep(10);

    char buffer[1024];
    char tmpBuf[1024];
    int size = 0;
    int bytes = 0;

    buffer[0] = '\0';
    tmpBuf[0] = '\0';

    t.start(); /* Start a clock so we can check if time out */

    while(strchr(buffer, '>') == NULL)
    {
        /* Keep looping until we hit the > character in which case we are at end of response */

        bytes = m_connection->bytesAvailable();

        if (bytes > 0)
        {
            /* Bytes available in input buffer of serial device so read them */
            m_connection->read(tmpBuf, bytes);

            buffer[size] = '\0';
            tmpBuf[bytes] = '\0';

            /* Add this current buffer to previous to construct over all response */
            strcat(buffer,tmpBuf);
                    size += bytes;
        }
        else
            msleep(1);

        if (t.elapsed() > timeout)
        {
            /* If our clock has gone above the time out , then something wrong! Exit out of loop by doing jump */
#ifdef DEBUGAUTOMON
            qDebug() << "Timeout";
#endif
            goto timeout;
        }

    }

timeout:
    /* Either we finished successfully or we timed out */
    buffer[size] = '\0';

    /* Set the command's buffer so the calling method can now read the response from ELM327 */
    command.setBuffer(QString(buffer));

    /* Unlock the mutex again so another thread can use this method */
    m_mutexLock.unlock();

    return true;
}
