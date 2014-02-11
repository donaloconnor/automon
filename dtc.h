#ifndef DTC_H
#define DTC_H

/*
    ==================================================================================================
    |  dtc.h                                                                                         |
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

    This is a class defintion for the DTC object. It holds the code, english meaning and potential solution
    if present.
*/

#include <QString>

namespace AutomonKernel
{
    class DTC
    {

    public:

        enum CODETYPE { POWERTRAIN, CHASSIS, BODY, NETWORK };

        DTC(QString code, QString englishMeaning, QString potentialSolution);
        QString getCode();
        QString getEnglishMeaning();
        QString getPotentialSolution();


    private:
        QString m_code;
        QString m_englishMeaning;
        QString m_potentialSolution;
        CODETYPE m_codeType;
    };
}
#endif // DTC_H
