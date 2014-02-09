/*
    ==================================================================================================
    |  sensor.cpp                                                                                    |
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

    This is the implementation file of the Sensor object. More details below.
*/

#include <QRegExp>

#include "automon.h"

using namespace AutomonKernel;

void Sensor::setFrequency(int frequency)
{
    m_maxFrequency = frequency;
}

bool Sensor::isTurn()
{
    /*
        This method is responsible for checking if it is time to give this sensor access to the ELM.
        Depending on the frequency of the sensor, this method will determine how often it will get access.
        For example, if frequency is 10Hz, then this will get called every round in the serial thread but only
        every 10 times will it be given access to the ELM and processed. This saves on bandwidth. No point updating
        coolant temperature every round when it changes slow relative to engine RPM for example.

        This method also records the time period between each time it gets processes and gives an average update frequency
        time
    */

    if (!m_lastTime)
    {
        gettimeofday(&m_timeVal, NULL);
        m_lastTime = m_timeVal.tv_sec+(m_timeVal.tv_usec/1000000.0);
    }

    if (m_currentFrequency == m_maxFrequency)
    {
        /* If in here, it is this sensor's turn to get access to the ELM.*/

        m_currentFrequency = 1; /* Reset the current frequency */

        /* Get the time and do calculations to work out difference from last time */
        gettimeofday(&m_timeVal, NULL);
        double thisTime = m_timeVal.tv_sec+(m_timeVal.tv_usec/1000000.0);
        m_instRefreshRate = 1/(thisTime - m_lastTime);
        m_lastTime = thisTime;

        if (!m_avgRefreshRate)
            m_avgRefreshRate = m_instRefreshRate;
        else
            m_avgRefreshRate = (m_instRefreshRate + m_avgRefreshRate)/2;

#ifdef DEBUGAUTOMON
        qDebug("Avg Refresh Rate: %.6lf Hz", m_instRefreshRate);
#endif

        return true;
    }
    else
    {
        /* It is not our turn so increment the current frequency so we get closer to having a turn */
        m_currentFrequency++;
    }

    return false;
}

QString Sensor::getName()
{
    /* Return the name of this sensor */
    return m_englishMeaning;
}

QString Sensor::getPid()
{
    return m_command;
}


float Sensor::getAvgRefreshRate()
{
    /* Get the average refresh rate */
    return m_avgRefreshRate;
}

void Sensor::validateSensorData(QString buffer)
{
    /*
        This method is responsible for validating the buffer response from the ELM327
    */

    /* Take copy of the buffer */
    QString data = buffer;

    /* Remove spaces and the line break character */
    QRegExp removeSpaces( " " );
    data.replace(QRegExp("[ \x0D]"), "");

    if (data.compare(QString("NODATA>")) == 0 || data.compare(QString("?>")) == 0)
    {
        /* The no data found message signifies that this sensor was not supported. We shouldn't get to this stage */
#ifdef DEBUGAUTOMON
        qDebug() << "Sending: " << getCommand() << " resulted in no data not found!";
#endif
        throw nodata_exception();
    }

    if (data.compare(QString("BUSERROR>")) == 0)
    {
        /* This occurs when communication breaks down between ELM and ECU */
#ifdef DEBUGAUTOMON
        qDebug() << "An error occured when communicating with the bus!";
#endif
        throw buserror_exception();
    }

    if (data.contains(QRegExp("[^a-fA-F0-9 >\x0D]")))
    {
        /*
            If the data contains characters other than a-f, A-f or 0-9, ie: NOT HEX, (excl > and line break)
            then response invalid */
#ifdef DEBUGAUTOMON
        qDebug() << data;
        qDebug() << "The response from the ELM327 was not hexidecimal when it should have being";
#endif
        throw nothexdata_exception();
    }
}

void Sensor::setBuffer(QString bufferResponse)
{
    /* This method is called by the serial I/O thread to set the returned bytes from ELM */

    if (bufferResponse.compare(m_bufferResponse) !=0 || m_changeTimes == 0)
    {
        /* Only if response has changed from last response or if this is our first update (m_changeTimes = 0) */

       try
       {
           /* First validate the data, this will throw exception if data invalid */
            validateSensorData(bufferResponse);

            /* Now update the buffer to the response received from ELM */
            m_bufferResponse = bufferResponse;

            /* The set Result method is used to convert the result and look after signaling */
            setResult();
        }
       catch (exception & e)
       {
           /* Data received was invalid, we choose to ignore it. this can happen easily */
            qWarning() << "An exception occured when reading data from device so result won't be recorded";
            qWarning() << "The exception was: " << e.what();
       }

    }
}

int Sensor::getChangeTimes()
{
    /* Get the change times. Used in the rules class */
    return m_changeTimes;
}

void Sensor::setResult()
{
    /* Convert the result to get it's double value */
    m_result = convertResult();

    if (!m_wasOutOfRange && !checkIfOutOfRange(m_result))
    {
        /* If not outOfRange before and is out of range now */

        m_wasOutOfRange = true; /* Set out of range to true. Prevents us coming in here in consecutive out of range times */

#ifdef DEBUGAUTOMON
        qDebug() << "The sensor, " << getEnglishMeaning() << " was out of range with a result of " << QString::number(m_result);
#endif
        emit outOfRangeError(QString("The value obtained for <strong>") + getEnglishMeaning() + QString("</strong>  is in an <strong><font color=\"red\">unsafe</font></strong> range"));
    }

    else if (checkIfOutOfRange(m_result))
    {
        /* Not out of range. (Really the bools should be opposite in the checkIfOutOfRange method, mis leading) */
        if (m_wasOutOfRange)
            m_wasOutOfRange = false; /* Reset the out of range variable so if we encounter a out of range in future, we deal with it */

        /* We only got into this method if a change occured, so now emit the change passing the result */
        emit changeOccurred(m_result);

        /* Update the change times. WARNING, could end up with overflow of int here if it got big enough */
        m_changeTimes++;
    }

#ifdef DEBUGAUTOMON
    qDebug() << "Emitting Change of result to connected slots";
#endif
    

}

void Sensor::resetSensor()
{
    /* Reset the change times. Important in some things like rules */
    m_changeTimes = 0;
}

Sensor::Sensor()
{
    /* Sensor constructor, set up defaults for all variables */

    setExpectedBytes(0);
    setSupported(false);
    m_wasOutOfRange = false;
    m_maxFrequency = 1;
    m_currentFrequency = 1;
    m_changeTimes = 0;
}


double Sensor::convertResult()
{
    /* If convertResult not implemented by derieved classes, just return 0 */
    return 0.0;
}

void Sensor::setMax(double max)
{
    /* Set maximum value this sensor can have */
    m_maxVal = max;
}

void Sensor::setMin(double min)
{
    /* Set minimum value this sensor can have */
    m_minVal = min;
}

double Sensor::getMin()
{
    /* Get minimum value sensor can have */
    return m_minVal;
}

double Sensor::getMax()
{
    /* Get maximum value sensor can have */
    return m_maxVal;
}

double Sensor::getResult() const
{
    /* Get the current value of the sensor */
    return m_result;
}

bool Sensor::checkIfOutOfRange(double value)
{

    /* If out of range, return false. By right the bools should be opposite. bit mis leading */
    if (value < m_minVal || value > m_maxVal)
        return false;

    return true;
}

void Sensor::setUnits(UNITS resultUnits)
{
    /* Set the unit type of this sensor */
    m_resultUnits = resultUnits;
}


void Sensor::setSupported(bool isSupported)
{
    /* Set support for current sensor on current vehicle */
    m_isSupported = isSupported;
}

bool Sensor::isSupported()
{
    /* Check if sensor supported */
    return m_isSupported;
}
