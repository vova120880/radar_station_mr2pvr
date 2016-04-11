// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
// ======================================================================

#ifndef RADARSTATIONEMUL_H
#define RADARSTATIONEMUL_H

#include <QObject>
#include "./../radarStationPrototype/rscontroller/rsprotocols/radarstationprotocols.h"

QT_BEGIN_NAMESPACE
class QByteArray;
class QDataStream;
QT_END_NAMESPACE

class RadarStationEmul : public QObject
{
    Q_OBJECT
public:
    explicit RadarStationEmul(QObject *parent = 0);
    ~RadarStationEmul();

    //--------------------------------------------
    // сформировать датаграммы протоколов:
    //--------------------------------------------
    // Протокол передачи состояния режимов обработки данных от РЛС
    QByteArray* buildModeDataStateProtocolDatagram();
    // Протокол передачи состояния РЛС
    QByteArray* buildModeStateProtocolDatagram();
    // Протокол передачи целей
    QByteArray* buildTargetsProtocolDatagram();
    // ПРЛИ
    QByteArray* buildPRIProtocolDatagram();

    void parsePri(QDataStream* stream);
    void parseTargets(QDataStream* stream);

signals:
    void logMessage(const QString& str);

public slots:
    void parsePriFile(const QString& path);
    void parseTargetsFile(const QString& path);

protected:

    // функция вычисления суммы Флетчера:
    quint8 FletchSum(const quint8 *buf, size_t len)
    {
        quint8 S = 0;
        for(; len > 0; len--)
        {
            quint8 R = *buf++;
            S += R;
            if(S < R)
                ;//S++;
        }
        /*if(S = 255) S = 0;*/
        return S;
    }

    void targetLogMessage(const TargetMsgFormat& header,
                          const TargetForm& target);
    void priLogMessage();

    // test only
public:
    quint16 szIn;
    quint16 szOut;

};

#endif // RADARSTATIONEMUL_H
