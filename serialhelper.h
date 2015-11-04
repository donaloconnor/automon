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

#ifndef SERIALHELPER_H
#define SERIALHELPER_H

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
