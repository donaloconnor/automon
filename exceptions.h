#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

/*
    ==================================================================================================
    |  exceptions.h                                                                                  |
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

    This file gives a list of mini exception classes that are thrown in certain situations in Automon.
*/

#include <iostream>
#include <exception>

using namespace std;

namespace AutomonKernel
{

    class monitoring_exception : public exception
    {
        virtual const char* what() const throw()
        {
            return "An attempt was made to use Serial I/O, however Automon was in monitoring mode";
        }
    };

    class nothex_exception : public exception
    {
        virtual const char* what() const throw()
        {
            return "The returned data was not hex data";
        }
    };

    class init_exception : public exception
    {
        virtual const char* what() const throw()
        {
            return "The ECU->Automon Bus could not be initialised.";
        }
    };

    class nothexdata_exception : public exception
    {
        virtual const char* what() const throw()
        {
            return "Sensor Data retrieved was not hex data when should be";
        }
    };

    class nodata_exception : public exception
    {
        virtual const char* what() const throw()
        {
            return "Received a NO DATA from ELM327";
        }
    };

    class buserror_exception : public exception
    {
        virtual const char* what() const throw()
        {
            return "There was a problem communicating with the Bus";
        }
    };

    class elmnotcontactable_exception : public exception
    {
        virtual const char* what() const throw()
        {
            return "The ELM interface that Automon depends on is not available!";
        }
    };

    class serialio_exception : public exception
    {
        virtual const char* what() const throw()
        {
            return "The serial port cannot be opened. Major problem!";
        }
    };

    class rulefile_exception : public exception
    {
        virtual const char* what() const throw()
        {
            return "The rule's file could not be opened!";
        }
    };
}


#endif // EXCEPTIONS_H
