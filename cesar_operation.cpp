#include "cesar_operation.h"

CesarOperation::CesarOperation(QObject *parent) :
    QObject (parent)
{
    m_rampTimer = new QTimer(this);
    m_rampTimer->setTimerType(Qt::PreciseTimer);
    m_rampTimer->setInterval(100);  //Triggers a timeout every 100ms.

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
    connect(m_rampTimer, &QTimer::timeout, this, &CesarOperation::rampTimeout, Qt::QueuedConnection);
}

void CesarOperation::cesarLoadDefaults()
{
    outputState             = false;
    ramping_EN              = true;
    rampUpTime              = 1;
    rampDownTime            = 1;
    activeControlMode       = 2;
    reflectedPowerLimitTime = 0;
    reflectedPowerLimit     = 0;
    forwardPowerLimit       = 300;
    setPoint                = 0;
    forwardPower            = 0;
    dblForwardPower         = 0;
    reflectedPower          = 0;
    DCBias                  = 0;
    loadCapPos              = 0;
    tuneCapPos              = 0;
    regulationMode          = RegulationMode::ForwardPower;

    updateDisplay();
}

void CesarOperation::rampTimeout()
{
    if (qRound(dblForwardPower) != rampFinalPower)
    {
        if (dblForwardPower < rampFinalPower)
        {
            dblForwardPower += rampStepSize;

            if ((dblForwardPower + rampStepSize) > rampFinalPower)  //To avoid infinite back and forth due to double precision numbers.
            {
                dblForwardPower = rampFinalPower;
            }
        }
        else if (dblForwardPower > rampFinalPower)
        {
            dblForwardPower -= rampStepSize;

            if ((dblForwardPower - rampStepSize) < rampFinalPower)  //To avoid infinite back and forth due to double precision numbers.
            {
                dblForwardPower = rampFinalPower;
            }
        }
        forwardPower = qRound(dblForwardPower);
        updateDisplay();
    }

    if (forwardPower == rampFinalPower)
    {
        m_rampTimer->stop();
        dblForwardPower = 0;
        if (forwardPower == 0)
        {
            outputState = false;
            updateDisplay();
        }
    }
}

void CesarOperation::startRamping(bool mode)
{
    if (mode)
    {
        if (setPoint > forwardPower)
        {
            rampSteps = int(rampUpTime * 1000) / 100;
            rampStepSize = double(setPoint - forwardPower) / rampSteps;
        }
        else if (setPoint < forwardPower)
        {
            rampSteps = int(rampDownTime * 1000) / 100;
            rampStepSize = double(forwardPower - setPoint) / rampSteps;
        }
        rampFinalPower = setPoint;
        dblForwardPower = forwardPower;
        outputState = true;
        m_rampTimer->start();
    }
    else
    {
        rampSteps = int(rampDownTime * 1000) / 100;
        rampFinalPower = 0;
        rampStepSize = double(forwardPower - rampFinalPower) / rampSteps;
        dblForwardPower = forwardPower;
        m_rampTimer->start();
    }
    //qDebug() << "setpoint:" << setPoint;
    //qDebug() << "forward power:" << forwardPower;
    //qDebug() << "Ramp steps:" << rampSteps;
    //qDebug() << "Ramp step size:" << rampStepSize;
}

void CesarOperation::updateDisplay()
{
    m_ui->lblCesarLoadCap->setText(QString::number(loadCapPos, 'f', 1) + "%");
    m_ui->lblCesarTuneCap->setText(QString::number(tuneCapPos, 'f', 1) + "%");
    m_ui->lblCesarFwdPwr->setText(QString::number(forwardPower) + "W");
    switch (regulationMode)
    {
        case 6:
            m_ui->lblCesarRegMode->setText("f");
            break;
        case 7:
            m_ui->lblCesarRegMode->setText("real");
            break;
        case 8:
            m_ui->lblCesarRegMode->setText("bias");
            break;
    }

    if (outputState)
    {
        m_ui->lblCesarRefPwr->setText(QString::number(reflectedPower) + "W");
        m_ui->lblCesaRefIndicator->setText("r");
    }
    else
    {
        m_ui->lblCesarRefPwr->setText("RF OFF");
        m_ui->lblCesaRefIndicator->setText("");
    }
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

void CesarOperation::setOutputState(uint8_t command, bool state)
{
    QByteArray replyData;

    if (outputState != state)
    {
        if (ramping_EN)
        {
            startRamping(state);
        }
        else
        {
            forwardPower = setPoint;
            outputState = state;
            updateDisplay();
        }
    }

    replyData.append(char(command));
    replyData.append(char(CSRcode::command_accepted));
    sendReply(replyData);
}

void CesarOperation::setPowerSetPoint(uint8_t command, int value)
{
    QByteArray replyData;

    setPoint = value;
    if (ramping_EN)
    {
        startRamping(true);
    }
    else
    {
        if (outputState)
        {
            forwardPower = setPoint;
            updateDisplay();
        }
    }

    replyData.append(char(command));
    replyData.append(char(CSRcode::command_accepted));
    sendReply(replyData);
}

void CesarOperation::setRegulationMode(uint8_t command, char mode)
{
    QByteArray replyData;

    regulationMode = mode;
    updateDisplay();

    replyData.append(char(command));
    replyData.append(char(CSRcode::command_accepted));
    sendReply(replyData);
}

void CesarOperation::setActiveControlMode(uint8_t command, int mode)
{
    QByteArray replyData;

    if (activeControlMode != mode)
    {
        if (outputState)
        {
            outputState = false;
            updateDisplay();
        }
        activeControlMode = mode;
    }

    replyData.append(char(command));
    replyData.append(CSRcode::command_accepted);
    sendReply(replyData);
}

void CesarOperation::setMatchNetworkControl(uint8_t command, int mode)
{
    QByteArray replyData;

    replyData.append(char(command));
    if (activeControlMode == ActiveControlMode::Host)
    {
        matchNetworkControl = mode;

        replyData.append(CSRcode::command_accepted);
    }
    else
    {
        replyData.append(CSRcode::command_not_accepted);
    }
    sendReply(replyData);
}

void CesarOperation::setReflectedPowerLimit(uint8_t command, int value)
{
    QByteArray replyData;

    replyData.append(char(command));
    if (activeControlMode == ActiveControlMode::Host)
    {
        reflectedPowerLimit = value;
        replyData.append(CSRcode::command_accepted);
    }
    else
    {
        replyData.append(CSRcode::command_not_accepted);
    }
    sendReply(replyData);
}

void CesarOperation::setReflectedPowerParameters(uint8_t command, int time, int value)
{
    QByteArray replyData;

    replyData.append(char(command));
    if (activeControlMode == ActiveControlMode::Host)
    {
        reflectedPowerLimitTime = time;
        reflectedPowerLimit = value;
        replyData.append(CSRcode::command_accepted);
    }
    else
    {
        replyData.append(CSRcode::command_not_accepted);
    }
    sendReply(replyData);
}

void CesarOperation::setCapPositions(uint8_t command, bool cap, int value)
{
    QByteArray replyData;

    replyData.append(char(command));
    if (activeControlMode == ActiveControlMode::Host)
    {
        cap ? loadCapPos = double(value) / 10 : tuneCapPos = double(value) / 10;
        updateDisplay();
        replyData.append(CSRcode::command_accepted);
    }
    else
    {
        replyData.append(CSRcode::command_not_accepted);
    }
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
    int loadCapInt = int(loadCapPos * 10);
    int tuneCapInt = int(tuneCapPos * 10);

    replyData.append(char(command));
    replyData.append(char(loadCapInt & 0xFF));    //Load capacitor 1st byte.
    replyData.append(char((loadCapInt >> 8) & 0xFF)); //Load capacitor 2nd byte.
    replyData.append(char(tuneCapInt & 0xFF));    //Tune capacitor 1st byte.
    replyData.append(char((tuneCapInt >> 8) & 0xFF)); //Tune capacitor 2nd byte.

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
