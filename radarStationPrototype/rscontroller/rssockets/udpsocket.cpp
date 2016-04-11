// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
// ======================================================================

#include "udpsocket.h"

UdpSocket::UdpSocket(QObject *parent)
    : QUdpSocket(parent)
    , inPort(0)
    , outPort(0)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
}

UdpSocket::~UdpSocket()
{
}

void UdpSocket::listen(quint16 inPort_, quint16 outPort_)
{
    abort();
    inPort = inPort_;
    outPort = outPort_;
    bind(inPort, QUdpSocket::ShareAddress);
    emit logMessage(QString("UdpSocket::bind: %1, state: %2")
                    .arg(inPort)
                    .arg(state()));
}

void UdpSocket::listen(quint16 inPort_)
{
    abort();
    inPort = inPort_;
    bind(inPort, QUdpSocket::ShareAddress);
    emit logMessage(QString("UdpSocket::bind: %1, state: %2")
                    .arg(inPort)
                    .arg(state()));
}

void UdpSocket::socketError(QAbstractSocket::SocketError socketError)
{
    emit displayError(socketError);
}

void UdpSocket::readData()
{
    QByteArray* in = new QByteArray();

    while(hasPendingDatagrams())
    {
        in->resize(pendingDatagramSize());
        readDatagram(in->data(), in->size());
    }
    emit logMessage(QString("UdpSocket::read: %1, sz: %2")
                    .arg(inPort)
                    .arg(in->size()));

    emit datagramReceived(in);
}

void UdpSocket::writeData(QByteArray* datagram)
{
    emit logMessage("UdpSocket::writeData: " + QString::number(datagram->size()));
    while(writeDatagram(datagram->data(), datagram->size(), QHostAddress::Broadcast, outPort) != datagram->size());
    //while(udpSocket->writeDatagram(datagram->data(), datagram->size(), QHostAddress::QHostAddress("127.0.0.1"), outPort) != datagram->size());
    emit datagramSended();

    delete datagram;
}


