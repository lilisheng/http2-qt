/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#include "pingframe.h"
#include "streamcontext.h"
#include <QDebug>
#include <QtEndian>
#include <QJsonObject>
#include <QJsonDocument>

namespace http2 {

enum PingFlags {
    SETTINGS_FLAG_ACK = 0x01
};

PingFrame::PingFrame() :
    ack_(false),
    payload_(0)
{
}

PingFrame::PingFrame(const BasicFrame& frame)
{
    if (frame.identifier() != 0) {
        // ERR
    }
    if (frame.length() == sizeof(payload_)) {
        const QByteArray& payload = frame.payload();
        payload_ = qFromBigEndian<quint64>(reinterpret_cast<const uchar*>(payload.data()));
        ack_ = frame.flags() & SETTINGS_FLAG_ACK;
    }
    else {
        // ERR
    }
}

QByteArray PingFrame::dump(StreamContext& context) const
{
    if (context.identifier != 0) {
        qWarning() << "GOAWAY Frame identifier must be 0";
    }

    QByteArray payload(sizeof(payload_), 0);
    qToBigEndian(payload_, reinterpret_cast<uchar*>(payload.data()));

    uchar flags = 0;
    if (ack_) flags |= SETTINGS_FLAG_ACK;

    BasicFrame frame;
    frame.setType(FRAME_PING);
    frame.setPayload(payload);
    frame.setIdentifier(0);
    frame.setFlags(flags);
    return frame.dump();
}

QString PingFrame::inspect() const
{
    QJsonObject obj;
    obj["ack"] = ack_;
    obj["payload"] = QString("%1").arg(payload_);

    return "PING " + QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Indented));
}

bool PingFrame::ack() const
{
    return ack_;
}

void PingFrame::setAck(bool ack)
{
    ack_ = ack;
}

quint64 PingFrame::payload() const
{
    return payload_;
}

void PingFrame::setPayload(quint64 payload)
{
    payload_ = payload;
}



}
