// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
//
// ======================================================================

#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QtNetwork/QTcpSocket>
#include <QObject>

class TcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit TcpSocket(QObject *parent = 0);
    ~TcpSocket();

public slots:
    void readData();

    void writePacket(QString packet);
    void writePacket(QByteArray* packet);

    void connect2Host(const QString& host, quint16 port);
    void disconnect();

    void displayError(QAbstractSocket::SocketError);

signals:
    void packetReceived(char* packet, int sz);
    void logMessage(const QString& msg);

protected:

    std::vector<quint8> data;

    QString host;
    quint16 port;
};

#endif // TCPSOCKET_H
