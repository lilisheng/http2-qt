/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#include "hpack.h"
#include "huffman.h"
#include <cmath>
#include <QDebug>

namespace http2 {

static const QList<QPair<QString, QString>> staticHeaderTable =
#include "headertable.h"
;

HPACK::HPACK(ConnectionType type) :
    type_(type),
    headerTableSize_(4096)
{

}

QByteArray HPACK::pack(const QMap<QString, QString>& map)
{
    QByteArray header;
    for(auto key: map.keys()) {
        const auto& value = map[key];

        const auto& headerTableMap = getHeaderTableMap();
        if (headerTableMap.contains(key)) {
            const auto& pair = headerTableMap[key];
            int index = pair.second;
            if (pair.first == value) {
                // Indexed Header Field
                QByteArray num = encodeInteger(7, index);
                num[0] = num[0] | 0x80;
                header += num;
            }
            else {
                // Literal Header Field without Indexing - Indexed Name
                QByteArray num = encodeInteger(6, index);
                num[0] = num[0] | 0x40;
                header += num;
                header += packLiteral(value);
            }
        }
        else {
            // Literal Header Field without Indexing - New Name
            header.push_back(static_cast<char>(0x40));
            header += packLiteral(key);
            header += packLiteral(value);
        }
    }
    return header;
}

void HPACK::unpack(const QByteArray& payload, QMap<QString, QString>* map)
{
    const uchar* payloadPtr = reinterpret_cast<const uchar*>(payload.data());
    const uchar* const payloadEnd = reinterpret_cast<const uchar*>(payload.data() + payload.size());

    while (payloadPtr < payloadEnd) {
        uchar prefix = *payloadPtr;
        payloadPtr += sizeof(uchar);

        if (prefix == 0x80) {
            referenceSet_.clear();
        }
        else if ((prefix & 0x3F) == 0) {
            uchar namePrefix = *payloadPtr;
            payloadPtr += sizeof(uchar);
            bool nameHuffman = namePrefix & 0x80;
            int nameLen = decodeInteger(7, namePrefix, &payloadPtr);

            QString name = unpackLiteral(nameHuffman, nameLen, payloadPtr);
            payloadPtr += nameLen;

            uchar valuePrefix = *payloadPtr;
            payloadPtr += sizeof(uchar);
            bool valueHuffman = valuePrefix & 0x80;
            int valueLen = decodeInteger(7, valuePrefix, &payloadPtr);

            QString value = unpackLiteral(valueHuffman, valueLen, payloadPtr);
            payloadPtr += valueLen;

            if (referenceSet_.contains(name) && referenceSet_[name] == value) {
                referenceSet_.remove(name);
            }
            else {
                referenceSet_[name] = value;
            }

            if (!(prefix & 0x40)) {
                // Incremental Indexing
                const QPair<QString, QString>& pair = qMakePair(name, value);
                if (!headerTable_.contains(pair)) {
                    headerTable_.push_front(pair);
                    evictHeaderTableEntries();
                }
            }
        }
        else {
            int idx = decodeInteger(6, prefix, &payloadPtr);

            QPair<QString, QString> pair;
            if (idx <= headerTable_.size()) {
                pair = headerTable_[idx - 1];
            }
            else {
                pair = staticHeaderTable[idx - headerTable_.size() - 1];
            }

            QString name = pair.first;
            QString value;
            if ((prefix & 0xC0) == 0x80) {
                value = pair.second;
            }
            else {
                uchar valuePrefix = *payloadPtr;
                payloadPtr += sizeof(uchar);
                bool valueHuffman = valuePrefix & 0x80;
                int valueLen = decodeInteger(7, valuePrefix, &payloadPtr);
                value = unpackLiteral(valueHuffman, valueLen, payloadPtr);
                payloadPtr += valueLen;
            }

            if (referenceSet_.contains(name) && referenceSet_[name] == value) {
                referenceSet_.remove(name);
            }
            else {
                referenceSet_[name] = value;
            }

            if (!(prefix & 0x40)) {
                // Incremental Indexing
                const QPair<QString, QString>& pair = qMakePair(name, value);
                if (!headerTable_.contains(pair)) {
                    headerTable_.push_front(pair);
                    evictHeaderTableEntries();
                }
            }
        }
    }

    *map = referenceSet_;
}

QString HPACK::unpackLiteral(bool huffman, uchar length, const uchar* ptr)
{
    if (huffman) {
        QByteArray ary(reinterpret_cast<const char*>(ptr), length);
        if (type_ == CONNECTION_CLIENT) {
            return Huffman::Response.decode(ary);
        }
        else {
            return Huffman::Requst.decode(ary);
        }
    }
    else {
        return QString::fromUtf8(reinterpret_cast<const char*>(ptr), length);
    }
}

void HPACK::evictHeaderTableEntries()
{
    int length = 0;
    int byte = 0;
    for (const auto& pair : headerTable_) {
        byte += pair.first.size() + pair.second.size() + 32;
        if (byte > headerTableSize_) {
            break;
        }
        else {
            length++;
        }
    }
    headerTable_.resize(length);
}

int HPACK::decodeInteger(int length, uchar first, const uchar** ptr)
{
    uchar mask = (0xFF >> (8 - length));
    int idx = first & mask;
    if (idx == mask) {
        int base = 1;
        bool cont;
        do {
            uchar len = **ptr & 0x7F;
            cont = **ptr & 0x80;
            *ptr += sizeof(uchar);
            idx += len * base;
            base *= 128;
        } while (cont);
    }
    return idx;
}

QByteArray HPACK::encodeInteger(int length, int num)
{
    QByteArray result;
    uchar prefixMax = std::pow(2, length) - 1;
    if (num < prefixMax) {
        result.push_back(num);
    }
    else {
        result.push_back(prefixMax);
        num -= prefixMax;
        while (num >= 128) {
            result.push_back(num % 128 + 128);
            num /= 128;
        }
        result.push_back(num);
    }
    return result;
}

QByteArray HPACK::packLiteral(const QString& str)
{
    QByteArray result;
    QByteArray data = str.toUtf8();
    QByteArray packed;
    const Huffman& huffman = (type_ == CONNECTION_CLIENT) ? Huffman::Requst : Huffman::Response;
    bool useHuffman = huffman.getEncodedLength(data) < data.length();
    if (useHuffman) {
        packed = huffman.encode(data);
    }
    else {
        packed = data;
    }
    result = encodeInteger(7, packed.length()) + packed;
    if (useHuffman) result[0] = result[0] | 0x80;
    return result;
}

const HPACK::HeaderTableMap& HPACK::getHeaderTableMap()
{
    static HeaderTableMap map;
    if (map.empty()) {
        for (int i = 0; i < staticHeaderTable.size(); ++i) {
            const auto& pair = staticHeaderTable[i];
            map[pair.first] = qMakePair(pair.second, i + 1);
        }
    }
    return map;
}

int HPACK::headerTableSize() const
{
    return headerTableSize_;
}

void HPACK::setHeaderTableSize(int headerTableSize)
{
    headerTableSize_ = headerTableSize;
    evictHeaderTableEntries();
}

}
