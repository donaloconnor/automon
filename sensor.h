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

#ifndef SENSOR_H
#define SENSOR_H

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
