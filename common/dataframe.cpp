/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#include "dataframe.h"
#include "streamcontext.h"
#include <QJsonObject>
#include <QJsonDocument>

namespace http2 {

enum DataFlags {
    DATA_FLAG_END_STREAM  = 0x1
};

DataFrame::DataFrame(QObject *parent) :
    QObject(parent),
    endStream_(false)
{
}

DataFrame::DataFrame(const BasicFrame& frame, QObject *parent) :
    QObject(parent)
{
    endStream_ = frame.flags() & DATA_FLAG_END_STREAM;
    payload_ = frame.payload();
}

QByteArray DataFrame::dump(StreamContext& context) const
{
    uchar flags = 0;
    if (endStream_) flags |= DATA_FLAG_END_STREAM;

    BasicFrame frame;
    frame.setType(FRAME_DATA);
    frame.setFlags(flags);
    frame.setPayload(payload_);
    frame.setIdentifier(context.identifier);
    return frame.dump();
}

QString DataFrame::inspect() const
{
    QJsonObject obj;
    obj["endStream"]  = endStream();
    obj["length"]     = payload().size();
    obj["payload"]    = QString::fromUtf8(payload());

    return "DATA " + QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Indented));
}

QByteArray DataFrame::payload() const
{
    return payload_;
}

void DataFrame::setPayload(const QByteArray &payload)
{
    payload_ = payload;
}

bool DataFrame::endStream() const
{
    return endStream_;
}

void DataFrame::setEndStream(bool endStream)
{
    endStream_ = endStream;
}

}
