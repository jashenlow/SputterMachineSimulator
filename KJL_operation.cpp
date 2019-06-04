#include "KJL_operation.h"

KJLOperation::KJLOperation(QObject *parent) :
    QObject (parent)
{
    m_rampTimer = new QTimer(this);
    m_rampTimer->setTimerType(Qt::PreciseTimer);
    m_rampTimer->setInterval(100);  //Triggers a timeout every 100ms.

    m_ui = nullptr;
}

KJLOperation::~KJLOperation()
{
    delete m_ui;
}

void KJLOperation::setUiPointers(Ui::MainWindow *ptr)
{
    if (ptr != nullptr)
    {
        m_ui = ptr;
    }
    connect(m_rampTimer, &QTimer::timeout, this, &KJLOperation::rampTimer_timeout, Qt::QueuedConnection);
}

void KJLOperation::KJLloadDefaults()
{
    //The following values were referenced from the KJL manual.

    limit_maxPower = 300;
    limit_impedance = 1000;
    limit_DCVoltage = 9999;
    limit_rampTime = 9999000;
    limit_dissipation = true;

    alarm_EXT = false;
    alarm_MAX = false;
    alarm_PAC = false;
    alarm_REF = false;
    alarm_TMP = false;
    alarm_XIMP = false;
    alarm_REF_EN = true;

    rampUpTime     = 1000;  //1 second
    rampDownTime   = 1000;
    rampFinalPower = 0;
    reflectedPower = 0;
    powerSetPoint  = 0;
    forwardPower   = 0;
    loadCapPos     = 50;
    tuneCapPos     = 50;
    DCBias         = 0;
    alarmStatus    = m_alarm.OFF;
    outputRegFeedback = OutputRegFeedback::InternalSensor;
    controlSource = ControlandSetpointSource::Serial;   //For the sake of this simulator.

    exciterMode = false; //MASTER mode.
    pulseMode = false;  //DISABLED
    interlockStatus = false; //Assumes interface connector is connected, specifically for this simulator.
    commsLinkStatus = true;

    setEchoMode(true);
    setAlarmStatus(alarmStatus);
    setControlSource("SERIAL");
    setOutput(false);   //Set output to Off.
    setCapPosition(true, loadCapPos);
    setCapPosition(false, tuneCapPos);
    setPowerSetPoint(powerSetPoint);
    setOutputRamping(true);
}

void KJLOperation::rampTimer_timeout()
{
    double dblForwardPower = double(forwardPower);

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
        m_ui->lblKJL1_L1P2->setText(QString::number(forwardPower) + "W");
    }

    if (forwardPower == rampFinalPower)
    {
        m_rampTimer->stop();
        if (!outputStatus)
        {
            setOutput(outputStatus);    //For updating the display on the UI.
        }
    }
}

void KJLOperation::outputRamping(bool mode)     //true = Turning output ON or changing setpoint. false = Turning output OFF.
{
    if (mode)
    {
        if (powerSetPoint > forwardPower)
        {
            rampSteps = rampUpTime / 100;
            rampStepSize = (powerSetPoint - forwardPower) / rampSteps;
        }
        else if (powerSetPoint < forwardPower)
        {
            rampSteps = rampDownTime / 100;
            rampStepSize = (forwardPower - powerSetPoint) / rampSteps;
        }
        rampFinalPower = powerSetPoint;
        m_rampTimer->start();
    }
    else
    {
        rampSteps = rampDownTime / 100;
        rampFinalPower = 0;
        rampStepSize = (forwardPower - rampFinalPower) / rampSteps;
        m_rampTimer->start();
    }
}

void KJLOperation::setEchoMode(bool mode)
{
    echoStatus = mode;
}

void KJLOperation::setCommsLinkStatus(bool status)
{
    commsLinkStatus = status;
}

void KJLOperation::setAlarmStatus(QString status)
{
    status == m_alarm.EXT   ? alarm_EXT = true  : alarm_EXT = false;
    status == m_alarm.MAX   ? alarm_MAX = true  : alarm_MAX = false;
    status == m_alarm.PAC   ? alarm_PAC = true  : alarm_PAC = false;
    status == m_alarm.REF   ? alarm_REF = true  : alarm_REF = false;
    status == m_alarm.TMP   ? alarm_REF = true  : alarm_REF = false;
    status == m_alarm.XIMP  ? alarm_XIMP = true : alarm_XIMP = false;

    m_ui->lblKJL1_L2P4->setText(status);
}

void KJLOperation::setOutput(bool status)
{
    if (status)
    {
        outputStatus = true;

        if (outputRamping_EN)
        {
            if (!m_rampTimer->isActive())
            {
                outputRamping(true);
            }
        }
        else
        {
            forwardPower = powerSetPoint;
        }
        m_ui->lblKJL1_L1P1->setText("FWD:");
        m_ui->lblKJL1_L1P2->setText(QString::number(forwardPower) + "W");

        m_ui->lblKJL1_OutputOn->setStyleSheet("background-color: red;");
        m_ui->lblKJL1_OutputOff->setStyleSheet("background-color: gray;");
    }
    else
    {
        outputStatus = false;

        if (outputRamping_EN)
        {
            if (forwardPower > 0)
            {
                outputRamping(false);
            }
        }
        else
        {
            forwardPower = 0;
        }

        if (forwardPower == 0)
        {
            m_ui->lblKJL1_L1P1->setText("SET:");
            m_ui->lblKJL1_L1P2->setText(QString::number(powerSetPoint) + "W");

            m_ui->lblKJL1_OutputOn->setStyleSheet("background-color: gray;");
            m_ui->lblKJL1_OutputOff->setStyleSheet("background-color: rgb(0, 170, 255);");  //Slightly more blue than cyan.
        }
    }
}

void KJLOperation::setPowerSetPoint(int value)
{
    powerSetPoint = value;

    if (!outputStatus)
    {
        m_ui->lblKJL1_L1P2->setText(QString::number(powerSetPoint) + "W");
    }
    else
    {
        if (outputRamping_EN)
        {
            if (powerSetPoint != forwardPower)
            {
                outputRamping(true);
            }
        }
        else
        {
            forwardPower = powerSetPoint;
            m_ui->lblKJL1_L1P2->setText(QString::number(forwardPower) + "W");
        }
    }
}

void KJLOperation::setCapPosition(bool cap, int position)
{
    if (cap)    //Set load cap.
    {
        loadCapPos = position;

        m_ui->lblKJL1_2_L1P4->setText(QString::number(loadCapPos) + "%");
    }
    else    //Set tune cap.
    {
        tuneCapPos = position;

        m_ui->lblKJL1_2_L1P2->setText(QString::number(tuneCapPos) + "%");
    }
}

void KJLOperation::setControlSource(QByteArray data)
{
    if (data == "SERIAL" || data == "**")
    {
        controlSource = ControlandSetpointSource::Serial;
        m_ui->lblKJL1_L2P1->setText(QString(data));
    }
    else if (data == "ANALOG")
    {
        controlSource = ControlandSetpointSource::Analog;
        m_ui->lblKJL1_L2P1->setText(QString(data));
    }
    else if (data == "PANEL")
    {
        controlSource = ControlandSetpointSource::FrontPanel;
        m_ui->lblKJL1_L2P1->setText(QString(data));
    }
}

void KJLOperation::setOutputRamping(bool mode)
{
    if (mode)
    {
        outputRamping_EN = true;
    }
    else
    {
        outputRamping_EN = false;
    }
}

void KJLOperation::setRampDownTime(int time)
{
    if (outputRamping_EN)
    {
        rampDownTime = time * 1000;
    }
}

void KJLOperation::setRampUpTime(int time)
{
    if (outputRamping_EN)
    {
        rampUpTime = time * 1000;
    }
}

void KJLOperation::queryForwardPower()
{
    QByteArray dataToSend;

    dataToSend.append(QString::number(forwardPower));

    while (dataToSend.length() < 4) //As stated in the manual, 4 characters are required to be sent to represent forward power.
    {
        dataToSend.prepend(' ');
    }
    dataToSend.append('\r');

    emit writeQueryData(dataToSend);
}

void KJLOperation::queryExternalFeedback()
{
    QByteArray dataToSend;

    dataToSend.append(QString::number(DCBias));
    dataToSend.append('\r');

    emit writeQueryData(dataToSend);
}

void KJLOperation::queryReflectedPower()
{
    QByteArray dataToSend;

    dataToSend.append(QString::number(reflectedPower));

    while (dataToSend.length() < 4) //As stated in the manual, 4 characters are required to be sent to represent forward power.
    {
        dataToSend.prepend(' ');
    }
    dataToSend.append('\r');

    emit writeQueryData(dataToSend);
}

void KJLOperation::queryStatus()    //Response: "XXXXXXX aaaa bbbb ccc dddd<cr>"
{
    /*For some strange reason, bits in characters 4, 5, and 6 are counted from left to right, instead of right to left
     * Example: [bit0, bit1, bit2, bit3, ...], instead of [bit7, bit6, bit5, bit4, ...].
    */

    QByteArray dataToSend;
    QString statusString, str_powerSetPoint, str_forwardPower, str_reflectedPower, str_limit_maxPower;
    int charFour = 0, charFive = 0, charSix = 0x30, charSeven = 0;

    //Assembling character four.
    outputStatus ? charFour |= ((0x01 << 4) & 0xFF) : charFour &= ~(0x01 << 4);
    alarm_REF_EN ? charFour |= ((0x01 << 5) & 0xFF) : charFour &= ~(0x01 << 5);
    alarm_MAX    ? charFour |= ((0x01 << 6) & 0xFF) : charFour &= ~(0x01 << 6);
    alarm_PAC    ? charFour |= ((0x01 << 7) & 0xFF) : charFour &= ~(0x01 << 7);

    //Assembling character five.
    (outputStatus && !alarm_REF) ? charFive |= ((0x01 << 4) & 0xFF) : charFive &= ~(0x01 << 4);
    limit_dissipation            ? charFive |= ((0x01 << 5) & 0xFF) : charFive &= ~(0x01 << 5);
    exciterMode                  ? charFive |= ((0x01 << 6) & 0xFF) : charFive &= ~(0x01 << 6);
    pulseMode                    ? charFive |= ((0x01 << 7) & 0xFF) : charFive &= ~(0x01 << 7);

    //Assembling character six.
    interlockStatus ? charSix |= ((0x01 << 6) & 0xFF) : charSix &= ~(0x01 << 6);
    alarm_TMP       ? charSix |= ((0x01 << 7) & 0xFF) : charSix &= ~(0x01 << 7);

    //Assembling character seven.
    commsLinkStatus ? charSeven = 0 : charSeven = 1;

    //Assembling status string.
    statusString.append(QString::number(controlSource));   //Character one.
    statusString.append(QString::number(outputRegFeedback));   //Character two.
    statusString.append(QString::number(controlSource));   //Character three.
    statusString.append(uchar(charFour));
    statusString.append(uchar(charFive));
    statusString.append(uchar(charSix));
    statusString.append(QString::number(charSeven));

    dataToSend.append(statusString);    //Append the 7 character string.
    dataToSend.append(' ');

    str_powerSetPoint = QString::number(powerSetPoint).rightJustified(4, '0');
    dataToSend.append(str_powerSetPoint);  //Append "aaaa" field.
    dataToSend.append(' ');

    str_forwardPower = QString::number(forwardPower).rightJustified(4, '0');
    dataToSend.append(str_forwardPower);  //Append "bbbb" field.
    dataToSend.append(' ');

    str_reflectedPower = QString::number(reflectedPower).rightJustified(3, '0');
    dataToSend.append(str_reflectedPower);  //Append "ccc" field.
    dataToSend.append(' ');

    str_limit_maxPower = QString::number(limit_maxPower).rightJustified(4, '0');
    dataToSend.append(str_limit_maxPower);  //Append "dddd" field.

    dataToSend.append('\r');    //Append <cr>

    //qDebug() << "dataToSend:" << dataToSend;
    emit writeQueryData(dataToSend);
}
