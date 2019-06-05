#include "cesar_operation.h"

CesarOperation::CesarOperation(QObject *parent) :
    QObject (parent)
{
    m_ui = nullptr;
}

CesarOperation::~CesarOperation()
{
    delete m_ui;
}

void CesarOperation::setUiPointers(Ui::MainWindow *ptr)
{
    if (ptr != nullptr)
    {
        m_ui = ptr;
    }
}

void CesarOperation::cesarLoadDefaults()
{
    outputState = false;

    setPoint       = 0;
    forwardPower   = 0;
    reflectedPower = 0;
    DCBias         = 0;
    loadCapPos     = 0;
    tuneCapPos     = 0;
    regulationMode = RegulationMode::ForwardPower;

    m_ui->lblCesarRefPwr->setText("RF OFF");
    m_ui->lblCesaRefIndicator->setText("");
}

QByteArray CesarOperation::assemblePacket(QByteArray packet)
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

    return packet;
}

void CesarOperation::sendReply(QByteArray data)
{
    QByteArray replyPacket;

    replyPacket.append(data);
    replyPacket = assemblePacket(replyPacket);
    emit writeToCesarPort(replyPacket);
}

void CesarOperation::setOutputState(bool state)
{
    QByteArray replyData;

    outputState = state;

    if (state)
    {
        m_ui->lblCesarRefPwr->setText(QString::number(reflectedPower) + "W");
        m_ui->lblCesaRefIndicator->setText("r");
    }
    else
    {
        m_ui->lblCesarRefPwr->setText("RF OFF");
        m_ui->lblCesaRefIndicator->setText("");
    }

    replyData.append(char(CSRcode::command_accepted));
    sendReply(replyData);
}

void CesarOperation::setPowerSetPoint(int value)
{
    QByteArray replyData;

    setPoint = value;
    qDebug() << "Cesar setpoint: " << setPoint;

    replyData.append(char(CSRcode::command_accepted));
    sendReply(replyData);
}

void CesarOperation::setRegulationMode(char mode)
{
    QByteArray replyData;

    switch (mode)
    {
        case 0x06:
            m_ui->lblCesarRegMode->setText("f");
            break;
        case 0x07:
            m_ui->lblCesarRegMode->setText("real");
            break;
        case 0x08:
            m_ui->lblCesarRegMode->setText("bias");
            break;
    }

    replyData.append(char(CSRcode::command_accepted));
    sendReply(replyData);
}

void CesarOperation::reportExternalFeedback(uint8_t command)
{
    QByteArray replyData;

    replyData.append(char(command));
    replyData.append(char(DCBias & 0xFF));
    replyData.append(char((DCBias >> 8) & 0xFF));

    emit sendReply(replyData);
}

void CesarOperation::reportForwardPower(uint8_t command)
{
    QByteArray replyData;

    replyData.append(char(command));
    replyData.append(char(forwardPower & 0xFF));
    replyData.append(char((forwardPower >> 8) & 0xFF));

    emit sendReply(replyData);
}

void CesarOperation::reportReflectedPower(uint8_t command)
{
    QByteArray replyData;

    replyData.append(char(command));
    replyData.append(char(reflectedPower & 0xFF));
    replyData.append(char((reflectedPower >> 8) & 0xFF));

    emit sendReply(replyData);
}

void CesarOperation::reportCapPositions(uint8_t command)
{
    QByteArray replyData;

    replyData.append(char(command));
    replyData.append(char(loadCapPos & 0xFF));    //Load capacitor 1st byte.
    replyData.append(char((loadCapPos >> 8) & 0xFF)); //Load capacitor 2nd byte.
    replyData.append(char(tuneCapPos & 0xFF));    //Tune capacitor 1st byte.
    replyData.append(char((tuneCapPos >> 8) & 0xFF)); //Tune capacitor 2nd byte.

    emit sendReply(replyData);
}

void CesarOperation::reportSetPointandRegMode(uint8_t command)  //6 = Forward Power, 7 = Load Power, 8 = External Power
{
    QByteArray replyData;

    replyData.append(char(command));
    replyData.append(char(setPoint & 0xFF));  //Byte 0.
    replyData.append(char((setPoint >> 8) & 0xFF));   //Byte 1.
    replyData.append(char(regulationMode));   //Byte 2.

    emit sendReply(replyData);
}
