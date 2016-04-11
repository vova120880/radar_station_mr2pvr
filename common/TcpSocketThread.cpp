#include "TcpSocketThread.h"
#include "./../radarStationPrototype/rscontroller/tcpsocket.h"

Receiver::Receiver(QObject *parent) 
	: QTcpSocket(parent)
{
	//qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
	//connect(tcpSocket, SIGNAL(connected()), this, SIGNAL(connected()));
	//connect(tcpSocket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
	connect(this, SIGNAL(readyRead()), this, SLOT(readData()));		
}

Receiver::~Receiver() 
{
    abort();
    //waitForDisconnected();
}

void Receiver::displayError(QAbstractSocket::SocketError err)
{
    QAbstractSocket::SocketState st = state();
    int i = 0;
}

void Receiver::connect2Host(const QString& _host, quint16 _port)
{
	abort();
	host = _host;
	port = _port;
	connectToHost(host, port);			
}

void Receiver::disconnect()
{
	abort();
}

void Receiver::readData()
{
	QDataStream in(this);
    in.setVersion(QDataStream::Qt_5_4);

	if(bytesAvailable() <= 0)
		return;
		
	int sz = bytesAvailable();
	char* packet = new char[sz + 1];
	memset(packet, 0, sz + 1);
	in.readRawData(packet, sz);
	emit packetReceived(packet, sz);	
}

void  Receiver::writePacket(QString packet)
{
	write(packet.toStdString().c_str(), packet.toStdString().size());
}

void Receiver::writePacket(QByteArray packet)
{
    write(packet.data(), packet.size());
    emit logMessage(
                QString("write to %1 socket %2 bytes")
                .arg(port).arg(packet.size())
                );
}

//--------------------------------------------------------------------------------------
/*
Sender::Sender(QObject *parent)
    : QTcpServer(parent)
{
    //qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    //connect(tcpSocket, SIGNAL(connected()), this, SIGNAL(connected()));
    //connect(tcpSocket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    //connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(displayError(QAbstractSocket::SocketError)));
    connect(this, SIGNAL(readyRead()), this, SLOT(readData()));
}

Sender::~Sender()
{
    abort();
    //waitForDisconnected();
}
*/

TcpSocketThread::TcpSocketThread(QObject *parent)
    : QThread(parent)
	, restart(false)
	, abort(false)
    //, receiver(NULL)
{
}

TcpSocketThread::~TcpSocketThread()
{
	abort = true; 
	deleteReceiver();
}

void TcpSocketThread::deleteReceiver()
{
    /*
	if(receiver)
	{
        delete receiver;
        receiver = NULL;
	}
    */
}

void TcpSocketThread::connect2Device(const QString& host, quint16 port)
{
	emit connectToHost(host, port);
}

void TcpSocketThread::disconnect()
{
	emit disconnectFromHost();
}

void TcpSocketThread::run()
{
    // Receiver* receiver = new Receiver();

    TcpSocket* receiver = new TcpSocket();
	
	connect(this, SIGNAL(connectToHost(const QString&, quint16)), receiver, SLOT(connect2Host(const QString&, quint16)));
	connect(this, SIGNAL(disconnectFromHost()), receiver, SLOT(disconnect()));
	connect(this, SIGNAL(writeData(QString)), receiver, SLOT(writePacket(QString)));
    connect(this, SIGNAL(writePacket(QByteArray*)), receiver, SLOT(writePacket(QByteArray*)));

	connect(receiver, SIGNAL(connected()), this, SIGNAL(connected()));
	connect(receiver, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
//	connect(receiver, SIGNAL(displayError(QAbstractSocket::SocketError)), this, SIGNAL(displayError(QAbstractSocket::SocketError)));
	connect(receiver, SIGNAL(packetReceived(char*, int)), this, SLOT(packetReceived(char*, int)));
	connect(receiver, SIGNAL(logMessage(const QString&)), this, SIGNAL(logMessage(const QString&)));		
	
	exec();	

    //deleteReceiver();
    delete receiver;
}

void TcpSocketThread::packetReceived(char* packet, int sz)
{
	QString msg = packet;
	emit logMessage(msg);
}
