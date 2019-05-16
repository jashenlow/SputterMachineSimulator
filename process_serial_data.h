#ifndef PROCESS_SERIAL_DATA_H
#define PROCESS_SERIAL_DATA_H

#include "serial_config.h"
#include "ui_mainwindow.h"

class ProcessSerialData : public QObject
{
    Q_OBJECT

public:
    explicit ProcessSerialData(QObject *parent = nullptr);
    ~ProcessSerialData();

    SerialConfig    *m_serialConfig;

    void setUiPointers(Ui::MainWindow *ptr);    //Pass the UI pointer to this class.

signals:
    void sendToLog(QString data);

    void writeToCesarPort(QByteArray packet);
    void writeToKJLPort(QByteArray command);
    void writeToKJL2Port(QByteArray command);

    void KJLSetOutput(bool status);
    void KJLSetPowerSetPoint(int value);
    void KJLSetCapPosition(bool cap, int position);
    void KJLQueryForwardPower();
    void KJLQueryExternalFeedback();
    void KJLQueryReflectedPower();
    void KJLQueryStatus();
    void KJLSetControlSource(QByteArray data);
    void KJLSetEchoMode(bool mode);
    void KJLSetCommsLinkStatus(bool status);
    void KJLSetOutputRamping(bool mode);
    void KJLSetRampUpTime(int time);
    void KJLSetRampDownTime(int time);

    void KJL2SetOutput(bool status);
    void KJL2SetPowerSetPoint(int value);
    void KJL2SetCapPosition(bool cap, int position);
    void KJL2QueryForwardPower();
    void KJL2QueryExternalFeedback();
    void KJL2QueryReflectedPower();
    void KJL2QueryStatus();
    void KJL2SetControlSource(QByteArray data);
    void KJL2SetEchoMode(bool mode);
    void KJL2SetCommsLinkStatus(bool status);
    void KJL2SetOutputRamping(bool mode);
    void KJL2SetRampUpTime(int time);
    void KJL2SetRampDownTime(int time);

    void MFCQueryRange(int channel);
    void MFCQueryRemoteMode();
    void MFCSetRemoteMode(QByteArray mode);
    void MFCSetRange(char channel, QString data);
    void MFCSetValveState(char channel, QByteArray state);
    void MFCQueryValveState(char channel);
    void MFCQueryActualValue(char channel);
    void MFCQueryActualValueSetSetpoint(char channel, double setPoint);
    void MFCInvalidCmd(QByteArray error);

    void hofiSetOnOff(bool state);
    void hofiSetPort(char port);
    void hofiQueryCurrentState();

public slots:
    void processCesar(uint8_t command, QByteArray data);
    void processKJL(QByteArray command);
    void processKJL2(QByteArray command);
    void processMFC(QByteArray command);
    void processHofi(QByteArray command);
    void KJLcommsLinkTimeout();
    void KJL2commsLinkTimeout();

private:
    QTimer     *m_KJLcommsLinkTimer, *m_KJL2commsLinkTimer;

    enum CSRcode    //Codes that are sent back to the PC after receiving any Cesar "set..." commands.
    {
        command_accepted = 0,
        controlCode_isIncorrect,
        output_isOn,
        data_out_of_range = 4,
        active_faults_exist = 7,
        data_byte_count_isIncorrect = 9,
        recipe_isActive = 19,
        frequency_out_of_range = 50,
        dutyCycle_out_of_range,
        device_controlled_not_detected = 53,
        command_not_accepted = 99
    };

    QByteArray  assemblePacket(QByteArray packet);
    int         KJLcommsLinkTime, KJL2commsLinkTime;

    Ui::MainWindow  *m_ui;
};

#endif // PROCESS_SERIAL_DATA_H
