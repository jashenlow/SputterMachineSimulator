#ifndef SERIAL_CONFIG_H
#define SERIAL_CONFIG_H

#include <QObject>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>

class SerialConfig : public QObject
{
    Q_OBJECT

public:
    explicit SerialConfig(QObject *parent = nullptr);
    ~SerialConfig();

    QStringList serialNames;

    void enumeratePorts();
    void initSerialPorts();
    void closePorts();
    int calculate_checkSum(QByteArray packet);

    //Add more pointers for new devices here as well.
    QSerialPort *m_cesarSerial, *m_KJLSerial, *m_KJL2Serial, *m_MFCSerial, *m_hofiSerial;


signals:
    void sendToLog(QString data);

    void cesar_readyToProcess(uint8_t command, QByteArray data);
    void KJL_readyToProcess(QByteArray command);
    void KJL2_readyToProcess(QByteArray command);
    void MFC_readyToProcess(QByteArray command);
    void hofi_readyToProcess(QByteArray command);

public slots:
    void writeCesarPort(QByteArray packet);
    void writeKJLPort(QByteArray command);
    void writeKJL2Port(QByteArray command);
    void writeMFCPort(QByteArray command);
    void writeHofiPort(QByteArray command);

    void setKJLEchoMode(bool mode);
    void setKJL2EchoMode(bool mode);

private slots:
    void readCesarData();
    void readKJLData();
    void readKJL2Data();
    void readMFCData();
    void readHofiData();
    void cesarACKTimeout();

private:
    QByteArrayList  KJLEchoCmdQueue;
    QByteArrayList  KJL2EchoCmdQueue;
    QByteArray      cesarReceiveBuffer;
    QByteArray      KJLReceiveBuffer;
    QByteArray      KJL2ReceiveBuffer;
    QByteArray      MFCReceiveBuffer;
    QByteArray      hofiReceiveBuffer;
    QStringList     forbiddenSerialPorts;
    QByteArrayList  cesarWriteQueue;
    QByteArrayList  KJLWriteQueue;
    QByteArrayList  KJL2WriteQueue;
    QByteArrayList  MFCWriteQueue;
    QByteArrayList  hofiWriteQueue;
    bool            cesarACKTimer_flag;
    bool            KJLEchoMode;
    bool            KJL2EchoMode;

    QTimer          *m_ACKTimer;

    //For future developments, add more structs for new devices here.
    struct CesarSettings
    {
        qint32 cesar_baudRate = QSerialPort::Baud9600;
        QSerialPort::DataBits cesar_dataBits = QSerialPort::Data8;
        QSerialPort::Parity cesar_parity = QSerialPort::OddParity;
        QSerialPort::StopBits cesar_stopBits = QSerialPort::OneStop;
        QSerialPort::FlowControl cesar_flowControl = QSerialPort::NoFlowControl;
    };

    struct KJLSettings
    {
        qint32 kjl_baudRate = QSerialPort::Baud19200;
        QSerialPort::DataBits kjl_dataBits = QSerialPort::Data8;
        QSerialPort::Parity kjl_parity = QSerialPort::NoParity;
        QSerialPort::StopBits kjl_stopBits = QSerialPort::OneStop;
        QSerialPort::FlowControl kjl_flowControl = QSerialPort::NoFlowControl;
    };

    struct KJL2Settings
    {
        qint32 kjl2_baudRate = QSerialPort::Baud19200;
        QSerialPort::DataBits kjl2_dataBits = QSerialPort::Data8;
        QSerialPort::Parity kjl2_parity = QSerialPort::NoParity;
        QSerialPort::StopBits kjl2_stopBits = QSerialPort::OneStop;
        QSerialPort::FlowControl kjl2_flowControl = QSerialPort::NoFlowControl;
    };

    struct MFCSettings
    {
        qint32 mfc_baudRate = QSerialPort::Baud9600;
        QSerialPort::DataBits mfc_dataBits = QSerialPort::Data8;
        QSerialPort::Parity mfc_parity = QSerialPort::NoParity;
        QSerialPort::StopBits mfc_stopBits = QSerialPort::OneStop;
        QSerialPort::FlowControl mfc_flowControl = QSerialPort::NoFlowControl;
    };

    struct HofiSettings
    {
        qint32 hofi_baudRate = QSerialPort::Baud9600;
        QSerialPort::DataBits hofi_dataBits = QSerialPort::Data8;
        QSerialPort::Parity hofi_parity = QSerialPort::NoParity;
        QSerialPort::StopBits hofi_stopBits = QSerialPort::OneStop;
        QSerialPort::FlowControl hofi_flowControl = QSerialPort::NoFlowControl;
    };

    CesarSettings   m_CesarSettings;
    KJLSettings     m_KJLSettings;
    KJL2Settings    m_KJL2Settings;
    MFCSettings     m_MFCSettings;
    HofiSettings    m_HofiSettings;

    void readValidPacket(QByteArray packet);
};

#endif // SERIAL_CONFIG_H
