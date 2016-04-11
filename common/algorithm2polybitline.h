// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
// ======================================================================

#ifndef ALGORITHM2POLYBITLINE_H
#define ALGORITHM2POLYBITLINE_H

#include <QByteArray>

class Algorithm2PolyBitLine
{
public:
    Algorithm2PolyBitLine();
    ~Algorithm2PolyBitLine();

    static void compression(const QByteArray& in, QByteArray& out);
    static void decompression(const QByteArray& in, QByteArray& out);
    static void generatePRID(QByteArray& in, size_t sz);
};

#endif // ALGORITHM2POLYBITLINE_H
