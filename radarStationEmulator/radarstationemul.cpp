// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
// ======================================================================

#include "radarstationemul.h"
#include <QByteArray>
#include <QFile>
#include <qdatastream.h>
#include <QtEndian>
#include <QtCore/QThread>

#include "./../radarStationPrototype/rscontroller/rsprotocols/radarstationprotocols.h"

RadarStationEmul::RadarStationEmul(QObject *parent) : QObject(parent)
{

}

RadarStationEmul::~RadarStationEmul()
{

}

// Протокол передачи состояния режимов обработки данных от РЛС
QByteArray* RadarStationEmul::buildModeDataStateProtocolDatagram()
{
    QByteArray* data = new QByteArray();
    data->append("BEGIN\n");

    data->append("RAIN 50\n"); // p1 – целое, значение в диапазоне 0 - 100
    data->append("LFFILTERVALUE 5\n"); // p1 – целое, значение в диапазоне 0 – 255. 0 – фильтр выключен, 1 – 254 – параметр фильтрации
    data->append("SYNCINTERFERENCEMODE 1\n"); // p1 – целое, принимает значения 0 – выкл и 1 – вкл
    data->append("COG 90\n"); // p1 – число с плавающей точкой. Значение курса ГЛОНАСС в градусах. Принимает значения из диапазона (0.-360.)
    data->append("SOG 10.8\n"); // p1 – число с плавающей точкой. Значение скорости относительно земли от ГЛОНАСС в метрах в секунду
    data->append("RADARLATINGRAD -30.8\n"); // p1 – число с плавающей точкой. Широта центра развертки в градусах. Принимает для северной широты положительное значение, для южной – отрицательное.
    data->append("RADARLONINGRAD 45.7\n"); // p1 – число с плавающей точкой. Долгота центра развертки в градусах. Принимает для восточной долготы положительное значение, для западной – отрицательное.
    data->append("HDG -180.5\n"); // p1 – число с плавающей точкой. Значение путевого угла гирокомпаса в градусах. Принимает значения из диапазона (0.-360.)
    data->append("SPD 10.8\n"); // p1 – число с плавающей точкой. Значение скорости от лага в метрах в секунду
    data->append("GAIN 105\n"); // p1 – целое число от 0 до 255
    data->append("SEA 15\n"); // p1 – целое число от 0 до 255
    data->append("SEA_DIST 8\n"); // p1 – целое число от 0 до 255
    data->append("DEVICECONDITION 0\n"); // p1 – целое число
                                         // бит 0 - тревога "Отсутствует видео сигнал"
                                         // бит 1 - тревога "Отсутствует  синхро сигнал"
                                         // бит 2 - тревога "Отсутствует сигнал ТИН "
                                         // бит 3 - тревога "Отсутствует сигнал ИНА "

    data->append("END\n\n");
    data->append(char(0));

    return data;
}

// Протокол передачи состояния РЛС
QByteArray* RadarStationEmul::buildModeStateProtocolDatagram()
{
    QByteArray* data = new QByteArray();
    data->append("BEGIN\n");

    data->append("POWER ON\n"); // s1 – строка принимающая значения: "ON" - включено, "OFF" - выключено
    data->append("ANTENA ON\n"); // s1 – строка принимающая значения: "ON" - включено, "OFF" - выключено
    data->append("WORK ON\n"); // s1 – строка принимающая значения: "ON" - включено, "OFF" - выключено
    data->append("PULSE SHORT\n"); // s1 – строка принимающая значения: "SHORT" - короткий, "MIDDLE" - средний, "LONG" - длинный
    data->append("GAIN 7\n"); // p1 – целое число от 0 до 255
    data->append("SEA_AMPL 55\n"); // p1 – целое число от 0 до 255
    data->append("SEA_DIST 134\n"); // p1 – целое число от 0 до 255
    data->append("FAIL_SINCHRONIZATOR ON\n"); // s1 – строка принимающая значения: "ON" - включено, "OFF" - выключено
    data->append("FAIL_ANT ON\n"); // s1 – строка принимающая значения: "ON" - включено, "OFF" - выключено
    data->append("FAIL_TRANSMITTER ON\n"); // s1 – строка принимающая значения: "ON" - включено, "OFF" - выключено
    data->append("FAIL_RECIEVER ON\n"); // s1 – строка принимающая значения: "ON" - включено, "OFF" - выключено
    data->append("FAIL_RLMSOURCE1 ON\n"); // s1 – строка принимающая значения: "ON" - включено, "OFF" - выключено
    data->append("FAIL_RLMSOURCE2 ON\n"); // s1 – строка принимающая значения: "ON" - включено, "OFF" - выключено
    data->append("FAIL_RLMTRANSMITTERSOURCE ON\n"); // s1 – строка принимающая значения: "ON" - включено, "OFF" - выключено
    data->append("FAIL_RLMOVERHEATING ON\n"); // s1 – строка принимающая значения: "ON" - включено, "OFF" - выключено
    data->append("FAIL_RLMTIN ON\n"); // s1 – строка принимающая значения: "ON" - включено, "OFF" - выключено

    data->append("END\n\n");
    data->append(char(0));

    return data;
}

// Протокол передачи целей
QByteArray* RadarStationEmul::buildTargetsProtocolDatagram()
{
    TargetMsgFormat header;
    header.header = '$';

    header.msgType = 2;
    memcpy(header.timeStamp, "210523", 6);
    header.unused1 = 0;
    header.unused2 = 0;

    TargetForm target;
    target.course = 2906;
    target.latitude = 36012801;
    target.longitude = 17669836;
    target.number = 14;
    target.reserve1 = 0;
    target.reserve2 = 0;
    target.speed = 2;
    target.status = 2;

    TargetMsgCrcFormat crc;

    // упаковать в датаграмму:
    QByteArray* datagram = new QByteArray();
    QDataStream stream(datagram, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    header.sz  = 20 + 23; // todo: правильный размер пакета???
    stream << header;
    stream << target;

    crc.checkSum1 = FletchSum((const quint8*)datagram->constData(), datagram->size());
    stream << crc.checkSum1;

    crc.checkSum2 = FletchSum((const quint8*)datagram->constData(), datagram->size());
    stream << crc.checkSum2;

    return datagram;
}

// ПРЛИ
QByteArray* RadarStationEmul::buildPRIProtocolDatagram()
{
    PrimaryRadarInformationHeader header;
    header.header = '$';
    header.longitude = 3600;
    header.latitude = -3600;
    header.reserve = 0;

    // ПРЛИ передаётся по одной или несколько линеек в посылке, так,
    // чтобы суммарный объём данных не превышал 1460 байт

    PrimaryRadarInformationData data;
    data.number = 0x80000000|0;
    data.range = 10;
    data.t1 = 100;
    //data.t2 = 200;

    QByteArray in, out;
    Algorithm2PolyBitLine::generatePRID(in, 1000);
    Algorithm2PolyBitLine::compression(in, out);
    szIn = in.size();
    szOut = out.size();

    data.sz = 14 + out.size();

    // упаковать в датаграмму:
    QByteArray* datagram = new QByteArray();
    QDataStream stream(datagram, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << header;
    stream << data;
    datagram->append(out);

    return datagram;

    // РЛС выполняет 20 оборотов в минуту и на каждом обороте выдает 4095 линеек,
    // т.е. 4095*20/60 = 1365 линеек в секунду.
}

void RadarStationEmul::parsePriFile(const QString& path)
{
    QFile file(path);
    if(!file.open(QIODevice::QIODevice::ReadOnly))
            return;

    QByteArray blob = file.readAll();
    QDataStream stream(blob);
    stream.setVersion(QDataStream::Qt_5_4);
    stream.setByteOrder(QDataStream::LittleEndian);

    parsePri(&stream);
}

void RadarStationEmul::parsePri(QDataStream* stream)
{
    PrimaryRadarInformationHeader header;
    PrimaryRadarInformationData data;
    QByteArray in;

    while(!stream->atEnd())
    {
        (*stream) >> header;
        (*stream) >> data;

        in.resize(data.sz - 14);
        stream->readRawData(in.data(), data.sz - 14);

        QByteArray out;
        Algorithm2PolyBitLine::decompression(in, out);

        emit logMessage(
            QString("h = %1, lon = %2, lat = %3 : "
                    "sz = %4 , num = %5 , range = %6 , t1 = %7, "
                    "out.sz = %8")
                    .arg(header.header)
                    .arg(header.longitude)
                    .arg(header.latitude)
                    .arg(data.sz - 14)
                    .arg(data.number)
                    .arg(data.range)
                    .arg(data.t1)
                    .arg(out.size()));

        QThread::msleep(10);

    }
}

void RadarStationEmul::parseTargetsFile(const QString& path)
{
    QFile file(path);
    if(!file.open(QIODevice::QIODevice::ReadOnly))
            return;

    QByteArray blob = file.readAll();
    QDataStream stream(blob);
    stream.setVersion(QDataStream::Qt_5_4);
    stream.setByteOrder(QDataStream::LittleEndian);

    parseTargets(&stream);
}

void RadarStationEmul::parseTargets(QDataStream* stream)
{
    TargetMsgFormat header;
    TargetForm target;
    TargetMsgCrcFormat crc;

    while(!stream->atEnd())
    {
        (*stream) >> header;
        // прочитать цели:
        int targetsCount((header.sz - 20)/23); // 20б - размер хедера, 23б - 1 цель.
        for(int i = 0; i < targetsCount; ++i)
        {
            (*stream) >> target;

            targetLogMessage(header, target);
            QThread::msleep(5);
        }

        (*stream) >> crc.checkSum1;
        (*stream) >> crc.checkSum2;

        // проверить crc
        QByteArray datagram;
        QDataStream stream2(&datagram, QIODevice::WriteOnly);
        stream2 << header;
        stream2 << target;

        quint8 checkSum1 = FletchSum((const quint8*)datagram.constData(), datagram.size());
        stream2 << checkSum1;
        quint8 checkSum2 = FletchSum((const quint8*)datagram.constData(), datagram.size());
    }
    int i = 0;
}

void RadarStationEmul::targetLogMessage(const TargetMsgFormat& header,
                                        const TargetForm& target)
{
    char timeStamp[7];
    memcpy(timeStamp, header.timeStamp, 6);
    timeStamp[6] = 0;

    emit logMessage(
        QString("h = %1, sz = %2, time = %3 : "
                "num = %4 , sp = %5 , st = %6 , c = %7, "
                "")
                .arg(header.header)
                .arg(header.sz)
                .arg(timeStamp)
                .arg(target.number)
                .arg(target.speed)
                .arg(target.status)
                .arg(target.course));

}

void RadarStationEmul::priLogMessage()
{

}
