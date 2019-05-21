#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_SPSConfig = new SPSConfig(this);
    m_processSerialData = new ProcessSerialData(this);
    m_cesarOperation = new CesarOperation(this);
    m_KJLOperation = new KJLOperation(this);
    m_KJL2Operation = new KJL2Operation(this);
    m_MFCOperation = new MFCOperation(this);
    m_hofiOperation = new HofiOperation(this);

    connectSignals();
    initUiPointers();
    m_KJLOperation->KJLloadDefaults();
    m_KJL2Operation->KJL2loadDefaults();
    m_MFCOperation->MFCloadDefaults();
    m_hofiOperation->hofiLoadDefaults();
    m_processSerialData->m_serialConfig->enumeratePorts();
    initSerialComboBoxes();
    m_processSerialData->m_serialConfig->closePorts();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initSerialComboBoxes() //Add more entries here when more comboboxes are added.
{
    ui->cbCesarPort->addItems(m_processSerialData->m_serialConfig->serialNames);
    ui->cbKJLPort->addItems(m_processSerialData->m_serialConfig->serialNames);
    ui->cbKJL2Port->addItems(m_processSerialData->m_serialConfig->serialNames);
    ui->cbMFCPort->addItems(m_processSerialData->m_serialConfig->serialNames);
    ui->cbHofiPort->addItems(m_processSerialData->m_serialConfig->serialNames);
}

bool MainWindow::checkportNames()   //Checks for duplicate COM port selections.
{
    QStringList currentSelections;

    //Add more combobox entries here.
    currentSelections << ui->cbCesarPort->currentText()
                      << ui->cbKJLPort->currentText()
                      << ui->cbKJL2Port->currentText()
                      << ui->cbMFCPort->currentText()
                      << ui->cbHofiPort->currentText();

    for (int i = 0; i < currentSelections.length() - 1; i++)
    {
        for (int j = i + 1; j < currentSelections.length(); j++)
        {
            if (currentSelections[i] == currentSelections[j])
            {
                return true;
            }
        }
    }
    m_processSerialData->m_serialConfig->serialNames = currentSelections;
    return false;
}

void MainWindow::initUiPointers()   //Initialises pointers used to change UI elements.
{
    m_processSerialData->setUiPointers(ui); //TODO: Re-assign this statement to m_CesarOperation->setUiPointers(ui); after implementing more functionalities.
    m_KJLOperation->setUiPointers(ui);
    m_KJL2Operation->setUiPointers(ui);
    m_MFCOperation->setUiPointers(ui);
    m_hofiOperation->setUiPointers(ui);
}

void MainWindow::connectSignals()
{
    //Connections for logs.
    connect(m_processSerialData->m_serialConfig, &SerialConfig::sendToLog, this, &MainWindow::receiveLog, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::sendToLog, this, &MainWindow::receiveLog, Qt::QueuedConnection);
    connect(m_SPSConfig, &SPSConfig::sendToLog, this, &MainWindow::receiveLog, Qt::QueuedConnection);

    //Connections for writing to serial port.
    connect(m_processSerialData, &ProcessSerialData::writeToCesarPort, m_processSerialData->m_serialConfig, &SerialConfig::writeCesarPort, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::writeToKJLPort, m_processSerialData->m_serialConfig, &SerialConfig::writeKJLPort, Qt::QueuedConnection);
    connect(m_KJLOperation, &KJLOperation::writeQueryData, m_processSerialData->m_serialConfig, &SerialConfig::writeKJLPort, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::writeToKJL2Port, m_processSerialData->m_serialConfig, &SerialConfig::writeKJL2Port, Qt::QueuedConnection);
    connect(m_KJL2Operation, &KJL2Operation::writeQueryData, m_processSerialData->m_serialConfig, &SerialConfig::writeKJL2Port, Qt::QueuedConnection);
    connect(m_MFCOperation, &MFCOperation::writetoMFCPort, m_processSerialData->m_serialConfig, &SerialConfig::writeMFCPort, Qt::QueuedConnection);
    connect(m_hofiOperation, &HofiOperation::writetoHofiPort, m_processSerialData->m_serialConfig, &SerialConfig::writeHofiPort, Qt::QueuedConnection);

    //Connections for processing received serial data.
    connect(m_processSerialData->m_serialConfig, &SerialConfig::cesar_readyToProcess, m_processSerialData, &ProcessSerialData::processCesar, Qt::QueuedConnection);
    connect(m_processSerialData->m_serialConfig, &SerialConfig::KJL_readyToProcess, m_processSerialData, &ProcessSerialData::processKJL, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJLSetEchoMode, m_processSerialData->m_serialConfig, &SerialConfig::setKJLEchoMode, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJLSetEchoMode, m_KJLOperation, &KJLOperation::setEchoMode, Qt::QueuedConnection);
    connect(m_processSerialData->m_serialConfig, &SerialConfig::KJL2_readyToProcess, m_processSerialData, &ProcessSerialData::processKJL2, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJL2SetEchoMode, m_processSerialData->m_serialConfig, &SerialConfig::setKJL2EchoMode, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJL2SetEchoMode, m_KJL2Operation, &KJL2Operation::setEchoMode, Qt::QueuedConnection);
    connect(m_processSerialData->m_serialConfig, &SerialConfig::MFC_readyToProcess, m_processSerialData, &ProcessSerialData::processMFC, Qt::QueuedConnection);
    connect(m_processSerialData->m_serialConfig, &SerialConfig::hofi_readyToProcess, m_processSerialData, &ProcessSerialData::processHofi, Qt::QueuedConnection);

    //Cesar operation connections.


    //KJL operation connections.
    connect(m_processSerialData, &ProcessSerialData::KJLSetOutput, m_KJLOperation, &KJLOperation::setOutput, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJLSetControlSource, m_KJLOperation, &KJLOperation::setControlSource, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJLSetPowerSetPoint, m_KJLOperation, &KJLOperation::setPowerSetPoint, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJLSetCapPosition, m_KJLOperation, &KJLOperation::setCapPosition, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJLQueryForwardPower, m_KJLOperation, &KJLOperation::queryForwardPower, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJLQueryExternalFeedback, m_KJLOperation, &KJLOperation::queryExternalFeedback, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJLQueryReflectedPower, m_KJLOperation, &KJLOperation::queryReflectedPower, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJLQueryStatus, m_KJLOperation, &KJLOperation::queryStatus, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJLSetCommsLinkStatus, m_KJLOperation, &KJLOperation::setCommsLinkStatus, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJLSetOutputRamping, m_KJLOperation, &KJLOperation::setOutputRamping, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJLSetRampDownTime, m_KJLOperation, &KJLOperation::setRampDownTime, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJLSetRampUpTime, m_KJLOperation, &KJLOperation::setRampUpTime, Qt::QueuedConnection);

    //KJL2 operation connections.
    connect(m_processSerialData, &ProcessSerialData::KJL2SetOutput, m_KJL2Operation, &KJL2Operation::setOutput, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJL2SetControlSource, m_KJL2Operation, &KJL2Operation::setControlSource, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJL2SetPowerSetPoint, m_KJL2Operation, &KJL2Operation::setPowerSetPoint, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJL2SetCapPosition, m_KJL2Operation, &KJL2Operation::setCapPosition, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJL2QueryForwardPower, m_KJL2Operation, &KJL2Operation::queryForwardPower, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJL2QueryExternalFeedback, m_KJL2Operation, &KJL2Operation::queryExternalFeedback, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJL2QueryReflectedPower, m_KJL2Operation, &KJL2Operation::queryReflectedPower, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJL2QueryStatus, m_KJL2Operation, &KJL2Operation::queryStatus, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJL2SetCommsLinkStatus, m_KJL2Operation, &KJL2Operation::setCommsLinkStatus, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJL2SetOutputRamping, m_KJL2Operation, &KJL2Operation::setOutputRamping, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJL2SetRampDownTime, m_KJL2Operation, &KJL2Operation::setRampDownTime, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::KJL2SetRampUpTime, m_KJL2Operation, &KJL2Operation::setRampUpTime, Qt::QueuedConnection);

    //MFC operation connections.
    connect(m_processSerialData, &ProcessSerialData::MFCQueryRange, m_MFCOperation, &MFCOperation::queryRange, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::MFCQueryRemoteMode, m_MFCOperation, &MFCOperation::queryRemoteMode, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::MFCSetRemoteMode, m_MFCOperation, &MFCOperation::setRemoteMode, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::MFCSetRange, m_MFCOperation, &MFCOperation::setRange, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::MFCSetValveState, m_MFCOperation, &MFCOperation::setValveState, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::MFCQueryValveState, m_MFCOperation, &MFCOperation::queryValveState, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::MFCQueryActualValue, m_MFCOperation, &MFCOperation::queryActualValue, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::MFCQueryActualValueSetSetpoint, m_MFCOperation, &MFCOperation::queryActualValueSetSetpoint, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::MFCInvalidCmd, m_MFCOperation, &MFCOperation::errorHandler, Qt::QueuedConnection);

    //Hofi operation connections.
    connect(this, &MainWindow::hofiSetOnOff, m_hofiOperation, &HofiOperation::setOnOff, Qt::QueuedConnection);
    connect(this, &MainWindow::hofiSetPort, m_hofiOperation, &HofiOperation::setPort, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::hofiSetOnOff, m_hofiOperation, &HofiOperation::setOnOff, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::hofiSetPort, m_hofiOperation, &HofiOperation::setPort, Qt::QueuedConnection);
    connect(m_processSerialData, &ProcessSerialData::hofiQueryCurrentState, m_hofiOperation, &HofiOperation::queryCurrentState, Qt::QueuedConnection);

}

void MainWindow::receiveLog(QString data)
{
    data.replace(6, "0x06");
    data.replace(10, "0x0A");
    data.replace('\r', "\\r");
    ui->teSerialLog->append(data);
}

void MainWindow::on_pbConnectPorts_released()
{
    if (ui->pbConnectPorts->text() == "Connect")
    {
        ui->teSerialLog->clear();
        if (checkportNames())
        {
            m_msgBox.setIcon(QMessageBox::Critical);
            m_msgBox.setText("Error: Duplicate port names.");
            m_msgBox.exec();
        }
        else
        {
            m_SPSConfig->initTcpServer();
            m_processSerialData->m_serialConfig->initSerialPorts();
            ui->cbCesarPort->setEnabled(false);
            ui->cbKJLPort->setEnabled(false);
            ui->cbKJL2Port->setEnabled(false);
            ui->cbMFCPort->setEnabled(false);
            ui->cbHofiPort->setEnabled(false);
            ui->pbConnectPorts->setText("Disconnect");
        }
    }
    else if (ui->pbConnectPorts->text() == "Disconnect")
    {
        m_processSerialData->m_serialConfig->closePorts();
        m_SPSConfig->stopListening();
        ui->cbCesarPort->setEnabled(true);
        ui->cbKJLPort->setEnabled(true);
        ui->cbKJL2Port->setEnabled(true);
        ui->cbMFCPort->setEnabled(true);
        ui->cbHofiPort->setEnabled(true);
        ui->pbConnectPorts->setText("Connect");
    }
}

void MainWindow::on_btnHofi_OnOff_released()
{
    m_hofiOperation->onOffState = !m_hofiOperation->onOffState;
    emit hofiSetOnOff(m_hofiOperation->onOffState);
}

void MainWindow::on_btnHofi_Port1_released()
{
    emit hofiSetPort('1');
}

void MainWindow::on_btnHofi_Port2_released()
{
    emit hofiSetPort('2');
}

void MainWindow::on_btnHofi_Port3_released()
{
    emit hofiSetPort('3');
}

void MainWindow::on_btnHofi_Port4_released()
{
    emit hofiSetPort('4');
}

void MainWindow::on_btnHofi_Port5_released()
{
    emit hofiSetPort('5');
}
