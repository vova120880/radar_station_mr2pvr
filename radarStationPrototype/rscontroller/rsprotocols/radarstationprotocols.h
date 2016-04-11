// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
// ======================================================================

#ifndef RADARSTATIONPROTOCOLS
#define RADARSTATIONPROTOCOLS

#include <QtGlobal>
#include <QString>
#include <QByteArray>
#include <qdatastream.h>

//------------------------------------------------------
// описание протоколов взаимодействия с радарным вычислителем РЛС МР-2ПВ-Р.
//------------------------------------------------------

struct ProtocolTagsKeyWordList
{
    static const QString BEGIN;
    static const QString END;
    static const QString SEPARATOR;
    static const QString STOP;
};

typedef ProtocolTagsKeyWordList ptlst;

//------------------------------------------------------
// Протокол управления режимами обработки данных от РЛС
// Общий вид посылки управления, принимаемой Вычислителем: BEGIN\nДАННЫЕ\nEND\n\n\0
//------------------------------------------------------
// Варианты поля ДАННЫЕ
//------------------------------------------------------
struct RpCtrlProtocolKeyWordList
{
    static const QString RAIN; // Дождь
    static const QString LFFILTERVALUE; // Асинхронный фильтр
    static const QString SYNCINTERFERENCEMODE; // Шумовой фильтр
    static const QString SCALE; // Переключение шкалы дальности
    static const QString GAIN; // Уровень обнаружения
    static const QString SEA; // Волны-А
    static const QString SEA_DIST; // Волны-Д
};

typedef RpCtrlProtocolKeyWordList RpCtrlLst;

//------------------------------------------------------
// Протокол передачи состояния режимов обработки данных от РЛС
// посылка следующего вида: BEGIN\nДАННЫЕ1\n…\nДАННЫЕN\nEND\n\n\0
//------------------------------------------------------
// Варианты поля ДАННЫЕ
//------------------------------------------------------
struct RpStateProtocolKeyWordList
{
    static const QString RAIN; // Дождь
    static const QString LFFILTERVALUE; // Асинхронный фильтр
    static const QString SYNCINTERFERENCEMODE; // Шумовой фильтр
    static const QString COG; // Курс относительно земли (данные ГЛОНАСС)
    static const QString SOG; // Скорость относительно земли (данные ГЛОНАСС)
    static const QString RADARLATINGRAD; // Широта
    static const QString RADARLONINGRAD; // Долгота
    static const QString HDG; // Путевой угол (данные гирокомпаса)
    static const QString SPD; // Скорость относительно воды (данные лага)
    static const QString GAIN; // Уровень обнаружения
    static const QString SEA; // Волны-А
    static const QString SEA_DIST; // Волны-Д
    static const QString DEVICECONDITION; // Состояние РП
};

typedef RpStateProtocolKeyWordList RpStateLst;

//------------------------------------------------------
// Протокол управления режимами работы РЛС
// Общий вид посылки управления, принимаемой Вычислителем: BEGIN\nДАННЫЕ\nEND\n\n\0
//------------------------------------------------------
// Варианты поля ДАННЫЕ
//------------------------------------------------------
struct RlmCtrlProtocolKeyWordList
{
    static const QString POWER; // Питание РЛМ
    static const QString ANTENA; // Вращение антенны
    static const QString WORK; // Излучение
    static const QString PULSE; // Длительность импульса
    static const QString GAIN; // Усиление приемника
    static const QString SEA_AMPL; // ВАРУ-А
    static const QString SEA_DIST; // ВАРУ-Д
};

typedef RlmCtrlProtocolKeyWordList RlmCtrlLst;

//------------------------------------------------------
// Протокол передачи состояния РЛС
// посылка следующего вида: BEGIN\nДАННЫЕ1\n…\nДАННЫЕN\nEND\n\n\0
//------------------------------------------------------
// Варианты поля ДАННЫЕ
//------------------------------------------------------
struct RlmStateProtocolKeyWordList
{
    static const QString POWER; // Питание РЛМ
    static const QString ANTENA; // Вращение антенны
    static const QString WORK; // Излучение
    static const QString PULSE; // Длительность импульса
    static const QString GAIN; // Усиление приемника
    static const QString SEA_AMPL; // ВАРУ-А
    static const QString SEA_DIST; // ВАРУ-Д
    static const QString FAIL_SINCHRONIZATOR; // Неисправность синхронизатора
    static const QString FAIL_ANT; // Неисправность антенны
    static const QString FAIL_TRANSMITTER; // Неисправность передатчика
    static const QString FAIL_RECIEVER; // Неисправность приемника
    static const QString FAIL_RLMSOURCE1; // Неисправность источника питания 1
    static const QString FAIL_RLMSOURCE2; // Неисправность источника питания 2
    static const QString FAIL_RLMTRANSMITTERSOURCE; // Неисправность источника питания передатчика
    static const QString FAIL_RLMOVERHEATING; // Перегрев модуля
    static const QString FAIL_RLMTIN; // Отсутствует сигнал ТИН
};

typedef RlmStateProtocolKeyWordList RlmStateLst;

//------------------------------------------------------
// Протокол управления захватом и сбросом радиолокационных целей
//------------------------------------------------------
// Общий вид посылки управления, принимаемой Вычислителем: BEGIN\nДАННЫЕ\nEND\n\n\0
//------------------------------------------------------
struct  VrliCtrlProtocolKeyWordList
{
    static const QString TGT_MANUAL_ALL; // Захват цели на ручное сопровождение
    static const QString TGT_RESET; // Сброс цели с сопровождения
    static const QString TGT_RESET_LOST; // Сброс всех потерянных целей
    static const QString AUTO_CAPTURE_ON; // Включение автоматического захвата целей
    static const QString AUTO_CAPTURE_OFF; // Выключение автоматического захвата целей
};

typedef VrliCtrlProtocolKeyWordList VrliCtrlLst;

//------------------------------------------------------
// Протокол передачи целей
//------------------------------------------------------
//
//------------------------------------------------------

// формат посылки
struct TargetMsgFormat
{
    quint8 header; // всегда $
    quint16 sz; // размер посылки в байтах (размер поля данных + 20)
    quint32 unused1; // Не используется
    quint32 unused2; // Не используется
    quint8 timeStamp[6]; // Время GMT в виде ччммсс
    quint8 msgType; // тип посылки
};
    //
    // поле данных (переменное)???

struct TargetMsgCrcFormat
{
    quint8 checkSum1; // контрольная сумма – сумма всех байт с переполнением,
                      // начиная с $, включая поле данных
    quint8 checkSum2; // вторая контрольная сумма – сумма всех байт с переполнением,
                      // начиная с $, включая поле данных и первую контрольную сумму

};

// Формуляр цели РЛС
struct TargetForm
{
    quint16 number; // номер цели
    qint32 longitude; // долгота цели в 1/10000 минуты
                      // (180 град., Е - положительное значение, W - отрицательное)
    qint32 latitude;  // широта цели в 1/10000 минуты
                      // ( 90, N- положительное значение, S - отрицательное)
    quint16 course; // курс цели в десятых градуса (0-3599)
    quint16 speed; // скорость цели в десятых узла
    quint8 status; // Статус цели: 0 – новая (курс и скорость не расчитаны)
                   // 2 – сопровождается, 8 – потеряна
    quint32 reserve1; // Не используется
    quint32 reserve2; // Не используется
};

//------------------------------------------------------
// Формат посылки 2-битной первичной радиолокационной информации (ПРЛИ),
// передаваемой по UDP-протоколу
//------------------------------------------------------
// ПРЛИ передаётся по одной или несколько линеек в посылке, так,
// чтобы суммарный объём данных не превышал 1460 байт.
//------------------------------------------------------
// Формат заголовка посылки:
//------------------------------------------------------
struct PrimaryRadarInformationHeader
{
    quint8 header; // всегда $
    qint32 longitude; // Географическая широта места установки радара в 1/10000 минуты
                      // («+» - СШ, «–» - ЮШ)
    qint32 latitude;  // Географическая долгота места установки радара в 1/10000 минуты
                      // («+» - ВД, «–» - ЗД)
    quint32 reserve; // резерв
};

// Формат линейки:
struct PrimaryRadarInformationData
{
    quint32 sz; // размер линейки с заголовком в байтах (длина поля ПРЛИ* + 14)
    quint32 number; // Hомер линейки (0-4095).
              // Для первой линейки (0-й) в обороте – СЕВЕР, первый бит в этом поле
              // устанавливается в единицу (операция 0x80000000|num, побитовое ИЛИ).

    quint16 range; // Цена одного дискрета дальности в миллиметрах.

    quint32 t1;
    //quint32 t2; // время прихода линейки (укладывает РП)

    //quint8* data;

};

//------------------------------------------------------
// Алгоритм сжатия 2-битной линейки ПРЛИ
// В линейке ПРЛИ выделяются байты 0x00, 0x55, 0xAA, 0xFF. Все остальные байты
// записываются без изменения. Если байты 0x00, 0x55, 0xAA, 0xFF встречаются
// несколько раз подряд, вместо них записывается значение этого байта и количество
// его повторений. Если же эти (0x00, 0x55, 0xAA, 0xFF) байты встречаются 1 раз,
// то они изменяются следующим образом: 0x00->0x01, 0x55->0x54, 0xAA->0xA9, 0xFF->0xFE.
//------------------------------------------------------

class Algorithm2PolyBitLine
{
public:
    Algorithm2PolyBitLine();
    ~Algorithm2PolyBitLine();

    static void compression(const QByteArray& in, QByteArray& out);
    static void decompression(const QByteArray& in, QByteArray& out);
    static void generatePRID(QByteArray& in, size_t sz);

    // функция вычисления суммы Флетчера:
    static quint8 FletchSum(const quint8 *buf, size_t len)
    {
        quint8 S = 0;
        for(; len > 0; len--)
        {
            quint8 R = *buf++;
            S += R;
            //if(S < R)
            //    ;S++;
        }
        /*if(S = 255) S = 0;*/
        return S;
    }
};

//-------------------------------------------------------
// сериализация / десериализация
//-------------------------------------------------------
QDataStream &operator>>(QDataStream &in, TargetMsgFormat &info);
QDataStream &operator<<(QDataStream &out, TargetMsgFormat &info);
QDataStream &operator>>(QDataStream &in, TargetForm &info);
QDataStream &operator<<(QDataStream &out, TargetForm &info);

QDataStream &operator>>(QDataStream &in, PrimaryRadarInformationHeader &info);
QDataStream &operator<<(QDataStream &out, PrimaryRadarInformationHeader &info);
QDataStream &operator>>(QDataStream &in, PrimaryRadarInformationData &info);
QDataStream &operator<<(QDataStream &out, PrimaryRadarInformationData &info);


#endif // RADARSTATIONPROTOCOLS

