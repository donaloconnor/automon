#ifndef DTCHELPER_H
#define DTCHELPER_H

/*
    ==================================================================================================
    |  dtchelper.h                                                                                   |
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

    The DTCHelper class is used by the AutomonKernel for DTC related functions such as loading up
    number of DTCs present, loading in DTC database, creating DTC objects. Checking if MIL on,
    resetting the MIL and clearing the DTCs
*/

#include "dtc.h"
#include "serialhelper.h"

namespace AutomonKernel
{
    class DTCHelper
    {
    public:
        DTCHelper(SerialHelper * serialHelper);
        QList<Sensor> getFreezeFrame() const;
        QList<DTC*> getCodesFound() const;
        bool resetMilAndClearCodes();
        bool addDTCs(QList<DTC>);
        int getNumberOfCodes() const;
        bool checkMil() const;
        void init();
        void refreshDTCInformation();


    private:
        void loadCodes();
        void loadFoundCodes();
        void setNumCodes();
        QList<DTC*> m_codeDB;
        QList<DTC*> m_codesFound;
        QList<Sensor*> m_freezeFrame;
        int m_numCodes;
        bool m_milOn;
        SerialHelper * m_serialHelper;

    };
}

#endif // DTCHELPER_H
