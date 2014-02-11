#ifndef SERIALHELPER_H
#define SERIALHELPER_H

/*
    ==================================================================================================
    |  serialhelper.h                                                                                |
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

    The SerialHelper class is one of the main critical classes of Automon. it is responsible for the
    communication between Automon and the ELM327. It uses the QExtSerialPort library to perform this communication.
    It handles the scheduling of monitoring sensors and sending commands to ELM327.

*/

#include <QThread>
#include <QMutex>
#include <QtSerialPort/QSerialPort> // [LA]
//#ifdef win32
//    #include <QtSerialPort/QtSerialPort>
//    #include <QtSerialPort/QSerialPortInfo>
//#else
//    #include
//    #include "QtSerialPort/qserialportinfo.h"
//#endif

#include "sensor.h"
#include "command.h"

namespace AutomonKernel
{
    class SerialHelper : public QThread
    {
    public:
        SerialHelper(QString port="/dev/ttyUSB0");
        ~SerialHelper();
        void run();
        bool addActiveSensor(Sensor * sensor);
        bool removeActiveSensorByCommand(QString command);
        bool isMonitoring();
        bool sendCommand(Command & command, int timeout = 5000);
        void clearReadBuffer();
        void setMonitoring(bool);
        void removeAllActiveSensors();

    private:
        QSerialPort * m_connection;
        QList<Sensor*> m_activeSensors;
        bool m_isMonitoring;
        bool m_isPaused;
        bool m_pausedStarted;
        bool m_stop;
        QMutex m_mutexLock;



    };
}
#endif // SERIALHELPER_H
