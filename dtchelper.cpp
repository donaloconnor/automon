/*
    ==================================================================================================
    |  dtchelper.cpp                                                                                 |
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

    Implementation of the DTC Helper. More details below
*/

#include <QFile>
#include "automon.h"


using namespace AutomonKernel;

DTCHelper::DTCHelper(SerialHelper * serialHelper)
        : m_serialHelper(serialHelper)
{
    /* Assume MIL not on first and no DTC codes present. These will get set in the init stage below */
    m_milOn = false;
    m_numCodes = 0;
 }

void DTCHelper::loadCodes()
{
    /*
      This method is responsible for opening the code database file and adding each code to the code DB in ram
    */

    /* Open DTC Code file */
    QFile file(DTCCODEFILE);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;


    /* Create an input stream from the file */
    QTextStream in(&file);

    while (!in.atEnd())
    {
        /* While not at end of file, read the line. Split on the tab delimiter */
        QString line = in.readLine();
        QStringList attributes = line.split("\t");

        /* Ensure that size is 2, one for code, another for english meaning */
        if (attributes.size() == 2)
            m_codeDB.append(new DTC(attributes[0], attributes[1],"")); /* Append a new DTC to code database */
     }
}

void DTCHelper::init()
{
    /*
        This method is responsible for checking the number of codes and setting what codes avaiable in ECU
        and if MIL is on/off
    */

    /* Set number codes found */
    setNumCodes();

    /* Load code DB */
    loadCodes();

    if (m_numCodes > 0)
        loadFoundCodes(); /* Only load found codes if codes are present in the ECU */

}

void DTCHelper::setNumCodes()
{
    /*
        This method is responsible for setting the number of codes on the ECU.
        To do this it must send a command to the ELM327 which in turn will query the ECU
    */

    /* Create command and send it */
    Command mode0101;
    mode0101.setCommand("0101");

    m_serialHelper->sendCommand(mode0101);

    /* Read back bytes in integer format */
    QList<int> bytes = Automon::getBytes(mode0101);

    /* Do a bit wise and to check if the MIL is on / off as it is bit encoded */
    if ((int)(bytes[2] & 0x80) == 128)
        m_milOn = true;

    /* The number of codes is specified by the lower bytes */
    m_numCodes = ((int)(bytes[2] & 0x7F));
}


QList<DTC*> DTCHelper::getCodesFound() const
{
    /* Return a list of DTC pointers of the codes found */
    return m_codesFound;
}

int DTCHelper::getNumberOfCodes() const
{
    /* Return number of codes present in ECU */
    return m_numCodes;
}

bool DTCHelper::checkMil() const
{
    /* Return a bool, stating if MIL on or off */
    return m_milOn;
}

void DTCHelper::refreshDTCInformation()
{
    /*
        This method is responsible for re checking the ECU for new codes and if MIL set
    */

    /* Update number of codes found */
    setNumCodes();

    if (m_numCodes > 0)
        loadFoundCodes(); /* If 1 or more codes, update the DTC code list */
}

void DTCHelper::loadFoundCodes()
{
    /*
        This method is responsible for creating the list of DTCs found
    */

    /* Clear list incase old ones still present */
    m_codesFound.clear();

    if (m_numCodes == 0)
        return;

    /*
        This is a complicated process since multiple ECU's may exist and '43' is a valid part of a command
        so we don't know for sure where our codes are in the returned string
        This means we must turn headers on. Capture the header part - Checksum and then use this as a delimiter.
        So now we have each string in a QString list, and next read each consecutive bytes.
    */


    /* Turn headers on */
    Command turnHeadersOn;
    turnHeadersOn.setCommand("ATH1");
    m_serialHelper->sendCommand(turnHeadersOn);

    /* Next send mode3 request */

    Command mode3Command;
    mode3Command.setCommand("03");
    m_serialHelper->sendCommand(mode3Command);

    /* Now we must parse the buffer and remove spaces */
    QString bufferResponse = mode3Command.getBuffer();

    QRegExp removeSpaces( " " );
    bufferResponse.replace(removeSpaces, "");
    QRegExp removeLineBreak( "\x0D" );
    bufferResponse.replace(removeLineBreak, "");


    QList<QString> bytes;
    QList<QString> unParsedCodes;

    if (bufferResponse.at(bufferResponse.size()-1) != '>')
        qDebug() << "No prompt character found!";
    else
    {
        /* Remove prompt character */
        bufferResponse = bufferResponse.section("",0,bufferResponse.size()-1);

        /* Check if buffer is even bytes */
        if ((bufferResponse.size() % 2) != 0)
        {
#ifdef DEBUGAUTOMON
            qDebug() << "Uneven number of bytes, error!";
#endif
        }
        else
        {
            /* Get delimiter, ie the header of the message */
            QString delimiter = bufferResponse.section("",0,8);

            /* Split on header to get lines of codes */
            QStringList codeLines = bufferResponse.split(delimiter);

            int numCodesAdded = 0;

            for (int i = 0; i < codeLines.size(); i++)
            {
                /* Now for each line we continue reading until we reach the number of codes reported by m_numCodes */
                codeLines[i] = codeLines[i].section("",0,codeLines[i].size()-2);
                for(int k = 0; k < codeLines[i].size(); k+=4)
                {
                    /* Break if we've reached the number of codes found */
                    if (++numCodesAdded > m_numCodes)
                        break;

                    /* Get code */
                    QString byte1 = codeLines[i].section("",k+1,k+2);
                    QString byte2 = codeLines[i].section("",k+3, k+4);

                    /* Add to unparsed codes. This don't include the P,U prefixes */
                    unParsedCodes.append(QString(byte1+byte2));

                }
            }

        }
    }

    /* Now it is time to parse the codes and do a check against our code DB and add them to the foundList if matched */

    for(int i = 0; i < unParsedCodes.size(); i++)
    {
        /* for each code found... do a match against our db */

        QByteArray codeType = unParsedCodes[i].section("",0,1).toLatin1();

        char codeTypeChar;
        qstrcpy(&codeTypeChar,codeType);

        QString firstChar;

        /* Set the first char of each code depending on its first number */
        switch (codeTypeChar)
        {
            case '0':
                firstChar = "P0";
                break;
            case '1':
                firstChar = "P1";
                break;
            case '2':
                firstChar = "P2";
                break;
            case '3':
                firstChar = "P3";
                break;
            case '4':
                firstChar = "C0";
                break;
            case '5':
                firstChar = "C1";
                break;
            case '6':
                firstChar = "C2";
                break;
            case '7':
                firstChar = "C3";
                break;
            case '8':
                firstChar = "B0";
                break;
            case '9':
                firstChar = "B1";
                break;
            case 'A':
                firstChar = "B2";
                break;
            case 'B':
                firstChar = "B3";
                break;
            case 'C':
                firstChar = "U0";
                break;
            case 'D':
                firstChar = "U1";
                break;
            case 'E':
                firstChar = "U2";
                break;
            case 'F':
                firstChar = "U3";
                break;
            default:
                firstChar = "XX";
                break;
        }

        /* Get the remaining part of the DTC */
        QString remainingPart = unParsedCodes[i].section("",2,4);

        /* Create the full code */
        QString thisCode = QString(firstChar+remainingPart);

        /* Assume code not foun d*/
        bool found = false;

        for (int k = 0; k < m_codeDB.size(); k++)
        {
            if (thisCode.compare(m_codeDB[k]->getCode()) == 0)
            {
                /* Code is found. Add it to the codes found list */
                m_codesFound.append(m_codeDB[k]);
                found = true;
                break;
            }
        }

        if (!found)
        {
            /* Now if we did not find a match for all codes, we will still add this code and give unknown english meaning */
            m_codesFound.append(new DTC(thisCode,"Unknown Code. Not found in Code DB", "Unknown Solution"));
        }

    }

    /* Turn off headers again */
    Command turnHeadersOff;
    turnHeadersOff.setCommand("ATH0");
    m_serialHelper->sendCommand(turnHeadersOff);
}

bool DTCHelper::resetMilAndClearCodes()
{
    /*
        This method is responsible for clearing the DTCs and resetting MIL.
        This is a simple mode 4 command so create this command and send it to ELM327
    */

    Command mode4Reset;
    mode4Reset.setCommand("04");

    m_serialHelper->sendCommand(mode4Reset);

    return true;
}
