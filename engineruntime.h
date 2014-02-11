#ifndef ENGINERUNTIMESENSOR_H
#define ENGINERUNTIMESENSOR_H

/*
    ==================================================================================================
    |  engineruntime.h                                                                               |
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

class QVariant;

namespace AutomonKernel
{
    class EngineRunTime : public Sensor
    {
    public:
        EngineRunTime(QString command, QString englishMeaning);
        EngineRunTime();
        double convertResult();
        ~EngineRunTime() { }
    };
}
#endif // ENGINERUNTIMESENSOR_H
