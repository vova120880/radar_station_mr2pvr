// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
// ======================================================================

#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <QtNetwork/qudpsocket>

// udp сокет для приема данных
class UdpSocket : public QUdpSocket
{
    Q_OBJECT

public:
    UdpSocket(QObject *parent = 0);
    ~UdpSocket();

public slots:
    void writeData(QByteArray* datagram);
    void listen(quint16 inPort_, quint16 outPort_);
    void listen(quint16 inPort_);

private slots:
    void readData();
    void socketError(QAbstractSocket::SocketError socketError);

signals:
    void datagramReceived(QByteArray* datagram);
    void datagramSended();
    void displayError(QAbstractSocket::SocketError socketError);
    void logMessage(const QString& msg);

protected:

    quint16 outPort;
    quint16 inPort;
};

#endif // UDPSOCKET_H
