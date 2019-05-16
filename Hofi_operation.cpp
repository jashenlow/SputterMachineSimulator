#include "Hofi_operation.h"

HofiOperation::HofiOperation(QObject *parent) :
    QObject (parent)
{
    m_ui = nullptr;
}

HofiOperation::~HofiOperation()
{
    delete m_ui;
}

void HofiOperation::setUiPointers(Ui::MainWindow *ptr)
{
    if (ptr != nullptr)
    {
        m_ui = ptr;
    }
}

void HofiOperation::hofiLoadDefaults()
{
    onOffState = false;
    currentPort = '1';
}

void HofiOperation::setOnOff(bool state)
{
    onOffState = state;

    if (onOffState)
    {
        m_ui->lblHofi_OnOffLED->setStyleSheet("background-color: red;");
        setPort(currentPort);
    }
    else
    {
        m_ui->lblHofi_OnOffLED->setStyleSheet("background-color: gray;");
        m_ui->lblHofi_Port1LED->setStyleSheet("background-color: gray;");
        m_ui->lblHofi_Port2LED->setStyleSheet("background-color: gray;");
        m_ui->lblHofi_Port3LED->setStyleSheet("background-color: gray;");
        m_ui->lblHofi_Port4LED->setStyleSheet("background-color: gray;");
        m_ui->lblHofi_Port5LED->setStyleSheet("background-color: gray;");
    }
}

void HofiOperation::setPort(char port)
{
    if (onOffState)
    {
        switch (port)
        {
            case '1':
                currentPort = Ports::Port1;
                m_ui->lblHofi_Port1LED->setStyleSheet("background-color: lime;");
                m_ui->lblHofi_Port2LED->setStyleSheet("background-color: gray;");
                m_ui->lblHofi_Port3LED->setStyleSheet("background-color: gray;");
                m_ui->lblHofi_Port4LED->setStyleSheet("background-color: gray;");
                m_ui->lblHofi_Port5LED->setStyleSheet("background-color: gray;");
                break;
            case '2':
                currentPort = Ports::Port2;
                m_ui->lblHofi_Port1LED->setStyleSheet("background-color: gray;");
                m_ui->lblHofi_Port2LED->setStyleSheet("background-color: lime;");
                m_ui->lblHofi_Port3LED->setStyleSheet("background-color: gray;");
                m_ui->lblHofi_Port4LED->setStyleSheet("background-color: gray;");
                m_ui->lblHofi_Port5LED->setStyleSheet("background-color: gray;");
                break;
            case '3':
                currentPort = Ports::Port3;
                m_ui->lblHofi_Port1LED->setStyleSheet("background-color: gray;");
                m_ui->lblHofi_Port2LED->setStyleSheet("background-color: gray;");
                m_ui->lblHofi_Port3LED->setStyleSheet("background-color: lime;");
                m_ui->lblHofi_Port4LED->setStyleSheet("background-color: gray;");
                m_ui->lblHofi_Port5LED->setStyleSheet("background-color: gray;");
                break;
            case '4':
                currentPort = Ports::Port4;
                m_ui->lblHofi_Port1LED->setStyleSheet("background-color: gray;");
                m_ui->lblHofi_Port2LED->setStyleSheet("background-color: gray;");
                m_ui->lblHofi_Port3LED->setStyleSheet("background-color: gray;");
                m_ui->lblHofi_Port4LED->setStyleSheet("background-color: lime;");
                m_ui->lblHofi_Port5LED->setStyleSheet("background-color: gray;");
                break;
            case '5':
                currentPort = Ports::Port5;
                m_ui->lblHofi_Port1LED->setStyleSheet("background-color: gray;");
                m_ui->lblHofi_Port2LED->setStyleSheet("background-color: gray;");
                m_ui->lblHofi_Port3LED->setStyleSheet("background-color: gray;");
                m_ui->lblHofi_Port4LED->setStyleSheet("background-color: gray;");
                m_ui->lblHofi_Port5LED->setStyleSheet("background-color: lime;");
                break;
        }
    }
}

void HofiOperation::queryCurrentState()
{
    QByteArray dataToSend;

    onOffState ? dataToSend.append(0x06) : dataToSend.append(0x0A); //On = 0x06, Off = 0x0A.
    dataToSend.append(' ');
    dataToSend.append("LED");
    dataToSend.append(currentPort);

    emit writetoHofiPort(dataToSend);
}
