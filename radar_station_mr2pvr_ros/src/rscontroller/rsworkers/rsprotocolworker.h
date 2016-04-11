// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
// абстракция протокола РЛС
// ======================================================================

#ifndef RSPROTOCOLWORKER_H
#define RSPROTOCOLWORKER_H

#include <QObject>

QT_BEGIN_NAMESPACE
class UdpSocket;
class TcpSocket;
QT_END_NAMESPACE

class RsProtocolWorker : public QObject
{
    Q_OBJECT
public:
    explicit RsProtocolWorker(QObject *parent = 0);
    ~RsProtocolWorker();

signals:
    // номер udp порта для получения датаграмм состояния
    void setStatePort(quint16 inPort);
    // ip адрес и номер tcp порта для управления режимима работы
    void setCtrlHost(const QString& host, quint16 port);
    // подключиться к РЛС
    void connectToRadarStation();
    // получена датаграмма состояния
    void statePacketReceived(QByteArray* packet);
    // передать посылку управления
    void writeCtrlPacket(QByteArray* packet);


    void logMessage(const QString& msg);

    // temp
    void connected();
    void disconnected();

public slots:

protected:
    UdpSocket* stateSocket;
    TcpSocket* ctrlSocket;
};

#endif // RSPROTOCOLWORKER_H
