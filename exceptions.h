/*

    This file is part of the Automon Project (OBD Diagnostics) - http://www.automon.io/
    Source Repository: https://github.com/donaloconnor/automon/
    
    Copyright (c) 2015, Donal O'Connor <donaloconnor@gmail.com>

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
    
*/

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

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
