#ifndef HOFI_OPERATION_H
#define HOFI_OPERATION_H

#include <QObject>
#include <QDebug>
#include "ui_mainwindow.h"

class HofiOperation : public QObject
{
    Q_OBJECT

public:
    explicit HofiOperation(QObject *parent = nullptr);
    ~HofiOperation();

    bool onOffState;

    void setUiPointers(Ui::MainWindow *ptr);
    void hofiLoadDefaults();

signals:
    void writetoHofiPort(QByteArray data);

public slots:
    void setOnOff(bool state);
    void setPort(char port);
    void queryCurrentState();

private:
    char currentPort;

    enum Ports
    {
        Port1 = '1',
        Port2,
        Port3,
        Port4,
        Port5
    };

    Ui::MainWindow  *m_ui;
};

#endif // HOFI_OPERATION_H
