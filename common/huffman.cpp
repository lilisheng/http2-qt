/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#include "huffman.h"
#include <QDataStream>
#include <cstdint>
#include <QDebug>

namespace http2 {

Huffman::Node::Node() :
    t(nullptr),
    f(nullptr)
{
}

Huffman::Huffman(const QList<QPair<quint32, uchar>>& codelist) :
    root_(nullptr)
{
    int cnt = 0;
    root_ = new Node[256];
    Node* cur = root_;

    for (int i = 0; i < codelist.size(); ++i) {
        const auto& pair = codelist.at(i);
        map_[i] = pair;
        uchar length = pair.second;
        quint32 mask = 1 << (length - 1);
        for (; mask != 1; mask >>= 1) {
            if (pair.first & mask) {
                if (cur->t) {
                    intptr_t ptr = reinterpret_cast<intptr_t>(cur->t);
                    cur = reinterpret_cast<Node*>(ptr - 257);
                }
                else {
                    intptr_t ptr = reinterpret_cast<intptr_t>(&root_[++cnt]);
                    cur->t = reinterpret_cast<Node*>(ptr + 257);
                    cur = reinterpret_cast<Node*>(ptr);
                }
            }
            else {
                if (cur->f) {
                    intptr_t ptr = reinterpret_cast<intptr_t>(cur->f);
                    cur = reinterpret_cast<Node*>(ptr - 257);
                }
                else {
                    intptr_t ptr = reinterpret_cast<intptr_t>(&root_[++cnt]);
                    cur->f = reinterpret_cast<Node*>(ptr + 257);
                    cur = reinterpret_cast<Node*>(ptr);
                }
            }
        }
        if (pair.first & mask) {
            cur->t = reinterpret_cast<Node*>(i);
        }
        else {
            cur->f = reinterpret_cast<Node*>(i);
        }
        cur = root_;
    }
}

Huffman::~Huffman()
{
    delete[] root_;
}

QByteArray Huffman::encode(const QByteArray& data) const
{
    QByteArray result;

    uchar octet = 0;
    uchar octetShift = 0;
    for (uchar c : data) {
        quint32 code = map_[c].first;
        uchar codeLength = map_[c].second;
        uchar finished = 0;
        while (finished < codeLength) {
            int len = qMin((8 - octetShift), static_cast<int>(codeLength - finished));
            int shift = octetShift - finished + codeLength - 8;
            uchar mask = (0xff >> octetShift) ^ (0xff >> (octetShift + len));
            if (shift > 0) {
                octet |= ((code >> shift) & mask);
            }
            else {
                octet |= ((code << (-shift)) & mask);
            }
            finished += len;
            octetShift += len;
            if (octetShift == 8) {
                result.push_back(octet);
                octet = octetShift = 0;
            }
        }
    }

    // padding
    if (octetShift != 0) {
        // EOS
        quint32 code = map_[256].first;
        uchar codeLength = map_[256].second;
        int shift = octetShift + codeLength - 8;
        octet |= (code >> shift);
        result.push_back(octet);
    }

    return result;
}

QByteArray Huffman::decode(const QByteArray& data) const
{
    QByteArray result;

    Node* cur = root_;
    for (uchar c : data) {
        for (int i = 7; i >= 0; --i) {
            uchar mask = 1 << i;
            Node *child = (c & mask) ? cur->t : cur->f;
            if (child) {
                intptr_t ptr = reinterpret_cast<intptr_t>(child);
                if (ptr < 257) {
                    result.push_back(static_cast<char>(ptr));
                    cur = root_;
                }
                else {
                    cur = reinterpret_cast<Node*>(ptr - 257);
                }
            }
            else {
                cur = root_;
            }
        }
    }
    return result;
}

int Huffman::getEncodedLength(const QByteArray& data) const
{
    int length = 0;
    for (uchar c : data) {
        length += map_[c].second;
    }
    return (length + 7) / 8;
}

#include "huffmantable.h"

const Huffman Huffman::Requst   = Huffman(requestTable);
const Huffman Huffman::Response = Huffman(responseTable);

}
