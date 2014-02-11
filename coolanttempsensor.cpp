/*
    ==================================================================================================
    |  commandedegr.cpp                                                                              |
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

    Implementation file of the "Coolant Temperature" Sensor object.
    In here we set the Formula, Command, Units, Expected Bytes, Min and Max Values
*/

#include "automon.h"

using namespace AutomonKernel;

CoolantTempSensor::CoolantTempSensor(QString command, QString englishMeaning)
{
    m_command = command;
    m_englishMeaning = englishMeaning;
}

CoolantTempSensor::CoolantTempSensor()
{
    m_command = "0105";
    m_englishMeaning = "Engine coolant temperature";
    setUnits(DEGREES);
    setExpectedBytes(1);
    setMin(-40);
    setMax(215);
}

double CoolantTempSensor::convertResult()
{
    /* This is the conversion formula. It basically returns the value from the bytes retrieved from the ECU */

    QList<int> bytes = Automon::getBytes(*this);

    double value = (bytes[2]-40);

    return value;

}
