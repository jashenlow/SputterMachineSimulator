#ifndef CESAR_OPERATION_H
#define CESAR_OPERATION_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include "ui_mainwindow.h"

class CesarOperation : public QObject
{
    Q_OBJECT

public:
    explicit CesarOperation(QObject *parent = nullptr);
    ~CesarOperation();

    void setUiPointers(Ui::MainWindow *ptr);
    void cesarLoadDefaults();

signals:
    void writeToCesarPort(QByteArray data);


public slots:  
    void setOutputState(uint8_t command, bool state);
    void setRegulationMode(uint8_t command, char mode);
    void setPowerSetPoint(uint8_t command, int value);
    void setActiveControlMode(uint8_t command, int mode);
    void setMatchNetworkControl(uint8_t command, int mode);
    void setReflectedPowerLimit(uint8_t command, int value);
    void setReflectedPowerParameters(uint8_t command, int time, int value);
    void setCapPositions(uint8_t cmmand, bool cap, int value);

    void reportExternalFeedback(uint8_t command);
    void reportForwardPower(uint8_t command);
    void reportReflectedPower(uint8_t command);
    void reportCapPositions(uint8_t command);
    void reportSetPointandRegMode(uint8_t command);

private slots:
    void rampTimeout();
    void startRamping(bool mode);

private:
    bool ramping_EN;
    int setPoint, forwardPower, reflectedPower, DCBias, regulationMode, activeControlMode;
    int matchNetworkControl, reflectedPowerLimit, reflectedPowerLimitTime, forwardPowerLimit;
    int rampFinalPower, rampSteps;
    double loadCapPos, tuneCapPos, rampUpTime, rampDownTime, rampStepSize, dblForwardPower;
    bool outputState;

    enum RegulationMode
    {
        ForwardPower = 6,
        RealPower,
        Bias
    };

    enum ActiveControlMode
    {
        Host = 2,
        User = 4,
        FrontPanel = 6
        //More modes exist but they aren't important.
    };

    enum MatchNetworkControl
    {
        Manual = 0,
        Auto,
        Auto_With_Init
    };

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

    QByteArray assemblePacket(QByteArray packet);
    void sendReply(QByteArray data);
    void updateDisplay();

    Ui::MainWindow  *m_ui;
    QTimer          *m_rampTimer;

};

#endif // CESAR_OPERATION_H
