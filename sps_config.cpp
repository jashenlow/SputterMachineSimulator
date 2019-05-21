#include "sps_config.h"

SPSConfig::SPSConfig(QObject *parent) : QObject(parent)
{
    m_TcpServer = new QTcpServer(this);
    m_ServerSocket= new QTcpSocket(this);

    connect(m_TcpServer, &QTcpServer::newConnection, this, &SPSConfig::newConnection, Qt::QueuedConnection);
}

SPSConfig::~SPSConfig()
{
    delete m_TcpServer;
}

void SPSConfig::newConnection()
{
    m_ServerSocket = m_TcpServer->nextPendingConnection();
    tcpDescriptor = int(m_ServerSocket->socketDescriptor());
    connect(m_ServerSocket, &QAbstractSocket::disconnected, m_ServerSocket, &QObject::deleteLater);
    connect(m_ServerSocket, &QTcpSocket::readyRead, this, &SPSConfig::readIncomingData, Qt::QueuedConnection);

    if (m_ServerSocket->waitForConnected(3000))
    {
        if ((m_ServerSocket->state() == QAbstractSocket::ConnectedState) && m_ServerSocket->open(QIODevice::ReadWrite))
        {
            emit sendToLog("TCP new client connected. (" + QString::number(tcpDescriptor) + ")");
        }
    }
}

void SPSConfig::initTcpServer()
{
    SPSIpAddress = "192.168.0.2";  //Referenced from the SputterAutomation config file plcconfig.xml.
    port = 102;     //Referenced from the SputterAutomation config file plcconfig.xml.

    startListening(SPSIpAddress, port);
}

void SPSConfig::startListening(QString addr, quint16 port)
{
    if (!m_TcpServer->isListening())
    {
        if (m_TcpServer->listen(QHostAddress(addr), port))
        {
            emit sendToLog("TCP Server started. Listening on " + m_TcpServer->serverAddress().toString() + ":" + QString::number(m_TcpServer->serverPort()));
        }
        else
        {
            emit sendToLog("TCP Server unable to start.");
        }
    }
    else
    {
        emit sendToLog("TCP Server is already listening on " + m_TcpServer->serverAddress().toString() + ":" + QString::number(m_TcpServer->serverPort()));
    }
}

void SPSConfig::stopListening()
{
    if (m_TcpServer->isListening())
    {
        m_TcpServer->close();
        emit sendToLog("TCP Server closed.");
    }
    else
    {
        emit sendToLog("TCP Server already not listening");
    }
}

void SPSConfig::readIncomingData()
{
    QByteArray receivedData;

    if (m_ServerSocket->bytesAvailable())
    {
        receivedData = m_ServerSocket->readAll();
        emit sendToLog("TCP received data: " + receivedData);
    }

    //m_ServerSocket->write("wassup");
    //m_ServerSocket->flush();
    //m_ServerSocket->waitForBytesWritten(-1);
}
