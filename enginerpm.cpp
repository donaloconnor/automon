/*
    ==================================================================================================
    |  enginerpm.cpp                                                                                 |
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

    Implementation file of the "Engine RPM" Sensor object.
    In here we set the Formula, Command, Units, Expected Bytes, Min and Max Values
*/


#include "automon.h"

using namespace AutomonKernel;

EngineRPM::EngineRPM(QString command, QString englishMeaning)
{
    m_command = command;
    m_englishMeaning = englishMeaning;
}

EngineRPM::EngineRPM()
{
    m_command = "010C";
    m_englishMeaning = "Engine RPM";
    setUnits(RPM);
    setExpectedBytes(2);
    setMin(0);
    setMax(12000);
}

double EngineRPM::convertResult()
{
    /* This is the conversion formula. It basically returns the value from the bytes retrieved from the ECU */

    QList<int> bytes = Automon::getBytes(*this);

    double value = (((bytes[2]*256) + bytes[3])/4);

    return value;

}
