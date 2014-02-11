#include "automon.h"

/*
    ==================================================================================================
    |  fuelpressure.cpp                                                                            |
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

    Implementation file of the "Fuel Pressure" Sensor object.
    In here we set the Formula, Command, Units, Expected Bytes, Min and Max Values
*/


using namespace AutomonKernel;

FuelPressure::FuelPressure(QString command, QString englishMeaning)
{
    m_command = command;
    m_englishMeaning = englishMeaning;
}

FuelPressure::FuelPressure()
{
    m_command = "010A";
    m_englishMeaning = "Fuel Pressure";
    setUnits(KPA);
    setExpectedBytes(1);
    setMin(0);
    setMax(765);
}

double FuelPressure::convertResult()
{
    /* This is the conversion formula. It basically returns the value from the bytes retrieved from the ECU */

    QList<int> bytes = Automon::getBytes(*this);

    double value = (bytes[2]*3);

    return value;

}
