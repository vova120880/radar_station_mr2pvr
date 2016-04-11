#include "UdpSocketThread.h"

MyUdpSocket::MyUdpSocket(QObject *parent) 
	: QObject(parent) 
	, udpSocket(0)
{
}

MyUdpSocket::~MyUdpSocket() 
{
}

void MyUdpSocket::bind(quint16 _inPort, quint16 _outPort)
{
	inPort = _inPort;
	outPort = _outPort;
	if(!udpSocket)
	{
		udpSocket = new QUdpSocket();
        //udpSocket->setReadBufferSize(gAnswerPacketSize);
		connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readData()));
		connect(udpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
	}
    udpSocket->bind(inPort, QUdpSocket::ShareAddress);

    emit logMessage("MyUdpSocket::bind: "+ QString::number(inPort));

	//udpSocket->bind(QHostAddress::QHostAddress("127.0.0.1"), inPort, QUdpSocket::DontShareAddress);
}

void MyUdpSocket::socketError(QAbstractSocket::SocketError socketError)
{
	emit displayError(socketError);
}

void MyUdpSocket::readData()
{
    emit logMessage("MyUdpSocket::readData: "
                    + QString::number(udpSocket->pendingDatagramSize()));

	QByteArray* in = new QByteArray();
	QByteArray rdata;
    while (udpSocket->hasPendingDatagrams()) 
	{
        rdata.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(rdata.data(), rdata.size());
		in->append(rdata);
    }
	emit datagramReceived(in);
}

void MyUdpSocket::writeData(QByteArray* datagram)
{
    emit logMessage("MyUdpSocket::writeData: " + QString::number(datagram->size()));

	while(udpSocket->writeDatagram(datagram->data(), datagram->size(), QHostAddress::Broadcast, outPort) != datagram->size());
	//while(udpSocket->writeDatagram(datagram->data(), datagram->size(), QHostAddress::QHostAddress("127.0.0.1"), outPort) != datagram->size());
	emit datagramSended();
}

//--------------------------------------------------------------------------------------------------------

UdpSocketThread::UdpSocketThread(QObject *parent)
    : QThread(parent)
	, restart(false)
	, abort(false)
{
}

UdpSocketThread::~UdpSocketThread()
{
    abort = true; 
}

void UdpSocketThread::slotBind(quint16 _inPort, quint16 _outPort)
{
    emit signalBind(_inPort, _outPort);
}

void UdpSocketThread::sendDatagram(QByteArray* datagram)
{
	msleep(200);
	emit writeData(datagram);
}

void UdpSocketThread::run()
{
	
	MyUdpSocket* socket = new MyUdpSocket();
	
	QObject::connect(this, SIGNAL(signalBind(quint16, quint16)), socket, SLOT(bind(quint16, quint16)));
	QObject::connect(this, SIGNAL(writeData(QByteArray*)), socket, SLOT(writeData(QByteArray*)));

	QObject::connect(socket, SIGNAL(displayError(QAbstractSocket::SocketError)), this, SIGNAL(displayError(QAbstractSocket::SocketError)));
	QObject::connect(socket, SIGNAL(datagramReceived(QByteArray*)), this, SIGNAL(datagramReceived(QByteArray*)));
	QObject::connect(socket, SIGNAL(datagramSended()), this, SIGNAL(datagramSended()));
	QObject::connect(socket, SIGNAL(logMessage(const QString&)), this, SIGNAL(logMessage(const QString&)));

	exec(); 

	delete socket;
}

