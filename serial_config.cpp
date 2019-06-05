#include "serial_config.h"

SerialConfig::SerialConfig(QObject *parent) :
    QObject(parent)
{
    m_cesarSerial = new QSerialPort(this);
    m_KJLSerial = new QSerialPort(this);
    m_KJL2Serial = new QSerialPort(this);
    m_MFCSerial = new QSerialPort(this);
    m_hofiSerial = new QSerialPort(this);

    m_ACKTimer = new QTimer(this);
    m_ACKTimer->setInterval(100);  //Set 100ms interval for an ACK timeout as stated in the cesar manual.
    m_ACKTimer->setTimerType(Qt::PreciseTimer);
    m_ACKTimer->setSingleShot(true);
    connect(m_ACKTimer, &QTimer::timeout, this, &SerialConfig::cesarACKTimeout, Qt::QueuedConnection);

    connect(m_cesarSerial, SIGNAL(readyRead()), this, SLOT(readCesarData()), Qt::QueuedConnection);
    connect(m_KJLSerial, SIGNAL(readyRead()), this, SLOT(readKJLData()), Qt::QueuedConnection);
    connect(m_KJL2Serial, SIGNAL(readyRead()), this, SLOT(readKJL2Data()), Qt::QueuedConnection);
    connect(m_MFCSerial, SIGNAL(readyRead()), this, SLOT(readMFCData()), Qt::QueuedConnection);
    connect(m_hofiSerial, SIGNAL(readyRead()), this, SLOT(readHofiData()), Qt::QueuedConnection);

    //The following ports are used by the PC on the actual machine. The sputterautomation program will try to connect to these. Hence, they shouldn't be enumerated.
    forbiddenSerialPorts << "COM1" << "COM2" << "COM3" << "COM4" << "COM6";

    cesarACKTimer_flag = false;
}

SerialConfig::~SerialConfig()
{
    closePorts();
}

void SerialConfig::enumeratePorts()
{
    if (QSerialPortInfo::availablePorts().size() != 0)
    {
        foreach (const QSerialPortInfo &port, QSerialPortInfo::availablePorts())
        {
            serialNames << port.portName();
        }
        for (int i = 0; i < forbiddenSerialPorts.length(); i++) //Removes elements from serialNames based on forbiddenSerialPorts
        {
            for (int j = 0; j < serialNames.length(); j++)
            {
                if (forbiddenSerialPorts[i] == serialNames[j])
                {
                    serialNames.removeAt(j);
                }
            }
        }
    }
}

void SerialConfig::initSerialPorts()
{
    //Cesar
    m_cesarSerial->setPortName(serialNames[0]);
    m_cesarSerial->setBaudRate(m_CesarSettings.cesar_baudRate);
    m_cesarSerial->setDataBits(m_CesarSettings.cesar_dataBits);
    m_cesarSerial->setParity(m_CesarSettings.cesar_parity);
    m_cesarSerial->setStopBits(m_CesarSettings.cesar_stopBits);
    m_cesarSerial->setFlowControl(m_CesarSettings.cesar_flowControl);

    //KJL
    m_KJLSerial->setPortName(serialNames[1]);
    m_KJLSerial->setBaudRate(m_KJLSettings.kjl_baudRate);
    m_KJLSerial->setDataBits(m_KJLSettings.kjl_dataBits);
    m_KJLSerial->setParity(m_KJLSettings.kjl_parity);
    m_KJLSerial->setStopBits(m_KJLSettings.kjl_stopBits);
    m_KJLSerial->setFlowControl(m_KJLSettings.kjl_flowControl);

    //KJL2
    m_KJL2Serial->setPortName(serialNames[2]);
    m_KJL2Serial->setBaudRate(m_KJL2Settings.kjl2_baudRate);
    m_KJL2Serial->setDataBits(m_KJL2Settings.kjl2_dataBits);
    m_KJL2Serial->setParity(m_KJL2Settings.kjl2_parity);
    m_KJL2Serial->setStopBits(m_KJL2Settings.kjl2_stopBits);
    m_KJL2Serial->setFlowControl(m_KJL2Settings.kjl2_flowControl);

    //MFC
    m_MFCSerial->setPortName(serialNames[3]);
    m_MFCSerial->setBaudRate(m_MFCSettings.mfc_baudRate);
    m_MFCSerial->setDataBits(m_MFCSettings.mfc_dataBits);
    m_MFCSerial->setParity(m_MFCSettings.mfc_parity);
    m_MFCSerial->setStopBits(m_MFCSettings.mfc_stopBits);
    m_MFCSerial->setFlowControl(m_MFCSettings.mfc_flowControl);

    //Hofi
    m_hofiSerial->setPortName(serialNames[4]);
    m_hofiSerial->setBaudRate(m_HofiSettings.hofi_baudRate);
    m_hofiSerial->setDataBits(m_HofiSettings.hofi_dataBits);
    m_hofiSerial->setParity(m_HofiSettings.hofi_parity);
    m_hofiSerial->setStopBits(m_HofiSettings.hofi_stopBits);
    m_hofiSerial->setFlowControl(m_HofiSettings.hofi_flowControl);

    m_cesarSerial->open(QIODevice::ReadWrite);
    m_cesarSerial->isOpen() ? emit sendToLog("Cesar port opened.") : emit sendToLog("Error opening Cesar port.");

    m_KJLSerial->open(QIODevice::ReadWrite);
    m_KJLSerial->isOpen() ? emit sendToLog("KJL port opened.") : emit sendToLog("Error opening KJL port.");

    m_KJL2Serial->open(QIODevice::ReadWrite);
    m_KJL2Serial->isOpen() ? emit sendToLog("KJL2 port opened.") : emit sendToLog("Error opening KJL2 port.");

    m_MFCSerial->open(QIODevice::ReadWrite);
    m_MFCSerial->isOpen() ? emit sendToLog("MFC port opened.") : emit sendToLog("Error opening MFC port.");

    m_hofiSerial->open(QIODevice::ReadWrite);
    m_hofiSerial->isOpen() ? emit sendToLog("Hofi port opened.") : emit sendToLog("Error opening Hofi port.");
}

void SerialConfig::closePorts()
{
    if (m_cesarSerial->isOpen())
    {
        m_cesarSerial->close();
        !m_cesarSerial->isOpen() ? emit sendToLog("Cesar port closed.") : emit sendToLog("Error closing Cesar port.");
        cesarWriteQueue.clear();
    }
    if (m_KJLSerial->isOpen())
    {
        m_KJLSerial->close();
        !m_KJLSerial->isOpen() ? emit sendToLog("KJL port closed.") : emit sendToLog("Error closing KJL port.");
        KJLWriteQueue.clear();
        KJLEchoCmdQueue.clear();
    }
    if (m_KJL2Serial->isOpen())
    {
        m_KJL2Serial->close();
        !m_KJL2Serial->isOpen() ? emit sendToLog("KJL2 port closed.") : emit sendToLog("Error closing KJL2 port.");
        KJL2WriteQueue.clear();
        KJL2EchoCmdQueue.clear();
    }
    if (m_MFCSerial->isOpen())
    {
        m_MFCSerial->close();
        !m_MFCSerial->isOpen() ? emit sendToLog("MFC port closed.") : emit sendToLog("Error closing MFC port.");
        MFCWriteQueue.clear();
    }
    if (m_hofiSerial->isOpen())
    {
        m_hofiSerial->close();
        !m_hofiSerial->isOpen() ? emit sendToLog("Hofi port closed.") : emit sendToLog("Error closing Hofi port.");
        hofiWriteQueue.clear();
        hofiReceiveBuffer.clear();
    }
}

void SerialConfig::setKJLEchoMode(bool mode)
{
    KJLEchoMode = mode;
}

void SerialConfig::setKJL2EchoMode(bool mode)
{
    KJL2EchoMode = mode;
}

void SerialConfig::cesarACKTimeout()
{
    cesarACKTimer_flag = true;

    emit sendToLog("Cesar 100ms timeout elapsed. ACK assumed.");
    if (!cesarWriteQueue.isEmpty())
    {
        cesarWriteQueue.removeFirst();
        //qDebug() << "Cesar write queue:" << cesarWriteQueue;
    }
    cesarReceiveBuffer.clear(); //Clear buffer.
    cesarACKTimer_flag = false;

    if (m_ACKTimer->isActive())
    {
        //qDebug() << "ACKTimer still active!";
        m_ACKTimer->stop();
    }
}

void SerialConfig::readValidPacket(QByteArray packet)   //Checks for a valid packet and disassembles it accordingly.
{
    QByteArray dataBytes;
    int dataStartIndex;
    uint8_t receivedCmd;
    int dataLength;
    bool validLength = false;

    if ((packet[0] >> 3) == 0x01)  //Verify that address sent is 0x01.
    {
        dataLength = packet[0] & 0x07;   //Determine number of data bytes.
        //qDebug() << "Data length:" << dataLength;

        receivedCmd = uint8_t(packet[1]); //Determine the command received.
        //qDebug() << "Command:" << receivedCmd;
        if (dataLength == 7)    //More than 6 data bytes.
        {
            if (packet.length() == (dataLength + 4))    //Check for valid packet length.
            {
                validLength = true;
                dataLength = packet[2];
                dataStartIndex = 3;
            }
        }
        else
        {
            if (packet.length() == (dataLength + 3))    //Check for valid packet length.
            {
                validLength = true;
                dataStartIndex = 2;

                if (dataLength != 0)
                {
                    for (int i = dataStartIndex; i < (dataStartIndex + dataLength); i++)
                    {
                        dataBytes.append(packet[i]);
                    }
                    //qDebug() << "Data bytes:" << dataBytes;
                }
            }
        }
        if (validLength == true)
        {
            if (calculate_checkSum(packet) == 0x00)  //No transmission errors.
            {
                //qDebug() << "Cesar received packet: " << cesarReceiveBuffer;
                emit sendToLog("Cesar received packet: " + QString::fromLocal8Bit(cesarReceiveBuffer.toHex('\\')).toUpper());

                //Reply ACK.
                m_cesarSerial->write(QByteArray(1, 0x06), 1);
                m_cesarSerial->waitForBytesWritten(-1);
                emit sendToLog("Cesar ACK sent.");
                emit cesar_readyToProcess(receivedCmd, dataBytes);    //emit signal to cesar_process.
                //TODO: Start timer and wait 100ms for an ACK.
            }
            else
            {
                //Reply NACK.
                m_cesarSerial->write(QByteArray(1, 0x15), 1);
                m_cesarSerial->waitForBytesWritten(-1);
                emit sendToLog("Cesar NACK sent.");
            }
            cesarReceiveBuffer.clear();
        }
    }
}

int SerialConfig::calculate_checkSum(QByteArray packet)
{
    int checkSum = 0x00;

    for (int i = 0; i < packet.length(); i++)
    {
        checkSum ^= packet[i];
    }
    //qDebug() << "Checksum is:" << checkSum;
    return checkSum;
}

void SerialConfig::writeCesarPort(QByteArray packet)
{
    packet.append(char(calculate_checkSum(packet)));
    if (cesarWriteQueue.length() > 0)   //This nested if statement prevents appending duplicates next to the first element.
    {
        if (cesarWriteQueue.first() == packet)
        {
            //Do nothing.
        }
        else
        {
            cesarWriteQueue.append(packet);
        }
    }
    else
    {
        cesarWriteQueue.append(packet);
    }

    //qDebug() << "Cesar write queue:" << cesarWriteQueue;
    if (cesarWriteQueue.length() == 1)
    {
        m_cesarSerial->write(cesarWriteQueue[0], cesarWriteQueue[0].length());
        m_cesarSerial->waitForBytesWritten(-1);
        emit sendToLog("Cesar sent packet: " + QString::fromLocal8Bit(cesarWriteQueue[0].toHex('\\')).toUpper());
        //qDebug() << "Cesar sent packet:" << cesarWriteQueue[0];

        m_ACKTimer->start();   //Start ACK timer.
    }
}

void SerialConfig::writeKJLPort(QByteArray command)
{
    if (KJLEchoMode)
    {
        command.prepend(KJLEchoCmdQueue[0]);
        KJLEchoCmdQueue.removeFirst();
    }
    KJLWriteQueue.append(command);

    //qDebug() << "KJL write queue:" << KJLWriteQueue;
    if (KJLWriteQueue.length() > 0)
    {
        m_KJLSerial->write(KJLWriteQueue[0], KJLWriteQueue[0].length());
        m_KJLSerial->waitForBytesWritten(-1);
        emit sendToLog("KJL sent data: " + KJLWriteQueue[0]);

        KJLWriteQueue.removeFirst();
    }
}

void SerialConfig::writeKJL2Port(QByteArray command)
{
    if (KJL2EchoMode)
    {
        command.prepend(KJL2EchoCmdQueue[0]);
        KJL2EchoCmdQueue.removeFirst();
    }
    KJL2WriteQueue.append(command);

    //qDebug() << "KJL2 write queue:" << KJL2WriteQueue;
    if (KJL2WriteQueue.length() > 0)
    {
        m_KJL2Serial->write(KJL2WriteQueue[0], KJL2WriteQueue[0].length());
        m_KJL2Serial->waitForBytesWritten(-1);
        emit sendToLog("KJL2 sent data: " + KJL2WriteQueue[0]);

        KJL2WriteQueue.removeFirst();
    }
}

void SerialConfig::writeMFCPort(QByteArray command)
{
    command.append('\r');
    MFCWriteQueue.append(command);

    //qDebug() << "MFC write queue:" << MFCWriteQueue;
    if (MFCWriteQueue.length() > 0)
    {
        m_MFCSerial->write(MFCWriteQueue[0], MFCWriteQueue[0].length());
        m_MFCSerial->waitForBytesWritten(-1);
        emit sendToLog("MFC sent data: " + MFCWriteQueue[0]);
    }
}

void SerialConfig::writeHofiPort(QByteArray command)
{
    hofiWriteQueue.append(command);

    //qDebug() << "Hofi write queue:" << hofiWriteQueue;
    if (hofiWriteQueue.length() > 0)
    {
        m_hofiSerial->write(hofiWriteQueue[0], hofiWriteQueue[0].length());
        m_hofiSerial->waitForBytesWritten(-1);
        emit sendToLog("Hofi sent data: " + hofiWriteQueue[0]);
        hofiWriteQueue.removeFirst();
    }
}

void SerialConfig::readCesarData()  //TODO: Write in thesis about the development of this function, and how it detects and corrects inconsistent data.
{
    cesarReceiveBuffer.append(m_cesarSerial->readAll());

    if (cesarReceiveBuffer.length() == 1)   //Received a single byte.
    {
        if (!cesarACKTimer_flag)
        {
            if (m_ACKTimer->isActive())    //Check if timer is running. (Time elapsed < 100ms)
            {
                m_ACKTimer->stop();
            }

            if (cesarReceiveBuffer[0] == char(0x06)) //ACK
            {
                emit sendToLog("Cesar ACK received.");
                if (!cesarWriteQueue.isEmpty())
                {
                    cesarWriteQueue.removeFirst();
                    //qDebug() << "Cesar write queue:" << cesarWriteQueue;
                }
                cesarReceiveBuffer.clear(); //Clear buffer.
            }
            else if (cesarReceiveBuffer[0] == char(0x15))    //NACK
            {
                emit sendToLog("Cesar NACK received.");
                //Resend message.
                writeCesarPort(cesarWriteQueue[0]);
                cesarReceiveBuffer.clear(); //Clear buffer.
            }
        }
    }
    else if (cesarReceiveBuffer.length() >= 3)    //Received more than 1 byte.
    {
        if (cesarReceiveBuffer[0] == char(0x06) || cesarReceiveBuffer[0] == char(0x15))    //First byte is ACK or NACK by accident?
        {
            if (cesarReceiveBuffer[0] == char(0x06))
            {
                emit sendToLog("Cesar ACK received.");
                if (!cesarWriteQueue.isEmpty())
                {
                    cesarWriteQueue.removeFirst();
                    //qDebug() << "Cesar write queue:" << cesarWriteQueue;
                }
                cesarReceiveBuffer.remove(0, 1);    //Remove 1st element in cesarReceiveBuffer.
                readValidPacket(cesarReceiveBuffer);
            }
            else if (cesarReceiveBuffer[0] == char(0x15))
            {
                emit sendToLog("Cesar NACK received.");
                //Resend message.
                m_cesarSerial->write(cesarWriteQueue[0], cesarWriteQueue[0].length());
                m_cesarSerial->waitForBytesWritten(-1);
                cesarReceiveBuffer.remove(0, 1);    //Remove 1st element in cesarReceiveBuffer.
            }

            if ((cesarReceiveBuffer[0] >> 3) == 0x01)
            {
                readValidPacket(cesarReceiveBuffer);
            }

        }
        else if ((cesarReceiveBuffer[0] >> 3) == 0x01)  //First byte is a valid header byte.
        {
            readValidPacket(cesarReceiveBuffer);
        }
        else
        {
            cesarReceiveBuffer.clear();
        }
    }
}

void SerialConfig::readKJLData()    //TODO: Write in thesis about how this function manages inconsistent data.
{
    char cr = '\r';  //(0x0D) carriage return symbol.
    QByteArray organisedBuffer;

    KJLReceiveBuffer.append(m_KJLSerial->readAll());

    if (KJLReceiveBuffer.length() > 1)
    {
        if (KJLReceiveBuffer.contains(cr))
        {
            for (int i = 0; i <= KJLReceiveBuffer.indexOf(cr); i++)
            {
                organisedBuffer.append(KJLReceiveBuffer[i]);
            }
            KJLReceiveBuffer.remove(0, KJLReceiveBuffer.indexOf(cr) + 1);

            if (KJLReceiveBuffer.isEmpty()) //To ensure that the buffer isn't holding more data.
            {
                emit sendToLog("KJL received data: " + organisedBuffer);
                if (KJLEchoMode)
                {
                    KJLEchoCmdQueue.append(organisedBuffer);
                }
                organisedBuffer.remove(organisedBuffer.indexOf(cr), 1); //Remove '\r' character.
                emit KJL_readyToProcess(organisedBuffer);
            }
            else
            {
                readKJLData();
            }
        }
    }
}

void SerialConfig::readKJL2Data()    //TODO: Write in thesis about how this function manages inconsistent data.
{
    char cr = '\r';  //(0x0D) carriage return symbol.
    QByteArray organisedBuffer;

    KJL2ReceiveBuffer.append(m_KJL2Serial->readAll());

    if (KJL2ReceiveBuffer.length() > 1)
    {
        if (KJL2ReceiveBuffer.contains(cr))
        {
            for (int i = 0; i <= KJL2ReceiveBuffer.indexOf(cr); i++)
            {
                organisedBuffer.append(KJL2ReceiveBuffer[i]);
            }
            KJL2ReceiveBuffer.remove(0, KJL2ReceiveBuffer.indexOf(cr) + 1);

            if (KJL2ReceiveBuffer.isEmpty()) //To ensure that the buffer isn't holding more data.
            {
                emit sendToLog("KJL2 received data: " + organisedBuffer);
                if (KJL2EchoMode)
                {
                    KJL2EchoCmdQueue.append(organisedBuffer);
                }
                organisedBuffer.remove(organisedBuffer.indexOf(cr), 1); //Remove '\r' character.
                emit KJL2_readyToProcess(organisedBuffer);
            }
            else
            {
                readKJL2Data();
            }
        }
    }
}

void SerialConfig::readMFCData()
{
    char cr = '\r';  //(0x0D) carriage return symbol.

    MFCReceiveBuffer.append(m_MFCSerial->readAll());

    if (MFCReceiveBuffer.length() > 1)
    {
        if (MFCReceiveBuffer.contains(cr) && (MFCReceiveBuffer.back() == '\r'))
        {
            emit sendToLog("MFC received data: " + MFCReceiveBuffer);
            MFCReceiveBuffer.remove((MFCReceiveBuffer.indexOf('\r')), 1);
            emit MFC_readyToProcess(MFCReceiveBuffer);

            if (!MFCWriteQueue.isEmpty())
            {
                MFCWriteQueue.removeFirst();
            }
            MFCReceiveBuffer.clear();
        }
    }
}

void SerialConfig::readHofiData()
{
    QByteArrayList  hofiCmdPrefixes = {"HOFIMAIN", "HOFIPORT", "HOFISTATU"};
    QByteArray organisedBuffer;

    if (m_hofiSerial->bytesAvailable())
    {
        hofiReceiveBuffer.append(m_hofiSerial->readAll());
        //qDebug() << "Hofi receive buffer:" << hofiReceiveBuffer;
    }

    if (hofiReceiveBuffer.length() >= 9)    //All Hofi commands have a length of 9.
    {
        if (hofiReceiveBuffer.contains(hofiCmdPrefixes[0])) //Checking for "HOFIMAINE" or "HOFIMAINA".
        {
            if ((hofiReceiveBuffer[8] == 'E') || (hofiReceiveBuffer[8] == 'A'))
            {
                organisedBuffer = hofiReceiveBuffer.mid(0, 9);
                emit sendToLog("Hofi received data: " + organisedBuffer);
                emit hofi_readyToProcess(organisedBuffer);
                hofiReceiveBuffer.remove(0, 9);
            }
        }
        else if (hofiReceiveBuffer.contains(hofiCmdPrefixes[1]))
        {
            if ((hofiReceiveBuffer[8] >= '1') && (hofiReceiveBuffer[8] <= '5'))
            {
                organisedBuffer = hofiReceiveBuffer.mid(0, 9);
                emit sendToLog("Hofi received data: " + organisedBuffer);
                emit hofi_readyToProcess(organisedBuffer);
                hofiReceiveBuffer.remove(0, 9);
            }
        }
        else if (hofiReceiveBuffer.contains(hofiCmdPrefixes[2]))
        {
            organisedBuffer = hofiReceiveBuffer.mid(0, 9);
            emit sendToLog("Hofi received data: " + organisedBuffer);
            emit hofi_readyToProcess(organisedBuffer);
            hofiReceiveBuffer.remove(0, 9);
        }
        if (hofiReceiveBuffer.length() >= 9)    //Check again if another command still remains.
        {
            readHofiData();
        }
    }
}
