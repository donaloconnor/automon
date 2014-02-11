/*
    ==================================================================================================
    |  o2voltage.cpp                                                                                 |
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

    Implementation file of the "O2 Voltage" Sensor object.
    In here we set the Formula, Command, Units, Expected Bytes, Min and Max Values
*/

#include "automon.h"

using namespace AutomonKernel;

O2Voltage::O2Voltage(QString command, QString englishMeaning)
{
    m_command = command;
    m_englishMeaning = englishMeaning;
}

O2Voltage::O2Voltage()
{
    m_command = "0114";
    m_englishMeaning = "02 Voltage in Bank 1 Sensor 1";
    setUnits(VOLTS);
    setExpectedBytes(2);
    setMin(0);
    setMax(1.275);
}

double O2Voltage::convertResult()
{
    /* This is the conversion formula. It basically returns the value from the bytes retrieved from the ECU */

    QList<int> bytes = Automon::getBytes(*this);

    double value = (bytes[2]*0.005);

    return value;

}
