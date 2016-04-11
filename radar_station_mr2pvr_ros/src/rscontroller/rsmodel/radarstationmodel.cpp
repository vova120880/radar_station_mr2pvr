// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
// ======================================================================

#include "radarstationmodel.h"
#include "./../rsprotocols/radarstationprotocols.h"

RadarStationModel::RadarStationModel(QObject *parent) : QObject(parent)
{
    initConnectionParams();
}

RadarStationModel::~RadarStationModel()
{

}

void RadarStationModel::initConnectionParams()
{
    ctrlHost = "127.0.0.1";

    rlmStatePort = 10002;
    rlmCtrlPort = 10003;

    rpStatePort = 10004;
    rpCtrlPort = 10005;

    vrliStatePort = 10000;
    vrliCtrlPort = 10000;

    prliStatePort = 10001;

    nmeaCtrlPort = 7000;
}

//-----------------------------------------------------------
// состояние обработки данных
//-----------------------------------------------------------

void RadarStationModel::updateRpCmd(const QString& name, const QString& param)
{
    rpCmdLst[name] = param;
}

void RadarStationModel::writeRpCmd(const QString& name, const QString& param)
{
    if(rpCmdLst[name] != param)
    {
        QString cmd = ptlst::BEGIN + ptlst::SEPARATOR;
        cmd += name;
        cmd += " " + param;
        cmd += ptlst::SEPARATOR + ptlst::END + ptlst::STOP;

        QByteArray* ba = new QByteArray(cmd.toLatin1());
        ba->append(char(0));

        emit writeRpCtrlPacket(ba);
    }
}

void RadarStationModel::updateRpRaw(QByteArray* ba)
{
    rpRaw = *ba;
    delete ba;
    parseRpUdpDatagramm(rpRaw);

    emit rpStateChanged();
}

//-----------------------------------------------------------
// состояние РЛС
//-----------------------------------------------------------

void RadarStationModel::updateRlmCmd(const QString& name, const QString& param)
{
    rlmCmdLst[name] = param;
}

void RadarStationModel::writeRlmCmd(const QString& name, const QString& param)
{
    if(rlmCmdLst[name] != param)
    {
        QString cmd = ptlst::BEGIN + ptlst::SEPARATOR;
        cmd += name;
        cmd += " " + param;
        cmd += ptlst::SEPARATOR + ptlst::END + ptlst::STOP;

        QByteArray* ba = new QByteArray(cmd.toLatin1());
        ba->append(char(0));

        emit writeRlmCtrlPacket(ba);
    }
}

void RadarStationModel::updateRlmRaw(QByteArray* ba)
{
    rlmRaw = *ba;
    delete ba;
    parseRlmUdpDatagramm(rlmRaw);

    emit rlmStateChanged();
}

//-----------------------------------------------------------
// цели
//-----------------------------------------------------------

void RadarStationModel::writeVrliCmd(const QString& name, const QString& param)
{
    QString cmd = ptlst::BEGIN + ptlst::SEPARATOR;
    cmd += name;
    if((param != "") && (param != " "))
        cmd += " " + param;
    cmd += ptlst::SEPARATOR + ptlst::END + ptlst::STOP;

    QByteArray* ba = new QByteArray(cmd.toLatin1());
    ba->append(char(0));

    emit writeVrliCtrlPacket(ba);
}

void RadarStationModel::updateVrliRaw(QByteArray* ba)
{
    vrliRaw = *ba;
    delete ba;
    parseVrliUdpDatagramm(vrliRaw);

    emit vrliStateChanged();
}

//-----------------------------------------------------------
// ПРЛИ
//-----------------------------------------------------------

void RadarStationModel::updatePrliRaw(QByteArray* ba)
{
    prliRaw = *ba;
    delete ba;
    parsePrliUdpDatagramm(prliRaw);

    emit prliStateChanged();
}

//-----------------------------------------------------------
// геттеры/сеттеры параметры соединения с РЛС
//-----------------------------------------------------------

void RadarStationModel::setHost(const QString& host)
{
    ctrlHost = host;
}

QString RadarStationModel::getHost() const
{
    return ctrlHost;
}

void RadarStationModel::getRlmConnectionParams(quint16& statePort, quint16& ctrlPort) const
{
    statePort = rlmStatePort;
    ctrlPort = rlmCtrlPort;
}
void RadarStationModel::setRlmConnectionParams(quint16 statePort, quint16 ctrlPort)
{
    rlmStatePort = statePort;
    rlmCtrlPort = ctrlPort;
}

void RadarStationModel::getRpConnectionParams(quint16& statePort, quint16& ctrlPort) const
{
    statePort = rpStatePort;
    ctrlPort = rpCtrlPort;
}
void RadarStationModel::setRpConnectionParams(quint16 statePort, quint16 ctrlPort)
{
    rpStatePort = statePort;
    rpCtrlPort = ctrlPort;
}

void RadarStationModel::getVrliConnectionParams(quint16& statePort, quint16& ctrlPort) const
{
    statePort = vrliStatePort;
    ctrlPort = vrliCtrlPort;
}
void RadarStationModel::setVrliConnectionParams(quint16 statePort, quint16 ctrlPort)
{
    vrliStatePort = statePort;
    vrliCtrlPort = ctrlPort;
}

void RadarStationModel::getPrliConnectionParams(quint16& statePort) const
{
    statePort = prliStatePort;
}
void RadarStationModel::setPrliConnectionParams(quint16 statePort)
{
    prliStatePort = statePort;
}

void RadarStationModel::getNmeaConnectionParams(quint16& ctrlPort) const
{
    ctrlPort = nmeaCtrlPort;
}
void RadarStationModel::setNmeaConnectionParams(quint16 ctrlPort)
{
    nmeaCtrlPort = ctrlPort;
}

//-----------------------------------------------------------
// парсеры датаграмм протоколов
//-----------------------------------------------------------

void RadarStationModel::parseRlmUdpDatagramm(const QByteArray& datagram)
{
    QList<QString> cmdLst; // список команд

    // распарсить протокол на команды
    int from(0);
    int prev(0);
    while((from = datagram.indexOf('\n', prev)) != -1)
    {
        cmdLst.push_back(datagram.mid(prev, from - prev));
        prev = from + 1;
    }

    // распарсить команды
    foreach (QString cmd, cmdLst)
    {
        // разбить на имя команды и параметр:
        int space = cmd.indexOf(' ');
        QString cmdName = cmd.mid(0, space);
        QString cmdParam = cmd.mid(space + 1);

        updateRlmCmd(cmdName, cmdParam);
    }
}

void RadarStationModel::parseRpUdpDatagramm(const QByteArray& datagram)
{
    QList<QString> cmdLst; // список команд

    // распарсить протокол на команды
    int from(0);
    int prev(0);
    while((from = datagram.indexOf('\n', prev)) != -1)
    {
        cmdLst.push_back(datagram.mid(prev, from - prev));
        prev = from + 1;
    }

    // распарсить команды
    foreach (QString cmd, cmdLst)
    {
        // разбить на имя команды и параметр:
        int space = cmd.indexOf(' ');
        QString cmdName = cmd.mid(0, space);
        QString cmdParam = cmd.mid(space + 1);

        updateRpCmd(cmdName, cmdParam);
    }
}

void RadarStationModel::parseVrliUdpDatagramm(const QByteArray& datagram)
{

}

void RadarStationModel::parsePrliUdpDatagramm(const QByteArray& datagram)
{

}

//-----------------------------------------------------------
// вернуть последнюю датаграмму
//-----------------------------------------------------------

QByteArray RadarStationModel::getRlmRaw()
{
    return rlmRaw;
}

QByteArray RadarStationModel::getRpRaw()
{
    return rpRaw;
}

QByteArray RadarStationModel::getVrliRaw()
{
    return vrliRaw;
}

QByteArray RadarStationModel::getPrliRaw()
{
    return prliRaw;
}
