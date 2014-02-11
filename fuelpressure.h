#ifndef FUELPRESSURESENSOR_H
#define FUELPRESSURESENSOR_H

/*
    ==================================================================================================
    |  fuelpressure.h                                                                                |
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
    class FuelPressure : public Sensor
    {
    public:
        FuelPressure(QString command, QString englishMeaning);
        FuelPressure();
        double convertResult();
        ~FuelPressure() { }
    };
}
#endif // FUELPRESSURESENSOR_H
