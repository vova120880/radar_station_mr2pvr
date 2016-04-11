// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
// ======================================================================

#include "rsprotocolworker.h"
#include "./../rssockets/udpsocket.h"
#include "./../rssockets/tcpsocket.h"

RsProtocolWorker::RsProtocolWorker(QObject *parent)
    : QObject(parent)
{
    stateSocket = new UdpSocket();
    ctrlSocket = new TcpSocket();

    QObject::connect(
                this, SIGNAL(setStatePort(quint16)),
                stateSocket, SLOT(listen(quint16)));

    QObject::connect(
                stateSocket, SIGNAL(datagramReceived(QByteArray*)),
                this, SIGNAL(statePacketReceived(QByteArray*)));

    QObject::connect(
                stateSocket, SIGNAL(logMessage(const QString&)),
                this, SIGNAL(logMessage(const QString&)));

    QObject::connect(
                this, SIGNAL(setCtrlHost(const QString&, quint16)),
                ctrlSocket, SLOT(connect2Host(const QString&, quint16)));

    QObject::connect(
                this, SIGNAL(writeCtrlPacket(QByteArray*)),
                ctrlSocket, SLOT(writePacket(QByteArray*)));

    QObject::connect(ctrlSocket, SIGNAL(connected()), this, SIGNAL(connected()));
    QObject::connect(ctrlSocket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));

    QObject::connect(
                ctrlSocket, SIGNAL(logMessage(const QString&)),
                this, SIGNAL(logMessage(const QString&)));

}

RsProtocolWorker::~RsProtocolWorker()
{
    delete stateSocket;
    delete ctrlSocket;
}

