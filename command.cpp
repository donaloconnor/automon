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
