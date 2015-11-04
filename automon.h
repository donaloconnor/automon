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

#ifndef AUTOMON_H
#define AUTOMON_H

#include <QLCDNumber>

#include "command.h"
#include "coolanttempsensor.h"
#include "dtc.h"
#include "dtchelper.h"
#include "sensor.h"
#include "serialhelper.h"
#include "enginerpm.h"
#include "engineruntime.h"
#include "vehiclespeed.h"
#include "fuelpressure.h"
#include "throttleposition.h"
#include "fuellevelinput.h"
#include "mafairflowrate.h"
#include "commandedegr.h"
#include "o2voltage.h"
#include "rule.h"
#ifdef Q_OS_MACX
#include <err.h>
#else
#include "error.h"
#endif
#include "exceptions.h"

/* TURN OFF DEBUG */
#define DEBUGAUTOMON 1  /* Specifies if debug messages will be printed or not */

#define TURNOFFECHO 1    /* Warning don't remove this. It will probably upset formulas that work on fact no echo */
#define ADAPTIVETIMING 1 /* If set, adaptive timing will be set to speed up communication with ECU. Better to let enabled */

//#define RULEFILE "/home/eclipse/rules"
//#define DTCCODEFILE "/home/eclipse/codes"

//#define RULEFILE "rules"        /* Location of file for storing of user defined rules */
//#define DTCCODEFILE "codes"     /* Location of the DTC code description file */

// [LA]
#define RULEFILE ":/files/rules"        /* Location of file for storing of user defined rules */
#define DTCCODEFILE ":/files/codes"     /* Location of the DTC code description file */

namespace AutomonKernel
{
    class Automon : public QObject
    {
        Q_OBJECT

    public:
        Automon(QString port = "/dev/ttyUSB0");
        ~Automon();
        QString getVin();
        QString getOBDStandardType();
        QString getOBDProtocol();
        QString getElmVersion();
        QString getVoltage() const;
        QList<Sensor*> getFreezeFrame() const;
        QList<Sensor*> getAllSensors() const;
        QList<DTC*> getDTCs() const;
        bool sendCommand(Command & command);
        void addSensor(Sensor * newSensor);
        bool addActiveSensorByCommand(QString command);
        bool removeActiveSensorByCommand(QString command);
        bool startMonitoring();
        void stopMonitoring();
        Sensor * getActiveSensorByCommand(QString command) const;
        Sensor * getSensorByCommand(QString command) const;
        bool connectSensorToSlot(Sensor * sender,QObject * receiver) const;
        bool disconnectSensorFromSlot(Sensor * sender, QObject * receiver) const;
        bool connectToErrorToSlot(QObject * receiver);
        bool setSensorFrequency(Sensor * sensor, int frequency) const;
        bool addActiveSensor(Sensor * sensor);
        bool checkMil() const;
        int getNumCodes();
        bool resetMilAndClearCodes();
        bool init();
        bool testElmConnectivity() const;
        bool refreshDTCInformation() const;
        static QList<int> getBytes(Command & command);
        static QString cleanResponse(QString response);
        bool saveRuleList();
        bool loadRuleList();
        QStringList getRuleList();
        void addRuleString(QString rule);
        void removeRuleString(QString rule);
        QString convertRuleToEnglish(QString rule) const;
        void removeRuleEnglishMeaningString(QString rule);
        QStringList extractSensorsFromRule(QString & rule) const;
        void removeAllActiveSensors() const;
        bool isMonitoring() const;

    signals:
        void sendErrorMessage(QString); /* Used to send an error message to connected Slots */
        /* Used to send updates of progress during init stages to splash screen */
        void updateStatus(const QString & message, int alignment = Qt::AlignLeft, const QColor & color = Qt::black);

    public slots:
        void receiveErrorMessage(QString);

    private:
        bool initialiseBus();
        void loadSensors();
        void updateSensorSupport();

        QStringList m_ruleList;
        SerialHelper * m_serialHelper;
        DTCHelper * m_dtcHelper;
        QList<Sensor*> m_sensors;
        QList<Sensor*> m_activeSensors;
        QList<Sensor*> freezeFrame;
        QList<DTC*> m_dtcs;
        bool m_milOn;
        int m_numberDTCs;

        /*
           Constant cache data. First time, ECU will be queried for this. But further requests will
           be from these set variables to reduce communication with ECU
        */

        QString m_vinNumber;
        QString m_protocol;
        QString m_standardType;
        QString m_elmVersion;

        /*
            This variable is used as a check if the serial I/O thread is monitoring.
            Used for the main Automon application to prevent two tasks running together
        */
        bool m_isMonitoring;

    };
}
#endif // AUTOMON_H
