#ifndef COMMAND_H
#define COMMAND_H

/*
    ==================================================================================================
    |  command.h                                                                                     |
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

    This class, Command is one that is sent to the SerialHelper class in order to perform
    a command with the ELM327. All data is encapsulated in this. This object is
    passed by reference and the SerialHelper fills up the buffer for processing
    by another object.

*/

#include <QtCore>

namespace AutomonKernel
{
    class Command : public QObject
    {

    public:
        Command();
        Command(QString command, QString englishMeaning);
        virtual ~Command() { }
        virtual void setBuffer(QString bufferResponse);
        void setEnglishMeaning(QString englishMeaning);
        void setCommand(QString command);
        void setExpectedBytes(int expectedBytes);
        int getExpectedBytes();
        QString getBuffer() const;
        QString getEnglishMeaning() const;
        QString getCommand() const;

    protected:
        QString     m_command;
        QString     m_englishMeaning;
        QString     m_bufferResponse;

    private:
        int         m_expectedBytes;

    };
}

#endif // COMMAND_H
