/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#include "headersframe.h"
#include "hpack.h"
#include "streamcontext.h"
#include <QJsonObject>
#include <QJsonDocument>

namespace http2 {

enum HeadersFlags {
    HEADERS_FLAG_END_STREAM  = 0x1,
    HEADERS_FLAG_END_HEADERS = 0x4,
    HEADERS_FLAG_PRIORITY    = 0x8
};

HeadersFrame::HeadersFrame(QObject *parent) :
    QObject(parent),
    endStream_(false),
    endHeaders_(true),
    priority_(false)
{
}

HeadersFrame::HeadersFrame(const BasicFrame& frame, StreamContext& context, QObject *parent) :
    QObject(parent)
{
    endStream_  = frame.flags() & HEADERS_FLAG_END_STREAM;
    endHeaders_ = frame.flags() & HEADERS_FLAG_END_HEADERS;
    priority_   = frame.flags() & HEADERS_FLAG_PRIORITY;
    context.hpack.unpack(frame.payload(), &headers_);
}

QByteArray HeadersFrame::dump(StreamContext& context) const
{
    uchar flags = 0;
    if (endStream_)  flags |= HEADERS_FLAG_END_STREAM;
    if (endHeaders_) flags |= HEADERS_FLAG_END_HEADERS;
    if (priority_)   flags |= HEADERS_FLAG_PRIORITY;

    BasicFrame frame;
    frame.setType(FRAME_HEADERS);
    frame.setFlags(flags);
    frame.setPayload(context.hpack.pack(headers_));
    frame.setIdentifier(context.identifier);
    return frame.dump();
}

QString HeadersFrame::inspect() const
{
    QJsonObject headersObj;

    for(const QString& key: headers_.keys()) {
        headersObj[key] = headers_[key];
    }

    QJsonObject obj;
    obj["endStream"]  = endStream();
    obj["endHeaders"] = endHeaders();
    obj["priority"]   = priority();
    obj["headers"]    = headersObj;

    return "HEADERS " + QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Indented));
}

const QMap<QString, QString>& HeadersFrame::headers() const
{
    return headers_;
}

QString& HeadersFrame::operator[](const QString& key)
{
    return headers_[key];
}

QString HeadersFrame::operator[](const QString& key) const
{
    return headers_[key];
}

bool HeadersFrame::endStream() const
{
    return endStream_;
}

void HeadersFrame::setEndStream(bool endStream)
{
    endStream_ = endStream;
}

bool HeadersFrame::endHeaders() const
{
    return endHeaders_;
}

void HeadersFrame::setEndHeaders(bool endHeaders)
{
    endHeaders_ = endHeaders;
}

bool HeadersFrame::priority() const
{
    return priority_;
}

void HeadersFrame::setPriority(bool priority)
{
    priority_ = priority;
}

}
