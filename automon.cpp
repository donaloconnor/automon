/*
    ==================================================================================================
    |  automon.cpp                                                                                   |
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

    This is the implementation file of the Automon class, the interface to the Automon kernel.
    Below are details of what each method does.
*/

#include "automon.h"

using namespace AutomonKernel;


/*
    This is a quick class definition of a Thread that is used for sleep purposes.
    It has a static method, msleep. This allows us to sleep the main thread when we want,
    by classing SleeperThread::msleep(ms);
*/

class SleeperThread : public QThread
{
public:
    static void msleep(unsigned long msecs)
    {
        QThread::msleep(msecs);
    }
};


Automon::Automon(QString port)
{
    /*
        The Automon interface constructor. Here we set up the DTC helper and Serial helper classes.
        We also initialise important variables.

        The seiral helper is passed the port address. This defaults to /dev/ttyUSB0 if not specified
    */

    m_serialHelper = new SerialHelper(port);
    m_dtcHelper = new DTCHelper(m_serialHelper);

    m_isMonitoring = false; /* Used to determine if Automon in monitoring state */
    m_milOn = false;        /* Default to Malfunction Indicator Lamp off */
}

bool Automon::isMonitoring() const
{
    /*
        This method is called outside of the Automon Kernel to determine if the Serial I/O
        thread is currently active, in which case, it must be stopped before another task can use it
    */

    return m_isMonitoring;
}

QList<int> Automon::getBytes(Command & command)
{
    /*
        This method is used to grab an array of Integer bytes from the command executed by
        the serial I/O thread. The serial I/O thread will populate the command's buffer with
        hexidecimal ASCII. This has to be converted to integers so that sensors
        can do the neccessary formula conversions in order to obtain the result.
    */

    QList<int> result;

    /* First grab the buffer the was populated by the Serial I/O thread and the ELM327 chip */
    QString bufferResponse = command.getBuffer();

    /* Remove any spaces in the response using a regular expression */
    QRegExp removeSpaces( " " );
    bufferResponse.replace(removeSpaces, "");

    /* Remove the line break at the end that is sent back by the ELM327 */
    QRegExp removeLineBreak( "\x0D" );
    bufferResponse.replace(removeLineBreak, "");

    /* Create a list of String bytes. */
    QList<QString> bytes;

    /* Ensure that the > character is at the end of the response. This is always sent by the ELM327 */
    if (bufferResponse.at(bufferResponse.size()-1) != '>')
    {
#ifdef DEBUGAUTOMON
        qDebug() << "No prompt character found!";
#endif
    }
    else
    {
        /* We have the prompt character, so all is good. Now remove it */
        bufferResponse = bufferResponse.section("",0,bufferResponse.size()-1);

        /*
            The response back from the ELM327 will always be a even number of bytes.
            Do a check for this before procedding
        */


        if ((bufferResponse.size() % 2) != 0)
        {
#ifdef DEBUGAUTOMON
            qDebug() << "Uneven number of bytes, error!";
#endif
        }
        else
        {
            /* All is well now. Response can be assumed valid */
            for (int i = 0; i < bufferResponse.size(); i+=2)
            {
                /* For every second byte (ie, every Hexidecimal byte (2 ASCII bytes, ie: FF = 2 Bytes ASCII = 1 Byte int */
                bytes.append(bufferResponse.section("",i+1,i+2));
            }
        }
    }

    /* This check is used in the toInt method of QString to check if it is a valid integer */
    bool check;

    for (int i = 0; i < bytes.size(); i++)
    {
        /* For each Byte string, eg: "FF" = 15base10, convert into an Integer and push onto the result list */

        QByteArray temp(bytes[i].toLatin1());
        result.append(temp.toInt(&check, 16));
    }

    /*
        Now we have all the ASCII Hexidecimal strings converted into bytes.
        EG: 10FF11
            = "10", "FF", "11"
            = 10 = 0001010 = integer 10, FF = 11111111 = integer 15, 11 = 0001011 = Integer 11
    */

    /* Return the list to the calling method */
    return result;
}

bool Automon::saveRuleList()
{
    /* This method is used to save the current rule list in memory back onto the file */

    /* Open the file with WriteOnly. This will rewrite over the whole file */
    QFile file(RULEFILE);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        throw rulefile_exception(); /* File could not be opened. Throw exception to catch handler */

    /* Open a stream out to write back out to the file */
    QTextStream out(&file);

    /* Foreach rule in the rule list, write it back out to the file */
    for (int i = 0; i < m_ruleList.size(); i++)
        out << m_ruleList.at(i) << endl;

    /* Close file to prevent locking */
    file.close();

    /* Re load the rules list now, from file again back into ram */
    loadRuleList();

    return true;
}

bool Automon::loadRuleList()
{
    /* This method is used to open the rules file and read its contents into ram */

    /* Incase we already opened the rule list, we need to clear the current rule list before appending*/
    m_ruleList.clear();

    /* Open file with readonly permissions */
    QFile file(RULEFILE);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw rulefile_exception();

    /* Create an inward stream to read contents of file into a list */
    QTextStream in(&file);


    while (!in.atEnd())
    {
        /* We not at end of file, so read line */
        QString line = in.readLine();

        /* Append the rule in to the rule list in ram */
        m_ruleList.append(line);
     }

    /* Close the file again */
    file.close();

    return true;
}

void Automon::addRuleString(QString rule)
{
    /* This method is used to add a rule to the rule list in ram */

    /* Assume that the inputted new rule isn't found in the current list */
    bool found = false;

    /* Go through each rule in the rule list and compare to new rule. If found set found = true */
    for (int i = 0; i < m_ruleList.size(); i++)
        if (m_ruleList.at(i).compare(rule) == 0)
            found = true;

    /* Only add the new rule if it wasn't found, ie: Not in the current list already */
    if (!found)
        m_ruleList.append(rule);
}

void Automon::removeRuleString(QString rule)
{
    /* This method is used to remove a rule from the rule list in ram */

    /* Go through each rule and compare to the inputted rule. If found, remove it from the location and exit */
    for (int i = 0; i < m_ruleList.size(); i++)
        if (m_ruleList.at(i).compare(rule) == 0)
        {
            m_ruleList.removeAt(i);
            return;
        }
}

void Automon::removeRuleEnglishMeaningString(QString rule)
{
    /* This method is similar to the above, but using the human friendly rule instead of sensor codes */

    /*
        For each rule, convert it to human readable rule and conpare to inputted human readable rule and
        if found, remove it and return
    */
    for (int i = 0; i < m_ruleList.size(); i++)
        if (convertRuleToEnglish(m_ruleList.at(i)).compare(rule) == 0)
        {
            m_ruleList.removeAt(i);
            return;
        }
}


QStringList Automon::getRuleList()
{
    /* Simply return the current rule list that's in memory */
    return m_ruleList;
}

void Automon::loadSensors()
{
    /*
        This method is used when setting up the Automon kernel. It is responsible
        for adding new sensors to the kernel for monitoring.
        If more sensors are created, and inherited from the sensor class,
        they need to be added in the list here.
    */
    

    /* Clear all sensor lists. The m_sensors list is all sensors, m_activeSensors is ones currently in the seiral I/O thread */
    m_sensors.clear();
    m_activeSensors.clear();


    /* Create an instance of each sensor and append it to the main sensor list */
    m_sensors.append(new CoolantTempSensor);
    m_sensors.append(new VehicleSpeed);
    m_sensors.append(new EngineRPM); 
    m_sensors.append(new EngineRunTime);
    m_sensors.append(new ThrottlePosition);
    m_sensors.append(new FuelPressure);
    m_sensors.append(new FuelLevelInput);
    m_sensors.append(new MafAirFlowRate);
    m_sensors.append(new CommandedEgr);
    m_sensors.append(new O2Voltage);

    /* Update the sensor support for the current car */

    updateSensorSupport();
    
    /*
        Sensors have boundary values, a low value and a high.
        If the obtained value is outside of this boundary range, the sensor emits an OutOfRange signal.
        Connect this signal to the slot that receives these messages.
    */
    
    for (int i = 0; i < m_sensors.size(); i++)
        connect(m_sensors[i], SIGNAL(outOfRangeError(QString)), this, SLOT(receiveErrorMessage(QString)));

}

void Automon::receiveErrorMessage(QString message)
{
    /*
        This simply passes on the error to another error handler that is connected to the sendErrorMessage signal of this
        class
    */

    emit sendErrorMessage(message);
}

bool Automon::connectToErrorToSlot(QObject * receiver)
{
    /* This method allows and outside object to connect it's slot to the errorMessage signal from this class */

     if (connect(this, SIGNAL(sendErrorMessage(QString)), receiver, SLOT(errorslot(QString))))
        return true;

     return false;
}

void Automon::updateSensorSupport()
{
    /*
        This method is responsible for setting sensors that are not supported to not supported
        state, so the GUI won't provide these sensors for the current vehicle.
        OBD II does not state that manufacturers have to implement all sensor types
    */

    if (m_sensors.size() > 0)
    {
        /* Only if there are sensors to update */

        /*
            The OBD II protocol has a PID that is bitwise encoded to represent what sensors
            are supported by the vehicle's ECU. All cars must support the 0100 PID that defines
            what sensors in the range of 1 - 20, and another PID for the next range
        */


        /* We will only be concerned with first two ranges. These QStrings will hold the response in ASCII format */
        QString binaryEncodedString1;
        QString binaryEncodedString2;

        /* First we must query OBD to get the OBD Supported bit encoded string */

        Command supportCommand1;
        supportCommand1.setCommand("0100");

        m_serialHelper->sendCommand(supportCommand1);

        /* Get all the bytes back. Each byte will be a binary encoded byte */
        QList<int> bytes1 = Automon::getBytes(supportCommand1);


        for (int i = 2 /* Skip command echo */; i < 6; i++)
        {
            /*
                Get current byte. Convert base 2, binary to ASCII representation.
                ie: Integer: 10 = 00001010base2. Convert this to string "00001010"
            */
            QString thisByte = QString::number(bytes1[i],2);

            if (thisByte.size() < 8)
            {
                /*
                    If the size of the current byte is < 8, we must pad the front with 0's.
                    For example, 10 = "1010", we want "00001010"
                */

                int size = thisByte.size();

                /* Prepend with current number of 0's */
                for (int j=0; j < 8-size; j++)
                    thisByte.prepend("0");
            }

            /* Now append this byte to the binary encoded string list for first range */
            binaryEncodedString1.append(thisByte);
        }

        /*
            The last bit of the last byte determines if Pids over 20 are supported. Check this bit
            and if set, run the next command to get the next bit encoded string
        */

        bool checkPidsOver20 = (QChar('1') == binaryEncodedString1.at(31));

        if (checkPidsOver20)
        {
            /* Pids over 20 are supported by the current vehicle so continue like we did above for first range */

            Command supportCommand2;
            supportCommand2.setCommand("0120");

            m_serialHelper->sendCommand(supportCommand2);

            /* Get all the bytes back. Each byte will be a binary encoded byte */
            QList<int> bytes2 = Automon::getBytes(supportCommand2);

            for (int i = 2 /* Skip command echo */; i < 6; i++)
            {
                QString thisByte = QString::number(bytes2[i],2);
                if (thisByte.size() < 8)
                {
                    /*
                        If the size of the current byte is < 8, we must pad the front with 0's.
                        For example, 10 = "1010", we want "00001010"
                    */

                    int size = thisByte.size();

                    /* Prepend with correct number of 0's */
                    for (int j=0; j < 8-size; j++)
                        thisByte.prepend("0");
                }

                /* Add the binary encoded string for current byte to the list */
                binaryEncodedString2.append(thisByte);
            }
        }

        /* Now that we have our binary encoded strings all we have to do is go through each sensor and compare */

        for (int i = 0; i < m_sensors.size(); i++)
        {
            /* Get the PID of the current sensor. Example: Speed = 010D, so PID = 0D, position 3, 4 */
            QString pid = m_sensors.at(i)->getCommand().section("",3,4);

            /* Check used for integer conversion */
            bool check;

            /* Convert the QString to integer, so "0D" becomes 13 in base 10 */
            /* This becomes the position in the bit encoded strings then */
            int position = pid.toInt(&check, 16);

            if (position < 32)
            {
                /* In first range. Check if the bit encoded string at that position == 1*/
                if (binaryEncodedString1.at(position-1) == QChar('1'))
                    m_sensors.at(i)->setSupported(true); /* It is, so set to true, supported */
                else
                    m_sensors.at(i)->setSupported(false); /* It was 0, meaning not supported */
            }
            else if (checkPidsOver20 && (position > 32 && position < 64))
            {
                /* We are in the second range. Now we must offset the PID back 32, to get the position in the list */
                position = position - 32;

                if (binaryEncodedString2.at(position-1) == QChar('1'))
                    m_sensors.at(i)->setSupported(true); /* Is supported, set to true */
                else
                    m_sensors.at(i)->setSupported(false); /* Not supported, set to false */
            }
            else
                m_sensors.at(i)->setSupported(false); /* Sensor met none, so set to false incase vehicle doesn't support */
        }
    }

}

bool Automon::setSensorFrequency(Sensor * sensor, int frequency) const
{
    /*
        This method is responsible for updating the sensor's frequency in the serial I/O thread.
        The frequency determines how often it gets access to the ELM327 to contact the ECU.
        No point checking coolant temperature too often for example, as it changes slowly relative to
        engine RPM.
    */

    if (sensor == NULL)
    {
#ifdef DEBUGAUTOMON
        qDebug() << "Could not set frequency of sensor since pointer value was invalid";
#endif
        return false;
    }

#ifdef DEBUGAUTOMON
    qDebug() << "Setting frequency for sensor \"" << sensor->getCommand() << "\" to " << QString::number(frequency);
#endif

    /* Set the sensor's frequency to what was defined by calling method */
    sensor->setFrequency(frequency);

    return true;
}



bool Automon::addActiveSensor(Sensor * sensor)
{
    /*
        This method is responsible for taking a sensor pointer, and adding it to the active sensor list in
        the serial I/O thread. It also keeps it's own list for reference.
    */

    if (sensor == NULL)
    {
#ifdef DEBUGAUTOMON
        qDebug() << "Could not add sensor to monitoring list since pointer value was invalid";
#endif
        return false;
    }

    /* Only allow adding to the serial I/O thread if actually supported by vehicle. This avoids "NO DATA>" responses */
    if (!sensor->isSupported())
        return false;

    /* Append the sensor to the current active list locally and add it to the serial thread list */
    m_activeSensors.append(sensor);
    m_serialHelper->addActiveSensor(m_activeSensors.at(m_activeSensors.size()-1));

#ifdef DEBUGAUTOMON
    qDebug() << "Added Sensors to Active List";
#endif

    return true;

}

Sensor * Automon::getActiveSensorByCommand(QString command) const
{
    /*
      This is a convinence method that finds an Active sensor's pointer based on it's command
    */

    for(int i = 0; i < m_activeSensors.size(); i++)
        if (command.compare(m_activeSensors[i]->getCommand()) == 0)
            return m_activeSensors[i];

    /* Sensor wasn't found so return a NULL pointer. This NULL should be checked for by the calling method */
    return NULL;
}

Sensor * Automon::getSensorByCommand(QString command) const
{
    /*
      This is a convinence method that finds a sensor's pointer based on it's command
    */

    for(int i = 0; i < m_sensors.size(); i++)
        if (command.compare(m_sensors[i]->getCommand()) == 0)
            return m_sensors[i];

    /* Sensor wasn't found so return a NULL pointer. This NULL should be checked for by the calling method */
    return NULL;
}

bool Automon::testElmConnectivity() const
{
    /*
        This method is responsible for sending a command (means nothing to us) just to test to see if
        we get back the proper OK response
    */

    Command pseudoCommand;
    pseudoCommand.setCommand("ATPC");

    m_serialHelper->sendCommand(pseudoCommand, 1000);

    QString response = pseudoCommand.getBuffer();

    /* Remove spaces and line break */
    Automon::cleanResponse(response);

    if (response.contains(QRegExp("[OK]")))
        return true; /* Response was found, all is OK */

    return false;
}

bool Automon::init()
{
    /*
        This method is called before anything is done with Automon.
        It is responsible for setting up a connection between the ELM327 and the ECU.
        It has sleeps to prevent commands conflicting with each other
    */

    /* Update the status on the Splash Screen so user knows what is happening */
    emit updateStatus(tr("Testing ELM Connectivity"), Qt::AlignCenter, Qt::white);

//    if (!testElmConnectivity()) // [LA]
//    {
//        /* The ELM327 cannot be contacted! This is a serious problem and should not happen */
//        throw elmnotcontactable_exception();
//    }

//    emit updateStatus(tr("Initialising ECU to Automon OBDII Bus"), Qt::AlignCenter, Qt::white);

//    if (!initialiseBus())
//    {
//        /*
//            The bus could not be initialised. Communication breakdown between ECU and ELM327.
//            Automon cannot continue at this point
//        */

//        throw init_exception();
//    }


//    emit updateStatus(tr("Bus Successfully Initialized"), Qt::AlignCenter, Qt::white);

//    emit updateStatus(tr("Loading DTC Codes"), Qt::AlignCenter, Qt::white);

//    if (m_dtcHelper)
//        m_dtcHelper->init(); /* Get the DTC Helper to initialise, loading any faults, and checking if MIL on */

//    emit updateStatus(tr("DTC Codes Initialized"), Qt::AlignCenter, Qt::white);

    emit updateStatus(tr("Loading Rules"), Qt::AlignCenter, Qt::white);

    /* Read user saved rules from rules file */
    loadRuleList();

    emit updateStatus(tr("Loading Sensors"), Qt::AlignCenter, Qt::white);

//    /* Add all sensors to Automon and update their support in current vehicle */
//    loadSensors();

//    emit updateStatus(tr("Sensors Loaded"), Qt::AlignCenter, Qt::white);

    return true;
}

bool Automon::startMonitoring()
{
    /*
        This method is responsible for starting the serial I/O thread so the ELM327 is polled for the
        sensor values of the currently added active sensors
    */

    if (!m_serialHelper->isRunning())
    {
        /* Only attempt to start if not already running */
        /* Start the serial thread with TimeCriticalPriority */
        m_serialHelper->start(QThread::TimeCriticalPriority);

        /* A small 1/2 second sleep */
        SleeperThread::msleep(500);

        m_isMonitoring = true;
        return true;
    }

    return false;
}

bool Automon::refreshDTCInformation() const
{
    /*
      Refresh all DTC information by the DTC Helper. Interface method
    */

    m_dtcHelper->refreshDTCInformation();
    return true;
}

bool Automon::checkMil() const
{
    /* Return the checkMIL method. This returns true if MIL is on, otherwise off. */

    return m_dtcHelper->checkMil();
}

int Automon::getNumCodes()
{
    /* Return the number of DTC codes currently stored in the ECU */
    return m_numberDTCs = m_dtcHelper->getNumberOfCodes();
}

bool Automon::resetMilAndClearCodes()
{
    /* This method is an interface to the DTC helper method, that resets the ECU and clears all DTC codes */
    if (m_numberDTCs > 0)
        m_dtcHelper->resetMilAndClearCodes();
    else
        return false;

    return true;
}

void Automon::stopMonitoring()
{
    /* Stop the serial I/O thread and update the m_isMonitoring state variable */
    m_serialHelper->terminate();
    m_serialHelper->setMonitoring(false);
    m_isMonitoring = false;
}


Automon::~Automon()
{
    /*
        Automon Destructor. Delete any objects created
    */

    delete (m_serialHelper);
    delete (m_dtcHelper);

    for (int i = 0; i < m_sensors.size(); i++)
        delete(m_sensors.at(i));

    for (int i = 0; i < m_dtcs.size(); i++)
        delete(m_dtcs.at(i));

}

QString Automon::getVoltage() const
{
    /*
        Return the current voltage of the battery, known by the ELM327.
        Only allow entry to this method if monitoring is off since it requires communication with ELM327
    */

    if (m_serialHelper->isMonitoring())
        return QString("");


    /* Create command send send to ELM327 */
    Command requestVoltage;
    requestVoltage.setCommand("ATRV");

    m_serialHelper->sendCommand(requestVoltage);

    QString voltageReading;

    /* Remove the line break and prompt character */
    voltageReading = Automon::cleanResponse(requestVoltage.getBuffer());

    /* Return the reading */
    return voltageReading;
}

QString Automon::cleanResponse(QString response)
{
    /* This method is responsible for removing the line break at end of responses and removing the prompt character */

    /* Construct the regular expression that searches for the line break */
    QRegExp removeLineBreak( "\x0D" );
    response.replace(removeLineBreak, "");

    response = response.section("",0,response.size()-1); /* Remove Prompt Character */

    return response;
}

void Automon::removeAllActiveSensors() const
{
    /* This is an interface method that calls the serial helper thread to remove all current active sensors */
    m_serialHelper->removeAllActiveSensors();
}

QStringList Automon::extractSensorsFromRule(QString & rule) const
{
    /*
        This method accepts a rule string. It then does a search for all sensors.
        A sensor would be "s010D" for vehicle speed. It should return 010D as part of the list
        and other sensors as well
    */

    QStringList sensorCommands; /* Our list of sensors to return */

    /* Regular expression that finds a match for the sensors */
    QRegExp checkExp("s[a-fA-F0-9][a-fA-F0-9][a-fA-F0-9][a-fA-F0-9]");

    /* From pos is, the from position, where we search from basically */
    int fromPos = 0;

    /* This is a position of where the sensor was found in the rule string */
    int occurencePos = rule.indexOf(checkExp,fromPos);

    /* While we keep finding sensors */
    while (occurencePos != -1)
    {
        /* Get the sensor name, ie: s010D */
        QString sensorName = rule.section("", occurencePos+1, occurencePos+5);

        /* Get the actual command, ie: remove the s proceeding it. Becomes 010D for eg. */
        QString sensorCommand = sensorName.section("",2,5);

        /* Only if it not empty will we append it */
        if (sensorCommand != "")
            sensorCommands.append(sensorCommand);

        /* Move to after sensor found so we search ahead */
        fromPos = occurencePos + 5;
        occurencePos = rule.indexOf(checkExp,fromPos);
    }

    /* Return the sensors found in the rule */
    return sensorCommands;
}

QString Automon::convertRuleToEnglish(QString rule) const
{

    /*
        This method is responsible for accepting a rule and converting it to a human readable format
        ie: Rule: s010C < 5000 && s010D > 150 becomes: Engine RPM > 5000 AND Vehicle Speed > 150
    */

    /* Create the regular expression that finds the match of a sensor */
    QRegExp checkExp("s[a-fA-F0-9][a-fA-F0-9][a-fA-F0-9][a-fA-F0-9]");

    /* Position to search from in the rule string */
    int fromPos = 0;

    /* Where a match of a sensor is found */
    int occurencePos = rule.indexOf(checkExp,fromPos);

    /* While we have found a match */
    while (occurencePos != -1)
    {

        /* Get the sensor name. If at position 0, we go from 0 - 5, otherwise go from eg: 1 - 6 (5+1) */
        QString sensorName = rule.section("", (occurencePos == 0 ? occurencePos : occurencePos+1), occurencePos+5);

        /* Extract command, removing the proceeding s */
        QString sensorCommand = sensorName.section("",2,5);

        /* Only if we have a none empty string */
        if (sensorCommand != "")
        {
            /* Get a pointer to the string using the command found */
            Sensor * sensor = getSensorByCommand(sensorCommand);

            if (sensor != NULL)
            {
                /*  Only if not null, get the english meaning and replace the s010D with the english meaning for eg */
                QString englishMeaning = sensor->getEnglishMeaning();

                rule.replace(QRegExp(sensorName),englishMeaning);
            }
            else /* Else replace the sensor with UNKNOWN SENSOR. Should never happen */
                rule.replace(QRegExp(sensorName), QString("UNKNOWN SENSOR: "+sensorCommand));
        }

        /* Continue to after where we found last sensor */
        fromPos += occurencePos;
        occurencePos = rule.indexOf(checkExp,fromPos);
    }

    /* Replace the && with AND and the || with OR */
    rule.replace(QRegExp("[&][&]"), "AND");
    rule.replace(QRegExp("[|][|]"), "OR");

    /* Return the rule in human readable format */
    return rule;
}


QString Automon::getElmVersion()
{
    /*
        This method returns the version of the current ELM327 being used
    */

    /* Only check the ELM327 once. If we checked it already, m_elmVersion will not be empty so return it*/
    if(m_elmVersion.compare(""))
        return m_elmVersion;

    /* If here, first time checking. If monitoring return empty string to signify */
    if (m_serialHelper->isMonitoring())
        return QString("");


    QString elmVersion;

    /* Create the command send send it */
    Command getVersion;
    getVersion.setCommand("ATI");

    m_serialHelper->sendCommand(getVersion);

    /* Remove the line break and prompt character */
    elmVersion = Automon::cleanResponse(getVersion.getBuffer());

    /* Return the version assigning the m_elmVersion at the same time */
    return m_elmVersion = elmVersion;
}

QString Automon::getOBDProtocol()
{
    /*
        This method returns the OBD protocol being used by the ECU.
    */

    /* Only check the ELM327 once. If we checked it already, m_protocol will not be empty so return it*/
    if (m_protocol.compare(""))
        return m_protocol;

    /* If here, first time checking. If monitoring return empty string to signify */
    if (m_serialHelper->isMonitoring())
        return QString("");

    /* Create the command send send it */
    Command requestProtocol;
    requestProtocol.setCommand("ATDP");

    m_serialHelper->sendCommand(requestProtocol);

    /* Remove the line break and prompt character */
    QString protocol = Automon::cleanResponse(requestProtocol.getBuffer());

    /* Return the protocol type assigning the m_protocol at the same time */
    return m_protocol = protocol;
}

QString Automon::getOBDStandardType()
{
    /*
        This method returns the OBD standard being used by the ECU.
    */

    /* Only check the ELM327 once. If we checked it already, m_standardType will not be empty so return it*/
    if (m_standardType.compare(""))
        return m_standardType;

    /* If here, first time checking. If monitoring return empty string to signify */
    if (m_serialHelper->isMonitoring())
        return QString("");

    /* Create the command send send it */
    Command mode011C;
    mode011C.setCommand("011C");


    m_serialHelper->sendCommand(mode011C);

    /* We must parse the returned bytes since it is bit encoded */
    QList<int> bytes = getBytes(mode011C);

    QString standard; /* Will hold the standard name */

    /* Now byte A, (bytes[2]) is where the information is stored. */
    if (bytes.size() < 2)
        return QString("Request Could Not Determine Protocol");
    else
    {
        if (bytes[2] == 1)
            standard = "OBD-II as defined by the CARB";
        else if (bytes[2] == 2)
            standard = "OBD as defined by the EPA";
        else if (bytes[2] == 3)
            standard = "OBD and OBD-II";
        else if (bytes[2] == 4)
            standard = "OBD-I";
        else if (bytes[2] == 5)
            standard = "Not meant to comply with any OBD standard";
        else if(bytes[2] == 6)
            standard = "EOBD (European Protocol)";
        else
            standard = "Unknown Protocol Standard";
    }

    /* Return the standard type assigning the m_standardType at the same time */
    return m_standardType = standard;
}

bool Automon::initialiseBus()
{
    /*
        This method is used to restart the ELM327 and activate the ECU->ELM327 bus
    */

#ifdef DEBUGAUTOMON
    qDebug() << "Initalizing Bus...";
#endif

    /* First we restart the interface. Create the command and send it */

    Command resetInterface;
    resetInterface.setCommand("atz");

    m_serialHelper->sendCommand(resetInterface);

    /* Keep checking response until it gets to > meaning ELM327 up and running */
    while(QString(resetInterface.getBuffer().replace(QRegExp("\x0D"),"").at(resetInterface.getBuffer().replace(QRegExp("\x0D"),"").size()-1)).compare(">"));

    /* Sleep for a second to prevent conflicts with commands */
    SleeperThread::msleep(1000);

#ifdef TURNOFFECHO

    /* Turns off echos. This should always be set really */

    #ifdef DEBUGAUTOMON
        qDebug() << "Turning Off Echo by sending ELM327 ATE0";
    #endif

    /* Create command to turn off ELM327 echoing of commands entered. Useful for validating command sent but time wasting */
    Command turnOffEchoCommand;
    turnOffEchoCommand.setCommand("ATE0");
    m_serialHelper->sendCommand(turnOffEchoCommand);

#endif


#ifdef ADAPTIVETIMING
    /*
        Adaptive timing is an ELM327 feature that on the fly adapts to ECU response times.
        It reduces time it waits for response from ECU resulting in faster response to the Serial I/O.
    */

    /* Wait another second to prevent conflict with commands */
    SleeperThread::msleep(1000);

#ifdef DEBUGAUTOMON
    qDebug() << "Setting Adaptive Timing to excessive mode";
#endif

    /* Send a command turn on excessive adaptive timing mode (ATAT2) */
    Command turnOnAdaptiveTiming;
    turnOnAdaptiveTiming.setCommand("ATAT2");
    m_serialHelper->sendCommand(turnOnAdaptiveTiming);

#endif

    SleeperThread::msleep(1000);

    /* Ok now that the ELM327 is initialized, time to wake up the OBD bus */

    Command initialiseBus;
    initialiseBus.setCommand("0100");

    SleeperThread::msleep(1000);

    m_serialHelper->sendCommand(initialiseBus);

    QString returnedBytes = initialiseBus.getBuffer();

    returnedBytes.replace(QRegExp("\x0D"), "");
    returnedBytes.replace(QRegExp(" "), "");

    /* Do check to ensure correct response received */
    if (!returnedBytes.contains(QRegExp("UNABLETOCONNECT")) && QString(returnedBytes.at(returnedBytes.size()-1)).compare(">") == 0)
        return true;

    /* Initialization of bus failed :( */
    return false;
}


QString Automon::getVin()
{

    /*
        This method is responsible for returnining the VIN (Vehicle ID Number) of the car in a QString
        Send command is mode "09" with Pid "02".
        Result is 5 lines A is line ordering flag. B-E ASCII coded VIN digits.

        The 49 02 0n in all cases is ignored. We only interested in none NULL
        characters after that, so in example below:
        4F 5A 45 4E 45 4C 45 4B 54 52 4F 4E 49 4B 31 32 33
            =
        "OZENELEKTRONIK123"

        Example result:
        49 02 01 00 00 00 4F
        49 02 02 5A 45 4E 45
        49 02 03 4C 45 4B 54
        49 02 04 52 4F 4E 49
        49 02 05 4B 31 32 33

        >
    */


    /* We only query the ELM327 once for the VIN. If we done it already, m_vinNumber will have it so return */
    if (m_vinNumber.compare(""))
        return m_vinNumber;

    /* Send command, its a mode 9, command 2 */
    Command vinCommand;
    vinCommand.setCommand("0902");

    m_serialHelper->sendCommand(vinCommand);

    QString returnedBuffer = vinCommand.getBuffer();

    /* Remove spaces to make it easier to parse */
    QRegExp rx( " " );
    returnedBuffer.replace(rx, "");

    /* Split each Line */
    QStringList lineList = returnedBuffer.split("\x0D");

    /*
        Next for each line, we will cut out the "49020n" so simply a
        sub string from 6 to line.size()
    */

    QString fullLine;

    /* Put each line into a full line QSWtring */
    for (int i = 0; i < lineList.size(); i++)
    {
        QString thisLine = lineList.at(i);
        thisLine = thisLine.section("",7,thisLine.size());
        QRegExp nullFinder("00+");
        thisLine.replace(nullFinder,"");
        fullLine += thisLine;
    }


    /* The response should be even number so if not, return error */
    if (fullLine.size() % 2 !=0)
    {

#ifdef DEBUGAUTOMON
        qDebug("The returned string for VIN was not of equal bytes. Error");
#endif
        return QString("The Returned Number of Bytes for VIN was not even. Read Error");
    }

    QString vinNumber;

    /* Now go through the full line, catching each pair, "FF" as it is a single byte */
    for (int i=0; i < fullLine.size(); i += 2)
    {
        QString hexByte = fullLine.section("",i+1,i+2);
        vinNumber += QByteArray::fromHex(hexByte.toLatin1());
    }

    /* A VIN Number has to be 17 characters long by the standard! */
    if (vinNumber.size() != 17)
        return QString("Invalid VIN!");
    else
        return m_vinNumber = vinNumber; /* We have the correct VIN number so return */
}

QList<Sensor*> Automon::getFreezeFrame() const
{
    /*
        This method is responsible for returning a list of sensors that represent the freeze frame data
        This functinoality is not implemented in Automon yet. It will not be part of the FYP due to timing constraints
        and force of rescope due to this.
    */

    QList<Sensor*> temp;

    return temp;
}

QList<DTC*> Automon::getDTCs() const
{
    /* This method returns a list of DTC pointers to DTC codes stored by the dtc Helper */
    return m_dtcHelper->getCodesFound();
}

bool Automon::sendCommand(Command & command)
{
    /*
        This method is simply a interface/wrapper to the serial helper sendCommand method.
        It is used to send commands to the ELM327 and get response buffer populated
     */

    m_serialHelper->sendCommand(command);
    return true;
}

bool Automon::connectSensorToSlot(Sensor * sender, QObject * receiver) const
{
    /*
        This method is responsible for connecting the sensor's signal that emits when its value
        changes (improves performance) to a display slot of the receiver object.
        The receiver object has to have a display slot implemented or the connection will be invalid.
        However this will not crash the program due to the Meta Object Compiler behaviour of QT
    */

    if (sender == NULL || receiver == NULL)
    {
#ifdef DEBUGAUTOMON
        qDebug() << "One of the signal/slot pointers, either sender or receiver were NULL. Cannot connect. May crash";
#endif
        return false;
    }

    if (QObject::connect(sender, SIGNAL(changeOccurred(double)), receiver, SLOT(display(double))))
        return true; /* Connection was implemented successfully */
    else
    {
#ifdef DEBUGAUTOMON
        qDebug() << "There was an error connecting the \"" << sender->getCommand() << "\" sensor to the slot";
#endif
        /* An error connecting to slot */
        return false;
    }
}

bool Automon::disconnectSensorFromSlot(Sensor * sender, QObject * receiver) const
{
    /*
      This method is responsible for disconnecting a sensor's changeOccured signal from a slot
      that was already connected by the connectSensorToSlot method above
    */

    if (sender == NULL || receiver == NULL)
    {
        /* Sender or Receiver were null. Cannot countinue */
#ifdef DEBUGAUTOMON
        qDebug() << "One of the signal/slot pointers, either sender or receiver were NULL. Cannot connect. May crash";
#endif
        return false;
    }

    if (QObject::disconnect(sender, SIGNAL(changeOccurred(double)), receiver, SLOT(display(double))))
        return true; /* Disconnection successful */
    else
    {
        /* Failed to disconnect, maybe was never connected in first place */
#ifdef DEBUGAUTOMON
        qDebug() << "There was an error connecting the \"" << sender->getCommand() << "\" sensor to the slot";
#endif
        return false;
    }
}

void Automon::addSensor(Sensor * newSensor)
{
    /* Simply add a sensor pointer to the m_sensor list */
    m_sensors << newSensor;
}

bool Automon::addActiveSensorByCommand(QString command)
{
    /*
        This method takes a sensor command and adds the associated sensor pointer to the serial I/O threads sensor list
        and the local m_activeSensor list
    */

    for(int i = 0; i < m_sensors.size(); i++)
    {
        /* For each sensor in the m_sensors list, do a search and compare to the command. */
        if (m_sensors.at(i)->isSupported() && command.compare(m_sensors.at(i)->getCommand()) == 0)
        {
            /* We found the sensor so now add it to the m_activeSensor list */

            bool found = false; /* Assume not found already in the list */

            for(int j = 0; j < m_activeSensors.size(); j++)
                if (command.compare(m_activeSensors[j]->getCommand()) == 0)
                    found = true; /* Already in the list, don't add! */

            if (!found)
            {
                /* Only add to list if not already in there */
                m_activeSensors.append(m_sensors[i]);
                m_serialHelper->addActiveSensor(m_activeSensors.at(m_activeSensors.size()-1));
            }

            return true;
        }
    }

    /* If got here, the sensor that was requested wasn't found in the m_sensors list */
    return false;
}

QList<Sensor*> Automon::getAllSensors() const
{
    /* Return a list of sensor pointers */
    return m_sensors;
}

bool Automon::removeActiveSensorByCommand(QString command)
{
    /*
      This method is responsible for removing a sensor pointer from the serial I/O
      active sensor list. It accepts a command and using this removes it
    */

    for (int i = 0; i < m_activeSensors.size(); i++)
    {
        /* Go through each m_activeSensor and check if a match found with command inputted to this method */
        if (command.compare(m_activeSensors.at(i)->getCommand()) == 0)
        {
            /* Sensor match found so remove at this location in the local m_activeSensor list and remove from the serial I/O
               thread
            */
            m_activeSensors.removeAt(i);
            m_serialHelper->removeActiveSensorByCommand(command);
            return true;
        }
    }

    /* Sensor wasn't found */
    return false;
}
