/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#pragma once
#include "common.h"
#include <QByteArray>
#include <QString>
#include <QMap>
#include <QVector>
#include <QSharedPointer>

namespace http2 {

class HPACK
{
public:
    HPACK(ConnectionType type);
    QByteArray pack(const QMap<QString, QString>& map);
    void unpack(const QByteArray& payload, QMap<QString, QString>* map);

    int headerTableSize() const;
    void setHeaderTableSize(int headerTableSize);

private:
    QString unpackLiteral(bool huffman, uchar length, const uchar* ptr);
    void evictHeaderTableEntries();
    int decodeInteger(int length, uchar first, const uchar** ptr);
    QByteArray encodeInteger(int length, int num);
    QByteArray packLiteral(const QString& str);

    typedef QMap<QString, QPair<QString, int>> HeaderTableMap;
    static const HeaderTableMap& getHeaderTableMap();

private:
    ConnectionType type_;
    QVector<QPair<QString, QString>> headerTable_;
    QMap<QString, QString> referenceSet_;
    int headerTableSize_;

};

}
