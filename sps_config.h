#ifndef SPS_CONFIG_H
#define SPS_CONFIG_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>

class SPSConfig : public QObject
{
    Q_OBJECT

public:
    explicit SPSConfig(QObject *parent = nullptr);
    ~SPSConfig();

    void initTcpServer();
    void stopListening();

signals:
    void sendToLog(QString data);

private slots:
    void readIncomingData();

public slots:
    void newConnection();

private:
    QString SPSIpAddress;
    quint16 port;
    int     tcpDescriptor;

    void startListening(QString addr, quint16 port);

    QTcpServer  *m_TcpServer;
    QTcpSocket  *m_ServerSocket;
};

#endif // SPS_CONFIG_H
