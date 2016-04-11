#ifndef _UDP_SOCKET_THREAD_CDUMBE_H_
#define _UDP_SOCKET_THREAD_CDUMBE_H_

#include <QtCore/QObject>
#include <QtNetwork/qudpsocket>
#include <QtCore/QThread>

class MyUdpSocket : public QObject
{
    Q_OBJECT

public:
	MyUdpSocket(QObject *parent = 0);
	~MyUdpSocket();

public slots:
	void writeData(QByteArray* datagram);

private slots:
    void readData();
	void socketError(QAbstractSocket::SocketError socketError);
    void bind(quint16 _inPort, quint16 _outPort);

signals:
    void datagramReceived(QByteArray* datagram);
	void datagramSended();
    void displayError(QAbstractSocket::SocketError socketError);
	void logMessage(const QString& msg);	

protected:
	void parseQuery(const std::vector<char>& in);
    
	QUdpSocket *udpSocket;
    
	quint16 outPort;
	quint16 inPort;
};


class UdpSocketThread :  public QThread
{
    Q_OBJECT

public:
    UdpSocketThread(QObject *parent = 0);
    ~UdpSocketThread();

public slots:
    void slotBind(quint16 _inPort, quint16 _outPort);
	void sendDatagram(QByteArray* data); 

signals:    
    //
    void signalBind(quint16 _inPort, quint16 _outPort);
	void writeData(QByteArray* data); 

    //
	void datagramReceived(QByteArray* data);
	void datagramSended();
    void displayError(QAbstractSocket::SocketError socketError);
	void logMessage(const QString& msg);	

protected:
    void run();

private:
	
	bool restart;
    bool abort;
};

#endif //_UDP_SOCKET_THREAD_CDUMBE_H_
