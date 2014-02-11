/*
    ==================================================================================================
    |  command.cpp                                                                                   |
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

    The implementation file of the Command class. Further details below
*/


#include "automon.h"

using namespace AutomonKernel;

Command::Command()
{

}

Command::Command(QString command, QString englishMeaning)
           : m_command(command), m_englishMeaning(englishMeaning)
{
    /*
        The expected bytes is something that the ELM327 uses. If you specify this in the command to the ELM327
        it only waits for that many bytes, improving speed of response
    */
    m_expectedBytes = 0;
}

void Command::setBuffer(QString bufferResponse)
{

    /* Take copy of the buffer */
    QString testData = bufferResponse;

    /* Remove spaces and the line breaks with the help of Regular Expressions */
    QRegExp removeSpaces( " " );
    testData.replace(removeSpaces, "");
    QRegExp removeLineBreak( "\x0D" );
    testData.replace(removeLineBreak, "");

    if (testData.compare(QString("NODATA>")) == 0 || testData.compare(QString("?>")) == 0)
    {
        /* Command wasn't understood or no ECU data */
#ifdef DEBUGAUTOMON
        qDebug() << "Sending: " << getCommand() << " resulted in no data not found!";
#endif
    }
    else
        m_bufferResponse = bufferResponse;
}

void Command::setExpectedBytes(int expectedBytes)
{
    /* Simply assign the expected bytes. This is usually done from the sensor classes */
    m_expectedBytes = expectedBytes;
}

int Command::getExpectedBytes()
{
    /* Get the expected bytes value */
    return m_expectedBytes;
}

void Command::setEnglishMeaning(QString englishMeaning)
{
    /* A setter to set the english meaning of the current command object */
    m_englishMeaning = englishMeaning;
}

void Command::setCommand(QString command)
{
    /* A setter to set the command of the current object */
    m_command = command;
}

QString Command::getEnglishMeaning() const
{
    /* A getter to get the english meaning of the current command object */
    return m_englishMeaning;
}

QString Command::getCommand() const
{
    /* Return the command */
    return m_command;
}

QString Command::getBuffer() const
{
    /* Return the buffer filled by the serial I/O thread */
    return m_bufferResponse;
}
