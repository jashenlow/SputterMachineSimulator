#ifndef MFC_OPERATION_H
#define MFC_OPERATION_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include "ui_mainwindow.h"

class MFCOperation : public QObject
{
    Q_OBJECT

public:
    explicit MFCOperation(QObject *parent = nullptr);
    ~MFCOperation();

    void MFCloadDefaults();
    void setUiPointers(Ui::MainWindow *ptr);

signals:
    void writetoMFCPort(QByteArray data);

public slots:
    void queryRange(int channel);
    void queryRemoteMode();
    void queryValveState(char channel);
    void queryActualValue(char channel);
    void queryActualValueSetSetpoint(char channel, double setPoint);
    void setRemoteMode(QByteArray mode);
    void setRange(char channel, QString data);
    void setValveState(char channel, QByteArray state);
    void errorHandler(QByteArray error);

private slots:
    void updateDisplay();

private:
    bool remoteMode, setPoint_EN, CH1_valveState, CH2_valveState;
    int CH1_unit, CH2_unit, displayNumber;
    double FL1, FL2,
           SP1, SP2,
           PR1, PR2,
           RNG1, RNG2,
           GAIN1, OFFS1,
           GAIN2, OFFS2;

    QStringList powerOnDisplay;
    QStringList display1, display2, display3, display4;
    QStringList unitString;

    struct ErrorCode
    {
        QByteArray syntax          = "#E010";  //Syntax Error.
        QByteArray cmdExec         = "#E020";  //Command execution error.
        QByteArray comms           = "#E001";  //Communication error.
        QByteArray ADC             = "#E002";  //ADC Overflow/Underflow.
        QByteArray rangeSetpoint   = "#E003";  //Range error/Setpoint < 0 or out of range.
        QByteArray offset          = "#W001";  //Offset > 250mV.
    };

    enum Unit
    {
        ubar = 0, mbar, bar, mTor, Torr, KTor, Pa, kPa, mH2O, cH2O, PSI,
        N_qm, SCCM, SLM, SCM, SCFH, SCFM, mA, Volt, Percent, Celcius
    };

    void setDisplay(QStringList display);

    ErrorCode   m_errorCode;
    Ui::MainWindow  *m_ui;
    QTimer  *m_dispTimer;
};

#endif // MFC_OPERATION_H
