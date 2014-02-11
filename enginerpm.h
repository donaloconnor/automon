#ifndef ENGINERPMSENSOR_H
#define ENGINERPMSENSOR_H

/*
    ==================================================================================================
    |  enginerpm.h                                                                                   |
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

    This class is a derivitive of the Sensor class. It is used in the Serial I/O thread for reading this sensor
*/

#include "sensor.h"

namespace AutomonKernel
{
    class EngineRPM : public Sensor
    {
    public:
        EngineRPM(QString command, QString englishMeaning);
        EngineRPM();
        double convertResult();
        ~EngineRPM() { }
    };
}
#endif // ENGINERPMSENSOR_H
