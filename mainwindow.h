#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>
#include "process_serial_data.h"
#include "cesar_operation.h"
#include "KJL_operation.h"
#include "KJL2_operation.h"
#include "MFC_operation.h"
#include "Hofi_operation.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


signals:
    void hofiSetOnOff(bool state);
    void hofiSetPort(char port);

private slots:
    void receiveLog(QString data);
    void on_pbConnectPorts_released();
    void on_btnHofi_OnOff_released();
    void on_btnHofi_Port1_released();
    void on_btnHofi_Port2_released();
    void on_btnHofi_Port3_released();
    void on_btnHofi_Port4_released();
    void on_btnHofi_Port5_released();

private:
    Ui::MainWindow *ui;
    ProcessSerialData   *m_processSerialData;
    CesarOperation      *m_cesarOperation;
    KJLOperation        *m_KJLOperation;
    KJL2Operation        *m_KJL2Operation;
    MFCOperation        *m_MFCOperation;
    HofiOperation       *m_hofiOperation;
    QMessageBox         m_msgBox;

    QByteArray port_assignedStates;

    void initUiPointers();
    void connectSignals();
    void initSerialComboBoxes();
    bool checkportNames();
};

#endif // MAINWINDOW_H
