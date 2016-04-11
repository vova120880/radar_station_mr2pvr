// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
// ======================================================================

#include "radarstationprotocols.h"


const QString ptlst::BEGIN = "BEGIN";
const QString ptlst::END = "END";
const QString ptlst::SEPARATOR = "\n";
const QString ptlst::STOP = "\n\n";

//------------------------------------------------------
//
//------------------------------------------------------

const QString RpCtrlLst::RAIN = "RAIN";
const QString RpCtrlLst::LFFILTERVALUE = "LFFILTERVALUE";
const QString RpCtrlLst::SYNCINTERFERENCEMODE = "SYNCINTERFERENCEMODE";
const QString RpCtrlLst::SCALE = "SCALE";
const QString RpCtrlLst::GAIN = "GAIN";
const QString RpCtrlLst::SEA = "SEA";
const QString RpCtrlLst::SEA_DIST = "SEA_DIST";

//------------------------------------------------------
//
//------------------------------------------------------

const QString RpStateLst::RAIN = "RAIN";
const QString RpStateLst::LFFILTERVALUE = "LFFILTERVALUE";
const QString RpStateLst::SYNCINTERFERENCEMODE = "SYNCINTERFERENCEMODE";
const QString RpStateLst::COG = "COG";
const QString RpStateLst::SOG = "SOG";
const QString RpStateLst::RADARLATINGRAD = "RADARLATINGRAD";
const QString RpStateLst::RADARLONINGRAD = "RADARLONINGRAD";
const QString RpStateLst::HDG = "HDG";
const QString RpStateLst::SPD = "SPD";
const QString RpStateLst::GAIN = "GAIN";
const QString RpStateLst::SEA = "SEA";
const QString RpStateLst::SEA_DIST = "SEA_DIST";
const QString RpStateLst::DEVICECONDITION = "DEVICECONDITION";

//------------------------------------------------------
//
//------------------------------------------------------

const QString RlmCtrlLst::POWER = "POWER";
const QString RlmCtrlLst::ANTENA = "ANTENA";
const QString RlmCtrlLst::WORK = "WORK";
const QString RlmCtrlLst::PULSE = "PULSE";
const QString RlmCtrlLst::GAIN = "GAIN";
const QString RlmCtrlLst::SEA_AMPL = "SEA_AMPL";
const QString RlmCtrlLst::SEA_DIST = "SEA_DIST";

//------------------------------------------------------
//
//------------------------------------------------------

const QString RlmStateLst::POWER = "POWER";
const QString RlmStateLst::ANTENA = "ANTENA";
const QString RlmStateLst::WORK = "WORK";
const QString RlmStateLst::PULSE = "PULSE";
const QString RlmStateLst::GAIN = "GAIN";
const QString RlmStateLst::SEA_AMPL = "SEA_AMPL";
const QString RlmStateLst::SEA_DIST = "SEA_DIST";
const QString RlmStateLst::FAIL_SINCHRONIZATOR = "FAIL_SINCHRONIZATOR";
const QString RlmStateLst::FAIL_ANT = "FAIL_ANT";
const QString RlmStateLst::FAIL_TRANSMITTER = "FAIL_TRANSMITTER";
const QString RlmStateLst::FAIL_RECIEVER = "FAIL_RECIEVER";
const QString RlmStateLst::FAIL_RLMSOURCE1 = "FAIL_RLMSOURCE1";
const QString RlmStateLst::FAIL_RLMSOURCE2 = "FAIL_RLMSOURCE2";
const QString RlmStateLst::FAIL_RLMTRANSMITTERSOURCE = "FAIL_RLMTRANSMITTERSOURCE";
const QString RlmStateLst::FAIL_RLMOVERHEATING = "FAIL_RLMOVERHEATING";
const QString RlmStateLst::FAIL_RLMTIN = "FAIL_RLMTIN";

//------------------------------------------------------
//
//------------------------------------------------------

const QString VrliCtrlLst::TGT_MANUAL_ALL = "TGT_MANUAL";//_ALL";
const QString VrliCtrlLst::TGT_RESET = "TGT_RESET";
const QString VrliCtrlLst::TGT_RESET_LOST = "TGT_RESET_LOST";
const QString VrliCtrlLst::AUTO_CAPTURE_ON = "AUTO_CAPTURE_ON";
const QString VrliCtrlLst::AUTO_CAPTURE_OFF = "AUTO_CAPTURE_OFF";

//------------------------------------------------------
// Алгоритм сжатия 2-битной линейки ПРЛИ
//------------------------------------------------------

Algorithm2PolyBitLine::Algorithm2PolyBitLine()
{

}

Algorithm2PolyBitLine::~Algorithm2PolyBitLine()
{

}

void Algorithm2PolyBitLine::compression(const QByteArray& in, QByteArray& out)
{
    out.reserve(in.size());
    quint16 count0x00(0), count0x55(0), count0xAA(0), count0xFF(0);

    const size_t sz(in.size());
    quint8 prevB(in[0]);
    for(size_t i = 1; i < sz; ++i)
    {
        switch(prevB)
        {
            case 0x00: ++count0x00; break;
            case 0x55: ++count0x55; break;
            case 0xAA: ++count0xAA; break;
            case 0xFF: ++count0xFF; break;
        }

        const quint8 curB(in.at(i));

        if(prevB != curB) // байты не равны, надо записать
        {
            switch(prevB)
            {
                case 0x00:
                    if(count0x00) // записать неизмененный и количество
                    {
                        out.push_back(prevB); // 0x00
                        out.push_back(char(count0x00)); // количество
                        count0x00 = 0;
                    }
                    else // записать один измененный
                    {
                        out.push_back(0x01); // 0x01
                    }
                    break;
                case 0x55:
                    if(count0x55) // записать неизмененный и количество
                    {
                        out.push_back(prevB); // 0x55
                        out.push_back(char(count0x55)); // количество
                        count0x55 = 0;
                    }
                    else // записать один измененный
                    {
                        out.push_back(0x54); // 0x54
                    }
                    break;
                case 0xAA:
                    if(count0xAA) // записать неизмененный и количество
                    {
                        out.push_back(prevB); // 0xAA
                        out.push_back(char(count0xAA)); // количество
                        count0xAA = 0;
                    }
                    else // записать один измененный
                    {
                        out.push_back(0xA9); // 0xA9
                    }
                    break;
                case 0xFF:
                    if(count0xFF) // записать неизмененный и количество
                    {
                        out.push_back(prevB); // 0xFF
                        out.push_back(char(count0xFF)); // количество
                        count0xFF = 0;
                    }
                    else // записать один измененный
                    {
                        out.push_back(0xFE); // 0xFE
                    }
                    break;
                default: // байт не сжимается
                    out.push_back(prevB);
            }

            prevB = curB;
        }
        else // равны, но байт не сжимается, надо записать
        {
            if((prevB == 0x00) || (prevB == 0x55) || (prevB == 0xAA) || (prevB == 0xFF))
                ;
            else
                out.push_back(prevB);
        }
    }

    // последний байт:

    switch(prevB)
    {
        case 0x00: ++count0x00; break;
        case 0x55: ++count0x55; break;
        case 0xAA: ++count0xAA; break;
        case 0xFF: ++count0xFF; break;
    }

    switch(prevB)
    {
        case 0x00:
            if(count0x00) // записать неизмененный и количество
            {
                out.push_back(prevB); // 0x00
                out.push_back(char(count0x00)); // количество
                count0x00 = 0;
            }
            else // записать один измененный
            {
                out.push_back(0x01); // 0x01
            }
            break;
        case 0x55:
            if(count0x55) // записать неизмененный и количество
            {
                out.push_back(prevB); // 0x55
                out.push_back(char(count0x55)); // количество
                count0x55 = 0;
            }
            else // записать один измененный
            {
                out.push_back(0x54); // 0x54
            }
            break;
        case 0xAA:
            if(count0xAA) // записать неизмененный и количество
            {
                out.push_back(prevB); // 0xAA
                out.push_back(char(count0xAA)); // количество
                count0xAA = 0;
            }
            else // записать один измененный
            {
                out.push_back(0xA9); // 0xA9
            }
            break;
        case 0xFF:
            if(count0xFF) // записать неизмененный и количество
            {
                out.push_back(prevB); // 0xFF
                out.push_back(char(count0xFF)); // количество
                count0xFF = 0;
            }
            else // записать один измененный
            {
                out.push_back(0xFE); // 0xFE
            }
            break;
        default: // байт не сжимается
            out.push_back(prevB);
    }
}

// 0x00->0x01, 0x55->0x54, 0xAA->0xA9, 0xFF->0xFE
void Algorithm2PolyBitLine::decompression(const QByteArray& in, QByteArray& out)
{
    out.reserve(in.size()*5);

    const size_t sz(in.size());
    for(size_t i = 0; i < sz; ++i)
    {
        const quint8& b = in.at(i);
        switch(b)
        {
            case 0x00:
            case 0x55:
            case 0xAA:
            case 0xFF:
                ++i;
                for(int j = 0; j < (quint8)in.at(i); ++j)
                {
                    out.push_back(b);
                }
                break;
            case 0x01: out.push_back(char(0x00)); break;
            case 0x54: out.push_back(char(0x55)); break;
            case 0xA9: out.push_back(char(0xAA)); break;
            case 0xFE: out.push_back(char(0xFF)); break;
            default:
                out.push_back(b);
        }
    }
}

// ПРЛИ передаётся по одной или несколько линеек в посылке, так,
// чтобы суммарный объём данных не превышал 1460 байт.
// РЛС выполняет 20 оборотов в минуту и на каждом обороте выдает 4095 линеек,
// т.е. 4095*20/60 = 1365 линеек в секунду.

void Algorithm2PolyBitLine::generatePRID(QByteArray& in, size_t sz)
{
    in.resize(sz);
    for(int i = 0; i < sz; ++i)
    {
        in[i] = rand()%0xFF;

        // убрать 0x00->0x01, 0x55->0x54, 0xAA->0xA9, 0xFF->0xFE
        switch(quint8(in[i]))
        {
            case 0x01:
            case 0x54:
            case 0xA9:
            case 0xFE:
                in[i] = 0x00;
                break;
        }

        // повторить
        switch(quint8(in[i]))
        {
            case 0x00:
            case 0x55:
            case 0xAA:
            case 0xFF:
                int lim = rand()%10;
                for(int j = 0; j < lim; ++j)
                {
                    ++i;
                    if(i < sz)
                        in[i] = in[i - 1];
                    else
                        break;
                }
                break;
        }
    }
}

//-------------------------------------------------------
// сериализация / десериализация
//-------------------------------------------------------

QDataStream &operator>>(QDataStream &in, TargetMsgFormat &info)
{
    in >> info.header;
    in >> info.sz;
    //info.sz = qFromBigEndian(info.sz);
    in >> info.unused1;
    in >> info.unused2;

    in.readRawData((char*)info.timeStamp, sizeof(info.timeStamp));

    in >> info.msgType;
    //info.msgType = qFromBigEndian(info.msgType);

    return in;
}

QDataStream &operator<<(QDataStream &out, TargetMsgFormat &info)
{
    out << info.header;
    out << info.sz;
    out << info.unused1;
    out << info.unused2;
    out.writeRawData((char*)info.timeStamp, sizeof(info.timeStamp));
    out << info.msgType;

    return out;
}

QDataStream &operator>>(QDataStream &in, TargetForm &info)
{
    in >> info.number;
    //info.number = qFromBigEndian(info.number);
    in >> info.longitude;
    //info.longitude = qFromBigEndian(info.longitude);
    in >> info.latitude;
    //info.latitude = qFromBigEndian(info.latitude);
    in >> info.course;
    //info.course = qFromBigEndian(info.course);
    in >> info.speed;
    //info.speed = qFromBigEndian(info.speed);
    in >> info.status;
    //info.status = qFromBigEndian(info.status);
    in >> info.reserve1;
    in >> info.reserve2;

    return in;
}

QDataStream &operator<<(QDataStream &out, TargetForm &info)
{
    out << info.number;
    out << info.longitude;
    out << info.latitude;
    out << info.course;
    out << info.speed;
    out << info.status;
    out << info.reserve1;
    out << info.reserve2;

    return out;
}

QDataStream &operator>>(QDataStream &in, PrimaryRadarInformationHeader &info)
{
    in >> info.header;
    //info.header = qFromBigEndian(info.header);
    in >> info.longitude;
    //info.longitude = qFromBigEndian(info.longitude);
    in >> info.latitude;
    //info.latitude = qFromBigEndian(info.latitude);
    in >> info.reserve;
    return in;
}

QDataStream &operator<<(QDataStream &out, PrimaryRadarInformationHeader &info)
{
    out << info.header;
    out << info.longitude;
    out << info.latitude;
    out << info.reserve;

    return out;
}

QDataStream &operator>>(QDataStream &in, PrimaryRadarInformationData &info)
{
    in >> info.sz;
    //info.sz = qFromBigEndian(info.sz);
    in >> info.number;
    //info.number = qFromBigEndian(info.number);
    in >> info.range;
    //info.range = qFromBigEndian(info.range);
    in >> info.t1;
    //info.t1 = qFromBigEndian(info.t1);
    return in;
}

QDataStream &operator<<(QDataStream &out, PrimaryRadarInformationData &info)
{
    out << info.sz;
    out << info.number;
    out << info.range;
    out << info.t1;

    return out;
}
