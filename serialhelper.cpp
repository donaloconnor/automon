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

#include <QMutex>
#include "automon.h"
#ifndef Q_OS_ANDROID // [LA] Cross-compile to get this up and running
#include <QSerialPortInfo>
#endif

using namespace AutomonKernel;

SerialHelper::SerialHelper(QString port)
{
    /* Create a QSerialPort object passing in the parameters that the ELM327 communicates with */
    m_connection = new QSerialPort(port);

    // Override the default port, if it finds something connected to a different port [LA]
    qDebug() << "**********Populating Serial Port(s)*************";

    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        qDebug() << "Name        : " << info.portName();
        qDebug() << "Description : " << info.description();
        qDebug() << "Product ID : "  << info.productIdentifier();
        qDebug() << "Manufacturer: " << info.manufacturer();

        if( info.manufacturer() == "FTDI")
            m_connection->setPort(info);
    }


    int result = m_connection->open(QIODevice::ReadWrite);
    if(result)
    {
        m_connection->setBaudRate(QSerialPort::Baud57600);
        m_connection->setBaudRate(QSerialPort::Baud38400);
        m_connection->setFlowControl(QSerialPort::NoFlowControl);
        m_connection->setParity(QSerialPort::NoParity);
        m_connection->setDataBits(QSerialPort::Data8);
        m_connection->setStopBits(QSerialPort::OneStop);
    //    m_connection->clear();
    //        connect(m_connection, SIGNAL(readyRead()), this, SLOT(onDataReceived()));
    //    m_connection->setTimeout(0,10);
    }
    /* m_stop is used to stop the monitoring thread running */
    m_stop = true;
    m_isMonitoring = false;

    /* Open a connection to the ELM327 */
//    int result = m_connection->open(QSerialPort::ReadWrite);

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
            m_connection->write(command.toLatin1(), command.length());

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
    m_connection->write(message.toLatin1(),message.length());

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
