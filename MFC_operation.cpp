#include "MFC_operation.h"

MFCOperation::MFCOperation(QObject *parent) :
    QObject (parent)
{
    m_dispTimer = new QTimer(this);
    m_dispTimer->setSingleShot(true);

    unitString << "ubar" << "mbar" << "bar" << "mTor" << "Torr" << "KTor" << "Pa" << "kPa" << "mH2O" << "cH2O" << "PSI"
               << "N/qm" << "SCCM" << "SLM" << "SCM" << "SCFH" << "SCFM" << "mA" << "V" << "%" << "C";

    powerOnDisplay << "PR4000 2CHANNELS"
                   << "V1.00   May05,19";

    m_ui = nullptr;
}

MFCOperation::~MFCOperation()
{
    delete m_ui;
}

void MFCOperation::setUiPointers(Ui::MainWindow *ptr)
{
    if (ptr != nullptr)
    {
        m_ui = ptr;
    }

    connect(m_dispTimer, &QTimer::timeout, this, &MFCOperation::updateDisplay);
}

void MFCOperation::MFCloadDefaults()
{
    CH1_valveState = false;
    CH1_unit = Unit::SCCM;
    RNG1 = 100.0;
    SP1 = 1 * RNG1;
    FL1 = 100.0;
    GAIN1 = 1.0;
    OFFS1 = 0.0;

    CH2_valveState = false;
    CH2_unit = Unit::SCCM;
    RNG2 = 100.0;
    SP2 = 1 * RNG2;
    FL2 = 100.0;
    GAIN2 = 1.0;
    OFFS2 = 0.0;

    remoteMode = false;
    setPoint_EN = false;
    displayNumber = 1;

    display1 << "FL1  " + QString::number(FL1) + ' ' + unitString[Unit::SCCM].rightJustified(4, ' ');
    display1 << "SP1  " + QString::number(SP1) + ' ' + unitString[Unit::SCCM].rightJustified(4, ' ');

    display2 << "FL2  " + QString::number(FL2) + ' ' + unitString[Unit::SCCM].rightJustified(4, ' ');
    display2 << "SP2  " + QString::number(SP2) + ' ' + unitString[Unit::SCCM].rightJustified(4, ' ');

    display3 << "";
    display3 << "";

    display4 << "";
    display4 << "";

    setDisplay(powerOnDisplay);
    m_dispTimer->setInterval(2500);
    m_dispTimer->start();
}

void MFCOperation::updateDisplay()
{    
    switch (displayNumber)
    {
        case 1:
            display1[0] = "FL1  " + QString::number(FL1) + ' ' + unitString[CH1_unit].rightJustified(4, ' ');
            display1[1] = "SP1  " + QString::number(SP1) + ' ' + unitString[CH1_unit].rightJustified(4, ' ');
            setDisplay(display1);
            break;
        case 2:
            display2[0] = "FL2  " + QString::number(FL2) + ' ' + unitString[CH2_unit].rightJustified(4, ' ');
            display2[1] = "SP2  " + QString::number(SP2) + ' ' + unitString[CH2_unit].rightJustified(4, ' ');
            setDisplay(display2);
            break;
        case 3:
            setDisplay(display3);
            break;
        case 4:
            setDisplay(display4);
            break;
    }

    remoteMode  ? m_ui->lblMFCRemoteLED->setStyleSheet("background-color: red;") : m_ui->lblMFCRemoteLED->setStyleSheet("background-color: gray;");
    setPoint_EN ? m_ui->lblMFCSetpointLED->setStyleSheet("background-color: red;") : m_ui->lblMFCSetpointLED->setStyleSheet("background-color: gray;");
}

void MFCOperation::setDisplay(QStringList display)
{
    m_ui->lblMFCLine1->setText(display[0]);
    m_ui->lblMFCLine2->setText(display[1]);
}

void MFCOperation::errorHandler(QByteArray error)
{
    //See MFC_operation.h for info.
    emit writetoMFCPort(error);
}

void MFCOperation::queryRange(int channel)
{
    QByteArray dataToSend;
    if (channel == 1)
    {
        dataToSend.append(QString::number(RNG1, 'f', 2));
        dataToSend.append(',');
        dataToSend.append(QString::number(CH1_unit).rightJustified(5, '0'));
    }
    else if (channel == 2)
    {
        dataToSend.append(QString::number(RNG2, 'f', 2));
        dataToSend.append(',');
        dataToSend.append(QString::number(CH2_unit).rightJustified(5, '0'));
    }
    emit writetoMFCPort(dataToSend);
}

void MFCOperation::queryRemoteMode()
{
    QByteArray dataToSend;

    remoteMode == true ? dataToSend = "ON" : dataToSend = "OFF";
    emit writetoMFCPort(dataToSend);
}

void MFCOperation::setRemoteMode(QByteArray mode)
{
    mode == "ON" ? remoteMode = true : remoteMode = false;
    updateDisplay();
    emit writetoMFCPort("");
}

void MFCOperation::setRange(char channel, QString data)
{
    QStringList dataList = data.split(',');

    switch (channel)
    {
        case '1':
            RNG1 = dataList[0].toDouble();
            CH1_unit = dataList[1].toInt();
            break;

        case '2':
            RNG2 = dataList[0].toDouble();
            CH2_unit = dataList[1].toInt();
            break;
    }
    updateDisplay();
    emit writetoMFCPort("");
}

void MFCOperation::setValveState(char channel, QByteArray state)
{
    switch (channel)
    {
        case '0':
            if (state == "ON")
            {
                CH1_valveState = true;
                CH2_valveState  = true;
            }
            else
            {
                CH1_valveState = false;
                CH2_valveState  = false;
            }
            break;

        case '1':
            state == "ON" ? CH1_valveState = true : CH1_valveState = false;
            break;

        case '2':
            state == "ON" ? CH2_valveState = true : CH2_valveState = false;
            break;
    }
    (CH1_valveState || CH2_valveState) ? setPoint_EN = true : setPoint_EN = false;
    updateDisplay();
    emit writetoMFCPort("");
}

void MFCOperation::queryValveState(char channel)
{
    QByteArray dataToSend;

    switch (channel)
    {
        case '1':
            CH1_valveState == true ? dataToSend = "ON" : dataToSend = "OFF";
            break;

        case '2':
            CH2_valveState == true ? dataToSend = "ON" : dataToSend = "OFF";
            break;
    }
    emit writetoMFCPort(dataToSend);
}

void MFCOperation::queryActualValue(char channel)
{
    QByteArray dataToSend;

    switch (channel)
    {
        case '1':
            dataToSend.append(QString::number(FL1));
            emit writetoMFCPort(dataToSend);
            break;
        case '2':
            dataToSend.append(QString::number(FL2));
            emit writetoMFCPort(dataToSend);
            break;
    }
}

void MFCOperation::queryActualValueSetSetpoint(char channel, double setPoint)
{
    queryActualValue(channel);

    switch (channel)
    {
        case '1':
            SP1 = setPoint;
            FL1 = SP1;
            break;
        case '2':
            SP2 = setPoint;
            FL2 = SP2;
            break;
    }
    updateDisplay();
}
