#include "automon.h"

/*
    ==================================================================================================
    |  vehiclespeed.cpp                                                                              |
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

    Implementation file of the "Vehicle Speed" Sensor object.
    In here we set the Formula, Command, Units, Expected Bytes, Min and Max Values
*/

using namespace AutomonKernel;

VehicleSpeed::VehicleSpeed(QString command, QString englishMeaning)
{
    m_command = command;
    m_englishMeaning = englishMeaning;
}

VehicleSpeed::VehicleSpeed()
{
    m_command = "010D";
    m_englishMeaning = "Vehicle Speed";
    setUnits(MPH);
    setExpectedBytes(1);
    setMin(0);
    setMax(255);
}

double VehicleSpeed::convertResult()
{
    /* This is the conversion formula. It basically returns the value from the bytes retrieved from the ECU */

    QList<int> bytes = Automon::getBytes(*this);

    double value = (bytes[2]);

    return value;

}
