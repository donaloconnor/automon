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

    Implementation file of the "Commanded EGR" Sensor object.
    In here we set the Formula, Command, Units, Expected Bytes, Min and Max Values
*/

#include "automon.h"

using namespace AutomonKernel;

CommandedEgr::CommandedEgr(QString command, QString englishMeaning)
{
    /* Set the member variables */
    m_command = command;
    m_englishMeaning = englishMeaning;
}

CommandedEgr::CommandedEgr()
{
    /* Set properties of this sensor */
    m_command = "012C";
    m_englishMeaning = "Commanded EGR";
    setUnits(MPH);
    setExpectedBytes(1);
    setMin(0);
    setMax(100);
}


double CommandedEgr::convertResult()
{
    /* This is the conversion formula. It basically returns the value from the bytes retrieved from the ECU */

    QList<int> bytes = Automon::getBytes(*this);

    double value = (bytes[2]*100/255);

    return value;

}
