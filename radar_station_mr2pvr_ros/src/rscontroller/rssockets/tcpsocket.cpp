// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
//
// ======================================================================

#include "tcpsocket.h"
#include <QDataStream>

TcpSocket::TcpSocket(QObject *parent)
    : QTcpSocket(parent)
{
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
    connect(this, SIGNAL(readyRead()), this, SLOT(readData()));
}

TcpSocket::~TcpSocket()
{
    abort();
    //waitForDisconnected();
}

void TcpSocket::displayError(QAbstractSocket::SocketError err)
{
    emit logMessage(
                QString("TCP socket %1, error: %2, state: %3")
                .arg(port)
                .arg(errorString())//err)
                .arg(state())
                );
}

void TcpSocket::connect2Host(const QString& host_, quint16 port_)
{
    abort();
    host = host_;
    port = port_;
    connectToHost(host, port);
}

void TcpSocket::disconnect()
{
    abort();
}

void TcpSocket::readData()
{
    QDataStream in(this);
    in.setVersion(QDataStream::Qt_5_2);

    if(bytesAvailable() <= 0)
        return;

    int sz = bytesAvailable();
    char* packet = new char[sz + 1];
    memset(packet, 0, sz + 1);
    in.readRawData(packet, sz);
    emit packetReceived(packet, sz);
}

void TcpSocket::writePacket(QString packet)
{
    write(packet.toStdString().c_str(), packet.toStdString().size());
}

void TcpSocket::writePacket(QByteArray* packet)
{
    write(packet->data(), packet->size());
    emit logMessage(
                QString("write to %1 socket %2 bytes")
                .arg(port).arg(packet->size())
                );

    delete packet;
}

