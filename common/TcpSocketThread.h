#ifndef _TCPSOCKETTHREAD_H_
#define _TCPSOCKETTHREAD_H_

#include <QtCore/QObject>
#include <QtNetwork/qtcpsocket>
#include <QtNetwork/qtcpserver.h>
#include <QtCore/QThread>

#include <vector>

QT_BEGIN_NAMESPACE
class QTcpSocket;
class QThread;
QT_END_NAMESPACE 

class Receiver : public QTcpSocket
{
	Q_OBJECT

public:
	Receiver(QObject *parent = 0);
	~Receiver();

public slots:
	void readData();

    void writePacket(QString packet);
    void writePacket(QByteArray packet);

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
/*
class Sender : public QTcpServer
{
    Q_OBJECT

public:
    Sender(QObject *parent = 0);
    ~Sender();

public slots:
    void readData();

    void writePacket(QString packet);
    void writePacket(QByteArray* packet);

    void connect2Host(const QString& host, quint16 port);
    void disconnect();

signals:
    void packetReceived(char* packet, int sz);
    void logMessage(const QString& msg);

protected:

    QString host;
    quint16 port;
};
*/
class TcpSocketThread :  public QThread
{
	Q_OBJECT

public:
	TcpSocketThread(QObject *parent = 0);
	~TcpSocketThread();

public slots:
	void connect2Device(const QString& host, quint16 port);
	void disconnect();
	
	void packetReceived(char* packet, int sz);

signals:	
	
	void packetReceived(QString packet); 
	void connectToHost(const QString& host, quint16 port);
	void disconnectFromHost();
	
	void writeData(QString packet);
    void writePacket(QByteArray packet);

	void connected();
	void disconnected();
	void displayError(QAbstractSocket::SocketError socketError);

	void logMessage(const QString& msg);

protected:
    void run();
	void deleteReceiver();
	
private:
	
	bool restart;
	bool abort;

    //Receiver* receiver;
};

#endif //_TCPSOCKETTHREAD_H_
