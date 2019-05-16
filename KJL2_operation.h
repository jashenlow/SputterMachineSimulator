#ifndef KJL2_OPERATION_H
#define KJL2_OPERATION_H

#include <QObject>
#include <QDebug>
#include "ui_mainwindow.h"

class KJL2Operation : public QObject
{
    Q_OBJECT

public:
    explicit KJL2Operation(QObject *parent = nullptr);
    ~KJL2Operation();

    void setUiPointers(Ui::MainWindow *ptr);
    void KJL2loadDefaults();


signals:
    void writeQueryData(QByteArray data);

public slots:
    void setOutput(bool status);
    void setPowerSetPoint(int value);
    void setCapPosition(bool cap, int position);
    void setControlSource(QByteArray data);
    void queryForwardPower();
    void queryExternalFeedback();
    void queryReflectedPower();
    void queryStatus();
    void setCommsLinkStatus(bool status);
    void setEchoMode(bool mode);
    void setOutputRamping(bool mode);
    void setRampDownTime(int time);
    void setRampUpTime(int time);

private:
    Ui::MainWindow  *m_ui;

    bool outputRamping_EN;
    bool exciterMode;   //true = SLAVE, false = MASTER
    bool pulseMode; //true = ENABLED, false = DISABLED
    bool commsLinkStatus;   //true = Fault, false = OK
    bool outputStatus, interlockStatus, echoStatus;
    bool alarm_EXT, alarm_PAC, alarm_REF, alarm_MAX, alarm_TMP, alarm_XIMP, alarm_REF_EN;
    int powerSetPoint, forwardPower, controlSource, outputRegFeedback, loadCapPos, tuneCapPos, DCBias, reflectedPower;
    int rampUpTime, rampDownTime;
    int limit_maxPower, limit_refPower, limit_PACurrent, limit_impedance, limit_temp, limit_DCVoltage, limit_dissipation;
    QString alarmStatus;

    enum ControlandSetpointSource
    {
        Serial = 2,
        Analog = 1,
        FrontPanel = 0
    };
    enum OutputRegFeedback
    {
        InternalSensor = 3,
        ExternalFeedback = 0
    };

    struct Alarm
    {
        QString OFF = "";       //Meaning: No alarms activated.
        QString EXT = "EXT";    //Meaning: INTERLOCK signal is at a HIGH state. Ensure Analog Interface Connector is fully seated.
        QString PAC = "PAC";    //Meaning: Power Amplifier's current limit threshold has been met/exceeded. Limiting forward power.
        QString REF = "REF";    //Meaning: Reflected Power alarm threshold has been met/exceeded. Limiting forward power.
        QString MAX = "MAX";    //Meaning: Max Power limit threshold has been met/exceeded.
        QString TMP = "TMP";    //Meaning: Over-temperature detected from internal sensors.
        QString XIMP = "XIMP";  //Meaning: Plasma impedance has exceeded its threshold.
    };

    Alarm   m_alarm;

    void setAlarmStatus(QString status);
};

#endif // KJL2_OPERATION_H
