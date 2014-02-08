/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#include "basicframe.h"
#include <QtEndian>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>

namespace http2 {

BasicFrame::BasicFrame() :
    type_(FRAME_DATA),
    flags_(0),
    identifier_(0),
    payload_()
{
}

QByteArray BasicFrame::dump() const
{
    QByteArray header(8, 0);
    uchar* headerPtr = reinterpret_cast<uchar*>(header.data());
    qToBigEndian(length(), headerPtr);
    headerPtr += sizeof(quint16);
    *headerPtr = static_cast<uchar>(type_);
    headerPtr += sizeof(uchar);
    *headerPtr = static_cast<uchar>(flags_);
    headerPtr += sizeof(uchar);
    qToBigEndian(identifier_, headerPtr);
    return header + payload_;
}

quint16 BasicFrame::length() const
{
    return payload_.size();
}

FrameType BasicFrame::type() const
{
    return type_;
}

void BasicFrame::setType(FrameType type)
{
    type_ = type;
}

uchar BasicFrame::flags() const
{
    return flags_;
}

void BasicFrame::setFlags(uchar flags)
{
    flags_ = flags;
}

quint32 BasicFrame::identifier() const
{
    return identifier_;
}

void BasicFrame::setIdentifier(quint32 identifier)
{
    identifier_ = identifier;
}

QByteArray BasicFrame::payload() const
{
    return payload_;
}

void BasicFrame::setPayload(const QByteArray &payload)
{
    payload_ = payload;
}

}
