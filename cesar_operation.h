#ifndef CESAR_OPERATION_H
#define CESAR_OPERATION_H

#include <QObject>
#include <QDebug>
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
    void setOutputState(bool state);
    void setRegulationMode(char mode);
    void setPowerSetPoint(int value);

    void reportExternalFeedback(uint8_t command);
    void reportForwardPower(uint8_t command);
    void reportReflectedPower(uint8_t command);
    void reportCapPositions(uint8_t command);
    void reportSetPointandRegMode(uint8_t command);

private:
    int setPoint, forwardPower, reflectedPower, DCBias, regulationMode;
    int loadCapPos, tuneCapPos;
    bool outputState;

    enum RegulationMode
    {
        ForwardPower = 6,
        RealPower,
        Bias
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

    Ui::MainWindow  *m_ui;

};

#endif // CESAR_OPERATION_H
