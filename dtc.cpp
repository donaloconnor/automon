/*
    ==================================================================================================
    |  dtc.cpp                                                                                       |
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

    This is the implementation of the basic DTC class. Self explanatory
*/


#include "automon.h"

using namespace AutomonKernel;

DTC::DTC(QString code, QString englishMeaning, QString potentialSolution)
        : m_code(code), m_englishMeaning(englishMeaning), m_potentialSolution(potentialSolution)
{
}

QString DTC::getCode()
{
    return m_code;
}

QString DTC::getEnglishMeaning()
{
    return m_englishMeaning;
}

QString DTC::getPotentialSolution()
{
    return m_potentialSolution;
}
