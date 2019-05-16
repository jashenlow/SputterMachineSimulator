#include "process_serial_data.h"

ProcessSerialData::ProcessSerialData(QObject *parent) :
    QObject (parent)
{
    m_serialConfig = new SerialConfig(this);

    KJLcommsLinkTime = 3000; //Using this function to set this parameter out of convenience. =) **3000ms is the default stated in the manual**
    m_KJLcommsLinkTimer = new QTimer(this);
    m_KJLcommsLinkTimer->setTimerType(Qt::PreciseTimer);
    m_KJLcommsLinkTimer->setInterval(KJLcommsLinkTime);
    m_KJLcommsLinkTimer->setSingleShot(true);

    KJL2commsLinkTime = 3000; //Using this function to set this parameter out of convenience. =) **3000ms is the default stated in the manual**
    m_KJL2commsLinkTimer = new QTimer(this);
    m_KJL2commsLinkTimer->setTimerType(Qt::PreciseTimer);
    m_KJL2commsLinkTimer->setInterval(KJL2commsLinkTime);
    m_KJL2commsLinkTimer->setSingleShot(true);

    m_ui = nullptr;
}

ProcessSerialData::~ProcessSerialData()
{
    delete m_ui;
}

void ProcessSerialData::setUiPointers(Ui::MainWindow *ptr)
{
    if (ptr != nullptr)
    {
        m_ui = ptr;
    }
    emit KJLSetEchoMode(true);
    connect(m_KJLcommsLinkTimer, &QTimer::timeout, this, &ProcessSerialData::KJLcommsLinkTimeout, Qt::QueuedConnection);
    emit KJL2SetEchoMode(true);
    connect(m_KJL2commsLinkTimer, &QTimer::timeout, this, &ProcessSerialData::KJL2commsLinkTimeout, Qt::QueuedConnection);
}

void ProcessSerialData::KJLcommsLinkTimeout()
{
    emit sendToLog("KJL Communication Link timed out after " + QString::number(KJLcommsLinkTime) + "ms.");
    emit KJLSetOutput(false);
    emit KJLSetCommsLinkStatus(false);
}

void ProcessSerialData::KJL2commsLinkTimeout()
{
    emit sendToLog("KJL2 Communication Link timed out after " + QString::number(KJL2commsLinkTime) + "ms.");
    emit KJL2SetOutput(false);
    emit KJL2SetCommsLinkStatus(false);
}

QByteArray ProcessSerialData::assemblePacket(QByteArray packet)
{
    int dataLength = 0;
    const int cesarAddress = 0x01;
    int headerByte;

    if (packet.length() > 1)
    {
        for (int i = 1; i < packet.length(); i++)   //Determine length of data bytes.
        {
            dataLength++;
        }
    }
    if (dataLength < 7)
    {
        headerByte = (cesarAddress << 3) | dataLength;  //Create the header byte.
        packet.prepend(char(headerByte));
    }
    else
    {
        headerByte = (cesarAddress << 3) | 0x07;  //Create the header byte.
        packet.insert(1, char(dataLength)); //Insert optional data byte.
        packet.prepend(char(headerByte));
    }

    packet.append(char(m_serialConfig->calculate_checkSum(packet)));    //Append checksum value to the end of the packet.
    return packet;
}

void ProcessSerialData::processCesar(uint8_t command, QByteArray data)
{
    QByteArray replyPacket;
    int regulationMode = 0;

    //qDebug() << "cesar_readyToProcess emitted!";

    switch (command)
    {
        case 0xA8:   //168  //Report external feedback. (DC Bias) (unit: V)
            replyPacket.append(char(command));
            replyPacket.append(char(m_ui->leExternalFeedback->text().toInt() & 0xFF));
            replyPacket.append(char(((m_ui->leExternalFeedback->text().toInt()) >> 8) & 0xFF));

            replyPacket = assemblePacket(replyPacket);
            emit writeToCesarPort(replyPacket);
            //TODO: emit signal in cesar_operation.cpp
            break;

        case 0xA5:  //165   //Report Forward Power. (unit: W)
            replyPacket.append(char(command));
            replyPacket.append(char(m_ui->leForwardPower->text().toInt() & 0xFF));
            replyPacket.append(char((m_ui->leForwardPower->text().toInt() >> 8) & 0xFF));

            replyPacket = assemblePacket(replyPacket);
            emit writeToCesarPort(replyPacket);
            //TODO: emit signal in cesar_operation.cpp
            break;

        case 0xA6:  //166   //Report Reflected Power. (unit: W)
            replyPacket.append(char(command));
            replyPacket.append(char(m_ui->leReflectedPower->text().toInt() & 0xFF));
            replyPacket.append(char((m_ui->leReflectedPower->text().toInt() >> 8) & 0xFF));

            replyPacket = assemblePacket(replyPacket);
            emit writeToCesarPort(replyPacket);
            //TODO: emit signal in cesar_operation.cpp
            break;

        case 0xAF:  //166   //Report Capacitor Positions. (unit: W) (Position value: 0 to 1000)
            replyPacket.append(char(command));
            replyPacket.append(char(m_ui->leLoadCapPos->text().toInt() & 0xFF));    //Load capacitor 1st byte.
            replyPacket.append(char((m_ui->leLoadCapPos->text().toInt() >> 8) & 0xFF)); //Load capacitor 2nd byte.
            replyPacket.append(char(m_ui->leTuneCapPos->text().toInt() & 0xFF));    //Tune capacitor 1st byte.
            replyPacket.append(char((m_ui->leTuneCapPos->text().toInt() >> 8) & 0xFF)); //Tune capacitor 2nd byte.

            replyPacket = assemblePacket(replyPacket);
            emit writeToCesarPort(replyPacket);
            //TODO: emit signal in cesar_operation.cpp
            break;

        case 0xA4:  //166   //Report Set Point and Regulation Mode. (unit: W/V, int) (6 = Forward Power, 7 = Load Power, 8 = External Power)
            replyPacket.append(char(command));
            replyPacket.append(char(m_ui->leSetPoint->text().toInt() & 0xFF));  //Byte 0.
            replyPacket.append(char((m_ui->leSetPoint->text().toInt() >> 8) & 0xFF));   //Byte 1.

            if (m_ui->leRegMode->text() == "Forward Power")
            {
                regulationMode = 6;
            }
            else if (m_ui->leRegMode->text() == "Load Power")
            {
                regulationMode = 7;
            }
            else if (m_ui->leRegMode->text() == "External Power")
            {
                regulationMode = 8;
            }
            replyPacket.append(char(regulationMode));   //Byte 2.

            replyPacket = assemblePacket(replyPacket);
            emit writeToCesarPort(replyPacket);
            //TODO: emit signal in cesar_operation.cpp
            break;
    }
}

void ProcessSerialData::processKJL(QByteArray command)  //TODO: implement error handling for out-of-range values.
{
    QByteArray replyCommand;
    QByteArray commandData;
    QString commandDataString;

    if (command == "G") //Enable Output.
    {
        replyCommand.append('\r');
        emit writeToKJLPort(replyCommand);
        emit KJLSetOutput(true);
    }
    else if (command == "S")    //Disable Output
    {
        replyCommand.append('\r');
        emit writeToKJLPort(replyCommand);
        emit KJLSetOutput(false);
    }
    else if (command.contains(" W") && command.back() == 'W')   //Set power setpoint.
    {
        replyCommand.append('\r');
        emit writeToKJLPort(replyCommand);

        for (int i = 0; i < command.indexOf(' '); i++)
        {
            commandData.append(command[i]);
        }
        commandDataString = QString(commandData);   //Converts the bytes in commandData into a QString of their equivalent ASCII characters.
        if (commandDataString.toInt() > 0)
        {
            emit KJLSetPowerSetPoint(commandDataString.toInt());
        }
    }
    else if (command.contains(" WG") && command.back() == 'G')   //Set power setpoint & enable output.
    {
        replyCommand.append('\r');
        emit writeToKJLPort(replyCommand);

        for (int i = 0; i < command.indexOf(' '); i++)
        {
            commandData.append(command[i]);
        }
        commandDataString = QString(commandData);   //Converts the bytes in commandData into a QString of their equivalent ASCII characters.
        if (commandDataString.toInt() > 0)
        {
            emit KJLSetPowerSetPoint(commandDataString.toInt());
            emit KJLSetOutput(true);
        }
    }
    else if (command == "SERIAL" || command == "***" || command == "ANALOG" || command == "PANEL")   //Set control source.
    {
        replyCommand.append('\r');
        emit writeToKJLPort(replyCommand);
        emit KJLSetControlSource(command);

    }
    else if (command.contains(" MPL") && command.back() == 'L')   //Set load capacitor position.
    {
        replyCommand.append('\r');
        emit writeToKJLPort(replyCommand);

        for (int i = 0; i < command.indexOf(' '); i++)
        {
            commandData.append(command[i]);
        }
        commandDataString = QString(commandData);   //Converts the bytes in commandData into a QString of their equivalent ASCII characters.

        if ((commandDataString.toInt()>= 0) && (commandDataString.toInt() <= 100))
        {
            emit KJLSetCapPosition(true, commandDataString.toInt());   //true = load cap.
        }
    }
    else if (command.contains(" MPT") && command.back() == 'T')   //Set tune capacitor position.
    {
        replyCommand.append('\r');
        emit writeToKJLPort(replyCommand);

        for (int i = 0; i < command.indexOf(' '); i++)
        {
            commandData.append(command[i]);
        }
        commandDataString = QString(commandData);   //Converts the bytes in commandData into a QString of their equivalent ASCII characters.

        if ((commandDataString.toInt()>= 0) && (commandDataString.toInt() <= 100))
        {
            emit KJLSetCapPosition(false, commandDataString.toInt());   //false = tune cap.
        }
    }
    else if ((command == "EU") || (command == "DU"))   //Enable/Disable output ramping.
    {
        replyCommand.append('\r');
        emit writeToKJLPort(replyCommand);

        command == "EU" ? emit KJLSetOutputRamping(true) : emit KJLSetOutputRamping(false);
    }
    else if (command.contains(" DN") && (command.back() == 'N'))    //Set ramp down time.
    {
        for (int i = 0; i < command.indexOf(' '); i++)
        {
            commandData.append(command[i]);
        }
        commandDataString = QString(commandData);

        if ((commandDataString.toInt() >= 1) && (commandDataString.toInt() <= 9999))
        {
            emit KJLSetRampDownTime(commandDataString.toInt());
        }
    }
    else if (command.contains(" UP") && (command.back() == 'P'))    //Set ramp up time.
    {
        for (int i = 0; i < command.indexOf(' '); i++)
        {
            commandData.append(command[i]);
        }
        commandDataString = QString(commandData);

        if ((commandDataString.toInt() >= 1) && (commandDataString.toInt() <= 9999))
        {
            emit KJLSetRampUpTime(commandDataString.toInt());
        }
    }
    else if ((command == "0?") || (command == "V?"))   //Query External Feedback (DC Bias Voltage)
    {
        emit KJLQueryExternalFeedback();
    }
    else if (command == "W?")   //Query forward power.
    {
        emit KJLQueryForwardPower();
    }
    else if (command == "R?")   //Query reflected power.
    {
        emit KJLQueryReflectedPower();
    }
    else if (command == "Q")   //Query status (long form).
    {
        if (m_KJLcommsLinkTimer->isActive())
        {
            m_KJLcommsLinkTimer->stop();
        }
        m_KJLcommsLinkTimer->start();

        emit KJLSetCommsLinkStatus(true);
        emit KJLQueryStatus();
    }
    else    //Command not recognised/accepted.
    {
        replyCommand.append('N');
        replyCommand.append('\r');
        emit writeToKJLPort(replyCommand);
    }
}

void ProcessSerialData::processKJL2(QByteArray command)  //TODO: implement error handling for out-of-range values.
{
    QByteArray replyCommand;
    QByteArray commandData;
    QString commandDataString;

    if (command == "G") //Enable Output.
    {
        replyCommand.append('\r');
        emit writeToKJL2Port(replyCommand);
        emit KJL2SetOutput(true);
    }
    else if (command == "S")    //Disable Output
    {
        replyCommand.append('\r');
        emit writeToKJL2Port(replyCommand);
        emit KJL2SetOutput(false);
    }
    else if (command.contains(" W") && command.back() == 'W')   //Set power setpoint.
    {
        replyCommand.append('\r');
        emit writeToKJL2Port(replyCommand);

        for (int i = 0; i < command.indexOf(' '); i++)
        {
            commandData.append(command[i]);
        }
        commandDataString = QString(commandData);   //Converts the bytes in commandData into a QString of their equivalent ASCII characters.
        if (commandDataString.toInt() > 0)
        {
            emit KJL2SetPowerSetPoint(commandDataString.toInt());
        }
    }
    else if (command.contains(" WG") && command.back() == 'G')   //Set power setpoint & enable output.
    {
        replyCommand.append('\r');
        emit writeToKJL2Port(replyCommand);

        for (int i = 0; i < command.indexOf(' '); i++)
        {
            commandData.append(command[i]);
        }
        commandDataString = QString(commandData);   //Converts the bytes in commandData into a QString of their equivalent ASCII characters.
        if (commandDataString.toInt() > 0)
        {
            emit KJL2SetPowerSetPoint(commandDataString.toInt());
            emit KJL2SetOutput(true);
        }
    }
    else if (command == "SERIAL" || command == "***" || command == "ANALOG" || command == "PANEL")   //Set control source.
    {
        replyCommand.append('\r');
        emit writeToKJL2Port(replyCommand);
        emit KJL2SetControlSource(command);

    }
    else if (command.contains(" MPL") && command.back() == 'L')   //Set load capacitor position.
    {
        replyCommand.append('\r');
        emit writeToKJL2Port(replyCommand);

        for (int i = 0; i < command.indexOf(' '); i++)
        {
            commandData.append(command[i]);
        }
        commandDataString = QString(commandData);   //Converts the bytes in commandData into a QString of their equivalent ASCII characters.

        if ((commandDataString.toInt()>= 0) && (commandDataString.toInt() <= 100))
        {
            emit KJL2SetCapPosition(true, commandDataString.toInt());   //true = load cap.
        }
    }
    else if (command.contains(" MPT") && command.back() == 'T')   //Set tune capacitor position.
    {
        replyCommand.append('\r');
        emit writeToKJL2Port(replyCommand);

        for (int i = 0; i < command.indexOf(' '); i++)
        {
            commandData.append(command[i]);
        }
        commandDataString = QString(commandData);   //Converts the bytes in commandData into a QString of their equivalent ASCII characters.

        if ((commandDataString.toInt()>= 0) && (commandDataString.toInt() <= 100))
        {
            emit KJL2SetCapPosition(false, commandDataString.toInt());   //false = tune cap.
        }
    }
    else if ((command == "EU") || (command == "DU"))   //Enable/Disable output ramping.
    {
        replyCommand.append('\r');
        emit writeToKJL2Port(replyCommand);

        command == "EU" ? emit KJL2SetOutputRamping(true) : emit KJL2SetOutputRamping(false);
    }
    else if (command.contains(" DN") && (command.back() == 'N'))    //Set ramp down time.
    {
        for (int i = 0; i < command.indexOf(' '); i++)
        {
            commandData.append(command[i]);
        }
        commandDataString = QString(commandData);

        if ((commandDataString.toInt() >= 1) && (commandDataString.toInt() <= 9999))
        {
            emit KJL2SetRampDownTime(commandDataString.toInt());
        }
    }
    else if (command.contains(" UP") && (command.back() == 'P'))    //Set ramp up time.
    {
        for (int i = 0; i < command.indexOf(' '); i++)
        {
            commandData.append(command[i]);
        }
        commandDataString = QString(commandData);

        if ((commandDataString.toInt() >= 1) && (commandDataString.toInt() <= 9999))
        {
            emit KJL2SetRampUpTime(commandDataString.toInt());
        }
    }
    else if ((command == "0?") || (command == "V?"))   //Query External Feedback (DC Bias Voltage)
    {
        emit KJL2QueryExternalFeedback();
    }
    else if (command == "W?")   //Query forward power.
    {
        emit KJL2QueryForwardPower();
    }
    else if (command == "R?")   //Query reflected power.
    {
        emit KJL2QueryReflectedPower();
    }
    else if (command == "Q")   //Query status (long form).
    {
        if (m_KJL2commsLinkTimer->isActive())
        {
            m_KJL2commsLinkTimer->stop();
        }
        m_KJL2commsLinkTimer->start();

        emit KJL2SetCommsLinkStatus(true);
        emit KJL2QueryStatus();
    }
    else    //Command not recognised/accepted.
    {
        replyCommand.append("N\r");
        emit writeToKJL2Port(replyCommand);
    }
}

void ProcessSerialData::processMFC(QByteArray command)
{
    if ((command == "?RG1") || (command == "?RG2"))     //Query range of channel 1 or 2.
    {
        command == "?RG1" ? emit MFCQueryRange(1) : emit MFCQueryRange(2);
    }
    else if (command == "?RT")      //Query remote mode status.
    {
        emit MFCQueryRemoteMode();
    }
    else if (command.contains("RT") && (command.contains("OFF") || command.contains("ON")))     //Set remote mode ON/OFF.
    {
        emit MFCSetRemoteMode(command.mid(3));
    }
    else if ((command.contains("RG1") || command.contains("RG2")) && !command.contains('?'))    //Set channel range and unit.
    {
        emit MFCSetRange(command[2], command.mid(4));
    }
    else if (command.contains("VL") && !command.contains('?'))      //Set valve states ON/OFF.
    {
        emit MFCSetValveState(command[2], command.mid(4));
    }
    else if (command == "?VL1" || command == "?VL2")        //Query valve states.
    {
        emit MFCQueryValveState(command.back());
    }
    else if (command == "AV1" || command == "AV2")  //Query actual value.
    {
        emit MFCQueryActualValue(command[2]);
    }
    else if (command.contains("AV") && command.contains(','))   //Query actual value & set sepoint.
    {
        QString actualValue;

        actualValue = command.mid(command.indexOf(',') + 1);
        emit MFCQueryActualValueSetSetpoint(command[2], actualValue.toDouble());

    }
    else    //Syntax Error. Invalid cmmand detected.
    {
        emit MFCInvalidCmd("#E010");  //Send syntax error code.
    }
}

void ProcessSerialData::processHofi(QByteArray command)
{
    if (command == "HOFIMAINE")     //Turn Hofi On.
    {
        emit hofiSetOnOff(true);
    }
    else if (command == "HOFIMAINA")    //Turn Hofi Off.
    {
        emit hofiSetOnOff(false);
    }
    else if (command.contains("HOFIPORT"))  //Set current Hofi port.
    {
        emit hofiSetPort(command[8]);
    }
    else if (command == "HOFISTATU")    //Query current Hofi status.
    {
        emit hofiQueryCurrentState();
    }
}
