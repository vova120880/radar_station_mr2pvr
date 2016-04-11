// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
// ======================================================================

#include "algorithm2polybitline.h"
#include <cstdlib>

Algorithm2PolyBitLine::Algorithm2PolyBitLine()
{

}

Algorithm2PolyBitLine::~Algorithm2PolyBitLine()
{

}

// ПРЛИ
//
//------------------------------------------------------
// Алгоритм сжатия 2-битной линейки ПРЛИ
// В линейке ПРЛИ выделяются байты 0x00, 0x55, 0xAA, 0xFF. Все остальные байты
// записываются без изменения. Если байты 0x00, 0x55, 0xAA, 0xFF встречаются
// несколько раз подряд, вместо них записывается значение этого байта и количество
// его повторений. Если же эти (0x00, 0x55, 0xAA, 0xFF) байты встречаются 1 раз,
// то они изменяются следующим образом: 0x00->0x01, 0x55->0x54, 0xAA->0xA9, 0xFF->0xFE.
//------------------------------------------------------
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

        const quint8 curB(in[i]);

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
        const quint8& b(in[i]);
        switch(b)
        {
            case 0x00:
            case 0x55:
            case 0xAA:
            case 0xFF:
                ++i;
                for(int j = 0; j < in[i]; ++j)
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
    for(size_t i = 0; i < sz; ++i)
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
