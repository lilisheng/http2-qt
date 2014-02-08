/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#include "goawayframe.h"
#include "streamcontext.h"
#include <QtEndian>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

namespace http2 {

GoawayFrame::GoawayFrame() :
    lastIdentifier_(0),
    errorCode_(0)
{
}

GoawayFrame::GoawayFrame(const BasicFrame& frame)
{
    if (frame.identifier() != 0) {
        // ERR
    }
    if (frame.length() >= sizeof(quint32) * 2) {
        const QByteArray& payload = frame.payload();
        const uchar *payloadPtr = reinterpret_cast<const uchar*>(payload.data());
        lastIdentifier_ = qFromBigEndian<quint32>(payloadPtr);
        payloadPtr += sizeof(quint32);
        errorCode_ = qFromBigEndian<quint32>(payloadPtr);
        debugData_ = payload.mid(sizeof(quint32) * 2);
    }
    else {
        // ERR
    }
}

QByteArray GoawayFrame::dump(StreamContext& context) const
{
    if (context.identifier != 0) {
        qWarning() << "GOAWAY Frame identifier must be 0";
    }

    QByteArray payload(sizeof(quint32) * 2, 0);
    uchar *payloadPtr = reinterpret_cast<uchar*>(payload.data());
    qToBigEndian(lastIdentifier_, payloadPtr);
    payloadPtr += sizeof(quint32);
    qToBigEndian(errorCode_, payloadPtr);

    BasicFrame frame;
    frame.setType(FRAME_GOAWAY);
    frame.setPayload(payload + debugData_);
    frame.setIdentifier(0);
    return frame.dump();
}

QString GoawayFrame::inspect() const
{
    QJsonObject obj;
    obj["lastIdentifier"] = static_cast<int>(lastIdentifier());
    obj["errorCode"]      = static_cast<int>(errorCode());
    obj["debugData"]      = QString::fromUtf8(debugData());

    return "GOAWAY " + QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Indented));
}

quint32 GoawayFrame::lastIdentifier() const
{
    return lastIdentifier_;
}

void GoawayFrame::setLastIdentifier(quint32 lastIdentifier)
{
    lastIdentifier_ = lastIdentifier;
}

quint32 GoawayFrame::errorCode() const
{
    return errorCode_;
}

void GoawayFrame::setErrorCode(quint32 errorCode)
{
    errorCode_ = errorCode;
}

QByteArray GoawayFrame::debugData() const
{
    return debugData_;
}

void GoawayFrame::setDebugData(const QByteArray &debugData)
{
    debugData_ = debugData;
}

}
