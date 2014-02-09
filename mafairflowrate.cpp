#include "automon.h"

/*
    ==================================================================================================
    |  mafairflowrate.cpp                                                                            |
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

    Implementation file of the "MAF Airflow Rate" Sensor object.
    In here we set the Formula, Command, Units, Expected Bytes, Min and Max Values
*/

using namespace AutomonKernel;

MafAirFlowRate::MafAirFlowRate(QString command, QString englishMeaning)
{
    m_command = command;
    m_englishMeaning = englishMeaning;
}

MafAirFlowRate::MafAirFlowRate()
{
    m_command = "0110";
    m_englishMeaning = "MAF Airflow Sensor";
    setUnits(GS);
    setExpectedBytes(2);
    setMin(0);
    setMax(655.35);
}

double MafAirFlowRate::convertResult()
{
    /* This is the conversion formula. It basically returns the value from the bytes retrieved from the ECU */

    QList<int> bytes = Automon::getBytes(*this);

    double value = (((bytes[2]*256)+bytes[3])/100);

    return value;

}
