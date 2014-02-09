#ifndef SENSOR_H
#define SENSOR_H

/*
    ==================================================================================================
    |  sensor.h                                                                                      |
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

    This class, Sensor is a base class used by all Sensor objects. It is inherited from Command class
    so it can be passed to the serial I/O thread for processing. It contains a signal that is emitted
    when it's value changes. More details are in the implementation file.
*/

#include <QString>
#include "command.h"

class QVariant;

namespace AutomonKernel
{
    class Sensor : public Command
    {
        Q_OBJECT

    public:
        Sensor();
        enum UNITS { MPH, RPM, DEGREES, PERCENTAGE, KPA, VOLTS, SECONDS, MINUTES, GS, NA };
        virtual ~Sensor() {}
        virtual double convertResult();
        void setMax(double max);
        void setMin(double min);
        double getMin();
        double getMax();
        double getResult() const;
        void setUnits(UNITS resultUnits);
        void setFrequency(int frequency);
        bool isTurn();
        float getAvgRefreshRate();
        virtual void setBuffer(QString bufferResponse);
        virtual void setResult();
        void setSupported(bool isSupported);
        bool isSupported();
        int getChangeTimes();
        void resetSensor();
        QString getName();
        QString getPid();

    signals:
        void changeOccurred(double);
        void outOfRangeError(QString);

    protected:
        bool checkIfOutOfRange(double value);
        bool m_wasOutOfRange;
        double m_maxVal;
        double m_minVal;
        UNITS m_resultUnits;
        double m_result;
        QList<int> m_returnedBytes;

    private:
        void validateSensorData(QString buffer);
        bool m_isSupported;
        int m_maxFrequency;
        int m_currentFrequency;
        double m_avgRefreshRate;
        double m_instRefreshRate;
        double m_lastTime;
        int m_changeTimes;
        timeval m_timeVal;

    };
}

#endif // SENSOR_H
