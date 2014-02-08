/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#pragma once
#include <QString>
#include <QByteArray>
#include <QPair>
#include <QMap>

namespace http2 {

class Huffman
{
public:
    Huffman(const QList<QPair<quint32, uchar>>& codelist);
    ~Huffman();
    QByteArray encode(const QByteArray& data) const;
    QByteArray decode(const QByteArray& data) const;
    int getEncodedLength(const QByteArray& data) const;

public:
    static const Huffman Requst;
    static const Huffman Response;

private:
    struct Node {
        Node();
        Node *t;
        Node *f;
    };

    Node *root_;
    QMap<int, QPair<quint32, uchar>> map_;
};

}
