// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
//  модель РЛС
// ======================================================================

#ifndef RADARSTATIONMODEL_H
#define RADARSTATIONMODEL_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QMap>
//#include <QMapIterator>

// текущее состояние РЛС
class RadarStationModel : public QObject
{
    Q_OBJECT
public:
    explicit RadarStationModel(QObject *parent = 0);
    ~RadarStationModel();

signals:
    // передать посылку управления состояния РЛС
    void writeRlmCtrlPacket(QByteArray* packet);
    void writeRpCtrlPacket(QByteArray* packet);
    void writeVrliCtrlPacket(QByteArray* packet);

    // изменение модели
    void rlmStateChanged();
    void rpStateChanged();
    void vrliStateChanged();
    void prliStateChanged();

public slots:

    // получение данных от РЛС
    void updateRlmRaw(QByteArray* ba);
    void updateRpRaw(QByteArray* ba);
    void updateVrliRaw(QByteArray* ba);
    void updatePrliRaw(QByteArray* ba);

    // управление РЛС

    // состояние обработки данных
    void writeRpCmd(const QString& name, const QString& param);
    // состояние РЛС
    void writeRlmCmd(const QString& name, const QString& param);
    // цели
    void writeVrliCmd(const QString& name, const QString& param);

protected slots:
    void updateRpCmd(const QString& name, const QString& param);
    void updateRlmCmd(const QString& name, const QString& param);

public:

    QString getRpCmdValue(const QString& name)
    {
        return rpCmdLst[name];
    }
    QByteArray getRpRaw();

    QString getRlmCmdValue(const QString& name)
    {
        return rlmCmdLst[name];
    }
    QByteArray getRlmRaw();

    QByteArray getVrliRaw();
    QByteArray getPrliRaw();

    // параметры соединения с РЛС
    void setHost(const QString& host);
    QString getHost() const;

    void getRlmConnectionParams(quint16& statePort, quint16& ctrlPort) const;
    void setRlmConnectionParams(quint16 statePort, quint16 ctrlPort);

    void getRpConnectionParams(quint16& statePort, quint16& ctrlPort) const;
    void setRpConnectionParams(quint16 statePort, quint16 ctrlPort);

    void getVrliConnectionParams(quint16& statePort, quint16& ctrlPort) const;
    void setVrliConnectionParams(quint16 statePort, quint16 ctrlPort);

    void getPrliConnectionParams(quint16& statePort) const;
    void setPrliConnectionParams(quint16 statePort);

    void getNmeaConnectionParams(quint16& ctrlPort) const;
    void setNmeaConnectionParams(quint16 ctrlPort);

protected:

    void initConnectionParams();

    // парсеры датаграмм протоколов
    void parseRlmUdpDatagramm(const QByteArray& datagram);
    void parseRpUdpDatagramm(const QByteArray& datagram);
    void parseVrliUdpDatagramm(const QByteArray& datagram);
    void parsePrliUdpDatagramm(const QByteArray& datagram);

protected:

    // последние полученные от РЛС датаграммы
    QByteArray rpRaw;
    QByteArray rlmRaw;
    QByteArray vrliRaw;
    QByteArray prliRaw;

    QMap<QString, QString> rpCmdLst;   // состояния режимов обработки данных от РЛС
    QMap<QString, QString> rlmCmdLst;  // состояния режимов состояния РЛС

    // ПРЛИ передаётся по одной или несколько линеек в посылке, так,
    // чтобы суммарный объём данных не превышал 1460 байт.
    // РЛС выполняет 20 оборотов в минуту и на каждом обороте выдает 4095 линеек,
    // т.е. 4095*20/60 = 1365 линеек в секунду.

    // TODO: хранить последний оборот

    // TODO: хранить список целей

    // параметры подключения к РЛС
    quint16 rlmStatePort;
    quint16 rlmCtrlPort;

    quint16 rpStatePort;
    quint16 rpCtrlPort;

    quint16 vrliStatePort;
    quint16 vrliCtrlPort;

    quint16 prliStatePort;

    quint16 nmeaCtrlPort;

    QString ctrlHost;
};

#endif // RADARSTATIONMODEL_H
