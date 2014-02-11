/*
    ==================================================================================================
    |  throttleposition.cpp                                                                          |
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

    Implementation file of the "Throttle Position" Sensor object.
    In here we set the Formula, Command, Units, Expected Bytes, Min and Max Values
*/

#include "automon.h"

using namespace AutomonKernel;

ThrottlePosition::ThrottlePosition(QString command, QString englishMeaning)
{
    m_command = command;
    m_englishMeaning = englishMeaning;
}

ThrottlePosition::ThrottlePosition()
{
    m_command = "0111";
    m_englishMeaning = "Throttle Position";
    setUnits(MPH);
    setExpectedBytes(1);
    setMin(0);
    setMax(100);
}

double ThrottlePosition::convertResult()
{
    /* This is the conversion formula. It basically returns the value from the bytes retrieved from the ECU */

    QList<int> bytes = Automon::getBytes(*this);

    double value = (bytes[2]*100/255);

    return value;

}
